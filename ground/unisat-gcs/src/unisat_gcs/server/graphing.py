import sys
import time

import numpy as np
import pyqtgraph as pg
from Ground_new import *

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QThread
import pyqtgraph.opengl as gl

from pymavlink import mavutil

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
        MyWin.plotting(myapp)


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
        self.ui.glwid.show()

        self.drawing_plot()

        # Здесь прописываем событие нажатия на кнопку
        self.ui.pushButton_3.clicked.connect(self.Remove_graf)
        self.ui.commandLinkButton.clicked.connect(self.send_command)
        self.ui.pushButton_13.clicked.connect(self.transfer_graf_top1)
        self.ui.pushButton_14.clicked.connect(self.transfer_graf_top2)


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
        self.sc_item_top1.removeItem(self.sc_item_top1)
        self.sc_item_top1.clear()

        self.sc_item_top2.removeItem(self.sc_item_top2)
        self.sc_item_top2.clear()

        self.p_thread.terminate()
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

        # if str_now_graf == 'graf_top3':
        #     pass
        # if str_now_graf == 'graf_middle1':
        #     pass
        # if str_now_graf == 'graf_middle2':
        #     pass
        # if str_now_graf == 'graf_middle3':
        #     pass
        # if str_now_graf == 'graf_down1':
        #     pass
        # if str_now_graf == 'graf_down2':
        #     pass
        # if str_now_graf == 'graf_down3':
        #     pass


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


    def drawing_plot(self):
        self.p_thread = PlotThread()
        self.p_thread.start()


    def plotting(self):

        wait = 0.05
        sec = 0
        i = 0

        # Создаем numpy массивы для каждого графика
        x_1 = np.array([])
        y_1 = np.array([])

        x_2 = np.array([])
        y_2 = np.array([])
        graf_3d = np.array([0, 0, 0])

        self.pl_graf_top1 = self.sc_item_top1.plot()
        self.pl_graf_top2 = self.sc_item_top2.plot()

        index = [0]
        while True:

            coords = coord.consid_coord(global_vars, sec)
            x_app = coords[0]
            y_app = coords[1]

            if coords[2] == '1':
                if len(x_1) == 20:
                    x_1 = np.delete(x_1, index)
                    y_1 = np.delete(y_1, index)

                x_1 = np.hstack((x_1, x_app))
                y_1 = np.hstack((y_1, y_app))
                self.pl_graf_top1.setData(x=x_1, y=y_1, pen=('b'), width=0.5)

                # Вывод в лог #
                # log = '' + 'sfkslk'+ "\n"
                # self.ui.textBrowser_2.append(log)
                #  #
            else:
                if len(x_2) == 20:
                    x_2 = np.delete(x_2, index)
                    y_2 = np.delete(y_2, index)

                x_2 = np.hstack((x_2, x_app))
                y_2 = np.hstack((y_2, y_app))
                self.pl_graf_top2.setData(x=x_2, y=y_2, pen=('r'))


            # График для двух прямых на одной коорд плоскости
            # coords = coord.consid_coord(global_vars, sec)
            # x_app = coords[0]
            # y_app = coords[1]
            #
            # if coords[2] =='1':
            #     x_1 = np.hstack((x_1, x_app))
            #     y_1 = np.hstack((y_1, y_app))
            #
            #     xe.setData(x=x_1, y=y_1, pen=(0,1,0), width=0.5)
            #     time.sleep(wait)
            # else:
            #
            #     x_2 = np.hstack((x_2, x_app))
            #     y_2 = np.hstack((y_2, y_app))
            #
            #     xe.setData(x=x_2, y=y_2, pen=(1, 0, 0), width=0.5)
            # ##############

            # app = []
            # app.append(x_app)
            # app.append(y_app)
            # app.append(sec)

            app = np.array([x_app, y_app, sec])
            print('app = ', app)
            graf_3d = np.vstack((graf_3d, app))

            self.plt.setData(pos=graf_3d, color=(1.0, 1.0, 1.0, 1.0))

            sec += 0.05
            time.sleep(wait)

            # Цвета в pg.glColor


class subwindow (QtWidgets.QMainWindow):
    def createWindow(self, WindowWidth, WindowHeight):
        parent = None
        super(subwindow, self).__init__(parent)
        self.resize(WindowWidth, WindowHeight)
        self.show()


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    myapp = MyWin()
    myapp.show()
    sys.exit(app.exec_())