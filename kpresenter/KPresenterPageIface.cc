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
#include "kptextobject.h"
#include "kpresenter_view.h"
#include "kpbackground.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kpresenter_doc.h>
#include "kpresenter_view.h"
#include "kprcanvas.h"
#include <kdebug.h>

KPresenterPageIface::KPresenterPageIface( KPrPage *_page )
    : DCOPObject()
{
   m_page = _page;
}

DCOPRef KPresenterPageIface::textObject( int num )
{
    KPTextObject * textObj=m_page->textFrameSet(num);
    if(textObj)
        return DCOPRef( kapp->dcopClient()->appId(),
                        textObj->dcopObject()->objId() );
    return DCOPRef();
}

//return a reference to selected object
DCOPRef KPresenterPageIface::selectedObject( )
{
    KPObject * obj=m_page->getSelectedObj();
    if(obj)
        return DCOPRef( kapp->dcopClient()->appId(),
                        obj->dcopObject()->objId() );
    return DCOPRef();
}


int KPresenterPageIface::numTextObject() const
{
    return m_page->numTextObject();
}


DCOPRef KPresenterPageIface::object( int num )
{
     if( num >= m_page->objNums())
         return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_page->getObject(num)->dcopObject()->objId() );
}

QString KPresenterPageIface::manualTitle()const
{
    return m_page->manualTitle();
}

void KPresenterPageIface::insertManualTitle(const QString & title)
{
    m_page->insertManualTitle(title);
    int pos=m_page->kPresenterDoc()->pageList().findRef(m_page);
    m_page->kPresenterDoc()->updateSideBarItem(pos);
}

QString KPresenterPageIface::pageTitle( const QString &_title ) const
{
    return m_page->pageTitle(_title);
}

void KPresenterPageIface::setNoteText( const QString &_text )
{
    m_page->setNoteText(_text);
}

QString KPresenterPageIface::noteText( )const
{
    return m_page->noteText();
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

int KPresenterPageIface::backXFactor()const
{
    return m_page->getBackXFactor();
}

int KPresenterPageIface::backYFactor( )const
{
    return m_page->getBackYFactor();
}

int KPresenterPageIface::pageTimer(  )const
{
    return m_page->getPageTimer();
}

bool KPresenterPageIface::pageSoundEffect( )const
{
    return m_page->getPageSoundEffect();
}

int KPresenterPageIface::backType()const
{
  return (int)m_page->getBackType();
}

int KPresenterPageIface::backView()const
{
    return (int)m_page->getBackView();
}

QColor KPresenterPageIface::backColor1()const
{
    return m_page->getBackColor1();
}

QColor KPresenterPageIface::backColor2()const
{
    return m_page->getBackColor2();
}

int KPresenterPageIface::backColorType()const
{
    return  (int)m_page->getBackColorType();
}

QString KPresenterPageIface::backPixFilename()const
{
    return m_page->getBackPixKey().filename();
}

QString KPresenterPageIface::backClipFilename()const
{
    return m_page->getBackPixKey().filename();
}

int KPresenterPageIface::pageEffect()const
{
    return (int)m_page->getPageEffect();
}

void KPresenterPageIface::setPageEffect(const QString &effect )
{
    if(effect=="NONE")
        m_page->setPageEffect(PEF_NONE);
    else if(effect=="CLOSE_HORZ")
        m_page->setPageEffect(PEF_CLOSE_HORZ);
        else if(effect=="CLOSE_VERT")
        m_page->setPageEffect(PEF_CLOSE_VERT);
    else if(effect=="CLOSE_ALL")
        m_page->setPageEffect(PEF_CLOSE_ALL);
    else if(effect=="OPEN_HORZ")
        m_page->setPageEffect(PEF_OPEN_HORZ);
    else if(effect=="OPEN_VERT")
        m_page->setPageEffect(PEF_OPEN_VERT);
    else if(effect=="OPEN_ALL")
        m_page->setPageEffect(PEF_OPEN_ALL);
    else if(effect=="INTERLOCKING_HORZ_1")
        m_page->setPageEffect(PEF_INTERLOCKING_HORZ_1);
    else if(effect=="INTERLOCKING_HORZ_2")
        m_page->setPageEffect(PEF_INTERLOCKING_HORZ_2);
    else if(effect=="INTERLOCKING_VERT_1")
        m_page->setPageEffect(PEF_INTERLOCKING_VERT_1);
    else if(effect=="INTERLOCKING_VERT_2")
        m_page->setPageEffect(PEF_INTERLOCKING_VERT_2);
    else if(effect=="SURROUND1")
        m_page->setPageEffect(PEF_SURROUND1);
    else if(effect=="FLY1")
        m_page->setPageEffect(PEF_FLY1);
    else
        kdDebug()<<"Error in setPageEffect\n";
}

bool KPresenterPageIface::backUnbalanced()const
{
    return (int)m_page->getBackUnbalanced();
}

bool KPresenterPageIface::setRectSettings( int _rx, int _ry )
{
    return m_page->setRectSettings(_rx,_ry);
}

QString KPresenterPageIface::pageSoundFileName()const
{
    return m_page->getPageSoundFileName();
}

int KPresenterPageIface::pieAngle( int pieAngle )const
{
    return m_page->getPieAngle(pieAngle);
}

int KPresenterPageIface::pieLength( int pieLength )const
{
    return m_page->getPieLength(pieLength);
}

QRect KPresenterPageIface::pageRect() const
{
    return m_page->getZoomPageRect();
}

bool KPresenterPageIface::isSlideSelected()
{
    return m_page->isSlideSelected();
}

void KPresenterPageIface::slideSelected(bool _b)
{
    m_page->slideSelected(_b);
    int pos=m_page->kPresenterDoc()->pageList().findRef(m_page);
    m_page->kPresenterDoc()->updateSideBarItem(pos);
}

void KPresenterPageIface::changePicture( const QString & filename )
{
    m_page->changePicture(filename);
}

void KPresenterPageIface::changeClipart( const QString & filename )
{
    m_page->changeClipart(filename);
}

//create a rectangle and return a dcop reference!
DCOPRef KPresenterPageIface::insertRectangle(int x,int y, int h, int w)
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertRect( QRect(x,y,h,w) );
  return selectedObject();
}

DCOPRef KPresenterPageIface::insertEllipse( int x,int y, int h, int w )
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertEllipse(QRect(x,y,h,w) );
  return selectedObject();
}

DCOPRef KPresenterPageIface::insertPie( int x,int y, int h, int w )
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertPie( QRect(x,y,h,w) );
  return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineH( int x,int y, int h, int w, bool rev )
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertLineH( QRect(x,y,h,w), rev );
  return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineV( int x,int y, int h, int w, bool rev )
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertLineV( QRect(x,y,h,w), rev );
  return selectedObject();

}

DCOPRef KPresenterPageIface::insertLineD1( int x,int y, int h, int w, bool rev )
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertLineD1( QRect(x,y,h,w), rev );
  return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineD2( int x,int y, int h, int w, bool rev )
{
  KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
  view->getCanvas()->insertLineD2( QRect(x,y,h,w), rev );
  return selectedObject();
}

DCOPRef KPresenterPageIface::insertTextObject( int x,int y, int h, int w )
{
    KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
    view->getCanvas()->insertTextObject( QRect(x,y,h,w));
    return selectedObject();

}

DCOPRef KPresenterPageIface::insertPicture( const QString & file,int x,int y, int h, int w )
{
    m_page->setInsPictureFile(file);
    KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
    view->getCanvas()->insertPicture( QRect(x,y,h,w));
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertClipart( const QString & file,int x,int y, int h, int w )
{
    m_page->setInsClipartFile(file);
    KPresenterView *view=m_page->kPresenterDoc()->getKPresenterView();
    view->getCanvas()->insertClipart( QRect(x,y,h,w));
    return selectedObject();
}

void KPresenterPageIface::setBackGroundColor1(const QColor &col)
{
    const QColor col2=backColor2();
    m_page->setBackColor( col, col2, m_page->getBackColorType(),m_page->getBackUnbalanced(), backXFactor(), backYFactor() );
}

void KPresenterPageIface::setBackGroundColor2(const QColor &col)
{
    const QColor col1=backColor1();
    m_page->setBackColor( col1, col, m_page->getBackColorType(),m_page->getBackUnbalanced(), backXFactor(), backYFactor() );
}

int KPresenterPageIface::rndY()
{
    int ret=-1;
    m_page->getRndY( ret );
    return ret;
}

int KPresenterPageIface::rndX()
{
    int ret=-1;
    m_page->getRndX( ret );
    return ret;

}
