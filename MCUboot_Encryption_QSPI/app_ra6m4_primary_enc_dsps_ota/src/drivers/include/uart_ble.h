#ifndef __UART_BLE_H__
#define __UART_BLE_H__

fsp_err_t uart_ble_init(void);
void    uart_ble_printf(const char *fmt, ...);
void    uart_ble_get_data(uint8_t *data, size_t len);
void    uart_ble_data_consumed(void);
size_t  uart_ble_get_data_available(void);
void    uart_ble_flash_ready_for_data(void);

#endif //__UART_BLE_H__
