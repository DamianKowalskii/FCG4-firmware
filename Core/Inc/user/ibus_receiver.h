#ifndef _IBUS_RECEIVER_H
#define _IBUS_RECEIVER_H
#include <stdint.h>
#include <stdbool.h>
typedef struct ibus_receiver_struct *ibus_receiver;
ibus_receiver ibus_receiver_create(uint32_t channels);
void ibus_receiver_set_new_received_value(ibus_receiver me, uint8_t value);
uint16_t ibus_receiver_get_channel_value(ibus_receiver me, uint32_t channel);
bool ibus_receiver_is_new_frame_avaible(ibus_receiver me);
#endif // _IBUS_RECEIVER_H
