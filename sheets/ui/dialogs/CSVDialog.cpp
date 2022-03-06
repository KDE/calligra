/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CSVDialog.h"

#include <KoFileDialog.h>

#include <QApplication>
#include <QByteArray>
#include <QMimeData>
#include <QString>
#include <QTimer>


#include <KoCanvasBase.h>

#include <KLocalizedString>
#include <kmessagebox.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Damages.h"
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"

#include "commands/CSVDataCommand.h"

using namespace Calligra::Sheets;

CSVDialog::CSVDialog(QWidget* parent, Selection* selection, Mode mode)
        : KoCsvImportDialog(parent),
        m_selection(selection),
        m_canceled(false),
        m_mode(mode)
{
    // Limit the range
    int column = m_selection->lastRange().left();
    Cell lastCell = m_selection->activeSheet()->cellStorage()->lastInColumn(column);
    if (!lastCell.isNull())
        if (m_selection->lastRange().bottom() > lastCell.row())
            m_selection->lastRange().setBottom(lastCell.row());

    if (m_mode == Clipboard) {
        setWindowTitle(i18n("Inserting From Clipboard"));
        const QMimeData* mime = QApplication::clipboard()->mimeData();
        if (!mime) {
            KMessageBox::information(this, i18n("There is no data in the clipboard."));
            m_canceled = true;
        }
        if (!mime->hasText()) {
            KMessageBox::information(this, i18n("There is no usable data in the clipboard."));
            m_canceled = true;
        }
    } else if (m_mode == File) {
        //setWindowTitle(i18n("Inserting Text File"));
        KoFileDialog dialog(parent, KoFileDialog::ImportFile, "OpenDocument");
        dialog.setCaption(i18n("Import CSV Data File"));
        dialog.setNameFilter(i18n("CSV data files (*.csv)"));
        m_filename = dialog.filename();
        //cancel action !
        if (m_filename.isEmpty()) {
            enableButton(Ok, false);
            m_canceled = true;
        }
    } else { // if ( m_mode == Column )
        setWindowTitle(i18n("Text to Columns"));
        setDataWidgetEnabled(false);
    }

    if (! m_canceled)
        QTimer::singleShot(0, this, &CSVDialog::init);
}

CSVDialog::~CSVDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void CSVDialog::init()
{
    if (m_canceled)
        return;

    if (m_mode == Clipboard) {
        const QMimeData* mime = QApplication::clipboard()->mimeData();
        Q_ASSERT(mime);
        Q_ASSERT(mime->hasText());
        setData(QByteArray(mime->text().toUtf8()));
    } else if (m_mode == File) {
        Q_ASSERT(! m_filename.isEmpty());
        QFile in(m_filename);
        if (!in.open(QIODevice::ReadOnly)) {
            KMessageBox::sorry(this, i18n("Cannot open input file."));
            in.close();
            enableButton(Ok, false);
            m_canceled = true;
            return;
        }
        setData(in.readAll());
        in.close();
    } else { // if ( m_mode == Column )
        setData(QByteArray());
        Cell cell;
        Sheet * sheet = m_selection->activeSheet();
        QByteArray data;
        int col = m_selection->lastRange().left();
        for (int i = m_selection->lastRange().top(); i <= m_selection->lastRange().bottom(); ++i) {
            cell = Cell(sheet, col, i);
            if (!cell.isEmpty()) {
                data.append(cell.displayText().toUtf8() /* FIXME */);
            }
            data.append('\n');
        }
        setData(data);
    }
}

bool CSVDialog::canceled()
{
    return m_canceled;
}

void CSVDialog::accept()
{
    Sheet * sheet  = m_selection->activeSheet();

    int numRows = rows();
    int numCols = cols();

    if ((numRows == 0) || (numCols == 0))
        return;  // nothing to do here

    QRect range = m_selection->lastRange();
    if ((numCols > range.width()) && (range.width() > 1)) {
        numCols = range.width();
    } else
        range.setRight(range.left() + numCols - 1);

    if ((numRows > range.height()) && (range.height() > 1))
        numRows = range.height();
    else
        range.setBottom(range.top() + numRows - 1);

    QList<KoCsvImportDialog::DataType> dataTypes;
    Value value(Value::Array);
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            value.setElement(col, row, Value(text(row, col)));
            if (row == 0)
                dataTypes.insert(col, dataType(col));
        }
    }

    CSVDataCommand* command = new CSVDataCommand();
    if (m_mode == Clipboard)
        command->setText(kundo2_i18n("Inserting From Clipboard"));
    else if (m_mode == File)
        command->setText(kundo2_i18n("Inserting Text File"));
    else
        command->setText(kundo2_i18n("Text to Columns"));
    command->setSheet(sheet);
    command->setValue(value);
    command->setColumnDataTypes(dataTypes);
    command->setDecimalSymbol(decimalSymbol());
    command->setThousandsSeparator(thousandsSeparator());

    const QMimeData* mimedata = QApplication::clipboard()->mimeData();
    if (m_mode == Clipboard &&
        !mimedata->hasFormat("application/x-kspread-snippet") &&
        !mimedata->hasHtml() && mimedata->hasText() &&
        mimedata->text().split('\n').count() >= 2 )
    {
        range.setSize(QSize(numCols, numRows));
    }
    command->add(range);
    if (!command->execute(m_selection->canvas()))
        delete command;

    m_selection->initialize(range, sheet);
    m_selection->emitModified();
    KoCsvImportDialog::accept();
}

