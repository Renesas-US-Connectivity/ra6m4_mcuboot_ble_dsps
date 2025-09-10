/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <dsps_downloader_thread.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "uart_ble.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "ring_buffer.h"
#include "header.h"

#define PRINTF_SZ (512)


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



static uart_fsm_t s_uart_fsm;
TaskHandle_t s_main_dsps_task;
static SemaphoreHandle_t s_uart_write_mutex;


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

static void rb_image_page_size_available_cb(ring_buffer_t *rb, void *context)
{
    FSP_PARAMETER_NOT_USED (rb);
    FSP_PARAMETER_NOT_USED (context);
    OS_TASK_NOTIFY_FROM_ISR(s_main_dsps_task, NOTIF_FLASH_WRITE_READY ,eSetBits);
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
                OS_TASK_NOTIFY_FROM_ISR(s_main_dsps_task, NOTIF_START_ERASE ,eSetBits);
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
                OS_TASK_NOTIFY_FROM_ISR(s_main_dsps_task, NOTIF_FLASH_WRITE_LAST ,eSetBits);
            }
            break;

        default:
            break;
    }


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

fsp_err_t uart_ble_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    s_main_dsps_task = xTaskGetCurrentTaskHandle();
    s_uart_write_mutex = xSemaphoreCreateMutex();

     /* Initialize UART channel with baud rate 115200 */
     err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
     uart_fsm_init();
     return err;

}

void uart_ble_get_data(uint8_t *data, size_t len)
{
    ring_buffer_get_task(&s_uart_fsm.image_ring_buffer, data, len);
}

void uart_ble_data_consumed(void)
{
    if(s_uart_fsm.hwmk_triggered)
    {
        size_t rb_count = ring_buffer_count(&s_uart_fsm.image_ring_buffer);
        if(rb_count > RING_BUFFER_LW_MARK)
        {
            OS_TASK_NOTIFY(s_main_dsps_task, NOTIF_FLASH_WRITE_READY, eSetBits);
        }else
        {
            uart_flow_set(UART_FLOW_ON);
        }
    }
}

size_t  uart_ble_get_data_available(void)
{
    return ring_buffer_count(&s_uart_fsm.image_ring_buffer);
}

void  uart_ble_flash_ready_for_data(void)
{
    s_uart_fsm.state = UART_STATE_PROGRAMMING_BANK;

    if(s_uart_fsm.hwmk_triggered)
    {
        OS_TASK_NOTIFY(s_main_dsps_task, NOTIF_FLASH_WRITE_READY, eSetBits);
    }
}


void uart_ble_printf(const char *format, ...)
{


        int written;
        char print_buffer[PRINTF_SZ];

        xSemaphoreTake(s_uart_write_mutex, (TickType_t ) 10 );

        va_list arg;
        va_start(arg, format);

        written = vsprintf(print_buffer, format, arg);
        va_end(arg);
        R_SCI_UART_Write (&g_uart0_ctrl, (uint8_t *)print_buffer, (uint32_t)written);


        xSemaphoreGive( s_uart_write_mutex );


}
