/* This file is part of the KDE project

   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2001-2002 Laurent Montel <montel@kde.org>
   Copyright 2001 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 Torben Weis <weis@kde.org>

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
#include "ViewAdaptor.h"

#include "part/Doc.h"
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "part/View.h"
#include "part/Canvas.h"
#include "MapAdaptor.h"

// commands
#include "commands/BorderColorCommand.h"
#include "commands/CommentCommand.h"
#include "commands/StyleCommand.h"

#include <KoShapeManager.h>
#include <KoSelection.h>

using namespace KSpread;

/************************************************
 *
 * ViewAdaptor
 *
 ************************************************/

ViewAdaptor::ViewAdaptor(View* t)
        : QDBusAbstractAdaptor(t)
{
    setAutoRelaySignals(true);
    m_view = t;

    Q_ASSERT(t->selection());
    connect(t->selection(), SIGNAL(changed(Region)), this, SIGNAL(selectionChanged()));
}

ViewAdaptor::~ViewAdaptor()
{
}

QString ViewAdaptor::doc() const
{
    return m_view->doc()->objectName();
}

QString ViewAdaptor::map() const
{
    return m_view->doc()->map()->objectName();
}

QString ViewAdaptor::sheet() const
{
    return m_view->activeSheet()->objectName();
}

void ViewAdaptor::changeNbOfRecentFiles(int _nb)
{
    if (_nb < 0)
        return;
    m_view->changeNbOfRecentFiles(_nb);
}


void ViewAdaptor::hide()
{
    m_view->hide();
}

void ViewAdaptor::show()
{
    m_view->show();
}

#if 0 // -> cell tool
void ViewAdaptor::find()
{
    m_view->find();
}

void ViewAdaptor::replace()
{
    m_view->replace();
}

void ViewAdaptor::conditional()
{
    m_view->conditional();
}

void ViewAdaptor::validity()
{
    m_view->validity();
}

void ViewAdaptor::insertSeries()
{
    m_view->insertSeries();
}

void ViewAdaptor::insertHyperlink()
{
    m_view->insertHyperlink();
}

void ViewAdaptor::gotoCell()
{
    m_view->gotoCell();
}

void ViewAdaptor::changeAngle()
{
    m_view->changeAngle();
}
#endif

void ViewAdaptor::preference()
{
    m_view->preference();
}

void ViewAdaptor::nextSheet()
{
    m_view->nextSheet();
}

bool ViewAdaptor::showSheet(const QString& sheetName)
{
    Sheet *const sheet = m_view->doc()->map()->findSheet(sheetName);
    if (!sheet) {
        kDebug(36001) << "Unknown sheet" << sheetName;
        return false;
    }
    m_view->selection()->emitCloseEditor(true); // save changes
    m_view->setActiveSheet(sheet);
    return true;
}

void ViewAdaptor::previousSheet()
{
    m_view->previousSheet();
}

#if 0 // -> cell tool
void ViewAdaptor::sortList()
{
    m_view->sortList();
}

void ViewAdaptor::setAreaName()
{
    m_view->setAreaName();
}

void ViewAdaptor::showAreaName()
{
    m_view->namedAreaDialog();
}

void ViewAdaptor::mergeCell()
{
    m_view->mergeCell();
}

void ViewAdaptor::dissociateCell()
{
    m_view->dissociateCell();
}

void ViewAdaptor::consolidate()
{
    m_view->consolidate();
}

void ViewAdaptor::deleteColumn()
{
    m_view->deleteColumn();
}

void ViewAdaptor::insertColumn()
{
    m_view->insertColumn();
}

void ViewAdaptor::deleteRow()
{
    m_view->deleteRow();
}

void ViewAdaptor::insertRow()
{
    m_view->insertRow();
}

void ViewAdaptor::hideRow()
{
    m_view->hideRow();
}

void ViewAdaptor::showRow()
{
    m_view->showRow();
}

void ViewAdaptor::hideColumn()
{
    m_view->hideColumn();
}

void ViewAdaptor::showColumn()
{
    m_view->showColumn();
}

void ViewAdaptor::upper()
{
    m_view->upper();
}

void ViewAdaptor::lower()
{
    m_view->lower();
}

void ViewAdaptor::equalizeColumn()
{
    m_view->equalizeColumn();
}

void ViewAdaptor::equalizeRow()
{
    m_view->equalizeRow();
}

void ViewAdaptor::clearTextSelection()
{
    m_view->clearContents();
}

void ViewAdaptor::clearCommentSelection()
{
    m_view->clearComment();
}

void ViewAdaptor::clearValiditySelection()
{
    m_view->clearValidity();
}

void ViewAdaptor::clearConditionalSelection()
{
    m_view->clearConditionalStyles();
}

void ViewAdaptor::goalSeek()
{
    m_view->goalSeek();
}

void ViewAdaptor::insertFromDatabase()
{
    m_view->insertFromDatabase();
}

void ViewAdaptor::insertFromTextfile()
{
    m_view->insertFromTextfile();
}

void ViewAdaptor::insertFromClipboard()
{
    m_view->insertFromClipboard();
}

void ViewAdaptor::textToColumns()
{
    m_view->textToColumns();
}
#endif

void ViewAdaptor::copyAsText()
{
    m_view->copyAsText();
}

void ViewAdaptor::setSelection(const QRect& selection)
{
    m_view->selection()->initialize(selection);//,m_view->activeSheet());
}

QRect ViewAdaptor::selection()
{
    return m_view->selection()->lastRange();
}

#if 0 // -> cell tool
void ViewAdaptor::setSelectionMoneyFormat(bool b)
{
    m_view->moneyFormat(b);
}

void ViewAdaptor::setSelectionPercent(bool b)
{
    m_view->percent(b);
}

void ViewAdaptor::setSelectionMultiRow(bool enable)
{
    m_view->wrapText(enable);
}

void ViewAdaptor::setSelectionSize(int size)
{
    m_view->setSelectionFontSize(size);
}

void ViewAdaptor::setSelectionUpper()
{
    m_view->upper();
}

void ViewAdaptor::setSelectionLower()
{
    m_view->lower();
}

void ViewAdaptor::setSelectionFirstLetterUpper()
{
    m_view->firstLetterUpper();
}

void ViewAdaptor::setSelectionVerticalText(bool enable)
{
    m_view->verticalText(enable);
}
#endif

void ViewAdaptor::setSelectionComment(const QString& comment)
{
    CommentCommand* command = new CommentCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Add Comment"));
    command->setComment(comment.trimmed());
    command->add(*m_view->selection());
    command->execute();
}

#if 0 // -> cell tool
void ViewAdaptor::setSelectionAngle(int value)
{
    m_view->setSelectionAngle(value);
}
#endif

void ViewAdaptor::setSelectionTextColor(const QColor& txtColor)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Text Color"));
    command->setFontColor(txtColor);
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setSelectionBgColor(const QColor& bgColor)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Background Color"));
    command->setBackgroundColor(bgColor);
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setSelectionBorderColor(const QColor& bgColor)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Background Color"));
    command->setBackgroundColor(bgColor);
    command->add(*m_view->selection());
    command->execute();
}

#if 0 // -> cell tool
void ViewAdaptor::deleteSelection()
{
    m_view->clearAll();
}

void ViewAdaptor::copySelection()
{
    m_view->copySelection();
}

void ViewAdaptor::cutSelection()
{
    m_view->cutSelection();
}
#endif

void ViewAdaptor::setLeftBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Border"));
    if (m_view->activeSheet()->layoutDirection() == Qt::RightToLeft)
        command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setTopBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setRightBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Border"));
    if (m_view->activeSheet()->layoutDirection() == Qt::RightToLeft)
        command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setBottomBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Border"));
    command->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setAllBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setHorizontalPen(QPen(color, 1, Qt::SolidLine));
    command->setVerticalPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

void ViewAdaptor::setOutlineBorderColor(const QColor& color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(m_view->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->add(*m_view->selection());
    command->execute();
}

#if 0 // -> cell tool
void ViewAdaptor::removeBorder()
{
    m_view->borderRemove();
}

void ViewAdaptor::increaseIndent()
{
    m_view->increaseIndent();
}

void ViewAdaptor::decreaseIndent()
{
    m_view->increaseIndent();
}

void ViewAdaptor::increasePrecision()
{
    m_view->increasePrecision();
}

void ViewAdaptor::decreasePrecision()
{
    m_view->decreasePrecision();
}

void ViewAdaptor::subtotals()
{
    m_view->subtotals();
}

void ViewAdaptor::sortInc()
{
    m_view->sortInc();
}

void ViewAdaptor::sortDec()
{
    m_view->sortDec();
}

void ViewAdaptor::layoutDlg()
{
    m_view->layoutDlg();
}


void ViewAdaptor::increaseFontSize()
{
    m_view->increaseFontSize();
}

void ViewAdaptor::decreaseFontSize()
{
    m_view->decreaseFontSize();
}
#endif

#include "ViewAdaptor.moc"
