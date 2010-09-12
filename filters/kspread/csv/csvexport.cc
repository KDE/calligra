/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <csvexport.h>

#include <QFile>
#include <QTextCodec>
//Added by qt3to4:
#include <QTextStream>
#include <QByteArray>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>

#include <kspread/CellStorage.h>
#include <kspread/Map.h>
#include <kspread/Sheet.h>
#include <kspread/part/Doc.h>
#include <kspread/Value.h>
#include <kspread/part/View.h>
#include <kspread/ui/Selection.h>

#include <csvexportdialog.h>

using namespace KSpread;

typedef KGenericFactory<CSVExport> CSVExportFactory;
K_EXPORT_COMPONENT_FACTORY(libcsvexport, CSVExportFactory("kofficefilters"))

class Cell
{
public:
    int row, col;
    QString text;

    bool operator < (const Cell & c) const {
        return row < c.row || (row == c.row && col < c.col);
    }
    bool operator == (const Cell & c) const {
        return row == c.row && col == c.col;
    }
};


CSVExport::CSVExport(QObject* parent, const QStringList &)
        : KoFilter(parent), m_eol("\n")
{
}

QString CSVExport::exportCSVCell(const KSpread::Doc* doc, Sheet const * const sheet,
                                 int col, int row, QChar const & textQuote, QChar csvDelimiter)
{
    // This function, given a cell, returns a string corresponding to its export in CSV format
    // It proceeds by:
    //  - getting the value of the cell, if any
    //  - protecting quote characters within cells, if any
    //  - enclosing the cell in quotes if the cell is non empty

    Q_UNUSED(doc);
    const KSpread::Cell cell(sheet, col, row);
    QString text;

    if (!cell.isDefault() && !cell.isEmpty()) {
        if (cell.isFormula())
            text = cell.displayText();
        else if (!cell.link().isEmpty())
            text = cell.userInput(); // untested
        else if (cell.isTime())
            text = cell.value().asTime(sheet->map()->calculationSettings()).toString("hh:mm:ss");
        else if (cell.isDate())
            text = cell.value().asDate(sheet->map()->calculationSettings()).toString("yyyy-MM-dd");
        else
            text = cell.displayText();
    }

    // quote only when needed (try to mimic excel)
    bool quote = false;
    if (!text.isEmpty()) {
        if (text.indexOf(textQuote) != -1) {
            QString doubleTextQuote(textQuote);
            doubleTextQuote.append(textQuote);
            text.replace(textQuote, doubleTextQuote);
            quote = true;

        } else if (text[0].isSpace() || text[text.length()-1].isSpace())
            quote = true;
        else if (text.indexOf(csvDelimiter) != -1)
            quote = true;
    }

    if (quote) {
        text.prepend(textQuote);
        text.append(textQuote);
    }

    return text;
}

// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
KoFilter::ConversionStatus CSVExport::convert(const QByteArray & from, const QByteArray & to)
{
    kDebug(30501) << "CSVExport::convert";
    KoDocument* document = m_chain->inputDocument();

    if (!document)
        return KoFilter::StupidError;

    if (!qobject_cast<const KSpread::Doc *>(document)) {
        kWarning(30501) << "document isn't a KSpread::Doc but a " << document->metaObject()->className();
        return KoFilter::NotImplemented;
    }
    if ((to != "text/csv" && to != "text/plain") || from != "application/x-kspread") {
        kWarning(30501) << "Invalid mimetypes " << to << " " << from;
        return KoFilter::NotImplemented;
    }

    Doc const * const ksdoc = static_cast<const Doc *>(document);

    if (ksdoc->mimeType() != "application/x-kspread") {
        kWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType();
        return KoFilter::NotImplemented;
    }

    CSVExportDialog *expDialog = 0;
    if (!m_chain->manager()->getBatchMode()) {
        expDialog = new CSVExportDialog(0);

        if (!expDialog) {
            kError(30501) << "Dialog has not been created! Aborting!" << endl;
            return KoFilter::StupidError;
        }
        expDialog->fillSheet(ksdoc->map());

        if (!expDialog->exec()) {
            delete expDialog;
            return KoFilter::UserCancelled;
        }
    }

    QTextCodec* codec = 0;
    QChar csvDelimiter;
    if (expDialog) {
        codec = expDialog->getCodec();
        if (!codec) {
            delete expDialog;
            return KoFilter::StupidError;
        }
        csvDelimiter = expDialog->getDelimiter();
        m_eol = expDialog->getEndOfLine();
    } else {
        codec = QTextCodec::codecForName("UTF-8");
        csvDelimiter = ',';
    }


    // Now get hold of the sheet to export
    // (Hey, this could be part of the dialog too, choosing which sheet to export....
    //  It's great to have parametrable filters... IIRC even MSOffice doesn't have that)
    // Ok, for now we'll use the first sheet - my document has only one sheet anyway ;-)))

    bool first = true;
    QString str;
    QChar textQuote;
    if (expDialog)
        textQuote = expDialog->getTextQuote();
    else
        textQuote = '"';

    if (expDialog && expDialog->exportSelectionOnly()) {
        kDebug(30501) << "Export as selection mode";
        View const * const view = ksdoc->views().isEmpty() ? 0 : static_cast<View*>(ksdoc->views().first());

        if (!view) { // no view if embedded document
            delete expDialog;
            return KoFilter::StupidError;
        }

        Sheet const * const sheet = view->activeSheet();

        QRect selection = view->selection()->lastRange();
        // Compute the highest row and column indexes (within the selection)
        // containing non-empty cells, respectively called CSVMaxRow CSVMaxCol.
        // The CSV will have CSVMaxRow rows, all with CSVMaxCol columns
        int right       = selection.right();
        int bottom      = selection.bottom();
        int CSVMaxRow   = 0;
        int CSVMaxCol   = 0;

        for (int idxRow = 1, row = selection.top(); row <= bottom; ++row, ++idxRow) {
            for (int idxCol = 1, col = selection.left(); col <= right; ++col, ++idxCol) {
                if (!KSpread::Cell(sheet, col, row).isEmpty()) {
                    if (idxRow > CSVMaxRow)
                        CSVMaxRow = idxRow;

                    if (idxCol > CSVMaxCol)
                        CSVMaxCol = idxCol;
                }
            }
        }

        for (int idxRow = 1, row = selection.top();
                row <= bottom && idxRow <= CSVMaxRow; ++row, ++idxRow) {
            int idxCol = 1;
            for (int col = selection.left();
                    col <= right && idxCol <= CSVMaxCol; ++col, ++idxCol) {
                str += exportCSVCell(ksdoc, sheet, col, row, textQuote, csvDelimiter);

                if (idxCol < CSVMaxCol)
                    str += csvDelimiter;
            }

            // This is to deal with the case of non-rectangular selections
            for (; idxCol < CSVMaxCol; ++idxCol)
                str += csvDelimiter;

            str += m_eol;
        }
    } else {
        kDebug(30501) << "Export as full mode";
        foreach(Sheet const * const sheet, ksdoc->map()->sheetList()) {
            if (expDialog && !expDialog->exportSheet(sheet->sheetName())) {
                continue;
            }

            // Compute the highest row and column indexes containing non-empty cells,
            // respectively called CSVMaxRow CSVMaxCol.
            // The CSV will have CSVMaxRow rows, all with CSVMaxCol columns
            int sheetMaxRow = sheet->cellStorage()->rows();
            int sheetMaxCol = sheet->cellStorage()->columns();
            int CSVMaxRow   = 0;
            int CSVMaxCol   = 0;

            for (int row = 1 ; row <= sheetMaxRow ; ++row) {
                for (int col = 1 ; col <= sheetMaxCol ; col++) {
                    if (!KSpread::Cell(sheet, col, row).isEmpty()) {
                        if (row > CSVMaxRow)
                            CSVMaxRow = row;

                        if (col > CSVMaxCol)
                            CSVMaxCol = col;
                    }
                }
            }

            // Skip the sheet altogether if it is empty
            if (CSVMaxRow + CSVMaxCol == 0)
                continue;

            kDebug(30501) << "Max row x column:" << CSVMaxRow << " x" << CSVMaxCol;

            // Print sheet separators, except for the first sheet
            if (!first || (expDialog && expDialog->printAlwaysSheetDelimiter())) {
                if (!first)
                    str += m_eol;

                QString name;
                if (expDialog)
                    name = expDialog->getSheetDelimiter();
                else
                    name = "********<SHEETNAME>********";
                const QString tname(i18n("<SHEETNAME>"));
                int pos = name.indexOf(tname);
                if (pos != -1) {
                    name.replace(pos, tname.length(), sheet->sheetName());
                }
                str += name;
                str += m_eol;
                str += m_eol;
            }

            first = false;


            // this is just a bad approximation which fails for documents with less than 50 rows, but
            // we don't need any progress stuff there anyway :) (Werner)
            int value = 0;
            int step  = CSVMaxRow > 50 ? CSVMaxRow / 50 : 1;

            // Print the CSV for the sheet data
            for (int row = 1, i = 1 ; row <= CSVMaxRow ; ++row, ++i) {
                if (i > step) {
                    value += 2;
                    emit sigProgress(value);
                    i = 0;
                }

                QString collect;  // buffer delimiters while reading empty cells

                for (int col = 1 ; col <= CSVMaxCol ; col++) {
                    const QString txt = exportCSVCell(ksdoc, sheet, col, row, textQuote, csvDelimiter);

                    // if we encounter a non-empty cell, commit the buffered delimiters
                    if (!txt.isEmpty()) {
                        str += collect + txt;
                        collect.clear();
                    }

                    collect += csvDelimiter;
                }
                // Here, throw away buffered delimiters. They're trailing and therefore
                // superfluous.

                str += m_eol;
            }
        }
    }

    emit sigProgress(100);

    QFile out(m_chain->outputFile());
    if (!out.open(QIODevice::WriteOnly)) {
        kError(30501) << "Unable to open output file!" << endl;
        out.close();
        delete expDialog;
        return KoFilter::StupidError;
    }

    QTextStream outStream(&out);
    outStream.setCodec(codec);

    outStream << str;

    out.close();
    delete expDialog;
    return KoFilter::OK;
}

#include <csvexport.moc>
