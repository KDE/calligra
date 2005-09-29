""" 
Python script for a GUI-dialog.

Description:
Python script to provide an abstract GUI for other python scripts. That
way we've all the GUI-related code within one single file and are
able to easily modify GUI-stuff in a central place.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Published as-is without any warranties.
"""

def getHome():
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

class TkDialog:
    """ This class is used to wrap Tkinter into a more abstract interface."""

    def __init__(self, title):
        import Tkinter
        self.root = Tkinter.Tk()
        self.root.title(title)
        self.root.deiconify()

        mainframe = self.Frame(self, self.root)
        self.widget = mainframe.widget

    class Widget:
        def __init__(self, dialog, parent):
            self.dialog = dialog
            self.parent = parent
        #def setVisible(self, visibled): pass
        #def setEnabled(self, enabled): pass

    class Frame(Widget):
        def __init__(self, dialog, parent):
            #TkDialog.Widget.__init__(self, dialog, parent)
            import Tkinter
            self.widget = Tkinter.Frame(parent)
            self.widget.pack()

    class Label(Widget):
        def __init__(self, dialog, parent, caption):
            #TkDialog.Widget.__init__(self, dialog, parent)
            import Tkinter
            self.widget = Tkinter.Label(parent, text=caption)
            self.widget.pack(side=Tkinter.LEFT)

    class Checkbox(Widget):
        def __init__(self, dialog, parent, caption):
            #TkDialog.Widget.__init__(self, dialog, parent)
            import Tkinter
            self.widget = Tkinter.Checkbutton(parent, text=caption)
            self.widget.pack(side=Tkinter.LEFT)

    class List(Widget):
        def __init__(self, dialog, parent, caption, items):
            #TkDialog.Widget.__init__(self, dialog, parent)
            import Tkinter

            listframe = Tkinter.Frame(parent)
            listframe.pack()

            Tkinter.Label(listframe, text=caption).pack(side=Tkinter.LEFT)

            self.items = Tkinter.StringVar()
            itemlist = apply(Tkinter.OptionMenu, (listframe, self.items) + tuple( items ))
            itemlist.pack(side=Tkinter.LEFT)

    class Button(Widget):
        def __init__(self, dialog, parent, caption, commandmethod):
            #TkDialog.Widget.__init__(self, dialog, parent)
            import Tkinter
            self.widget = Tkinter.Button(parent, text=caption, command=self.doCommand)
            self.commandmethod = commandmethod
            self.widget.pack(side=Tkinter.LEFT)
        def doCommand(self):
            try:
                self.commandmethod()
            except:
                #TODO why the heck we arn't able to redirect exceptions?
                import traceback
                import StringIO
                fp = StringIO.StringIO()
                traceback.print_exc(file=fp)
                import tkMessageBox
                tkMessageBox.showerror("Exception", fp.getvalue())
                #self.dialog.root.destroy()

    class Entry(Widget):
        def __init__(self, dialog, parent, caption):
            #TkDialog.Widget.__init__(self, dialog, parent)
            import Tkinter

            self.widget = Tkinter.Frame(parent)
            self.widget.pack()

            label = Tkinter.Label(self.widget, text=caption)
            label.pack(side=Tkinter.LEFT)

            self.entrytext = Tkinter.StringVar()
            self.entry = Tkinter.Entry(self.widget, width=36, textvariable=self.entrytext)
            self.entry.pack(side=Tkinter.LEFT)
        def get(self):
            return self.entrytext.get()

    class FileChooser(Entry):
        def __init__(self, dialog, parent, caption, initialfile = None, filetypes = None):
            TkDialog.Entry.__init__(self, dialog, parent, caption)
            import Tkinter

            self.initialfile = initialfile
            self.entrytext.set(initialfile)

            btn = Tkinter.Button(self.widget, text="...", command=self.browse)
            btn.pack(side=Tkinter.LEFT)

            if filetypes:
                self.filetypes = filetypes
            else:
                self.filetypes = (('All files', '*'),)

        def browse(self):
            import os
            text = self.entrytext.get()
            d = os.path.dirname(text) or os.path.dirname(self.initialfile)
            f = os.path.basename(text) or os.path.basename(self.initialfile)

            import tkFileDialog
            file = tkFileDialog.asksaveasfilename(
                       initialdir=d,
                       initialfile=f,
                       #defaultextension='.html',
                       filetypes=self.filetypes
            )
            if file:
                self.entrytext.set( file )

    def show(self):
        self.root.mainloop()

class Dialog:
    """ Central class that provides abstract GUI-access to the outer world. """

    def __init__(self, title):
        self.dialog = TkDialog(title)
        self.widget = self.dialog.widget

    def show(self):
        self.dialog.show()

    def quitDialog(self):
        self.dialog.root.destroy()

    def addFrame(self, parentwidget):
        return self.dialog.Frame(self.dialog, parentwidget.widget)

    def addLabel(self, parentwidget, caption):
        return self.dialog.Label(self.dialog, parentwidget.widget, caption)

    def addCheckbox(self, parentwidget, caption):
        return self.dialog.Checkbox(self.dialog, parentwidget.widget, caption)

    def addButton(self, parentwidget, caption, commandmethod):
        return self.dialog.Button(self.dialog, parentwidget.widget, caption, commandmethod)

    def addEntry(self, parentwidget, caption):
        return self.dialog.Entry(self.dialog, parentwidget.widget, caption)

    def addFileChooser(self, parentwidget, caption, initialfile = None, filetypes = None):
        return self.dialog.FileChooser(self.dialog, parentwidget.widget, caption, initialfile, filetypes)

    def addList(self, parentwidget, caption, items):
        return self.dialog.List(self.dialog, parentwidget.widget, caption, items)

