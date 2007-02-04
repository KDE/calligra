#!/usr/bin/env python

class Actions:

    def __init__(self, scriptaction):
        try: import Kross
        except: raise "Failed to import the Kross module."
        try: import KWord
        except: raise "Failed to import the KWord module."

        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("Execute Action")
        self.dialog.minimumWidth = 500
        self.dialog.minimumHeight = 360
        self.dialog.setButtons("Ok|Cancel")
        #self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        #koapplicationadaptor = KWord.application()
        #kmainWindow = KWord.mainWindow()
        #kodocumentadaptor = KWord.document()
        self.komainwindow = KWord.shell()
        self.actions = [ s for s in dir(self.komainwindow) if s.startswith('slot') ]

        page = self.dialog.addPage("", "")
        widget = self.forms.createWidgetFromUI(page,
            '<ui version="4.0" >'
            ' <class>Form</class>'
            ' <widget class="QWidget" name="Form" >'
            '  <layout class="QHBoxLayout" >'
            '   <item>'
            '    <widget class="QListWidget" name="List">'
            '     <property name="currentRow"><number>%i</number></property>'
            '     %s'
            '    </widget>'
            '   </item>'
            '  </layout>'
            ' </widget>'
            '</ui>'
            % ( 0, ''.join( [ '<item><property name="text" ><string>%s</string></property></item>' % s for s in self.actions ] ) )
        )
        self.widgetlist = widget["List"]

        if self.dialog.exec_loop():
            self.execAction()

    def __del__(self):
        self.dialog.delayedDestruct()

    def execAction(self):
        import KWord, traceback
        try:
            actionName = self.actions[ self.widgetlist.currentRow ]
            getattr(self.komainwindow,actionName)()
        except:
            message = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
            self.forms.showMessageBox("Error", "Error", "%s" % message)

Actions(self)
