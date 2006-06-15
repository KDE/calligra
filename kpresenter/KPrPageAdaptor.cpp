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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPageAdaptor.h"

#include "KPrPage.h"
#include "KPrTextObject.h"
#include "KPrView.h"
#include "KPrBackground.h"
#include <kapplication.h>
#include "KPrDocument.h"
#include "KPrCanvas.h"
#include <kdebug.h>
#include <kcommand.h>
#include "KPrCommand.h"
#include <KoPoint.h>
#include <KoRect.h>
//Added by qt3to4:
#include <Q3CString>

KPrPageAdaptor::KPrPageAdaptor( KPrPage *_page, int pgnum )
{
    m_page = _page;
    // Make up a nice D-Bus object path name like "Document/0/Page/1".
    // ### But the page number will be stale if pages are added/removed/moved around....
    QString objectPath = _page->kPresenterDoc()->dbusObject()->objectName()
        + "/Page/" + QString::number( pgnum ).toLatin1();
    QDBus::sessionBus().registerObject(objectPath, _page, QDBusConnection::ExportSlots);
}

QString KPrPageAdaptor::textObject( int num )
{
    KPrTextObject * textObj=m_page->textFrameSet(num);
    if(textObj)
        return textObj->dbusObject()->objectName();
    return QString();
}

//return a reference to selected object
QString KPrPageAdaptor::selectedObject( )
{
    KPrObject * obj=m_page->getSelectedObj();
    if(obj)
        return obj->dbusObject()->objectName();
    return QString();
}


int KPrPageAdaptor::numTextObject() const
{
    return m_page->numTextObject();
}

QString KPrPageAdaptor::object( int num )
{
    if( num >= (int)m_page->objNums())
        return QString();
    return m_page->getObject(num)->dbusObject()->objectName();
}

QString KPrPageAdaptor::manualTitle()const
{
    return m_page->manualTitle();
}

void KPrPageAdaptor::insertManualTitle(const QString & title)
{
    m_page->insertManualTitle(title);
    m_page->kPresenterDoc()->updateSideBarItem( m_page );
}

QString KPrPageAdaptor::pageTitle( const QString &_title ) const
{
    return m_page->pageTitle(_title);
}

void KPrPageAdaptor::setNoteText( const QString &_text )
{
    m_page->setNoteText(_text);
}

QString KPrPageAdaptor::noteText( )const
{
    return m_page->noteText();
}

unsigned int KPrPageAdaptor::objNums() const
{
    return m_page->objNums();
}

int KPrPageAdaptor::numSelected() const
{
    return m_page->numSelected();
}

QString KPrPageAdaptor::groupObjects()
{
    m_page->groupObjects();
    return selectedObject();
}

void KPrPageAdaptor::ungroupObjects()
{
    //FIXME
    //m_page->ungroupObjects();
}

void KPrPageAdaptor::raiseObjs()
{
    m_page->raiseObjs(false);
}

void KPrPageAdaptor::lowerObjs()
{
    m_page->lowerObjs(false);
}

void KPrPageAdaptor::sendBackward()
{
    m_page->lowerObjs(true);
}

void KPrPageAdaptor::bringForward()
{
    m_page->raiseObjs(true);
}

void KPrPageAdaptor::copyObjs()
{
    //FIXME
    //m_page->copyObjs();
}

void KPrPageAdaptor::slotRepaintVariable()
{
    m_page->slotRepaintVariable();
}

void KPrPageAdaptor::recalcPageNum()
{
    m_page->recalcPageNum();
}

void KPrPageAdaptor::setPageTimer( int pageTimer )
{
    m_page->setPageTimer(pageTimer);
}

void KPrPageAdaptor::setPageSoundEffect( bool soundEffect )
{
    m_page->setPageSoundEffect(soundEffect);
}

void KPrPageAdaptor::setPageSoundFileName( const QString &fileName )
{
    m_page->setPageSoundFileName(fileName);
}

int KPrPageAdaptor::backXFactor() const
{
    return m_page->getBackXFactor();
}

int KPrPageAdaptor::backYFactor( ) const
{
    return m_page->getBackYFactor();
}

int KPrPageAdaptor::pageTimer(  ) const
{
    return m_page->getPageTimer();
}

bool KPrPageAdaptor::pageSoundEffect( ) const
{
    return m_page->getPageSoundEffect();
}

int KPrPageAdaptor::backType() const
{
    return (int)m_page->getBackType();
}

int KPrPageAdaptor::backView() const
{
    return (int)m_page->getBackView();
}

QColor KPrPageAdaptor::backColor1() const
{
    return m_page->getBackColor1();
}

QColor KPrPageAdaptor::backColor2() const
{
    return m_page->getBackColor2();
}

int KPrPageAdaptor::backColorType() const
{
    return  (int)m_page->getBackColorType();
}

QString KPrPageAdaptor::backPixFilename() const
{
    return m_page->getBackPictureKey().filename();
}

QString KPrPageAdaptor::backClipFilename() const
{
    return m_page->getBackPictureKey().filename();
}

int KPrPageAdaptor::pageEffect() const
{
    return (int)m_page->getPageEffect();
}

void KPrPageAdaptor::setPageEffect(const QString &effect )
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
        kDebug(33001) << "Error in setPageEffect" << endl;
}

QString KPrPageAdaptor::pageEffectString( )const
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
        kDebug(33001) << "Page effect not defined!" << endl;
        break;
    }
    return QString("");
}

bool KPrPageAdaptor::backUnbalanced() const
{
    return (int)m_page->getBackUnbalanced();
}

QString KPrPageAdaptor::pageSoundFileName() const
{
    return m_page->getPageSoundFileName();
}

QRect KPrPageAdaptor::pageRect() const
{
    return m_page->getZoomPageRect();
}

bool KPrPageAdaptor::isSlideSelected()
{
    return m_page->isSlideSelected();
}

void KPrPageAdaptor::slideSelected(bool _b)
{
    m_page->slideSelected(_b);
    m_page->kPresenterDoc()->updateSideBarItem( m_page );
    m_page->kPresenterDoc()->updatePresentationButton();
}

void KPrPageAdaptor::changePicture( const QString & filename )
{
    // TODO: find a better parent
    m_page->changePicture(filename, 0);
}

//create a rectangle and return a dbus reference!
QString KPrPageAdaptor::insertRectangle(double x, double y, double h, double w)
{
    KPrView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return QString();
    view->getCanvas()->insertRect( KoRect( x, y, h, w ) );
    return selectedObject();
}

QString KPrPageAdaptor::insertEllipse( double x, double y, double h, double w )
{
    KPrView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return QString();

    view->getCanvas()->insertEllipse( KoRect( x, y, h, w ) );
    return selectedObject();
}

QString KPrPageAdaptor::insertPie( double x, double y, double h, double w )
{
    KPrView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return QString();

    view->getCanvas()->insertPie( KoRect( x, y, h, w ) );
    return selectedObject();
}

QString KPrPageAdaptor::insertLine( double x1, double y1, double x2, double y2 )
{
    KPrView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return QString();

    view->getCanvas()->insertLine( KoPoint( x1, y1), KoPoint( x2, y2 ) );
    return selectedObject();
}

QString KPrPageAdaptor::insertTextObject( double x, double y, double h, double w )
{
    KPrView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return QString();

    view->getCanvas()->insertTextObject( KoRect( x, y, h, w ) );
    return selectedObject();
}

QString KPrPageAdaptor::insertPicture( const QString & file, double x, double y, double h, double w )
{
    m_page->setInsPictureFile(file);
    KPrView *view=m_page->kPresenterDoc()->firstView();
    m_page->kPresenterDoc()->deSelectAllObj();
    if ( !view  )
        return QString();

    view->getCanvas()->insertPicture( KoRect( x, y, h, w ) );
    return selectedObject();
}

void KPrPageAdaptor::setBackGroundColor1(const QColor &col)
{
    const QColor col2=backColor2();
    m_page->setBackColor( col, col2, m_page->getBackColorType(),
                          m_page->getBackUnbalanced(), backXFactor(), backYFactor() );
}

void KPrPageAdaptor::setBackGroundColor2(const QColor &col)
{
    const QColor col1=backColor1();
    m_page->setBackColor( col1, col, m_page->getBackColorType(),
                          m_page->getBackUnbalanced(), backXFactor(), backYFactor() );
}

void KPrPageAdaptor::setBackGroundColorType(const QString &type)
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
        kDebug(33001)<<"Error KPrPageAdaptor::setBackGroundColorType" << endl;
        return;
    }

    m_page->setBackColor( backColor1(), backColor2(), bctype,m_page->getBackUnbalanced(),
                          backXFactor(), backYFactor() );
}

void KPrPageAdaptor::deSelectAllObj()
{
    m_page->deSelectAllObj();
}

bool KPrPageAdaptor::oneObjectTextExist() const
{
    return m_page->oneObjectTextExist();
}

bool KPrPageAdaptor::isOneObjectSelected() const
{
    return m_page->isOneObjectSelected();
}

bool KPrPageAdaptor::haveASelectedPartObj() const
{
    return m_page->haveASelectedPartObj();
}

bool KPrPageAdaptor::haveASelectedGroupObj() const
{
    return m_page->haveASelectedGroupObj();
}

void KPrPageAdaptor::rotateAllObjSelected(float _newAngle)
{
    KCommand *cmd= m_page->rotateSelectedObjects( _newAngle);
    delete cmd;
}

void KPrPageAdaptor::moveAllObjectSelected( int diffx,int diffy)
{
    KPrView *_view=m_page->kPresenterDoc()->firstView();
    if(_view)
    {
        KCommand *cmd=m_page->moveObject(_view, diffx,diffy);
        delete cmd;
    }
}

void KPrPageAdaptor::deleteAllObjectSelected()
{
    KCommand * cmd=m_page->deleteSelectedObjects();
    delete cmd;
}

bool KPrPageAdaptor::hasHeader()const
{
    return m_page->hasHeader();
}

bool KPrPageAdaptor::hasFooter()const
{
    return m_page->hasFooter();
}

void KPrPageAdaptor::setHeader( bool b )
{
    m_page->setHeader( b );
}

void KPrPageAdaptor::setFooter( bool b )
{
    m_page->setFooter( b );
}


void KPrPageAdaptor::setUseMasterBackground( bool useMasterBackground )
{
    m_page->setUseMasterBackground( useMasterBackground );
}

bool KPrPageAdaptor::useMasterBackground() const
{
    return m_page->useMasterBackground();
}

void KPrPageAdaptor::setDisplayObjectFromMasterPage( bool _b )
{
    m_page->setDisplayObjectFromMasterPage( _b );
}

bool KPrPageAdaptor::displayObjectFromMasterPage() const
{
    return m_page->displayObjectFromMasterPage();
}

void KPrPageAdaptor::setDisplayBackground( bool _b )
{
    m_page->setDisplayBackground( _b );
}

bool KPrPageAdaptor::displayBackground() const
{
    return m_page->displayBackground();
}
