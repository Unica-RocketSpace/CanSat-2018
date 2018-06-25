import sys
import logging
import math
import serial


from argparse import ArgumentParser

from pymavlink import mavutil
from pymavlink.dialects.v20.UNISAT import *

from . import _log as _root_log

from . import graphing as gr
from . import gcs_ui as qt

from PyQt5 import QtCore, QtGui, QtWidgets

_log = _root_log.getChild("main")

# f = open("/home/developer/calibration/accel_1.txt", "a")

a_ISC_x = []
a_ISC_y = []
a_ISC_z = []

a_RSC_x = []
a_RSC_y = []
a_RSC_z = []

v_x = []
v_y = []
v_z = []

av_x = []
av_y = []
av_z = []

mov_x = []
mov_y = []
mov_z = []

vmf_x = []
vmf_y = []
vmf_z = []

temp_atmega = []
temp_sensors = []
pressure_atmega = []
pressure_sensors = []

x = []
y = []
z = []

GPS = []

quat = []

state = None
time_atm = None
time_RSC = None
time_ISC = None
time_sens = None


def msg_parser(msg):
    global a_ISC_x, a_ISC_y, a_ISC_z, a_RSC_x, a_RSC_y, a_RSC_z, v_x, v_y, v_z, av_x, av_y, av_z, mov_x, mov_y, mov_z, vmf_x, vmf_y, vmf_z
    global x, y, z, state, temp_atmega, temp_sensors, pressure_atmega, pressure_sensors, quat, time_atm, time_RSC, time_ISC, time_sens, GPS


    if isinstance(msg, MAVLink_bad_data):
        pass
    elif isinstance(msg, MAVLink_atmega_message):
        pressure_atmega.append(msg.pressure)
        temp_atmega.append(msg.temp)
        state = msg.state
        time_atm = msg.time

    elif isinstance(msg, MAVLink_imu_rsc_message):
        av_x.append(msg.gyro[0])
        av_y.append(msg.gyro[1])
        av_z.append(msg.gyro[2])

        a_RSC_x.append(msg.accel[0])
        a_RSC_y.append(msg.accel[1])
        a_RSC_z.append(msg.accel[2])

        time_RSC = msg.time

    elif isinstance(msg, MAVLink_imu_isc_message):
        a_ISC_x.append(msg.accel[0])
        a_ISC_y.append(msg.accel[1])
        a_ISC_z.append(msg.accel[2])

        vmf_x.append(msg.compass[0])
        vmf_y.append(msg.compass[1])
        vmf_z.append(msg.compass[2])

        v_x.append(msg.velocities[0])
        v_y.append(msg.velocities[1])
        v_z.append(msg.velocities[2])

        mov_x.append(msg.coordinates[0])
        mov_y.append(msg.coordinates[1])
        mov_z.append(msg.coordinates[2])

        quat = msg.quaternion

    elif isinstance(msg, MAVLink_sensors_message):
        pressure_sensors.append(msg.pressure)
        temp_sensors.append(msg.temp)
        time_sens.append(msg.time)

    elif isinstance(msg, MAVLink_gps_message):
        x.append(msg.coordinates[0])
        y.append(msg.coordinates[1])
        z.append(msg.coordinates[2])

        GPS.append(msg.coordinates)

    elif isinstance(msg, MAVLink_camera_orientation_message):
        pass
    else:
        return 1


def process_message(msg):
    _log.info("%s", msg)

    if isinstance(msg, MAVLink_bad_data):
        pass
    elif isinstance(msg, MAVLink_atmega_message):
        _log.info(
            "ATmega {n: %ld, time : %0.3f, pressure : %0.3f, temp : %0.1f}"
            %
            (msg.get_header().seq, msg.time, msg.pressure, msg.temp)
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "ATmega {n: %ld, time : %0.3f, pressure : %0.3f, temp : %0.1f}"
            %
            (msg.get_header().seq, msg.time, msg.pressure, msg.temp)
        )

    elif isinstance(msg, MAVLink_imu_rsc_message):
        _log.info(
            "IMU_RSC\t {n: %ld, time: %0.3f, A: [%0.4f, %0.4f, %0.4f] G: [%0.4f, %0.4f, %0.4f] M: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.gyro, *msg.compass)
        )

        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "IMU_RSC\t {n: %ld, time: %0.3f, A: [%0.4f, %0.4f, %0.4f] G: [%0.4f, %0.4f, %0.4f] M: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.gyro, *msg.compass)
        )

        # f.write(str(msg.accel[0]) + "\t" + str(msg.accel[1]) + "\t" + str(msg.accel[2]) + "\n")

    elif isinstance(msg, MAVLink_imu_isc_message):
        _log.info(
            "IMU_ISC\t {n: %ld, time: %0.3f, A: [%0.4f, %0.4f, %0.7f] M: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.compass)
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "IMU_ISC\t {n: %ld, time: %0.3f, A: [%0.4f, %0.4f, %0.7f] M: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.compass)
        )
        _log.info(
            "QUAT\t {n: %ld, time: %0.3f, quat: [%0.4f, %0.4f, %0.4f, %0.4f]}"
            %
            (msg.get_header().seq, msg.time, *msg.quaternion)
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "QUAT\t {n: %ld, time: %0.3f, quat: [%0.4f, %0.4f, %0.4f, %0.4f]}"
            %
            (msg.get_header().seq, msg.time, *msg.quaternion)
        )
        _log.info(
            "POS\t {n: %ld, time: %0.3f, velo: [%0.3f, %0.3f, %0.3f], pos: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.velocities, *msg.coordinates)
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "POS\t {n: %ld, time: %0.3f, velo: [%0.3f, %0.3f, %0.3f], pos: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.velocities, *msg.coordinates)
        )

    elif isinstance(msg, MAVLink_sensors_message):
        _log.info(
            "SENSORS  {n: %ld, time: %0.3f, temp: %0.3f, pressure: %0.3f}"
            %
            (msg.get_header().seq, msg.time, msg.temp, msg.pressure)
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "SENSORS  {n: %ld, time: %0.3f, temp: %0.3f, pressure: %0.3f}"
            %
            (msg.get_header().seq, msg.time, msg.temp, msg.pressure)
        )
    elif isinstance(msg, MAVLink_gps_message):
        _log.info(
            "GPS\t {n: %ld, time: %0.3f, coordinates: [%0.5f, %0.5f, %0.5f]}"
            %
            (msg.get_header().seq, msg.time, msg.coordinates[1], msg.coordinates[0], msg.coordinates[2])
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "GPS\t {n: %ld, time: %0.3f, coordinates: [%0.5f, %0.5f, %0.5f]}"
            %
            (msg.get_header().seq, msg.time, msg.coordinates[1], msg.coordinates[0], msg.coordinates[2])
        )
    elif isinstance(msg, MAVLink_camera_orientation_message):
        _log.info(
            "CAM\t {n: %ld, time: %0.3f, SE: %0.3f, SERVO: %0.3f}"
            %
            (msg.get_header().seq, msg.time, 180*msg.step_engine_pos/math.pi, 180*msg.servo_pos/math.pi)
        )
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(
            "CAM\t {n: %ld, time: %0.3f, SE: %0.3f, SERVO: %0.3f}"
            %
            (msg.get_header().seq, msg.time, 180 * msg.step_engine_pos / math.pi, 180 * msg.servo_pos / math.pi)
        )
    else:
        _log.info(msg)
        Win = qt.Ui_MainWindow()
        Win.textBrowser_2.append(msg)


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
            msg_parser(msg1)
            gr.MyWin.plotting()

        if msg2:
            process_message(msg2)
            msg_parser(msg2)
            gr.MyWin.plotting()

if __name__ == "__main__":
    # exit(main(sys.argv[1:]))
    # f.close()
    app = QtWidgets.QApplication(sys.argv)
    myapp = gr.MyWin()
    myapp.show()
    sys.exit(app.exec_())