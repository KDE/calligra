#!/usr/bin/env kross

"""
Python script to export an OpenDocument Spreadsheet File to
an OpenDocument File using the ODFPY python module.

The script could be used in two ways;

    1. Embedded in KSpread by execution via the "Tools=>Scripts"
       menu or from the "Tools=>Script Manager". In that case
       the document currently loaded and displayed by KSpread
       will be exported.

    2. As standalone script by running;

            # make the script executable
            chmod 755 `kde4-config --install data`/kspread/scripts/extensions/odfpyexport.py
            # run the script
            `kde4-config --install data`/kspread/scripts/extensions/odfpyexport.py

       In that case the odfpyexport.py-script will use the with Kross
       distributed krossrunner commandline-application to execute the
       python script. In that case you need to define the readOdsFile
       variable passed at the constructor of the OdfPyExport class at
       the bottom of this script.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.koffice.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

try:
    import Kross
except:
    raise "Failed to import the Kross module."

try:
    import odf.opendocument
except:
    raise "<qt>Failed to import the ODFPY python module.<br><br>To use this odfpyexport.py python script to export to OpenDocument you need to install the <a href=\"http://opendocumentfellowship.org/projects/odfpy\">ODFPY</a> python module.</qt>"

class OdfExporter:

    class _OpenDocumentInterface_:
        def __init__(self, kspread, doc):
            self.kspread = kspread
            self.doc = doc

        def getMaxColumn(self, ranges):
            maxcolumn = 0
            for r in ranges:
                if r[2] > maxcolumn:
                    maxcolumn = r[2]
            if maxcolumn < 1:
                raise "Failed to determinate number of columns."
            #mincolumn = maxcolumn
            #for r in ranges:
            #    if r[0] < mincolumn:
            #        mincolumn = r[0]
            return maxcolumn

        def getRanges(self, sheet, ranges):
            (lastRow, lastColumn) = (sheet.lastRow(), sheet.lastColumn())
            rangeList = []
            for r in ranges:
                if len(r) < 2:
                    continue
                if len(r) < 4:
                    r = (r[:2] + (lastColumn, lastRow))
                if r[0] > r[2]:
                    raise "Invalid range for sheet \"%s\" cause left column value \"%i\" is bigger then right column value \"%i\"." % (sheet.sheetName(),r[0],r[2])
                if r[1] > r[3]:
                    raise "Invalid range for sheet \"%s\" cause top row value \"%i\" is bigger then bottom row value \"%i\"." % (sheet.sheetName(),r[1],r[3])
                rangeList.append(r)
            if len(rangeList) < 1:
                rangeList.append( (1, 1, lastColumn, lastRow) )
            return rangeList

    class OpenDocumentText(_OpenDocumentInterface_):
        filtername = "OpenDocument Text"
        filtermask = "*.odt"

        def __init__(self, kspread, exportSheets, writeOdfFile):
            import odf.opendocument, odf.style, odf.text
            OdfExporter._OpenDocumentInterface_.__init__(self, kspread, odf.opendocument.OpenDocumentText())

            # Create the heading style
            self.h1style = odf.style.Style(name="Heading 1", family="paragraph")
            self.h1style.addElement(odf.style.TextProperties(attributes={'fontsize':"24pt",'fontweight':"bold" }))
            self.doc.styles.addElement(self.h1style)

            # Create a style for the table content
            self.tablestyle = odf.style.Style(name="Table Contents", family="paragraph")
            self.tablestyle.addElement(odf.style.ParagraphProperties(numberlines="false", linenumber="0"))
            self.doc.styles.addElement(self.tablestyle)

            # Process the sheets
            for sheetlist in exportSheets:
                if sheetlist[1]: # if sheet is enabled for export
                    sheet = kspread.sheetByName( sheetlist[0] )
                    self.writeSheet(sheet, self.getRanges(sheet, sheetlist[2:]))

            # Finally write the document to a file
            self.doc.save(writeOdfFile)

        def writeSheet(self, sheet, ranges):
            import odf.text, odf.table

            # Create a header and add it to the document
            header = odf.text.H(outlinelevel=1, stylename=self.h1style, text="%s" % sheet.sheetName())
            self.doc.text.addElement(header)

            # Create the table
            table = odf.table.Table(name=sheet.sheetName())
            # Add the columns
            for i in range( self.getMaxColumn(ranges) ):
                table.addElement(odf.table.TableColumn())

            # Add the rows to the table
            for r in ranges:
                for row in range(r[1],r[3]+1):
                    tr = odf.table.TableRow()
                    table.addElement(tr)
                    for col in range(r[0],r[2]+1):
                        td = odf.table.TableCell()
                        tr.addElement(td)
                        #value = "%s" % sheet.value(row,col)
                        value = "%s" % sheet.text(col,row)
                        #print "row=%i col=%i value=%s" % (row,col,value)
                        p = odf.text.P(stylename=self.tablestyle, text=value)
                        td.addElement(p)

            # Add the table to the document
            self.doc.text.addElement(table)

    class OpenDocumentSpreadsheet(_OpenDocumentInterface_):
        filtername = "OpenDocument Spreadsheet"
        filtermask = "*.ods"

        def __init__(self, kspread, exportSheets, writeOdfFile):
            import odf.opendocument, odf.style, odf.table
            OdfExporter._OpenDocumentInterface_.__init__(self, kspread, odf.opendocument.OpenDocumentSpreadsheet())

            # Create a style for the table content
            self.tablestyle = odf.style.Style(name="Table Contents", family="paragraph")
            self.tablestyle.addElement(odf.style.ParagraphProperties(numberlines="false", linenumber="0"))
            self.doc.styles.addElement(self.tablestyle)

            # Create automatic styles for the column widths. We want two different widths, one in inches, the other one in metric. ODF Standard section 15.9.1
            #widthshort = Style(name="Wshort", family="table-column")
            #widthshort.addElement(TableColumnProperties(columnwidth="1.7cm"))
            #doc.automaticstyles.addElement(widthshort)
            #widthwide = Style(name="Wwide", family="table-column")
            #widthwide.addElement(TableColumnProperties(columnwidth="1.5in"))
            #doc.automaticstyles.addElement(widthwide)

            # Process the sheets
            for sheetlist in exportSheets:
                if sheetlist[1]: # if sheet is enabled for export
                    sheet = kspread.sheetByName( sheetlist[0] )
                    self.writeSheet(sheet, self.getRanges(sheet, sheetlist[2:]))

            # Finally write the document to a file
            self.doc.save(writeOdfFile)

        def writeSheet(self, sheet, ranges):
            import odf.text, odf.table

            # Create the table
            table = odf.table.Table(name=sheet.sheetName())
            # Add the columns
            for i in range( self.getMaxColumn(ranges) ):
                table.addElement(odf.table.TableColumn())

            # Add the rows to the table
            for r in ranges:
                for row in range(r[1],r[3]+1):
                    tr = odf.table.TableRow()
                    table.addElement(tr)
                    for col in range(r[0],r[2]+1):
                        td = odf.table.TableCell()
                        tr.addElement(td)
                        #value = "%s" % sheet.value(row,col)
                        value = "%s" % sheet.text(col,row)
                        #print "row=%i col=%i value=%s" % (row,col,value)
                        p = odf.text.P(stylename=self.tablestyle, text=value)
                        td.addElement(p)

            # Add the table to the document
            self.doc.spreadsheet.addElement(table)

    #class OpenDocumentPresentation(_OpenDocumentInterface_):
        #filtername = "OpenDocument Presentation"
        #filtermask = "*.odp"
        #def __init__(self, kspread, exportSheets, writeOdfFile):
            #import odf.opendocument, odf.style
            #_OpenDocumentInterface_.__init__(self, kspread, odf.opendocument.OpenDocumentPresentation())

class OdfPyExport:

    def __init__(self, scriptaction, readOdsFile = None, exportSheets = None, writeOdfFile = None, odfExporterClass = None):
        import os, sys, traceback, Kross

        embeddedInKSpread = False
        try:
            import KSpread
            self.kspread = KSpread
            embeddedInKSpread = True
        except ImportError:
            try:
                self.kspread = Kross.module("kspread")
            except ImportError:
                raise "Failed to import the Kross module. Please run this script with \"kross odtexport.py\""

        if readOdsFile:
            if not self.kspread.openUrl(readOdsFile):
                raise "Failed to read OpenDocument Spreadsheet file \"%s\"." % readOdsFile
        elif not embeddedInKSpread:
            raise "No OpenDocument Spreadsheet file to read from defined."

        global OdfExporter
        self.exporterClasses = []
        for d in dir(OdfExporter):
            if not d.startswith('_'):
                f = getattr(OdfExporter,d)
                if hasattr(f,"filtername") and hasattr(f,"filtermask"):
                    self.exporterClasses.append(f)

        self.scriptaction = scriptaction

        if embeddedInKSpread or not exportSheets or not writeOdfFile:
            forms = Kross.module("forms")
            dialog = forms.createDialog("OdfPy Export")
            dialog.setButtons("Ok|Cancel")
            dialog.setFaceType("List") #Auto Plain List Tree Tabbed
            try:
                if not writeOdfFile:
                    savepage = dialog.addPage("Save","Save to OpenDocument File","filesave")
                    self.savewidget = forms.createFileWidget(savepage, "kfiledialog:///kspreadodfpyexport")
                    self.savewidget.setMode("Saving")

                    filters = []
                    for f in self.exporterClasses:
                        filters.append("%s|%s" % (f.filtermask,f.filtername))
                    if len(self.exporterClasses) > 1:
                        filters.insert(0, "%s|All Supported Files" % " ".join([f.filtermask for f in self.exporterClasses]))
                    filters.append("*|All Files")
                    self.savewidget.setFilter("\n".join(filters))

                if not exportSheets:
                    datapage = dialog.addPage("Sheets","Export Sheets","spreadsheet")
                    self.sheetslistview = self.kspread.createSheetsListView(datapage)

                if not dialog.exec_loop():
                    return
                if hasattr(self,"savewidget"):
                    writeOdfFile = self.savewidget.selectedFile()
                    if not odfExporterClass:
                        mime = self.savewidget.currentMimeFilter()
                        for f in self.exporterClasses:
                            if f.filtermask == mime:
                                odfExporterClass = f
                                break
                if hasattr(self,"sheetslistview"):
                    exportSheets = self.sheetslistview.sheets()
            finally:
                dialog.delayedDestruct()

        if exportSheets == None or len(exportSheets) < 1:
            exportSheets = []
            for sheetname in self.kspread.sheetNames():
                exportSheets.append( [sheetname, True] )

        if not writeOdfFile:
            raise "No OpenDocument file to write to defined."

        if not odfExporterClass:
            if len(self.exporterClasses) == 1:
                odfExporterClass = self.exporterClasses[0]
            else:
                odfExporterClass = self.getOdfExporterClassForFile(writeOdfFile)
                if not odfExporterClass:
                    raise "Failed to determinate the OdfExporter class for the file \"%s\"." % writeOdfFile

        print "OdfExporter: %s" % odfExporterClass
        print "Read from OpenDocument File: %s" % self.kspread.document().url()
        print "Export Sheets: %s" % exportSheets
        print "Write to OpenDocument File: %s" % writeOdfFile

        odfExporterClass(self.kspread, exportSheets, writeOdfFile)

    def getOdfExporterClassForFile(self, filename):
        for f in self.exporterClasses:
            for m in f.filtermask.split(' '):
                try:
                    if filename.lower().endswith( m[m.rindex('.'):].lower().strip() ):
                        return f
                except ValueError:
                    pass
        return None

OdfPyExport(self)


#readOdsFile = "/home/kde4/kspreaddocument.ods"
#exportSheets = [ ['Sheet1', 1, [1, 1, 4, 8]], ['Sheet2', 0] ]
#writeOdfFile = "/home/kde4/testdoc2.odt"
#odfExporterClass = OdfExporter.OpenDocumentText
#OdfPyExport(self, readOdsFile, exportSheets, writeOdfFile, odfExporterClass)
