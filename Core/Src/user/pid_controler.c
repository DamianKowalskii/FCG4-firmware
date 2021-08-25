#include "pid_controler.h"
#include <stdlib.h>

static float calculate_error(float input, float setpoint);
static float calculate_d_input(float input, float last_input, float d_time);
static float calculate_value_for_error_sum(float error, float ki, float d_time);
static float calculate_term(float gain, float value);
static float limit_value(float value, float min, float max);
bool pid_controler_ctor(pid_controler_t *me, float kp, float ki, float kd)
{
	if (me)
	{
		pid_controler_set_kp(me, kp);
		pid_controler_set_ki(me, ki);
		pid_controler_set_kd(me, kd);
		pid_controler_set_out_min(me,0);
		pid_controler_set_out_max(me,0);
		me->error_sum = 0;
		me->last_input = 0;
		return true;
	}
	return false;
}
float pid_controler_get_kp(pid_controler_t *me)
{
	return me->kp;
}
float pid_controler_get_ki(pid_controler_t *me)
{
	return me->ki;
}
float pid_controler_get_kd(pid_controler_t *me)
{
	return me->kd;
}
void pid_controler_set_kp(pid_controler_t *me, float kp)
{
	me->kp = kp;
}
void pid_controler_set_ki(pid_controler_t *me, float ki)
{
	me->ki = ki;
}
void pid_controler_set_kd(pid_controler_t *me, float kd)
{
	me->kd = kd;
}
void pid_controler_set_out_min(pid_controler_t *me, float out_min)
{
	me->out_min = out_min;
}

void pid_controler_set_out_max(pid_controler_t *me, float out_max)
{
	me->out_max = out_max;
}
float pid_controler_calculate_output(pid_controler_t *me, float input,
									 float setpoint, float d_time)
{
	//improvements to basic pid from:http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
	float error = calculate_error(input, setpoint);
	float d_input = calculate_d_input(input, me->last_input, d_time);
	me->last_input = input;
	me->error_sum += calculate_value_for_error_sum(error, me->ki, d_time);
	float proportional_term = calculate_term(me->kp, error);
	float integral_term = limit_value(me->error_sum, me->out_min, me->out_max);
	float derivative_term = calculate_term(me->kd, d_input);
	float output = proportional_term + integral_term + derivative_term;
	output = limit_value(output, me->out_min, me->out_max);
	return output;
}
static float calculate_error(float input, float setpoint)
{
	return setpoint - input;
}
static float calculate_d_input(float input, float last_input, float d_time)
{
	return (input - last_input) / d_time;
}
static float calculate_value_for_error_sum(float error, float ki, float d_time)
{
	return ki * error * d_time;
}
static float calculate_term(float gain, float value)
{
	return gain * value;
}
static float limit_value(float value, float min, float max)
{
	if (value > max)
	{
		return max;
	}
	else if (value < min)
	{
		return min;
	}
	return value;
}
