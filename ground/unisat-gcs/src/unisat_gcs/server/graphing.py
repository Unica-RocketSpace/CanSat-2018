import sys
import time

import numpy as np
import pyqtgraph as pg
from .gcs_ui import *

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *

from pymavlink.dialects.v20.UNISAT import *
from pymavlink import mavutil

import pyqtgraph.opengl as gl

from PyQt5.QtCore import QThread
import logging
import math

from . import _log as _root_log


_log = _root_log.getChild("main")

class Mavlink_thread(QThread):
    # new_record = QtCore.pyqtSignal(list)

    def __init__(self):
        QThread.__init__(self)

    def MAV_INIT(self):
        logging.basicConfig(
            stream=sys.stdout, level=logging.INFO,
            format="%(asctime)-15s %(message)s"
        )

        _log.info("Запускаюсь. Использую url:")
        print('init')
        connect1 = mavutil.mavlink_connection("udpin:172.16.164.222:12000")
        # connect2 = mavutil.mavlink_connection("udpin:0.0.0.0:12001")
        return connect1

    def msg_parser(self, msg):
        if isinstance(msg, MAVLink_bad_data):
            print('bad data')
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

            print('v_x', v_x)

            mov_x.append(msg.coordinates[0])
            mov_y.append(msg.coordinates[1])
            mov_z.append(msg.coordinates[2])

            quat = msg.quaternion

            time_ISC = msg.time

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
            print(msg)


    def main(self, mav1):
        while True:

            print("main")
            msg1 = mav1.recv_match(blocking=True)
            # msg2 = mav2.recv_match(blocking=False)
            print(msg1)
            # if msg1 == None:
            #     self.MAV_INIT()

            if msg1:
                print('msg1')
                self.msg_parser(msg1)
                # self.new_record.emit(msg1)

            # if msg2:
            #     print('msg2')
            #     self.msg_parser(msg2)
                # self.new_record.emit()

    def run(self):
        mav1 = self.MAV_INIT()
        self.main(mav1)




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

    elif isinstance(msg, MAVLink_imu_rsc_message):
        _log.info(
            "IMU_RSC\t {n: %ld, time: %0.3f, A: [%0.4f, %0.4f, %0.4f] G: [%0.4f, %0.4f, %0.4f] M: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.gyro, *msg.compass)
        )

    elif isinstance(msg, MAVLink_imu_isc_message):
        _log.info(
            "IMU_ISC\t {n: %ld, time: %0.3f, A: [%0.4f, %0.4f, %0.7f] M: [%0.3f, %0.3f, %0.3f]}"
            %
            (msg.get_header().seq, msg.time, *msg.accel, *msg.compass)
        )
        _log.info(
            "QUAT\t {n: %ld, time: %0.3f, quat: [%0.4f, %0.4f, %0.4f, %0.4f]}"
            %
            (msg.get_header().seq, msg.time, *msg.quaternion)
        )
        _log.info(
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
    elif isinstance(msg, MAVLink_gps_message):
        _log.info(
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
    else:
        _log.info(msg)





log_text = ''
global_vars={'x': 0, 'y': 0}
now_graf = None
str_now_graf = None
lenght = 20

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

time_atm = []
time_RSC = []
time_ISC = []
time_sens = []

state = None
class PlotThread(QThread):
    def __init__(self):
        QThread.__init__(self)

    def __del__(self):
        self.wait()

    def run(self):
        myapp = MyWin()
        while True:
            # MyWin.msg_parser(myapp, msg)
            MyWin.update_graf(myapp)



# Главный класс
class MyWin(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        QtWidgets.QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        # self.ui = Ground.Ui_MainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle('Unica gcs')

        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')

        self.ui.glv_top = pg.GraphicsLayoutWidget(self.ui.centralwidget)
        self.ui.top.addWidget(self.ui.glv_top)
        self.ui.plot_top = pg.GraphicsLayout()
        self.ui.glv_top.addItem(self.ui.plot_top)
        # Создаем title для графика
        # title="title"
        # График для нескольких прямых
        # self.sc_item = self.ui.plot.addPlot(title="title")
        #######

        self.sc_item_top1 = pg.PlotItem(title='Accel ISC', labels={'left': 'accel', 'bottom': 'time'})
        self.ui.plot_top.addItem(self.sc_item_top1)
        self.graf_top1 = pg.PlotCurveItem()
        self.sc_item_top1.addItem(self.graf_top1)

        self.sc_item_top2 = pg.PlotItem(title='Accel RSC')
        self.ui.plot_top.addItem(self.sc_item_top2)
        self.graf_top2 = pg.PlotCurveItem()
        self.sc_item_top2.addItem(self.graf_top2)

        self.sc_item_top3 = pg.PlotItem(title='Velosity')
        self.ui.plot_top.addItem(self.sc_item_top3)
        self.graf_top3 = pg.PlotCurveItem()
        self.sc_item_top3.addItem(self.graf_top3)


        self.ui.glv_middle = pg.GraphicsLayoutWidget(self.ui.centralwidget)
        self.ui.middle.addWidget(self.ui.glv_middle)
        self.ui.plot_middle = pg.GraphicsLayout()
        self.ui.glv_middle.addItem(self.ui.plot_middle)

        self.sc_item_middle1 = pg.PlotItem(title='Angles velosity')
        self.ui.plot_middle.addItem(self.sc_item_middle1)
        self.graf_middle1 = pg.PlotCurveItem()
        self.sc_item_middle1.addItem(self.graf_middle1)

        self.sc_item_middle2 = pg.PlotItem(title='Movement')
        self.ui.plot_middle.addItem(self.sc_item_middle2)
        self.graf_middle2 = pg.PlotCurveItem()
        self.sc_item_middle2.addItem(self.graf_middle2)

        self.sc_item_middle3 = pg.PlotItem(title='Vector of magnetic field')
        self.ui.plot_middle.addItem(self.sc_item_middle3)
        self.graf_middle3 = pg.PlotCurveItem()
        self.sc_item_middle3.addItem(self.graf_middle3)


        self.ui.glv_down = pg.GraphicsLayoutWidget(self.ui.centralwidget)
        self.ui.down.addWidget(self.ui.glv_down)
        self.ui.plot_down = pg.GraphicsLayout()
        self.ui.glv_down.addItem(self.ui.plot_down)

        self.sc_item_down1 = pg.PlotItem(title='Temperature')
        self.ui.plot_down.addItem(self.sc_item_down1)
        self.graf_down1 = pg.PlotCurveItem()
        self.sc_item_down1.addItem(self.graf_down1)

        self.sc_item_down2 = pg.PlotItem(title='Pressure')
        self.ui.plot_down.addItem(self.sc_item_down2)
        self.graf_down2 = pg.PlotCurveItem()
        self.sc_item_down2.addItem(self.graf_down2)

        self.sc_item_down3 = pg.PlotItem(title='GPS')
        self.ui.plot_down.addItem(self.sc_item_down3)
        self.graf_down3 = pg.PlotCurveItem()
        self.sc_item_down3.addItem(self.graf_down3)


        self.ui.glv_large = pg.GraphicsLayoutWidget(self.ui.centralwidget)
        self.ui.grid_large_graf.addWidget(self.ui.glv_large)
        self.ui.plot_large = pg.GraphicsLayout()
        self.ui.glv_large.addItem(self.ui.plot_large)
        self.sc_item_large = pg.PlotItem()
        self.ui.plot_large.addItem(self.sc_item_large)
        # self.graf_large = pg.PlotCurveItem()
        # self.sc_item_large.addItem(self.graf_large)

        self.ui.dockwid = QtWidgets.QDockWidget()
        self.ui.grid_3D.addWidget(self.ui.dockwid)

        self.ui.glwid = gl.GLViewWidget()
        self.ui.dockwid.setWidget(self.ui.glwid)
        self.gx = gl.GLGridItem()
        self.gx.rotate(90, 0, 1, 0)
        self.gx.translate(-10, 0, 0)
        self.ui.glwid.addItem(self.gx)
        self.gy = gl.GLGridItem()
        self.gy.rotate(90, 1, 0, 0)
        self.gy.translate(0, -10, 0)
        self.ui.glwid.addItem(self.gy)
        self.gz = gl.GLGridItem()
        self.gz.translate(0, 0, -10)
        self.ui.glwid.addItem(self.gz)
        self.plt = gl.GLLinePlotItem()
        self.ui.glwid.addItem(self.plt)
        self.or_mod = gl.GLAxisItem()
        self.ui.glwid.addItem(self.or_mod)
        self.ui.glwid.show()

        self.pl_graf_top1 = self.sc_item_top1.plot()
        self.pl_graf_top2 = self.sc_item_top2.plot()
        self.pl_graf_top3 = self.sc_item_top3.plot()

        self.pl_graf_middle1 = self.sc_item_middle1.plot()
        self.pl_graf_middle2 = self.sc_item_middle2.plot()
        self.pl_graf_middle3 = self.sc_item_middle3.plot()

        self.pl_graf_down1 = self.sc_item_down1.plot()
        self.pl_graf_down2 = self.sc_item_down2.plot()
        self.pl_graf_down3 = self.sc_item_down3.plot()

        # self.drawing_plot()
        # self.msg_parser()



        # Здесь прописываем событие нажатия на кнопку
        self.ui.pushButton_3.clicked.connect(self.Remove_graf)
        self.ui.commandLinkButton.clicked.connect(self.send_command)
        self.ui.pushButton_13.clicked.connect(self.transfer_graf_top1)
        self.ui.pushButton_14.clicked.connect(self.transfer_graf_top2)
        self.ui.pushButton_12.clicked.connect(self.transfer_graf_top3)
        self.ui.pushButton_6.clicked.connect(self.transfer_graf_middle1)
        self.ui.pushButton_7.clicked.connect(self.transfer_graf_middle2)
        self.ui.pushButton_5.clicked.connect(self.transfer_graf_middle3)
        self.ui.pushButton_8.clicked.connect(self.transfer_graf_down1)
        self.ui.pushButton_9.clicked.connect(self.transfer_graf_down2)
        self.ui.pushButton_10.clicked.connect(self.transfer_graf_down3)


    def clear_log(self):
        global log_text
        log_text = ''
        self.ui.textBrowser_2.setText(log_text)


    def log_add(self, log_msg):
        global log_text
        log_text = log_text + str(log_msg) + "\n"
        self.ui.textBrowser_2.setText(log_text)


    def send_command(self):
        com = self.ui.textBrowser.toPlainText()
        self.ui.textBrowser.clear()
        print(com)


    def Remove_graf(self):
        global now_graf, str_now_graf

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()
    # FIXME: возможно не работает

    def check_now_graf(self):
        global now_graf, str_now_graf

        if now_graf == None:
            return 0, 0

        if str_now_graf == 'graf_top1':
            graf_1 = self.sc_item_top1
            graf_2 = self.pl_graf_top1
            return graf_1, graf_2

        if str_now_graf == 'graf_top2':
            graf_1 = self.sc_item_top2
            graf_2 = self.pl_graf_top2
            return graf_1, graf_2

        if str_now_graf == 'graf_top3':
            graf_1 = self.sc_item_top3
            graf_2 = self.pl_graf_top3
            return graf_1, graf_2

        if str_now_graf == 'graf_middle1':
            graf_1 = self.sc_item_middle1
            graf_2 = self.pl_graf_middle1
            return graf_1, graf_2

        if str_now_graf == 'graf_middle2':
            graf_1 = self.sc_item_middle2
            graf_2 = self.pl_graf_middle2
            return graf_1, graf_2

        if str_now_graf == 'graf_middle3':
            graf_1 = self.sc_item_middle3
            graf_2 = self.pl_graf_middle3
            return graf_1, graf_2

        if str_now_graf == 'graf_down1':
            graf_1 = self.sc_item_down1
            graf_2 = self.pl_graf_down1
            return graf_1, graf_2

        if str_now_graf == 'graf_down2':
            graf_1 = self.sc_item_down2
            graf_2 = self.pl_graf_down2
            return graf_1, graf_2

        if str_now_graf == 'graf_down3':
            graf_1 = self.sc_item_down3
            graf_2 = self.pl_graf_down3
            return graf_1, graf_2


    def transfer_graf_top1(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_top1)
        now_graf = self.graf_top1
        str_now_graf = 'graf_top1'


    def transfer_graf_top2(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_top2)
        now_graf = self.graf_top2
        str_now_graf = 'graf_top2'

    def transfer_graf_top3(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_top3)
        now_graf = self.graf_top3
        str_now_graf = 'graf_top3'


    def transfer_graf_middle1(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_middle1)
        now_graf = self.graf_middle1
        str_now_graf = 'graf_middle1'


    def transfer_graf_middle2(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_middle2)
        now_graf = self.graf_middle2
        str_now_graf = 'graf_middle2'


    def transfer_graf_middle3(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_middle3)
        now_graf = self.graf_middle3
        str_now_graf = 'graf_middle3'


    def transfer_graf_down1(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_down1)
        now_graf = self.graf_down1
        str_now_graf = 'graf_down1'


    def transfer_graf_down2(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_down2)
        now_graf = self.graf_down2
        str_now_graf = 'graf_down2'


    def transfer_graf_down3(self):
        global now_graf, str_now_graf

        grafs_1, grafs_2 = self.check_now_graf()

        if grafs_1 != 0:
            grafs_1.addItem(grafs_2)

        self.sc_item_large.removeItem(self.sc_item_large)
        self.sc_item_large.clear()

        self.sc_item_large.addItem(self.pl_graf_down3)
        now_graf = self.graf_down3
        str_now_graf = 'graf_down3'

    def update_graf(self):
        if (time_sens == None) or (time_atm == None) or (time_RSC == None) or (time_ISC == None):
            return 1

        if len(time_RSC) == lenght:
            a_RSC_x.pop()
            a_RSC_y.pop()
            a_RSC_z.pop()

            time_RSC.pop()

            av_x.pop()
            av_y.pop()
            av_z.pop()

        self.pl_graf_top2.setData(x=a_RSC_x, y=time_RSC, pen=('r'), width=0.5)
        self.pl_graf_top2.setData(x=a_RSC_y, y=time_RSC, pen=('g'), width=0.5)
        self.pl_graf_top2.setData(x=a_RSC_z, y=time_RSC, pen=('b'), width=0.5)

        self.pl_graf_middle1.setData(x=av_x, y=time_RSC, pen=('r'), width=0.5)
        self.pl_graf_middle1.setData(x=av_y, y=time_RSC, pen=('g'), width=0.5)
        self.pl_graf_middle1.setData(x=av_z, y=time_RSC, pen=('b'), width=0.5)

        # Вывод в лог #
        # log = '' + 'sfkslk'+ "\n"
        # self.ui.textBrowser_2.append(log)
        #  #

        if len(time_ISC) == lenght:
            a_ISC_x.pop()
            a_ISC_y.pop()
            a_ISC_z.pop()

            time_ISC.pop()

            vmf_x.pop()
            vmf_y.pop()
            vmf_z.pop()

            v_x.pop()
            v_y.pop()
            v_z.pop()

            mov_x.pop()
            mov_y.pop()
            mov_z.pop()

        self.pl_graf_top1.setData(x=a_ISC_x, y=time_ISC, pen=('r'))
        self.pl_graf_top1.setData(x=a_ISC_y, y=time_ISC, pen=('g'))
        self.pl_graf_top1.setData(x=a_ISC_z, y=time_ISC, pen=('b'))

        self.pl_graf_middle3.setData(x=vmf_x, y=time_ISC, pen=('r'))
        self.pl_graf_middle3.setData(x=vmf_y, y=time_ISC, pen=('g'))
        self.pl_graf_middle3.setData(x=vmf_z, y=time_ISC, pen=('b'))

        self.pl_graf_top3.setData(x=v_x, y=time_ISC, pen=('r'))
        self.pl_graf_top3.setData(x=v_y, y=time_ISC, pen=('g'))
        self.pl_graf_top3.setData(x=v_z, y=time_ISC, pen=('b'))

        self.pl_graf_middle2.setData(x=mov_x, y=time_ISC, pen=('r'))
        self.pl_graf_middle2.setData(x=mov_y, y=time_ISC, pen=('g'))
        self.pl_graf_middle2.setData(x=mov_z, y=time_ISC, pen=('b'))

        if len(time_atm) == lenght:
            time_atm.pop()
            pressure_atmega.pop()
            temp_atmega.pop()

        self.pl_graf_down1.setData(x=temp_atmega, y=time_atm, pen=('r'))
        self.pl_graf_down2.setData(x=pressure_atmega, y=time_atm, pen=('r'))

        self.ui.state.clear()
        self.ui.state.setText(state)

        if len(time_sens) == lenght:
            time_sens.pop()
            pressure_sensors.pop()
            temp_sensors.pop()

        self.pl_graf_down1.setData(x=temp_sensors, y=time_sens, pen=('g'))
        self.pl_graf_down2.setData(x=pressure_sensors, y=time_sens, pen=('g'))

        self.pl_graf_down3.setData(x=x, y=y, pen=('r'))

        # self.plt.setData(pos=GPS, color=(1.0, 1.0, 1.0, 1.0)
        # i = len(x)
        # if i == 0:
        #     self.or_mod.translate(x, y, z)
        # else:
        #     self.or_mod.translate(x[i] - x[i - 1], y[i] - y[i - 1], z[i] - z[i - 1])
        # Цвета в pg.glColor

    # @QtCore.pyqtSlot(list)

    # def drawing_plot(self):
    #     pass

