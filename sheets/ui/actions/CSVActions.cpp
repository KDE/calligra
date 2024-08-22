/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CSVActions.h"
#include "Actions.h"

#include "ui/actions/dialogs/CSVDialog.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "engine/MapBase.h"
#include "engine/ValueConverter.h"

#include <KoFileDialog.h>

#include <KLocalizedString>
#include <KMessageBox>

#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QMimeData>

// These three actions have a fairly similary structured execute() routinhjes, and could probably have a shared ancestor with that structure.
// For now opting not to do that, as the common code isn't that long, butu it's something to consider if more functionality gets added here.

using namespace Calligra::Sheets;

InsertFromFile::InsertFromFile(Actions *actions)
    : CellAction(actions,
                 "insertFromTextfile",
                 i18n("Insert From &Text File..."),
                 koIcon("text-plain"),
                 i18n("Insert data from a text file to the current cursor position/selection"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

InsertFromFile::~InsertFromFile()
{
    if (m_dlg)
        delete m_dlg;
}

QAction *InsertFromFile::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Text File"));
    return res;
}

void InsertFromFile::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    KoFileDialog dialog(canvasWidget, KoFileDialog::ImportFile, "OpenDocument");
    dialog.setCaption(i18n("Import CSV Data File"));
    dialog.setNameFilter(i18n("CSV data files (*.csv)"));
    QString filename = dialog.filename();
    if (filename.isEmpty())
        return;
    QFile in(filename);
    if (!in.open(QIODevice::ReadOnly)) {
        KMessageBox::error(canvasWidget, i18n("Cannot open input file."));
        in.close();
        return;
    }
    QByteArray data = in.readAll();
    in.close();

    m_dlg = new CSVDialog(canvasWidget);
    m_dlg->setWindowTitle(i18n("Inserting Text File"));
    Localization *locale = sheet->map()->calculationSettings()->locale();
    m_dlg->setDecimalSymbol(locale->decimalSymbol());
    m_dlg->setThousandsSeparator(locale->thousandsSeparator());

    m_dlg->setData(data);

    if (m_dlg->exec()) {
        int numRows = m_dlg->rows();
        int numCols = m_dlg->cols();

        if ((numRows == 0) || (numCols == 0)) {
            delete m_dlg;
            m_dlg = nullptr;
            return; // nothing to do here
        }

        QRect range = selection->lastRange();
        if (numCols < range.width())
            range.setRight(range.left() + numCols - 1);
        if (numRows < range.height())
            range.setBottom(range.top() + numRows - 1);

        CSVDataCommand *command = new CSVDataCommand();
        command->setText(kundo2_i18n("Inserting Text File"));
        command->setSheet(sheet);
        command->setValue(m_dlg->value());
        command->setColumnDataTypes(m_dlg->dataTypes());
        command->setDecimalSymbol(locale->decimalSymbol());
        command->setThousandsSeparator(locale->thousandsSeparator());
        command->add(range);
        command->execute(selection->canvas());

        selection->initialize(range, sheet);
        selection->emitModified();
    }

    delete m_dlg;
    m_dlg = nullptr;
}

InsertFromClipboard::InsertFromClipboard(Actions *actions)
    : CellAction(actions,
                 "insertFromClipboard",
                 i18n("Insert From &Clipboard..."),
                 koIcon("edit-paste"),
                 i18n("Insert CSV data from the clipboard to the current cursor position/selection"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

InsertFromClipboard::~InsertFromClipboard()
{
    if (m_dlg)
        delete m_dlg;
}

QAction *InsertFromClipboard::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Clipboard"));
    return res;
}

void InsertFromClipboard::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    if (!mime) {
        KMessageBox::information(canvasWidget, i18n("There is no data in the clipboard."));
        return;
    }
    if (!mime->hasText()) {
        KMessageBox::information(canvasWidget, i18n("There is no usable data in the clipboard."));
        return;
    }

    m_dlg = new CSVDialog(canvasWidget);
    m_dlg->setWindowTitle(i18n("Inserting From Clipboard"));

    Localization *locale = sheet->map()->calculationSettings()->locale();
    m_dlg->setDecimalSymbol(locale->decimalSymbol());
    m_dlg->setThousandsSeparator(locale->thousandsSeparator());
    QString oldDelimiter = m_dlg->delimiter();
    m_dlg->setDelimiter(QString());

    m_dlg->setData(QByteArray(mime->text().toUtf8()));

    if (m_dlg->exec()) {
        int numRows = m_dlg->rows();
        int numCols = m_dlg->cols();

        if ((numRows == 0) || (numCols == 0)) {
            delete m_dlg;
            m_dlg = nullptr;
            return; // nothing to do here
        }

        QRect range = selection->lastRange();
        if (numCols != range.width())
            range.setRight(range.left() + numCols - 1);
        if (numRows != range.height())
            range.setBottom(range.top() + numRows - 1);

        CSVDataCommand *command = new CSVDataCommand();
        command->setText(kundo2_i18n("Inserting From Clipboard"));
        command->setSheet(sheet);
        command->setValue(m_dlg->value());
        command->setColumnDataTypes(m_dlg->dataTypes());
        command->setDecimalSymbol(locale->decimalSymbol());
        command->setThousandsSeparator(locale->thousandsSeparator());
        command->add(range);
        command->execute(selection->canvas());

        selection->initialize(range, sheet);
        selection->emitModified();
    }

    m_dlg->setDelimiter(oldDelimiter); // this is because the dialog saves its settings when destroyed, and we don't want to save the empty delim
    delete m_dlg;
    m_dlg = nullptr;
}

TextToColumns::TextToColumns(Actions *actions)
    : CellAction(actions, "textToColumns", i18n("&Text to Columns..."), QIcon(), i18n("Expand the content of cells to multiple columns"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

TextToColumns::~TextToColumns()
{
    if (m_dlg)
        delete m_dlg;
}

void TextToColumns::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    QRect area = selection->lastRange();
    area.setRight(area.left()); // only use the first column
    Region oldSelection = *selection; // store
    selection->initialize(area);

    QByteArray data;

    // Limit the range, grab the data.
    int column = area.left();
    int firstRow = area.top();
    int lastRow = area.bottom();
    Cell lastCell = sheet->fullCellStorage()->lastInColumn(column);
    if (lastCell.isNull())
        lastRow = firstRow;
    else if (lastRow > lastCell.row())
        lastRow = lastCell.row();

    for (int i = firstRow; i <= lastRow; ++i) {
        Cell cell = Cell(sheet, column, i);
        if (!cell.isEmpty()) {
            data.append(cell.displayText().toUtf8() /* FIXME */);
        }
        data.append('\n');
    }

    m_dlg = new CSVDialog(canvasWidget);
    m_dlg->setWindowTitle(i18n("Text to Columns"));
    m_dlg->setDataWidgetEnabled(false);

    Localization *locale = sheet->map()->calculationSettings()->locale();
    m_dlg->setDecimalSymbol(locale->decimalSymbol());
    m_dlg->setThousandsSeparator(locale->thousandsSeparator());

    m_dlg->setData(data);

    if (m_dlg->exec()) {
        int numRows = m_dlg->rows();
        int numCols = m_dlg->cols();

        if ((numRows == 0) || (numCols == 0)) {
            delete m_dlg;
            m_dlg = nullptr;
            return; // nothing to do here
        }

        QRect range = selection->lastRange();
        if (numCols != range.width())
            range.setRight(range.left() + numCols - 1);
        if (numRows != range.height())
            range.setBottom(range.top() + numRows - 1);

        CSVDataCommand *command = new CSVDataCommand();
        command->setText(kundo2_i18n("Text to Columns"));
        command->setSheet(sheet);
        command->setValue(m_dlg->value());
        command->setColumnDataTypes(m_dlg->dataTypes());
        command->setDecimalSymbol(locale->decimalSymbol());
        command->setThousandsSeparator(locale->thousandsSeparator());

        const QMimeData *mimedata = QApplication::clipboard()->mimeData();
        if (!mimedata->hasFormat("application/x-calligra-sheets-snippet") && !mimedata->hasHtml() && mimedata->hasText()
            && mimedata->text().split('\n').count() >= 2) {
            range.setSize(QSize(numCols, numRows));
        }
        command->add(range);
        command->execute(selection->canvas());

        selection->initialize(range, sheet);
        selection->emitModified();
    } else
        selection->initialize(oldSelection);

    delete m_dlg;
    m_dlg = nullptr;
}

bool TextToColumns::enabledForSelection(Selection *selection, const Cell &)
{
    if (selection->isRowSelected())
        return false;
    return true;
}

CSVDataCommand::CSVDataCommand()
    : AbstractDataManipulator()
{
}

CSVDataCommand::~CSVDataCommand() = default;

void CSVDataCommand::setValue(const Value &value)
{
    m_value = value;
}

void CSVDataCommand::setColumnDataTypes(const QList<KoCsvImportDialog::DataType> &dataTypes)
{
    m_dataTypes = dataTypes;
}

void CSVDataCommand::setDecimalSymbol(const QString &symbol)
{
    m_decimalSymbol = symbol;
}

void CSVDataCommand::setThousandsSeparator(const QString &separator)
{
    m_thousandsSeparator = separator;
}

Value CSVDataCommand::newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType)
{
    Q_UNUSED(fmtType)
    const int colidx = col - element->rect().left();
    const int rowidx = row - element->rect().top();

    Value value = m_value.element(colidx, rowidx);
    switch (m_dataTypes.value(colidx)) {
    case KoCsvImportDialog::Generic:
        *parse = true;
        break;
    case KoCsvImportDialog::Text:
        break;
    case KoCsvImportDialog::Date:
        value = m_sheet->map()->converter()->asDate(value);
        break;
    case KoCsvImportDialog::Currency:
        value = m_sheet->map()->converter()->asFloat(value);
        value.setFormat(Value::fmt_Money);
        break;
    case KoCsvImportDialog::None:
        break;
    }
    return value;
}

bool CSVDataCommand::wantChange(Element *element, int col, int row)
{
    Q_UNUSED(row)
    return (m_dataTypes.value(col - element->rect().left()) != KoCsvImportDialog::None);
}
