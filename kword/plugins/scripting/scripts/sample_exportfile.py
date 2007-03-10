#!/usr/bin/env python

import Kross
import KWord

class Writer:
    """ The Writer class provides us implementations for the different file formats
    to write to. """

    class TextFile:
        filtername = "Text Files"
        filtermask = "*.txt"
        def __init__(self, file):
            import KWord
            doc = KWord.mainFrameSet().document()

            f = open(file, "w")
            f.write( doc.toText() )
            f.close()

    class HtmlFile:
        filtername = "Html Files"
        filtermask = "*.htm *.html"
        def __init__(self, file):
            import KWord
            doc = KWord.mainFrameSet().document()

            f = open(file, "w")
            f.write( doc.toHtml() )
            f.close()

class ExportFile:
    def __init__(self, scriptaction):
        writerClazzes = []
        global Writer
        for f in dir(Writer):
            if not f.startswith('_'):
                writerClazzes.append( getattr(Writer,f) )

        import Kross
        forms = Kross.module("forms")
        self.dialog = forms.createDialog("Export File")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed

        try:
            savepage = self.dialog.addPage("Save","Export File","filesave")
            savewidget = forms.createFileWidget(savepage, "kfiledialog:///kwordsampleexportfile")
            savewidget.setMode("Saving")
            #savewidget.minimumWidth = 540
            #savewidget.minimumHeight = 400

            filters = []
            for f in writerClazzes:
                filters.append("%s|%s" % (f.filtermask,f.filtername))
            if len(writerClazzes) > 1:
                filters.insert(0, "%s|All Supported Files" % " ".join([f.filtermask for f in writerClazzes]))
            filters.append("*|All Files")
            savewidget.setFilter("\n".join(filters))

            if self.dialog.exec_loop():
                file = savewidget.selectedFile()
                if not file:
                    raise "No file selected"
                #print savewidget.currentMimeFilter().lower()

                def getWriterClazz():
                    for f in writerClazzes:
                        for m in f.filtermask.split(' '):
                            try:
                                if file.lower().endswith( m[m.rindex('.'):].lower().strip() ):
                                    return f
                            except ValueError:
                                pass
                    return None

                writerClazz = getWriterClazz()
                if not writerClazz:
                    raise "No writer for file \"%s\"" % file

                writerClazz(file)

        except:
            import traceback
            tb = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
            forms.showMessageBox("Error","Error",tb)

    def __del__(self):
        self.dialog.delayedDestruct()

ExportFile(self)
