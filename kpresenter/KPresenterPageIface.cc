// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002,2003,2004 Laurent MONTEL <montel@kde.org>

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
#include "kpresenter_doc.h"
#include "kprcanvas.h"
#include <kdebug.h>
#include <kcommand.h>
#include "kprcommand.h"

KPresenterPageIface::KPresenterPageIface( KPrPage *_page, int pgnum )
    // Make up a nice DCOPObject name like "Document-0 Page-1".
    // ### But the page number will be stale if pages are added/removed/moved around....
    : DCOPObject( QCString( _page->kPresenterDoc()->dcopObject()->objId() ) + " Page-" + QString::number( pgnum ).latin1() )
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
    if( num >= (int)m_page->objNums())
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
    m_page->kPresenterDoc()->updateSideBarItem( m_page );
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

DCOPRef KPresenterPageIface::groupObjects()
{
    m_page->groupObjects();
    return selectedObject();
}

void KPresenterPageIface::ungroupObjects()
{
    //FIXME
    //m_page->ungroupObjects();
}

void KPresenterPageIface::raiseObjs()
{
    m_page->raiseObjs(false);
}

void KPresenterPageIface::lowerObjs()
{
    m_page->lowerObjs(false);
}

void KPresenterPageIface::sendBackward()
{
    m_page->lowerObjs(true);
}

void KPresenterPageIface::bringForward()
{
    m_page->raiseObjs(true);
}

void KPresenterPageIface::copyObjs()
{
    //FIXME
    //m_page->copyObjs();
}

void KPresenterPageIface::slotRepaintVariable()
{
    m_page->slotRepaintVariable();
}

void KPresenterPageIface::recalcPageNum()
{
    m_page->recalcPageNum();
}

void KPresenterPageIface::setPageTimer( int pageTimer )
{
    m_page->setPageTimer(pageTimer);
}

void KPresenterPageIface::setPageSoundEffect( bool soundEffect )
{
    m_page->setPageSoundEffect(soundEffect);
}

void KPresenterPageIface::setPageSoundFileName( const QString &fileName )
{
    m_page->setPageSoundFileName(fileName);
}

int KPresenterPageIface::backXFactor() const
{
    return m_page->getBackXFactor();
}

int KPresenterPageIface::backYFactor( ) const
{
    return m_page->getBackYFactor();
}

int KPresenterPageIface::pageTimer(  ) const
{
    return m_page->getPageTimer();
}

bool KPresenterPageIface::pageSoundEffect( ) const
{
    return m_page->getPageSoundEffect();
}

int KPresenterPageIface::backType() const
{
    return (int)m_page->getBackType();
}

int KPresenterPageIface::backView() const
{
    return (int)m_page->getBackView();
}

QColor KPresenterPageIface::backColor1() const
{
    return m_page->getBackColor1();
}

QColor KPresenterPageIface::backColor2() const
{
    return m_page->getBackColor2();
}

int KPresenterPageIface::backColorType() const
{
    return  (int)m_page->getBackColorType();
}

QString KPresenterPageIface::backPixFilename() const
{
    return m_page->getBackPictureKey().filename();
}

QString KPresenterPageIface::backClipFilename() const
{
    return m_page->getBackPictureKey().filename();
}

int KPresenterPageIface::pageEffect() const
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
    else if(effect=="BLINDS_HOR")
        m_page->setPageEffect(PEF_BLINDS_HOR);
    else if(effect=="BLINDS_VER")
        m_page->setPageEffect(PEF_BLINDS_VER);
    else if(effect=="BOX_IN")
        m_page->setPageEffect(PEF_BOX_IN);
    else if(effect=="BOX_OUT")
        m_page->setPageEffect(PEF_BOX_OUT);
    else if(effect=="CHECKBOARD_ACROSS")
        m_page->setPageEffect(PEF_CHECKBOARD_ACROSS);
    else if(effect=="CHECKBOARD_DOWN")
        m_page->setPageEffect(PEF_CHECKBOARD_DOWN);
    else if(effect=="COVER_DOWN")
        m_page->setPageEffect(PEF_COVER_DOWN);
    else if(effect=="UNCOVER_DOWN")
        m_page->setPageEffect(PEF_UNCOVER_DOWN);
    else if(effect=="COVER_UP")
        m_page->setPageEffect(PEF_COVER_UP);
    else if(effect=="UNCOVER_UP")
        m_page->setPageEffect(PEF_UNCOVER_UP);
    else if(effect=="COVER_LEFT")
        m_page->setPageEffect(PEF_COVER_LEFT);
    else if(effect=="UNCOVER_LEFT")
        m_page->setPageEffect(PEF_UNCOVER_LEFT);
    else if(effect=="COVER_RIGHT")
        m_page->setPageEffect(PEF_COVER_RIGHT);
    else if(effect=="UNCOVER_RIGHT")
        m_page->setPageEffect(PEF_UNCOVER_RIGHT);
    else if(effect=="COVER_LEFT_UP")
        m_page->setPageEffect(PEF_COVER_LEFT_UP);
    else if(effect=="UNCOVER_LEFT_UP")
        m_page->setPageEffect(PEF_UNCOVER_LEFT_UP);
    else if(effect=="COVER_LEFT_DOWN")
        m_page->setPageEffect(PEF_COVER_LEFT_DOWN);
    else if(effect=="UNCOVER_LEFT_DOWN")
        m_page->setPageEffect(PEF_UNCOVER_LEFT_DOWN);
    else if(effect=="COVER_RIGHT_UP")
        m_page->setPageEffect(PEF_COVER_RIGHT_UP);
    else if(effect=="UNCOVER_RIGHT_UP")
        m_page->setPageEffect(PEF_UNCOVER_RIGHT_UP);
    else if(effect=="COVER_RIGHT_DOWN")
        m_page->setPageEffect(PEF_COVER_RIGHT_DOWN);
    else if(effect=="UNCOVER_RIGHT_DOWN")
        m_page->setPageEffect(PEF_UNCOVER_RIGHT_DOWN);
    else if(effect=="DISSOLVE")
        m_page->setPageEffect(PEF_DISSOLVE);
    else if(effect=="STRIPS_LEFT_UP")
        m_page->setPageEffect(PEF_STRIPS_LEFT_UP);
    else if(effect=="STRIPS_LEFT_DOWN")
        m_page->setPageEffect(PEF_STRIPS_LEFT_DOWN);
    else if(effect=="STRIPS_RIGHT_UP")
        m_page->setPageEffect(PEF_STRIPS_RIGHT_UP);
    else if(effect=="STRIPS_RIGHT_DOWN")
        m_page->setPageEffect(PEF_STRIPS_RIGHT_DOWN);
    else if(effect=="MELTING")
        m_page->setPageEffect(PEF_MELTING);
    else if(effect=="RANDOM")
        m_page->setPageEffect(PEF_RANDOM);
    else
        kdDebug(33001) << "Error in setPageEffect" << endl;
}

QString KPresenterPageIface::pageEffectString( )const
{
    switch(m_page->getPageEffect( ))
    {
    case PEF_NONE:
        return QString("NONE");
        break;
    case PEF_CLOSE_HORZ:
        return QString("CLOSE_HORZ");
        break;
    case PEF_CLOSE_VERT:
        return QString("CLOSE_VERT");
        break;
    case PEF_CLOSE_ALL:
        return QString("CLOSE_ALL");
        break;
    case PEF_OPEN_HORZ:
        return QString("OPEN_HORZ");
        break;
    case PEF_OPEN_VERT:
        return QString("OPEN_VERT");
        break;
    case PEF_OPEN_ALL:
        return QString("OPEN_ALL");
        break;
    case PEF_INTERLOCKING_HORZ_1:
        return QString("INTERLOCKING_HORZ_1");
        break;
    case PEF_INTERLOCKING_HORZ_2:
        return QString("INTERLOCKING_HORZ_2");
        break;
    case PEF_INTERLOCKING_VERT_1:
        return QString("INTERLOCKING_VERT_1");
        break;
    case PEF_INTERLOCKING_VERT_2:
        return QString("INTERLOCKING_VERT_2");
        break;
    case PEF_SURROUND1:
        return QString("SURROUND1");
        break;
    case PEF_BLINDS_HOR:
        return QString("BLINDS_HOR");
        break;
    case PEF_BLINDS_VER:
        return QString("BLINDS_VER");
        break;
    case PEF_BOX_IN:
        return QString("BOX_IN");
        break;
    case PEF_BOX_OUT:
        return QString("BOX_OUT");
        break;
    case PEF_CHECKBOARD_ACROSS:
        return QString("CHECKBOARD_ACROSS");
        break;
    case PEF_COVER_DOWN:
        return QString("COVER_DOWN");
        break;
    case PEF_UNCOVER_DOWN:
        return QString("UNCOVER_DOWN");
        break;
    case PEF_COVER_UP:
        return QString("COVER_UP");
        break;
    case PEF_UNCOVER_UP:
        return QString("UNCOVER_UP");
        break;
    case PEF_COVER_LEFT:
        return QString("COVER_LEFT");
        break;
    case PEF_UNCOVER_LEFT:
        return QString("UNCOVER_LEFT");
        break;
    case PEF_COVER_RIGHT:
        return QString("COVER_RIGHT");
        break;
    case PEF_UNCOVER_RIGHT:
        return QString("UNCOVER_RIGHT");
        break;
    case PEF_COVER_LEFT_UP:
        return QString("COVER_LEFT_UP");
        break;
    case PEF_UNCOVER_LEFT_UP:
        return QString("UNCOVER_LEFT_UP");
        break;
    case PEF_COVER_LEFT_DOWN:
        return QString("COVER_LEFT_DOWN");
        break;
    case PEF_UNCOVER_LEFT_DOWN:
        return QString("UNCOVER_LEFT_DOWN");
        break;
    case PEF_COVER_RIGHT_UP:
        return QString("COVER_RIGHT_UP");
        break;
    case PEF_UNCOVER_RIGHT_UP:
        return QString("UNCOVER_RIGHT_UP");
        break;
    case PEF_COVER_RIGHT_DOWN:
        return QString("COVER_RIGHT_DOWN");
        break;
    case PEF_UNCOVER_RIGHT_DOWN:
        return QString("UNCOVER_RIGHT_DOWN");
        break;
    case PEF_DISSOLVE:
        return QString("DISSOLVE");
        break;
    case PEF_STRIPS_LEFT_UP:
        return QString("STRIPS_LEFT_UP");
        break;
    case PEF_STRIPS_LEFT_DOWN:
        return QString("STRIPS_LEFT_DOWN");
        break;
    case PEF_STRIPS_RIGHT_UP:
        return QString("STRIPS_RIGHT_UP");
        break;
    case PEF_STRIPS_RIGHT_DOWN:
        return QString("STRIPS_RIGHT_DOWN");
        break;
    case PEF_MELTING:
        return QString("MELTING");
        break;
    case PEF_RANDOM:
        return QString("RANDOM");
        break;
    default:
        kdDebug(33001) << "Page effect not defined!" << endl;
        break;
    }
    return QString("");
}

bool KPresenterPageIface::backUnbalanced() const
{
    return (int)m_page->getBackUnbalanced();
}

bool KPresenterPageIface::setRectSettings( int _rx, int _ry )
{
    return m_page->setRectSettings(_rx,_ry, RectValueCmd::All);
}

QString KPresenterPageIface::pageSoundFileName() const
{
    return m_page->getPageSoundFileName();
}

int KPresenterPageIface::pieAngle( int pieAngle ) const
{
    return m_page->getPieAngle(pieAngle);
}

int KPresenterPageIface::pieLength( int pieLength ) const
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
    m_page->kPresenterDoc()->updateSideBarItem( m_page );
    m_page->kPresenterDoc()->updatePresentationButton();
}

void KPresenterPageIface::changePicture( const QString & filename )
{
    // TODO: find a better parent
    m_page->changePicture(filename, 0);
}

//create a rectangle and return a dcop reference!
DCOPRef KPresenterPageIface::insertRectangle(int x, int y, int h, int w)
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();
    view->getCanvas()->insertRect( QRect(x,y,h,w) );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertEllipse( int x, int y, int h, int w )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertEllipse(QRect(x,y,h,w) );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertPie( int x, int y, int h, int w )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertPie( QRect(x,y,h,w) );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineH( int x, int y, int h, int w, bool rev )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertLineH( QRect(x,y,h,w), rev );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineV( int x, int y, int h, int w, bool rev )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertLineV( QRect(x,y,h,w), rev );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineD1( int x, int y, int h, int w, bool rev )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertLineD1( QRect(x,y,h,w), rev );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertLineD2( int x, int y, int h, int w, bool rev )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertLineD2( QRect(x,y,h,w), rev );
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertTextObject( int x, int y, int h, int w )
{
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertTextObject( QRect(x,y,h,w));
    return selectedObject();
}

DCOPRef KPresenterPageIface::insertPicture( const QString & file, int x, int y, int h, int w )
{
    m_page->setInsPictureFile(file);
    KPresenterView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return DCOPRef();

    view->getCanvas()->insertPicture( QRect(x,y,h,w));
    return selectedObject();
}

void KPresenterPageIface::setBackGroundColor1(const QColor &col)
{
    const QColor col2=backColor2();
    m_page->setBackColor( col, col2, m_page->getBackColorType(),
                          m_page->getBackUnbalanced(), backXFactor(), backYFactor() );
}

void KPresenterPageIface::setBackGroundColor2(const QColor &col)
{
    const QColor col1=backColor1();
    m_page->setBackColor( col1, col, m_page->getBackColorType(),
                          m_page->getBackUnbalanced(), backXFactor(), backYFactor() );
}

void KPresenterPageIface::setBackGroundColorType(const QString &type)
{
    BCType bctype;
    if(type=="PLAIN")
        bctype=BCT_PLAIN;
    else if(type=="GHORZ")
        bctype=BCT_GHORZ;
    else if(type=="GVERT")
        bctype=BCT_GVERT;
    else if(type=="GDIAGONAL1")
        bctype=BCT_GDIAGONAL1;
    else if(type=="GDIAGONAL2")
        bctype=BCT_GDIAGONAL2;
    else if(type=="GCIRCLE")
        bctype=BCT_GCIRCLE;
    else if(type=="GRECT")
        bctype=BCT_GRECT;
    else if(type=="GPIPECROSS")
        bctype=BCT_GPIPECROSS;
    else if(type=="GPYRAMID")
        bctype=BCT_GPYRAMID;
    else
    {
        kdDebug(33001)<<"Error KPresenterPageIface::setBackGroundColorType" << endl;
        return;
    }

    m_page->setBackColor( backColor1(), backColor2(), bctype,m_page->getBackUnbalanced(),
                          backXFactor(), backYFactor() );
}

int KPresenterPageIface::rndY() const
{
    int ret=-1;
    m_page->getRndY( ret );
    return ret;
}

int KPresenterPageIface::rndX() const
{
    int ret=-1;
    m_page->getRndX( ret );
    return ret;
}

void KPresenterPageIface::deSelectAllObj()
{
    m_page->deSelectAllObj();
}

bool KPresenterPageIface::oneObjectTextExist() const
{
    return m_page->oneObjectTextExist();
}

bool KPresenterPageIface::isOneObjectSelected() const
{
    return m_page->isOneObjectSelected();
}

bool KPresenterPageIface::haveASelectedPartObj() const
{
    return m_page->haveASelectedPartObj();
}

bool KPresenterPageIface::haveASelectedGroupObj() const
{
    return m_page->haveASelectedGroupObj();
}

void KPresenterPageIface::rotateAllObjSelected(float _newAngle)
{
    KCommand *cmd= m_page->rotateSelectedObjects( _newAngle);
    delete cmd;
}

void KPresenterPageIface::stickyAllObjSelected(bool sticky)
{
    KCommand *cmd=m_page->stickyObj(sticky, m_page);
    delete cmd;
}

void KPresenterPageIface::moveAllObjectSelected( int diffx,int diffy)
{
    KPresenterView *_view=m_page->kPresenterDoc()->firstView();
    if(_view)
    {
        KCommand *cmd=m_page->moveObject(_view, diffx,diffy);
        delete cmd;
    }
}

void KPresenterPageIface::deleteAllObjectSelected()
{
    KCommand * cmd=m_page->deleteSelectedObjects();
    delete cmd;
}

bool KPresenterPageIface::hasHeader()const
{
    return m_page->hasHeader();
}

bool KPresenterPageIface::hasFooter()const
{
    return m_page->hasFooter();
}

void KPresenterPageIface::setHeader( bool b )
{
    m_page->setHeader( b );
}

void KPresenterPageIface::setFooter( bool b )
{
    m_page->setFooter( b );
}


#if MASTERPAGE
void KPresenterPageIface::setUseMasterBackground( bool useMasterBackground )
{
    m_page->setUseMasterBackground( useMasterBackground );
}

bool KPresenterPageIface::useMasterBackground() const
{
    return m_page->useMasterBackground();
}
#endif
