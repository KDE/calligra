#!/usr/bin/env kross

"""
Interactive Python Console Docker for KSpread.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.koffice.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import Kross, KoScriptingDocker, KSpread
import PyQt4.Qt as Qt
from sip import wrapinstance, unwrapinstance

class ConsoleDocker(Qt.QWidget):
    def __init__(self):
        Qt.QWidget.__init__(self)
        layout = Qt.QVBoxLayout()
        layout.setMargin(0)
        layout.setSpacing(0)
        self.setLayout(layout)

        self.browser = Qt.QTextBrowser(self)
        self.browser.setFrameShape(Qt.QFrame.NoFrame)
        layout.addWidget(self.browser)

        self.edit = Qt.QComboBox(self)
        self.edit.setEditable(True)
        self.browser.setFocusProxy(self.edit)
        Qt.QObject.connect(self.edit.lineEdit(), Qt.SIGNAL("returnPressed()"), self.returnPressed)
        layout.addWidget(self.edit)

        docker = wrapinstance(KoScriptingDocker.__toPointer__(), Qt.QDockWidget)
        docker.setWidget(self)

    def returnPressed(self):
        text = "%s" % self.edit.currentText()
        self.edit.clearEditText()
        self.execute(text)

    def execute(self, code):
        _stdout = sys.stdout
        _stderr = sys.stderr
        class Base():
            def __init__(self, browser):
                self.browser = browser
            def write(self, text):
                self.browser.append(text)
        class StdOut(Base):
            def __init__(self, browser):
                Base.__init__(self, browser)
            def write(self, text):
                Base.write(self, text.strip().replace("\n","<br>"))
        class StdErr(Base):
            def __init__(self, browser):
                Base.__init__(self, browser)
            def write(self, text):
                Base.write(self, "<b>%s</b>" % text.strip().replace("\n","<br>"))
        sys.stdout = StdOut(self.browser)
        sys.stderr = StdErr(self.browser)
        
        sys.stdout.write("&gt; <i>%s</i>" % code.strip())
        exec code in globals(), globals()
        
        sys.stdout = _stdout
        sys.stderr = _stderr

print "Execute ConsoleDocker Script"
ConsoleDocker()
