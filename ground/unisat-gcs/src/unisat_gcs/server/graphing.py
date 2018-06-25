import sys
import time

import numpy as np
import pyqtgraph as pg
from gcs_ui import *

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QThread
import pyqtgraph.opengl as gl

import __main__

# from . import __main__

log_text = ''
global_vars={'x': 0, 'y': 0}
now_graf = None
str_now_graf = None

# glwid = gl.GLViewWidget()

class PlotThread(QThread):
    def __init__(self):
        QThread.__init__(self)

    def __del__(self):
        self.wait()

    def run(self):
        myapp = MyWin()
        __main__.msg_parser(myapp)


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
        # self.sc_item = self.ui.plot.addPlot(title="Acsell")
        #######

        self.sc_item_top1 = pg.PlotItem(title='V')
        self.ui.plot_top.addItem(self.sc_item_top1)
        self.graf_top1 = pg.PlotCurveItem()
        self.sc_item_top1.addItem(self.graf_top1)

        self.sc_item_top2 = pg.PlotItem()
        self.ui.plot_top.addItem(self.sc_item_top2)
        self.graf_top2 = pg.PlotCurveItem()
        self.sc_item_top2.addItem(self.graf_top2)

        self.sc_item_top3 = pg.PlotItem()
        self.ui.plot_top.addItem(self.sc_item_top3)
        self.graf_top3 = pg.PlotCurveItem()
        self.sc_item_top3.addItem(self.graf_top3)


        self.ui.glv_middle = pg.GraphicsLayoutWidget(self.ui.centralwidget)
        self.ui.middle.addWidget(self.ui.glv_middle)
        self.ui.plot_middle = pg.GraphicsLayout()
        self.ui.glv_middle.addItem(self.ui.plot_middle)

        self.sc_item_middle1 = pg.PlotItem()
        self.ui.plot_middle.addItem(self.sc_item_middle1)
        self.graf_middle1 = pg.PlotCurveItem()
        self.sc_item_middle1.addItem(self.graf_middle1)

        self.sc_item_middle2 = pg.PlotItem()
        self.ui.plot_middle.addItem(self.sc_item_middle2)
        self.graf_middle2 = pg.PlotCurveItem()
        self.sc_item_middle2.addItem(self.graf_middle2)

        self.sc_item_middle3 = pg.PlotItem()
        self.ui.plot_middle.addItem(self.sc_item_middle3)
        self.graf_middle3 = pg.PlotCurveItem()
        self.sc_item_middle3.addItem(self.graf_middle3)


        self.ui.glv_down = pg.GraphicsLayoutWidget(self.ui.centralwidget)
        self.ui.down.addWidget(self.ui.glv_down)
        self.ui.plot_down = pg.GraphicsLayout()
        self.ui.glv_down.addItem(self.ui.plot_down)

        self.sc_item_down1 = pg.PlotItem()
        self.ui.plot_down.addItem(self.sc_item_down1)
        self.graf_down1 = pg.PlotCurveItem()
        self.sc_item_down1.addItem(self.graf_down1)

        self.sc_item_down2 = pg.PlotItem()
        self.ui.plot_down.addItem(self.sc_item_down2)
        self.graf_down2 = pg.PlotCurveItem()
        self.sc_item_down2.addItem(self.graf_down2)

        self.sc_item_down3 = pg.PlotItem()
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

        self.drawing_plot()

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
        # self.sc_item_top1.removeItem(self.sc_item_top1)
        # self.sc_item_top1.clear()
        #
        # self.sc_item_top2.removeItem(self.sc_item_top2)
        # self.sc_item_top2.clear()
        #
        # self.p_thread.terminate()
        self.drawing_plot()


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


    def drawing_plot(self):
        self.p_thread = PlotThread()
        self.p_thread.start()


    def plotting(self, msg):
        global a_ISC_x, a_ISC_y, a_ISC_z, a_RSC_x, a_RSC_y, a_RSC_z, v_x, v_y, v_z, av_x, av_y, av_z, mov_x, mov_y, mov_z, vmf_x, vmf_y, vmf_z
        global x, y, z, state, temp_atmega, temp_sensors, pressure_atmega, pressure_sensors, quat, time_atm, time_RSC, time_ISC, time_sens, GPS

        if time_sens == None or time_atm == None or time_RSC == None or time_ISC:
            return 'No data'

        self.pl_graf_top1 = self.sc_item_top1.plot()
        self.pl_graf_top2 = self.sc_item_top2.plot()
        self.pl_graf_top3 = self.sc_item_top3.plot()

        self.pl_graf_middle1 = self.sc_item_middle1.plot()
        self.pl_graf_middle2 = self.sc_item_middle2.plot()
        self.pl_graf_middle3 = self.sc_item_middle3.plot()

        self.pl_graf_down1 = self.sc_item_down1.plot()
        self.pl_graf_down2 = self.sc_item_down2.plot()
        self.pl_graf_down3 = self.sc_item_down3.plot()


        if len(time_RSC) == 20:
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


        if len(time_ISC) == 20:
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

        self.pl_graf_top1.setData(x=a_ISC_x, y=time_ISC, pen=('r'))
        self.pl_graf_top1.setData(x=a_ISC_y, y=time_ISC, pen=('g'))
        self.pl_graf_top1.setData(x=a_ISC_z, y=time_ISC, pen=('b'))

        self.pl_graf_middle3.setData(x=vmf_x, y=time_ISC, pen=('r'))
        self.pl_graf_middle3.setData(x=vmf_y, y=time_ISC, pen=('g'))
        self.pl_graf_middle3.setData(x=vmf_z, y=time_ISC, pen=('b'))

        self.pl_graf_top3.setData(x=v_x, y=time_ISC, pen=('r'))
        self.pl_graf_top3.setData(x=v_y, y=time_ISC, pen=('g'))
        self.pl_graf_top3.setData(x=v_z, y=time_ISC, pen=('b'))


        if len(time_atm) == 20:
            time_atm.pop()
            pressure_atmega.pop()
            temp_atmega.pop()

        self.pl_graf_down1.setData(x=temp_atmega, y=time_atm, pen=('r'))
        self.pl_graf_down2.setData(x=pressure_atmega, y=time_atm, pen=('r'))

        self.ui.state.clear()
        self.ui.state.append(state)


        if len(time_sens) == 20:
            time_sens.pop()
            pressure_sensors.pop()
            temp_sensors.pop()

        self.pl_graf_down1.setData(x=temp_sensors, y=time_sens, pen=('g'))
        self.pl_graf_down2.setData(x=pressure_sensors, y=time_sens, pen=('g'))


        # app = []
        # app.append(x_app)
        # app.append(y_app)
        # app.append(sec)
        #
        # app = np.array([x_app, y_app, sec])
        # print('app = ', app)
        # graf_3d = np.vstack((graf_3d, app))

        # self.plt.setData(pos=GPS, color=(1.0, 1.0, 1.0, 1.0)
        i = len(x)
        if i == 0:
            self.or_mod.translate(x, y, z)
        else:
            self.or_mod.translate(x[i] - x[i-1], y[i] - y[i-1], z[i] - z[i-1])
        # Цвета в pg.glColor


# if __name__ == "__main__":
#     app = QtWidgets.QApplication(sys.argv)
#     myapp = MyWin()
#     myapp.show()
#     sys.exit(app.exec_())