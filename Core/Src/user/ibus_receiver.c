#include "ibus_receiver.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
static const uint8_t HEADER_FIRST_BYTE = 0x20;
static const uint8_t HEADER_SECOND_BYTE = 0x40;
//each channel in ibus protocol takes 2 bytes, the whole packets is: 2 bytes * channels + 2 bytes of header
typedef struct ibus_receiver_struct
{
    uint32_t channels;
    uint32_t size_of_channels_in_bytes;
    uint32_t received_data_counter;
    uint8_t *buffer;
    uint8_t previous_byte;
    bool is_new_frame_avaible;
    bool is_new_start_of_frame;
} ibus_receiver_t;
static bool check_for_header(ibus_receiver me, uint8_t actual_byte);
static void procces_new_value(ibus_receiver me, uint8_t new_value);
static bool  check_for_header(ibus_receiver me, uint8_t actual_byte)
{
    if (actual_byte == HEADER_SECOND_BYTE && me->previous_byte == HEADER_FIRST_BYTE)
    {
        return true;
    }
    return false;
}
static void procces_new_value(ibus_receiver me, uint8_t new_value)
{
    me->buffer[me->received_data_counter] = new_value;
    me->received_data_counter++;
    if (me->received_data_counter >= me->size_of_channels_in_bytes)
    {
        me->is_new_frame_avaible = true;
    }
}
ibus_receiver ibus_receiver_create(uint32_t channels)
{
    ibus_receiver me;
    me = malloc(sizeof(ibus_receiver_t));
    //prevent null pointers
    if (!me)
    {
        return NULL;
    }
    me->buffer = malloc(sizeof(uint8_t) * channels * 2);
    if (!me->buffer)
    {
        return NULL;
    }
    me->channels = channels;
    me->received_data_counter = 0;
    me->size_of_channels_in_bytes = (channels * 2);
    me->is_new_start_of_frame = false;
    me->is_new_frame_avaible = false;
    return me;
}
void ibus_receiver_set_new_received_value(ibus_receiver me, uint8_t value)
{
    if (check_for_header(me, value))
    {
        me->is_new_start_of_frame = true;
        return;
    }
    if (me->is_new_start_of_frame)
    {
        procces_new_value(me, value);
    }
    me->previous_byte = value;
}
uint16_t ibus_receiver_get_channel_value(ibus_receiver me, uint32_t channel)
{
    uint32_t index = (channel - 1);
    uint16_t value = (me->buffer[index + 1] << 8) + me->buffer[index];
    me->is_new_frame_avaible = false;
    return value;
}
bool ibus_receiver_is_new_frame_avaible(ibus_receiver me)
{
    return me->is_new_frame_avaible;
}
