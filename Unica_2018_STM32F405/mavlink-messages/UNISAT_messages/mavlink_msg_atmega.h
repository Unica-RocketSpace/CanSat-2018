#pragma once
// MESSAGE ATMEGA PACKING

#define MAVLINK_MSG_ID_ATMEGA 157

MAVPACKED(
typedef struct __mavlink_atmega_t {
 uint32_t package_number; /*< number of transmitted package*/
 float time; /*< current time*/
 float pressure; /*< pressure*/
 float temperature; /*< temperature*/
 float height; /*< height*/
 uint8_t state; /*< state of ATmega*/
}) mavlink_atmega_t;

#define MAVLINK_MSG_ID_ATMEGA_LEN 21
#define MAVLINK_MSG_ID_ATMEGA_MIN_LEN 21
#define MAVLINK_MSG_ID_157_LEN 21
#define MAVLINK_MSG_ID_157_MIN_LEN 21

#define MAVLINK_MSG_ID_ATMEGA_CRC 175
#define MAVLINK_MSG_ID_157_CRC 175



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ATMEGA { \
    157, \
    "ATMEGA", \
    6, \
    {  { "package_number", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_atmega_t, package_number) }, \
         { "time", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_atmega_t, time) }, \
         { "pressure", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_atmega_t, pressure) }, \
         { "temperature", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_atmega_t, temperature) }, \
         { "height", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_atmega_t, height) }, \
         { "state", NULL, MAVLINK_TYPE_UINT8_T, 0, 20, offsetof(mavlink_atmega_t, state) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ATMEGA { \
    "ATMEGA", \
    6, \
    {  { "package_number", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_atmega_t, package_number) }, \
         { "time", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_atmega_t, time) }, \
         { "pressure", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_atmega_t, pressure) }, \
         { "temperature", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_atmega_t, temperature) }, \
         { "height", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_atmega_t, height) }, \
         { "state", NULL, MAVLINK_TYPE_UINT8_T, 0, 20, offsetof(mavlink_atmega_t, state) }, \
         } \
}
#endif

/**
 * @brief Pack a atmega message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param package_number number of transmitted package
 * @param time current time
 * @param pressure pressure
 * @param temperature temperature
 * @param height height
 * @param state state of ATmega
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_atmega_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint32_t package_number, float time, float pressure, float temperature, float height, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ATMEGA_LEN];
    _mav_put_uint32_t(buf, 0, package_number);
    _mav_put_float(buf, 4, time);
    _mav_put_float(buf, 8, pressure);
    _mav_put_float(buf, 12, temperature);
    _mav_put_float(buf, 16, height);
    _mav_put_uint8_t(buf, 20, state);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ATMEGA_LEN);
#else
    mavlink_atmega_t packet;
    packet.package_number = package_number;
    packet.time = time;
    packet.pressure = pressure;
    packet.temperature = temperature;
    packet.height = height;
    packet.state = state;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ATMEGA_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ATMEGA;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
}

/**
 * @brief Pack a atmega message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param package_number number of transmitted package
 * @param time current time
 * @param pressure pressure
 * @param temperature temperature
 * @param height height
 * @param state state of ATmega
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_atmega_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint32_t package_number,float time,float pressure,float temperature,float height,uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ATMEGA_LEN];
    _mav_put_uint32_t(buf, 0, package_number);
    _mav_put_float(buf, 4, time);
    _mav_put_float(buf, 8, pressure);
    _mav_put_float(buf, 12, temperature);
    _mav_put_float(buf, 16, height);
    _mav_put_uint8_t(buf, 20, state);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ATMEGA_LEN);
#else
    mavlink_atmega_t packet;
    packet.package_number = package_number;
    packet.time = time;
    packet.pressure = pressure;
    packet.temperature = temperature;
    packet.height = height;
    packet.state = state;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ATMEGA_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ATMEGA;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
}

/**
 * @brief Encode a atmega struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param atmega C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_atmega_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_atmega_t* atmega)
{
    return mavlink_msg_atmega_pack(system_id, component_id, msg, atmega->package_number, atmega->time, atmega->pressure, atmega->temperature, atmega->height, atmega->state);
}

/**
 * @brief Encode a atmega struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param atmega C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_atmega_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_atmega_t* atmega)
{
    return mavlink_msg_atmega_pack_chan(system_id, component_id, chan, msg, atmega->package_number, atmega->time, atmega->pressure, atmega->temperature, atmega->height, atmega->state);
}

/**
 * @brief Send a atmega message
 * @param chan MAVLink channel to send the message
 *
 * @param package_number number of transmitted package
 * @param time current time
 * @param pressure pressure
 * @param temperature temperature
 * @param height height
 * @param state state of ATmega
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_atmega_send(mavlink_channel_t chan, uint32_t package_number, float time, float pressure, float temperature, float height, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ATMEGA_LEN];
    _mav_put_uint32_t(buf, 0, package_number);
    _mav_put_float(buf, 4, time);
    _mav_put_float(buf, 8, pressure);
    _mav_put_float(buf, 12, temperature);
    _mav_put_float(buf, 16, height);
    _mav_put_uint8_t(buf, 20, state);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ATMEGA, buf, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
#else
    mavlink_atmega_t packet;
    packet.package_number = package_number;
    packet.time = time;
    packet.pressure = pressure;
    packet.temperature = temperature;
    packet.height = height;
    packet.state = state;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ATMEGA, (const char *)&packet, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
#endif
}

/**
 * @brief Send a atmega message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_atmega_send_struct(mavlink_channel_t chan, const mavlink_atmega_t* atmega)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_atmega_send(chan, atmega->package_number, atmega->time, atmega->pressure, atmega->temperature, atmega->height, atmega->state);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ATMEGA, (const char *)atmega, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
#endif
}

#if MAVLINK_MSG_ID_ATMEGA_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_atmega_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t package_number, float time, float pressure, float temperature, float height, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint32_t(buf, 0, package_number);
    _mav_put_float(buf, 4, time);
    _mav_put_float(buf, 8, pressure);
    _mav_put_float(buf, 12, temperature);
    _mav_put_float(buf, 16, height);
    _mav_put_uint8_t(buf, 20, state);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ATMEGA, buf, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
#else
    mavlink_atmega_t *packet = (mavlink_atmega_t *)msgbuf;
    packet->package_number = package_number;
    packet->time = time;
    packet->pressure = pressure;
    packet->temperature = temperature;
    packet->height = height;
    packet->state = state;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ATMEGA, (const char *)packet, MAVLINK_MSG_ID_ATMEGA_MIN_LEN, MAVLINK_MSG_ID_ATMEGA_LEN, MAVLINK_MSG_ID_ATMEGA_CRC);
#endif
}
#endif

#endif

// MESSAGE ATMEGA UNPACKING


/**
 * @brief Get field package_number from atmega message
 *
 * @return number of transmitted package
 */
static inline uint32_t mavlink_msg_atmega_get_package_number(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field time from atmega message
 *
 * @return current time
 */
static inline float mavlink_msg_atmega_get_time(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field pressure from atmega message
 *
 * @return pressure
 */
static inline float mavlink_msg_atmega_get_pressure(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field temperature from atmega message
 *
 * @return temperature
 */
static inline float mavlink_msg_atmega_get_temperature(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field height from atmega message
 *
 * @return height
 */
static inline float mavlink_msg_atmega_get_height(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field state from atmega message
 *
 * @return state of ATmega
 */
static inline uint8_t mavlink_msg_atmega_get_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  20);
}

/**
 * @brief Decode a atmega message into a struct
 *
 * @param msg The message to decode
 * @param atmega C-struct to decode the message contents into
 */
static inline void mavlink_msg_atmega_decode(const mavlink_message_t* msg, mavlink_atmega_t* atmega)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    atmega->package_number = mavlink_msg_atmega_get_package_number(msg);
    atmega->time = mavlink_msg_atmega_get_time(msg);
    atmega->pressure = mavlink_msg_atmega_get_pressure(msg);
    atmega->temperature = mavlink_msg_atmega_get_temperature(msg);
    atmega->height = mavlink_msg_atmega_get_height(msg);
    atmega->state = mavlink_msg_atmega_get_state(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ATMEGA_LEN? msg->len : MAVLINK_MSG_ID_ATMEGA_LEN;
        memset(atmega, 0, MAVLINK_MSG_ID_ATMEGA_LEN);
    memcpy(atmega, _MAV_PAYLOAD(msg), len);
#endif
}
