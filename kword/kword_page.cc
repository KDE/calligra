/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <kfiledialog.h>
#include <kapp.h>
#include <kmimemagic.h>
#include <klocale.h>
#include <qpoint.h>
#include <kurl.h>

#include <koRuler.h>
#include <koPartSelectDia.h>
#include <koQueryTrader.h>

#include "kword_doc.h"
#include "kword_page.h"
#include "kword_page.moc"
#include "kword_view.h"
#include "footnote.h"
#include "clipbrd_dnd.h"
#include "char.h"
#include "kword_frame.h"
#include "framedia.h"
#include "font.h"
#include "docstruct.h"
#include "variabledlgs.h"
#include "serialletter.h"
#include "contents.h"

#include <qtextstream.h>
#include <qevent.h>
#include <qkeycode.h>
#include <qclipboard.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qregion.h>
#include <qdropsite.h>
#include <qpaintdevice.h>
#include <qpicture.h>
#include <kmessagebox.h>
#include <qfile.h>
#include <kdebug.h>

#include <stdlib.h>
#include <math.h>

/******************************************************************/
/* Class: KWPage
 * This class is responsible for the rendering of the frames to
 * the screen as well as the interaction with the user via mouse
 * and keyboard.
 ******************************************************************/

/*================================================================*/
KWPage::KWPage( QWidget *parent, KWordDocument *_doc, KWordGUI *_gui )
    : QScrollView( parent, "", WNorthWestGravity ), format( _doc ),
      blinkTimer( this ), scrollTimer( this ), formatTimer( this ),
      inputTimer( this ), cachedParag( 0L ),
      cachedContentsPos( QPoint( -1, -1 ) ), _setErase( TRUE ),
      redrawOnlyCurrFrameset( FALSE ), _scrolled( FALSE ), currFrameSet( -1 ),
      scrollDummy( 0 )
{
    setKeyCompression( TRUE );
    setFocusPolicy( QWidget::StrongFocus );
    viewport()->setFocusProxy( this );

    trows = 7;
    tcols = 5;
    twid = TblAuto;
    thei = TblAuto;

    editNum = -1;
    recalcingText = FALSE;
    maybeDrag = FALSE;

    doc = _doc;
    gui = _gui;

    redrawAllWhileScrolling = FALSE;

    if ( doc )
        calcVisiblePages();

    fc = new KWFormatContext( doc, 1 );

    fc->init( doc->getFirstParag( 0 ) );

    fc->cursorGotoLine( 0 );

    oldFc = new KWFormatContext( doc, 1 );

    viewport()->setCursor( ibeamCursor );
    mousePressed = FALSE;
    setMouseTracking( TRUE );
    viewport()->setMouseTracking( TRUE );

    inKeyEvent = FALSE;
    recalcAll = FALSE;

    recalcWholeText();
    mouseMode = MM_EDIT;

    setupMenus();
    inKeyEvent = TRUE;
    setMouseMode( MM_EDIT );
    inKeyEvent = FALSE;

    oldMx = oldMy = 0;
    deleteMovingRect = TRUE;

    hiliteFrameSet = -1;

    frameDia = 0;
    pixmap_name = "";
    doRaster = TRUE;

    currFindParag = 0L;
    currFindPos = 0;
    currFindFS = 0;
    currFindLen = 0;

    fc->init( fc->getParag(), TRUE );
    recalcWholeText( );
    recalcCursor( FALSE, 0, fc );

    editMode = EM_NONE;

    doc->setSelStart( *fc );
    doc->setSelEnd( *fc );
    doc->setSelection( FALSE );

    curTable = 0L;

    setAcceptDrops( FALSE ); // #### enable again when drop event processing has been rewrittem
    viewport()->setAcceptDrops( FALSE ); // #### enable again when drop event processing has been rewrittem

    cursorIsVisible = false;

    connect( &blinkTimer, SIGNAL( timeout() ), this, SLOT( blinkCursor() ) );
    connect( &scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
    startBlinkCursor();

    resizeContents( ptPaperWidth(), doc->getPages() * ptPaperHeight() );
    connect( this, SIGNAL( contentsMoving( int, int ) ), this, SLOT( contentsWillMove( int, int ) ) );

    viewport()->setBackgroundColor( white );
    viewport()->setBackgroundMode( NoBackground );

    repaintScreen( TRUE );
    setFrameStyle( QFrame::NoFrame );
    setLineWidth( 0 );

    connect( &formatTimer, SIGNAL( timeout() ),
             this, SLOT( formatMore() ) );
    connect( &inputTimer, SIGNAL( timeout() ),
             this, SLOT( noInput() ) );
    formatFC = new KWFormatContext( doc, 1 );

    pasteLaterData = 0;

    // HACK
    if ( doc && doc->getContents()->hasContents() ) {
        KWParag *p = ( (KWTextFrameSet*)doc->getFrameSet( 0 ) )->getFirstParag();
        QList<KoTabulator> tabList;
        KoTabulator *tab = new KoTabulator;
        tab->ptPos = ( (KWTextFrameSet*)doc->getFrameSet( 0 ) )->getFrame( 0 )->width() - 10;
        tab->mmPos = POINT_TO_MM( tab->ptPos );
        tab->inchPos = POINT_TO_INCH( tab->ptPos );
        tab->type = T_RIGHT;
        tabList.append( tab );
        bool didCont = FALSE;
        while ( p ) {
            if ( p->getInfo() == KWParag::PI_CONTENTS ) {
                didCont = TRUE;
                p->getParagLayout()->setTabList( &tabList );
            } else if ( didCont )
                break;
            p = p->getNext();
        }
    }
}

KWPage::~KWPage()
{
  delete oldFc;
  delete formatFC;
  delete fc;
  delete mm_menu;
  delete frame_edit_menu;
  selectAllFrames( FALSE );
}
/*================================================================*/
unsigned int KWPage::ptLeftBorder()
{
    return doc->getPTLeftBorder();
}

/*================================================================*/
unsigned int KWPage::ptRightBorder()
{
    return doc->getPTRightBorder();
}

/*================================================================*/
unsigned int KWPage::ptTopBorder()
{
    return doc->getPTTopBorder();
}

/*================================================================*/
unsigned int KWPage::ptBottomBorder()
{
    return doc->getPTBottomBorder();
}

/*================================================================*/
unsigned int KWPage::ptPaperWidth()
{
    return doc->getPTPaperWidth();
}

/*================================================================*/
unsigned int KWPage::ptPaperHeight()
{
    return doc->getPTPaperHeight();
}

/*================================================================*/
unsigned int KWPage::ptColumnWidth()
{
    return doc->getPTColumnWidth();
}

/*================================================================*/
unsigned int KWPage::ptColumnSpacing()
{
    return doc->getPTColumnSpacing();
}

/*================================================================*/
void KWPage::init()
{
    if ( fc->getParag() ) {
        gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
        gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
        gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
        gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
        gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                         fc->getParag()->getParagLayout()->getRightBorder(),
                                         fc->getParag()->getParagLayout()->getTopBorder(),
                                         fc->getParag()->getParagLayout()->getBottomBorder() );
        setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
        setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
        gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                            getFrame( fc->getFrame() - 1 )->x() );
        gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
        gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
    }
}

/*================================================================*/
void KWPage::vmmEdit( int mx, int my )
{
    int frameset = doc->getFrameSet( mx, my );

    // only if we are in the _same_ frameset as before!!
    if ( frameset != -1 && frameset == static_cast<int>( fc->getFrameSet() ) - 1 &&
         doc->getFrameSet( frameset )->getFrameType() == FT_TEXT ) {
            doSelect(mx, my);
            if ( scrollTimer.isActive() )
                return;

            scrollTimer.start( 100, FALSE );
        }
}

/*================================================================*/
void KWPage::vmmEditFrameSizeAll( int mx, int my )
{
    if ( (int)mx != oldMy || (int)my != oldMy ) {
        QList<KWGroupManager> undos, updates;
        undos.setAutoDelete( FALSE );
        updates.setAutoDelete( FALSE );

        QPainter p;
        p.begin( viewport() );
        p.setRasterOp( NotROP );
        p.setPen( black );
        p.setBrush( NoBrush );

        KWFrameSet *frameset;
        KWFrame *frame;
        for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
            frameset = doc->getFrameSet( i );
            if ( doc->getProcessingType() == KWordDocument::WP && i == 0 ||
                 frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() != FI_BODY ) continue;

            for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
                frame = frameset->getFrame( j );
                if ( frame->isSelected() ) {
                    if ( frameset->getGroupManager() ) {
                        if ( updates.findRef( frameset->getGroupManager() ) == -1 )
                            updates.append( frameset->getGroupManager() );
                    } else {
                        if ( deleteMovingRect )
                            p.drawRect( frame->x() - contentsX(), frame->y() - contentsY(), frame->width(),
                                        frame->height() );
                        frame->moveBy( mx - oldMx, my - oldMy );
                        if ( frame->x() < 0 ||
                             frame->y() < getPageOfRect( QRect( frame->x(), frame->y(), frame->width(),
                                                                frame->height() ) ) *
                             static_cast<int>( ptPaperHeight() ) ||
                             frame->right() > static_cast<int>( ptPaperWidth() ) ||
                             frame->bottom() > ( getPageOfRect( QRect( frame->x(), frame->y(), frame->width(),
                                                                       frame->height() ) ) + 1 ) *
                             static_cast<int>( ptPaperHeight() ) )
                            frame->moveBy( oldMx - mx, oldMy - my );
                        p.drawRect( frame->x() - contentsX(), frame->y() - contentsY(), frame->width(),
                                    frame->height() );
                    }
                }
            }
        }

        if ( !updates.isEmpty() ) {
            for ( unsigned int i = 0; i < updates.count(); i++ ) {
                KWGroupManager *grpMgr = updates.at( i );
                for ( unsigned k = 0; k < grpMgr->getNumCells(); k++ ) {
                    frame = grpMgr->getCell( k )->frameSet->getFrame( 0 );
                    if ( deleteMovingRect )
                        p.drawRect( frame->x() - contentsX(), frame->y() - contentsY(), frame->width(),
                                    frame->height() );
                    frame->moveBy( mx - oldMx, my - oldMy );
                    if ( frame->x() < 0 || frame->right() > static_cast<int>( ptPaperWidth() ) || frame->y() < 0 ) {
                        if ( undos.findRef( grpMgr ) == -1 )
                            undos.append( grpMgr );
                    }
                    p.drawRect( frame->x() - contentsX(), frame->y() - contentsY(), frame->width(), frame->height() );
                }
            }
        }

        if ( !undos.isEmpty() ) {
            for ( unsigned int i = 0; i < undos.count(); i++ ) {
                undos.at( i )->drawAllRects( p, contentsX(), contentsY() );
                undos.at( i )->moveBy( oldMx - mx, oldMy - my );
                undos.at( i )->drawAllRects( p, contentsX(), contentsY() );
            }
        }

        p.end();
    }
}

/*================================================================*/
void KWPage::vmmEditFrameResize( unsigned int mx, unsigned int my , bool top, bool bottom, bool left, bool right ) {

    KWFrame *frame = doc->getFirstSelectedFrame();
    if ( doc->getProcessingType() == KWordDocument::WP  && frame->getFrameSet() == doc->getFrameSet(0))
        return;

    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );


    if ( deleteMovingRect ){ // erase old one.
        unsigned int drawX, drawWidth, drawY, drawHeight;
        drawX=frame->left() - contentsX();
        drawWidth=frame->width();
        drawY=frame->top() - contentsY();
        drawHeight=frame->height();
        if(frame->getFrameSet()->getGroupManager()) { // is table
            if(!(top || bottom)) { /// full height.
                drawY=frame->getFrameSet()->getGroupManager()->getBoundingRect().y()- contentsY();
                drawHeight=frame->getFrameSet()->getGroupManager()->getBoundingRect().height();
            } else if(!(left || right)) { // full width.
                drawX=frame->getFrameSet()->getGroupManager()->getBoundingRect().x() - contentsX();
                drawWidth=frame->getFrameSet()->getGroupManager()->getBoundingRect().width();
            }
        }
        p.drawRect( drawX, drawY, drawWidth, drawHeight );
    }

    unsigned int newX1 = frame->left();
    unsigned int newY1 = frame->top();
    unsigned int newX2 = frame->right();
    unsigned int newY2 = frame->bottom();
    if(top && newY1 != my) {
        bool move=true;
        if(isAFooter(frame->getFrameSet()->getFrameInfo())) move=false;
        if(newY2-my < minFrameHeight+5) my=newY2-minFrameHeight-5;
        if(my < (unsigned int)( frame->getPageNum() * ptPaperHeight()))
            my = frame->getPageNum() * ptPaperHeight();

        if(move) newY1=my;
    } else if(bottom && newY2 != my) {
        bool move=true;
        if(isAHeader(frame->getFrameSet()->getFrameInfo())) move=false;
        if(my-newY1 < minFrameHeight+5) my=newY1+minFrameHeight+5;
        if(my >= (unsigned int)((frame->getPageNum()+1) * ptPaperHeight()))
            my = (frame->getPageNum()+1) * ptPaperHeight();

        if(move) newY2=my;
    }

    if(left && newX1 != mx) {
        bool move=true;
        if(isAHeader(frame->getFrameSet()->getFrameInfo())) move=false;
        if(isAFooter(frame->getFrameSet()->getFrameInfo())) move=false;
        if(newX2-mx < minFrameWidth) mx=newX2-minFrameHeight-5;
        if(mx <= 0) mx=0;

        if(move) newX1=mx;
    } else if(right && newX2 != mx) {
        bool move=true;
        if(isAHeader(frame->getFrameSet()->getFrameInfo())) move=false;
        if(isAFooter(frame->getFrameSet()->getFrameInfo())) move=false;
        if(mx-newX1 < minFrameWidth) mx=newX1+minFrameHeight+5;
        if(mx > static_cast<unsigned int> (ptPaperWidth()))
            mx = ptPaperWidth();

        if(move) newX2=mx;
    }
    frame->setLeft(newX1);
    frame->setTop(newY1);
    frame->setRight(newX2);
    frame->setBottom(newY2);

    int drawX, drawWidth, drawY, drawHeight;
    drawX=frame->left() - contentsX();
    drawWidth=frame->width();
    drawY=frame->top() - contentsY();
    drawHeight=frame->height();
    if(frame->getFrameSet()->getGroupManager()) { // is table
        if(!(top || bottom)) { /// full height.
            drawY=frame->getFrameSet()->getGroupManager()->getBoundingRect().y()- contentsY();
            drawHeight=frame->getFrameSet()->getGroupManager()->getBoundingRect().height();
        } else if(!(left || right)) { // full width.
            drawX=frame->getFrameSet()->getGroupManager()->getBoundingRect().x() - contentsX();
            drawWidth=frame->getFrameSet()->getGroupManager()->getBoundingRect().width();
        }
    }
    p.drawRect( drawX, drawY, drawWidth, drawHeight );
    p.end();
}

/*================================================================*/
void KWPage::vmmCreate( int mx, int my )
{
    mx -= contentsX();
    my -= contentsY();

    if ( doRaster ) {
        mx = ( mx / doc->getRastX() ) * doc->getRastX();
        my = ( my / doc->getRastY() ) * doc->getRastY();
    }

    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );

    if ( deleteMovingRect ) {
        if ( useAnchor ) {
            p.drawLine( anchor->getOrigin(), insRect.topLeft() );
        }
        p.drawRect( insRect );
    }
    else {
        if ( useAnchor ) {
            KWGroupManager *grpMgr;

            grpMgr = new KWGroupManager( doc );
            insertAnchor( grpMgr );
            anchor = grpMgr;
        };
    }
    insRect.setWidth( insRect.width() + mx - oldMx );
    insRect.setHeight( insRect.height() + my - oldMy );

    if ( insRect.normalize().x() + static_cast<int>( contentsX() ) < 0 || insRect.normalize().y()
         + static_cast<int>( contentsY() ) <
         getPageOfRect( QRect( insRect.normalize().x() + static_cast<int>( contentsX() ), insRect.normalize().y()
                               + static_cast<int>( contentsY() ),
                               insRect.normalize().width(), insRect.normalize().height() ) )
         * static_cast<int>( ptPaperHeight() ) ||
         insRect.normalize().right() + static_cast<int>( contentsX() ) > static_cast<int>( ptPaperWidth() ) ||
         insRect.normalize().bottom() + static_cast<int>( contentsY() ) > ( getPageOfRect(
                QRect( insRect.normalize().x() +
                static_cast<int>( contentsX() ),
                insRect.normalize().y() + static_cast<int>( contentsY() ),
                insRect.normalize().width(), insRect.normalize().height() ) ) + 1 ) *
                static_cast<int>( ptPaperHeight() ) ) {

        insRect.setWidth( insRect.width() - ( mx - oldMx ) );
        insRect.setHeight( insRect.height() - ( my - oldMy ) );
    }

    if ( useAnchor ) {
        p.drawLine( anchor->getOrigin(), insRect.topLeft() );
    }
    p.drawRect( insRect );
    p.end();
    oldMx = mx;
    oldMy = my;
    deleteMovingRect = TRUE;
}

/*================================================================*/
void KWPage::viewportMouseMoveEvent( QMouseEvent *e )
{
    if ( maybeDrag ) {
        maybeDrag = FALSE;
        mousePressed = FALSE;
        mouseMoved = FALSE;
        startDrag();
        return;
    }

    mouseMoved = TRUE;
    int mx = e->x() + contentsX();
    int my = e->y() + contentsY();

    if ( mousePressed ) {
        switch ( mouseMode ) {
        case MM_EDIT:
            vmmEdit( mx, my );
            break;
        case MM_EDIT_FRAME: {
            mx = ( mx / doc->getRastX() ) * doc->getRastX();
            my = ( my / doc->getRastY() ) * doc->getRastY();

            switch ( viewport()->cursor().shape() ) {
            case SizeAllCursor:
                vmmEditFrameSizeAll( mx, my );
                break;
            default: break;
            }
            deleteMovingRect = TRUE;
            oldMx = mx; oldMy = my;
        } break;
        case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
        case MM_CREATE_TABLE: case MM_CREATE_FORMULA: case MM_CREATE_KSPREAD_TABLE:
            vmmCreate( mx, my );
        default: break;
        }
    } else {
        switch ( mouseMode ) {
        case MM_EDIT_FRAME:
            viewport()->setCursor( doc->getMouseCursor( mx, my ) );
            break;
        default: break;
        }
    }
    doRaster = TRUE;
}

/*================================================================*/
bool KWPage::vmpEdit( int mx, int my )
{
    showCursor(false);

    int frameset = doc->getFrameSet( mx, my );

    if ( frameset != -1 && doc->getFrameSet( frameset )->getFrameType() == FT_FORMULA ) {
        gui->getView()->showFormulaToolbar( TRUE );
        KWFormulaFrameSet *fs = dynamic_cast<KWFormulaFrameSet*>( doc->getFrameSet( frameset ) );
        fs->activate( this );
        editNum = frameset;
        KWFormat *f = fs->getFormat();
        fc->apply( *f );
        //formatChanged( *f, FALSE );
        gui->getView()->setFormat( *f, TRUE );
        //delete f;

        return TRUE;
    }
    if ( frameset != -1 && doc->getFrameSet( frameset )->getFrameType() == FT_PART ) {
        KWPartFrameSet *fs = dynamic_cast<KWPartFrameSet*>( doc->getFrameSet( frameset ) );
        fs->activate( gui->getView() );
        editNum = frameset;
        return TRUE;
    }

    selectedFrameSet = selectedFrame = -1;
    if ( frameset != -1 && doc->getFrameSet( frameset )->getFrameType() == FT_TEXT ) {
        fc->setFrameSet( frameset + 1 );

        fc->cursorGotoPixelLine( mx, my );
        fc->cursorGotoPixelInLine( mx, my );

        scrollToCursor();
        showCursor( true );

        if ( isInSelection( fc ) ) {
            maybeDrag = TRUE;
            return FALSE;
        } else {
            if ( doc->has_selection() ) {
                QPainter _painter;
                _painter.begin( viewport() );
                doc->drawSelection( _painter, contentsX(), contentsY() );
                doc->setSelection( FALSE );
                _painter.end();
            }
        }

        startKeySelection();

        if ( doc->getProcessingType() == KWordDocument::DTP ) {
            int frame = doc->getFrameSet( frameset )->getFrame( mx, my );
            if ( frame != -1 ) {
                if ( doc->getProcessingType() == KWordDocument::DTP )
                    setRuler2Frame( frameset, frame );
            }
            selectedFrame = frame;
            selectedFrameSet = frameset;
        }

        gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

        if ( fc->getParag() ) {
            gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
            gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
            gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
            gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
            gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                             fc->getParag()->getParagLayout()->getRightBorder(),
                                             fc->getParag()->getParagLayout()->getTopBorder(),
                                             fc->getParag()->getParagLayout()->getBottomBorder() );
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
            gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
            gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
        }
    } else {
        showCursor( true );
    }

    return TRUE;
}

/*================================================================*/
void KWPage::vmpEditFrame( QMouseEvent *e, int mx, int my )
{
    if ( e ) {
        // only simulate selection - we do real selection below
        int currentSelection = doc->selectFrame( mx, my, TRUE );

        KWFrameSet *fs = doc->getFrameSet( doc->getFrameSet( mx, my ) );
        if ( currentSelection != 0 && ( e->state() & ShiftButton ) && fs->getGroupManager() ) { // is table and we hold shift
            fs->getGroupManager()->selectUntil( fs );
            curTable = doc->getFrameSet( doc->getFrameSet( mx, my ) )->getGroupManager();
        } else if ( currentSelection == 0 ) { // none selected
            selectAllFrames( FALSE );
        } else if ( currentSelection == 1 ) { // 1 selected
            if ( !( e->state() & ControlButton || e->state() & ShiftButton ) )
                selectAllFrames( FALSE );
            selectFrame( mx, my, TRUE );
            curTable = doc->getFrameSet( doc->getFrameSet( mx, my ) )->getGroupManager();
        } else if ( currentSelection == 2 ) { // was allready selected
            if ( e->state() & ControlButton || e->state() & ShiftButton ) {
                selectFrame( mx, my, FALSE );
                curTable = doc->getFrameSet( doc->getFrameSet( mx, my ) )->getGroupManager();
            } else if ( viewport()->cursor().shape() != SizeAllCursor ) {
                selectAllFrames( FALSE );
                selectFrame( mx, my, TRUE );
                curTable = doc->getFrameSet( doc->getFrameSet( mx, my ) )->getGroupManager();
            }
        }
    }

    mousePressed = FALSE;
    mouseMoveEvent( e );
    mousePressed = TRUE;

    deleteMovingRect = FALSE;
    mx = ( mx / doc->getRastX() ) * doc->getRastX();
    oldMx = mx;
    my = ( my / doc->getRastX() ) * doc->getRastY();
    oldMy = my;
}

/*================================================================*/
void KWPage::vmpCreate( int mx, int my )
{
    mx -= contentsX();
    my -= contentsY();
    mx = ( mx / doc->getRastX() ) * doc->getRastX();
    oldMx = mx;
    my = ( my / doc->getRastX() ) * doc->getRastY();
    oldMy = my;
    insRect = QRect( mx, my, 0, 0 );
    deleteMovingRect = FALSE;
}

/*================================================================*/
void KWPage::vmpCreatePixmap( int mx, int my )
{
    mx -= contentsX();
    my -= contentsY();
    if ( !pixmap_name.isEmpty() ) {
        QPixmap _pix( pixmap_name );
        mx = ( mx / doc->getRastX() ) * doc->getRastX();
        oldMx = mx;
        my = ( my / doc->getRastX() ) * doc->getRastY();
        oldMy = my;
        insRect = QRect( mx, my, 0, 0 );
        deleteMovingRect = FALSE;
        doRaster = FALSE;
        QCursor::setPos( viewport()->mapToGlobal( QPoint( mx + _pix.width(), my + _pix.height() ) ) );
    }
    else pixmap_name = "";
}

/*================================================================*/
/* pastes the clipboard to cursor position
 */
void KWPage::vmpMidButton()
{
    QClipboard *cb = QApplication::clipboard();
    bool emptyMessage = FALSE;

    if ( cb->data()->provides( MIME_TYPE ) ) {
        if ( cb->data()->encodedData( MIME_TYPE ).size() )
            editPaste( cb->data()->encodedData( MIME_TYPE ), MIME_TYPE );
    } else if ( cb->data()->provides( "text/plain" ) ) {
        if ( cb->data()->encodedData( "text/plain" ).size() )
            editPaste( QString::fromLocal8Bit(cb->data()->encodedData( "text/plain" )) );
    }
    else if ( !cb->text().isEmpty() )
    {
       // jwc - do not *paste* message about empty clipboard into document
       // instead, show message that clipboard is empty.  The Qt clipboard
       // contents can be not empty, but still null. The <empty message>
       // text inserted into clipboard when clipboard is empty is a Kde
       // klipper bug but until that is fixed checking for that message
       // is needed to avoid improper insertion of the message text

        if( !cb->text().isNull() )
        {
            QString text = cb->text();
            text.simplifyWhiteSpace();
            if(text.compare("<empty clipboard>"))
                editPaste( cb->text() );
        else
            emptyMessage = TRUE;
        }
        else
            emptyMessage = TRUE;
    }
    else
        emptyMessage = TRUE;

    if(emptyMessage)
    {
        maybeDrag = FALSE;

        if ( doc->has_selection())
        {
            doc->setSelection( FALSE );
            repaintScreen( FALSE );
        }
    }
}

/*================================================================*/
void KWPage::vmpRightButton( QMouseEvent *e, int mx, int my )
{
    if ( mouseMode != MM_EDIT_FRAME ) {
        KWCharAttribute *attrib = 0L;

        QPainter painter;
        painter.begin( viewport() );
        attrib = fc->getObjectType( mx, my );
        painter.end();

        if ( attrib )
            kdDebug() << "ObjectType: " << attrib->getClassId() << attrib << endl;
        return;
    }
    if ( doc->getFrameSet( mx, my ) == -1 ) {
        QPoint pnt( QCursor::pos() );
        mm_menu->popup( pnt );
    } else {
        switch ( mouseMode ) {
        case MM_EDIT_FRAME: {
            oldMx = mx;
            oldMy = my;
            if ( doc->selectFrame( mx, my ) == 1 && !( e->state() & ControlButton || e->state() & ShiftButton ) )
                doc->deSelectAllFrames();
            selectAllFrames( FALSE );
            selectFrame( mx, my, TRUE );
            QPoint pnt( QCursor::pos() );
            KWFrame *frame=doc->getFirstSelectedFrame();
            // if a header/footer etc. Dont show the popup.
            if(frame->getFrameSet() && frame->getFrameSet()->getFrameInfo() != FI_BODY) return;
            // enable delete
            frame_edit_menu->setItemEnabled(frEditDel, true);
            // if text frame,
            if(frame->getFrameSet() && frame->getFrameSet()->getFrameType() == FT_TEXT) {
                // if frameset 0 disable delete
                if(doc->getProcessingType() == KWordDocument::WP && frame->getFrameSet() == doc->getFrameSet(0)) {
                    frame_edit_menu->setItemEnabled(frEditReconnect, false);
                    frame_edit_menu->setItemEnabled(frEditDel, false);
                } else {
                    frame_edit_menu->setItemEnabled(frEditReconnect, true);
                }
            } else
                frame_edit_menu->setItemEnabled(frEditReconnect, false);
            frame_edit_menu->popup( pnt );
        } break;
        default: break;
        }
    }
}

/*================================================================*/
void KWPage::viewportMousePressEvent( QMouseEvent *e )
{
    stopBlinkCursor();
    maybeDrag = FALSE;

    if ( editNum != -1 ) {
        if ( doc->getFrameSet( editNum )->getFrameType() == FT_PART ) {
            dynamic_cast<KWPartFrameSet*>( doc->getFrameSet( editNum ) )->deactivate();
            viewport()->setFocus();
            recalcCursor( FALSE );
            setMouseMode( MM_EDIT );
            editNum = -1;
            return;
        } else if ( doc->getFrameSet( editNum )->getFrameType() == FT_FORMULA ) {
            gui->getView()->showFormulaToolbar( FALSE );
            dynamic_cast<KWFormulaFrameSet*>( doc->getFrameSet( editNum ) )->deactivate();
            setFocusProxy( 0 );
            viewport()->setFocusProxy( this );
            viewport()->setFocus();
            recalcCursor( FALSE );
            setMouseMode( MM_EDIT );
            editNum = -1;
            gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
            return;
        }
    }

    oldMx = e->x() + contentsX();
    oldMy = e->y() + contentsY();
    int mx = e->x() + contentsX();
    int my = e->y() + contentsY();

    // focus change.
    QPainter _painter;
    _painter.begin( viewport() );
    if ( doc->has_selection() && *doc->getSelStart() == *doc->getSelEnd()
            && mouseMode != MM_EDIT ) {
        doc->drawSelection( _painter, contentsX(), contentsY() );
        doc->setSelection( FALSE );
    }
    _painter.end();

    switch ( e->button() ) {
        case LeftButton: {
            mousePressed = TRUE;
            switch ( mouseMode ) {
            case MM_EDIT:
                if ( !vmpEdit( mx, my ) )
                    return;
                break;
            case MM_EDIT_FRAME:
                vmpEditFrame( e, mx, my );
                break;
            case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE:
            case MM_CREATE_FORMULA: case MM_CREATE_KSPREAD_TABLE:
                vmpCreate( mx, my );
                break;
            case MM_CREATE_PIX:
                vmpCreatePixmap( mx, my );
                break;
            default: break;
            }
            // The "edit" modes don't modify the doc. The "create" ones do.
            if ( mouseMode != MM_EDIT && mouseMode != MM_EDIT_FRAME )
                doc->setModified( TRUE );
        } break;
        case MidButton:
            vmpMidButton();
            break;
        case RightButton:
           if ( !vmpEdit( mx, my ) )
               return;
            vmpRightButton( e, mx, my );
            break;
        default: break;
    }

    mouseMoved = FALSE;
}

/*================================================================*/
void KWPage::vmrEdit()
{
    if ( doc->has_selection() && *doc->getSelStart() != *doc->getSelEnd() && mouseMoved )
        doc->copySelectedText();

    if ( doc->has_selection() && *doc->getSelStart() == *doc->getSelEnd() )
        doc->setSelection( FALSE );

    gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
    gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
    setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
    setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
    gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                        getFrame( fc->getFrame() - 1 )->x() );
    gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
    format = *( ( KWFormat* )fc );
}

/*================================================================*/
void KWPage::vmrEditFrame( int /*mx*/, int /*my*/ )
{
    int frameset=0;
    KWFrame *frame= doc->getFirstSelectedFrame(frameset);
    if(!frame) return;
    if ( doc->getProcessingType() == KWordDocument::DTP )
        setRuler2Frame( frame );
    gui->getHorzRuler()->setFrameStart( frame->x() );

    if ( mouseMoved ) {
        doc->recalcFrames();
        doc->updateAllFrames();
        KWGroupManager *grpMgr = frame->getFrameSet()->getGroupManager();
        if(grpMgr) {
            grpMgr->recalcCols();
            grpMgr->recalcRows();
            grpMgr->updateTempHeaders();
            repaintTableHeaders( grpMgr );
        }
        recalcAll = TRUE;
        recalcText();
        recalcCursor();
        repaintScreen( TRUE );
        recalcAll = FALSE;
        updateSelections();
    }  else
        doc->updateAllViews( gui->getView() );
}

/*================================================================*/
void KWPage::vmrCreateText()
{
    repaintScreen( FALSE );
    KWFrame *frame = new KWFrame(0L, insRect.x() + contentsX(), insRect.y() + contentsY(), insRect.width(), insRect.height() );

    insRect = insRect.normalize();
    if ( insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY() ) {
        if ( frameDia ) {
            frameDia->close();
            delete frameDia;
            frameDia = 0;
        }

        frameDia = new KWFrameDia( this, frame,doc,FT_TEXT);
        connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
        frameDia->setCaption(i18n("Connect frame"));
        frameDia->show();
    }
}

/*================================================================*/
void KWPage::vmrCreatePixmap()
{
    repaintScreen( FALSE );

    insRect = insRect.normalize();
    if ( insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY() && !pixmap_name.isEmpty() ) {
        KWPictureFrameSet *frameset = new KWPictureFrameSet( doc );
        frameset->setFileName( pixmap_name, QSize( insRect.width(), insRect.height() ) );
        insRect = insRect.normalize();
        KWFrame *frame = new KWFrame(frameset, insRect.x() + contentsX(), insRect.y() + contentsY(), insRect.width(),
                                     insRect.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        repaintScreen( FALSE );
    }
    mmEdit();
}

/*================================================================*/
void KWPage::vmrCreatePartAnSoOn()
{
    repaintScreen( FALSE );

    insRect = insRect.normalize();
    if ( insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY() ) {
        doc->insertObject( insRect, partEntry, contentsX(), contentsY() );
        repaintScreen( TRUE );
    }
    mmEdit();
}

/*================================================================*/
void KWPage::vmrCreateFormula()
{
    repaintScreen( FALSE );

    insRect = insRect.normalize();
    if ( insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY() ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( doc, this );
        KWFrame *frame = new KWFrame(frameset, insRect.x() + contentsX(), insRect.y() + contentsY(), insRect.width(),
                                     insRect.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        repaintScreen( FALSE );
    }
    mmEdit();
}

/*================================================================*/
void KWPage::vmrCreateTable()
{
    repaintScreen( FALSE );

    insRect = insRect.normalize();
    if ( insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY() ) {
        if ( tcols * minFrameWidth + insRect.x() > doc->getPTPaperWidth() )
            {
                KMessageBox::sorry(0, i18n("KWord is unable to insert the table because there\n"
                                           "is not enough space available."));
            }
        else {
            KWGroupManager *grpMgr;

            if ( useAnchor ) {
                grpMgr = static_cast<KWGroupManager *>(anchor);
            } else
                grpMgr = new KWGroupManager( doc );

            QString _name;
            int numGroupManagers=doc->getNumGroupManagers();
            bool found=true;
            while(found) { // need a new name for the new groupmanager.
                bool same = false;
                _name.sprintf( "grpmgr_%d",numGroupManagers);
                for ( unsigned int i = 0;!same && i < doc->getNumGroupManagers(); ++i ) {
                    if ( doc->getGroupManager( i )->getName() == _name ){
                        same = true;
                        break;
                    }
                }
                if(!same) found=false;
                numGroupManagers++;
            }
            grpMgr->setName( _name );
            doc->addGroupManager( grpMgr );
            for ( unsigned int i = 0; i < trows; i++ ) {
                for ( unsigned int j = 0; j < tcols; j++ ) {
                    KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
                    KWFrame *frame = new KWFrame(_frameSet, insRect.x() + contentsX(), insRect.y() + contentsY(), (doc->getPTPaperWidth() - insRect.x())/tcols, insRect.height() );
                    _frameSet->addFrame( frame );
                    frame->setFrameBehaviour(AutoExtendFrame);
                    frame->setNewFrameBehaviour(NoFollowup);
                    _frameSet->setGroupManager( grpMgr );
                    grpMgr->addFrameSet( _frameSet, i, j );
                }
            }
            grpMgr->init( insRect.x() + contentsX(), insRect.y() + contentsY(), insRect.width(), insRect.height(),
                          twid, thei );
            grpMgr->recalcRows();
        }
        recalcWholeText( TRUE );
        doc->updateAllFrames();
    }
    mmEdit();
    useAnchor = false;
}

/*================================================================*/
void KWPage::viewportMouseReleaseEvent( QMouseEvent *e )
{
    if ( scrollTimer.isActive() )
        scrollTimer.stop();

    if ( maybeDrag && doc->has_selection() && mouseMode == MM_EDIT ) {
        doc->setSelection( FALSE );
        repaintScreen( FALSE );
    }

    mousePressed = FALSE;
    maybeDrag = FALSE;
    int mx = e->x() + contentsX();
    int my = e->y() + contentsY();

    switch ( mouseMode ) {
    case MM_EDIT:
        vmrEdit();
        break;
    case MM_EDIT_FRAME:
        vmrEditFrame( mx, my );
        break;
    case MM_CREATE_TEXT:
        vmrCreateText();
        break;
    case MM_CREATE_PIX:
        vmrCreatePixmap();
        break;
    case MM_CREATE_PART: case MM_CREATE_KSPREAD_TABLE:
        vmrCreatePartAnSoOn();
        break;
    case MM_CREATE_TABLE:
        vmrCreateTable();
        break;
    case MM_CREATE_FORMULA:
        vmrCreateFormula();
        break;
    default:
        repaintScreen( FALSE );
    }

    if ( hasFocus() )
    startBlinkCursor();
}

/*================================================================*/
void KWPage::vmdEdit( int mx, int my )
{
    if ( doc->has_selection() )
        {
            QPainter _painter;
            _painter.begin( viewport() );
            doc->drawSelection( _painter, contentsX(), contentsY() );
            doc->setSelection( FALSE );
            _painter.end();
        }

    int frameset = doc->getFrameSet( mx, my );

    if ( (frameset != -1) &&
         (doc->getFrameSet( frameset )->getFrameType() == FT_TEXT) ) {
        showCursor(false);

        fc->setFrameSet( frameset + 1 );

        fc->cursorGotoPixelLine( mx, my );
        fc->cursorGotoPixelInLine( mx, my );

        KWFormatContext fc1( doc, fc->getFrameSet() ), fc2( doc, fc->getFrameSet() );
        if ( fc->selectWord( fc1, fc2 ) ) {
            showCursor( true );

            doc->setSelStart( fc1 );
            doc->setSelEnd( fc2 );
            doc->setSelection( TRUE );
            QPainter _painter;
            _painter.begin( viewport() );
            doc->drawSelection( _painter, contentsX(), contentsY() );
            _painter.end();
        } else {
            showCursor( true );
        }

        if ( doc->getProcessingType() == KWordDocument::DTP )
            setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );

        gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

        if ( fc->getParag() ) {
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
            gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
            gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
        }
    }
}

/*================================================================*/
void KWPage::vmdEditFrame( int mx, int my )
{
    int frameset = doc->getFrameSet( mx, my );

    if(frameset==-1)
        return;
    KWFrameSet *fset = doc->getFrameSet( frameset );
    if( !fset )
        return;

    if( fset->getFrameType() == FT_PART ) {
        KWPartFrameSet *fs = dynamic_cast<KWPartFrameSet*>( fset );
        fs->activate( gui->getView() );
        editNum = frameset;
    }
    else if( fset->getFrameType() == FT_FORMULA ) {
        gui->getView()->showFormulaToolbar( true );
        KWFormulaFrameSet *fs = dynamic_cast<KWFormulaFrameSet*>( fset );
        fs->activate( this );
        editNum = frameset;
        KWFormat *f = fs->getFormat();
        fc->apply( *f );
        //formatChanged( *f, FALSE );
        gui->getView()->setFormat( *f, true, false );
        delete f;
    }
}

/*================================================================*/
void KWPage::viewportMouseDoubleClickEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
        return;

    stopBlinkCursor();

    int mx = e->x() + contentsX();
    int my = e->y() + contentsY();

    mousePressed = FALSE;

    if ( mouseMode == MM_EDIT )
        vmdEdit( mx, my );
    else if ( mouseMode == MM_EDIT_FRAME )
        vmdEditFrame( mx, my );

    if ( hasFocus() )
    startBlinkCursor();
}

/*================================================================*/
void KWPage::recalcCursor( bool _repaint, int _pos, KWFormatContext *_fc )
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    if ( !_fc )
        _fc = fc;

    unsigned int pos = _fc->getTextPos();
    if ( _pos != -1 ) pos = static_cast<unsigned int>( _pos );

    _fc->init( _fc->getParag()->getPrev() ? _fc->getParag()->getPrev()->getNext() :
               dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( _fc->getFrameSet() - 1 ) )->getFirstParag(),
               FALSE );

    _fc->gotoStartOfParag();
    _fc->cursorGotoLineStart();
    _fc->cursorGotoRight( pos );

    if ( _repaint )
        repaintScreen( FALSE );

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
int KWPage::getVertRulerPos(int y)
{
    return ( -(y==-1 ? contentsY() : y) + (fc->getPage() - 1) * ptPaperHeight() );
}

int KWPage::getHorzRulerPos(int x)
{
    return ( -(x==-1 ? contentsX() : x) );
}

/*================================================================*/
void KWPage::insertPictureAsChar( QString _filename )
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    fc->getParag()->insertPictureAsChar( fc->getTextPos(), _filename );
    recalcCursor();

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::editCut()
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    if ( doc->has_selection() ) {
        doc->copySelectedText();
        doc->getAutoFormat().setEnabled( TRUE );
        doc->deleteSelectedText( fc );
        doc->setSelection( FALSE );
        recalcCursor();
        doc->getAutoFormat().setEnabled( FALSE );
    }

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::editCopy()
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    if ( doc->has_selection() ) {
        doc->copySelectedText();
        doc->setSelection( FALSE );
        repaintScreen( FALSE );
    }

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::editPaste( QString _string, const QString &_mime )
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    doc->getAutoFormat().setEnabled( TRUE );
    doc->paste( fc, _string, this, 0L, _mime );
    doc->setSelection( FALSE );
    recalcText();
    recalcCursor();
    doc->getAutoFormat().setEnabled( FALSE );

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::editDeleteFrame()
{
    if ( mouseMode != MM_EDIT_FRAME ) {
        KMessageBox::sorry( this, i18n( "Please switch to the frame edit tool and\n"
                                        "select the frame you want to delete." ),
                            i18n( "Delete Frame" ) );
        return;
    }

    QList<KWFrame> frames=doc->getSelectedFrames();
    if(frames.count()>1)  {
        KMessageBox::sorry( this, i18n( "You have selected multiple frames.\n"
                                        "You can only delete one frame at the time." ),
                            i18n( "Delete Frame" ) );
        return;
    }
    if(frames.count()<1)  {
        KMessageBox::sorry( this, i18n( "You have not selected a frame.\n"
                                        "You need to select a frame first in order to delete it."),
                            i18n( "Delete Frame" ) );
        return;
    }
    KWFrame *theFrame = frames.at(0);

    if ( isAHeader(theFrame->getFrameSet()->getFrameInfo()) ) {
        KMessageBox::sorry( this, i18n( "This is a Header frame, it can not be deleted."),
                            i18n( "Delete Frame"  ) );
        return;
    }
    if ( isAFooter(theFrame->getFrameSet()->getFrameInfo()) ) {
        KMessageBox::sorry( this, i18n( "This is a Footer frame, it can not be deleted."),
                            i18n( "Delete Frame"  ) );
        return;
    }

    // frame is part of a table?
    if ( theFrame->getFrameSet()->getGroupManager() ) {
        int result;
        result = KMessageBox::warningContinueCancel(this,
                                                    i18n( "You are about to delete a table\n"
                                                          "Doing so will delete all the text in the table\n"
                                                          "Are you sure you want to do that?"), i18n("Delete Table"), i18n("&Delete"));
        if (result != KMessageBox::Continue)
            return;
        deleteTable( theFrame->getFrameSet()->getGroupManager() );
        return;
    }

    if ( theFrame->getFrameSet()->getNumFrames() == 1 && theFrame->getFrameSet()->getFrameType() == FT_TEXT) {
        if ( doc->getProcessingType() == KWordDocument::WP && doc->getFrameSetNum( theFrame->getFrameSet() ) == 0 )
            return;
        KWParag *parag= dynamic_cast <KWTextFrameSet *> (theFrame->getFrameSet())->getFirstParag();
        if(!( parag!=parag->getNext() && parag->getKWString()->size()==0)) {
            int result;
            result = KMessageBox::warningContinueCancel(this,
                                                        i18n( "You are about to delete the last Frame of the\n"
                                                              "Frameset '%1'.\n"
                                                              "Doing so will delete this Frameset and all the\n"
                                                              "text contained in it as well!\n\n"
                                                              "Are you sure you want to do that?").arg(theFrame->getFrameSet()->getName()),
                                                        i18n("Delete Frame"), i18n("&Delete"));
            if (result != KMessageBox::Continue)
                return;
        }
    }
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    // do the actual delete.
    if ( theFrame->getFrameSet()->getNumFrames() > 1 )
        theFrame->getFrameSet()->delFrame( theFrame );
    else
        doc->delFrameSet( theFrame->getFrameSet() );


    // set FC to new frameset
    fc->setFrameSet( 1 );
    fc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getFirstParag() );

    doc->recalcFrames();
    doc->updateAllFrames();
    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    repaintScreen( TRUE );
    recalcAll = FALSE;
}

/*================================================================*/
void KWPage::deleteTable( KWGroupManager *groupManager )
{
    if ( !groupManager )
        return;

    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    doc->delGroupManager( groupManager );
    doc->recalcFrames();
    doc->updateAllFrames();

    fc->setFrameSet( 0 );
    fc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getFirstParag() );

    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    repaintScreen( TRUE );
    recalcAll = FALSE;

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::editReconnectFrame()
{
    // This is ugly please rethink. (TZ)
    if ( mouseMode != MM_EDIT_FRAME ) {
        KMessageBox::sorry( this, i18n( "Please switch to the frame edit tool and\n"
                                        "select the frame you want to reconnect." ),
                            i18n( "Reconnect Frame" ) );
        return;
    }

    KWFrameSet *fs = 0;
    KWFrame *frame = 0;
    for ( unsigned int i = 0;i < doc->getNumFrameSets(); ++i ) {
        KWFrameSet *f = doc->getFrameSet( i );
        for ( unsigned int j = 0;j < f->getNumFrames(); ++j ) {
            KWFrame *_f = f->getFrame( j );
            if ( _f->isSelected() ) {
                if ( frame ) {
                    KMessageBox::sorry( this, i18n( "You have selected multiple frames.\n"
                                                    "You can only reconnect one frame at the time." ),
                                        i18n( "Reconnect Frame" ) );
                    return;
                }
                frame = _f;
                fs = f;
            }
        }
    }

    if ( !frame || !fs ) {
        KMessageBox::sorry( this, i18n( "You have not selected a frame.\n"
                                        "You need to select a frame first in order to reconnect it."),
                            i18n( "Reconnect Frame" ) );
        return;
    }

    if ( fs->getGroupManager() || fs->getFrameType() != FT_TEXT )
        return;

    if ( fs->getNumFrames() == 1 ) {
        if ( doc->getProcessingType() == KWordDocument::WP && doc->getFrameSetNum( fs ) == 0 )
            return;
        KWParag *parag= dynamic_cast <KWTextFrameSet *> (fs)->getFirstParag();
        if(!( parag!=parag->getNext() && parag->getKWString()->size()==0)) {
            int result;
            result = KMessageBox::warningContinueCancel(this,
                                                        i18n( "You are about to reconnect the last Frame of the\n"
                                                              "Frameset '%1'.\n"
                                                              "Doing so will delete the Frameset and all the\n"
                                                              "text contained in it!\n\n"
                                                              "Are you sure you want to do that?").arg(fs->getName()),
                                                        i18n("Reconnect Frame"), i18n("&Delete"));
            if (result != KMessageBox::Continue)
                return;
        }
    }

    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    KWFrameSet *f = doc->getFrameSet( fc->getFrameSet() - 1 );
    if ( f == fs ) {
        fc->setFrameSet( 1 );
        fc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getFirstParag() );
    }

    if ( frameDia ) {
        frameDia->close();
        delete frameDia;
        frameDia = 0;
    }

    frameDia = new KWFrameDia( this,  frame,doc,frame->getFrameType());

    connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
    frameDia->setCaption( i18n( "Reconnect Frame" ) );
    frameDia->show();

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::recalcText()
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    KWFormatContext _fc( doc, fc->getFrameSet() );
    _fc.init( doc->getFirstParag( fc->getFrameSet() - 1 ) );

    bool bend = FALSE;

    while ( !bend ) {
        bend = !_fc.makeNextLineLayout();
        if ( doc->getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->y() >
             static_cast<int>( contentsY() + height() + 20 ) )
            bend = TRUE;
    }

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::recalcWholeText( bool _cursor)
{
    if ( recalcingText )
        return;

    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    QApplication::setOverrideCursor( waitCursor );
    viewport()->setCursor( waitCursor );

    recalcingText = TRUE;

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        if ( doc->getFrameSet( i )->getFrameType() != FT_TEXT || doc->getFrameSet( i )->getNumFrames() == 0 || doc->getFrameSet(i)->getGroupManager())
            continue;
        KWFormatContext _fc( doc, i + 1 );
        _fc.init( doc->getFirstParag( i ) );

        bool bend = FALSE;

        while ( !bend ) {
            bend = !_fc.makeNextLineLayout();
            if ( doc->getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->y() >
                 static_cast<int>( contentsY() + height() + height() / 2 ) )
                bend = TRUE;
        }
    }

    if ( _cursor ) recalcCursor();
    recalcingText = FALSE;

    QApplication::restoreOverrideCursor();
    viewport()->setCursor( ibeamCursor );

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::recalcWholeText( KWParag *start, unsigned int fs )
{
    if ( recalcingText ) return;

    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    QApplication::setOverrideCursor( waitCursor );
    viewport()->setCursor( waitCursor );

    recalcingText = TRUE;

    KWFormatContext _fc( doc, fs + 1 );
    _fc.init( start, FALSE );

    bool bend = FALSE;

    while ( !bend )
        {
            bend = !_fc.makeNextLineLayout();
            if ( doc->getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->y() >
                 static_cast<int>( contentsY() + height() + height() / 2 ) )
                bend = TRUE;
        }

    recalcingText = FALSE;

    QApplication::restoreOverrideCursor();
    viewport()->setCursor( ibeamCursor );

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::footerHeaderDisappeared()
{
    if ( isAHeader( doc->getFrameSet( fc->getFrameSet() - 1 )->getFrameInfo() ) ||
         isAFooter( doc->getFrameSet( fc->getFrameSet() - 1 )->getFrameInfo() ) )
        {
            fc->setFrameSet( 1 );
            fc->init( doc->getFirstParag( 0 ), FALSE );

            gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
            gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
            gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
            gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
            gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                             fc->getParag()->getParagLayout()->getRightBorder(),
                                             fc->getParag()->getParagLayout()->getTopBorder(),
                                             fc->getParag()->getParagLayout()->getBottomBorder() );
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
            gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
            gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
            format = *( ( KWFormat* )fc );
        }
}

/*================================================================*/
void KWPage::recalcPage( KWParag *_p )
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    calcVisiblePages();
    KWFormatContext *paintfc = new KWFormatContext( doc, 1 );
    for ( unsigned i = 0; i < doc->getNumFrameSets(); i++ ) {
        switch ( doc->getFrameSet( i )->getFrameType() ) {
        case FT_TEXT: {
            KWParag *p = doc->findFirstParagOfRect( contentsY(), firstVisiblePage, i );
            if ( p ) {
                paintfc->setFrameSet( i + 1 );
                paintfc->init( p, recalcAll );

                if ( i == fc->getFrameSet() - 1 && _p ) {
                    while ( paintfc->getParag() != _p->getNext() )
                        paintfc->makeNextLineLayout();
                } else {
                    bool bend = FALSE;
                    while ( !bend ) {
                        if ( allowBreak1( paintfc, i ) )
                            break;
                        bend = !paintfc->makeNextLineLayout();
                        if ( paintfc->getPage() > lastVisiblePage )
                            bend = TRUE;
                    }
                }
            }
        } break;
        default: break;
        }
    }

    *formatFC = *paintfc;
    formatTimer.stop();
    formatTimer.start( 0, TRUE );

    delete paintfc;

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
bool KWPage::allowBreak1( KWFormatContext *paintfc, unsigned int i )
{
    if ( paintfc->getFrameSet() == 1 && doc->getProcessingType() == KWordDocument::WP &&
         static_cast<int>( paintfc->getParag()->getPTYStart() - contentsY() ) > height() && doc->getColumns() == 1 )
        return TRUE;
    if ( doc->getFrameSet( i )->getFrame( paintfc->getFrame() - 1 )->isMostRight() &&
         doc->getFrameSet( i )->getNumFrames() > paintfc->getFrame() &&
         doc->getFrameSet( i )->getFrame( paintfc->getFrame() )->top() -
         static_cast<int>( contentsY() ) >
         static_cast<int>( lastVisiblePage ) * static_cast<int>( ptPaperHeight() ) &&
         static_cast<int>( paintfc->getPTY() - contentsY() ) > height() )
        return TRUE;
    if ( doc->getFrameSet( i )->getFrame( paintfc->getFrame() - 1 )->top() - static_cast<int>( contentsY() ) >
         static_cast<int>( lastVisiblePage ) * static_cast<int>( ptPaperHeight() ) )
        return TRUE;

    return FALSE;
}

/*================================================================*/
void KWPage::paintPicture( QPainter &painter, int i )
{
    KWPictureFrameSet *picFS = dynamic_cast<KWPictureFrameSet*>( doc->getFrameSet( i ) );
    KWFrame *frame = picFS->getFrame( 0 );
    QSize _size = QSize( frame->width(), frame->height() );

    if ( _size != picFS->getImage()->size() )
        picFS->setSize( _size );

    painter.drawImage( frame->x() - contentsX(), frame->y() - contentsY(), *picFS->getImage() );
}

/*================================================================*/
void KWPage::paintPart( QPainter &painter, int i )
{
    KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>( doc->getFrameSet( i ) );
    KWFrame *frame = partFS->getFrame( 0 );

    painter.end();
    QPicture *pic = partFS->getPicture();
    painter.begin( viewport() );

    painter.save();
    QRect r = painter.viewport();
    painter.setClipRect( frame->x() - contentsX(), frame->y() - contentsY(),
                         frame->width() - 1, frame->height() - 1 );
    painter.setViewport( frame->x() - contentsX(), frame->y() - contentsY(), r.width(), r.height() );
    if ( pic )
        painter.drawPicture( *pic );
    painter.setViewport( r );
    painter.restore();
}

/*================================================================*/
void KWPage::paintFormula( QPainter &painter, int i )
{
    KWFormulaFrameSet *formulaFS = dynamic_cast<KWFormulaFrameSet*>( doc->getFrameSet( i ) );
    KWFrame *frame = formulaFS->getFrame( 0 );

    painter.end();
    QPicture *pic = formulaFS->getPicture();
    painter.begin( viewport() );

    painter.save();
    QRect r = painter.viewport();
    painter.setViewport( frame->x() - contentsX(), frame->y() - contentsY(), r.width(), r.height() );
    if ( pic )
        painter.drawPicture( *pic );
    painter.setViewport( r );
    painter.restore();
}

/*================================================================*/
void KWPage::paintText( QPainter &painter, KWFormatContext *paintfc, int i, QPaintEvent *e )
{
    if ( isAHeader( doc->getFrameSet( i )->getFrameInfo() ) && !doc->hasHeader() ||
         isAFooter( doc->getFrameSet( i )->getFrameInfo() ) && !doc->hasFooter() ||
         isAWrongHeader( doc->getFrameSet( i )->getFrameInfo(), doc->getHeaderType() ) ||
         isAWrongFooter( doc->getFrameSet( i )->getFrameInfo(), doc->getFooterType() ) ||
         !doc->getFrameSet( i )->isVisible() )
        return;

    if ( doc->getFrameSet( i )->getFrameInfo() == FI_BODY ) {
        KWParag *p = 0L;
        p = doc->findFirstParagOfRect( e->rect().y() + contentsY(), firstVisiblePage, i );

        if ( p ) {
            paintfc->setFrameSet( i + 1 );
            paintfc->init( p, recalcAll );

            QRegion emptyRegion = paintfc->getEmptyRegion();
            QRect r = *doc->getFrameSet( i )->getFrame( paintfc->getFrame() - 1 );
            r.setHeight( paintfc->getPTY() - r.y() );
            emptyRegion = emptyRegion.subtract( r );

            KWFrame *frame = doc->getFrameSet( i )->getFrame( paintfc->getFrame() - 1 );
            KWFrame *oldFrame = frame;
            bool bend = FALSE;
            while ( !bend ) {
                if ( allowBreak1( paintfc, i ) )
                    break;
                oldFrame = frame;
                frame = doc->getFrameSet( i )->getFrame( paintfc->getFrame() - 1 );

                if ( oldFrame != frame ) {
                    painter.save();
                    emptyRegion.translate( -contentsX(), -contentsY() );
                    painter.save();
                    painter.setClipRegion( emptyRegion );
                    painter.fillRect( emptyRegion.boundingRect(), QBrush( oldFrame->getBackgroundColor() ) );
                    painter.restore();
                    drawFrameBorder( painter, oldFrame );
                    painter.restore();

                    emptyRegion = paintfc->getEmptyRegion();
                    QRect r = *doc->getFrameSet( i )->getFrame( paintfc->getFrame() - 1 );
                    r.setHeight( paintfc->getPTY() - r.y() );
                    emptyRegion = emptyRegion.subtract( r );
                }

                unsigned int _x = frame->x() - contentsX();
                unsigned int _wid = frame->width();
                unsigned int li = frame->getLeftIndent( paintfc->getPTY(), paintfc->getLineHeight() );
                unsigned int re = frame->getRightIndent( paintfc->getPTY(), paintfc->getLineHeight() );
                QRect fr( _x + li, paintfc->getPTY() - contentsY(), _wid - li - re, paintfc->getLineHeight() );
                emptyRegion = emptyRegion.subtract( QRect( _x + li, paintfc->getPTY(),
                                                           _wid - li - re, paintfc->getLineHeight() ) );
                doc->printLine( *paintfc, painter, contentsX(), contentsY(), width(), height(),
                                gui->getView()->getViewFormattingChars(), TRUE,
                                fr.x(), fr.y(), fr.width(), fr.height(),
                                QBrush( frame->getBackgroundColor() ));
                bend = !paintfc->makeNextLineLayout();
                if ( paintfc->getPage() > lastVisiblePage )
                    bend = TRUE;
            }

            *formatFC = *paintfc;
            formatTimer.stop();
            formatTimer.start( 0, TRUE );

            if ( frame ) {
                painter.save();
                emptyRegion.translate( -contentsX(), -contentsY() );
                painter.save();
                painter.setClipRegion( emptyRegion );
                painter.fillRect( emptyRegion.boundingRect(), QBrush( frame->getBackgroundColor() ) );
                painter.restore();
                drawFrameBorder( painter, frame );
                painter.restore();
            }
            KWFrameSet *fs = doc->getFrameSet( i );
            KWFrame *f = 0;
            int frm = fs->getFrameFromPtr( frame );
            if ( frm != -1 && frm < (int)fs->getNumFrames() - 1 ) {
                // #### quite unefficient
                for ( unsigned int j = frm + 1; j < fs->getNumFrames(); ++j ) {
                    painter.save();
                    f = fs->getFrame( j );
                    QRegion reg = f->getEmptyRegion();
                    reg.translate( -contentsX(), -contentsY() );
                    reg = reg.intersect( e->rect() );
                    painter.setClipRegion( reg );
                    QRect r = *f;
                    r.moveBy( -contentsX(), -contentsY() );
                    if ( r.intersects( e->rect() ) )
                        painter.fillRect( r, f->getBackgroundColor() );
                    painter.restore();
                }
            }

        }
    } else {
        KWParag *p = 0L;

        KWFrameSet *fs = doc->getFrameSet( i );
        QRect r = QRect( e->rect().x() + contentsX(), e->rect().y() + contentsY(),
                         e->rect().width(), e->rect().height() );
        QRect v = QRect( e->rect().x() + contentsX(), e->rect().y() + contentsY(),
                         e->rect().width(), e->rect().height() );

        while ( TRUE ) {
            int frm = fs->getNext( r );
            if ( !v.intersects( r ) )
                break;
            if ( frm == -1 )
                break;

            r.moveBy( 0, fs->getFrame( frm )->height() + 1 );

            p = dynamic_cast<KWTextFrameSet*>( fs )->getFirstParag();
            paintfc->setFrameSet( i + 1 );
            paintfc->init( p, TRUE, frm + 1, fs->getFrame( frm )->getPageNum() + 1 );

            QRect fr = *fs->getFrame( paintfc->getFrame() - 1 );
            fr.moveBy( -contentsX(), -contentsY() );
            painter.fillRect( fr, QBrush( fs->getFrame( 0 )->getBackgroundColor() ) );

            bool bend = FALSE;
            while ( !bend ) {
                if ( allowBreak1( paintfc, i ) )
                    break;

                doc->printLine( *paintfc, painter, contentsX(), contentsY(), width(), height(),
                                gui->getView()->getViewFormattingChars() );
                bend = !paintfc->makeNextLineLayout();
                if ( paintfc->getPage() > lastVisiblePage )
                    bend = TRUE;
            }

            drawFrameBorder( painter, fs->getFrame( 0 ),  0, fs->getFrame( frm )->y() -
                             fs->getFrame( 0 )->y() );
            *formatFC = *paintfc;
            formatTimer.stop();
            formatTimer.start( 0, TRUE );

        }
    }
}

/*================================================================*/
void KWPage::finishPainting( QPaintEvent *e, QPainter &painter )
{
    KWFormatContext _fc( doc, fc->getFrameSet() );
    _fc = *fc;
    KWFrameSet *frameSet = doc->getFrameSet( _fc.getFrameSet() - 1 );
    KWFrame *frame = frameSet->getFrame( _fc.getFrame() - 1 );
    unsigned int _x = frame->x() - contentsX();
    unsigned int _wid = frame->width();

    if ( e->rect().intersects( QRect( _x + frameSet->getFrame( _fc.getFrame() - 1 )->
                                      getLeftIndent( _fc.getPTY(), _fc.getLineHeight() ),
                                      _fc.getPTY() - contentsY(),
                                      _wid - frameSet->getFrame( _fc.getFrame() - 1 )->
                                      getLeftIndent( _fc.getPTY(), _fc.getLineHeight() ) -
                                      frameSet->getFrame( _fc.getFrame() - 1 )->
                                      getRightIndent( _fc.getPTY(),
                                                      _fc.getLineHeight() ),
                                      _fc.getLineHeight() ) ) )
        {
            if ( !e->rect().contains( QRect( _x + frameSet->getFrame( _fc.getFrame() - 1 )->
                                             getLeftIndent( _fc.getPTY(), _fc.getLineHeight() ),
                                             _fc.getPTY() - contentsY(),
                                             _wid - frameSet->getFrame( _fc.getFrame() - 1 )->
                                             getLeftIndent( _fc.getPTY(), _fc.getLineHeight() )
                                             - frameSet->getFrame( _fc.getFrame() - 1 )->
                                             getRightIndent( _fc.getPTY(), _fc.getLineHeight() ),
                                             _fc.getLineHeight() ) ) )
                {
                    painter.setClipping( FALSE );
                }
        }

    QRect fr( _x + frame->getLeftIndent( _fc.getPTY(), _fc.getLineHeight() ),
              _fc.getPTY() - contentsY(),
              _wid - frame->getLeftIndent( _fc.getPTY(), _fc.getLineHeight() ) -
              frame->getRightIndent( _fc.getPTY(), _fc.getLineHeight() ),
              _fc.getLineHeight() );
    doc->printLine( _fc, painter, contentsX(), contentsY(), width(), height(),
                    gui->getView()->getViewFormattingChars(), TRUE, fr.x(), fr.y(), fr.width(), fr.height(),
                    QBrush( frame->getBackgroundColor() ) );
    drawFrameBorder( painter, frame );

    if ( doc->has_selection() )
        {
            doc->drawSelection( painter, contentsX(), contentsY() );
        }

    if ( cursorIsVisible )
        {
            doc->drawMarker( *fc, &painter, contentsX(), contentsY() );
        }
}

/*================================================================*/
void KWPage::viewportPaintEvent( QPaintEvent *e )
{
    calcVisiblePages();

    QPainter painter;
    painter.begin( viewport() );
    painter.setClipRect( e->rect() );

    QRegion cr( e->rect() );
    QRegion r = cr;
    if ( contentsX() == 0 ) {
        r = QRegion( QRect( 1, 0, ptPaperWidth() - 2, visibleHeight() ) );
        r = r.intersect( cr );
    }

    if ( !_setErase )
        _erase = e->erased();

    if ( _scrolled )
        _erase = TRUE;

    _scrolled = FALSE;
    _setErase = FALSE;

    if ( _erase )
        painter.eraseRect( e->rect().x(), e->rect().y(),
                           e->rect().width(), e->rect().height() );

    drawBorders( painter, e->rect(), _erase, &r );

    int cf = currFrameSet == -1 ? fc->getFrameSet() - 1 : currFrameSet;

    KWFormatContext *paintfc = new KWFormatContext( doc, 1 );
    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        if ( redrawOnlyCurrFrameset && (int)i != cf )
            continue;

        switch ( doc->getFrameSet( i )->getFrameType() ) {
        case FT_PICTURE:
            paintPicture( painter, i );
            break;
        case FT_PART:
            paintPart( painter, i );
            break;
        case FT_FORMULA:
            paintFormula( painter, i );
            break;
        case FT_TEXT:
            paintText( painter, paintfc, i, e );
            break;
        default: break;
        }
    }
    delete paintfc;

    finishPainting( e, painter );

    painter.end();

    cachedContentsPos = QPoint( contentsX(), contentsY() );
    doc->setPageLayoutChanged( FALSE );
}

/*================================================================*/
void KWPage::repaintKeyEvent1( KWTextFrameSet *frameSet, bool /*full*/, bool exitASAP )
{
    QPainter painter;
    painter.begin( viewport() );

    KWFormatContext paintfc( doc, fc->getFrameSet() );
    paintfc = *fc;

    QRegion emptyRegion = paintfc.getEmptyRegion();
    QRect r = *frameSet->getFrame( paintfc.getFrame() - 1 );
    r.setHeight( paintfc.getPTY() - r.y() );
    emptyRegion = emptyRegion.subtract( r );

    bool bend = FALSE;

    unsigned int currFrameNum = paintfc.getFrame() - 1;
    unsigned int ptYEnd = fc->getParag()->getPTYEnd();
    bool lookInCache = paintfc.getParag() == cachedParag;
    cachedParag = paintfc.getParag();

    QStringList tmpCachedLines;
    QStringList::Iterator it = cachedLines.begin();

    KWFrame *frame = frameSet->getFrame( paintfc.getFrame() - 1 );
    KWFrame *oldFrame = frame;
    while ( !bend ) {
        bool forceDraw = FALSE;
        oldFrame = frame;
        frame = frameSet->getFrame( paintfc.getFrame() - 1 );

        if ( oldFrame != frame ) {
            painter.save();
            emptyRegion.translate( -contentsX(), -contentsY() );
            painter.save();
            painter.setClipRegion( emptyRegion );
            painter.fillRect( emptyRegion.boundingRect(), QBrush( oldFrame->getBackgroundColor() ) );
            painter.restore();
            drawFrameBorder( painter, oldFrame );
            painter.restore();

            emptyRegion = paintfc.getEmptyRegion();
            QRect r = *frameSet->getFrame( paintfc.getFrame() - 1 );
            r.setHeight( paintfc.getPTY() - r.y() );
            emptyRegion = emptyRegion.subtract( r );
        }

        if ( paintfc.getParag() != fc->getParag() )
            forceDraw = TRUE;
        if ( paintfc.getParag() != fc->getParag() && paintfc.getParag() != fc->getParag()->getPrev() &&
             fc->getParag()->getPTYEnd() == ptYEnd && exitASAP ) {
            QRect r = *frameSet->getFrame( paintfc.getFrame() - 1 );
            r.setY( paintfc.getPTY() );
            emptyRegion = emptyRegion.subtract( r );
            break;
        }
        if ( frameSet->getFrame( currFrameNum )->isMostRight() &&
             frameSet->getNumFrames() > currFrameNum + 1 &&
             frameSet->getFrame( paintfc.getFrame() )->top() - static_cast<int>( contentsY() ) >
             static_cast<int>( lastVisiblePage ) * static_cast<int>( ptPaperHeight() ) &&
             static_cast<int>( paintfc.getPTY() - contentsY() ) > height() )
            break;
        if ( frame->top() - static_cast<int>( contentsY() ) >
             static_cast<int>( lastVisiblePage ) * static_cast<int>( ptPaperHeight() ) )
            break;
        unsigned int _x = frame->x() - contentsX();
        unsigned int _wid = frame->width();

        QString str = paintfc.getParag()->getKWString()->toString( paintfc.getLineStartPos(),
                                                                   paintfc.getLineEndPos() -
                                                                   paintfc.getLineStartPos() + 1,
                                                                   TRUE );

        unsigned int pty = paintfc.getPTY();
        str += QString( "%1" ).arg( pty );
        bool drawIt = TRUE;
        if ( !forceDraw && lookInCache ) {
            if ( it == cachedLines.end() )
                it = cachedLines.begin();
            it = cachedLines.find( it, str );
            if ( it != cachedLines.end() ) {
                drawIt = FALSE;
                //              qDebug( "cached: %s", ( *it ).latin1() );
            } else {
                //              qDebug( "%d %d", it != cachedLines.end() , *it == (int)paintfc.getPTY() );
                //              if ( *it != (int)paintfc.getPTY() )
                //                  qDebug( "it: %d, pty: %d", *it, paintfc.getPTY() );
            }
        }

        unsigned int li = frame->getLeftIndent( paintfc.getPTY(), paintfc.getLineHeight() );
        unsigned int re = frame->getRightIndent( paintfc.getPTY(), paintfc.getLineHeight() );
        emptyRegion = emptyRegion.subtract( QRect( _x + li, paintfc.getPTY(), _wid - li - re,
                                                   paintfc.getLineHeight() ) );
        if ( drawIt || forceDraw ) {
            //      qDebug( "draw: %s", str.latin1() );
            doc->printLine( paintfc, painter, contentsX(), contentsY(), width(), height(),
                            gui->getView()->getViewFormattingChars(), TRUE,
                            _x + li, paintfc.getPTY() - contentsY(), _wid - li - re, paintfc.getLineHeight(),
                            QBrush( frame->getBackgroundColor() ) );
        }
        //      qDebug( "append: %d %s", pty, str.latin1() );
        tmpCachedLines.append( str );

        //      qDebug( "----------------------" );

        bend = !paintfc.makeNextLineLayout();

        if ( paintfc.getFrame() - 1 != currFrameNum )
            currFrameNum = paintfc.getFrame() - 1;

        if ( paintfc.getPage() > lastVisiblePage )
            bend = TRUE;
    }

    if ( frame ) {
        painter.save();
        emptyRegion.translate( -contentsX(), -contentsY() );
        painter.save();
        painter.setClipRegion( emptyRegion );
        painter.fillRect( emptyRegion.boundingRect(), QBrush( frame->getBackgroundColor() ) );
        painter.restore();
        drawFrameBorder( painter, frame );
        painter.restore();
    }

    painter.end();

    if ( !cachedLines.isEmpty() )
        tmpCachedLines.remove( tmpCachedLines.last() );
    cachedLines = tmpCachedLines;

    *formatFC = paintfc;
    formatTimer.stop();
}

/*================================================================*/
void KWPage::startProcessKeyEvent()
{
    keyboardRepeat = TRUE;
    continueSelection = FALSE;
    inKeyEvent = TRUE;
    XGetKeyboardControl( kapp->getDisplay(), &kbds );
    keyboardRepeat = kbds.global_auto_repeat;
    kbdc.auto_repeat_mode = FALSE;
    XChangeKeyboardControl( kapp->getDisplay(), KBAutoRepeatMode, &kbdc );
    doc->getAutoFormat().setEnabled( TRUE );
    stopBlinkCursor();
}

/*================================================================*/
void KWPage::stopProcessKeyEvent()
{
    kbdc.auto_repeat_mode = keyboardRepeat;
    XChangeKeyboardControl( kapp->getDisplay(), KBAutoRepeatMode, &kbdc );
    inKeyEvent = FALSE;
    doc->getAutoFormat().setEnabled( FALSE );

    startBlinkCursor();
    inputTimer.start( 500, TRUE );
}

/*================================================================*/
bool KWPage::kInsertTableRow()
{
    showCursor(false);

    KWGroupManager *grpMgr = doc->getFrameSet( fc->getFrameSet() - 1 )->getGroupManager();
    unsigned int row, col;
    row=grpMgr->getCell( doc->getFrameSet( fc->getFrameSet() - 1 ))->row;
    col=grpMgr->getCell( doc->getFrameSet( fc->getFrameSet() - 1 ))->col;

    grpMgr->insertRow( row + 1 );

    doc->recalcFrames();
    doc->updateAllFrames();

    fc->setFrameSet( doc->getFrameSetNum( grpMgr->getFrameSet( row + 1, col ) ) + 1 );
    fc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( fc->getFrameSet() - 1 ) )->getFirstParag(), TRUE );
    fc->gotoStartOfParag();
    fc->cursorGotoLineStart();

    scrollToCursor();
    showCursor( true );

    gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
    gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
    setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
    setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
    gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                        getFrame( fc->getFrame() - 1 )->x() );
    gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );

    if ( doc->getProcessingType() == KWordDocument::DTP )
        setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );

    doc->updateAllViews( 0L );
    doc->getAutoFormat().setEnabled( FALSE );

    return TRUE;
}

/*================================================================*/
bool KWPage::kContinueSelection( QKeyEvent *e )
{
    QPainter painter;
    painter.begin( viewport() );

    if ( e->key() == Key_Shift || ( e->state() & ShiftButton ) &&
         ( e->key() == Key_Left || e->key() == Key_Right ||
           e->key() == Key_Up || e->key() == Key_Down  ||
           e->key() == Key_End || e->key() == Key_Home ) )
        {
            continueSelection = TRUE;
        }
    else if ( doc->has_selection() && *doc->getSelStart() != *doc->getSelEnd() )
        {
            doc->setSelection( FALSE );
            doc->drawSelection( painter, contentsX(), contentsY() );
            painter.end();
            if ( e->key() == Key_Delete || e->key() == Key_Backspace || e->key() == Key_Return ||
                 e->key() == Key_Enter || e->ascii() >= 32 )
                {
                    doc->deleteSelectedText( fc );
                    recalcCursor();
                    if ( e->key() == Key_Delete || e->key() == Key_Backspace || e->key() == Key_Return ||
                         e->key() == Key_Enter )
                        return FALSE;
                }
        }

    return TRUE;
}

/*================================================================*/
bool KWPage::kHome( QKeyEvent *e, int, int, KWParag *, KWTextFrameSet * )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    fc->cursorGotoLineStart();
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

    if ( continueSelection || e->state() & ShiftButton ) {
        continueKeySelection();
        return FALSE;
    }
    return TRUE;
}

/*================================================================*/
bool KWPage::kEnd( QKeyEvent *e, int, int, KWParag *, KWTextFrameSet * )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    fc->cursorGotoLineEnd();
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

    if ( continueSelection || e->state() & ShiftButton ) {
        continueKeySelection();
        return FALSE;
    }
    return TRUE;
}

/*================================================================*/
bool KWPage::kRight( QKeyEvent *e, int , int , KWParag *, KWTextFrameSet * )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    fc->cursorGotoRight();
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

    if ( continueSelection || e->state() & ShiftButton ) {
        continueKeySelection();
        return FALSE;
    }
    return TRUE;
}

/*================================================================*/
bool KWPage::kLeft( QKeyEvent *e, int , int , KWParag *, KWTextFrameSet * )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    fc->cursorGotoLeft();
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

    if ( continueSelection || e->state() & ShiftButton ) {
        continueKeySelection();
        return FALSE;
    }
    return TRUE;
}

/*================================================================*/
bool KWPage::kUp( QKeyEvent *e, int, int, KWParag *parag, KWTextFrameSet *fs )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    bool gotoPrevTableCell = FALSE;
    if ( fs->getGroupManager() && !parag->getPrev() ) {
        if ( fc->isCursorInFirstLine() )
            gotoPrevTableCell = TRUE;
    }

    fc->cursorGotoUp();
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

    if ( continueSelection || e->state() & ShiftButton ) {
        continueKeySelection();
        return FALSE;
    } else if ( gotoPrevTableCell )
        cursorGotoPrevTableCell();

    return TRUE;
}

/*================================================================*/
bool KWPage::kDown( QKeyEvent *e, int, int, KWParag *parag, KWTextFrameSet *fs )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    bool gotoNextTableCell = FALSE;
    if ( fs->getGroupManager() && !parag->getNext() ) {
        if ( fc->isCursorInLastLine() )
            gotoNextTableCell = TRUE;
    }

    fc->cursorGotoDown();
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

    if ( continueSelection || e->state() & ShiftButton ) {
        continueKeySelection();
        return FALSE;
    } else if ( gotoNextTableCell )
        cursorGotoNextTableCell();

    return TRUE;
}


/*=============================
  kPrior - Prior key handler - same as PageUp. Move cursor up one
  visible document area height and adjust page view accordingly
                          ===================================*/

bool KWPage::kPrior( QKeyEvent *e, int, int, KWParag *parag, KWTextFrameSet *fs )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    bool gotoPrevTableCell = FALSE;
    if ( fs->getGroupManager() && !parag->getPrev() )
    {
        if ( fc->isCursorInFirstLine() )
            gotoPrevTableCell = TRUE;
    }

     unsigned int curY  = fc->getPTY();
     unsigned int oldY  = curY;
     unsigned int newY  = curY;
     unsigned int travY = 0;
     newY = (unsigned int)visibleHeight();

     // scroll cursor up until the total Y distance traversed is
     // equal to a viewport height.  In framesets with colums it will
     // go up and down columns with flow

     while(travY < newY)
     {
        fc->cursorGotoUp();
       travY += fc->getLineHeight();

        gui->getView()->setFormat( *( ( KWFormat* )fc ) );
        gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
        gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
        gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
       curY = fc->getPTY();
       if(curY == oldY)
           break;
       else
           oldY = curY;
    }
    fc->cursorGotoLineStart( );
    scrollToCursor();

    if ( continueSelection || e->state() & ShiftButton )
    {
        continueKeySelection();
        return FALSE;
    }
        else if ( gotoPrevTableCell )
        cursorGotoPrevTableCell();

    return TRUE;
}

/*=============================
  kNext - Next key handler - same as PageDown
  move cursor down one visible document area height
  and adjust page view accordingly
===================================*/

bool KWPage::kNext( QKeyEvent *e, int, int, KWParag *parag, KWTextFrameSet *fs )
{
    if ( !doc->has_selection() && e->state() & ShiftButton )
        startKeySelection();
    else
        *oldFc = *fc;

    bool gotoNextTableCell = FALSE;
    if ( fs->getGroupManager() && !parag->getNext() ) {
        if ( fc->isCursorInLastLine() )
            gotoNextTableCell = TRUE;
    }

     unsigned int curY  = fc->getPTY();
     unsigned int oldY  = curY;
     unsigned int newY  = curY;
     unsigned int travY = 0;
     newY = (unsigned int)visibleHeight();

      // move cursor down until the total Y distance traversed is
      // equal to a viewport height.  In framesets with colums it will
      // go up and down columns with flow

     while(travY <= newY)
     {
         fc->cursorGotoDown();
         travY += fc->getLineHeight();

         gui->getView()->setFormat( *( ( KWFormat* )fc ) );
         gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
         gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
         gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );

         curY = fc->getPTY();
         if(curY == oldY)
              break;
         else
              oldY = curY;
      }

     fc->cursorGotoLineEnd( );
     scrollToCursor();

     // scroll view so that cursor is in middle of view - makes editing a whole
     // lot easier than leaving it at bottom, ofen partly hidden by scroll bar

    scrollToOffset( contentsX(), contentsY() + ((unsigned int)visibleHeight()) / 2 );

    if ( continueSelection || e->state() & ShiftButton )
    {
        continueKeySelection();
        return FALSE;
    }
        else if ( gotoNextTableCell )
        cursorGotoNextTableCell();

    return TRUE;
}

/*================================================================*/
bool KWPage::kReturn( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet )
{
    redrawOnlyCurrFrameset = TRUE;

    QString pln = fc->getParag()->getParagLayout()->getName();
    KWFormat _format( doc );
    _format = *( ( KWFormat* )fc );
    unsigned int tmpTextPos = fc->getTextPos();

    int h_ = frameSet->getFrame( oldFrame - 1 )->height();
    int y_ = fc->getPTY() - 5 - contentsY();

    if ( fc->isCursorAtParagEnd() ) {
        frameSet->insertParag( fc->getParag(), I_AFTER );
        fc->setTextPos( 0 );
        recalcPage( 0L );
        if ( e->state() & ControlButton ) fc->getParag()->getNext()->setHardBreak( TRUE );
        fc->init( fc->getParag()->getNext(), e->state() & ControlButton );
    } else if ( fc->isCursorAtParagStart() ) {
        KWParag *oldFirst = frameSet->getFirstParag();
        frameSet->insertParag( fc->getParag(), I_BEFORE );
        fc->setTextPos( 0 );
        recalcPage( 0L );
        if ( e->state() & ControlButton ) fc->getParag()->setHardBreak( TRUE );
        fc->init( fc->getParag(), oldFirst != frameSet->getFirstParag() || e->state() & ControlButton );
    } else {
        bool _insert = fc->isCursorAtLineStart();
        frameSet->splitParag( fc->getParag(), tmpTextPos );
        fc->setTextPos( 0 );
        recalcPage( 0L );
        if ( ( e->state() & ControlButton ) && !_insert ) fc->getParag()->getNext()->setHardBreak( TRUE );
        fc->init( fc->getParag()->getNext(), ( e->state() & ControlButton ) && !_insert );
        if ( _insert ) {
            frameSet->insertParag( fc->getParag(), I_BEFORE );
            fc->setTextPos( 0 );
            recalcPage( 0L );
            if ( e->state() & ControlButton ) fc->getParag()->setHardBreak( TRUE );
            fc->init( fc->getParag(), e->state() & ControlButton );
        }
    }

    if ( h_ != frameSet->getFrame( oldFrame - 1 )->height() )
        redrawOnlyCurrFrameset = FALSE;

    recalcCursor( FALSE, 0 );
    doc->setModified( TRUE );

    int yp = contentsY();
    redrawAllWhileScrolling = TRUE;
    if ( redrawOnlyCurrFrameset )
        scrollClipRect = QRect( 0, y_, visibleWidth(), visibleHeight() - y_ );
    else
        scrollClipRect = QRect( 0, 0, visibleWidth(), visibleHeight() );
    scrollToCursor();
    redrawAllWhileScrolling = FALSE;
    bool scrolled = yp != contentsY();

    if ( oldPage != (int)fc->getPage() )
        gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
    if ( oldParag != fc->getParag() && fc->getParag() ) {
        gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
        setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
        setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
        gui->getView()->setFormat( *( ( KWFormat* )fc ) );
        gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
        gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
        gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                         fc->getParag()->getParagLayout()->getRightBorder(),
                                         fc->getParag()->getParagLayout()->getTopBorder(),
                                         fc->getParag()->getParagLayout()->getBottomBorder() );
        gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                            getFrame( fc->getFrame() - 1 )->x() );
        gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
    }
    if ( doc->getProcessingType() == KWordDocument::DTP && oldFrame != (int)fc->getFrame() )
        setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );

    if ( pln != fc->getParag()->getParagLayout()->getName() ) {
        format = fc->getParag()->getParagLayout()->getFormat();
        fc->apply( format );
    } else
        fc->apply( _format );
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );

    if ( scrolled ) {
        KWParag *rp = fc->getParag();
        if ( rp->getPrev() )
            rp = rp->getPrev();

        KWFrame *frame = frameSet->getFrame( fc->getFrame() - 1 );
        if ( frame )
            repaintScreen( QRect( frame->x() - contentsX(), rp->getPTYStart() - contentsY(),
                                  frame->width(), frame->height() ), FALSE );
    }

    redrawOnlyCurrFrameset = FALSE;

    return FALSE;
}

/*================================================================*/
bool KWPage::kDelete( QKeyEvent *, int, int, KWParag *, KWTextFrameSet *frameSet )
{
    unsigned int tmpTextPos = fc->getTextPos();
    bool del = fc->getParag()->deleteText( tmpTextPos, 1 );
    bool recalc = FALSE;
    bool goNext = FALSE;
    unsigned int lineEndPos;
    bool exitASAP = TRUE;

    if ( !del && fc->getParag()->getTextLen() == 0 && fc->getParag()->getNext() ) {
        KWParag *p = fc->getParag()->getNext();
        int ptYStart = fc->getParag()->getPTYStart();
        int startPage = fc->getParag()->getStartPage();
        int startFrame = fc->getParag()->getStartFrame();
        p->setPTYStart( ptYStart );
        p->setStartPage( startPage );
        p->setStartFrame( startFrame );
        frameSet->deleteParag( fc->getParag() );
        if ( p ) fc->init( p, FALSE );
        exitASAP = FALSE;
    } else if ( !del && fc->getParag()->getTextLen() > 0 ) {
        frameSet->joinParag( fc->getParag(), fc->getParag()->getNext() );
        exitASAP = FALSE;
    }

    lineEndPos = fc->getLineEndPos();

    if ( !fc->isCursorInFirstLine() ) {
        goNext = TRUE;
        fc->cursorGotoPrevLine();
    } else
        fc->makeLineLayout();

    repaintKeyEvent1( frameSet, TRUE, exitASAP );

    if ( goNext )
        fc->cursorGotoNextLine();
    recalc = lineEndPos != fc->getLineEndPos();

    if ( recalc && goNext )
        recalcCursor( FALSE, tmpTextPos );
    else
        fc->cursorGotoPos( tmpTextPos );

    doc->setModified( TRUE );
    return TRUE;
}

/*================================================================*/
bool KWPage::kBackspace( QKeyEvent *, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet )
{
    if ( fc->isCursorAtLineStart() && fc->isCursorAtParagStart() && fc->getParag() == frameSet->getFirstParag() )
        return TRUE;

    if ( fc->isCursorAtLineStart() && !fc->isCursorAtParagStart() ) {
        fc->cursorGotoLeft();

        if ( oldPage != (int)fc->getPage() )
            gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
        if ( oldParag != fc->getParag() && fc->getParag() ) {
            gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
            gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
            gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
            gui->getView()->setFormat( *( ( KWFormat* )fc ) );
            gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
            gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
            gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                             fc->getParag()->getParagLayout()->getRightBorder(),
                                             fc->getParag()->getParagLayout()->getTopBorder(),
                                             fc->getParag()->getParagLayout()->getBottomBorder() );
        }
        if ( doc->getProcessingType() == KWordDocument::DTP && oldFrame != (int)fc->getFrame() )
            setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );

        return FALSE;
    }

    unsigned int tmpTextPos = fc->getTextPos()-1;
    unsigned int paraLen = ( fc->getParag()->getPrev() ? fc->getParag()->getPrev()->getTextLen() : 0 );
    KWChar *theString = fc->getParag()->getText();
    if(fc->getTextPos()!=0 &&
          theString[tmpTextPos].attrib &&
          theString[tmpTextPos].attrib->getClassId()==ID_KWCharAnchor) {
        tmpTextPos--;
    }
    bool del = fc->getParag()->deleteText( tmpTextPos, 1 );
    bool joined = FALSE;
    bool recalc = FALSE;
    bool goNext = FALSE;
    unsigned int lineEndPos;
    bool exitASAP = TRUE;

    if ( !del && fc->getParag()->getTextLen() == 0 ) {
        KWParag *p = fc->getParag()->getPrev();
        frameSet->deleteParag( fc->getParag() );
        if ( p ) {
            fc->init( p, FALSE );
            tmpTextPos = p->getTextLen();
        }
        exitASAP = FALSE;
    } else if ( !del && fc->getParag()->getTextLen() > 0 ) {
        KWParag *p = fc->getParag()->getPrev();
        frameSet->joinParag( fc->getParag()->getPrev(), fc->getParag() );
        if ( p ) fc->init( p, FALSE );
        joined = p ? TRUE : FALSE;
        exitASAP = FALSE;
    }

    lineEndPos = fc->getLineEndPos();

    if ( !fc->isCursorInFirstLine() ) {
        goNext = TRUE;
        fc->cursorGotoPrevLine();
    } else
        fc->makeLineLayout();

    repaintKeyEvent1( frameSet, TRUE, exitASAP );

    if ( goNext )
        fc->cursorGotoNextLine();
    recalc = lineEndPos != fc->getLineEndPos();

    if ( recalc && goNext )
        recalcCursor( FALSE, tmpTextPos );
    else {
        if ( !joined ) {
            if ( !del ) {
                fc->init( fc->getParag(), FALSE );
                fc->cursorGotoLineStart();
                while ( paraLen > fc->getLineEndPos() )
                    fc->cursorGotoNextLine();
                fc->cursorGotoPos( paraLen );
            } else
                fc->cursorGotoPos( tmpTextPos );
        } else {
            fc->init( fc->getParag(), FALSE );
            fc->cursorGotoLineStart();
            while ( paraLen > fc->getLineEndPos() )
                fc->cursorGotoNextLine();
            fc->cursorGotoPos( paraLen );
        }
    }

    doc->setModified( TRUE );
    return TRUE;
}

/*================================================================*/
bool KWPage::kTab( QKeyEvent *, int, int, KWParag *, KWTextFrameSet *frameSet )
{
    if ( fc->getParag()->getParagLayout()->getTabList()->isEmpty() )
        return TRUE;

    unsigned int tmpTextPos = fc->getTextPos();
    fc->getParag()->insertTab( fc->getTextPos() );
    fc->makeLineLayout();

    repaintKeyEvent1( frameSet, FALSE );

    fc->makeLineLayout();

    if ( tmpTextPos + 1 <= fc->getLineEndPos() )
        fc->cursorGotoPos( tmpTextPos + 1 );
    else {
        fc->cursorGotoNextLine();
        fc->cursorGotoPos( tmpTextPos + 1 );
    }

    doc->updateAllViews( gui->getView() );
    doc->setModified( TRUE );

    return TRUE;
}

/*================================================================*/
bool KWPage::kDefault( QKeyEvent *e, int, int, KWParag *, KWTextFrameSet *frameSet )
{
    if ( e->text().isEmpty() )
        return TRUE;

    bool isPrev = FALSE;
    unsigned int tmpTextPos = fc->getTextPos();
    fc->getParag()->insertText( fc->getTextPos(), e->text() );
    fc->getParag()->setFormat( fc->getTextPos(), e->text().length(), format );
    fc->makeLineLayout();

    if ( e->ascii() == ' ' && !fc->isCursorInFirstLine() ) {
        fc->cursorGotoPrevLine();
        fc->makeLineLayout();
        isPrev = TRUE;
    }

    repaintKeyEvent1( frameSet, FALSE );

    if ( isPrev )
        fc->cursorGotoNextLine();

    fc->makeLineLayout();

    if ( tmpTextPos + e->text().length() <= fc->getLineEndPos() )
        fc->cursorGotoPos( tmpTextPos + e->text().length() );
    else {
        fc->cursorGotoNextLine();
        fc->cursorGotoPos( tmpTextPos + e->text().length() );
    }

    doc->updateAllViews( doc->needRedraw() ? 0L : gui->getView() );
    doc->setNeedRedraw( FALSE );
    if ( !doc->isModified() )
        doc->setModified( TRUE );
    return TRUE;
}

/*================================================================*/
void KWPage::keyPressEvent( QKeyEvent *e )
{
#define STOP { stopProcessKeyEvent(); return; }

    if ( mouseMode != MM_EDIT || e->key() == Key_Control )
        return;

    inputTimer.stop();
    formatTimer.stop();
    startProcessKeyEvent();
    editModeChanged( e );

    // if we are in a table and CTRL-Return was pressed
    if ( ( e->key() == Key_Return || e->key() == Key_Enter ) && ( e->state() & ControlButton ) &&
         doc->getFrameSet( fc->getFrameSet() - 1 )->getGroupManager() )
        if ( !kInsertTableRow() )
            STOP;

    /*============================================
     scroll up or down by one paper page height, aligning top of view
     with top of page. Uses PageUp and PageDown with CTRL. key.

     Note that pages returned from calcVisiblePages() start with 1, not 0.
     Note also that firstVisiblePage and lastVisiblePage are always different
     even when only one page is visible - first takes floor and last takes
     ceiling so there is always a difference of at least one!
     I adjust by always subtracting 1 from last visible page.
     if two or more pages are visible this will scroll up to the top of
     higher one so such pages are not skipped, then scroll a full page
     on page alignments from there on.  Scrolling down does not have this
     problem as it always goes to the start of the next page after top
     page in view, even if the lower page is partially visible to start.
    ===========================================*/

    if ((e->key() == Key_Prior || e->key() == Key_Next)
    && (e->state() & ControlButton))
    {
          calcVisiblePages();
          int ifvPage = static_cast<int>(firstVisiblePage);
          int ilvPage = static_cast<int>(lastVisiblePage) - 1;

          // determine whether current view is exactly aligned
          // to page boundaries or is in the middle of a page somewhere

          unsigned int topY = (unsigned int)contentsY();
          unsigned int leftover = topY % ptPaperHeight();

          if(e->key() == Key_Prior)
          {
              stopProcessKeyEvent();
              int priorPage = 0;

              // scroll to top of next page
              if((ifvPage == ilvPage) && (leftover < 2))
              {
                  priorPage = ifvPage - 1;
                  if(priorPage > 0)  priorPage -= 1;
              }
              else // scroll to top of current page
              {
                  priorPage = ifvPage;
                  if(priorPage > 0) priorPage -= 1;
              }

              scrollToOffset( contentsX(), (priorPage) * (ptPaperHeight()) + 1);
          }
          else if(e->key() == Key_Next)
          {
              stopProcessKeyEvent();
              int nextPage = 1;

              if(ifvPage == ilvPage)
                  nextPage = ifvPage + 1;
              else
                  nextPage =  ifvPage + 1;

              scrollToOffset( contentsX(), (nextPage - 1) * (ptPaperHeight()) + 1);
          }

          unsigned int mx = contentsX() + ptLeftBorder();
          unsigned int my = contentsY() + ptTopBorder();

          int frameset = doc->getFrameSet(mx, my);

          if ( frameset != -1
          && doc->getFrameSet( frameset )->getFrameType() == FT_TEXT )
          {
                fc->setFrameSet( frameset + 1 );
                fc->cursorGotoPixelLine( mx, my );
                fc->cursorGotoPixelInLine( mx, my );
          }

          gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

          if ( fc->getParag() )
          {
                 gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
                 gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
                 gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
                 gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
                 gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                             fc->getParag()->getParagLayout()->getRightBorder(),
                                             fc->getParag()->getParagLayout()->getTopBorder(),
                                             fc->getParag()->getParagLayout()->getBottomBorder() );
                 setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
                 setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
                 gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
                 gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
          }
          return;
    }
    // other keys modified by ControlButton
    else if ( e->state() & ControlButton ) {
        if ( e->key() == Key_B ) {
            stopProcessKeyEvent();
            gui->getView()->textBold();
            return;
        } else if ( e->key() == Key_I ) {
            stopProcessKeyEvent();
            gui->getView()->textItalic();
            return;
        } else if ( e->key() == Key_U ) {
            stopProcessKeyEvent();
            gui->getView()->textUnderline();
            return;
        }
    }


    unsigned int oldPage = fc->getPage();
    unsigned int oldFrame = fc->getFrame();
    KWParag* oldParag = fc->getParag();
    KWTextFrameSet *frameSet = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( fc->getFrameSet() - 1 ) );

    if ( doc->has_selection() )
        if ( !kContinueSelection( e ) )
            STOP;

    switch( e->key() ) {

    case Key_Prior: // PageUp
        {
        if ( !kPrior( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
        }
    case Key_Next: // PageDown
        {
        if ( !kNext( e, oldPage, oldFrame, oldParag, frameSet ) )
                  STOP;
        break;
        }
    case Key_Home:
        if ( !kHome( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_End:
        if ( !kEnd( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Right:
        if ( !kRight( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Left:
        if ( !kLeft( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Up:
        if ( !kUp( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Down:
        if ( !kDown( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Return: case Key_Enter:
        if ( !kReturn( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Delete:
        if ( !kDelete( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Backspace:
        if ( !kBackspace( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    case Key_Shift: case Key_Control: case Key_Alt: case Key_Meta:
        break;
    case Key_Tab:
        if ( !kTab( e, oldPage, oldFrame, oldParag, frameSet ) )
            STOP;
        break;
    default: {
            if ( e->text().length() &&
                 ( !e->ascii() || e->ascii() >= 32) ) {
                if ( !kDefault( e, oldPage, oldFrame, oldParag, frameSet ) )
                    STOP;
            } else if ( e->state() & ControlButton ) {
                switch( e->key() ) {
                case Key_E:
                    if ( !kEnd( e, oldPage, oldFrame, oldParag, frameSet ) )
                        STOP;
                    break;
                case Key_A:
                    if ( !kHome( e, oldPage, oldFrame, oldParag, frameSet ) )
                        STOP;
                    break;
                }
            }
        } break;
    }

    scrollToCursor();

    if ( oldPage != fc->getPage() )
        gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
    if ( oldParag != fc->getParag() && fc->getParag() )
        {
            gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
            gui->getView()->setFormat( *( ( KWFormat* )fc ) );
            gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
            gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
            gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                             fc->getParag()->getParagLayout()->getRightBorder(),
                                             fc->getParag()->getParagLayout()->getTopBorder(),
                                             fc->getParag()->getParagLayout()->getBottomBorder() );
            gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
            gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
        }
    if ( doc->getProcessingType() == KWordDocument::DTP && oldFrame != fc->getFrame() )
        setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );

    STOP;
}

/*================================================================*/
void KWPage::scrollToCursor()
{
    int cy = isCursorYVisible( *fc );
    int cx = isCursorXVisible( *fc );

    if ( cx == 0 && cy == 0 ) {
        if ( redrawAllWhileScrolling ) {
            bool drawCursor = cursorIsVisible;
            showCursor( false );
            repaintScreen( scrollClipRect, FALSE );
            showCursor( drawCursor );
        }
        return;
    }

    int oy = contentsY(), ox = contentsX();
    if ( cy < 0 ) {
        oy = fc->getPTY();
        if ( oy < 0 ) oy = 0;
    }
    else if ( cy > 0 )
        oy = fc->getPTY() - height() + fc->getLineHeight() + 10;

    if ( cx < 0 ) {
        ox = fc->getPTPos() - width() / 3;
        if ( ox < 0 ) ox = 0;
    }
    else if ( cx > 0 )
        ox = fc->getPTPos() - width() * 2 / 3;

    scrollToOffset( ox, oy );
}

/*================================================================*/
void KWPage::scrollToParag( KWParag *_parag )
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    fc->init( _parag, TRUE );
    fc->gotoStartOfParag();
    fc->cursorGotoLineStart();

    scrollToCursor();

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::scrollToOffset( int _x, int _y )
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    setContentsPos( _x, _y );

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::formatChanged( KWFormat &_format, bool _redraw, int flags )
{
    if ( editNum != -1 ) {
        if ( formulaIsActive() ) {
            QFont f( _format.getUserFont()->getFontName(), _format.getPTFontSize() );
            f.setBold( _format.getWeight() == QFont::Bold );
            f.setItalic( _format.getItalic() );
            f.setUnderline( _format.getUnderline() );
            QColor c( _format.getColor() );
            ( ( KWFormulaFrameSet* )doc->getFrameSet( editNum ) )->setFormat( f, c );
        }
        return;
    }
    format = _format;

    if ( doc->has_selection() && !inKeyEvent && _redraw ) {
        QPainter p;

        p.begin( viewport() );
        doc->drawSelection( p, contentsX(), contentsY() );
        p.end();

        doc->setSelection( FALSE );
        doc->setFormat( format, flags );
        KWFormatContext fc1( doc, doc->getSelStart()->getFrameSet() ),
            fc2( doc, doc->getSelEnd()->getFrameSet() );
        fc1 = *doc->getSelStart();
        fc2 = *doc->getSelEnd();
        recalcCursor( FALSE, -1, &fc1 );
        recalcCursor( FALSE, -1, &fc2 );
        doc->setSelStart( fc1 );
        doc->setSelEnd( fc2 );

        recalcCursor();
        doc->setSelection( TRUE );

        p.begin( viewport() );
        doc->drawSelection( p, contentsX(), contentsY() );
        p.end();
    }
}

/*================================================================*/
int KWPage::isCursorYVisible( KWFormatContext &_fc )
{
    if ( ( int )_fc.getPTY() - ( int )contentsY() < 0 )
        return -1;

    if ( _fc.getPTY() - contentsY() + _fc.getLineHeight() > ( unsigned int )height() )
        return 1;

    return 0;
}

/*================================================================*/
int KWPage::isCursorXVisible( KWFormatContext &_fc )
{
    if ( ( int )_fc.getPTPos() - ( int )contentsX() < 0 )
        return -1;

    if ( _fc.getPTPos() - contentsX() + 2 > ( unsigned int )width() )
        return 1;

    return 0;
}

/*================================================================*/
void KWPage::calcVisiblePages()
{
    firstVisiblePage = 1 + ( unsigned int )floor( ( float )contentsY() / ( float )ptPaperHeight() );
    lastVisiblePage = ( unsigned int )ceil( ( float )( contentsY() + height() ) /
                                            ( float )ptPaperHeight() ) + 1;
}

/*================================================================*/
void KWPage::drawBorders( QPainter &_painter, QRect v_area, bool drawBack, QRegion *region )
{
    _painter.save();
    _painter.setBrush( NoBrush );

    KWFrameSet *frameset = 0;
    KWFrame *tmp;
    QRect frame;
    bool should_draw;

    int cf = currFrameSet == -1 ? fc->getFrameSet() - 1 : currFrameSet;

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        if ( !frameset->isVisible() )
            continue;

        if ( isAHeader( frameset->getFrameInfo() ) && !doc->hasHeader() ||
             isAFooter( frameset->getFrameInfo() ) && !doc->hasFooter() ||
             isAWrongHeader( frameset->getFrameInfo(), doc->getHeaderType() ) ||
             isAWrongFooter( frameset->getFrameInfo(), doc->getFooterType() ) )
            continue;
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            tmp = frameset->getFrame( j );
            frame = QRect( tmp->x() - contentsX() - 1, tmp->y() - contentsY() - 1, tmp->width() + 2,
                           tmp->height() + 2 );
            if ( !v_area.intersects( frame ) )
                continue;

            _painter.setBrush( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) ?
                               frameset->getFrame( 0 )->getBackgroundColor() : tmp->getBackgroundColor() );
            _painter.setPen( lightGray );
            should_draw = TRUE;
            if ( frameset->getGroupManager() ) {
                if ( gui->getView()->getViewTableGrid() )
                    _painter.setPen( QPen( black, 1, DotLine ) );
                else
                    _painter.setPen( NoPen );
            }
            if ( FALSE )//static_cast<int>( i ) == hiliteFrameSet )
                _painter.setPen( blue );
            else if ( !gui->getView()->getViewFrameBorders() ) should_draw = FALSE;

            if ( v_area.intersects( frame ) && should_draw && !frameset->getGroupManager() ) {
                if ( region )
                    *region = region->subtract( QRect( frame.x(), frame.y(), frame.width(), frame.height() ) );
                if ( redrawOnlyCurrFrameset && cf != (int)i )
                    ;
                else {
                    if ( !drawBack )
                        _painter.setBrush( Qt::NoBrush );
                    _painter.drawRect( frame );
                }
            }

            _painter.setBrush( Qt::NoBrush );
            if ( v_area.intersects( frame ) && frameset->getGroupManager() ) {
                if ( region )
                    *region = region->subtract( QRect( frame.x(), frame.y(), frame.width(), frame.height() ) );
                if ( redrawOnlyCurrFrameset && cf != (int)i )
                    ;
                else {
                    _painter.fillRect( frame, tmp->getBackgroundColor() );
                    _painter.drawLine( tmp->right() - contentsX() + 1, tmp->y() - contentsY() - 1,
                                       tmp->right() - contentsX() + 1, tmp->bottom() - contentsY() + 1 );
                    _painter.drawLine( tmp->x() - contentsX() - 1, tmp->bottom() - contentsY() + 1,
                                       tmp->right() - contentsX() + 1, tmp->bottom() - contentsY() + 1 );
                    unsigned int row = 0, col = 0;
                    row=frameset->getGroupManager()->getCell( frameset)->row;
                    col=frameset->getGroupManager()->getCell( frameset)->col;
                    if ( row == 0 )
                        _painter.drawLine( tmp->x() - contentsX() - 1, tmp->y() - contentsY() - 1,
                                           tmp->right() - contentsX() + 1, tmp->y() - contentsY() - 1 );
                    if ( col == 0 )
                        _painter.drawLine( tmp->x() - contentsX() - 1, tmp->y() - contentsY() - 1,
                                           tmp->x() - contentsX() - 1, tmp->bottom() - contentsY() + 1 );
                }
            }

            if ( redrawOnlyCurrFrameset && cf != (int)i )
                continue;

            if ( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) )
                tmp = frameset->getFrame( 0 );
            else
                tmp = frameset->getFrame( j );
            if ( tmp->getLeftBorder().ptWidth > 0 && tmp->getLeftBorder().color !=
                 tmp->getBackgroundColor().color() ) {
                QPen p( doc->setBorderPen( tmp->getLeftBorder() ) );
                _painter.setPen( p );
                _painter.drawLine( frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.y(),
                                   frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.bottom() + 1 );
            }
            if ( tmp->getRightBorder().ptWidth > 0 && tmp->getRightBorder().color !=
                 tmp->getBackgroundColor().color() ) {
                QPen p( doc->setBorderPen( tmp->getRightBorder() ) );
                _painter.setPen( p );
                int w = tmp->getRightBorder().ptWidth;
                if ( ( w / 2 ) * 2 == w ) w--;
                w /= 2;
                _painter.drawLine( frame.right() - w, frame.y(),
                                   frame.right() - w, frame.bottom() + 1 );
            }
            if ( tmp->getTopBorder().ptWidth > 0 && tmp->getTopBorder().color !=
                 tmp->getBackgroundColor().color() ) {
                QPen p( doc->setBorderPen( tmp->getTopBorder() ) );
                _painter.setPen( p );
                _painter.drawLine( frame.x(), frame.y() + tmp->getTopBorder().ptWidth / 2,
                                   frame.right() + 1,
                                   frame.y() + tmp->getTopBorder().ptWidth / 2 );
            }
            if ( tmp->getBottomBorder().ptWidth > 0 && tmp->getBottomBorder().color !=
                 tmp->getBackgroundColor().color() ) {
                int w = tmp->getBottomBorder().ptWidth;
                if ( ( w / 2 ) * 2 == w ) w--;
                w /= 2;
                QPen p( doc->setBorderPen( tmp->getBottomBorder() ) );
                _painter.setPen( p );
                _painter.drawLine( frame.x(), frame.bottom() - w,
                                   frame.right() + 1,
                                   frame.bottom() - w );
            }
        }
    }

    if ( region ) {
        // clear empty space
        _painter.save();
        _painter.setClipRegion( *region );
        _painter.fillRect( region->boundingRect(), Qt::white );
        _painter.restore();
    }

    _painter.setPen( red );
    _painter.setBrush( Qt::NoBrush );

    for ( int k = 0; k < doc->getPages(); k++ ) {
        QRect tmp2 = QRect( -contentsX(), ( k * doc->getPTPaperHeight() ) - contentsY(), doc->getPTPaperWidth(),
                            doc->getPTPaperHeight() );
        if ( v_area.intersects( tmp2 ) )
            _painter.drawRect( tmp2 );
    }

    _painter.restore();
}

/*================================================================*/
void KWPage::drawFrameBorder( QPainter &_painter, KWFrame *tmp, int dx, int dy )
{
    if ( !tmp->getLeftBorder().ptWidth &&
         !tmp->getRightBorder().ptWidth &&
         !tmp->getTopBorder().ptWidth &&
         !tmp->getBottomBorder().ptWidth )
        return;

    QRect frame = QRect( tmp->x() - contentsX() - 1, tmp->y() - contentsY() - 1, tmp->width() + 2,
                         tmp->height() + 2 );
    if ( dx != 0 || dy != 0 )
        frame.moveBy( dx, dy );

    if ( tmp->getLeftBorder().ptWidth > 0 && tmp->getLeftBorder().color !=
         tmp->getBackgroundColor().color() ) {
        QPen p( doc->setBorderPen( tmp->getLeftBorder() ) );
        _painter.setPen( p );
        _painter.drawLine( frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.y(),
                           frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.bottom() + 1 );
    }
    if ( tmp->getRightBorder().ptWidth > 0 && tmp->getRightBorder().color !=
         tmp->getBackgroundColor().color() ) {
        QPen p( doc->setBorderPen( tmp->getRightBorder() ) );
        _painter.setPen( p );
        int w = tmp->getRightBorder().ptWidth;
        if ( ( w / 2 ) * 2 == w ) w--;
        w /= 2;
        _painter.drawLine( frame.right() - w, frame.y(),
                           frame.right() - w, frame.bottom() + 1 );
    }
    if ( tmp->getTopBorder().ptWidth > 0 && tmp->getTopBorder().color !=
         tmp->getBackgroundColor().color() ) {
        QPen p( doc->setBorderPen( tmp->getTopBorder() ) );
        _painter.setPen( p );
        _painter.drawLine( frame.x(), frame.y() + tmp->getTopBorder().ptWidth / 2,
                           frame.right() + 1,
                           frame.y() + tmp->getTopBorder().ptWidth / 2 );
    }
    if ( tmp->getBottomBorder().ptWidth > 0 && tmp->getBottomBorder().color !=
         tmp->getBackgroundColor().color() ) {
        int w = tmp->getBottomBorder().ptWidth;
        if ( ( w / 2 ) * 2 == w ) w--;
        w /= 2;
        QPen p( doc->setBorderPen( tmp->getBottomBorder() ) );
        _painter.setPen( p );
        _painter.drawLine( frame.x(), frame.bottom() - w,
                           frame.right() + 1,
                           frame.bottom() - w );
    }
}

/*================================================================*/
void KWPage::frameSizeChanged( KoPageLayout /* _layout */)
{
    setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );
}

/*================================================================*/
/* this method is depricated, use
   void KWPage::setRuler2Frame( KWFrame *frame) instead
*/
void KWPage::setRuler2Frame( unsigned int _frameset, unsigned int _frame ) {
   setRuler2Frame (doc->getFrameSet( _frameset )->getFrame( _frame ));
}

/*================================================================*/
void KWPage::setRuler2Frame( KWFrame *frame)
{
    if ( doc->getProcessingType() != KWordDocument::DTP ) return;

    KoPageLayout _layout;
    KoColumns _cl;
    KoKWHeaderFooter hf;
    doc->getPageLayout( _layout, _cl, hf );

    unsigned int page = 0;
    for ( int i = 0; i < doc->getPages(); i++ )
        {
            if ( frame->intersects( QRect( 0, i * ptPaperHeight(), ptPaperWidth(), ptPaperHeight() ) ) )
                {
                    page = i;
                    break;
                }
        }

    _layout.mmLeft = POINT_TO_MM( frame->left() );
    _layout.mmTop = POINT_TO_MM( frame->top() ) - page * doc->getMMPaperHeight();
    _layout.mmRight = _layout.mmWidth - POINT_TO_MM( frame->right() );
    _layout.mmBottom = _layout.mmHeight - POINT_TO_MM( frame->bottom() )
        + page * doc->getMMPaperHeight();
    _layout.ptLeft = frame->left();
    _layout.inchLeft = POINT_TO_INCH( frame->left() );
    _layout.inchTop = POINT_TO_INCH( frame->top() ) - page * doc->getINCHPaperHeight();
    _layout.inchRight = _layout.inchWidth - POINT_TO_INCH( frame->right() );
    _layout.inchBottom = _layout.inchHeight - POINT_TO_INCH( frame->bottom() ) + page * doc->getINCHPaperHeight();
    _layout.ptLeft = frame->left();
    _layout.ptTop = frame->top() - page * ptPaperHeight();
    _layout.ptRight = _layout.ptWidth - frame->right();
    _layout.ptBottom = _layout.ptHeight - frame->bottom() + page * ptPaperHeight();
    gui->getHorzRuler()->setPageLayout( _layout );
    gui->getVertRuler()->setPageLayout( _layout );
    gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                        getFrame( fc->getFrame() - 1 )->x() );
}

/*================================================================*/
void KWPage::setMouseMode( MouseMode _mm )
{
    if ( editNum != -1 ) {
        if ( doc->getFrameSet( editNum )->getFrameType() == FT_PART ) {
            dynamic_cast<KWPartFrameSet*>( doc->getFrameSet( editNum ) )->deactivate();
            viewport()->setFocus();
            recalcCursor( FALSE );
            editNum = -1;
        } else if ( doc->getFrameSet( editNum )->getFrameType() == FT_FORMULA ) {
            gui->getView()->showFormulaToolbar( FALSE );
            dynamic_cast<KWFormulaFrameSet*>( doc->getFrameSet( editNum ) )->deactivate();
            setFocusProxy( 0 );
            viewport()->setFocusProxy( this );
            viewport()->setFocus();
            recalcCursor( FALSE );
            editNum = -1;
            gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
        }
    }

    if ( mouseMode != _mm )
        selectAllFrames( FALSE );

    mouseMode = _mm;
    mmUncheckAll();
    gui->getView()->setTool( mouseMode );

    switch ( mouseMode ) {
    case MM_EDIT: {
        viewport()->setCursor( ibeamCursor );
        mm_menu->setItemChecked( mm_edit, TRUE );
        if ( !inKeyEvent ) {
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
            gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                getFrame( fc->getFrame() - 1 )->x() );
            gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
            if ( doc->getProcessingType() == KWordDocument::DTP )
                setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );
        }
    } break;
    case MM_EDIT_FRAME: {
        viewport()->setCursor( arrowCursor );
        mm_menu->setItemChecked( mm_edit_frame, TRUE );
    } break;
    case MM_CREATE_TEXT: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_text, TRUE );
    } break;
    case MM_CREATE_PIX: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_pix, TRUE );
    } break;
    case MM_CREATE_CLIPART: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_clipart, TRUE );
    } break;
    case MM_CREATE_TABLE: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_table, TRUE );
    } break;
    case MM_CREATE_KSPREAD_TABLE: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_kspread_table, TRUE );
    } break;
    case MM_CREATE_FORMULA: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_formula, TRUE );
    } break;
    case MM_CREATE_PART: {
        viewport()->setCursor( crossCursor );
        mm_menu->setItemChecked( mm_create_part, TRUE );
    } break;
    }

    repaintScreen( FALSE );
}

/*================================================================*/
void KWPage::setupMenus()
{
    QString pixdir;
    QPixmap pixmap;

    mm_menu = new QPopupMenu;
    CHECK_PTR( mm_menu );
    mm_edit = mm_menu->insertItem( i18n( "Edit" ), this, SLOT( mmEdit() ) );
    mm_edit_frame = mm_menu->insertItem( i18n( "Edit Frames" ), this, SLOT( mmEditFrame() ) );
    mm_create_text = mm_menu->insertItem( i18n( "Create Text-Frame" ), this, SLOT( mmCreateText() ) );
    mm_create_pix = mm_menu->insertItem( i18n( "Create Pixmap-Frame" ), this, SLOT( mmCreatePix() ) );
    mm_create_clipart = mm_menu->insertItem( i18n( "Create Clipart-Frame" ), this, SLOT( mmClipart() ) );
    mm_create_table = mm_menu->insertItem( i18n( "Create Table-Frame" ), this, SLOT( mmTable() ) );
    mm_create_kspread_table = mm_menu->insertItem( i18n( "Create KSpread Table-Frame" ), this,
                                                   SLOT( mmKSpreadTable() ) );
    mm_create_formula = mm_menu->insertItem( i18n( "Create Formula-Frame" ), this, SLOT( mmFormula() ) );
    mm_create_part = mm_menu->insertItem( i18n( "Create Part-Frame" ), this, SLOT( mmPart() ) );
    mm_menu->setCheckable( TRUE );

    frame_edit_menu = new QPopupMenu;
    CHECK_PTR( frame_edit_menu );
    frEditProps = frame_edit_menu->insertItem( i18n( "Properties..." ), this, SLOT( femProps() ) );
    frame_edit_menu->insertSeparator();
    frEditDel = frame_edit_menu->insertItem( i18n( "Delete Frame" ), this, SLOT( editDeleteFrame() ) );
    frEditReconnect = frame_edit_menu->insertItem( i18n( "Reconnect Frame" ), this, SLOT( editReconnectFrame() ) );
}

/*================================================================*/
void KWPage::mmUncheckAll()
{
    mm_menu->setItemChecked( mm_edit, FALSE );
    mm_menu->setItemChecked( mm_edit_frame, FALSE );
    mm_menu->setItemChecked( mm_create_text, FALSE );
    mm_menu->setItemChecked( mm_create_pix, FALSE );
    mm_menu->setItemChecked( mm_create_clipart, FALSE );
    mm_menu->setItemChecked( mm_create_table, FALSE );
    mm_menu->setItemChecked( mm_create_kspread_table, FALSE );
    mm_menu->setItemChecked( mm_create_formula, FALSE );
    mm_menu->setItemChecked( mm_create_part, FALSE );
}

/*================================================================*/
int KWPage::getPageOfRect( QRect _rect )
{
    for ( int i = 0; i < doc->getPages(); i++ ) {
        if ( _rect.intersects( QRect( 0, i * ptPaperHeight(), ptPaperWidth(), ptPaperHeight() ) ) )
            return i;
    }

    return -1;
}

/*================================================================*/
void KWPage::femProps()
{
    //repaintScreen( FALSE );
    KWFrame *frame=doc->getFirstSelectedFrame();
    if(frame) {
        if ( frameDia ) {
            frameDia->close();
            delete frameDia;
            frameDia = 0;
        }

        frameDia = new KWFrameDia( this, frame);
        connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
        frameDia->setCaption(i18n("Frame Properties"));
        frameDia->show();
    }
    //repaintScreen(iFrameset,true);
}

/*================================================================*/
void KWPage::newLeftIndent( double _left )
{
    KWUnit u;
    u.setPT( _left );
    setLeftIndent( u );

    switch ( KWUnit::unitType( doc->getUnit() ) ) {
    case U_MM:
        gui->getHorzRuler()->setLeftIndent( fc->getParag()->getParagLayout()->getLeftIndent().mm() );
        break;
    case U_INCH:
        gui->getHorzRuler()->setLeftIndent( fc->getParag()->getParagLayout()->getLeftIndent().inch() );
        break;
    case U_PT:
        gui->getHorzRuler()->setLeftIndent( fc->getParag()->getParagLayout()->getLeftIndent().pt() );
        break;
    }
}

/*================================================================*/
void KWPage::newFirstIndent( double _first )
{
    KWUnit u;
    u.setPT( _first );
    setFirstLineIndent( u );

    switch ( KWUnit::unitType( doc->getUnit() ) ) {
    case U_MM:
        gui->getHorzRuler()->setFirstIndent( fc->getParag()->getParagLayout()->getFirstLineLeftIndent().mm() );
        break;
    case U_INCH:
        gui->getHorzRuler()->setFirstIndent( fc->getParag()->getParagLayout()->getFirstLineLeftIndent().inch() );
        break;
    case U_PT:
        gui->getHorzRuler()->setFirstIndent( fc->getParag()->getParagLayout()->getFirstLineLeftIndent().pt() );
        break;
    }
}

/*================================================================*/
void KWPage::frameDiaClosed()
{
    hiliteFrameSet = -1;
    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    recalcAll = FALSE;
    repaintScreen (true);
    doc->updateAllFrames();
}

/*================================================================*/
void KWPage::applyStyle( QString _style )
{
    if ( !doc->has_selection() )
        fc->getParag()->applyStyle( _style );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        while ( p && p != doc->getSelBottom()->getParag()->getNext() ) {
            p->applyStyle( _style );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }

    dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( fc->getFrameSet() - 1 ) )->updateCounters();
    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    recalcAll = FALSE;
}
/*================================================================*/
void KWPage::setCounter( KWParagLayout::Counter _counter )
{
    fc->getParag()->getParagLayout()->setCounter( _counter );
    dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( fc->getFrameSet() - 1 ) )->updateCounters();
    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    recalcAll = FALSE;
}
/*================================================================*/
void KWPage::setEnumList()
{
    int f = doc->getApplyStyleTemplate();
    doc->setApplyStyleTemplate( KWordDocument::U_NUMBERING | KWordDocument::U_SMART );

    applyStyle( "Enumerated List" );

    doc->setApplyStyleTemplate( f );
    gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
}

/*================================================================*/
void KWPage::setBulletList()
{
    int f = doc->getApplyStyleTemplate();
    doc->setApplyStyleTemplate( KWordDocument::U_NUMBERING | KWordDocument::U_SMART );

    applyStyle( "Bullet List" );

    doc->setApplyStyleTemplate( f );
    gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
}

/*================================================================*/
void KWPage::setNormalText()
{
    int f = doc->getApplyStyleTemplate();
    doc->setApplyStyleTemplate( KWordDocument::U_NUMBERING | KWordDocument::U_SMART );

    applyStyle( "Standard" );

    doc->setApplyStyleTemplate( f );
    gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName() );
    gui->getView()->setFormat( *( ( KWFormat* )fc ) );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
}

/*================================================================*/
void KWPage::forceFullUpdate()
{
    gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
    gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
    gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
    gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
    gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                     fc->getParag()->getParagLayout()->getRightBorder(),
                                     fc->getParag()->getParagLayout()->getTopBorder(),
                                     fc->getParag()->getParagLayout()->getBottomBorder() );
    setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
    setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
    gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                        getFrame( fc->getFrame() - 1 )->x() );
    gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
    format = *( ( KWFormat* )fc );
}
/*================================================================*/
void KWPage::setFlow( KWParagLayout::Flow _flow )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setFlow( _flow );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setFlow( _flow );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::setLeftIndent( KWUnit _left )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setLeftIndent( _left );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setLeftIndent( _left );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::setFirstLineIndent( KWUnit _first )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setFirstLineLeftIndent( _first );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setFirstLineLeftIndent( _first );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::setSpaceBeforeParag( KWUnit _before )
{
    recalcAll = TRUE;
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setParagHeadOffset( _before );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setParagHeadOffset( _before );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
    recalcAll = FALSE;
}

/*================================================================*/
void KWPage::setSpaceAfterParag( KWUnit _after )
{
    recalcAll = TRUE;
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setParagFootOffset( _after );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setParagFootOffset( _after );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
    recalcAll = FALSE;
}

/*================================================================*/
void KWPage::setLineSpacing( KWUnit _spacing )
{
    recalcAll = TRUE;
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setLineSpacing( _spacing );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setLineSpacing( _spacing );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
    recalcAll = FALSE;
}

/*================================================================*/
void KWPage::setParagLeftBorder( Border _brd )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setLeftBorder( _brd );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setLeftBorder( _brd );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::setParagRightBorder( Border _brd )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setRightBorder( _brd );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setRightBorder( _brd );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::setParagTopBorder( Border _brd )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setTopBorder( _brd );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setTopBorder( _brd );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::setParagBottomBorder( Border _brd )
{
    if ( !doc->has_selection() )
        fc->getParag()->getParagLayout()->setBottomBorder( _brd );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->getParagLayout()->setBottomBorder( _brd );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
void KWPage::tabListChanged( QList<KoTabulator> *_tablist )
{
    gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                        getFrame( fc->getFrame() - 1 )->x() );
    if ( !doc->has_selection() )
        fc->getParag()->tabListChanged( _tablist );
    else {
        KWParag *p = doc->getSelTop()->getParag();
        KWParag *pEnd = doc->getSelBottom()->getParag();
        while ( p && p != pEnd->getNext() ) {
            p->tabListChanged( _tablist );
            p = p->getNext();
        }
        recalcCursor( FALSE, -1, doc->getSelStart() );
        recalcCursor( FALSE, -1, doc->getSelEnd() );
    }
    recalcCursor();
}

/*================================================================*/
bool KWPage::find( QString _expr, KWSearchDia::KWSearchEntry *_format,
                   bool _first, bool _cs, bool _whole,
                   bool _regexp, bool _wildcard,
                   bool &_addlen, bool _select )
{
    if ( _first || !currFindParag ) {
        for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
            if ( doc->getFrameSet( i )->getFrameType() == FT_TEXT &&
                 doc->getFrameSet( i )->getFrameInfo() == FI_BODY ) {
                currFindParag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( i ) )->getFirstParag();
                currFindPos = 0;
                currFindFS = i;
                currFindLen = 0;
                break;
            }
        }
    }

    while ( TRUE ) {
        int tmpFindPos = currFindPos;
        if ( !_regexp ) {
            currFindPos = currFindParag->find( _expr, _format, currFindPos, _cs, _whole );
            currFindLen = _expr.length();
        } else
            currFindPos = currFindParag->find( QRegExp( _expr ), _format, currFindPos, currFindLen, _cs, _wildcard );

        if ( currFindPos >= 0 ) {
            selectText( currFindPos, currFindLen, currFindFS,
                        dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( currFindFS ) ), currFindParag, _select );
            if ( _addlen )
                currFindPos += currFindLen;
            _addlen = !_addlen;
            return TRUE;
        } else {
            if ( currFindPos == -2 && tmpFindPos + currFindLen < static_cast<int>( currFindParag->getTextLen() ) )
                currFindPos = tmpFindPos + currFindLen;
            else if ( currFindParag->getNext() ) {
                currFindParag = currFindParag->getNext();
                currFindPos = 0;
            } else if ( !currFindParag->getNext() && currFindFS <= static_cast<int>( doc->getNumFrameSets() ) ) {
                currFindPos = -1;
                for ( unsigned int i = currFindFS + 1; i < doc->getNumFrameSets(); i++ ) {
                    if ( doc->getFrameSet( i )->getFrameType() == FT_TEXT &&
                         doc->getFrameSet( i )->getFrameInfo() == FI_BODY ) {
                        currFindParag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( i ) )->getFirstParag();
                        currFindPos = 0;
                        currFindFS = i;
                        currFindLen = 0;
                        break;
                    }
                }
                if ( currFindPos == -1 )
                    return FALSE;
            } else
                return FALSE;
        }
    }
}

/*================================================================*/
bool KWPage::findRev( QString _expr, KWSearchDia::KWSearchEntry *_format,
                      bool _first, bool _cs, bool _whole,
                      bool _regexp, bool _wildcard,
                      bool &_addlen, bool _select )
{
    _addlen = FALSE;
    if ( _first || !currFindParag ) {
        for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- ) {
            if ( doc->getFrameSet( i )->getFrameType() == FT_TEXT &&
                 doc->getFrameSet( i )->getFrameInfo() == FI_BODY ) {
                currFindParag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( i ) )->getLastParag();
                currFindPos = currFindParag->getTextLen() - 1;
                currFindFS = i;
                currFindLen = 0;
                break;
            }
        }
    }

    while ( TRUE ) {
        int tmpFindPos = currFindPos;
        if ( !_regexp ) {
            currFindPos = currFindParag->findRev( _expr, _format, currFindPos, _cs, _whole );
            currFindLen = _expr.length();
        } else
            currFindPos = currFindParag->findRev( QRegExp( _expr ), _format, currFindPos, currFindLen, _cs,
                                                  _wildcard );

        if ( currFindPos >= 0 ) {
            selectText( currFindPos, currFindLen, currFindFS,
                        dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( currFindFS ) ), currFindParag, _select );
            if ( currFindPos > 0 ) currFindPos--;
            return TRUE;
        } else {
            if ( currFindPos == -2 && tmpFindPos - currFindLen > 0 )
                currFindPos = tmpFindPos - currFindLen;
            else if ( currFindParag->getPrev() ) {
                currFindParag = currFindParag->getPrev();
                currFindPos = currFindParag->getTextLen() - 1;
            } else if ( !currFindParag->getPrev() && currFindFS <= static_cast<int>( doc->getNumFrameSets() ) ) {
                currFindPos = -1;
                if ( currFindFS > 0 ) {
                    for ( int i = currFindFS - 1; i >= 0; i-- ) {
                        if ( doc->getFrameSet( i )->getFrameType() == FT_TEXT &&
                             doc->getFrameSet( i )->getFrameInfo() == FI_BODY ) {
                            currFindParag = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( i ) )->getLastParag();
                            currFindPos = currFindParag->getTextLen() - 1;
                            currFindFS = i;
                            currFindLen = 0;
                            break;
                        }
                    }
                }
                if ( currFindPos == -1 )
                    return FALSE;
            } else
                return FALSE;
        }
    }
}

/*================================================================*/
void KWPage::replace( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _addlen )
{
    KWFormat *format = new KWFormat( doc );
    *format = *( dynamic_cast<KWCharFormat*>( currFindParag->getKWString()->data()[ fc->getTextPos() ].attrib )
                 ->getFormat() );

    if ( _format->checkFamily && _format->family != format->getUserFont()->getFontName() )
        format->setUserFont( doc->findUserFont( _format->family ) );
    if ( _format->checkColor && _format->color != format->getColor() )
        format->setColor( _format->color );
    if ( _format->checkSize && _format->size != format->getPTFontSize() )
        format->setPTFontSize( _format->size );
    if ( _format->checkBold && _format->bold != ( format->getWeight() == QFont::Bold ) )
        format->setWeight( _format->bold ? QFont::Bold : QFont::Normal );
    if ( _format->checkItalic && _format->italic != format->getItalic() )
        format->setItalic( _format->italic );
    if ( _format->checkUnderline && _format->underline != format->getUnderline() )
        format->setUnderline( _format->underline );
    if ( _format->checkVertAlign && _format->vertAlign != format->getVertAlign() )
        format->setVertAlign( _format->vertAlign );

    doc->getAutoFormat().setEnabled( TRUE );
    doc->deleteSelectedText( fc );

    // NOTE: KWordDoc::paste() deletes the format, so it _MUST_NOT_ be deleted here!
    doc->paste( fc, _expr, this, format );

    if ( _addlen ) currFindPos += currFindLen;

    doc->setSelection( FALSE );
    recalcText();
    recalcCursor();
    doc->getAutoFormat().setEnabled( FALSE );
}

/*================================================================*/
void KWPage::selectText( int _pos, int _len, int _frameSetNum, KWTextFrameSet */*_frameset*/, KWParag *_parag,
                         bool _select )
{
    showCursor( false );
    removeSelection();

    KWFormatContext fc1( doc, _frameSetNum + 1 );
    KWFormatContext fc2( doc, _frameSetNum + 1 );

    fc1.init( _parag, TRUE );
    fc2.init( _parag, TRUE );

    fc1.gotoStartOfParag();
    fc1.cursorGotoLineStart();
    fc2.gotoStartOfParag();
    fc2.cursorGotoLineStart();
    fc1.cursorGotoRight( _pos );
    fc2 = fc1;
    fc2.cursorGotoRight( _len );
    *fc = fc1;

    doc->setSelStart( fc1 );
    doc->setSelEnd( fc2 );

    if ( _select ) {
        QPainter p;
        p.begin( viewport() );
        doc->setSelection( TRUE );
        doc->drawSelection( p, contentsX(), contentsY() );
        p.end();
    }

    scrollToCursor();

    showCursor( true );
}

/*================================================================*/
void KWPage::removeSelection()
{
    if ( doc->has_selection() ) {
        QPainter p;
        p.begin( viewport() );
        doc->drawSelection( p, contentsX(), contentsY() );
        doc->setSelection( FALSE );
        p.end();
    }
}

/*================================================================*/
void KWPage::setLeftFrameBorder( Border _brd, bool _enable )
{
    KWFrameSet *frameset = 0L;
    KWFrame *frame = 0L;
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            frame = frameset->getFrame( j );
            if ( frame->isSelected() ) {
                if ( !_enable ) {
                    _brd.ptWidth = 1;
                    _brd.color = frame->getBackgroundColor().color();
                }
                frame->setLeftBorder( _brd );
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    doc->updateTableHeaders( grpMgrs );
    doc->updateAllViews( 0L );
}

/*================================================================*/
void KWPage::setRightFrameBorder( Border _brd, bool _enable )
{
    KWFrameSet *frameset = 0L;
    KWFrame *frame = 0L;
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            frame = frameset->getFrame( j );
            if ( frame->isSelected() ) {
                if ( !_enable ) {
                    _brd.ptWidth = 1;
                    _brd.color = frame->getBackgroundColor().color();
                }
                frame->setRightBorder( _brd );
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    doc->updateTableHeaders( grpMgrs );
    doc->updateAllViews( 0L );
}

/*================================================================*/
void KWPage::setTopFrameBorder( Border _brd, bool _enable )
{
    KWFrameSet *frameset = 0L;
    KWFrame *frame = 0L;
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            frame = frameset->getFrame( j );
            if ( frame->isSelected() ) {
                if ( !_enable ) {
                    _brd.ptWidth = 1;
                    _brd.color = frame->getBackgroundColor().color();
                }
                frame->setTopBorder( _brd );
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    doc->updateTableHeaders( grpMgrs );
    doc->updateAllViews( 0L );
}

/*================================================================*/
void KWPage::setBottomFrameBorder( Border _brd, bool _enable )
{
    KWFrameSet *frameset = 0L;
    KWFrame *frame = 0L;
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            frame = frameset->getFrame( j );
            if ( frame->isSelected() ) {
                if ( !_enable ) {
                    _brd.ptWidth = 1;
                    _brd.color = frame->getBackgroundColor().color();
                }
                frame->setBottomBorder( _brd );
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    doc->updateTableHeaders( grpMgrs );
    doc->updateAllViews( 0L );
}


/*================================================================*/
void KWPage::setFrameBorderColor( const QColor &_color )
{
    KWFrameSet *frameset = 0L;
    KWFrame *frame = 0L;
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            frame = frameset->getFrame( j );
            if ( frame->isSelected() ) {
                Border _brd;
                if(frame->getLeftBorder().color!=frame->getBackgroundColor().color()
                && frame->getLeftBorder().color!=_color
                && frame->getLeftBorder().ptWidth>0)
                {
                        _brd=frame->getLeftBorder();
                        _brd.color=_color;
                        frame->setLeftBorder(_brd);
                }
                if(frame->getRightBorder().color!=frame->getBackgroundColor().color()
                &&frame->getRightBorder().color!=_color
                        && frame->getRightBorder().ptWidth>0)
                {
                        _brd=frame->getRightBorder();
                        _brd.color=_color;
                        frame->setRightBorder(_brd);
                }
                if(frame->getTopBorder().color!=frame->getBackgroundColor().color()
                &&frame->getTopBorder().color!=_color
                && frame->getTopBorder().ptWidth>0)
                {
                        _brd=frame->getTopBorder();
                        _brd.color=_color;
                        frame->setTopBorder(_brd);
                }
                if(frame->getBottomBorder().color!=frame->getBackgroundColor().color()
                && frame->getBottomBorder().color!=_color
                && frame->getBottomBorder().ptWidth>0)
                {
                        _brd=frame->getBottomBorder();
                        _brd.color=_color;
                        frame->setBottomBorder(_brd);
                }
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    doc->updateTableHeaders( grpMgrs );
    doc->updateAllViews( 0L );
}

/*================================================================*/
void KWPage::setFrameBackgroundColor( QBrush _color )
{
    KWFrameSet *frameset = 0L;
    KWFrame *frame = 0L;
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        frameset = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            frame = frameset->getFrame( j );
            if ( frame->isSelected() ) {
                if ( frame->getLeftBorder().color == frame->getBackgroundColor().color() )
                    frame->getLeftBorder().color = _color.color();
                if ( frame->getRightBorder().color == frame->getBackgroundColor().color() )
                    frame->getRightBorder().color = _color.color();
                if ( frame->getTopBorder().color == frame->getBackgroundColor().color() )
                    frame->getTopBorder().color = _color.color();
                if ( frame->getBottomBorder().color == frame->getBackgroundColor().color() )
                    frame->getBottomBorder().color = _color.color();
                frame->setBackgroundColor( _color );
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    doc->updateTableHeaders( grpMgrs );
    doc->updateAllViews( 0L );
    repaintScreen( TRUE );
}

/*================================================================*/
KWGroupManager *KWPage::getTable()
{
    return doc->getFrameSet( fc->getFrameSet() - 1 )->getGroupManager();
}

/*================================================================*/
void KWPage::setRulerFirstIndent( KoRuler *ruler, KWUnit _value )
{
    switch ( KWUnit::unitType( doc->getUnit() ) ) {
    case U_MM:
        ruler->setFirstIndent( _value.mm() );
        break;
    case U_INCH:
        ruler->setFirstIndent( _value.inch() );
        break;
    case U_PT:
        ruler->setFirstIndent( _value.pt() );
        break;
    }
}

/*================================================================*/
void KWPage::setRulerLeftIndent( KoRuler *ruler, KWUnit _value )
{
    switch ( KWUnit::unitType( doc->getUnit() ) ) {
    case U_MM:
        ruler->setLeftIndent( _value.mm() );
        break;
    case U_INCH:
        ruler->setLeftIndent( _value.inch() );
        break;
    case U_PT:
        ruler->setLeftIndent( _value.pt() );
        break;
    }
}

/*================================================================*/
bool KWPage::editModeChanged( QKeyEvent * /*e */ )
{
    KWFrameSet *fs = doc->getFrameSet( fc->getFrameSet() - 1 );
    KWGroupManager *grpMgr = fs->getGroupManager();

    if ( grpMgr && grpMgr->isTableHeader( fs ) ) {
        grpMgr->updateTempHeaders();
        repaintTableHeaders( grpMgr );
    }

    return FALSE;
/* Who inserted the above return ??
    switch ( e->key() ) {
    case Key_Delete: {
        if ( editMode != EM_DELETE ) {
            editMode = EM_DELETE;
            doc->saveParagInUndoBuffer( fc->getParag(), fc->getFrameSet() - 1, fc );
            return TRUE;
        }
    } break;
    case Key_Backspace: {
        if ( editMode != EM_BACKSPACE ) {
            editMode = EM_BACKSPACE;
            doc->saveParagInUndoBuffer( fc->getParag(), fc->getFrameSet() - 1, fc );
            return TRUE;
        }
    } break;
    case Key_Return: case Key_Enter: {
        if ( editMode != EM_RETURN ) {
            editMode = EM_RETURN;
            doc->saveParagInUndoBuffer( fc->getParag(), fc->getFrameSet() - 1, fc );
            return TRUE;
        }
    } break;
    default: {
        if ( e->ascii() && e->ascii() > 31 ) {
            if ( editMode != EM_INSERT ) {
                editMode = EM_INSERT;
                doc->saveParagInUndoBuffer( fc->getParag(), fc->getFrameSet() - 1, fc );
                return TRUE;
            }
        } else {
            if ( editMode != EM_NONE ) {
                editMode = EM_NONE;
                return TRUE;
            }
        }
    }
    }

    return FALSE; */
}

/*================================================================*/
void KWPage::repaintTableHeaders( KWGroupManager *grpMgr )
{
    QPainter painter;

    painter.begin( viewport() );

    QRect r = grpMgr->getBoundingRect();
    r = QRect( r.x() - contentsY(), r.y() - contentsY(), r.width(), r.height() );
    painter.setClipRect( r );
    KWTextFrameSet *fs;

    KWFormatContext *paintfc = new KWFormatContext( doc, doc->getFrameSetNum( grpMgr->getCell( 0 )->frameSet ) + 1 );
    for ( unsigned int i = 0; i < grpMgr->getNumCells(); i++ ) {
        fs = dynamic_cast<KWTextFrameSet*>( grpMgr->getCell( i )->frameSet );
        if ( !fs->isRemoveableHeader() ) continue;

        KWParag *p = 0L;
        p = fs->getFirstParag();

        if ( p ) {
            paintfc->setFrameSet( doc->getFrameSetNum( grpMgr->getCell( i )->frameSet ) + 1 );
            paintfc->init( p, TRUE );

            bool bend = FALSE;
            while ( !bend ) {
                doc->printLine( *paintfc, painter, contentsX(), contentsY(), width(), height(),
                                gui->getView()->getViewFormattingChars() );
                bend = !paintfc->makeNextLineLayout();
                if ( paintfc->getPage() > lastVisiblePage )
                    bend = TRUE;
            }
        }
    }

    *formatFC = *paintfc;
    formatTimer.stop();
    formatTimer.start( 0, TRUE );

    delete paintfc;

    painter.end();
}

/*================================================================*/
/* Add an anchor at the current cursor position.                  */
/*================================================================*/
void KWPage::insertAnchor( KWCharAnchor *_anchor )
{
    // Note the origin of the anchor. This allows the drawing logic
    // to point to the anchor if required.

    _anchor->setOrigin( QPoint( fc->getPTPos(), fc->getPTY() ) );
    fc->getParag()->insertAnchor( fc->getTextPos(), _anchor );
}

/*================================================================*/
void KWPage::insertVariable( VariableType type )
{
    if ( !doc->getVarFormats().find( static_cast<int>( type ) ) ) {
        kdWarning() << "HUHU... No variable format for type " << static_cast<int>( type ) << " available!" << endl;
        return;
    }

    switch ( type ) {
    case VT_DATE_FIX: {
        KWDateVariable *var = new KWDateVariable( doc, TRUE, QDate::currentDate() );
        var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
        fc->getParag()->insertVariable( fc->getTextPos(), var );
        fc->getParag()->setFormat( fc->getTextPos(), 1, format );
    } break;
    case VT_DATE_VAR: {
        KWDateVariable *var = new KWDateVariable( doc, FALSE, QDate::currentDate() );
        var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
        fc->getParag()->insertVariable( fc->getTextPos(), var );
        fc->getParag()->setFormat( fc->getTextPos(), 1, format );
    } break;
    case VT_TIME_FIX: {
        KWTimeVariable *var = new KWTimeVariable( doc, TRUE, QTime::currentTime() );
        var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
        fc->getParag()->insertVariable( fc->getTextPos(), var );
        fc->getParag()->setFormat( fc->getTextPos(), 1, format );
    } break;
    case VT_TIME_VAR: {
        KWTimeVariable *var = new KWTimeVariable( doc, FALSE, QTime::currentTime() );
        var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
        fc->getParag()->insertVariable( fc->getTextPos(), var );
        fc->getParag()->setFormat( fc->getTextPos(), 1, format );
    } break;
    case VT_PGNUM: {
        KWPgNumVariable *var = new KWPgNumVariable( doc );
        var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
        fc->getParag()->insertVariable( fc->getTextPos(), var );
        fc->getParag()->setFormat( fc->getTextPos(), 1, format );
    } break;
    case VT_CUSTOM: {
        KWVariableNameDia *dia = new KWVariableNameDia( this, doc->getVariables() );
        if ( dia->exec() == QDialog::Accepted ) {
            KWCustomVariable *var = new KWCustomVariable( doc, dia->getName() );
            var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
            fc->getParag()->insertVariable( fc->getTextPos(), var );
            fc->getParag()->setFormat( fc->getTextPos(), 1, format );
        }
        delete dia;
    } break;
    case VT_SERIALLETTER: {
        KWSerialLetterVariableInsertDia
            *dia = new KWSerialLetterVariableInsertDia( this, doc->getSerialLetterDataBase() );
        if ( dia->exec() == QDialog::Accepted ) {
            KWSerialLetterVariable *var = new KWSerialLetterVariable( doc, dia->getName() );
            var->setVariableFormat( doc->getVarFormats().find( static_cast<int>( type ) ) );
            fc->getParag()->insertVariable( fc->getTextPos(), var );
            fc->getParag()->setFormat( fc->getTextPos(), 1, format );
        }
        delete dia;
    } break;
    default: break;
    }

    recalcPage( 0L );
    recalcCursor( TRUE );
    doc->setModified( TRUE );
}

/*================================================================*/
void KWPage::insertFootNote( KWFootNote *fn )
{
    fc->getParag()->insertFootNote( fc->getTextPos(), fn );
    doc->getFootNoteManager().insertFootNote( fn );
    KWFormat fmt( doc, format );
    if ( doc->getFootNoteManager().showFootNotesSuperscript() )
        fmt.setVertAlign( KWFormat::VA_SUPER );
    else
        fmt.setVertAlign( KWFormat::VA_NORMAL );

    fc->getParag()->setFormat( fc->getTextPos(), 1, fmt );

    recalcPage( 0L );
    recalcCursor( TRUE );
    doc->setModified( TRUE );
}

/*================================================================*/
void KWPage::startDrag()
{
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();

    KWordDrag *drag = new KWordDrag( this );

    if ( doc->has_selection() )
        doc->copySelectedText();

    QClipboard *cb = QApplication::clipboard();
    drag->setKWord( cb->data()->encodedData( MIME_TYPE ) );
    drag->setPlain( cb->data()->encodedData( "text/plain" ) );
    if ( drag->drag() ) {
        if ( pasteLaterData.isEmpty() ) {
            doc->deleteSelectedText( fc );
            doc->setSelection( FALSE );
            recalcCursor();
        }
        pasteLaterData = 0;
    }

    if ( blinking )
        startBlinkCursor();
}

/*================================================================*/
void KWPage::viewportDragEnterEvent( QDragEnterEvent *e )
{
    stopBlinkCursor();
    if ( KWordDrag::canDecode( e ) ||
         QTextDrag::canDecode( e ) ||
         QImageDrag::canDecode( e ) ||
         QUriDrag::canDecode( e ) )
        e->accept();//Action();
}

/*================================================================*/
void KWPage::viewportDragMoveEvent( QDragMoveEvent *e )
{
    if ( KWordDrag::canDecode( e ) ||
         QTextDrag::canDecode(e ) ||
         QImageDrag::canDecode( e ) ||
         QUriDrag::canDecode( e ) ) {
        if ( mouseMode != MM_EDIT )
            setMouseMode( MM_EDIT );

        unsigned int mx = e->pos().x() + contentsX();
        unsigned int my = e->pos().y() + contentsY();

        showCursor( false );

        int frameset = doc->getFrameSet( mx, my );

        selectedFrameSet = selectedFrame = -1;
        if ( frameset != -1 && doc->getFrameSet( frameset )->getFrameType() == FT_TEXT ) {
            fc->setFrameSet( frameset + 1 );

            fc->cursorGotoPixelLine( mx, my );
            fc->cursorGotoPixelInLine( mx, my );

            scrollToCursor();

            showCursor( true );

            if ( doc->getProcessingType() == KWordDocument::DTP ) {
                int frame = doc->getFrameSet( frameset )->getFrame( mx, my );
                if ( frame != -1 ) {
                    if ( doc->getProcessingType() == KWordDocument::DTP )
                        setRuler2Frame( frameset, frame );
                }
                selectedFrame = frame;
                selectedFrameSet = frameset;
            }

            gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

            if ( fc->getParag() ) {
                gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
                gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
                gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
                gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
                gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                                 fc->getParag()->getParagLayout()->getRightBorder(),
                                                 fc->getParag()->getParagLayout()->getTopBorder(),
                                                 fc->getParag()->getParagLayout()->getBottomBorder() );
                setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
                setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
                gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                    getFrame( fc->getFrame() - 1 )->x() );
                gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
            }

            e->accept();//Action();
        } else {
            e->accept();//Action();
            showCursor( true );
        }
    } else
        e->ignore();
}

/*================================================================*/
void KWPage::viewportDragLeaveEvent( QDragLeaveEvent * )
{
    startBlinkCursor();
}

/*================================================================*/
void KWPage::viewportDropEvent( QDropEvent *e )
{
    QDropEvent *drop = e;

    if ( KWordDrag::canDecode( e ) ) {
        if ( drop->provides( MIME_TYPE ) ) {
            e->ignore();
            return;
            if ( drop->encodedData( MIME_TYPE ).size() ) {
                if ( ( drop->source() == this || drop->source() == viewport() ) &&
                     drop->action() == QDropEvent::Move ) { // #### todo
                    editPaste( drop->encodedData( MIME_TYPE ), MIME_TYPE );
                    KWFormatContext oldFc( doc, fc->getFrameSet() );
                    oldFc = *fc;
                    doc->deleteSelectedText( fc );
                    doc->setSelection( FALSE );
                    *fc = oldFc;
                    recalcCursor();
                    pasteLaterData = drop->encodedData( MIME_TYPE );
                } else
                    editPaste( drop->encodedData( MIME_TYPE ), MIME_TYPE );
            }
        } else if ( drop->provides( "text/plain" ) ) {
            e->ignore();
            return;
            if ( drop->encodedData( "text/plain" ).size() ) {
                if ( ( drop->source() == this || drop->source() == viewport() ) &&
                     drop->action() == QDropEvent::Move ) { // #### todo
                    editPaste( drop->encodedData( "text/plain" ) );
                    KWFormatContext oldFc( doc, fc->getFrameSet() );
                    oldFc = *fc;
                    doc->deleteSelectedText( fc );
                    doc->setSelection( FALSE );
                    *fc = oldFc;
                    recalcCursor();
                    pasteLaterData = drop->encodedData( "text/plain" );
                } else
                    editPaste( drop->encodedData( "text/plain" ) );
            }
        }
        e->accept();//Action();
    } else if ( QImageDrag::canDecode( e ) ) {
        QImage pix;
        QImageDrag::decode( e, pix );

        QString uid = getenv( "USER" );
        QString num;
        num.setNum( doc->getNumFrameSets() );
        uid += "_";
        uid += num;

        QString filename = "/tmp/kword";
        filename += uid;
        filename += ".png";

        pix.save( filename, "PNG" );
        KWPictureFrameSet *frameset = new KWPictureFrameSet( doc );
        frameset->setFileName( filename, QSize( pix.width(), pix.height() ) );
        KWFrame *frame = new KWFrame(frameset, e->pos().x() + contentsX(), e->pos().y() + contentsY(), pix.width(),
                                     pix.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        repaintScreen( FALSE );

        QString cmd = "rm -f ";
        cmd += filename;
        system( cmd.ascii() );
        e->accept();//Action();
    } else if ( QUriDrag::canDecode( e ) ) {

        QStringList lst;
        QUriDrag::decodeLocalFiles( e, lst );

        QStringList::Iterator it = lst.begin();
        for ( ;it != lst.end(); ++it ) {
            KURL url( *it );
            if ( !url.isLocalFile() )
                return;

            QString filename = url.path();
            KMimeMagicResult *res = KMimeMagic::self()->findFileType( filename );

            if ( res && res->isValid() ) {
                QString mimetype = res->mimeType();
                if ( mimetype.contains( "image" ) ) {
                    QPixmap pix( filename );
                    KWPictureFrameSet *frameset = new KWPictureFrameSet( doc );
                    frameset->setFileName( filename, QSize( pix.width(), pix.height() ) );
                    KWFrame *frame = new KWFrame(frameset, e->pos().x() + contentsX(), e->pos().y() + contentsY(),
                                                 pix.width(), pix.height() );
                    frameset->addFrame( frame );
                    doc->addFrameSet( frameset );
                    repaintScreen( FALSE );
                    continue;
                }
            }

            //          open any non-picture as text
            //          in the future we should open specific mime types with "their" programms and embed them
            QFile f( filename );
            QTextStream t( &f );
            QString text = "", tmp;

            if ( f.open( IO_ReadOnly ) ) {
                while ( !t.eof() ) {
                    tmp = t.readLine();
                    tmp += "\n";
                    text.append( tmp );
                }
                f.close();
            }
            doc->getAutoFormat().setEnabled( TRUE );
            doc->paste( fc, text, this );
            repaintScreen( FALSE );
            doc->getAutoFormat().setEnabled( FALSE );
        }
        e->accept();
    } else if ( QTextDrag::canDecode( e ) ) {
        QString s;
        QTextDrag::decode( e, s );
        editPaste( s );
        e->accept();//Action();
    }

    startBlinkCursor();
}

/*================================================================*/
bool KWPage::isInSelection( KWFormatContext *_fc )
{
    if ( !doc->has_selection() )
        return FALSE;

    if ( doc->getSelTop()->getParag() == _fc->getParag() ) {
        if ( _fc->getTextPos() >= doc->getSelTop()->getTextPos() ) {
            if ( doc->getSelTop()->getParag() == doc->getSelBottom()->getParag() ) {
                if ( _fc->getTextPos() <= doc->getSelBottom()->getTextPos() )
                    return TRUE;
                return FALSE;
            }
            return TRUE;
        }
        return FALSE;
    }

    if ( doc->getSelTop()->getParag() == doc->getSelBottom()->getParag() )
        return FALSE;

    if ( doc->getSelBottom()->getParag() == _fc->getParag() ) {
        if ( _fc->getTextPos() <= doc->getSelBottom()->getTextPos() )
            return TRUE;
        return FALSE;
    }

    KWParag *parag = doc->getSelTop()->getParag()->getNext();

    while ( parag && parag != doc->getSelBottom()->getParag() ) {
        if ( parag == _fc->getParag() )
            return TRUE;

        parag = parag->getNext();
    }

    return FALSE;
}

/*================================================================*/
void KWPage::startBlinkCursor()
{
    showCursor(true);
    blinkTimer.start( 1000 );
}

/*================================================================*/
void KWPage::blinkCursor()
{
    showCursor(!cursorIsVisible);

    blinkTimer.changeInterval( kapp->cursorFlashTime() / 2 );
}

/*================================================================*/
void KWPage::stopBlinkCursor( bool visible )
{
    showCursor( visible );

    blinkTimer.stop();
}

/*================================================================*/
void KWPage::showCursor( bool visible )
{
    if (visible == cursorIsVisible)
        return;

    cursorIsVisible = !cursorIsVisible;

    QPainter p;
    p.begin( viewport() );
    doc->drawMarker( *fc, &p, contentsX(), contentsY() );
    p.end();
}


void KWPage::keyReleaseEvent( QKeyEvent * )
{
    startBlinkCursor();
}

/*================================================================*/
void KWPage::focusInEvent( QFocusEvent * )
{
    startBlinkCursor();
}

/*================================================================*/
void KWPage::focusOutEvent( QFocusEvent * )
{
    stopBlinkCursor(true);
}

/*================================================================*/
void KWPage::clear()
{
}

/*================================================================*/
void KWPage::resizeContents( int w, int h )
{
    QScrollView::resizeContents( w, h );
    calcVisiblePages();
}

/*================================================================*/
void KWPage::viewportResizeEvent( QResizeEvent *e )
{
    QScrollView::viewportResizeEvent( e );
    calcVisiblePages();
}

/*================================================================*/
void KWPage::setContentsPos( int x, int y )
{
    QScrollView::setContentsPos( x, y );
    calcVisiblePages();
}

/*================================================================*/
void KWPage::repaintScreen( bool erase )
{
    _erase = erase;
    _setErase = TRUE;
    viewport()->repaint( FALSE );
}

/*================================================================*/
void KWPage::repaintScreen( const QRect &r, bool erase )
{
    _erase = erase;
    _setErase = TRUE;
    viewport()->repaint( r, FALSE );
}

/*================================================================*/
void KWPage::repaintScreen( int currFS, bool erase )
{
    _erase = erase;
    _setErase = TRUE;
    currFrameSet = currFS;
    redrawOnlyCurrFrameset = TRUE;
    viewport()->repaint( FALSE );
    currFrameSet = -1;
    redrawOnlyCurrFrameset = FALSE;
}

/*================================================================*/
void KWPage::contentsWillMove( int x , int y )
{
    calcVisiblePages();
    gui->getVertRuler()->setOffset( 0, -getVertRulerPos(y) );
    gui->getHorzRuler()->setOffset( -getHorzRulerPos(x), 0 );
    _scrolled = TRUE;
}

/*================================================================*/
void KWPage::startKeySelection()
{
    doc->setSelStart( *fc );
    doc->setSelEnd( *fc );
    doc->setSelection( TRUE );
    *oldFc = *fc;
}

/*================================================================*/
void KWPage::continueKeySelection()
{
    bool flickerAndSlow = FALSE;

    int cy = isCursorYVisible( *fc );
    if ( cy != 0 ) {
        if ( cy < 0 )
            if ( *doc->getSelStart() < *fc )
                flickerAndSlow = TRUE;
        if ( cy > 0 )
            if ( *doc->getSelStart() > *fc )
                flickerAndSlow = TRUE;
    }

    if ( !continueSelection || flickerAndSlow ) {
        QPainter painter;
        painter.begin( viewport() );
        doc->drawSelection( painter, contentsX(), contentsY() );
        doc->setSelEnd( *fc );
        doc->setSelection( FALSE );
        painter.end();
        scrollToCursor();
        doc->setSelection( TRUE );
        painter.begin( viewport() );
        doc->drawSelection( painter, contentsX(), contentsY() );
        painter.end();
    } else {
        scrollToCursor();
        doc->setSelEnd( *fc );
        QPainter painter;
        painter.begin( viewport() );
        doc->drawSelection( painter, contentsX(), contentsY(),
                            oldFc, fc );
        painter.end();
    }
}

/*================================================================*/
void KWPage::doAutoScroll()
{
    QPoint pos = QCursor::pos();
    pos = viewport()->mapFromGlobal( pos );
    int mx = pos.x() + contentsX();
    int my = pos.y() + contentsY();

    if ( pos.y() < 0 || pos.y() > viewport()->height() )
        {
            ensureVisible( contentsX(), my, 0, 5 );
            doSelect(mx, my);
        }
}

/*================================================================*/
void KWPage::doSelect(int mx, int my)
{
    int frameset = doc->getFrameSet( mx, my );

    if ( frameset != -1 && frameset == static_cast<int>( fc->getFrameSet() ) - 1 &&
         doc->getFrameSet( frameset )->getFrameType() == FT_TEXT ) {
        *oldFc = *fc;

        showCursor( false );

        fc->setFrameSet( frameset + 1 );

        fc->cursorGotoPixelLine( mx, my );
        fc->cursorGotoPixelInLine( mx, my );

        continueSelection = TRUE;
        continueKeySelection();
        continueSelection = FALSE;

        if ( doc->getProcessingType() == KWordDocument::DTP )
            setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );

        gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

        if ( fc->getParag() ) {
            setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
            setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
        }
    }
}

/*================================================================*/
void KWPage::selectAllFrames( bool select )
{
    KWFrameSet *fs = 0;
    KWFrame *frame = 0;

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); ++i ) {
        fs = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < fs->getNumFrames(); ++j ) {
            frame = fs->getFrame( j );
            if ( frame->isSelected() != select ) {
                bool s = frame->isSelected();
                frame->setSelected( select );
                if ( select )
                    frame->createResizeHandles();
                else if ( s )
                    frame->removeResizeHandles();
            }
        }
    }
}

/*================================================================*/
void KWPage::selectFrame( int mx, int my, bool select )
{
    int fs = doc->getFrameSet( mx, my );
    if ( fs != -1 ) {
        KWFrameSet *frameset = doc->getFrameSet( fs );
        int frm = frameset->getFrame( mx, my );
        if ( frm != -1 ) {
            KWFrame *frame = frameset->getFrame( frm );
            if ( frame->isSelected() != select ) {
                bool s = frame->isSelected();
                frame->setSelected( select );
                if ( select )
                    frame->createResizeHandles();
                else if ( s )
                    frame->removeResizeHandles();
            }
        }
    }
}

/*================================================================*/
void KWPage::selectAll()
{
    KWFormatContext *sfc = new KWFormatContext( doc, fc->getFrameSet() );
    *sfc = *fc;

    KWFormatContext *efc = new KWFormatContext( doc, fc->getFrameSet() );
    *efc = *fc;

    sfc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( fc->getFrameSet() - 1 ) )
               ->getFirstParag() );
    sfc->cursorGotoLineStart();
    efc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( fc->getFrameSet() - 1 ) )
               ->getLastParag(), TRUE );

    doc->setSelStart( *sfc );
    doc->setSelEnd( *efc );
    doc->setSelection( TRUE );

    repaintScreen( TRUE );
}

/*================================================================*/
bool KWPage::focusNextPrevChild( bool )
{
    return FALSE;
}

/*================================================================*/
void KWPage::formatMore()
{
    if ( inKeyEvent || !formatFC )
        return;

    if ( doc->getFrameSet( formatFC->getFrameSet() - 1)->getGroupManager() )
        return;

    //qDebug( "formatMore" );
    bool bend = FALSE;

    int i = 0;
    while ( !bend && i < 15 ) {
        bend = !formatFC->makeNextLineLayout( FALSE );
        ++i;
    }

    if ( !bend )
        formatTimer.start( 0, TRUE );
}

/*================================================================*/
void KWPage::noInput()
{
    formatTimer.start( 0, TRUE );
}

/*================================================================*/
void KWPage::insertFormulaChar( int c )
{
    if ( editNum != -1 && doc->getFrameSet( editNum )->getFrameType() == FT_FORMULA )
        ( ( KWFormulaFrameSet* )doc->getFrameSet( editNum ) )->insertChar( c );
    doc->setModified( TRUE );
}

/*================================================================*/
bool KWPage::formulaIsActive() const
{
    return ( editNum != -1 &&
             doc->getFrameSet( editNum )->getFrameType() == FT_FORMULA );
}

/*================================================================*/
void KWPage::updateSelections()
{
    if ( mouseMode != MM_EDIT_FRAME )
        return;

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); ++i ) {
        for ( unsigned int j = 0; j < doc->getFrameSet( i )->getNumFrames(); ++j ) {
            KWFrame *frame = doc->getFrameSet( i )->getFrame( j );
            if ( frame->isSelected() ) {
                frame->updateResizeHandles();
            }
        }
    }
}

/*================================================================*/
void KWPage::cursorGotoNextTableCell()
{
    KWTextFrameSet *fs = (KWTextFrameSet*)doc->getFrameSet( fc->getFrameSet() - 1 );
    KWGroupManager::Cell *cell = 0L;
    for ( unsigned int i = 0; i < fs->getGroupManager()->getNumCells(); ++i ) {
        if ( fs->getGroupManager()->getCell( i )->frameSet == fs ) {
            cell = fs->getGroupManager()->getCell( i );
            break;
        }
    }

    if ( !cell )
        return;

    if ( cell->col < fs->getGroupManager()->getCols() - 1 )
        fs = (KWTextFrameSet*)fs->getGroupManager()->getCell( cell->row, cell->col + 1 )->frameSet;
    else if ( cell->row < fs->getGroupManager()->getRows() - 1 )
        fs = (KWTextFrameSet*)fs->getGroupManager()->getCell( cell->row + 1, 0 )->frameSet;
    else
        fs = (KWTextFrameSet*)fs->getGroupManager()->getCell( 0, 0 )->frameSet;

    int frameset = doc->getFrameSetNum( fs );

    fc->setFrameSet( frameset + 1 );
    fc->init( fs->getFirstParag() );
    fc->cursorGotoLineStart();
    scrollToCursor();
    if ( doc->getProcessingType() == KWordDocument::DTP ) {
        int frame = 0;
        if ( frame != -1 ) {
            if ( doc->getProcessingType() == KWordDocument::DTP )
                setRuler2Frame( frameset, frame );
        }
        selectedFrame = frame;
        selectedFrameSet = frameset;
    }

    gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

    if ( fc->getParag() ) {
        gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
        gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
        gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
        gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
        gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                         fc->getParag()->getParagLayout()->getRightBorder(),
                                         fc->getParag()->getParagLayout()->getTopBorder(),
                                         fc->getParag()->getParagLayout()->getBottomBorder() );
        setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
        setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
        gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                            getFrame( fc->getFrame() - 1 )->x() );
        gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
        format = *( ( KWFormat* )fc );
    }
}

/*================================================================*/
void KWPage::cursorGotoPrevTableCell()
{
    KWTextFrameSet *fs = (KWTextFrameSet*)doc->getFrameSet( fc->getFrameSet() - 1 );
    KWGroupManager::Cell *cell = 0L;
    for ( unsigned int i = 0; i < fs->getGroupManager()->getNumCells(); ++i ) {
        if ( fs->getGroupManager()->getCell( i )->frameSet == fs ) {
            cell = fs->getGroupManager()->getCell( i );
            break;
        }
    }

    if ( !cell )
        return;

    if ( cell->col > 0 )
        fs = (KWTextFrameSet*)fs->getGroupManager()->getCell( cell->row, cell->col - 1 )->frameSet;
    else if ( cell->row > 0 )
        fs = (KWTextFrameSet*)fs->getGroupManager()->getCell( cell->row - 1,
                                                              fs->getGroupManager()->getCols() - 1 )->frameSet;
    else
        fs = (KWTextFrameSet*)fs->getGroupManager()->getCell( fs->getGroupManager()->getRows() - 1,
                                                              fs->getGroupManager()->getCols() - 1 )->frameSet;

    int frameset = doc->getFrameSetNum( fs );

    fc->setFrameSet( frameset + 1 );
    fc->init( fs->getFirstParag() );
    fc->cursorGotoLineStart();
    scrollToCursor();
    if ( doc->getProcessingType() == KWordDocument::DTP ) {
        int frame = 0;
        if ( frame != -1 ) {
            if ( doc->getProcessingType() == KWordDocument::DTP )
                setRuler2Frame( frameset, frame );
        }
        selectedFrame = frame;
        selectedFrameSet = frameset;
    }

    gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );

    if ( fc->getParag() ) {
        gui->getView()->updateStyle( fc->getParag()->getParagLayout()->getName(), FALSE );
        gui->getView()->setFormat( *( ( KWFormat* )fc ), TRUE, FALSE );
        gui->getView()->setFlow( fc->getParag()->getParagLayout()->getFlow() );
        gui->getView()->setLineSpacing( fc->getParag()->getParagLayout()->getLineSpacing().pt() );
        gui->getView()->setParagBorders( fc->getParag()->getParagLayout()->getLeftBorder(),
                                         fc->getParag()->getParagLayout()->getRightBorder(),
                                         fc->getParag()->getParagLayout()->getTopBorder(),
                                         fc->getParag()->getParagLayout()->getBottomBorder() );
        setRulerFirstIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
        setRulerLeftIndent( gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
        gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                            getFrame( fc->getFrame() - 1 )->x() );
        gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
        format = *( ( KWFormat* )fc );
    }
}

/******************************************************************/
/* Class: KWResizeHandle                                          */
/******************************************************************/

/*================================================================*/
KWResizeHandle::KWResizeHandle( KWPage *p, Direction d, KWFrame *frm )
    : QWidget( p->viewport() ), page( p ), direction( d ), frame( frm )
{
    mousePressed = FALSE;
    setMouseTracking( TRUE );
    setBackgroundMode( PaletteHighlight );

    switch ( direction ) {
    case LeftUp:
        setCursor( Qt::sizeFDiagCursor );
        break;
    case Up:
        setCursor( Qt::sizeVerCursor );
        break;
    case RightUp:
        setCursor( Qt::sizeBDiagCursor );
        break;
    case Right:
        setCursor( Qt::sizeHorCursor );
        break;
    case RightDown:
        setCursor( Qt::sizeFDiagCursor );
        break;
    case Down:
        setCursor( Qt::sizeVerCursor );
        break;
    case LeftDown:
        setCursor( Qt::sizeBDiagCursor );
        break;
    case Left:
        setCursor( Qt::sizeHorCursor );
        break;
    }

    updateGeometry();
    show();
}

/*================================================================*/
void KWResizeHandle::mouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed )
        return;
    page->mouseMoved = TRUE;

    int my = y() + e->y();
    int mx = x() + e->x();
    mx += page->contentsX();
    my += page->contentsY();
    mx = ( mx / page->doc->getRastX() ) * page->doc->getRastX();
    my = ( my / page->doc->getRastY() ) * page->doc->getRastY();
    switch ( direction ) {
    case LeftUp:
        page->vmmEditFrameResize( mx, my, true, false, true, false );
        break;
    case Up:
        page->vmmEditFrameResize( mx, my ,true, false, false, false);
        break;
    case RightUp:
        page->vmmEditFrameResize( mx, my, true, false, false, true );
        break;
    case Right:
        page->vmmEditFrameResize( mx, my, false, false, false, true );
        break;
    case RightDown:
        page->vmmEditFrameResize( mx, my, false, true, false, true );
        break;
    case Down:
        page->vmmEditFrameResize( mx, my, false, true, false, false );
        break;
    case LeftDown:
        page->vmmEditFrameResize( mx, my, false, true, true, false );
        break;
    case Left:
        page->vmmEditFrameResize( mx, my , false, false, true, false);
        break;
    }
    page->oldMy = my;
    page->oldMx = mx;
    page->deleteMovingRect = TRUE;
    page->doRaster = TRUE;
    page->doc->setModified( TRUE );
}

/*================================================================*/
void KWResizeHandle::mousePressEvent( QMouseEvent *e )
{
    KWFrameSet *fs = 0;
    KWFrame *frm = 0;

    for ( unsigned int i = 0; i < page->doc->getNumFrameSets(); ++i ) {
        fs = page->doc->getFrameSet( i );
        for ( unsigned int j = 0; j < fs->getNumFrames(); ++j ) {
            frm = fs->getFrame( j );
            if ( frame->isSelected() && frm != frame ) {
                frm->setSelected( FALSE );
                frm->removeResizeHandles();
            }
        }
    }

    mousePressed = TRUE;
    oldX = e->x();
    oldY = e->y();
    page->mouseMoved = FALSE;
    page->mousePressed = TRUE;
    page->vmpEditFrame( 0, x() + e->x() + page->contentsX(),
                        y() + e->y() + page->contentsY() );
    page->doc->setModified( TRUE );
}

/*================================================================*/
void KWResizeHandle::mouseReleaseEvent( QMouseEvent *e )
{
    mousePressed = FALSE;
    page->vmrEditFrame( x() + e->x() + page->contentsX(),
                        y() + e->y() + page->contentsY() );
    page->mousePressed = FALSE;
}

/*================================================================*/
void KWResizeHandle::updateGeometry()
{
    switch ( direction ) {
    case LeftUp:
        page->moveChild( this, frame->x(), frame->y() );
        break;
    case Up:
        page->moveChild( this, frame->x() + frame->width() / 2 - 3, frame->y() );
        break;
    case RightUp:
        page->moveChild( this, frame->x() + frame->width() - 6, frame->y() );
        break;
    case Right:
        page->moveChild( this, frame->x() + frame->width() - 6,
                         frame->y() + frame->height() / 2 - 3 );
        break;
    case RightDown:
        page->moveChild( this, frame->x() + frame->width() - 6,
                         frame->y() + frame->height() - 6 );
        break;
    case Down:
        page->moveChild( this, frame->x() + frame->width() / 2 - 3,
                         frame->y() + frame->height() - 5 );
        break;
    case LeftDown:
        page->moveChild( this, frame->x(), frame->y() + frame->height() - 6 );
        break;
    case Left:
        page->moveChild( this, frame->x(), frame->y() + frame->height() / 2 - 3 );
        break;
    }
    resize( 6, 6 );
}

/*================================================================*/
KWPage *KWResizeHandle::getPage() {
    return page;
}

