#!/usr/bin/env kross

"""
Interactive Python Console Docker for KSpread.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.koffice.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import os, sys, types
import Kross, KoDocker, KSpread
import PyQt4.Qt as Qt
import sip #wrapinstance, unwrapinstance

def _getHome():
    """ Return the homedirectory. """
    import os
    try:
        home = os.getenv("HOME")
        if not home:
            import pwd
            user = os.getenv("USER") or os.getenv("LOGNAME")
            if not user:
                pwent = pwd.getpwuid(os.getuid())
            else:
                pwent = pwd.getpwnam(user)
            home = pwent[6]
        return home
    except (KeyError, ImportError):
        return os.curdir

#def executeFile(fileName):
    #""" Execute a file. """
    #if fileName.startswith("file://"):
        #fileName = fileName[7:]
    #fileName = fileName.replace("$HOME", _getHome())
    #execfile(fileName, globals(), globals())

class _ConsoleDocker(Qt.QWidget):
    
    class ConsoleModel(Qt.QAbstractItemModel):
    
        class Item:
            def __init__(self, parentItem = None, object = None, name = ""):
                self.parent = parentItem
                self.object = object
                self.name = name
                self.children = []
                if self.parent:
                    self.parent.children.append(self)
            def lazyLoadChildren(self):
                if self.object:
                    for s in dir(self.object):
                        if not s.startswith('_'):
                            try:
                                _ConsoleDocker.ConsoleModel.Item(self, getattr(self.object,s), s)
                            except:
                                pass
            def row(self):
                return 0
            def hasChildren(self):
                if not self.parent:
                    return True
                if isinstance(self.object, types.ClassType) or isinstance(self.object, types.ModuleType):
                    return True
                if type(self.object) == type(Kross):
                    return True
                return False
            def child(self, row):
                if len(self.children) == 0:
                    self.lazyLoadChildren()
                return self.children[row]
            def childCount(self):
                if len(self.children) == 0:
                    self.lazyLoadChildren()
                return len(self.children)
            def data(self, column):
                if column == 0:
                    return Qt.QVariant("%s" % self.name)
                if column == 1 and self.object:
                    if type(self.object) == type(Kross): return Qt.QVariant("ext")
                    t = "%s" % type(self.object)
                    return Qt.QVariant( t[ t.find("'")+1 : t.rfind("'") ] )
                return Qt.QVariant()
            
        def __init__(self):
            Qt.QAbstractItemModel.__init__(self)
            self.rootItem = _ConsoleDocker.ConsoleModel.Item()
            for s in globals():
                if not s.startswith('_') and not s.startswith('PyQt4.Qt'):
                    _ConsoleDocker.ConsoleModel.Item(self.rootItem, globals()[s], s)
        def columnCount(self, parent):
            return 2
        def flags(self, index):
            return Qt.QAbstractItemModel.flags(self, index)
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
                parentItem = index.internalPointer().parent
                if parentItem and parentItem != self.rootItem:
                    return self.createIndex(parentItem.row(), 0, parentItem)
            return Qt.QModelIndex()
        def data(self, index, role):
            if index.isValid():
                if role == 0: #Qt.DisplayRole:
                    return index.internalPointer().data(index.column())
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
        def headerData(self, section, orientation, role = 0):
            if role == 0: #Qt.DisplayRole:
                if section == 0:
                    return Qt.QVariant("Name")
                if section == 1:
                    return Qt.QVariant("Type")
            return Qt.QVariant()

    def __init__(self):
        Qt.QWidget.__init__(self)
        layout = Qt.QVBoxLayout()
        layout.setMargin(0)
        layout.setSpacing(0)
        self.setLayout(layout)
        
        self.pages = Qt.QTabWidget(self)
        layout.addWidget(self.pages)

        consoleWidget = Qt.QWidget(self)
        consoleLayout = Qt.QVBoxLayout()
        consoleLayout.setMargin(0)
        consoleLayout.setSpacing(0)
        consoleWidget.setLayout(consoleLayout)
        self.pages.addTab(consoleWidget, "Console")

        self.browser = Qt.QTextBrowser(consoleWidget)
        self.browser.setFrameShape(Qt.QFrame.NoFrame)
        consoleLayout.addWidget(self.browser)
        
        self.edit = Qt.QComboBox(consoleWidget)
        self.edit.setEditable(True)
        self.edit.insertItems(0, ['','print globals().keys()','print dir(Kross)','print dir(KSpread)','print KSpread.sheetNames()','print Qt.PYQT_VERSION_STR','print sip.SIP_VERSION_STR'])
        self.browser.setFocusProxy(self.edit)
        Qt.QObject.connect(self.edit.lineEdit(), Qt.SIGNAL("returnPressed()"), self.returnPressed)
        consoleLayout.addWidget(self.edit)

        self.tree = Qt.QTreeView(self)
        self.tree.setFrameShape(Qt.QFrame.NoFrame)
        self.tree.setRootIsDecorated(True)
        self.tree.setSortingEnabled(False)
        #self.tree.setAlternatingRowColors(True)
        #self.tree.header().setResizeMode(0, Qt.QHeaderView.Stretch)
        #self.tree.header().hide()
        self.tree.header().setClickable(False)
        self.model = _ConsoleDocker.ConsoleModel()
        self.tree.setModel(self.model)
        self.pages.addTab(self.tree, "Inspect")

        self.treeExpired = True
        Qt.QObject.connect(self.tree, Qt.SIGNAL("activated(QModelIndex)"), self.itemActivated)
        Qt.QObject.connect(self.pages, Qt.SIGNAL("currentChanged(int)"), self.currentChanged)

        docker = sip.wrapinstance(KoDocker.__toPointer__(), Qt.QDockWidget)
        docker.setWidget(self)

    def returnPressed(self):
        text = "%s" % self.edit.currentText()
        self.edit.clearEditText()
        self.execute(text)
        self.treeExpired = True
        
    def itemActivated(self, index):
        s = self.model.data(index, 0).toString()
        parent = index
        while True:
            parent = self.model.parent(parent)
            if not parent.isValid(): break
            s = "%s.%s" % (self.model.data(parent, 0).toString(), s)
        self.edit.lineEdit().setText("print %s" % s)
        self.pages.setCurrentIndex(0)
        #self.returnPressed()

    def currentChanged(self, *args):
        if self.pages.currentWidget() == self.tree:
            if self.treeExpired:
                self.tree.reset()
                self.treeExpired = False
        
    def execute(self, code):
        _stdout = sys.stdout
        _stderr = sys.stderr
        try:
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
        finally:
            sys.stdout = _stdout
            sys.stderr = _stderr

print "Execute _ConsoleDocker Script"
_ConsoleDocker()
