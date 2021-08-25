#include "icm20602.h"
#include <stdio.h>
#include <stdlib.h>
#include "interface_implementation.h"
static const float RANGE_250_LSB = 131.0f;
static const float RANGE_500_LSB = 65.5f;
static const float RANGE_1000_LSB = 32.9f;
static const float RANGE_2000_LSB = 16.4f;
static const uint8_t GYROSCOSCOPE_OUTPUT_REGISTERS = 6;
static void icm20602_set_bit(icm20602_t *me, uint16_t address, uint8_t offset) {
	uint8_t value = 0;
	me->read_byte(address, &value);
	value |= (1 << offset);
	me->write_byte(address, &value);
}
static void icm20602_clear_bit(icm20602_t *me, uint16_t address, uint8_t offset) {
	uint8_t value = 0;
	me->read_byte(address, &value);
	value &= ~(1 << offset);
	me->write_byte(address, &value);
}
static void icm20602_change_bit_to_state(icm20602_t *me, uint16_t address,
		uint8_t offset, bool state) {
	if (state) {
		icm20602_set_bit(me, address, offset);
	} else {
		icm20602_clear_bit(me, address, offset);
	}
}
static bool get_n_bit(uint32_t value, uint8_t offset) {
	uint32_t mask = 1 << offset;
	uint32_t masked_bit = value & mask;
	bool bit_value = (bool) (masked_bit >> offset);
	return bit_value;
}
bool icm20602_ctor(icm20602_t *me, ptr_icm20602_write_byte write_byte,
		ptr_icm20602_read_byte read_byte, gyro_scale_t range) {
	//prevent null pointers
	if (me && read_byte && write_byte) {
		me->buffer = malloc(sizeof(uint8_t) * GYROSCOSCOPE_OUTPUT_REGISTERS);
		if (me->buffer) {
			me->write_byte = write_byte;
			me->read_byte = read_byte;
		}
		icm20602_change_bit_to_state(me, PWR_MGMT_1, 7, 1);
		icm20602_change_bit_to_state(me, PWR_MGMT_1, 6, 0);
		icm20602_write_byte(me, PWR_MGMT_2, 0);
		icm20602_set_scale_range(me, range);
		return true;
	}
	return false;
}
void icm20602_dtor(icm20602_t *me) {
	free(me->buffer);
}
void icm20602_write_byte(icm20602_t *me, uint16_t address, uint8_t *value) {
	me->write_byte(address, value);
}
void icm20602_read_byte(icm20602_t *me, uint16_t address, uint8_t *value) {
	me->read_byte(address, value);
}
void icm20602_collect_data(icm20602_t *me) {
	for (size_t i = 0; i < GYROSCOSCOPE_OUTPUT_REGISTERS; i++) {
		me->read_byte(GYRO_XOUT + i, me->buffer + i);
	}
}
void icm20602_set_scale_range(icm20602_t *me, gyro_scale_t range) {
	uint8_t bit = get_n_bit(range, 1);
	icm20602_change_bit_to_state(me, GYRO_CONFIG, 4, bit);
	bit = get_n_bit(range, 0);
	icm20602_change_bit_to_state(me, GYRO_CONFIG, 3, bit);
	switch (range) {
	case GYRO_RANGE_250:
		me->lsb = RANGE_250_LSB;
		break;
	case GYRO_RANGE_500:
		me->lsb = RANGE_500_LSB;
		break;
	case GYRO_RANGE_1000:
		me->lsb = RANGE_1000_LSB;
		break;
	case GYRO_RANGE_2000:
		me->lsb = RANGE_2000_LSB;
		break;
	default:
		me->lsb = RANGE_250_LSB;
		break;
	}
}
float icm20602_get_x(icm20602_t *me) {
	int16_t value = (((int16_t) me->buffer[0]) << 8) + me->buffer[1];
	float x = value / me->lsb;
	x -= me->offset_x;
	return x;
}
float icm20602_get_y(icm20602_t *me) {
	int16_t value = (((int16_t) me->buffer[2]) << 8) + me->buffer[3];
	float x = value / me->lsb;
	x -= me->offset_x;
	return x;
}
float icm20602_get_z(icm20602_t *me) {
	int16_t value = (((int16_t) me->buffer[4]) << 8) + me->buffer[5];
	float x = value / me->lsb;
	x -= me->offset_x;
	return x;
}
void icm20602_calibrate(icm20602_t *me) {
	float sum_x = 0;
	float sum_y = 0;
	float sum_z = 0;
	uint32_t samples = 1000;
	for (size_t i = 0; i < samples; i++) {
		icm20602_collect_data(me);
		sum_x += icm20602_get_x(me);
		sum_y += icm20602_get_y(me);
		sum_z += icm20602_get_z(me);
	}
	me->offset_x = sum_x / (float) (samples);
	me->offset_y = sum_y / (float) (samples);
	me->offset_z = sum_z / (float) (samples);
}
