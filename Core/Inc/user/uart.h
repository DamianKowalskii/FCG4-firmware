#ifndef _UART_H
#define _UART_H
#include <stdint.h>
#include <stdbool.h>
typedef void (*ptr_send_byte)(uint8_t byte);
typedef uint8_t (*ptr_get_byte)();
typedef struct uart {
	//Each char is first written to the buffer and then sent to hardware uart
	uint8_t *buffer_image;
	uint32_t buffer_size;
	ptr_send_byte send_byte;
	ptr_get_byte get_byte;
} uart_t;
bool uart_ctor(uart_t *me, uint32_t buffer_size, ptr_send_byte send_byte,
		ptr_get_byte get_byte);
void uart_dtor(uart_t *me);
void uart_write_int32(uart_t *me, int32_t value);
void uart_write_float(uart_t *me, float value, uint32_t accuracy);
void uart_write_char(uart_t *me, char c);
void uart_write_string(uart_t *me, const char *str);
void uart_write_new_line(uart_t *me);
char uart_read_char(uart_t *me);
void uart_read_string(uart_t *me, uint8_t *buffer);
void uart_get_buffer(uart_t *me, uint8_t *destination_buffer, uint32_t size);
#endif // _UART_H
