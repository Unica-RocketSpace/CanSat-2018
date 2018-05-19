import sys
import logging
import math

from argparse import ArgumentParser

from pymavlink import mavutil
from pymavlink.dialects.v20.UNISAT import *

from . import _log as _root_log

_log = _root_log.getChild("main")


def process_message(msg):
        # _log.info("%s", msg)

    if isinstance(msg, MAVLink_bad_data):
        pass
    elif isinstance(msg, MAVLink_atmega_message):
        _log.info(
            "ATmega {cseq: %ld, time : %0.3f, pressure : %0.3f, temp : %0.1f}"
            %
            (msg.get_header().seq, msg.time, msg.pressure, msg.temp)
        )
    elif isinstance(msg, MAVLink_imu_rsc_message):
        _log.info(
            "IMU\t {cseq: %ld, time: %0.3f, accel: [%0.3f, %0.3f, %0.3f] gyro: [%0.3f, %0.3f, %0.3f] compass: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.gyro, *msg.compass)
        )
    elif isinstance(msg, MAVLink_imu_isc_message):
        _log.info(
            "IMU\t {cseq: %ld, time: %0.3f, accel: [%0.3f, %0.3f, %0.3f] gyro: [%0.3f, %0.3f, %0.3f] compass: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.gyro, *msg.compass)
        )
        _log.info(
            "QUAT\t {cseq: %ld, time: %0.3f, quat: [%0.3f, %0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.quaternion)
        )
    elif isinstance(msg, MAVLink_sensors_message):
        _log.info(
            "SENSORS  {cseq: %ld, time: %0.3f, temp: %0.3f, pressure: %0.3f}"
            %
            (msg.get_header().seq, msg.time, msg.temp, msg.pressure)
        )
    elif isinstance(msg, MAVLink_gps_message):
        _log.info(
            "GPS\t {cseq: %ld, time: %0.3f, coordinates: [%0.5f, %0.5f, %0.5f]}"
            %
            (msg.get_header().seq, msg.time, msg.coordinates[1], msg.coordinates[0], msg.coordinates[2])
        )
    elif isinstance(msg, MAVLink_camera_orientation_message):
        _log.info(
            "CAM\t {cseq: %ld, time: %0.3f, SE: %0.3f, SERVO: %0.3f}"
            %
            (msg.get_header().seq, msg.time, 180*msg.step_engine_pos/math.pi, 180*msg.servo_pos/math.pi)
        )
    else:
        _log.info(msg)



def main(argv):
    parser = ArgumentParser(add_help=True)
    parser.add_argument("mavuri", nargs="?", action="store", default="udpin:0.0.0.0:12000")

    logging.basicConfig(
        stream=sys.stdout, level=logging.INFO,
        format="%(asctime)-15s %(message)s"
    )

    args = parser.parse_args(argv)
    mavuri = args.mavuri

    _log.info("Запускаюсь. Использую url: %s" % mavuri)
    connection1 = mavutil.mavlink_connection("udpin:0.0.0.0:12000")
    connection2 = mavutil.mavlink_connection("udpin:0.0.0.0:12001")
    mav1, mav2 = connection1, connection2

    while True:
        msg1 = mav1.recv_match(blocking=False)
        msg2 = mav2.recv_match(blocking=False)

        if msg1:
            process_message(msg1)

        if msg2:
            process_message(msg2)


if __name__ == "__main__":
    exit(main(sys.argv[1:]))
