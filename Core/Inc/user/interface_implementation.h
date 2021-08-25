

#ifndef INC_USER_INTERFACE_IMPLEMENTATION_H_
#define INC_USER_INTERFACE_IMPLEMENTATION_H_
#include <stdbool.h>
#include <stdint.h>
#include "usart.h"
extern void error_handler();
void start_uart();
uint8_t read_uart_byte();
void start_pwm();
void led_set_state(bool state);
void esc1_write_pulse(uint32_t pulse);
void esc2_write_pulse(uint32_t pulse);
void esc3_write_pulse(uint32_t pulse);
void esc4_write_pulse(uint32_t pulse);
void gyroscope_write_byte(uint16_t address, uint8_t *byte);
void gyroscope_read_byte(uint16_t address, uint8_t *byte);
void disable_gyroscope_css_pin();
uint32_t get_milis();
uint32_t get_micros();
void delay_ms(uint32_t time);
#endif /* INC_USER_INTERFACE_IMPLEMENTATION_H_ */
