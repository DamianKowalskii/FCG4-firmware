#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "conversion.h"
#include "uart.h"

static char new_line[] = "\r\n";
static void
clear_buffer(uart_t *me);
bool uart_ctor(uart_t *me, uint32_t buffer_size, ptr_send_byte send_byte,
		ptr_get_byte get_byte) {
	//prevent null pointers
	if (me && get_byte && send_byte) {
		me->buffer_image = malloc(sizeof(uint8_t) * buffer_size);
		if (me->buffer_image) {
			me->buffer_size = buffer_size;
			me->send_byte = send_byte;
			me->get_byte = get_byte;
			return true;
		}
	}
	return false;
}
void uart_dtor(uart_t* me){
	free(me->buffer_image);
}
void uart_write_int32(uart_t *me, int32_t value) {
	clear_buffer(me);
	uint32_t string_lenght = convert_integer_to_ascii(me->buffer_image, value);
	for (size_t i = 0; i < string_lenght; i++) {
		me->send_byte(me->buffer_image[i]);
	}
}

void uart_write_float(uart_t *me, float value, uint32_t accuracy) {
	clear_buffer(me);
	uint32_t string_lenght = convert_float_to_ascii(me->buffer_image, value, accuracy);
	for (size_t i = 0; i < string_lenght; i++) {
		me->send_byte(me->buffer_image[i]);
	}
}
void uart_write_char(uart_t *me, char c) {
	clear_buffer(me);
	me->buffer_image[0] = c;
	me->send_byte(me->buffer_image[0]);
}
void uart_write_string(uart_t *me, const char *str) {
	clear_buffer(me);
	uint32_t string_lenght = strlen(str);
	memcpy(me->buffer_image, str, string_lenght);
	for (size_t i = 0; i < string_lenght; i++) {
		me->send_byte(me->buffer_image[i]);
	}
}
void uart_write_new_line(uart_t *me) {
	clear_buffer(me);
	uart_write_string(me, new_line);
}
char uart_read_char(uart_t *me) {
	clear_buffer(me);
	char c = me->get_byte();
	return c;
}
void uart_read_string(uart_t *me, uint8_t *buffer) {
	char c = 1;
	uint32_t str_lenght = 0;
	while (c != '\0') {
		c = me->get_byte();
		me->buffer_image[str_lenght] = c;
		str_lenght++;
	}
	me->buffer_image[str_lenght] = '\0';
	memcpy(buffer, me->buffer_image, str_lenght);
}
void uart_get_buffer(uart_t *me, uint8_t *destination_buffer, uint32_t size) {
	memcpy(destination_buffer, me->buffer_image, size);
}
static void clear_buffer(uart_t *me) {
	memset(me->buffer_image, 0, me->buffer_size);
}
