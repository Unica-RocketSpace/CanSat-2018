import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QThread

from .pymavlink_usr import *
from .graphing import *




if __name__ == "__main__":
    # f.close()

    thread = Mavlink_thread()
    # msg = thread.new_record.connect(MyWin.msg_parser)
    thread.start()

    ######################################
    app = QtWidgets.QApplication(sys.argv)
    myapp = MyWin()
    myapp.show()
    ######################################


    # p_thread = PlotThread()
    # p_thread.start()

    sys.exit(app.exec_())
    # exit(main(sys.argv[1:]))