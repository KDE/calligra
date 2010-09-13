/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "SubtotalDialog.h"

#include "ui_SubtotalWidget.h"
#include "ui_SubtotalsDetailsWidget.h"

// Qt
#include <QCheckBox>
#include <QListWidget>
#include <QVector>

// KDE
#include <kcombobox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

// KSpread
#include "ui/Selection.h"
#include "Sheet.h"

// commands
#include "commands/DataManipulators.h"

using namespace KSpread;

class SubtotalDialog::Private
{
public:
    Selection *selection;
    Ui::SubtotalsWidget mainWidget;
    Ui::SubtotalsDetailsWidget detailsWidget;
};

SubtotalDialog::SubtotalDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , d(new Private)
{
    d->selection = selection;

    setCaption(i18n("Subtotals"));
    setButtons(Ok | Cancel | Details | User1);
    setButtonGuiItem(User1, KGuiItem(i18n("Remove All")));

    QWidget* widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);

    widget = new QWidget(this);
    d->detailsWidget.setupUi(widget);
    setDetailsWidget(widget);

    fillColumnBoxes();
    fillFunctionBox();
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotUser1()));
}

SubtotalDialog::~SubtotalDialog()
{
    delete d;
}

void SubtotalDialog::accept()
{
    Sheet *const sheet = d->selection->lastSheet();
    const QRect range = d->selection->lastRange();

    int numOfCols = range.width();
    QVector<int> columns(numOfCols);

    bool empty = true;
    int left = range.left();
    for (int i = 0; i < d->mainWidget.m_columnList->count(); ++i) {
        QListWidgetItem* item = d->mainWidget.m_columnList->item(i);
        if (item->checkState() == Qt::Checked) {
            columns[i] = left + i;
            empty = false;
        } else
            columns[i] = -1;
    }

    if (empty) {
        KMessageBox::sorry(this, i18n("You need to select at least one column for adding subtotals."));
        return;
    }

    if (d->detailsWidget.m_replaceSubtotals->isChecked())
        removeSubtotalLines();

    int mainCol = left + d->mainWidget.m_columnBox->currentIndex();
    int bottom = range.bottom();
    int top    = range.top();
    left       = range.left();
    QString oldText = Cell(sheet, mainCol, top).displayText();
    QString newText;
    QString result(' ' + i18n("Result"));
    int lastChangedRow = top;

    bool ignoreEmptyCells = d->detailsWidget.m_IgnoreBox->isChecked();
    bool addRow;
    if (!d->detailsWidget.m_summaryOnly->isChecked()) {
        int y = top + 1;
        kDebug() << "Starting in row" << y;
        while (y <= bottom) {
            addRow = true;
            newText = Cell(sheet, mainCol, y).displayText();

            if (ignoreEmptyCells && (newText.length() == 0)) {
                ++y;
                kDebug() << "Still the same ->" << y;
                continue;
            }

            if (newText != oldText) {
                int saveY = y;
                for (int x = 0; x < numOfCols; ++x) {
                    kDebug() << "Column:" << x << "," << columns[x];
                    if (columns[x] != -1) {
                        if (!addSubtotal(mainCol, columns[x], y - 1, lastChangedRow, addRow, oldText + result))
                            reject();

                        if (addRow) {
                            ++saveY;
                            ++bottom;
                        }

                        addRow = false;
                    }
                }
                y = saveY;
                lastChangedRow = y;
            }
            oldText = newText;
            ++y;
        }

        addRow = true;
        for (int x = 0; x < numOfCols; ++x) {
            if (columns[x] != -1) {
                if (!addSubtotal(mainCol, columns[x], y - 1, lastChangedRow, addRow, oldText + result))
                    reject();
                addRow = false;
            }
        }
        ++y;
    }

    if (d->detailsWidget.m_summaryBelow->isChecked()) {
        addRow = true;
        int bottom = range.bottom();
        for (int x = 0; x < numOfCols; ++x) {
            if (columns[x] != -1) {
                addSubtotal(mainCol, columns[x], bottom, top, addRow, i18n("Grand Total"));
                addRow = false;
            }
        }
    }

    KDialog::accept();
}

void SubtotalDialog::reject()
{
    KDialog::reject();
}

void SubtotalDialog::slotUser1()
{
    removeSubtotalLines();
    KDialog::accept();
}

void SubtotalDialog::removeSubtotalLines()
{
    kDebug() << "Removing subtotal lines";

    Sheet *const sheet = d->selection->lastSheet();
    QRect range = d->selection->lastRange();

    int r = range.right();
    int l = range.left();
    int t = range.top();

    Cell cell;
    QString text;

    for (int y = range.bottom(); y >= t; --y) {
        kDebug() << "Checking row:" << y;
        bool containsSubtotal = false;
        for (int x = l; x <= r; ++x) {
            cell = Cell(sheet, x, y);
            if (!cell.isFormula())
                continue;

            text = cell.userInput();
            if (text.indexOf("SUBTOTAL") != -1) {
                containsSubtotal = true;
                break;
            }
        }

        if (containsSubtotal) {
            kDebug() << "Line" << y << " contains a subtotal";
            QRect rect(l, y, range.width(), 1);

            ShiftManipulator* manipulator = new ShiftManipulator();
            manipulator->setSheet(sheet);
            manipulator->setDirection(ShiftManipulator::ShiftBottom);
            manipulator->setReverse(true);
            manipulator->add(Region(rect));
            manipulator->execute(d->selection->canvas());
            range.setHeight(range.height() - 1);
        }
    }

    d->selection->initialize(range, sheet);
    kDebug() << "Done removing subtotals";
}

void SubtotalDialog::fillColumnBoxes()
{
    Sheet *const sheet = d->selection->lastSheet();
    const QRect range = d->selection->lastRange();

    int r = range.right();
    int row = range.top();

    Cell cell;
    QListWidgetItem * item;

    QString text;

    int index = 0;
    for (int i = range.left(); i <= r; ++i) {
        cell = Cell(sheet, i, row);
        text = cell.displayText();

        //if ( text.length() > 0 )
        {
            text = i18n("Column '%1' ", Cell::columnName(i));
        }

        d->mainWidget.m_columnBox->insertItem(index++, text);

        item = new QListWidgetItem(text);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        d->mainWidget.m_columnList->addItem(item);
    }
}

void SubtotalDialog::fillFunctionBox()
{
    QStringList lst;
    lst << i18n("Average");
    lst << i18n("Count");
    lst << i18n("CountA");
    lst << i18n("Max");
    lst << i18n("Min");
    lst << i18n("Product");
    lst << i18n("StDev");
    lst << i18n("StDevP");
    lst << i18n("Sum");
    lst << i18n("Var");
    lst << i18n("VarP");
    d->mainWidget.m_functionBox->insertItems(0, lst);
}

bool SubtotalDialog::addSubtotal(int mainCol, int column, int row, int topRow,
                                 bool addRow, QString const & text)
{
    Sheet *const sheet = d->selection->lastSheet();
    QRect range = d->selection->lastRange();

    kDebug() << "Adding subtotal:" << mainCol << "," << column << ", Rows:" << row << "," << topRow
    << ": addRow: " << addRow << ", Text: " << text << endl;
    if (addRow) {
        QRect rect(range.left(), row + 1, range.width(), 1);
        ShiftManipulator* manipulator = new ShiftManipulator();
        manipulator->setSheet(sheet);
        manipulator->setDirection(ShiftManipulator::ShiftBottom);
        manipulator->add(Region(rect));
        manipulator->execute(d->selection->canvas());

        range.setHeight(range.height() + 1);

        Cell cell = Cell(sheet, mainCol, row + 1);
        cell.parseUserInput(text);
        Style style;
        style.setFontBold(true);
        style.setFontItalic(true);
        style.setFontUnderline(true);
        cell.setStyle(style);
    }

    QString colName = Cell::columnName(column);

    QString formula("=SUBTOTAL(");
    formula += QString::number(d->mainWidget.m_functionBox->currentIndex() + 1);
    formula += "; ";
    formula += colName;
    formula += QString::number(topRow);
    // if ( topRow != row )
    {
        formula += ':';
        formula += colName;
        formula += QString::number(row);
    }
    formula += ')';

    Cell cell = Cell(sheet, column, row + 1);
    cell.parseUserInput(formula);
    Style style;
    style.setFontBold(true);
    style.setFontItalic(true);
    style.setFontUnderline(true);
    cell.setStyle(style);

    d->selection->initialize(range, sheet);
    return true;
}

#include "SubtotalDialog.moc"
