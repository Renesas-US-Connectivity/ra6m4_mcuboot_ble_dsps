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
    uint32_t        payload_index;
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

/**
 * @brief UART DSPS callback handler.
 *
 * This function is invoked whenever a UART event occurs for the DSPS (Dialog Serial Port Service).
 * It processes the UART callback arguments provided by the driver and can be extended
 * to handle events such as data received, transmission complete, errors, or state changes.
 *
 * @param[in] p_args Pointer to the UART callback arguments structure.
 *                   Contains information about the event type and context data.
 *
 * @return None.
 *
 * @note This function is typically registered with the UART driver as the callback handler.
 */
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


/**
 * @brief Initialize the UART finite state machine (FSM).
 *
 * Sets the UART FSM to its initial state and prepares any
 * required resources or internal variables for operation.
 * This function should be called once during system startup
 * or before using the UART FSM for the first time.
 *
 * @return None.
 *
 * @note This is a static function and intended for use only
 *       within this source file.
 */

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

/**
 * @brief Initialize the UART interface for BLE communication.
 *
 * Configures the UART peripheral and any associated resources
 * required for Bluetooth Low Energy (BLE) data exchange.
 * This may include setting baud rate, enabling interrupts,
 * and registering callback handlers.
 *
 * @return fsp_err_t FSP_SUCCESS if the initialization completed successfully.
 *                   Otherwise, returns an error code indicating the failure reason.
 *
 * @note This function must be called before starting any BLE-related
 *       UART transactions.
 */
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

/**
 * @brief Retrieve data from the BLE UART interface.
 *
 * Copies received data from the BLE UART buffer into the
 * user-provided buffer. This function is typically used
 * after a notification from the UART driver that there's data for flash write.
 *
 * @param[out] data Pointer to the buffer where received data will be stored.
 * @param[in]  len  Length of the buffer in bytes. Specifies the maximum
 *                  number of bytes to copy.
 *
 * @return None.
 *
 * @note Ensure that @p data has sufficient space to hold @p len bytes.
 *       If fewer than @p len bytes are available, only the available
 *       number of bytes will be copied.
 */
void uart_ble_get_data(uint8_t *data, size_t len)
{
    ring_buffer_get_task(&s_uart_fsm.image_ring_buffer, data, len);
}

/**
 * @brief Notify that BLE UART data has been consumed.
 *
 * Informs the UART BLE driver or buffer manager that the
 * previously retrieved data has been processed by the application.
 * This allows the driver to free or reuse the buffer space
 * for subsequent incoming data.
 *
 * @return None.
 *
 * @note Call this function after handling data obtained from
 *       uart_ble_get_data().
 *
 * @par Usage Flow:
 * @code
 * uint8_t buffer[64];
 * size_t  len = sizeof(buffer);
 *
 * // Retrieve incoming BLE UART data
 * uart_ble_get_data(buffer, len);
 *
 * // Process the data (application-specific logic)
 * process_ble_payload(buffer, len);
 *
 * // Notify driver that data has been consumed
 * uart_ble_data_consumed();
 * @endcode
 */
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

/**
 * @brief Get the number of bytes available in the BLE UART receive buffer.
 *
 * Returns the number of bytes currently stored in the BLE UART buffer
 * that can be retrieved using uart_ble_get_data().
 *
 * @return size_t Number of bytes available to read from the buffer.
 *                Returns 0 if no data is available.
 *
 * @note This function is non-blocking and can be used to poll
 *       for incoming BLE UART data.
 *
 * @par Usage Flow:
 * @code
 * size_t available = uart_ble_get_data_available();
 *
 * if (available > 0)
 * {
 *     uint8_t buffer[64];
 *     size_t len = (available < sizeof(buffer)) ? available : sizeof(buffer);
 *
 *     // Retrieve available data
 *     uart_ble_get_data(buffer, len);
 *
 *     // Process the data
 *     process_ble_payload(buffer, len);
 *
 *     // Notify driver that data has been consumed
 *     uart_ble_data_consumed();
 * }
 * @endcode
 */
size_t  uart_ble_get_data_available(void)
{
    return ring_buffer_count(&s_uart_fsm.image_ring_buffer);
}

/**
 * @brief Indicate that flash memory is ready to accept new BLE UART data.
 *
 * Signals to the BLE UART driver that flash has been prepared (e.g., erased)
 * and the system is ready to receive the next chunk(s) of incoming data.
 * The driver will aggregate incoming bytes and notify the main FreeRTOS task
 * once a full *page* of data is ready to be written.
 *
 * @return None.
 *
 * @note Call this after completing the flash erase for the next page.
 *
 * @par Event-Driven Usage Flow (FreeRTOS):
 * @code
 * // --- One-time setup (not shown): driver is configured to notify the main task
 * // when a page of data is ready, e.g., via task notification, queue, or semaphore.
 *
 * // 1) Prepare flash for the next page
 * flash_erase(target_addr, FLASH_PAGE_SIZE);
 *
 * // 2) Tell the BLE UART driver we are ready to receive data for this page
 * uart_ble_flash_ready_for_data();
 *
 * // 3) Main FreeRTOS task waits for "page ready" notification from the UART BLE driver
 * for (;;)
 * {
 *     uint32_t notif = 0;
 *     xTaskNotifyWait(0, 0xFFFFFFFFu, &notif, portMAX_DELAY); // or queue/semaphore
 *
 *     if (notif & NOTIF_FLASH_WRITE_READY)  // set by the driver when a full page is buffered
 *     {
 *         // 4) Pull exactly one page (or the reported available length)
 *         size_t available = uart_ble_get_data_available(); // expected >= FLASH_PAGE_SIZE
 *         size_t len = (available >= FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : available;
 *
 *         static uint8_t page_buf[FLASH_PAGE_SIZE];
 *         uart_ble_get_data(page_buf, len);
 *
 *         // 5) Program the page to flash
 *         flash_write(target_addr, page_buf, len);
 *
 *         // 6) Mark the just-written bytes as consumed so the driver can reuse buffers
 *         uart_ble_data_consumed();
 *
 *         // 7) Advance target address and prepare the next page (erase, then ready)
 *         target_addr += len;                 // advance to next page (or next region)
 *     }
 * }
 * @endcode
 */
void  uart_ble_flash_ready_for_data(void)
{
    s_uart_fsm.state = UART_STATE_PROGRAMMING_BANK;

    if(s_uart_fsm.hwmk_triggered)
    {
        OS_TASK_NOTIFY(s_main_dsps_task, NOTIF_FLASH_WRITE_READY, eSetBits);
    }
}

/**
 * @brief Print formatted output over the BLE UART interface.
 *
 * Sends formatted text through the BLE UART channel using a
 * printf-style interface. This function accepts a format string
 * and a variable number of arguments, which are formatted into
 * a string and transmitted over the BLE link.
 *
 * @param[in] format C-style format string (printf-compatible).
 *                   Supported specifiers may depend on the underlying
 *                   implementation (e.g., %s, %d, %u, %x).
 * @param[in] ...    Variable arguments corresponding to the format string.
 *
 * @return None.
 *
 * @note This function may block until the formatted string is placed
 *       into the UART transmit buffer. Buffer length limitations may apply.
 *
 * @par Usage Example:
 * @code
 * // Send a static string
 * uart_ble_printf("BLE UART initialized.\r\n");
 *
 * @endcode
 */
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
