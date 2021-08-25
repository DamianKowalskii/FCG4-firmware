#ifndef _ESC_H
#define _ESC_H
#include <stdint.h>
#include <stdbool.h>
typedef void (*ptr_write_to_esc)(uint32_t pulse);
typedef struct esc
{
	uint32_t pulse;
	uint32_t min_pulse;
	uint32_t max_pulse;
	ptr_write_to_esc write_to_esc;
} esc_t;
bool esc_ctor(esc_t *esc, uint32_t min_pulse, uint32_t max_pulse, ptr_write_to_esc write_to_esc);
void esc_write_pulse(esc_t *me, uint32_t pulse);
uint32_t esc_get_pulse(esc_t *me);
void esc_stop(esc_t *me);
#endif // _ESC_H
