#!/usr/bin/env python

import Kross
#import KWord

class Reader:
    """ The Reader class provides us implementations for the different file formats
    to read the variable value from. """

    class TextFile:
        """ Read the value of a variable from a text file. """
        filtername = "Text Files"
        filtermask = "*.txt"

    class HtmlFile:
        """ Read the value of a variable from a html file. """
        filtername = "Html Files"
        filtermask = "*.htm *.html"

def callbackGetValue(*args):
    """ This function got called by KWord if getting the variable is requested.
    The callback is mapped in the scripts.rc configurationfile. """
    print "=====> callbackGetValue args=%s" % args
    try:
        global __main__
        if hasattr(__main__,"_variableReadFile_"):
            file = __main__._variableReadFile_
            print "read file: %s" % file
            return file
    except:
        import traceback
        tb = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
        print tb
        return tb
    return "Value of a scripting variable"

class OptionsWidget:
    def __init__(self):
        readerClazzes = []
        global Reader
        for f in dir(Reader):
            if not f.startswith('_'):
                readerClazzes.append( getattr(Reader,f) )

        import Kross
        forms = Kross.module("forms")
        try:
            mainwidget = forms.createWidget("QWidget")
            self.widget = forms.createFileWidget(mainwidget, "kfiledialog:///kwordscriptingvariablereadfile")
            self.widget.setMode("Opening")
            self.widget.minimumWidth = 540
            self.widget.minimumHeight = 400

            filters = []
            for f in readerClazzes:
                filters.append("%s|%s" % (f.filtermask,f.filtername))
            if len(readerClazzes) > 1:
                filters.insert(0, "%s|All Supported Files" % " ".join([f.filtermask for f in readerClazzes]))
            filters.append("*|All Files")
            self.widget.setFilter("\n".join(filters))

            print "1 ################################################################"

            #def callbackFileSelected(args):
                #print "==================> callbackFileSelected %s" % args
            #if not self.widget.connect("fileSelected(QString)", callbackFileSelected):
                #raise "Failed to connect with filewidget \"fileSelected(QString)\" signal"

            def callbackFileHighlighted(*args):
                print "callbackFileHighlighted %s" % args
                file = args[0]
                print "Selected file: %s" % file
                global __main__
                __main__._variableReadFile_ = file
            if not self.widget.connect("fileHighlighted(QString)", callbackFileHighlighted):
                raise "Failed to connect with filewidget \"fileHighlighted(QString)\" signal"

            #def callbackSelectionChanged(args):
                #print "==================> callbackSelectionChanged %s" % args
            #if not self.widget.connect("selectionChanged()", callbackSelectionChanged):
                #raise "Failed to connect with filewidget \"selectionChanged()\" signal"

            #def callbackFilterChanged(args):
                #print "==================> callbackFilterChanged %s" % args
            #if not self.widget.connect("filterChanged(QString)", callbackFilterChanged):
                #raise "Failed to connect with filewidget \"filterChanged(QString)\" signal"

            print "2 ################################################################"

        except:
            import traceback
            tb = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
            forms.showMessageBox("Error","Error",tb)
            raise tb

    def __del__(self):
        print "--------------------------------------------------> OptionsWidget.__del__()"

def callbackCreateOptionsWidget(*args):
    """ This function got called by KWord if creating an options widget is requested.
    The callback is mapped in the scripts.rc configurationfile. """
    print "=====> callbackCreateOptionsWidget args=%s" % args

    try:
        global OptionsWidget, __main__
        optionswidget = OptionsWidget()
        __main__._variableReadFileOptionsWidget_ = optionswidget
        return optionswidget.widget
    except:
        import traceback
        tb = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
        print tb
        raise tb
