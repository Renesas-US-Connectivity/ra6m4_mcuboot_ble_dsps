#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "uart_printf.h"

#include "hal_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "dsps_downloader_thread.h"

#define PRINTF_SZ (512)

SemaphoreHandle_t uart_mutex;



void uart_printf_init(void)
{
    uart_mutex = xSemaphoreCreateMutex();
}

void uart_printf(const char *format, ...)
{


        int written;
        char print_buffer[PRINTF_SZ];

        xSemaphoreTake(uart_mutex, (TickType_t ) 10 );

        va_list arg;
        va_start(arg, format);

        written = vsprintf(print_buffer, format, arg);
        va_end(arg);
        fsp_err_t err;
        err = R_SCI_UART_Write (&g_uart0_ctrl, (uint8_t *)print_buffer, (uint32_t)written);


        xSemaphoreGive( uart_mutex );



}
