import sys
from PyQt4.QtGui import *

app = QApplication(sys.argv)
QMessageBox.information(QWidget(), "Test", "Hello World")