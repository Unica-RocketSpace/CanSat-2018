#pragma once
// MESSAGE STATE PACKING

#define MAVLINK_MSG_ID_STATE 150

MAVPACKED(
typedef struct __mavlink_state_t {
 float time; /*< current time*/
 uint8_t accel_state; /*< state of accelerometer*/
 uint8_t gyro_state; /*< state of gyroscope*/
 uint8_t compass_state; /*< state of magnetometer*/
 uint8_t baro_state; /*< state of barometer*/
 uint8_t GPS_state; /*< state of GPS-module*/
 uint8_t RPI_state; /*< state of RaspRi*/
 uint8_t globalStage; /*< current global stage number*/
}) mavlink_state_t;

#define MAVLINK_MSG_ID_STATE_LEN 11
#define MAVLINK_MSG_ID_STATE_MIN_LEN 11
#define MAVLINK_MSG_ID_150_LEN 11
#define MAVLINK_MSG_ID_150_MIN_LEN 11

#define MAVLINK_MSG_ID_STATE_CRC 83
#define MAVLINK_MSG_ID_150_CRC 83



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_STATE { \
    150, \
    "STATE", \
    8, \
    {  { "accel_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_state_t, accel_state) }, \
         { "gyro_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 5, offsetof(mavlink_state_t, gyro_state) }, \
         { "compass_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 6, offsetof(mavlink_state_t, compass_state) }, \
         { "baro_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 7, offsetof(mavlink_state_t, baro_state) }, \
         { "GPS_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_state_t, GPS_state) }, \
         { "RPI_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 9, offsetof(mavlink_state_t, RPI_state) }, \
         { "globalStage", NULL, MAVLINK_TYPE_UINT8_T, 0, 10, offsetof(mavlink_state_t, globalStage) }, \
         { "time", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_state_t, time) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_STATE { \
    "STATE", \
    8, \
    {  { "accel_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_state_t, accel_state) }, \
         { "gyro_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 5, offsetof(mavlink_state_t, gyro_state) }, \
         { "compass_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 6, offsetof(mavlink_state_t, compass_state) }, \
         { "baro_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 7, offsetof(mavlink_state_t, baro_state) }, \
         { "GPS_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_state_t, GPS_state) }, \
         { "RPI_state", NULL, MAVLINK_TYPE_UINT8_T, 0, 9, offsetof(mavlink_state_t, RPI_state) }, \
         { "globalStage", NULL, MAVLINK_TYPE_UINT8_T, 0, 10, offsetof(mavlink_state_t, globalStage) }, \
         { "time", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_state_t, time) }, \
         } \
}
#endif

/**
 * @brief Pack a state message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param accel_state state of accelerometer
 * @param gyro_state state of gyroscope
 * @param compass_state state of magnetometer
 * @param baro_state state of barometer
 * @param GPS_state state of GPS-module
 * @param RPI_state state of RaspRi
 * @param globalStage current global stage number
 * @param time current time
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_state_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t accel_state, uint8_t gyro_state, uint8_t compass_state, uint8_t baro_state, uint8_t GPS_state, uint8_t RPI_state, uint8_t globalStage, float time)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_STATE_LEN];
    _mav_put_float(buf, 0, time);
    _mav_put_uint8_t(buf, 4, accel_state);
    _mav_put_uint8_t(buf, 5, gyro_state);
    _mav_put_uint8_t(buf, 6, compass_state);
    _mav_put_uint8_t(buf, 7, baro_state);
    _mav_put_uint8_t(buf, 8, GPS_state);
    _mav_put_uint8_t(buf, 9, RPI_state);
    _mav_put_uint8_t(buf, 10, globalStage);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_STATE_LEN);
#else
    mavlink_state_t packet;
    packet.time = time;
    packet.accel_state = accel_state;
    packet.gyro_state = gyro_state;
    packet.compass_state = compass_state;
    packet.baro_state = baro_state;
    packet.GPS_state = GPS_state;
    packet.RPI_state = RPI_state;
    packet.globalStage = globalStage;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_STATE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_STATE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
}

/**
 * @brief Pack a state message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param accel_state state of accelerometer
 * @param gyro_state state of gyroscope
 * @param compass_state state of magnetometer
 * @param baro_state state of barometer
 * @param GPS_state state of GPS-module
 * @param RPI_state state of RaspRi
 * @param globalStage current global stage number
 * @param time current time
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_state_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t accel_state,uint8_t gyro_state,uint8_t compass_state,uint8_t baro_state,uint8_t GPS_state,uint8_t RPI_state,uint8_t globalStage,float time)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_STATE_LEN];
    _mav_put_float(buf, 0, time);
    _mav_put_uint8_t(buf, 4, accel_state);
    _mav_put_uint8_t(buf, 5, gyro_state);
    _mav_put_uint8_t(buf, 6, compass_state);
    _mav_put_uint8_t(buf, 7, baro_state);
    _mav_put_uint8_t(buf, 8, GPS_state);
    _mav_put_uint8_t(buf, 9, RPI_state);
    _mav_put_uint8_t(buf, 10, globalStage);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_STATE_LEN);
#else
    mavlink_state_t packet;
    packet.time = time;
    packet.accel_state = accel_state;
    packet.gyro_state = gyro_state;
    packet.compass_state = compass_state;
    packet.baro_state = baro_state;
    packet.GPS_state = GPS_state;
    packet.RPI_state = RPI_state;
    packet.globalStage = globalStage;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_STATE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_STATE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
}

/**
 * @brief Encode a state struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_state_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_state_t* state)
{
    return mavlink_msg_state_pack(system_id, component_id, msg, state->accel_state, state->gyro_state, state->compass_state, state->baro_state, state->GPS_state, state->RPI_state, state->globalStage, state->time);
}

/**
 * @brief Encode a state struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_state_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_state_t* state)
{
    return mavlink_msg_state_pack_chan(system_id, component_id, chan, msg, state->accel_state, state->gyro_state, state->compass_state, state->baro_state, state->GPS_state, state->RPI_state, state->globalStage, state->time);
}

/**
 * @brief Send a state message
 * @param chan MAVLink channel to send the message
 *
 * @param accel_state state of accelerometer
 * @param gyro_state state of gyroscope
 * @param compass_state state of magnetometer
 * @param baro_state state of barometer
 * @param GPS_state state of GPS-module
 * @param RPI_state state of RaspRi
 * @param globalStage current global stage number
 * @param time current time
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_state_send(mavlink_channel_t chan, uint8_t accel_state, uint8_t gyro_state, uint8_t compass_state, uint8_t baro_state, uint8_t GPS_state, uint8_t RPI_state, uint8_t globalStage, float time)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_STATE_LEN];
    _mav_put_float(buf, 0, time);
    _mav_put_uint8_t(buf, 4, accel_state);
    _mav_put_uint8_t(buf, 5, gyro_state);
    _mav_put_uint8_t(buf, 6, compass_state);
    _mav_put_uint8_t(buf, 7, baro_state);
    _mav_put_uint8_t(buf, 8, GPS_state);
    _mav_put_uint8_t(buf, 9, RPI_state);
    _mav_put_uint8_t(buf, 10, globalStage);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_STATE, buf, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
#else
    mavlink_state_t packet;
    packet.time = time;
    packet.accel_state = accel_state;
    packet.gyro_state = gyro_state;
    packet.compass_state = compass_state;
    packet.baro_state = baro_state;
    packet.GPS_state = GPS_state;
    packet.RPI_state = RPI_state;
    packet.globalStage = globalStage;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_STATE, (const char *)&packet, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
#endif
}

/**
 * @brief Send a state message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_state_send_struct(mavlink_channel_t chan, const mavlink_state_t* state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_state_send(chan, state->accel_state, state->gyro_state, state->compass_state, state->baro_state, state->GPS_state, state->RPI_state, state->globalStage, state->time);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_STATE, (const char *)state, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
#endif
}

#if MAVLINK_MSG_ID_STATE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_state_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t accel_state, uint8_t gyro_state, uint8_t compass_state, uint8_t baro_state, uint8_t GPS_state, uint8_t RPI_state, uint8_t globalStage, float time)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_float(buf, 0, time);
    _mav_put_uint8_t(buf, 4, accel_state);
    _mav_put_uint8_t(buf, 5, gyro_state);
    _mav_put_uint8_t(buf, 6, compass_state);
    _mav_put_uint8_t(buf, 7, baro_state);
    _mav_put_uint8_t(buf, 8, GPS_state);
    _mav_put_uint8_t(buf, 9, RPI_state);
    _mav_put_uint8_t(buf, 10, globalStage);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_STATE, buf, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
#else
    mavlink_state_t *packet = (mavlink_state_t *)msgbuf;
    packet->time = time;
    packet->accel_state = accel_state;
    packet->gyro_state = gyro_state;
    packet->compass_state = compass_state;
    packet->baro_state = baro_state;
    packet->GPS_state = GPS_state;
    packet->RPI_state = RPI_state;
    packet->globalStage = globalStage;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_STATE, (const char *)packet, MAVLINK_MSG_ID_STATE_MIN_LEN, MAVLINK_MSG_ID_STATE_LEN, MAVLINK_MSG_ID_STATE_CRC);
#endif
}
#endif

#endif

// MESSAGE STATE UNPACKING


/**
 * @brief Get field accel_state from state message
 *
 * @return state of accelerometer
 */
static inline uint8_t mavlink_msg_state_get_accel_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  4);
}

/**
 * @brief Get field gyro_state from state message
 *
 * @return state of gyroscope
 */
static inline uint8_t mavlink_msg_state_get_gyro_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  5);
}

/**
 * @brief Get field compass_state from state message
 *
 * @return state of magnetometer
 */
static inline uint8_t mavlink_msg_state_get_compass_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  6);
}

/**
 * @brief Get field baro_state from state message
 *
 * @return state of barometer
 */
static inline uint8_t mavlink_msg_state_get_baro_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  7);
}

/**
 * @brief Get field GPS_state from state message
 *
 * @return state of GPS-module
 */
static inline uint8_t mavlink_msg_state_get_GPS_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  8);
}

/**
 * @brief Get field RPI_state from state message
 *
 * @return state of RaspRi
 */
static inline uint8_t mavlink_msg_state_get_RPI_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  9);
}

/**
 * @brief Get field globalStage from state message
 *
 * @return current global stage number
 */
static inline uint8_t mavlink_msg_state_get_globalStage(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  10);
}

/**
 * @brief Get field time from state message
 *
 * @return current time
 */
static inline float mavlink_msg_state_get_time(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Decode a state message into a struct
 *
 * @param msg The message to decode
 * @param state C-struct to decode the message contents into
 */
static inline void mavlink_msg_state_decode(const mavlink_message_t* msg, mavlink_state_t* state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    state->time = mavlink_msg_state_get_time(msg);
    state->accel_state = mavlink_msg_state_get_accel_state(msg);
    state->gyro_state = mavlink_msg_state_get_gyro_state(msg);
    state->compass_state = mavlink_msg_state_get_compass_state(msg);
    state->baro_state = mavlink_msg_state_get_baro_state(msg);
    state->GPS_state = mavlink_msg_state_get_GPS_state(msg);
    state->RPI_state = mavlink_msg_state_get_RPI_state(msg);
    state->globalStage = mavlink_msg_state_get_globalStage(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_STATE_LEN? msg->len : MAVLINK_MSG_ID_STATE_LEN;
        memset(state, 0, MAVLINK_MSG_ID_STATE_LEN);
    memcpy(state, _MAV_PAYLOAD(msg), len);
#endif
}
