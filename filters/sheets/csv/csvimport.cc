/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "csvimport.h"

#include <QFile>
#include <QGuiApplication>

#include <KMessageBox>
#include <KPluginFactory>

#include <KoCsvImportDialog.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>

#include <sheets/engine/ElapsedTime_p.h>
#include <sheets/engine/CalculationSettings.h>
#include <sheets/engine/Localization.h>
#include <sheets/engine/Value.h>
#include <sheets/engine/ValueConverter.h>
#include <sheets/core/Cell.h>
#include <sheets/core/ColFormatStorage.h>
#include <sheets/core/DocBase.h>
#include <sheets/core/Map.h>
#include <sheets/core/Sheet.h>

using namespace Calligra::Sheets;

/*
 To generate a test CSV file:

 perl -e '$i=0;while($i<30000) { print rand().",".rand()."\n"; $i++ }' > file.csv
*/

K_PLUGIN_FACTORY_WITH_JSON(CSVImportFactory, "calligra_filter_csv2sheets.json", registerPlugin<CSVFilter>();)

Q_LOGGING_CATEGORY(lcCsvImport, "calligra.filter.csv.import")

CSVFilter::CSVFilter(QObject* parent, const QVariantList&) :
        KoFilter(parent)
{
}

KoFilter::ConversionStatus CSVFilter::convert(const QByteArray& from, const QByteArray& to)
{
    QString file(m_chain->inputFile());
    KoDocument* document = m_chain->outputDocument();

    if (!document)
        return KoFilter::StupidError;

    if (!qobject_cast<const Calligra::Sheets::DocBase *>(document)) {
        qWarning(lcCsvImport) << "document isn't a Calligra::Sheets::Doc but a " << document->metaObject()->className();
        return KoFilter::NotImplemented;
    }
    if ((from != "text/csv" && from != "text/tab-separated-values" && from != "text/plain") || to != "application/vnd.oasis.opendocument.spreadsheet") {
        qWarning(lcCsvImport) << "Invalid mimetypes " << from << " " << to;
        return KoFilter::NotImplemented;
    }

    DocBase *ksdoc = dynamic_cast<DocBase *>(document);   // type checked above

//    if (ksdoc->mimeType() != "application/vnd.oasis.opendocument.spreadsheet") {
//       qWarning(lcCsvImport) << "Invalid document mimetype " << ksdoc->mimeType();
//        return KoFilter::NotImplemented;
//    }

    QFile in(file);
    if (!in.open(QIODevice::ReadOnly)) {
        KMessageBox::error(nullptr, i18n("CSV filter cannot open input file - please report."));
        in.close();
        return KoFilter::FileNotFound;
    }

    // ###### FIXME: disabled for now
    //QString csv_delimiter;
    //if (!config.isNull())
    //    csv_delimiter = config[0];

    QByteArray inputFile(in.readAll());
    in.close();

    Localization *locale = ksdoc->map()->calculationSettings()->locale();
    ValueConverter *conv = ksdoc->map()->converter();

    QString decimal = locale->decimalSymbol();
    QString thousands = locale->thousandsSeparator();

    KoCsvImportDialog* dialog = new KoCsvImportDialog(nullptr);
    dialog->setData(inputFile);
    dialog->setDecimalSymbol(decimal);
    dialog->setThousandsSeparator(thousands);
    if (!m_chain->manager()->getBatchMode() && !dialog->exec())
        return KoFilter::UserCancelled;
    inputFile.resize(0);   // Release memory (input file content)

    ElapsedTime t("Filling data into document");

    Sheet *sheet = dynamic_cast<Sheet *>(ksdoc->map()->addNewSheet());

    int numRows = dialog->rows();
    int numCols = dialog->cols();

    if (numRows == 0)
        ++numRows;

    // Initialize the decimal symbol and thousands separator to use for parsing.
    decimal = dialog->decimalSymbol();
    thousands = dialog->thousandsSeparator();

    int value = 0;

    Q_EMIT sigProgress(value);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    Cell cell(sheet, 1, 1);

    int processed = 0;
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            processed++;
            int progress = (int) (100.0 * processed / (numRows * numCols));
            if (progress != value) {
                value = progress;
                Q_EMIT sigProgress(value);
            }

            const QString text(dialog->text(row, col));

            cell = Cell(sheet, col + 1, row + 1);

            // TODO - try to format numbers

            switch (dialog->dataType(col)) {
            case KoCsvImportDialog::Generic:
            default: {
                Value val;
                // Is this a valid number?
                QString numtext = text;
                numtext = numtext.replace(' ', QString());
                numtext = numtext.replace(thousands, QString());
                numtext = numtext.replace(decimal, ".");
                bool ok = false;
                double num = numtext.toDouble(&ok);
                if (ok) val = Value(num);

                if (!val.isEmpty())
                    cell.setCellValue(val);
                else
                    cell.parseUserInput(text);
                break;
            }
            case KoCsvImportDialog::Text: {
                Value value(text);
                cell.setCellValue(value);
                break;
            }
            case KoCsvImportDialog::Date: {
                Value value(text);
                cell.setCellValue(conv->asDate(value));
                break;
            }
            case KoCsvImportDialog::Currency: {
                Value value(text);
                value = conv->asNumeric(value);
                value.setFormat(Value::fmt_Money);
                cell.setCellValue(value);
                break;
            }
            case KoCsvImportDialog::None: {
                // just skip the content
                break;
            }
            }
        }
    }

    Q_EMIT sigProgress(100);
    QGuiApplication::restoreOverrideCursor();
    delete dialog;

    return KoFilter::OK;
}

#include <csvimport.moc>
