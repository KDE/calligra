/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KPresenterPageIface.h"

#include "kprpage.h"
#include "kpresenter_view.h"
#include "kpbackground.h"
#include <kapplication.h>
#include <dcopclient.h>

KPresenterPageIface::KPresenterPageIface( KPrPage *_page )
    : DCOPObject()
{
   m_page = _page;
}

QString KPresenterPageIface::getManualTitle()const
{
    return m_page->getManualTitle();
}

void KPresenterPageIface::insertManualTitle(const QString & title)
{
    m_page->insertManualTitle(title);
}

QString KPresenterPageIface::pageTitle( const QString &_title ) const
{
    return m_page->pageTitle(_title);
}

void KPresenterPageIface::setNoteText( const QString &_text )
{
    m_page->setNoteText(_text);
}

QString KPresenterPageIface::getNoteText( )const
{
    return m_page->getNoteText();
}

unsigned int KPresenterPageIface::objNums() const
{
    return m_page->objNums();
}

int KPresenterPageIface::numSelected() const
{
    return m_page->numSelected();
}

void KPresenterPageIface::groupObjects()
{
    m_page->groupObjects();
}

void KPresenterPageIface::ungroupObjects()
{
    m_page->ungroupObjects();
}

void KPresenterPageIface::raiseObjs()
{
    m_page->raiseObjs();
}

void KPresenterPageIface::lowerObjs()
{
    m_page->lowerObjs();
}

void KPresenterPageIface::copyObjs()
{
    m_page->copyObjs();
}

void KPresenterPageIface::alignObjsLeft()
{
    m_page->alignObjsLeft();
}

void KPresenterPageIface::alignObjsCenterH()
{
    m_page->alignObjsCenterH();
}

void KPresenterPageIface::alignObjsRight()
{
    m_page->alignObjsRight();
}

void KPresenterPageIface::alignObjsTop()
{
    m_page->alignObjsTop();
}

void KPresenterPageIface::alignObjsCenterV()
{
    m_page->alignObjsCenterV();
}

void KPresenterPageIface::alignObjsBottom()
{
    m_page->alignObjsBottom();
}

void KPresenterPageIface::slotRepaintVariable()
{
    m_page->slotRepaintVariable();
}

void KPresenterPageIface::recalcPageNum()
{
    m_page->recalcPageNum();
}


void KPresenterPageIface::setPageTimer(  int pageTimer )
{
    m_page->setPageTimer(pageTimer);
}

void KPresenterPageIface::setPageSoundEffect(  bool soundEffect )
{
    m_page->setPageSoundEffect(soundEffect);
}

void KPresenterPageIface::setPageSoundFileName(  const QString &fileName )
{
    m_page->setPageSoundFileName(fileName);
}

int KPresenterPageIface::getBackXFactor()const
{
    return m_page->getBackXFactor();
}

int KPresenterPageIface::getBackYFactor( )const
{
    return m_page->getBackYFactor();
}

int KPresenterPageIface::getPageTimer(  )const
{
    return m_page->getPageTimer();
}

bool KPresenterPageIface::getPageSoundEffect( )const
{
    return m_page->getPageSoundEffect();
}

int KPresenterPageIface::getBackType()const
{
  return (int)m_page->getBackType();
}

int KPresenterPageIface::getBackView()const
{
    return (int)m_page->getBackView();
}

QColor KPresenterPageIface::getBackColor1()const
{
    return m_page->getBackColor1();
}

QColor KPresenterPageIface::getBackColor2()const
{
    return m_page->getBackColor2();
}

int KPresenterPageIface::getBackColorType()const
{
    return  (int)m_page->getBackColorType();
}

QString KPresenterPageIface::getBackPixFilename()const
{
    return m_page->getBackPixKey().filename();
}

QString KPresenterPageIface::getBackClipFilename()const
{
    return m_page->getBackPixKey().filename();
}

int KPresenterPageIface::getPageEffect()const
{
    return (int)m_page->getPageEffect();
}

bool KPresenterPageIface::getBackUnbalanced()const
{
    return (int)m_page->getBackUnbalanced();
}

QSize KPresenterPageIface::getSize()const
{
    return m_page->background()->getSize();
}

bool KPresenterPageIface::setRectSettings( int _rx, int _ry )
{
    return m_page->setRectSettings(_rx,_ry);
}

QString KPresenterPageIface::getPageSoundFileName()const
{
    return m_page->getPageSoundFileName();
}

int KPresenterPageIface::getPieAngle( int pieAngle )const
{
    return m_page->getPieAngle(pieAngle);
}

int KPresenterPageIface::getPieLength( int pieLength )const
{
    return m_page->getPieLength(pieLength);
}

