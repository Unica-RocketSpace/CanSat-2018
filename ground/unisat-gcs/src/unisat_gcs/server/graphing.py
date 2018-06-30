import sys
import time
import os

import numpy as np
import pyqtgraph as pg
from .gcs_ui import *

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *


import pyqtgraph.opengl as gl

from PyQt5.QtCore import QThread
import logging
import math

from . import _log as _root_log


_log = _root_log.getChild("main")

# index = 0
# print(way)


log_text = ''
global_vars={'x': 0, 'y': 0}
now_graf = None
str_now_graf = None



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

        self.lenght = 200

        self.temp_atmega = []
        self.pressure_atmega = []
        self.time_atm = []
        self.state = None

        self.a_RSC_x = []
        self.a_RSC_y = []
        self.a_RSC_z = []

        self.av_x = []
        self.av_y = []
        self.av_z = []

        self.time_RSC = []

        self.a_ISC_x = []
        self.a_ISC_y = []
        self.a_ISC_z = []

        self.v_x = []

        self.v_y = []
        self.v_z = []

        self.mov_x = []
        self.mov_y = []
        self.mov_z = []

        self.vmf_x = []
        self.vmf_y = []
        self.vmf_z = []

        self.time_ISC = []

        self.quat = []

        self.temp_sensors = []
        self.pressure_sensors = []
        self.time_sens = []

        self.x = []
        self.y = []
        self.z = []

        self.GPS = []

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

        self.pl_graf_top1_x = self.sc_item_top1.plot()
        self.pl_graf_top2_x = self.sc_item_top2.plot()
        self.pl_graf_top3_x = self.sc_item_top3.plot()

        self.pl_graf_top1_y = self.sc_item_top1.plot()
        self.pl_graf_top2_y = self.sc_item_top2.plot()
        self.pl_graf_top3_y = self.sc_item_top3.plot()

        self.pl_graf_top1_z = self.sc_item_top1.plot()
        self.pl_graf_top2_z = self.sc_item_top2.plot()
        self.pl_graf_top3_z = self.sc_item_top3.plot()

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


    @QtCore.pyqtSlot(list)
    def atm_msg(self, msgs):
        i = 0
        for i in range(len(msgs)):
            self.time_atm.append(msgs[i].time)
            self.pressure_atmega.append(msgs[i].pressure)
            self.temp_atmega.append(msgs[i].temp)
            self.state = msgs[i].state

        if len(self.time_atm) == self.lenght:
            self.time_atm = self.time_atm[4:(self.lenght-1)]
            self.pressure_atmega = self.pressure_atmega[4:(self.lenght-1)]
            self.temp_atmega = self.temp_atmega[4:(self.lenght-1)]

        self.pl_graf_down1.setData(x=self.time_atm, y=self.temp_atmega, pen=('r'))
        self.pl_graf_down2.setData(x=self.time_atm, y=self.pressure_atmega, pen=('r'))

        self.ui.state.clear()
        self.ui.state.setText(str(self.state))


    @QtCore.pyqtSlot(list)
    def imu_rsc_msg(self, msgs):
        i = 0
        for i in range(len(msgs)):
            self.a_RSC_x.append(msgs[i].accel[0])
            self.a_RSC_y.append(msgs[i].accel[1])
            self.a_RSC_z.append(msgs[i].accel[2])

            self.time_RSC.append(msgs[i].time)

            self.av_x.append(msgs[i].gyro[0])
            self.av_y.append(msgs[i].gyro[1])
            self.av_z.append(msgs[i].gyro[2])

        if len(self.time_RSC) == self.lenght:
            self.a_RSC_x = self.a_RSC_x[4:(self.lenght - 1)]
            self.a_RSC_y = self.a_RSC_y[4:(self.lenght - 1)]
            self.a_RSC_z = self.a_RSC_z[4:(self.lenght - 1)]
            self.time_RSC = self.time_RSC[4:(self.lenght - 1)]
            self.av_x = self.av_x[4:(self.lenght - 1)]
            self.av_y = self.av_y[4:(self.lenght - 1)]
            self.av_z = self.av_z[4:(self.lenght - 1)]

        self.pl_graf_top2_x.setData(x=self.time_RSC, y=self.a_RSC_x, pen=('r'), width=0.5)
        self.pl_graf_top2_y.setData(x=self.time_RSC, y=self.a_RSC_y, pen=('g'), width=0.5)
        self.pl_graf_top2_z.setData(x=self.time_RSC, y=self.a_RSC_z, pen=('b'), width=0.5)

        self.pl_graf_middle1.setData(x=self.time_RSC, y=self.av_x, pen=('r'), width=0.5)
        self.pl_graf_middle1.setData(x=self.time_RSC, y=self.av_y, pen=('g'), width=0.5)
        self.pl_graf_middle1.setData(x=self.time_RSC, y=self.av_z, pen=('b'), width=0.5)

        # Вывод в лог #
        # log = '' + 'sfkslk'+ "\n"
        # self.ui.textBrowser_2.append(log)
        #  #

    @QtCore.pyqtSlot(list)
    def imu_isc_msg(self, msgs):
        i = 0
        for i in range(len(msgs)):
            self.a_ISC_x.append(msgs[i].accel[0])
            self.a_ISC_y.append(msgs[i].accel[1])
            self.a_ISC_z.append(msgs[i].accel[2])

            self.time_ISC.append(msgs[i].time)

            self.vmf_x.append(msgs[i].compass[0])
            self.vmf_y.append(msgs[i].compass[1])
            self.vmf_z.append(msgs[i].compass[2])

            self.v_x.append(msgs[i].velocities[0])
            self.v_y.append(msgs[i].velocities[1])
            self.v_z.append(msgs[i].velocities[2])

            self.mov_x.append(msgs[i].coordinates[0])
            self.mov_y.append(msgs[i].coordinates[1])
            self.mov_z.append(msgs[i].coordinates[2])

        if len(self.time_ISC) == self.lenght:
            self.a_ISC_x = self.a_ISC_x[4:(self.lenght - 1)]
            self.a_ISC_y = self.a_ISC_y[4:(self.lenght - 1)]
            self.a_ISC_z = self.a_ISC_z[4:(self.lenght - 1)]
            self.time_ISC = self.time_ISC[4:(self.lenght - 1)]
            self.vmf_x = self.vmf_x[4:(self.lenght - 1)]
            self.vmf_y = self.vmf_y[4:(self.lenght - 1)]
            self.vmf_z = self.vmf_z[4:(self.lenght - 1)]
            self.v_x = self.v_x[4:(self.lenght - 1)]
            self.v_y = self.v_y[4:(self.lenght - 1)]
            self.v_z = self.v_z[4:(self.lenght - 1)]
            self.mov_x = self.mov_x[4:(self.lenght - 1)]
            self.mov_y = self.mov_y[4:(self.lenght - 1)]
            self.mov_z = self.mov_z[4:(self.lenght - 1)]

        self.pl_graf_top1.setData(x=self.time_ISC, y=self.a_ISC_x, pen=('r'))
        self.pl_graf_top1.setData(x=self.time_ISC, y=self.a_ISC_y, pen=('g'))
        self.pl_graf_top1.setData(x=self.time_ISC, y=self.a_ISC_z, pen=('b'))

        self.pl_graf_middle3.setData(x=self.time_ISC, y=self.vmf_x, pen=('r'))
        self.pl_graf_middle3.setData(x=self.time_ISC, y=self.vmf_y, pen=('g'))
        self.pl_graf_middle3.setData(x=self.time_ISC, y=self.vmf_z, pen=('b'))

        self.pl_graf_top3.setData(x=self.time_ISC, y=self.v_x, pen=('r'))
        self.pl_graf_top3.setData(x=self.time_ISC, y=self.v_y, pen=('g'))
        self.pl_graf_top3.setData(x=self.time_ISC, y=self.v_z, pen=('b'))

        self.pl_graf_middle2.setData(x=self.time_ISC, y=self.mov_x, pen=('r'))
        self.pl_graf_middle2.setData(x=self.time_ISC, y=self.mov_y, pen=('g'))
        self.pl_graf_middle2.setData(x=self.time_ISC, y=self.mov_z, pen=('b'))


    @QtCore.pyqtSlot(list)
    def sens_msg(self, msgs):
        i = 0
        for i in range(len(msgs)):
            self.time_sens.append(msgs[i].time)
            self.pressure_sensors.append(msgs[i].pressure)
            self.temp_sensors.append(msgs[i].temp)

        if len(self.time_sens) == self.lenght:
            self.time_sens = self.time_sens[4:(self.lenght - 1)]
            self.pressure_sensors = self.pressure_sensors[4:(self.lenght - 1)]
            self.temp_sensors = self.temp_sensors[4:(self.lenght - 1)]

        self.pl_graf_down1.setData(x=self.time_sens, y=self.temp_sensors, pen=('g'))
        self.pl_graf_down2.setData(x=self.time_sens, y=self.pressure_sensors, pen=('g'))



    @QtCore.pyqtSlot(list)
    def gps_msg(self, msgs):
        i = 0
        for i in range(len(msgs)):
            self.x.append(msgs[i].coordinates[0])
            self.y.append(msgs[i].coordinates[1])

        if len(self.x) == self.lenght:
            self.x = self.x[4:(self.lenght - 1)]
            self.y = self.y[4:(self.lenght - 1)]

        self.pl_graf_down3.setData(x=self.x, y=self.y, pen=('r'))

        # self.plt.setData(pos=GPS, color=(1.0, 1.0, 1.0, 1.0))
        # i = len(x)
        # if i == 0:
        #     self.or_mod.translate(x, y, z)
        # else:
        #     self.or_mod.translate(x[i] - x[i - 1], y[i] - y[i - 1], z[i] - z[i - 1])
        # Цвета в pg.glColor


