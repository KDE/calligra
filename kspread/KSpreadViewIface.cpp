/* This file is part of the KDE project
   
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
 * Boston, MA 02110-1301, USA.
*/

#include <dcopclient.h>
#include <dcopref.h>
#include <kapplication.h>

#include "Doc.h"
#include "Map.h"
#include "Selection.h"
#include "Sheet.h"
#include "View.h"

#include "KSpreadViewIface.h"

using namespace KSpread;

/************************************************
 *
 * ViewIface
 *
 ************************************************/

ViewIface::ViewIface( View* t )
    : KoViewIface( t )
{
    m_view = t;
}

ViewIface::~ViewIface()
{
}

DCOPRef ViewIface::doc() const
{
    return DCOPRef( kapp->dcopClient()->appId(), m_view->doc()->dcopObject()->objId() );
}

DCOPRef ViewIface::map() const
{
    return DCOPRef( kapp->dcopClient()->appId(), m_view->doc()->map()->dcopObject()->objId() );
}

DCOPRef ViewIface::sheet() const
{
    return DCOPRef( kapp->dcopClient()->appId(), m_view->activeSheet()->dcopObject()->objId() );
}

void ViewIface::changeNbOfRecentFiles(int _nb)
{
    if(_nb<0)
        return;
    m_view->changeNbOfRecentFiles(_nb);
}


void ViewIface::hide()
{
    m_view->hide();
}

void ViewIface::show()
{
    m_view->show();
}

void ViewIface::find()
{
    m_view->find();
}

void ViewIface::replace()
{
    m_view->replace();
}

void ViewIface::conditional()
{
    m_view->conditional();
}

void ViewIface::validity()
{
    m_view->validity();
}

void ViewIface::insertSeries()
{
    m_view->insertSeries();
}

void ViewIface::insertHyperlink()
{
    m_view->insertHyperlink();
}

void ViewIface::gotoCell()
{
    m_view->gotoCell();
}

void ViewIface::changeAngle()
{
    m_view->changeAngle();
}

void ViewIface::preference()
{
    m_view->preference();
}

void ViewIface::nextSheet()
{
    m_view->nextSheet();
}

bool ViewIface::showSheet(QString sheetName)
{
   return m_view->showSheet(sheetName);
}

void ViewIface::previousSheet()
{
    m_view->previousSheet();
}

void ViewIface::sortList()
{
    m_view->sortList();
}

void ViewIface::setAreaName()
{
    m_view->setAreaName();
}

void ViewIface::showAreaName()
{
    m_view->showAreaName();
}

void ViewIface::mergeCell()
{
    m_view->mergeCell();
}

void ViewIface::dissociateCell()
{
    m_view->dissociateCell();
}

void ViewIface::consolidate()
{
    m_view->consolidate();
}

void ViewIface::deleteColumn()
{
    m_view->deleteColumn();
}

void ViewIface::insertColumn()
{
    m_view->insertColumn();
}

void ViewIface::deleteRow()
{
    m_view->deleteRow();
}

void ViewIface::insertRow()
{
    m_view->insertRow();
}

void ViewIface::hideRow()
{
    m_view->hideRow();
}

void ViewIface::showRow()
{
    m_view->showRow();
}

void ViewIface::hideColumn()
{
    m_view->hideColumn();
}

void ViewIface::showColumn()
{
    m_view->showColumn();
}

void ViewIface::upper()
{
    m_view->upper();
}

void ViewIface::lower()
{
    m_view->lower();
}

void ViewIface::equalizeColumn()
{
    m_view->equalizeColumn();
}

void ViewIface::equalizeRow()
{
    m_view->equalizeRow();
}

void ViewIface::clearTextSelection()
{
    m_view->clearTextSelection();
}

void ViewIface::clearCommentSelection()
{
    m_view->clearCommentSelection();
}

void ViewIface::clearValiditySelection()
{
    m_view->clearValiditySelection();
}

void ViewIface::clearConditionalSelection()
{
    m_view->clearConditionalSelection();
}

void ViewIface::goalSeek()
{
    m_view->goalSeek();
}

void ViewIface::insertFromDatabase()
{
    m_view->insertFromDatabase();
}

void ViewIface::insertFromTextfile()
{
    m_view->insertFromTextfile();
}

void ViewIface::insertFromClipboard()
{
    m_view->insertFromClipboard();
}

void ViewIface::textToColumns()
{
    m_view->textToColumns();
}

void ViewIface::copyAsText()
{
    m_view->copyAsText();
}

void ViewIface::setSelection(QRect selection)
{
  m_view->selectionInfo()->initialize(selection);//,m_view->activeSheet());
}

QRect ViewIface::selection()
{
  return m_view->selectionInfo()->selection();
}

void ViewIface::setSelectionMoneyFormat( bool b )
{
  m_view->moneyFormat(b);
}

void ViewIface::setSelectionPrecision( int delta )
{
  m_view->setSelectionPrecision(delta);
}

void ViewIface::setSelectionPercent( bool b )
{
  m_view->percent(b);
}

void ViewIface::setSelectionMultiRow( bool enable )
{
  m_view->wrapText(enable);
}

void ViewIface::setSelectionSize(int size)
{
  m_view->setSelectionFontSize(size);
}

void ViewIface::setSelectionUpper()
{
  m_view->upper();
}

void ViewIface::setSelectionLower()
{
  m_view->lower();
}

void ViewIface::setSelectionFirstLetterUpper()
{
  m_view->firstLetterUpper();
}

void ViewIface::setSelectionVerticalText(bool enable)
{
  m_view->verticalText(enable);
}

void ViewIface::setSelectionComment(QString comment)
{
  m_view->setSelectionComment(comment);
}

void ViewIface::setSelectionAngle(int value)
{
  m_view->setSelectionAngle(value);
}

void ViewIface::setSelectionTextColor( QColor txtColor )
{
  m_view->setSelectionTextColor(txtColor);
}

void ViewIface::setSelectionBgColor( QColor bgColor )
{
  m_view->setSelectionBackgroundColor(bgColor);
}

void ViewIface::setSelectionBorderColor( QColor bdColor )
{
  m_view->setSelectionBorderColor(bdColor);
}

void ViewIface::deleteSelection()
{
  m_view->clearTextSelection();
}

void ViewIface::copySelection()
{
  m_view->copySelection();
}

void ViewIface::cutSelection()
{
  m_view->cutSelection();
}

void ViewIface::setLeftBorderColor(QColor color)
{
  m_view->setSelectionLeftBorderColor(color);
}

void ViewIface::setTopBorderColor(QColor color)
{
  m_view->setSelectionTopBorderColor(color);
}

void ViewIface::setRightBorderColor(QColor color)
{
  m_view->setSelectionRightBorderColor(color);
}

void ViewIface::setBottomBorderColor(QColor color)
{
  m_view->setSelectionBottomBorderColor(color);
}

void ViewIface::setAllBorderColor(QColor color)
{
  m_view->setSelectionAllBorderColor(color);
}

void ViewIface::setOutlineBorderColor(QColor color)
{
  m_view->setSelectionOutlineBorderColor(color);
}

void ViewIface::removeBorder()
{
  m_view->borderRemove();
}

void ViewIface::increaseIndent()
{
  m_view->increaseIndent();
}

void ViewIface::decreaseIndent()
{
  m_view->increaseIndent();
}

void ViewIface::subtotals()
{
    m_view->subtotals();
}

void ViewIface::sortInc()
{
    m_view->sortInc();
}

void ViewIface::sortDec()
{
    m_view->sortDec();
}

void ViewIface::layoutDlg()
{
    m_view->layoutDlg();
}


void ViewIface::increaseFontSize()
{
    m_view->increaseFontSize();
}

void ViewIface::decreaseFontSize()
{
    m_view->decreaseFontSize();
}
