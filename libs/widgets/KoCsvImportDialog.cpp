/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoCsvImportDialog.h"

// Qt
#include <QButtonGroup>
#include <QTextStream>

#include <QTableWidget>
#include <QTableWidgetSelectionRange>

// KF5
#include <KConfig>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <WidgetsDebug.h>

#include "ui_KoCsvImportDialog.h"

class KoCsvImportWidget : public QWidget, public Ui::KoCsvImportWidget
{
    Q_OBJECT
public:
    explicit KoCsvImportWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

class Q_DECL_HIDDEN KoCsvImportDialog::Private
{
public:
    KoCsvImportDialog *q;
    KoCsvImportWidget *dialog;

    bool rowsAdjusted;
    bool columnsAdjusted;
    int startRow;
    int startCol;
    int endRow;
    int endCol;
    QChar textQuote;
    QString delimiter;
    QString commentSymbol;
    bool ignoreDuplicates;
    QByteArray data;
    QStringList formatList; ///< List of the column formats

    explicit Private(KoCsvImportDialog *qq)
        : q(qq)
    {
    }
    void loadSettings();
    void saveSettings();
    void fillTable();
    void setText(int row, int col, const QString &text);
    void adjustRows(int iRows);
    void adjustCols(int iCols);
    bool checkUpdateRange();
};

KoCsvImportDialog::KoCsvImportDialog(QWidget *parent)
    : KoDialog(parent)
    , d(new Private(this))
{
    d->dialog = new KoCsvImportWidget(this);
    d->rowsAdjusted = false;
    d->columnsAdjusted = false;
    d->startRow = 0;
    d->startCol = 0;
    d->endRow = -1;
    d->endCol = -1;
    d->textQuote = QChar('"');
    d->delimiter = QString(',');
    d->commentSymbol = QString('#');
    d->ignoreDuplicates = false;

    setButtons(KoDialog::Ok | KoDialog::Cancel);
    setCaption(i18n("Import Data"));

    setDataTypes(Generic | Text | Date | None);

    // XXX:	Qt3->Q4
    // d->dialog->m_sheet->setReadOnly( true );

    d->loadSettings();

    // resize(sizeHint());
    resize(600, 400); // Try to show as much as possible of the table view
    setMainWidget(d->dialog);

    d->dialog->m_sheet->setSelectionMode(QAbstractItemView::MultiSelection);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(d->dialog->m_radioComma, 0);
    buttonGroup->addButton(d->dialog->m_radioSemicolon, 1);
    buttonGroup->addButton(d->dialog->m_radioSpace, 2);
    buttonGroup->addButton(d->dialog->m_radioTab, 3);
    buttonGroup->addButton(d->dialog->m_radioOther, 4);

    connect(d->dialog->m_formatComboBox, &QComboBox::textActivated, this, &KoCsvImportDialog::formatChanged);
    connect(buttonGroup, &QButtonGroup::idClicked, this, &KoCsvImportDialog::delimiterClicked);
    connect(d->dialog->m_delimiterEdit, &QLineEdit::returnPressed, this, &KoCsvImportDialog::returnPressed);
    connect(d->dialog->m_delimiterEdit, &QLineEdit::textChanged, this, &KoCsvImportDialog::genericDelimiterChanged);
    connect(d->dialog->m_comboQuote, &QComboBox::textActivated, this, &KoCsvImportDialog::textquoteSelected);
    connect(d->dialog->m_sheet, &QTableWidget::currentCellChanged, this, &KoCsvImportDialog::currentCellChanged);
    connect(d->dialog->m_ignoreDuplicates, &QCheckBox::stateChanged, this, &KoCsvImportDialog::ignoreDuplicatesChanged);
    connect(d->dialog->m_updateButton, &QAbstractButton::clicked, this, &KoCsvImportDialog::updateClicked);
}

KoCsvImportDialog::~KoCsvImportDialog()
{
    d->saveSettings();
    delete d;
}

// ----------------------------------------------------------------
//                       public methods

void KoCsvImportDialog::setData(const QByteArray &data)
{
    d->data = data;
    d->fillTable();
}

bool KoCsvImportDialog::firstRowContainHeaders() const
{
    return d->dialog->m_firstRowHeader->isChecked();
}

bool KoCsvImportDialog::firstColContainHeaders() const
{
    return d->dialog->m_firstColHeader->isChecked();
}

int KoCsvImportDialog::rows() const
{
    int rows = d->dialog->m_sheet->rowCount();

    if (d->endRow >= 0)
        rows = d->endRow - d->startRow + 1;

    return rows;
}

int KoCsvImportDialog::cols() const
{
    int cols = d->dialog->m_sheet->columnCount();

    if (d->endCol >= 0)
        cols = d->endCol - d->startCol + 1;

    return cols;
}

QString KoCsvImportDialog::text(int row, int col) const
{
    // Check for overflow.
    if (row >= rows() || col >= cols())
        return QString();

    QTableWidgetItem *item = d->dialog->m_sheet->item(row - d->startRow, col - d->startCol);
    if (!item)
        return QString();
    return item->text();
}

void KoCsvImportDialog::setDataTypes(DataTypes dataTypes)
{
    d->formatList.clear();
    if (dataTypes & Generic)
        d->formatList << i18n("Generic");
    if (dataTypes & Text)
        d->formatList << i18n("Text");
    if (dataTypes & Date)
        d->formatList << i18n("Date");
    if (dataTypes & Currency)
        d->formatList << i18n("Currency");
    if (dataTypes & None)
        d->formatList << i18n("None");
    d->dialog->m_formatComboBox->insertItems(0, d->formatList);
}

void KoCsvImportDialog::setDataWidgetEnabled(bool enable)
{
    d->dialog->m_tabWidget->setTabEnabled(0, enable);
}

QString KoCsvImportDialog::decimalSymbol() const
{
    return d->dialog->m_decimalSymbol->text();
}

void KoCsvImportDialog::setDecimalSymbol(const QString &symbol)
{
    d->dialog->m_decimalSymbol->setText(symbol);
}

QString KoCsvImportDialog::thousandsSeparator() const
{
    return d->dialog->m_thousandsSeparator->text();
}

void KoCsvImportDialog::setThousandsSeparator(const QString &separator)
{
    d->dialog->m_thousandsSeparator->setText(separator);
}

QString KoCsvImportDialog::delimiter() const
{
    return d->delimiter;
}

void KoCsvImportDialog::setDelimiter(const QString &delimit)
{
    d->delimiter = delimit;
    if (delimit == ",")
        d->dialog->m_radioComma->setChecked(true);
    else if (delimit == "\t")
        d->dialog->m_radioTab->setChecked(true);
    else if (delimit == " ")
        d->dialog->m_radioSpace->setChecked(true);
    else if (delimit == ";")
        d->dialog->m_radioSemicolon->setChecked(true);
    else {
        d->dialog->m_radioOther->setChecked(true);
        d->dialog->m_delimiterEdit->setText(delimit);
    }
}

// ----------------------------------------------------------------

void KoCsvImportDialog::Private::loadSettings()
{
    KConfigGroup configGroup = KSharedConfig::openConfig()->group("CSVDialog Settings");
    textQuote = configGroup.readEntry("textQuote", "\"").at(0);
    delimiter = configGroup.readEntry("delimiter", ",");
    ignoreDuplicates = configGroup.readEntry("ignoreDups", false);

    // update widgets
    q->setDelimiter(delimiter);
    dialog->m_ignoreDuplicates->setChecked(ignoreDuplicates);
    dialog->m_comboQuote->setCurrentIndex(textQuote == '\'' ? 1 : textQuote == '"' ? 0 : 2);
}

void KoCsvImportDialog::Private::saveSettings()
{
    KConfigGroup configGroup = KSharedConfig::openConfig()->group("CSVDialog Settings");
    configGroup.writeEntry("textQuote", QString(textQuote));
    configGroup.writeEntry("delimiter", delimiter);
    configGroup.writeEntry("ignoreDups", ignoreDuplicates);
    configGroup.sync();
}

void KoCsvImportDialog::Private::fillTable()
{
    int row, column;
    bool lastCharDelimiter = false;
    enum { Start, InQuotedField, MaybeQuotedFieldEnd, QuotedFieldEnd, MaybeInNormalField, InNormalField } state = Start;

    QChar x;
    QString field;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    dialog->m_sheet->setRowCount(0);
    dialog->m_sheet->setColumnCount(0);

    int maxColumn = 1;
    row = column = 1;
    QTextStream inputStream(data, QIODevice::ReadOnly);

    int delimiterIndex = 0;
    const int delimiterLength = delimiter.size();
    bool lastCharWasCr = false; // Last character was a Carriage Return
    while (!inputStream.atEnd()) {
        inputStream >> x; // read one char

        // ### TODO: we should perhaps skip all other control characters
        if (x == '\r') {
            // We have a Carriage Return, assume that its role is the one of a LineFeed
            lastCharWasCr = true;
            x = '\n'; // Replace by Line Feed
        } else if (x == '\n' && lastCharWasCr) {
            // The end of line was already handled by the Carriage Return, so do nothing for this character
            lastCharWasCr = false;
            continue;
        } else if (x == QChar(0xc)) {
            // We have a FormFeed, skip it
            lastCharWasCr = false;
            continue;
        } else {
            lastCharWasCr = false;
        }

        if (column > maxColumn)
            maxColumn = column;
        switch (state) {
        case Start:
            if (x == textQuote) {
                state = InQuotedField;
            } else if (delimiterIndex < delimiterLength && x == delimiter.at(delimiterIndex)) {
                field += x;
                delimiterIndex++;
                if (field.right(delimiterIndex) == delimiter) {
                    if ((ignoreDuplicates == false) || (lastCharDelimiter == false))
                        column += delimiterLength;
                    lastCharDelimiter = true;
                    field.clear();
                    delimiterIndex = 0;
                    state = Start;
                } else if (delimiterIndex >= delimiterLength)
                    delimiterIndex = 0;
            } else if (x == '\n') {
                ++row;
                column = 1;
                if (row > (endRow - startRow) && endRow >= 0)
                    break;
            } else {
                field += x;
                state = MaybeInNormalField;
            }
            break;
        case InQuotedField:
            if (x == textQuote) {
                state = MaybeQuotedFieldEnd;
            } else if (x == '\n') {
                setText(row - startRow, column - startCol, field);
                field.clear();

                ++row;
                column = 1;
                if (row > (endRow - startRow) && endRow >= 0)
                    break;

                state = Start;
            } else {
                field += x;
            }
            break;
        case MaybeQuotedFieldEnd:
            if (x == textQuote) {
                field += x;
                state = InQuotedField;
            } else if (x == '\n') {
                setText(row - startRow, column - startCol, field);
                field.clear();
                ++row;
                column = 1;
                if (row > (endRow - startRow) && endRow >= 0)
                    break;
                state = Start;
            } else if (delimiterIndex < delimiterLength && x == delimiter.at(delimiterIndex)) {
                field += x;
                delimiterIndex++;
                if (field.right(delimiterIndex) == delimiter) {
                    setText(row - startRow, column - startCol, field.left(field.count() - delimiterIndex));
                    field.clear();
                    if ((ignoreDuplicates == false) || (lastCharDelimiter == false))
                        column += delimiterLength;
                    lastCharDelimiter = true;
                    field.clear();
                    delimiterIndex = 0;
                } else if (delimiterIndex >= delimiterLength)
                    delimiterIndex = 0;
                state = Start;
            } else {
                state = QuotedFieldEnd;
            }
            break;
        case QuotedFieldEnd:
            if (x == '\n') {
                setText(row - startRow, column - startCol, field);
                field.clear();
                ++row;
                column = 1;
                if (row > (endRow - startRow) && endRow >= 0)
                    break;
                state = Start;
            } else if (delimiterIndex < delimiterLength && x == delimiter.at(delimiterIndex)) {
                field += x;
                delimiterIndex++;
                if (field.right(delimiterIndex) == delimiter) {
                    setText(row - startRow, column - startCol, field.left(field.count() - delimiterIndex));
                    field.clear();
                    if ((ignoreDuplicates == false) || (lastCharDelimiter == false))
                        column += delimiterLength;
                    lastCharDelimiter = true;
                    field.clear();
                    delimiterIndex = 0;
                } else if (delimiterIndex >= delimiterLength)
                    delimiterIndex = 0;
                state = Start;
            } else {
                state = QuotedFieldEnd;
            }
            break;
        case MaybeInNormalField:
            if (x == textQuote) {
                field.clear();
                state = InQuotedField;
                break;
            }
            state = InNormalField;
            /* fall through */
        case InNormalField:
            if (x == '\n') {
                setText(row - startRow, column - startCol, field);
                field.clear();
                ++row;
                column = 1;
                if (row > (endRow - startRow) && endRow >= 0)
                    break;
                state = Start;
            } else if (delimiterIndex < delimiterLength && x == delimiter.at(delimiterIndex)) {
                field += x;
                delimiterIndex++;
                if (field.right(delimiterIndex) == delimiter) {
                    setText(row - startRow, column - startCol, field.left(field.count() - delimiterIndex));
                    field.clear();
                    if ((ignoreDuplicates == false) || (lastCharDelimiter == false))
                        column += delimiterLength;
                    lastCharDelimiter = true;
                    field.clear();
                    delimiterIndex = 0;
                } else if (delimiterIndex >= delimiterLength)
                    delimiterIndex = 0;
                state = Start;
            } else {
                field += x;
            }
        }
        if (delimiter.isEmpty() || x != delimiter.at(0))
            lastCharDelimiter = false;
    }

    if (!field.isEmpty()) {
        // the last line of the file had not any line end
        setText(row - startRow, column - startCol, field);
        ++row;
        field.clear();
    }
    if (row)
        row--; // row is higher by 1, so reduce it

    columnsAdjusted = true;
    adjustRows(row - startRow);
    adjustCols(maxColumn - startCol);

    for (column = 0; column < dialog->m_sheet->columnCount(); ++column) {
        const QTableWidgetItem *headerItem = dialog->m_sheet->horizontalHeaderItem(column);
        if (!headerItem || !formatList.contains(headerItem->text())) {
            dialog->m_sheet->setHorizontalHeaderItem(column, new QTableWidgetItem(i18n("Generic")));
        }
    }

    dialog->m_rowStart->setMinimum(1);
    dialog->m_colStart->setMinimum(1);
    dialog->m_rowStart->setMaximum(row);
    dialog->m_colStart->setMaximum(maxColumn);

    dialog->m_rowEnd->setMinimum(1);
    dialog->m_colEnd->setMinimum(1);
    dialog->m_rowEnd->setMaximum(row);
    dialog->m_colEnd->setMaximum(maxColumn);
    dialog->m_rowEnd->setValue(endRow == -1 ? row : endRow);
    dialog->m_colEnd->setValue(endCol == -1 ? maxColumn : endCol);

    QApplication::restoreOverrideCursor();
}

KoCsvImportDialog::DataType KoCsvImportDialog::dataType(int col) const
{
    const QString header = d->dialog->m_sheet->model()->headerData(col, Qt::Horizontal).toString();

    if (header == i18n("Generic"))
        return Generic;
    else if (header == i18n("Text"))
        return Text;
    else if (header == i18n("Date"))
        return Date;
    else if (header == i18n("Currency"))
        return Currency;
    else if (header == i18n("None"))
        return None;
    return Generic;
}

void KoCsvImportDialog::Private::setText(int row, int col, const QString &text)
{
    if (row < 1 || col < 1) // skipped by the user
        return;

    if ((row > (endRow - startRow) && endRow > 0) || (col > (endCol - startCol) && endCol > 0))
        return;

    if (dialog->m_sheet->rowCount() < row) {
        dialog->m_sheet->setRowCount(row + 5000); /* We add 5000 at a time to limit recalculations */
        rowsAdjusted = true;
    }

    if (dialog->m_sheet->columnCount() < col) {
        dialog->m_sheet->setColumnCount(col);
        columnsAdjusted = true;
    }

    QTableWidgetItem *item = dialog->m_sheet->item(row - 1, col - 1);
    if (!item) {
        item = new QTableWidgetItem();
        dialog->m_sheet->setItem(row - 1, col - 1, item);
    }
    item->setText(text);
}

/*
 * Called after the first fillTable() when number of rows are unknown.
 */
void KoCsvImportDialog::Private::adjustRows(int iRows)
{
    if (rowsAdjusted) {
        dialog->m_sheet->setRowCount(iRows);
        rowsAdjusted = false;
    }
}

void KoCsvImportDialog::Private::adjustCols(int iCols)
{
    if (columnsAdjusted) {
        dialog->m_sheet->setColumnCount(iCols);
        columnsAdjusted = false;

        if (endCol == -1) {
            if (iCols > (endCol - startCol))
                iCols = endCol - startCol;

            dialog->m_sheet->setColumnCount(iCols);
        }
    }
}

void KoCsvImportDialog::returnPressed()
{
    if (d->dialog->m_radioOther->isChecked())
        return;

    d->delimiter = d->dialog->m_delimiterEdit->text();
    d->fillTable();
}

void KoCsvImportDialog::genericDelimiterChanged(const QString &)
{
    d->dialog->m_radioOther->setChecked(true);
    delimiterClicked(d->dialog->m_radioOther->group()->id(d->dialog->m_radioOther)); // other
}

void KoCsvImportDialog::formatChanged(const QString &newValue)
{
    QList<QTableWidgetSelectionRange> selectionRanges = d->dialog->m_sheet->selectedRanges();
    foreach (const QTableWidgetSelectionRange &selectionRange, selectionRanges) {
        for (int j = selectionRange.leftColumn(); j <= selectionRange.rightColumn(); ++j) {
            d->dialog->m_sheet->horizontalHeaderItem(j)->setText(newValue);
        }
    }
}

void KoCsvImportDialog::delimiterClicked(int id)
{
    const QButtonGroup *group = d->dialog->m_radioComma->group();
    if (id == group->id(d->dialog->m_radioComma))
        d->delimiter = ',';
    else if (id == group->id(d->dialog->m_radioOther))
        d->delimiter = d->dialog->m_delimiterEdit->text();
    else if (id == group->id(d->dialog->m_radioTab))
        d->delimiter = '\t';
    else if (id == group->id(d->dialog->m_radioSpace))
        d->delimiter = ' ';
    else if (id == group->id(d->dialog->m_radioSemicolon))
        d->delimiter = ';';

    debugWidgets << "Delimiter" << d->delimiter << "selected.";
    d->fillTable();
}

void KoCsvImportDialog::textquoteSelected(const QString &mark)
{
    if (mark == i18n("None"))
        d->textQuote = {};
    else
        d->textQuote = mark[0];

    d->fillTable();
}

void KoCsvImportDialog::updateClicked()
{
    if (!d->checkUpdateRange())
        return;

    d->startRow = d->dialog->m_rowStart->value() - 1;
    d->endRow = d->dialog->m_rowEnd->value();

    d->startCol = d->dialog->m_colStart->value() - 1;
    d->endCol = d->dialog->m_colEnd->value();

    d->fillTable();
}

bool KoCsvImportDialog::Private::checkUpdateRange()
{
    if ((dialog->m_rowStart->value() > dialog->m_rowEnd->value()) || (dialog->m_colStart->value() > dialog->m_colEnd->value())) {
        KMessageBox::error(nullptr, i18n("Please check the ranges you specified. The start value must be lower than the end value."));
        return false;
    }
    return true;
}

void KoCsvImportDialog::currentCellChanged(int, int col)
{
    const QString header = d->dialog->m_sheet->model()->headerData(col, Qt::Horizontal).toString();
    const int index = d->dialog->m_formatComboBox->findText(header);
    d->dialog->m_formatComboBox->setCurrentIndex(index > -1 ? index : 0);
}

void KoCsvImportDialog::ignoreDuplicatesChanged(int)
{
    if (d->dialog->m_ignoreDuplicates->isChecked())
        d->ignoreDuplicates = true;
    else
        d->ignoreDuplicates = false;
    d->fillTable();
}

#include "KoCsvImportDialog.moc"
