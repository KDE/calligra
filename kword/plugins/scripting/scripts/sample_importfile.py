#!/usr/bin/env python

import Kross
import KWord

class Reader:
    """ The Reader class provides us implementations for the different file formats
    to read from. """

    class TextFile:
        filtername = "Text Files"
        filtermask = "*.txt"
        def __init__(self, file):
            import KWord
            doc = KWord.mainFrameSet().document()

            f = open(file, "r")
            doc.setText( ''.join(f.readlines()) )
            f.close()

    class HtmlFile:
        filtername = "Html Files"
        filtermask = "*.htm *.html"
        def __init__(self, file):
            import KWord
            doc = KWord.mainFrameSet().document()

            #cursor = doc.rootFrame().lastCursorPosition()
            #cursor = doc.lastCursor()
            #cursor.insertDefaultBlock()

            f = open(file, "r")
            #cursor.insertHtml( ' '.join(f.readlines()) )
            doc.setHtml( ' '.join(f.readlines()) )
            f.close()

class ImportFile:
    def __init__(self, scriptaction):
        readerClazzes = []
        global Reader
        for f in dir(Reader):
            if not f.startswith('_'):
                readerClazzes.append( getattr(Reader,f) )

        import Kross
        forms = Kross.module("forms")
        self.dialog = forms.createDialog("Import File")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed

        try:
            openpage = self.dialog.addPage("Open","Import File","fileopen")
            openwidget = forms.createFileWidget(openpage, "kfiledialog:///kwordsampleimportfile")
            openwidget.setMode("Opening")
            #openwidget.minimumWidth = 540
            #openwidget.minimumHeight = 400

            filters = []
            for f in readerClazzes:
                filters.append("%s|%s" % (f.filtermask,f.filtername))
            if len(readerClazzes) > 1:
                filters.insert(0, "%s|All Supported Files" % " ".join([f.filtermask for f in readerClazzes]))
            filters.append("*|All Files")
            openwidget.setFilter("\n".join(filters))

            if self.dialog.exec_loop():
                file = openwidget.selectedFile()
                if not file:
                    raise "No file selected"
                #print openwidget.currentMimeFilter().lower()

                def getReaderClazz():
                    for f in readerClazzes:
                        for m in f.filtermask.split(' '):
                            try:
                                if file.lower().endswith( m[m.rindex('.'):].lower().strip() ):
                                    return f
                            except ValueError:
                                pass
                    return None

                readerClazz = getReaderClazz()
                if not readerClazz:
                    raise "No reader for file \"%s\"" % file

                readerClazz(file)

        except e:
            import traceback
            #list = traceback.format_tb(sys.exc_info()[2], None)
            #s = traceback.format_exception_only(sys.exc_info()[0], sys.exc_info()[1])
            tb = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
            forms.showMessageBox("Error","Error","%s" % e,tb)

    def __del__(self):
        self.dialog.delayedDestruct()

ImportFile(self)
