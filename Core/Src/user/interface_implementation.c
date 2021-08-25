#include "interface_implementation.h"
#include "main.h"
#include "tim.h"
#include "spi.h"
#include "usart.h"
#define GYROSCOPE_READ	0x80
#define UART_BUFFER_SIZE 36
uint8_t uart_buffer[UART_BUFFER_SIZE];
uint32_t uart_buffer_index = 0;
void start_uart() {
	uart_buffer_index = 0;
	HAL_UART_Receive(&huart1, uart_buffer,
	UART_BUFFER_SIZE, 1);
}

uint8_t read_uart_byte() {
	uint8_t uart_byte = uart_buffer[uart_buffer_index];
	++uart_buffer_index;
	if (uart_buffer_index > UART_BUFFER_SIZE) {
		uart_buffer_index = 0;
	}
	return uart_byte;
}
void start_pwm() {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}
void led_set_state(bool state) {
	if (state) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
	} else {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
	}
}
uint32_t get_milis() {

	return HAL_GetTick();
}
static inline uint32_t LL_SYSTICK_IsActiveCounterFlag(void) {
	return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
			== (SysTick_CTRL_COUNTFLAG_Msk));
}
uint32_t get_micros() {
	//source: stackoverflow
	/* Ensure COUNTFLAG is reset by reading SysTick control and status register */
	LL_SYSTICK_IsActiveCounterFlag();
	uint32_t m = HAL_GetTick();
	const uint32_t tms = SysTick->LOAD + 1;
	__IO uint32_t u = tms - SysTick->VAL;
	if (LL_SYSTICK_IsActiveCounterFlag()) {
		m = HAL_GetTick();
		u = tms - SysTick->VAL;
	}
	return (m * 1000 + (u * 1000) / tms);
}

void esc1_write_pulse(uint32_t pulse) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}
void esc2_write_pulse(uint32_t pulse) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulse);
}
void esc3_write_pulse(uint32_t pulse) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pulse);
}
void esc4_write_pulse(uint32_t pulse) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pulse);
}
void gyroscope_write_byte(uint16_t address, uint8_t *byte) {
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
	uint8_t data_to_send[] = { address, *byte };
	HAL_StatusTypeDef st = HAL_SPI_Transmit(&hspi1, data_to_send, 2, 1);
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
	if (st != HAL_OK) {
		Error_Handler();
	}

}
void gyroscope_read_byte(uint16_t address, uint8_t *byte) {
	uint8_t address_to_send = address | GYROSCOPE_READ;
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef st = HAL_SPI_TransmitReceive(&hspi1, &address_to_send,
			byte, 1, 1);
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
	if (st != HAL_OK) {
		Error_Handler();
	}

}
void disable_gyroscope_css_pin() {
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}
void delay_ms(uint32_t time) {
	HAL_Delay(time);
}

