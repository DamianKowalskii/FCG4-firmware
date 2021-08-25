#ifndef INC_USER_FLIGHT_CONTROLER_H_
#define INC_USER_FLIGHT_CONTROLER_H_
#include <stdint.h>
#include <stdbool.h>
void error_handler();
uint32_t flight_controler_get_micros();
uint32_t flight_controler_get_milis();
void flight_controler_init();
void flight_controler_run();
#endif /* INC_USER_FLIGHT_CONTROLER_H_ */
