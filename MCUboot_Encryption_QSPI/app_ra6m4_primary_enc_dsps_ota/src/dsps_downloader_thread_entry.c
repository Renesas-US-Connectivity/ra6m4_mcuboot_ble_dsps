/***********************************************************************************************************************
 * File Name    : downloader_thread_entry.c
 * Description  : Contains Downloader Thread Implementation
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <dsps_downloader_thread.h>
#include "ring_buffer.h"
#include "header.h"
#include "uart_printf.h"
/* DSPS Downloader entry function */
/* pvParameters contains TaskHandle_t */
#include "qspi_operations.h"


#define OS_TASK_NOTIFY_FROM_ISR(task, value, action) \
        ({ \
                BaseType_t need_switch, ret; \
                ret = xTaskNotifyFromISR(task, value, action, &need_switch); \
                portEND_SWITCHING_ISR(need_switch); \
                ret; \
        })

#define OS_TASK_NOTIFY(task, value, action) xTaskNotify((task), (value), (action))

#define NOTIF_START_ERASE           (1 << 0)
#define NOTIF_FLASH_WRITE_READY     (1 << 1)
#define NOTIF_FLASH_WRITE_LAST      (1 << 2)


typedef enum {
    UART_STATE_WAIT_FOR_HEADER,
    UART_STATE_WAIT_FOR_LENGTH,
    UART_STATE_ERASING_BANK,
    UART_STATE_PROGRAMMING_BANK
} uart_state_t;

typedef enum
{
    UART_FLOW_STOP,
    UART_FLOW_ON
}uart_flow_setting_t;

typedef struct {
    uart_state_t    state;
    uint32_t        update_header;
    uint32_t        payload_length;
    uint8_t         temp_header_payload[4];
    uint32_t         payload_index;
    bool            hwmk_triggered;
    ring_buffer_t   image_ring_buffer;
} uart_fsm_t;


TaskHandle_t s_dsps_downloader_task;
static uart_fsm_t s_uart_fsm;
static uint8_t s_flash_write_buffer[FLASH_PAGE_PROGRAMMING_SIZE];
static uint32_t s_write_address;


static uint32_t swap_endianness_uint32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |
           ((value >> 8)  & 0x0000FF00) |
           ((value << 8)  & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}

static void uart_flow_set(uart_flow_setting_t flow)
{
    if(flow == UART_FLOW_STOP)
    {

        R_BSP_IrqDisable(g_uart0_cfg.rxi_irq);

    }else if(flow == UART_FLOW_ON)
    {
        R_BSP_IrqEnableNoClear(g_uart0_cfg.rxi_irq);
    }
}

static void fsp_error_check(fsp_err_t err)
{
    if(err != FSP_SUCCESS)
    {
        while(1){};
    }
}

static void rb_image_page_size_available_cb(ring_buffer_t *rb, void *context)
{
    FSP_PARAMETER_NOT_USED (rb);
    FSP_PARAMETER_NOT_USED (context);
    OS_TASK_NOTIFY_FROM_ISR(s_dsps_downloader_task, NOTIF_FLASH_WRITE_READY ,eSetBits);
}

static void rb_hwmk_cb(ring_buffer_t *rb, void *context)
{
    FSP_PARAMETER_NOT_USED (rb);
    FSP_PARAMETER_NOT_USED (context);
    s_uart_fsm.hwmk_triggered = true;
    uart_flow_set(UART_FLOW_STOP);

}


static void uart_fsm_process(uint8_t data)
{

    switch (s_uart_fsm.state){
        case UART_STATE_WAIT_FOR_HEADER:
          if( (s_uart_fsm.payload_index < IMAGE_HEADER_SIZE) && (data == (IMAGE_HEADER & 0xFF)))
          {
              s_uart_fsm.temp_header_payload[s_uart_fsm.payload_index] = data;
              s_uart_fsm.payload_index++;

              if(s_uart_fsm.payload_index >= IMAGE_HEADER_SIZE)
              {
                  uint32_t *rcvd_header = (uint32_t *)s_uart_fsm.temp_header_payload;
                  if(*rcvd_header == IMAGE_HEADER)
                  {
                      s_uart_fsm.state = UART_STATE_WAIT_FOR_LENGTH;
                      s_uart_fsm.payload_index = 0;
                      memset(s_uart_fsm.temp_header_payload, 0, sizeof(s_uart_fsm.temp_header_payload));
                  }
                  else
                  {
                      s_uart_fsm.payload_index = 0;
                  }
              }

          }else{
              s_uart_fsm.payload_index = 0;
          }
          break;
        case UART_STATE_WAIT_FOR_LENGTH:
            if( (s_uart_fsm.payload_index < IMAGE_LENGTH_SIZE)  )
            {
                s_uart_fsm.temp_header_payload[s_uart_fsm.payload_index] = data;
                s_uart_fsm.payload_index++;
            }
            else
            {
                ring_buffer_put_isr(&s_uart_fsm.image_ring_buffer, &data, sizeof(data));
                memcpy(&s_uart_fsm.payload_length, s_uart_fsm.temp_header_payload, sizeof(s_uart_fsm.payload_length));
                s_uart_fsm.payload_length = swap_endianness_uint32(s_uart_fsm.payload_length);
                s_uart_fsm.payload_index = 0;
                s_uart_fsm.state = UART_STATE_ERASING_BANK;
                OS_TASK_NOTIFY_FROM_ISR(s_dsps_downloader_task, NOTIF_START_ERASE ,eSetBits);
            }
            break;
        case UART_STATE_ERASING_BANK:
            s_uart_fsm.payload_index++;
            ring_buffer_put_isr(&s_uart_fsm.image_ring_buffer, &data, sizeof(data));
            ring_buffer_check_callbacks(&s_uart_fsm.image_ring_buffer);
            break;
        case UART_STATE_PROGRAMMING_BANK:
            s_uart_fsm.payload_index++;
            ring_buffer_put_isr(&s_uart_fsm.image_ring_buffer, &data, sizeof(data));
            ring_buffer_check_callbacks(&s_uart_fsm.image_ring_buffer);
            if((s_uart_fsm.payload_index + 1) >= s_uart_fsm.payload_length)
            {
                s_uart_fsm.payload_index = 0;
                s_uart_fsm.state = UART_STATE_WAIT_FOR_HEADER;
                OS_TASK_NOTIFY_FROM_ISR(s_dsps_downloader_task, NOTIF_FLASH_WRITE_LAST ,eSetBits);
            }
            break;

        default:
            break;
    }


}

static void uart_fsm_init(void)
{
    s_uart_fsm.state = UART_STATE_WAIT_FOR_HEADER;
    s_uart_fsm.update_header = 0;
    s_uart_fsm.payload_length = 0;
    s_uart_fsm.payload_index = 0;
    s_uart_fsm.hwmk_triggered = false;
    memset(s_uart_fsm.temp_header_payload, 0, sizeof(s_uart_fsm.temp_header_payload));

    ring_buffer_init(&s_uart_fsm.image_ring_buffer);
    ring_buffer_set_min_bytes_callback(&s_uart_fsm.image_ring_buffer, FLASH_PAGE_PROGRAMMING_SIZE, rb_image_page_size_available_cb, NULL);
    ring_buffer_set_high_watermark_callback(&s_uart_fsm.image_ring_buffer, RING_BUFFER_SIZE - 16, rb_hwmk_cb, NULL); //minus FIFO size
}


void uart_dsps_cb(uart_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_COMPLETE:
            break;
        case UART_EVENT_TX_COMPLETE:
            break;
        case UART_EVENT_RX_CHAR:
            uart_fsm_process((p_args->data)&0xFF);
            break;
        case UART_EVENT_ERR_PARITY:
            break;
        case UART_EVENT_ERR_FRAMING:
            __BKPT(0);
            break;
        case UART_EVENT_ERR_OVERFLOW:
            __BKPT(0);
            break;
        case UART_EVENT_BREAK_DETECT:
            __BKPT(0);
            break;
        case UART_EVENT_TX_DATA_EMPTY:
            break;
        default:
            break;
    }
}

static fsp_err_t uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

     /* Initialize UART channel with baud rate 115200 */
     err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
     uart_fsm_init();
     return err;

}


static void nvm_erase(void)
{
    fsp_err_t err;

    uart_printf("ERASE STARTING!!!!\r\n");

    err = qspi_op_flash_erase_sectors((uint8_t *)SECONDARY_IMAGE_START_ADDRESS, SECONDARY_IMAGE_NUM_SECTORS);
    fsp_error_check(err);

    uart_printf("ERASE FINISHED\r\n");
}

static void nvm_write_page(void)
{

    fsp_err_t write_err;

    ring_buffer_get_task(&s_uart_fsm.image_ring_buffer, s_flash_write_buffer, FLASH_PAGE_PROGRAMMING_SIZE);

#if 0

    write_err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)s_flash_write_buffer, (uint32_t)s_write_address, FLASH_PAGE_PROGRAMMING_SIZE);
#else

    write_err = qspi_op_flash_write_data((uint8_t *)s_write_address, (uint8_t *)s_flash_write_buffer, FLASH_PAGE_PROGRAMMING_SIZE);
#endif
    s_write_address += FLASH_PAGE_PROGRAMMING_SIZE;

    if(s_uart_fsm.hwmk_triggered)
    {
        size_t rb_count = ring_buffer_count(&s_uart_fsm.image_ring_buffer);
        if(rb_count > RING_BUFFER_LW_MARK)
        {
            OS_TASK_NOTIFY(s_dsps_downloader_task, NOTIF_FLASH_WRITE_READY, eSetBits);
        }else
        {
            uart_flow_set(UART_FLOW_ON);
        }
    }

    fsp_error_check(write_err);

    uart_printf("Write err: %d\r\n", write_err);

}

static void nvm_write_last_page(void)
{
    fsp_err_t write_err;
    size_t count = ring_buffer_count(&s_uart_fsm.image_ring_buffer);
    ring_buffer_get_task(&s_uart_fsm.image_ring_buffer, s_flash_write_buffer, count);

#if 0
    fsp_err_t write_err;
    write_err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)s_flash_write_buffer, (uint32_t)s_write_address, count);
#else
    write_err = qspi_op_flash_write_data((uint8_t *)s_write_address, (uint8_t *)s_flash_write_buffer, (uint32_t)count);
#endif
    uart_printf("Total Data written: %d bytes err: %d\r\n", s_uart_fsm.payload_length, write_err);

    fsp_error_check(write_err);
}

static void nvm_init(void)
{

#if 0
    fsp_err_t err =   R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);

    if(err != FSP_SUCCESS)
    {
        while(1){};
    }
#else
    qspi_op_init();
#endif
}


/* Downloader Thread entry function */
/* pvParameters contains TaskHandle_t */
void dsps_downloader_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    uart_init();
    uart_printf_init();
    nvm_init();
    uart_printf("UART DSPS MCU Boot Downloader Start\r\n");

    uint32_t notif;
    s_write_address = SECONDARY_IMAGE_START_ADDRESS;


    s_dsps_downloader_task = xTaskGetCurrentTaskHandle();

    while (1)
    {
        xTaskNotifyWait(0, 0xFFFFFFFF, &notif, portMAX_DELAY);

        if(notif & NOTIF_START_ERASE)
        {
            nvm_erase();
            s_uart_fsm.state = UART_STATE_PROGRAMMING_BANK;

            if(s_uart_fsm.hwmk_triggered)
            {
                OS_TASK_NOTIFY(s_dsps_downloader_task, NOTIF_FLASH_WRITE_READY, eSetBits);
            }
        }

        if(notif & NOTIF_FLASH_WRITE_READY)
        {

            nvm_write_page();

        }
        if(notif & NOTIF_FLASH_WRITE_LAST)
        {

            nvm_write_last_page();
            R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

            NVIC_SystemReset();
        }

    }
}

