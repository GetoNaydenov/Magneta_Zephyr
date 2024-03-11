#ifndef uart_h
#define uart_h
#ifdef __cplusplus
extern "C" {
#endif

void uart_init();
int uart_write_data(uint8_t *data);
uint16_t uart_buffer_size();
uint16_t uart_get_read_data_size();
uint8_t * uart_read_data();
void uart_set_disconnect_to_bm83();
bool uart_connected_to_bm83();
void uart_start_timer();
#ifdef __cplusplus
}
#endif

#endif