#include <stdio.h>
#include <stdlib.h>
#include "moving_average.h"
bool moving_average_ctor(moving_average_t *me, uint32_t samples) {
	//prevent null pointers
	if (me) {
		me->samples = samples;
		me->buffer = malloc(sizeof(float) * samples);
		if (me->buffer) {
			return true;
		}
	}
	return false;
}
void moving_average_dtor(moving_average_t *me) {
	free(me->buffer);
}
void moving_average_insert(moving_average_t *me, float value) {
	for (size_t i = 0; i < me->samples; i++) {
		me->buffer[i] = me->buffer[i + 1];
	}
	me->buffer[me->samples - 1] = value;
}
float moving_average_calculate(moving_average_t *me) {
	float sum = 0;
	for (size_t i = 0; i < me->samples; i++) {
		sum += me->buffer[i];
	}
	return sum / me->samples;
}
