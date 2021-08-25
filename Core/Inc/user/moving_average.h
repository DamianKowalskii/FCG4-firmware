#ifndef _MOVING_AVERAGE_H
#define _MOVING_AVERAGE_H
#include <stdint.h>
#include <stdbool.h>
typedef struct moving_average
{
    uint32_t samples;
    float *buffer;
} moving_average_t;
bool moving_average_ctor(moving_average_t *me,uint32_t samples);
void moving_average_dtor(moving_average_t *me);
void moving_average_insert(moving_average_t *me, float value);
float moving_average_calculate(moving_average_t *me);
#endif // _MOVING_AVERAGE_H
