#ifndef _PID_CONTROLER_H
#define _PID_CONTROLER_H
#include <stdbool.h>
typedef struct pid_controler
{
    float kp;
    float ki;
    float kd;
    float last_input;
    float error_sum;
    float out_min;
    float out_max;
} pid_controler_t;
bool pid_controler_ctor(pid_controler_t *me,float kp, float ki, float kd);
float pid_controler_get_kp(pid_controler_t *me);
float pid_controler_get_ki(pid_controler_t *me);
float pid_controler_get_kd(pid_controler_t *me);
void pid_controler_set_kp(pid_controler_t *me, float kp);
void pid_controler_set_ki(pid_controler_t *me, float ki);
void pid_controler_set_kd(pid_controler_t *me, float kd);
void pid_controler_set_out_min(pid_controler_t *me, float out_min);
void pid_controler_set_out_max(pid_controler_t *me, float out_max);
float pid_controler_calculate_output(pid_controler_t *me, float input, float setpoint,float d_time);
#endif // _PID_CONTROLER_H
