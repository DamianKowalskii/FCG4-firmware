#ifndef _CONVERSION_H
#define _CONVERSION_H

#include <stdint.h>

uint32_t convert_integer_to_ascii(uint8_t *buffer, int32_t value);
uint32_t convert_float_to_ascii(uint8_t *buffer, float value, uint32_t accuracy);
uint32_t get_int32_lenght(int32_t value);
uint32_t get_float_lenght(float value, uint32_t accuracy);
#endif // _CONVERSION_H
