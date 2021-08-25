#ifndef INC_USER_ICM20602_H_
#define INC_USER_ICM20602_H_
#include <stdint.h>
#include <stdbool.h>
typedef enum icm20602_registers {
	GYRO_XOUT = 0x43, GYRO_CONFIG = 0x1B, PWR_MGMT_1 = 0x6B, PWR_MGMT_2 = 0x6C
} icm20602_registers_t;
typedef void (*ptr_icm20602_write_byte)(uint16_t address, uint8_t *buffer);
typedef void (*ptr_icm20602_read_byte)(uint16_t address, uint8_t *buffer);
typedef enum gyro_scale_enum {
	GYRO_RANGE_2000 = 0x03,
	GYRO_RANGE_1000 = 0x02,
	GYRO_RANGE_500 = 0x01,
	GYRO_RANGE_250 = 0x00
} gyro_scale_t;
typedef enum gyro_power_mode_enum {
	GYRO_NORMAL = 0x00, GYRO_SUSPEND = 0x80, GYRO_DEEP_SUSPEND = 0x20,
} gyro_power_mode_t;
typedef struct icm20602 {
	ptr_icm20602_write_byte write_byte;
	ptr_icm20602_read_byte read_byte;
	uint8_t *buffer;
	float lsb;
	float offset_x;
	float offset_y;
	float offset_z;
} icm20602_t;
bool icm20602_ctor(icm20602_t *me, ptr_icm20602_write_byte write_byte,
		ptr_icm20602_read_byte read_byte, gyro_scale_t range);
void icm20602_dtor(icm20602_t *me);
void icm20602_write_byte(icm20602_t *me, uint16_t address, uint8_t *value);
void icm20602_read_byte(icm20602_t *me, uint16_t address, uint8_t *value);
void icm20602_collect_data(icm20602_t *me);
float icm20602_get_x(icm20602_t *me);
float icm20602_get_y(icm20602_t *me);
float icm20602_get_z(icm20602_t *me);
void icm20602_calibrate(icm20602_t *me);
void icm20602_set_scale_range(icm20602_t *me, gyro_scale_t range);
#endif /* INC_USER_ICM20602_H_ */
