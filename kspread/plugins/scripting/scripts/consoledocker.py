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

class ConsoleModel(Qt.QAbstractItemModel):
    
    class Item:
        def __init__(self, name = "aaaaaa"):
            self.name = name
            self.children = []
        def hasChildren(self):
            return true
        def child(self, row):
            #if len(self.children) <= 0:
                #lazyLoadChildren()
            return self.children[row]
        def childCount(self):
            #if len(self.children) <= 0:
                #lazyLoadChildren()
            return len(self.children)
        def data(self, column):
            return Qt.QVariant.new("%s" % self.name)

    def __init__(self):
        Qt.QAbstractItemModel.__init__(self)
        self.rootItem = ConsoleModel.Item()
        for s in dir( globals() ):
            self.rootItem.children.append( ConsoleModel.Item(s) )
    def columnCount(self, parent):
        return 1
    def flags(self, index):
        if not index.isValid():
            return Qt.ItemIsEnabled
        return Qt.ItemIsEnabled | Qt.ItemIsSelectable
    def index(self, row, column, parent):
        if parent.isValid():
            parentItem = parent.internalPointer()
        else:
            parentItem = self.rootItem
        childItem = parentItem.child(row)
        if childItem:
            return self.createIndex(row, column, childItem)
        return Qt.QModelIndex()
    def parent(self, index):
        if index.isValid():
            parentItem = index.internalPointer().parent()
            if parentItem != self.rootItem:
                return createIndex(parentItem.row, 0, parentItem)
        return Qt.QModelIndex()
    def data(self, index, role):
        if not index.isValid():
            return Qt.QVariant()
        if role == Qt.DisplayRole:
            item = index.internalPointer()
            return item.data(index.column)
        return Qt.QVariant()
    def rowCount(self, parent):
        if parent.isValid():
            parentItem = parent.internalPointer()
        else:
            parentItem = self.rootItem
        return parentItem.childCount()
    def hasChildren(self, parent):
        if parent.isValid():
            parentItem = parent.internalPointer()
        else:
            parentItem = self.rootItem
        return parentItem.hasChildren()
    
class ConsoleDocker(Qt.QWidget):
    
    def __init__(self):
        Qt.QWidget.__init__(self)
        layout = Qt.QVBoxLayout()
        layout.setMargin(0)
        layout.setSpacing(0)
        self.setLayout(layout)
        
        pages = Qt.QTabWidget(self)
        layout.addWidget(pages)

        self.browser = Qt.QTextBrowser(self)
        self.browser.setFrameShape(Qt.QFrame.NoFrame)
        pages.addTab(self.browser, "Output")

        self.edit = Qt.QComboBox(self)
        self.edit.setEditable(True)
        self.edit.insertItems(0, ['print dir(Kross)','print dir(KSpread)','print KSpread.sheetNames()'])
        self.browser.setFocusProxy(self.edit)
        Qt.QObject.connect(self.edit.lineEdit(), Qt.SIGNAL("returnPressed()"), self.returnPressed)
        layout.addWidget(self.edit)

        self.listview = Qt.QListView(self)
        self.listview.setFrameShape(Qt.QFrame.NoFrame)
        model = ConsoleModel()
        self.listview.setModel(model)
        pages.addTab(self.listview, "Objects")

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
