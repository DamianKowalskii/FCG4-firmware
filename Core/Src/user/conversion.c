#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "conversion.h"
#define INT32_MAX_CHARS 11
static bool is_value_positive(int32_t value);
static void reverse_array(uint8_t *array, uint32_t size);
static void add_negative_sign(uint8_t *array, uint32_t offset);
static uint32_t get_digits(uint8_t *buffer, int32_t value);
uint32_t convert_integer_to_ascii(uint8_t *buffer, int32_t value) {
	bool add_sign = false;
	if (!is_value_positive(value)) {
		value *= -1;
		add_sign = true;
	}

	uint8_t integer_part_buffer[INT32_MAX_CHARS + 1] = { 0 };
	uint32_t value_lenght = get_digits(integer_part_buffer, value);
	if (add_sign) {
		add_negative_sign(integer_part_buffer, value_lenght);
		value_lenght++;
	}
	reverse_array(integer_part_buffer, value_lenght);
	integer_part_buffer[value_lenght] = '\0';
	memcpy(buffer, integer_part_buffer, value_lenght + 1);
	return value_lenght;
}
uint32_t convert_float_to_ascii(uint8_t *buffer, float value, uint32_t accuracy) {
	if (accuracy == 0) {
		accuracy = 1;
	}
	//get integer part
	int32_t integer_part = (int32_t) value;
	uint8_t integer_part_buffer[INT32_MAX_CHARS + 1];
	uint32_t integer_part_lenght = convert_integer_to_ascii(integer_part_buffer,
			integer_part);
	uint32_t whole_value_buffer_lenght = integer_part_lenght + accuracy + 1;
	uint8_t whole_value_buffer[whole_value_buffer_lenght];
	memcpy(whole_value_buffer, integer_part_buffer, integer_part_lenght);
	//get fractional part
	if (value < 0) {
		value *= -1;
		integer_part *= -1;
	}
	uint8_t fractional_part_buffer[accuracy + 1];
	float fractional_part = value - (float) integer_part;
	fractional_part *= powf(10, accuracy);
	convert_integer_to_ascii(fractional_part_buffer, fractional_part);
	//join into whole value
	char *separator_sign = ".";
	uint8_t *end_of_int = &whole_value_buffer[integer_part_lenght];
	memcpy(end_of_int, separator_sign, 1);
	memcpy(end_of_int + 1, fractional_part_buffer, accuracy);
	memcpy(buffer, whole_value_buffer, accuracy);
	whole_value_buffer[whole_value_buffer_lenght] = '\0';
	memcpy(buffer, whole_value_buffer, whole_value_buffer_lenght);
	return whole_value_buffer_lenght;
}
static bool is_value_positive(int32_t value) {
	if (value < 0) {
		return false;
	}
	return true;
}

static void reverse_array(uint8_t *array, uint32_t size) {
	uint8_t array_clone[size];
	memcpy(array_clone, array, size);
	for (size_t i = 0; i < size; i++) {
		array[i] = array_clone[size - 1 - i];
	}
}
static void add_negative_sign(uint8_t *array, uint32_t offset) {
	array[offset] = '-';
}
static uint32_t get_digits(uint8_t *buffer, int32_t value) {
	uint32_t value_lenght = 0;
	if (value == 0) {
		buffer[0] = '0';
		value_lenght = 1;
	} else {
		uint8_t digit;
		for (size_t i = 0; value > 0; i++) {
			digit = value % 10 + '0';
			buffer[i] = digit;
			value /= 10;
			value_lenght++;
		}
	}
	return value_lenght;
}
uint32_t get_int32_lenght(int32_t value) {
	uint32_t lenght = 0;
	if (value == 0) {
		lenght = 1;
	} else {
		lenght = floor(log10(abs(value))) + 1;
	}
	return lenght;
}
uint32_t get_float_lenght(float value, uint32_t accuracy) {
	int32_t integer_part = (int32_t) value;
	return get_int32_lenght(integer_part) + accuracy;
}
