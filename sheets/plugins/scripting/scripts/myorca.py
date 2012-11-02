#!/usr/bin/env kross

import urllib, Kross, KSpread

T = Kross.module("kdetranslation")

class MyConfig:

    def __init__(self):
        self.url = "http://127.0.0.1:20433"
        self.sheetRange = "A1:F50"
        self.cellNameOnSelectionChanged = True
        self.cellValueOnSelectionChanged = True #TODO
        self.sheetNameOnSheetChanged = True

class MyOrca:

    def __init__(self, config):
        self.config = config
    def _send(self, data):
        f = urllib.urlopen(self.config.url, data)
        s = f.read()
        f.close()
        return s
    def speak(self, text):
        self._send("speak:%s" % text)
    def stop(self):
        self._send("stop")
    def isSpeaking(self):
        return self._send("isSpeaking")

class MyKSpread:

    def __init__(self, action, myorca):
        self.action = action
        self.myorca = myorca
        self.config = self.myorca.config

        def cleanUp(*args):
            if hasattr(__main__,"_KSpreadOrca"):
                getattr(__main__,"_KSpreadOrca").finalized()
        self.action.connect("started(Kross::Action*)", cleanUp)
        self.action.connect("finalized(Kross::Action*)", cleanUp)

        self._view = KSpread.view()
        self._setSheet( KSpread.currentSheet() )

        def selectionChanged():
            print "Selection changed"
            s = KSpread.currentSheet()
            if s:
                if s.sheetName() != self.sheetName:
                    self._setSheet(s)
                    if self.config.sheetNameOnSheetChanged:
                        self.speakSheetName()
                else:
                    if self.config.cellNameOnSelectionChanged:
                        self.speakCellName()
                    if self.config.cellValueOnSelectionChanged:
                        self.speakCellValue()
        self._view.connect("selectionChanged()", selectionChanged)

    def __del__(self):
        pass

    def finalized(self):
        if hasattr(self,"_listener") and self._listener:
            print "FINALIZE!!!"
            self._listener.deleteLater()
            self._listener = None
        if hasattr(__main__,"_KSpreadOrca"):
            delattr(__main__,"_KSpreadOrca")
        self.action = None
        self.myorca = None
        self.config = None
        self._view = None
        self.sheet = None
        self.__del__()

    def _setSheet(self, sheet):
        self.sheet = sheet
        self.sheetName = self.sheet.sheetName()

        self._listener = KSpread.createListener(self.sheetName, self.config.sheetRange)
        if not self._listener:
            raise Exception, T.i18n("Failed to create listener")
        def cellChanged(column, row):
            text = self.sheet.text(column, row)
            if text:
                #if self.myorca.isSpeaking():
                #    self.myorca.stop()
                self.myorca.speak(text)
        self._listener.connect("cellChanged(int,int)", cellChanged)

        def sheetNameChanged(*args):
            self.myorca.speak(T.i18n("Sheet name changed"))
        self.sheet.connect("nameChanged()", sheetNameChanged)

        def sheetShowChanged(*args):
            self.myorca.speak(T.i18n("Show sheet %1", [self.sheetName]))
        self.sheet.connect("showChanged()", sheetShowChanged)

        def sheetHideChanged(*args):
            self.myorca.speak(T.i18n("Hide sheet %1"), [self.sheetName])
        self.sheet.connect("hideChanged()", sheetHideChanged)

    def speakCellName(self):
        sheet = KSpread.currentSheet()
        (x1,y1,x2,y2) = KSpread.view().selection()
        p1 = sheet.cellName(x1,y1)
        self.myorca.speak(p1)

    def speakCellValue(self):
        sheet = KSpread.currentSheet()
        (x1,y1,x2,y2) = KSpread.view().selection()
        t = sheet.text(x1,y1)
        self.myorca.speak(t)

    def speakSheetName(self):
        n = KSpread.view().sheet()
        self.myorca.speak(n)

class MyDialog:
    def __init__(self, action, mykspread):
        self.action = action
        self.mykspread = mykspread
        self.config = mykspread.config

        forms = Kross.module("forms")
        self.dialog = forms.createDialog(T.i18n("Orca Screen Reader"))
        self.dialog.setButtons("None")
        self.dialog.setFaceType("Tabbed") #Auto Plain List Tree Tabbed
        #self.dialog.minimumWidth = 300
        self.dialog.minimumHeight = 40

        page = self.dialog.addPage(T.i18n("Action"),"")

        self.cellNameBtn = forms.createWidget(page, "QPushButton")
        self.cellNameBtn.text = T.i18n("Cell Name")
        #self.cellNameBtn.shortcut = "Ctrl+S"
        self.cellNameBtn.connect("clicked()", self.cellNameBtnClicked)

        self.cellValueBtn = forms.createWidget(page, "QPushButton")
        self.cellValueBtn.text = T.i18n("Cell Value")
        self.cellValueBtn.connect("clicked()", self.cellValueBtnClicked)

        self.sheetNameBtn = forms.createWidget(page, "QPushButton")
        self.sheetNameBtn.text = T.i18n("Sheet Name")
        self.sheetNameBtn.connect("clicked()", self.sheetNameBtnClicked)

        page = self.dialog.addPage("Option","")

        self.cellNameCheckbox = forms.createWidget(page, "QCheckBox")
        self.cellNameCheckbox.text = T.i18n("Cell Name if selection changed")
        self.cellNameCheckbox.checked = self.config.cellNameOnSelectionChanged
        self.cellNameCheckbox.connect("stateChanged(int)", self.optionChanged)

        self.cellValueCheckbox = forms.createWidget(page, "QCheckBox")
        self.cellValueCheckbox.text = T.i18n("Cell Value if selection changed")
        self.cellValueCheckbox.checked = self.config.cellValueOnSelectionChanged
        self.cellValueCheckbox.connect("stateChanged(int)", self.optionChanged)

        self.sheetNameChanged = forms.createWidget(page, "QCheckBox")
        self.sheetNameChanged.text = T.i18n("Sheet Name if sheet changed")
        self.sheetNameChanged.checked = self.config.sheetNameOnSheetChanged
        self.sheetNameChanged.connect("stateChanged(int)", self.optionChanged)

        mykspread.__del__ = self.dialog.delayedDestruct
        self.dialog.show()

    def cellNameBtnClicked(self, *args):
        self.mykspread.speakCellName()

    def cellValueBtnClicked(self, *args):
        self.mykspread.speakCellValue()

    def sheetNameBtnClicked(self, *args):
        self.mykspread.speakSheetName()

    def optionChanged(self, *args):
        self.config.cellNameOnSelectionChanged = self.cellNameCheckbox.checked
        self.config.cellValueOnSelectionChanged = self.cellValueCheckbox.checked
        self.config.sheetNameOnSheetChanged = self.sheetNameChanged.checked

def start(action, myconfig, myorca):
    while True:
        try:
            myorca.speak(T.i18n("Calligra Spreadsheet"))
            break
        except IOError:
            forms = Kross.module("forms")
            dialog = forms.createDialog(T.i18n("Orca Screen Reader"))
            dialog.minimumWidth = 400
            dialog.minimumHeight = 40
            dialog.setButtons("Ok|Cancel")
            page = dialog.addPage("","")
            label = forms.createWidget(page, "QLabel")
            label.text = T.i18n("Failed to connect with the Orca HTTP-Server.")
            widget = forms.createWidget(page, "QWidget")
            layout = forms.createLayout(widget, "QHBoxLayout")
            forms.createWidget(widget, "QLabel").text = T.i18n("Url:")
            urlEdit = forms.createWidget(widget, "QLineEdit")
            urlEdit.text = myconfig.url
            if not dialog.exec_loop():
                return
            myconfig.url = urlEdit.text

    mykspread = MyKSpread(action, myorca)
    setattr(__main__,"_KSpreadOrca",mykspread)
    MyDialog(self, mykspread)

myconfig = MyConfig()
myorca = MyOrca(myconfig)
start(self, myconfig, myorca)
