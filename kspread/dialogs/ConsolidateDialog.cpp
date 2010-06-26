/* This file is part of the KDE project
   Copyright 2010 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000-2001 Laurent Montel <montel@kde.org>
             (C) 1999-2002 David Faure <faure@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1999 Reginald Stadlbauer <reggie@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
#include "ConsolidateDialog.h"

//Qt Includes
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

//KDE Includes
#include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

//KSpread Includes
#include <Global.h>
#include "kspread_limits.h"
#include <Localization.h>
#include <Map.h>
#include <Selection.h>
#include <Sheet.h>
#include <Util.h>

#include <Formula.h>
#include <ValueConverter.h>
#include <Cell.h>

#include "commands/DataManipulators.h"

#include "ui_ConsolidateWidget.h"
#include "ui_ConsolidateDetailsWidget.h"

using namespace KSpread;

class ConsolidateDialog::Private
{
public:
    Selection *selection;
    Ui::ConsolidateWidget mainWidget;
    Ui::ConsolidateDetailsWidget detailsWidget;

public:
    void setContent(Sheet *sheet, int row, int column, const QString &text, QUndoCommand *parent);
};


ConsolidateDialog::ConsolidateDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , d(new Private)
{
    d->selection = selection;

    setCaption(i18n("Consolidate"));
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose);
    setButtons(Ok | Cancel | Details);
    enableButton(Ok, false);

    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);

    widget = new QWidget(this);
    d->detailsWidget.setupUi(widget);
    setDetailsWidget(widget);

    // ui refinements Designer is not capable of
    d->mainWidget.m_addButton->setIcon(KIcon("list-add"));
    d->mainWidget.m_removeButton->setIcon(KIcon("list-remove"));

    d->mainWidget.m_function->addItem(i18n("Sum"), "SUM");
    d->mainWidget.m_function->addItem(i18n("Average"), "AVERAGE");
    d->mainWidget.m_function->addItem(i18n("Count"), "COUNT");
    d->mainWidget.m_function->addItem(i18n("Max"), "MAX");
    d->mainWidget.m_function->addItem(i18n("Min"), "MIN");
    d->mainWidget.m_function->addItem(i18n("Product"), "PRODUCT");
    d->mainWidget.m_function->addItem(i18n("Standard Deviation"), "STDDEV");
    d->mainWidget.m_function->addItem(i18n("Variance"), "VAR");

    connect(d->mainWidget.m_addButton, SIGNAL(clicked()),
            this, SLOT(slotAdd()));
    connect(d->mainWidget.m_removeButton, SIGNAL(clicked()),
            this, SLOT(slotRemove()));
    connect(d->mainWidget.m_sourceRange, SIGNAL(returnPressed()),
            this, SLOT(slotReturnPressed()));

    connect(d->selection, SIGNAL(changed(const Region&)),
            this, SLOT(slotSelectionChanged()));
}

ConsolidateDialog::~ConsolidateDialog()
{
    delete d;
}

void ConsolidateDialog::accept()
{
    // Quit, if there are no source ranges.
    if (d->mainWidget.m_sourceRanges->count() == 0) {
        KMessageBox::error(this, i18n("You have to define at least one source range."));
        return;
    }

    Map *const map = d->selection->activeSheet()->map();
    ValueConverter *const converter = map->converter();

    Sheet *const destinationSheet = d->selection->activeSheet();
    int dx = d->selection->lastRange().left();
    int dy = d->selection->lastRange().top();

    const int index = d->mainWidget.m_function->currentIndex();
    const QString function = d->mainWidget.m_function->itemData(index).toString();

    QList<Region> ranges;
    for (int i = 0; i < d->mainWidget.m_sourceRanges->count(); ++i) {
        const QString address = d->mainWidget.m_sourceRanges->item(i)->text();
        const Region region(address, map, destinationSheet);
        if (!region.isValid()) {
            KMessageBox::error(this, i18n("%1 is not a valid cell range." , address));
            return;
        }
        ranges.append(region);
    }

    enum { D_ROW, D_COL, D_NONE, D_BOTH } desc;
    if (d->detailsWidget.m_rowHeader->isChecked() && d->detailsWidget.m_columnHeader->isChecked())
        desc = D_BOTH;
    else if (d->detailsWidget.m_rowHeader->isChecked())
        desc = D_ROW;
    else if (d->detailsWidget.m_columnHeader->isChecked())
        desc = D_COL;
    else
        desc = D_NONE;

    const QRect firstRange = ranges[0].firstRange();
    const int columns = firstRange.width();
    const int rows = firstRange.height();

    // Check, if the first cell range has a sufficient size.
    if (columns <= ((desc == D_BOTH || desc == D_COL) ? 1 : 0) ||
            rows <= ((desc == D_BOTH || desc == D_ROW) ? 1 : 0)) {
        KMessageBox::error(this, i18n("The range\n%1\nis too small", ranges[0].name()));
        return;
    }

    // Check, if the first cell range is too large.
    if (ranges[0].isAllSelected() || ranges[0].isColumnOrRowSelected()) {
        KMessageBox::error(this, i18n("The range\n%1\nis too large" , ranges[0].name()));
        return;
    }

    // Check the other cell ranges.
    for (int i = 1; i < ranges.count(); ++i) {
        const int columns2 = ranges[i].firstRange().width();
        const int rows2 = ranges[i].firstRange().height();

        // Check, if the cell range is too large.
        if (ranges[i].isAllSelected() || ranges[i].isColumnOrRowSelected()) {
            KMessageBox::error(this, i18n("The range\n%1\nis too large" , ranges[i].name()));
            return;
        }

        // Check whether all ranges...
        // - have the same size, if no headers are given.
        // - have the same amount of rows, if only column headers are given
        // - have the same amount of columns, if only row headers are given
        if ((desc == D_NONE && (columns != columns2 || rows != rows2)) ||
                (desc == D_COL && rows != rows2) ||
                (desc == D_ROW && columns != columns2)) {
            QString tmp = i18n("The ranges\n%1\nand\n%2\nhave different size", ranges[0].name() , ranges[i].name());
            KMessageBox::error(this, tmp);
            return;
        }
    }

    // Create the cumulating parent command.
    QUndoCommand *const command = new QUndoCommand(i18n("Consolidate"));

    // Create the destination cell range
    if (desc == D_NONE) {
        // Check whether the destination is part of the source ...
        const QRect destinationRange(dx, dy, columns, rows);
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();

            if (sheet == destinationSheet && range.intersects(destinationRange)) {
                QString tmp(i18n("The source range intersects the destination range."));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Now create the destination cell range
        for (int col = 0; col < columns; ++col) {
            for (int row = 0; row < rows; ++row) {
                bool novalue = true;
                QString formula = '=' + function + '(';
                for (int i = 0; i < ranges.count(); ++i) {
                    Sheet *const sheet = ranges[i].firstSheet();
                    Q_ASSERT(sheet);
                    const QRect range = ranges[i].firstRange();
                    const Cell cell(sheet, col + range.left(), row + range.top());
                    if (!cell.value().isEmpty())
                        novalue = false;
                    if (i != 0) {
                        formula += ';';
                    }
                    formula += (sheet != destinationSheet) ? cell.fullName() : cell.name();
                }
                formula += ')';

                if (!novalue) {
                    d->setContent(destinationSheet, dy + row, dx + col, formula, command);
                }
            }
        }
    } else if (desc == D_COL) {
        // Get list of all descriptions in the columns
        QHash<QString, QList<Cell> > columnHeaderCells;
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            for (int col = range.left(); col <= range.right() ; ++col) {
                const Cell cell(sheet, col, range.top());
                const Value value = cell.value();
                const QString columnHeader = converter->asString(value).asString();
                columnHeaderCells[columnHeader].append(cell);
            }
        }
        QStringList columnHeaders = columnHeaderCells.keys();
        columnHeaders.sort();

        // Check whether the destination is part of the source ...
        const QRect destinationRange(dx, dy, columnHeaders.count(), rows);
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            if (sheet == destinationSheet && range.intersects(destinationRange)) {
                QString tmp(i18n("The source range intersects the destination range."));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Now create the destination cell range
        for (int col = 0; col < columnHeaders.count(); ++col) {
            // Fill in the header data.
            const QString columnHeader = columnHeaders[col];
            d->setContent(destinationSheet, dy, dx + col, columnHeader, command);

            const QList<Cell> cells = columnHeaderCells[columnHeader];
            for (int row = 1; row < rows; ++row) {
                QString formula = '=' + function + '(';
                for (int i = 0; i < cells.count(); ++i) {
                    if (i != 0) {
                        formula += ';';
                    }
                    Sheet *const sheet = cells[i].sheet();
                    const int headerColumn = cells[i].column();
                    const int headerRow = cells[i].row();
                    const Cell cell(sheet, headerColumn, headerRow + row);
                    const bool fullName = sheet != destinationSheet;
                    formula += fullName ? cell.fullName() : cell.name();
                }
                formula += ')';

                d->setContent(destinationSheet, dy + row, dx + col, formula, command);
            }
        }
    } else if (desc == D_ROW) {
        // Get list of all descriptions in the rows
        QHash<QString, QList<Cell> > rowHeaderCells;
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            for (int row = range.top(); row <= range.bottom() ; ++row) {
                const Cell cell(sheet, range.left(), row);
                const Value value = cell.value();
                const QString rowHeader = converter->asString(value).asString();
                rowHeaderCells[rowHeader].append(cell);
            }
        }
        QStringList rowHeaders = rowHeaderCells.keys();
        rowHeaders.sort();

        // Check whether the destination is part of the source ...
        const QRect destinationRange(dx, dy, columns, rowHeaders.count());
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            if (sheet == destinationSheet && range.intersects(destinationRange)) {
                QString tmp(i18n("The source range intersects the destination range."));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Now create the destination cell range
        for (int row = 0; row < rowHeaders.count(); ++row) {
            // Fill in the header data.
            const QString rowHeader = rowHeaders[row];
            d->setContent(destinationSheet, dy + row, dx, rowHeader, command);

            const QList<Cell> cells = rowHeaderCells[rowHeader];
            for (int col = 1; col < columns; ++col) {
                QString formula = '=' + function + '(';
                for (int i = 0; i < cells.count(); ++i) {
                    if (i != 0) {
                        formula += ';';
                    }
                    Sheet *const sheet = cells[i].sheet();
                    const int headerColumn = cells[i].column();
                    const int headerRow = cells[i].row();
                    const Cell cell(sheet, headerColumn + col, headerRow);
                    const bool fullName = sheet != destinationSheet;
                    formula += fullName ? cell.fullName() : cell.name();
                }
                formula += ')';

                d->setContent(destinationSheet, dy + row, dx + col, formula, command);
            }
        }
    } else if (desc == D_BOTH) {
        // Get list of all descriptions in the rows
        QStringList rowHeaders;
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            for (int row = range.top() + 1; row <= range.bottom() ; ++row) {
                const Value value = Cell(sheet, range.left(), row).value();
                const QString rowHeader = converter->asString(value).asString();
                if (!rowHeaders.contains(rowHeader)) {
                    rowHeaders.append(rowHeader);
                }
            }
        }
        rowHeaders.sort();

        // Get list of all descriptions in the columns
        QStringList columnHeaders;
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            for (int col = range.left() + 1; col <= range.right() ; ++col) {
                const Value value = Cell(sheet, col, range.top()).value();
                const QString columnHeader = converter->asString(value).asString();
                if (!columnHeaders.contains(columnHeader)) {
                    columnHeaders.append(columnHeader);
                }
            }
        }
        columnHeaders.sort();

        // Check whether the destination is part of the source ...
        const QRect destinationRange(dx, dy, columnHeaders.count(), rowHeaders.count());
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            if (sheet == destinationSheet && range.intersects(destinationRange)) {
                QString tmp(i18n("The source range intersects the destination range."));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Fill the list with all interesting cells
        QHash<QString /* row */, QHash<QString /* col */, QList<Cell> > > list;
        for (int i = 0; i < ranges.count(); ++i) {
            Sheet *const sheet = ranges[i].firstSheet();
            Q_ASSERT(sheet);
            const QRect range = ranges[i].firstRange();
            for (int col = range.left() + 1; col <= range.right() ; ++col) {
                const Value columnValue = Cell(sheet, col, range.top()).value();
                const QString columnHeader = converter->asString(columnValue).asString();
                for (int row = range.top() + 1; row <= range.bottom() ; ++row) {
                    const Value rowValue = Cell(sheet, range.left(), row).value();
                    const QString rowHeader = converter->asString(rowValue).asString();
                    list[rowHeader][columnHeader].append(Cell(sheet, col, row));
                }
            }
        }

        // Draw the row description
        for (int i = 0; i < rowHeaders.count(); ++i) {
            d->setContent(destinationSheet, dy + 1 + i, dx, rowHeaders[i], command);
        }

        // Draw the column description
        for (int i = 0; i < columnHeaders.count(); ++i) {
            d->setContent(destinationSheet, dy, dx + 1 + i, columnHeaders[i], command);
        }

        // Draw the data
        for (int row = 0; row < rowHeaders.count(); ++row) {
            for (int col = 0; col < columnHeaders.count(); ++col) {
                QString formula = '=' + function + '(';
                const QString rowHeader = rowHeaders[row];
                const QString columnHeader = columnHeaders[col];
                const QList<Cell> lst = list[rowHeader][columnHeader];
                for (int i = 0; i < lst.count(); ++i) {
                    if (i != 0) {
                        formula += ';';
                    }
                    const bool fullName = lst[i].sheet() != destinationSheet;
                    formula += fullName ? lst[i].fullName() : lst[i].name();
                }
                formula += ')';

                d->setContent(destinationSheet, dy + 1 + row, dx + 1 + col, formula, command);
            }
        }
    }

    // execute the cumulating parent command
    map->addCommand(command);

    KDialog::accept();
}

void ConsolidateDialog::slotAdd()
{
    slotReturnPressed();
}

void ConsolidateDialog::slotRemove()
{
    int i = d->mainWidget.m_sourceRanges->currentRow();
    if (i < 0)
        return;

    delete d->mainWidget.m_sourceRanges->takeItem(i);

    if (d->mainWidget.m_sourceRanges->count() == 0)
        enableButton(Ok, false);
}

void ConsolidateDialog::slotSelectionChanged()
{
    if (!d->selection->isValid()) {
        d->mainWidget.m_sourceRange->setText("");
        return;
    }

    QString area = d->selection->name();
    d->mainWidget.m_sourceRange->setText(area);
    d->mainWidget.m_sourceRange->setSelection(0, area.length());
}

void ConsolidateDialog::slotReturnPressed()
{
    QString txt = d->mainWidget.m_sourceRange->text();

    const Region r(txt, d->selection->activeSheet()->map());
    if (!r.isValid()) {
        KMessageBox::error(this, i18n("The range\n%1\n is malformed", txt));
        return;
    }

    if (!txt.isEmpty()) {
        d->mainWidget.m_sourceRanges->addItem(txt);
        enableButton(Ok, true);
    }
}

void ConsolidateDialog::Private::setContent(Sheet *sheet, int row, int column,
                                            const QString &text, QUndoCommand *parent)
{
    Value value;
    // Directly evaluate the formula, i.e. copy data, i.e. do not link to data?
    if (detailsWidget.m_copyData->isChecked()) {
        Formula formula(sheet);
        formula.setExpression(text);
        if (!formula.isValid()) {
            kDebug() << "Invalid formula:" << text;
            return; // Quit before creating/adding the sub-command.
        }
        value = formula.eval();
    } else {
        value = Value(text);
    }

    DataManipulator *const command = new DataManipulator(parent);
    command->setSheet(sheet);
    command->setValue(value);
    command->setParsing(!detailsWidget.m_copyData->isChecked());
    command->add(QPoint(column, row));
    // executed by the parent command
}

#include "ConsolidateDialog.moc"
