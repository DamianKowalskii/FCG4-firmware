#include <stdbool.h>
#include <stdint.h>
#include <stm32g4xx.h>
#include "flight_controler.h"
#include "interface_implementation.h"
#include "flight_controler_config.h"
#include "icm20602.h"
#include "moving_average.h"
#include "uart.h"
#include "esc.h"
#include "pid_controler.h"
#include "ibus_receiver.h"
#include "stdio.h"
typedef enum STATES_ENUM {
	FLYING = 0,
	GET_MPU_DATA = 1,
	CALCULATE_PIDS = 2,
	WRITE_TO_ESC = 3,
	GET_FRAME = 4
} STATES;
typedef enum CHANNELS_ENUM {
	THROTTLE_CHANNEL = 0,
	YAW_CHANNEL = 1,
	PITCH_CHANNEL = 2,
	ROLL_CHANNEL = 3,
	ARM_CHANNEL = 4,
} CHANNELS;
typedef struct receiver_channels {
	uint32_t throttle;
	uint32_t pitch;
	uint32_t roll;
	uint32_t yaw;
	uint32_t arm;
} receiver_channels_t;
typedef struct axes_outputs {
	int32_t pitch;
	int32_t roll;
	int32_t yaw;
} axes_outputs_t;

static int32_t gyro_x;
static int32_t gyro_y;
static int32_t gyro_z;
float dt;
volatile uint32_t fc_state;
static axes_outputs_t ao;
//object interacting with hardware interfaces
static receiver_channels_t rc;
static ibus_receiver radio_receiver;
static esc_t esc1;
static esc_t esc2;
static esc_t esc3;
static esc_t esc4;
static icm20602_t gyroscope;
//
static pid_controler_t pid_controler_pitch_acro;
static pid_controler_t pid_controler_roll_acro;
static pid_controler_t pid_controler_yaw_acro;
static moving_average_t moving_average_esc_throttle;

static int32_t map_int32(int32_t x, int32_t in_min, int32_t in_max,
		int32_t out_min, int32_t out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
static void stop_if_init_error(bool status) {
	if (!status) {
		error_handler();
	}
}
static void flight_controler_set_state(uint32_t state) {
	fc_state = state;
}
static uint32_t flight_controler_get_state() {
	return fc_state;
}
static int32_t limit_value(int32_t value, int32_t min, int32_t max) {
	value = (value < min) ? min : value;
	value = (value > max) ? max : value;
	return value;
}
static bool is_drone_disarmed() {
	//arm channel min value is usually 1000
	if (rc.arm < 1200) {
		return true;
	}
	return false;
}
static bool is_new_flight_loop_time() {

	if (get_micros() >= FLIGHT_LOOP_MICROS) {
		return true;
	}
	return false;

}
static void flight_controler_get_frame() {
	rc.throttle = ibus_receiver_get_channel_value(radio_receiver,
			THROTTLE_CHANNEL);
	rc.pitch = ibus_receiver_get_channel_value(radio_receiver, PITCH_CHANNEL);
	rc.roll = ibus_receiver_get_channel_value(radio_receiver, ROLL_CHANNEL);
	rc.yaw = ibus_receiver_get_channel_value(radio_receiver, YAW_CHANNEL);
	rc.arm = ibus_receiver_get_channel_value(radio_receiver, ARM_CHANNEL);
	//sometimes with wrong configuration in transmitter, value can be out of bounds
	rc.throttle = limit_value(rc.throttle, 1000, 2000);
	rc.pitch = limit_value(rc.pitch, 1000, 2000);
	rc.roll = limit_value(rc.roll, 1000, 2000);
	rc.yaw = limit_value(rc.yaw, 1000, 2000);
	rc.arm = limit_value(rc.arm, 1000, 2000);
	flight_controler_set_state(FLYING);

}
uint32_t flight_controler_get_milis() {
	return get_milis();
}
void flight_controler_init() {

	disable_gyroscope_css_pin();
	stop_if_init_error(
			esc_ctor(&esc1, ESC_MIN_PULSE, ESC_MAX_PULSE, esc1_write_pulse));
	stop_if_init_error(
			esc_ctor(&esc2, ESC_MIN_PULSE, ESC_MAX_PULSE, esc2_write_pulse));
	stop_if_init_error(
			esc_ctor(&esc3, ESC_MIN_PULSE, ESC_MAX_PULSE, esc3_write_pulse));
	stop_if_init_error(
			esc_ctor(&esc4, ESC_MIN_PULSE, ESC_MAX_PULSE, esc4_write_pulse));
	stop_if_init_error(pid_controler_ctor(&pid_controler_pitch_acro, 0, 0, 0));
	stop_if_init_error(pid_controler_ctor(&pid_controler_roll_acro, 0, 0, 0));
	stop_if_init_error(pid_controler_ctor(&pid_controler_yaw_acro, 0, 0, 0));
	radio_receiver = ibus_receiver_create(6);
	stop_if_init_error(radio_receiver);
	stop_if_init_error(moving_average_ctor(&moving_average_esc_throttle, 5));
	stop_if_init_error(
			icm20602_ctor(&gyroscope, gyroscope_write_byte, gyroscope_read_byte,
					GYRO_RANGE_1000));
	start_pwm();
	esc_write_pulse(&esc1, ESC_MIN_PULSE);
	esc_write_pulse(&esc2, ESC_MIN_PULSE);
	esc_write_pulse(&esc3, ESC_MIN_PULSE);
	esc_write_pulse(&esc4, ESC_MIN_PULSE);
	//ESCs need couple of seconds to init, before they can normally operate
	delay_ms(5000);
	flight_controler_set_state(FLYING);
	led_set_state(true);
	delay_ms(2000);
	led_set_state(false);

}
static void flight_controler_flying() {
	/*there is an error when trying to read only 1 byte, so we are reading couple of
	packets and searching for header in these bytes*/
	start_uart();
	for (size_t i = 0; i < 36; i++) {
		ibus_receiver_set_new_received_value(radio_receiver, read_uart_byte());
	}
	if (is_drone_disarmed()) {
		esc_write_pulse(&esc1, ESC_MIN_PULSE);
		esc_write_pulse(&esc2, ESC_MIN_PULSE);
		esc_write_pulse(&esc3, ESC_MIN_PULSE);
		esc_write_pulse(&esc4, ESC_MIN_PULSE);
	}
	if (ibus_receiver_is_new_frame_avaible(radio_receiver)) {
		flight_controler_set_state(GET_FRAME);
		return;
	}
	if (is_new_flight_loop_time()) {
		flight_controler_set_state(GET_MPU_DATA);
		return;
	}

}
static void flight_controler_calculate_pids() {
	/* whole procedure of converting from axes dps to esc outputs is:axes dps --> pid_output dps --> esc pwm signal*/
	int32_t receiver_pitch_dps = map_int32(rc.pitch, 1000, 2000, PITCH_MIN_DPS,
			PITCH_MAX_DPS);
	int32_t receiver_roll_dps = map_int32(rc.roll, 1000, 2000, ROLL_MIN_DPS,
			ROLL_MAX_DPS);
	int32_t receiver_yaw_dps = map_int32(rc.yaw, 1000, 2000, YAW_MIN_DPS,
			YAW_MAX_DPS);

	ao.pitch = pid_controler_calculate_output(&pid_controler_pitch_acro, gyro_y,
			receiver_pitch_dps, dt);
	ao.roll = pid_controler_calculate_output(&pid_controler_roll_acro, gyro_x,
			receiver_roll_dps, dt);
	ao.yaw = pid_controler_calculate_output(&pid_controler_yaw_acro, gyro_z,
			receiver_yaw_dps, dt);
	flight_controler_set_state(WRITE_TO_ESC);
}
static void flight_controler_get_mpu_data() {
	icm20602_collect_data(&gyroscope);
	gyro_x = icm20602_get_x(&gyroscope);
	gyro_y = icm20602_get_y(&gyroscope);
	gyro_z = icm20602_get_z(&gyroscope);
	flight_controler_set_state(CALCULATE_PIDS);

}

static void flight_controler_write_to_esc() {

	uint32_t esc_throttle_signal = map_int32(rc.throttle, 1000, 2000,
			ESC_MIN_PULSE, ESC_MAX_PULSE);
	//sometines i get jitters in throttle channels
	moving_average_insert(&moving_average_esc_throttle, esc_throttle_signal);
	esc_throttle_signal = moving_average_calculate(
			&moving_average_esc_throttle);
	//TODO: mixing signals for quadcopter in x configuration
	uint32_t esc1_value = esc_throttle_signal;
	uint32_t esc2_value = esc_throttle_signal;
	uint32_t esc3_value = esc_throttle_signal;
	uint32_t esc4_value = esc_throttle_signal;
	esc_write_pulse(&esc1, esc1_value);
	esc_write_pulse(&esc2, esc2_value);
	esc_write_pulse(&esc3, esc3_value);
	esc_write_pulse(&esc4, esc4_value);
	flight_controler_set_state(FLYING);
}
void flight_controler_run() {
	switch (flight_controler_get_state()) {
	case GET_FRAME:
		flight_controler_get_frame();
		break;
	case FLYING:
		flight_controler_flying();
		break;
	case GET_MPU_DATA:
		flight_controler_get_mpu_data();
		break;
	case CALCULATE_PIDS:
		flight_controler_calculate_pids();
		break;
	case WRITE_TO_ESC:
		flight_controler_write_to_esc();
		break;
	}
}
void error_handler() {
	led_set_state(true);
	//even if drone is in the air  and  it will crash, we shoudl stop the motors!
	esc_write_pulse(&esc1, ESC_MIN_PULSE);
	esc_write_pulse(&esc2, ESC_MIN_PULSE);
	esc_write_pulse(&esc3, ESC_MIN_PULSE);
	esc_write_pulse(&esc4, ESC_MIN_PULSE);
	while (true) {
		;
	}
}
