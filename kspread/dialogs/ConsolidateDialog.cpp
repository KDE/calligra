/* This file is part of the KDE project
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


#include <assert.h>

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

using namespace KSpread;

ConsolidateDialog::ConsolidateDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Consolidate"));
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose);
    setButtons(Ok | Cancel);
    m_selection = selection;

    QWidget *page = new QWidget();
    setMainWidget(page);

    QGridLayout *grid1 = new QGridLayout(page);

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel(page);
    grid1->addWidget(tmpQLabel, 0, 0);
    tmpQLabel->setText(i18n("&Function:"));

    m_pFunction = new KComboBox(page);
    grid1->addWidget(m_pFunction, 0, 1);
    tmpQLabel->setBuddy(m_pFunction);

    m_pFunction->insertItem(Sum, i18n("Sum"));
    m_pFunction->insertItem(Average, i18n("Average"));
    m_pFunction->insertItem(Count, i18n("Count"));
    m_pFunction->insertItem(Max, i18n("Max"));
    m_pFunction->insertItem(Min, i18n("Min"));
    m_pFunction->insertItem(Product, i18n("Product"));
    m_pFunction->insertItem(StdDev, i18n("Standard Deviation"));
    m_pFunction->insertItem(Var, i18n("Variance"));

    tmpQLabel = new QLabel(page);
    tmpQLabel->setText(i18n("Re&ference:"));
    grid1->addWidget(tmpQLabel, 1, 0);

    m_pRef = new KLineEdit(page);
    grid1->addWidget(m_pRef, 1, 1);
    tmpQLabel->setBuddy(m_pRef);

    tmpQLabel = new QLabel(page);
    grid1->addWidget(tmpQLabel, 2, 0, Qt::AlignTop);
    tmpQLabel->setText(i18n("&Entered references:"));

    m_pRefs = new QListWidget(page);
    grid1->addWidget(m_pRefs, 2, 1);
    tmpQLabel->setBuddy(m_pRefs);

    m_pRow = new QCheckBox(i18n("&Description in row"), page);
    grid1->addWidget(m_pRow, 3, 0, 1, 3);
    m_pCol = new QCheckBox(i18n("De&scription in column"), page);
    grid1->addWidget(m_pCol, 4, 0, 1, 3);
    m_pCopy = new QCheckBox(i18n("Co&py data"), page);
    grid1->addWidget(m_pCopy, 5, 0, 1, 3);

    m_pAdd = new QPushButton(i18n("&Add"), page);
    grid1->addWidget(m_pAdd, 1, 2);
    m_pRemove = new QPushButton(i18n("&Remove"), page);
    grid1->addWidget(m_pRemove, 2, 2, Qt::AlignTop);

    connect(m_pAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
    connect(m_pRemove, SIGNAL(clicked()), this, SLOT(slotRemove()));
    connect(m_pRef, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));

    connect(m_selection, SIGNAL(changed(const Region&)),
            this, SLOT(slotSelectionChanged()));
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(slotCancel()));
}

ConsolidateDialog::~ConsolidateDialog()
{
    kDebug(36001) << "Consolidate::~Consolidate()";
}

enum Description { D_ROW, D_COL, D_NONE, D_BOTH };

struct st_cell {
    QString xdesc;
    QString ydesc;
    Cell cell;
    QString sheet;
    int x;
    int y;
};

void ConsolidateDialog::slotOk()
{
    Map *map = m_selection->activeSheet()->map();

    Sheet* sheet = m_selection->activeSheet();
    int dx = m_selection->lastRange().left();
    int dy = m_selection->lastRange().top();

    QString function;

    switch (m_pFunction->currentIndex()) {
    case Sum:      function = "SUM"; break;
    case Average:  function = "AVERAGE"; break;
    case Count:    function = "COUNT"; break;
    case Max:      function = "MAX"; break;
    case Min:      function = "MIN"; break;
    case Product:  function = "PRODUCT"; break;
    case StdDev:   function = "STDDEV"; break;
    case Var:      function = "VARIANCE"; break;
    default: break; // bad bad !
    }

    QStringList r = refs();
    QList<Region> ranges;
    for (int i = 0; i < r.count(); ++i) {
        const Region region(r[i], map, sheet);
        // TODO: Check for valid
        Q_ASSERT(region.isValid());
        ranges.append(region);
    }

    Description desc;
    if (m_pRow->isChecked() && m_pCol->isChecked())
        desc = D_BOTH;
    else if (m_pRow->isChecked())
        desc = D_ROW;
    else if (m_pCol->isChecked())
        desc = D_COL;
    else
        desc = D_NONE;

    // Check whether all ranges have same size
    Q_ASSERT(ranges.count() > 0);
    QList<Region>::Iterator it = ranges.begin();
    int w = (*it).firstRange().right() - (*it).firstRange().left() + 1;
    int h = (*it).firstRange().bottom() - (*it).firstRange().top() + 1;
    if (w <= ((desc == D_BOTH || desc == D_COL) ? 1 : 0) ||
            h <= ((desc == D_BOTH || desc == D_ROW) ? 1 : 0)) {
        m_selection->emitModified();
        KMessageBox::error(this, i18n("The range\n%1\nis too small" , *(r.begin())));
        return;
    }

    if ((*it).firstRange().bottom() == KS_rowMax || (*it).firstRange().right() == KS_colMax) {
        m_selection->emitModified();
        KMessageBox::error(this, i18n("The range\n%1\nis too large" , *(r.begin())));
        return;
    }

    ++it;
    int i = 1;
    for (; it != ranges.end(); ++it, i++) {
        QRect currentRange = (*it).firstRange();

        int w2 = currentRange.right() - currentRange.left() + 1;
        int h2 = currentRange.bottom() - currentRange.top() + 1;

        if (currentRange.bottom() == KS_rowMax || currentRange.right() == KS_colMax) {
            m_selection->emitModified();
            KMessageBox::error(this, i18n("The range\n%1\nis too large" , r[i]));
            return;
        }
        if ((desc == D_NONE && (w != w2 || h != h2)) ||
                (desc == D_ROW && h != h2) ||
                (desc == D_COL && w != w2)) {
            m_selection->emitModified();
            QString tmp = i18n("The ranges\n%1\nand\n%2\nhave different size", *(r.begin()) , r[i]);
            KMessageBox::error(this, tmp);
            return;
        }
    }

    // Create the consolidation sheet
    if (desc == D_NONE) {
        // Check whether the destination is part of the source ...
        QRect dest;
        dest.setCoords(dx, dy, dx + w - 1, dy + h - 1);
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            Q_ASSERT(t);
            QRect r;

            QRect currentRange = (*it).firstRange();

            r.setCoords(currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom());
            if (t == sheet && r.intersects(dest)) {
                m_selection->emitModified();
                QString tmp(i18n("The source tables intersect with the destination table"));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                bool novalue = true;
                QString formula = '=' + function + '(';
                it = ranges.begin();
                for (; it != ranges.end(); ++it) {
                    Sheet *t = (*it).firstSheet();
                    assert(t);
                    Cell cell = Cell(t, x + (*it).firstRange().left(), y + (*it).firstRange().top());
                    if (!cell.isDefault())
                        novalue = false;
                    if (it != ranges.begin())
                        formula += ';';
                    formula += (*it).firstSheet()->sheetName() + '!';
                    formula += cell.name();
                }
                formula += ')';

                if (!novalue) {
                    setText(sheet, dy + y, dx + x, m_pCopy->isChecked() ? evaluate(formula, sheet) : formula);
                }
            }
        }
    } else if (desc == D_ROW) {
        // Get list of all descriptions in the rows
        QStringList lst;
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
//      kDebug(36001) <<"FROM" << (*it).range.left() <<" to" << (*it).range.right();
            for (int x = (*it).firstRange().left(); x <= (*it).firstRange().right() ; ++x) {
                Cell cell = Cell(t, x, (*it).firstRange().top());
                if (!cell.isNull()) {
                    QString s = cell.value().asString();
                    if (!lst.contains(s))
                        lst.append(s);
                }
            }
        }
        lst.sort();

        // Check whether the destination is part of the source ...
        QRect dest;
        dest.setCoords(dx, dy, dx + lst.count() - 1, dy + h - 1);
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            QRect r;
            QRect currentRange = (*it).firstRange();
            r.setCoords(currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom());
            if (t == sheet && r.intersects(dest)) {
                m_selection->emitModified();
                QString tmp(i18n("The source tables intersect with the destination table"));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Now create the consolidation sheet
        int x = 0;
        QStringList::Iterator s = lst.begin();
        for (; s != lst.end(); ++s, ++x) {
            setText(sheet, dy, dx + x, *s);

            for (int y = 1; y < h; ++y) {
                int count = 0;
                QString formula = '=' + function + '(';
                it = ranges.begin();
                for (; it != ranges.end(); ++it) {
                    for (int i = (*it).firstRange().left(); i <= (*it).firstRange().right(); ++i) {
                        Sheet *t = (*it).firstSheet();
                        assert(t);
                        Cell cell = Cell(t, i, (*it).firstRange().top());
                        if (!cell.isNull()) {
                            if (cell.value().asString() == *s) {
//  Cell cell2 = Cell( t, i, y + (*it).range.top() );
                                count++;
                                if (it != ranges.begin())
                                    formula += ';';
                                formula += (*it).firstSheet()->sheetName() + '!';
                                formula += Cell::name(i, y + (*it).firstRange().top());
                            }
                        }
                    }
                }
                formula += ')';

                setText(sheet, dy + y, dx + x, m_pCopy->isChecked() ? evaluate(formula, sheet) : formula);
            }
        }
    } else if (desc == D_COL) {
        // Get list of all descriptions in the columns
        QStringList lst;
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            for (int y = (*it).firstRange().top(); y <= (*it).firstRange().bottom() ; ++y) {
                Cell cell = Cell(t, (*it).firstRange().left(), y);
                if (!cell.isNull()) {
                    QString s = cell.value().asString();
                    if (!s.isEmpty() && lst.indexOf(s) == -1)
                        lst.append(s);
                }
            }
        }
        lst.sort();

        // Check whether the destination is part of the source ...
        QRect dest;
        dest.setCoords(dx, dy, dx + w - 1, dy + lst.count() - 1);
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            QRect r;
            QRect currentRange = (*it).firstRange();
            r.setCoords(currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom());
            if (t == sheet && r.intersects(dest)) {
                m_selection->emitModified();
                QString tmp(i18n("The source tables intersect with the destination table"));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Now create the consolidation sheet
        int y = 0;
        QStringList::Iterator s = lst.begin();
        for (; s != lst.end(); ++s, ++y) {
            setText(sheet, dy + y, dx, *s);

            for (int x = 1; x < w; ++x) {
                int count = 0;
                QString formula = '=' + function + '(';
                it = ranges.begin();
                for (; it != ranges.end(); ++it) {
                    for (int i = (*it).firstRange().top(); i <= (*it).firstRange().bottom(); i++) {
                        Sheet *t = (*it).firstSheet();
                        assert(t);
                        Cell cell = Cell(t, (*it).firstRange().left(), i);
                        if (!cell.isNull()) {
                            QString v = cell.value().asString();
                            if (!v.isEmpty() && *s == v) {
//  Cell cell2 = Cell( t, x + (*it).range.left(), i );
                                count++;
                                if (it != ranges.begin()) formula += ';';
                                formula += (*it).firstSheet()->sheetName() + '!';
                                formula += Cell::name(i, y + (*it).firstRange().top());
                            }
                        }
                    }
                }

                formula += ')';

                setText(sheet, dy + y, dx + x, m_pCopy->isChecked() ? evaluate(formula, sheet) : formula);
            }
        }
    } else if (desc == D_BOTH) {
        // Get list of all descriptions in the columns
        QStringList cols;
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            for (int y = (*it).firstRange().top() + 1; y <= (*it).firstRange().bottom() ; ++y) {
                Cell cell = Cell(t, (*it).firstRange().left(), y);
                if (!cell.isNull()) {
                    QString s = cell.value().asString();
                    if (!s.isEmpty() && cols.indexOf(s) == -1)
                        cols.append(s);
                }
            }
        }
        cols.sort();

        // Get list of all descriptions in the rows
        QStringList rows;
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            for (int x = (*it).firstRange().left() + 1; x <= (*it).firstRange().right() ; ++x) {
                Cell cell = Cell(t, x, (*it).firstRange().top());
                if (!cell.isNull()) {
                    QString s = cell.value().asString();
                    if (!s.isEmpty() && rows.indexOf(s) == -1)
                        rows.append(s);
                }
            }
        }
        rows.sort();

        // Check whether the destination is part of the source ...
        QRect dest;
        dest.setCoords(dx, dy, dx + cols.count(), dy + rows.count());
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            QRect r;
            QRect currentRange = (*it).firstRange();
            r.setCoords(currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom());
            if (t == sheet && r.intersects(dest)) {
                m_selection->emitModified();
                QString tmp(i18n("The source tables intersect with the destination table"));
                KMessageBox::error(this, tmp);
                return;
            }
        }

        // Fill the list with all interesting cells
        QLinkedList<st_cell> lst;
        it = ranges.begin();
        for (; it != ranges.end(); ++it) {
            Sheet *t = (*it).firstSheet();
            assert(t);
            for (int x = (*it).firstRange().left() + 1; x <= (*it).firstRange().right() ; ++x) {
                Cell cell = Cell(t, x, (*it).firstRange().top());
                if (!cell.isNull()) {
                    QString ydesc = cell.value().asString();
                    for (int y = (*it).firstRange().top() + 1; y <= (*it).firstRange().bottom() ; ++y) {
                        Cell cell2 = Cell(t, (*it).firstRange().left(), y);
                        if (!cell2.isNull()) {
                            QString xdesc = cell2.value().asString();
                            Cell cell3 = Cell(t, x, y);
                            if (!cell3.isNull() && cell3.value().isNumber()) {
                                st_cell k;
                                k.xdesc = xdesc;
                                k.ydesc = ydesc;
                                k.cell = cell3;
                                k.sheet = (*it).firstSheet()->sheetName();
                                k.x = x;
                                k.y = y;
                                lst.append(k);
                            }
                        }
                    }
                }
            }
        }

        // Draw the row description
        int i = 1;
        QStringList::Iterator s = rows.begin();
        for (; s != rows.end(); ++s, ++i) {
            setText(sheet, dy, dx + i, *s);
        }

        // Draw the column description
        i = 1;
        s = cols.begin();
        for (; s != cols.end(); ++s, ++i) {
            setText(sheet, dy + i, dx, *s);
        }

        // Draw the data
        int x = 1;
        QStringList::Iterator ydesc = rows.begin();
        for (; ydesc != rows.end(); ++ydesc, x++) {
            int y = 1;
            QStringList::Iterator xdesc = cols.begin();
            for (; xdesc != cols.end(); ++xdesc, y++) {
                int count = 0;
                QString formula = '=' + function + '(';
                QLinkedList<st_cell>::Iterator lit = lst.begin();
                for (; lit != lst.end(); ++lit) {
                    if ((*lit).xdesc == *xdesc && (*lit).ydesc == *ydesc) {
                        count++;
                        if (it != ranges.begin()) formula += ';';
                        formula += (*it).firstSheet()->sheetName() + '!';
                        formula += Cell::name(i, y + (*it).firstRange().top());
                    }
                }
                formula += ')';

                setText(sheet, dy + y, dx + x, m_pCopy->isChecked() ? evaluate(formula, sheet) : formula);
            }
        }
    }
    m_selection->emitModified();
    done(QDialog::Accepted);
}

void ConsolidateDialog::slotCancel()
{
    done(QDialog::Rejected);
}

void ConsolidateDialog::slotAdd()
{
    slotReturnPressed();
}

void ConsolidateDialog::slotRemove()
{
    int i = m_pRefs->currentRow();
    if (i < 0)
        return;

    delete m_pRefs->takeItem(i);

    if (m_pRefs->count() == 0)
        enableButton(Ok, false);
}

QStringList ConsolidateDialog::refs()
{
    QStringList list;
    int c = m_pRefs->count();

    for (int i = 0; i < c; i++)
        list.append(m_pRefs->item(i)->text());

    return list;
}

void ConsolidateDialog::slotSelectionChanged()
{
    if (!m_selection->isValid()) {
        m_pRef->setText("");
        return;
    }

    QString area = m_selection->name();
    m_pRef->setText(area);
    m_pRef->setSelection(0, area.length());
}

void ConsolidateDialog::slotReturnPressed()
{
    QString txt = m_pRef->text();

    const Region r(txt, m_selection->activeSheet()->map());
    if (!r.isValid()) {
        KMessageBox::error(this, i18n("The range\n%1\n is malformed", txt));
        return;
    }

    if (!txt.isEmpty()) {
        m_pRefs->addItem(txt);
        enableButton(Ok, true);
    }
}

QString ConsolidateDialog::evaluate(const QString& formula, Sheet* sheet)
{
    QString result = "###";
    Formula *f = new Formula(sheet);
    f->setExpression(formula);
    if (!f->isValid()) {
        delete f;
        return result;
    }

    Value res = f->eval();
    delete f;
    result = sheet->map()->converter()->asString(res).asString();
    return result;
}

void ConsolidateDialog::setText(Sheet* sheet, int _row, int _column,
                                const QString& _text, bool asString)
{
    DataManipulator* const command = new DataManipulator();
    command->setSheet(sheet);
    command->setValue(Value(_text));
    command->setParsing(!asString);
    command->add(QPoint(_column, _row));
    command->execute();
}

#include "ConsolidateDialog.moc"
