#!/usr/bin/env kross

"""
Interactive Python Console Docker for Sheets.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import Kross, KoDocker, KSpread
import PyQt4.Qt as Qt
import sip #wrapinstance, unwrapinstance

T = Kross.module("kdetranslation")

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

def executeFile(fileName):
    """ Execute a file. """
    if fileName.startswith("file://"):
        fileName = fileName[7:]
    fileName = fileName.replace("$HOME", _getHome())
    execfile(fileName, globals(), globals())

class _ConsoleDocker(Qt.QWidget):
    """ The docker widget. """

    class Editor(Qt.QWidget):
        """ An editor to edit python scripting code. """
        
        def __init__(self, docker):
            self.docker = docker
            self.filename = None
            Qt.QWidget.__init__(self, self.docker)
            layout = Qt.QVBoxLayout(self)
            layout.setMargin(0)
            layout.setSpacing(0)
            self.setLayout(layout)
            menubar = Qt.QMenuBar(self)
            layout.addWidget(menubar)
            self.edit = Qt.QTextEdit(self)
            self.edit.setWordWrapMode(Qt.QTextOption.NoWrap)
            layout.addWidget(self.edit)
            self.status = Qt.QLabel('', self)
            layout.addWidget(self.status)
            menu = Qt.QMenu(T.i18nc("Menu", "File").decode('utf-8'), menubar)
            menubar.addMenu(menu)
            self.addAction(menu, T.i18n("New").decode('utf-8'), self.newClicked)
            self.addAction(menu, T.i18n("Open...").decode('utf-8'), self.openClicked)
            self.addAction(menu, T.i18n("Save").decode('utf-8'), self.saveClicked)
            self.addAction(menu, T.i18n("Save As...").decode('utf-8'), self.saveAsClicked)
            self.editmenu = self.edit.createStandardContextMenu()
            self.editmenu.setTitle(T.i18nc("Menu", "Edit").decode('utf-8'))
            menubar.addMenu(self.editmenu)
            menu = Qt.QMenu(T.i18nc("Menu", "Build").decode('utf-8'), menubar)
            menubar.addMenu(menu)
            self.addAction(menu, T.i18n("Compile").decode('utf-8'), self.compileClicked)
            self.addAction(menu, T.i18n("Execute").decode('utf-8'), self.executeClicked)
        def addAction(self, menu, text, func):
            action = Qt.QAction(text, self)
            Qt.QObject.connect(action, Qt.SIGNAL("triggered(bool)"), func)
            menu.addAction(action)
            return action
        def newClicked(self, *args):
            self.edit.clear()
            self.status.setText('')
            self.filename = None
        def openClicked(self, *args):
            if self.filename:
                filename = self.filename
            else:
                filename = _getHome()
            filename = Qt.QFileDialog.getOpenFileName(self, T.i18n("Open File").decode('utf-8'), filename, "*.py;;*")
            if filename:
                try:
                    f = open(filename, "r")
                    self.edit.setText( "%s" % f.read() )
                    f.close()
                    self.filename = filename
                except IOError, (errno, strerror):
                    Qt.QMessageBox.critical(self, T.i18n("Error").decode('utf-8'), T.i18n("<qt>Failed to open file \"%1\"<br><br>%2</qt>", [filename], [strerror]).decode('utf-8'))
        def saveClicked(self, *args):
            if not self.filename:
                self.saveAsClicked()
                return
            try:
                f = open(self.filename, "w")
                f.write( "%s" % self.edit.toPlainText() )
                f.close()
            except IOError, (errno, strerror):
                qt.QMessageBox.critical(self, T.i18n("Error").decode('utf-8'), T.i18n("<qt>Failed to save file \"%1\"<br><br>%2</qt>", [self.filename], [strerror]).decode('utf-8'))
        def saveAsClicked(self, *args):
            if self.filename:
                filename = self.filename
            else:
                filename = _getHome()
            filename = Qt.QFileDialog.getSaveFileName(self, T.i18n("Save File").decode('utf-8'), filename, "*.py;;*")
            if filename:
                self.filename = filename
                self.saveClicked()
        def compileClicked(self, *args):
            import time, traceback
            text = "%s" % self.edit.toPlainText()
            try:
                compile(text, '', 'exec')
                self.status.setText(T.i18n("Compiled! %1", [time.strftime('%H:%M.%S')]).decode('utf-8'))
            except Exception, e:
                self.status.setText("%s" % e)
                traceback.print_exc(file=sys.stderr)
        def executeClicked(self, *args):
            import time
            err = self.docker.execute("%s" % self.edit.toPlainText())
            if not err:
                self.status.setText(T.i18n("Executed! %1", [time.strftime('%H:%M.%S')]).decode('utf-8'))
            else:
                self.status.setText("%s" % err)

    class Model(Qt.QAbstractItemModel):
        """ The model for the treeview that displays the content of our globals(). """

        class Item:
            """ An item within the model. """

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
                                _ConsoleDocker.Model.Item(self, getattr(self.object,s), s)
                            except:
                                pass
            def row(self):
                return 0
            def hasChildren(self):
                import types
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
            self.rootItem = _ConsoleDocker.Model.Item()
            for s in globals():
                if not s.startswith('_') and not s.startswith('PyQt4.Qt'):
                    _ConsoleDocker.Model.Item(self.rootItem, globals()[s], s)
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
                if role == Qt.Qt.DisplayRole:
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
            if role == Qt.Qt.DisplayRole:
                if section == 0:
                    return Qt.QVariant(T.i18n("Name").decode('utf-8'))
                if section == 1:
                    return Qt.QVariant(T.i18n("Type").decode('utf-8'))
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
        self.pages.addTab(consoleWidget, T.i18n("Console").decode('utf-8'))

        self.browser = Qt.QTextBrowser(consoleWidget)
        self.browser.setFrameShape(Qt.QFrame.NoFrame)
        consoleLayout.addWidget(self.browser)
        
        self.edit = Qt.QComboBox(consoleWidget)
        self.edit.setEditable(True)
        self.edit.insertItems(0, ['','print globals().keys()','print dir(Kross)','print dir(KSpread)','print KSpread.sheetNames()','print Qt.PYQT_VERSION_STR','print sip.SIP_VERSION_STR'])
        self.browser.setFocusProxy(self.edit)
        Qt.QObject.connect(self.edit.lineEdit(), Qt.SIGNAL("returnPressed()"), self.returnPressed)
        consoleLayout.addWidget(self.edit)

        self.pages.addTab(_ConsoleDocker.Editor(self), T.i18n("Editor").decode('utf-8'))

        inspWidget = Qt.QWidget(self)
        inspLayout = Qt.QVBoxLayout()
        inspLayout.setMargin(0)
        inspLayout.setSpacing(0)
        inspWidget.setLayout(inspLayout)
        self.pages.addTab(inspWidget, T.i18n("Inspect").decode('utf-8'))

        self.treeFilter = Qt.QLineEdit(inspWidget)
        inspLayout.addWidget(self.treeFilter)

        self.tree = Qt.QTreeView(inspWidget)
        inspLayout.addWidget(self.tree)
        self.tree.setFrameShape(Qt.QFrame.NoFrame)
        self.tree.setRootIsDecorated(True)
        self.tree.setSortingEnabled(False)
        self.tree.header().setClickable(False)
        
        self.model = _ConsoleDocker.Model()
        self.proxyModel = Qt.QSortFilterProxyModel(self.tree)
        self.proxyModel.setDynamicSortFilter(True)
        self.proxyModel.setFilterCaseSensitivity(Qt.Qt.CaseInsensitive)
        self.proxyModel.setSourceModel(self.model)
        self.tree.setModel(self.proxyModel)

        self.treeExpired = True
        Qt.QObject.connect(self.treeFilter, Qt.SIGNAL("textChanged(QString)"), self.proxyModel, Qt.SLOT("setFilterFixedString(QString)"))
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
        s = self.proxyModel.data(index, 0).toString()
        parent = index
        while True:
            parent = self.proxyModel.parent(parent)
            if not parent.isValid(): break
            s = "%s.%s" % (self.proxyModel.data(parent, 0).toString(), s)
        self.edit.lineEdit().setText("print %s" % s)
        self.pages.setCurrentIndex(0)
        #self.returnPressed()

    def currentChanged(self, *args):
        if self.pages.currentWidget() == self.tree:
            if self.treeExpired:
                self.tree.reset()
                self.treeExpired = False
        
    def execute(self, code):
        import sys, traceback
        _stdout = sys.stdout
        _stderr = sys.stderr
        err = None
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
            try:
                exec code in globals(), globals()
            except:
                err = sys.exc_info()[1]
                sys.stderr.write("".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))
        finally:
            sys.stdout = _stdout
            sys.stderr = _stderr
        return err

print "Execute _ConsoleDocker Script"
_ConsoleDocker()
