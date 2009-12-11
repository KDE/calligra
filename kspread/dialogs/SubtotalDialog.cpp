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
#include "Selection.h"
#include "Sheet.h"

// commands
#include "commands/DataManipulators.h"

using namespace KSpread;

SubtotalDialog::SubtotalDialog(QWidget* parent, Selection* selection)
        : KDialog(parent),
        m_selection(selection),
        m_pSheet(m_selection->activeSheet()),
        m_range(selection->lastRange())
{
    setCaption(i18n("Subtotals"));
    setButtons(Ok | Cancel | User1);
    setButtonGuiItem(User1, KGuiItem(i18n("Remove All")));
    setButtonsOrientation(Qt::Vertical);

    QWidget* widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    fillColumnBoxes();
    fillFunctionBox();
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotUser1()));
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(slotCancel()));

}

SubtotalDialog::~SubtotalDialog()
{
}

void SubtotalDialog::slotOk()
{
    int numOfCols = m_range.width();
    QVector<int> columns(numOfCols);

    bool empty = true;
    int left = m_range.left();
    for (int i = 0; i < m_columnList->count(); ++i) {
        QListWidgetItem* item = m_columnList->item(i);
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

    if (m_replaceSubtotals->isChecked())
        removeSubtotalLines();

    int mainCol = left + m_columnBox->currentIndex();
    int bottom = m_range.bottom();
    int top    = m_range.top();
    left       = m_range.left();
    QString oldText = Cell(m_pSheet, mainCol, top).displayText();
    QString newText;
    QString result(' ' + i18n("Result"));
    int lastChangedRow = top;

    bool ignoreEmptyCells = m_IgnoreBox->isChecked();
    bool addRow;
    if (!m_summaryOnly->isChecked()) {
        int y = top + 1;
        kDebug() << "Starting in row" << y;
        while (y <= bottom) {
            addRow = true;
            newText = Cell(m_pSheet, mainCol, y).displayText();

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

    if (m_summaryBelow->isChecked()) {
        addRow = true;
        int bottom = m_range.bottom();
        for (int x = 0; x < numOfCols; ++x) {
            if (columns[x] != -1) {
                addSubtotal(mainCol, columns[x], bottom, top, addRow, i18n("Grand Total"));
                addRow = false;
            }
        }
    }

    m_selection->emitModified();
    accept();
}

void SubtotalDialog::slotCancel()
{
    reject();
}

void SubtotalDialog::slotUser1()
{
    removeSubtotalLines();
    m_selection->emitModified();
    accept();
}

void SubtotalDialog::removeSubtotalLines()
{
    kDebug() << "Removing subtotal lines";

    int r = m_range.right();
    int l = m_range.left();
    int t = m_range.top();

    Cell cell;
    QString text;

    for (int y = m_range.bottom(); y >= t; --y) {
        kDebug() << "Checking row:" << y;
        bool containsSubtotal = false;
        for (int x = l; x <= r; ++x) {
            cell = Cell(m_pSheet, x, y);
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
            QRect rect(l, y, m_range.width(), 1);

            ShiftManipulator* manipulator = new ShiftManipulator();
            manipulator->setSheet(m_pSheet);
            manipulator->setDirection(ShiftManipulator::ShiftBottom);
            manipulator->setReverse(true);
            manipulator->add(Region(rect));
            manipulator->execute(m_selection->canvas());
            m_range.setHeight(m_range.height() - 1);
        }
    }
    kDebug() << "Done removing subtotals";
}

void SubtotalDialog::fillColumnBoxes()
{
    int r = m_range.right();
    int row = m_range.top();

    Cell cell;
    QListWidgetItem * item;

    QString text;

    int index = 0;
    for (int i = m_range.left(); i <= r; ++i) {
        cell = Cell(m_pSheet, i, row);
        text = cell.displayText();

        //if ( text.length() > 0 )
        {
            text = i18n("Column '%1' ", Cell::columnName(i));
        }

        m_columnBox->insertItem(index++, text);

        item = new QListWidgetItem(text);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        m_columnList->addItem(item);
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
    m_functionBox->insertItems(0, lst);
}

bool SubtotalDialog::addSubtotal(int mainCol, int column, int row, int topRow,
                                 bool addRow, QString const & text)
{
    kDebug() << "Adding subtotal:" << mainCol << "," << column << ", Rows:" << row << "," << topRow
    << ": addRow: " << addRow << ", Text: " << text << endl;
    if (addRow) {
        QRect rect(m_range.left(), row + 1, m_range.width(), 1);
        ShiftManipulator* manipulator = new ShiftManipulator();
        manipulator->setSheet(m_pSheet);
        manipulator->setDirection(ShiftManipulator::ShiftBottom);
        manipulator->add(Region(rect));
        manipulator->execute(m_selection->canvas());

        m_range.setHeight(m_range.height() + 1);

        Cell cell = Cell(m_pSheet, mainCol, row + 1);
        cell.parseUserInput(text);
        Style style;
        style.setFontBold(true);
        style.setFontItalic(true);
        style.setFontUnderline(true);
        cell.setStyle(style);
    }

    QString colName = Cell::columnName(column);

    QString formula("=SUBTOTAL(");
    formula += QString::number(m_functionBox->currentIndex() + 1);
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

    Cell cell = Cell(m_pSheet, column, row + 1);
    cell.parseUserInput(formula);
    Style style;
    style.setFontBold(true);
    style.setFontItalic(true);
    style.setFontUnderline(true);
    cell.setStyle(style);
    return true;
}

#include "SubtotalDialog.moc"
