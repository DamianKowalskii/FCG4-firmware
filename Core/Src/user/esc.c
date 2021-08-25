#include <stdlib.h>
#include <stdio.h>
#include "esc.h"

bool esc_ctor(esc_t *me, uint32_t min_pulse, uint32_t max_pulse,
		ptr_write_to_esc write_to_esc) {
	//prevent null pointers
	if (me && write_to_esc) {
		me->min_pulse = min_pulse;
		me->max_pulse = max_pulse;
		me->write_to_esc = write_to_esc;
		return true;
	}
	return false;
}
void esc_write_pulse(esc_t *me, uint32_t pulse) {
	if (pulse < me->min_pulse) {
		pulse = me->min_pulse;
	} else if (pulse > me->max_pulse) {
		pulse = me->max_pulse;
	}
	me->pulse = pulse;
	me->write_to_esc(me->pulse);
}
uint32_t esc_get_pulse(esc_t *me) {
	return me->pulse;
}
void esc_stop(esc_t *me) {
	me->pulse = me->min_pulse;
	esc_write_pulse(me, me->pulse);
}
