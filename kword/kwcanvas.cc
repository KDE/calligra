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

#include "qrichtext_p.h"
#include "kwcanvas.h"
#include "kwframe.h"
#include "kwtextframeset.h"
#include "kwgroupmanager.h"
#include "kwdoc.h"
#include "kwview.h"
#include "kwtextparag.h"
#include "framedia.h"
#include "kwcommand.h"

#include <qtimer.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <kaction.h>
#include <kdebug.h>
#include <kmessagebox.h>

KWCanvas::KWCanvas(QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent ), doc( d )
{
    m_gui = lGui;
    m_currentFrameSet = doc->getFrameSet( 0 );
    m_currentFrameSetEdit = m_currentFrameSet->createFrameSetEdit( this );
    drawAll = TRUE;
    mousePressed = FALSE;
    setMouseMode( MM_EDIT );

    trows = 7;
    tcols = 5;
    twid = TblAuto;
    thei = TblAuto;

    curTable = 0L;

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setAcceptDrops( TRUE );

    setKeyCompression( TRUE );
    viewport()->setMouseTracking( TRUE );
    viewport()->setCursor( ibeamCursor );

    scrollTimer = new QTimer( this );
    connect( scrollTimer, SIGNAL( timeout() ),
	     this, SLOT( doAutoScroll() ) );

    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    setFocus();
    viewport()->installEventFilter( this );
    installEventFilter( this );

    connect( this, SIGNAL(contentsMoving( int, int )),
             this, SLOT(slotContentsMoving( int, int )) );

    connect( doc, SIGNAL( sig_newContentsSize( int, int ) ),
             this, SLOT( resizeContents( int, int ) ) );
    resizeContents( doc->ptPaperWidth(), doc->ptPaperHeight() * doc->getPages() );

    // Add an action for debugging
    (void) new KAction( "Print richtext debug info" , 0,
                        this, SLOT( printRTDebug() ),
                        m_gui->getView()->actionCollection(), "printrtdebug" );
}

KWCanvas::~KWCanvas()
{
    delete m_currentFrameSetEdit;
    m_currentFrameSetEdit = 0L;
    selectAllFrames( false ); // destroy resize handles properly (they are our children at the Qt level!)
}

void KWCanvas::repaintChanged()
{
    drawAll = FALSE;
    viewport()->repaint( FALSE );
    drawAll = TRUE;
}

void KWCanvas::repaintAll( bool erase /* = false */ )
{
    viewport()->repaint( erase );
}

// Return true if @p r is out of the page @p page
bool KWCanvas::isOutOfPage( QRect & r, int page ) const
{
    return r.x() < 0 ||
        r.right() > static_cast<int>( doc->ptPaperWidth() ) ||
        r.y() < page * static_cast<int>( doc->ptPaperHeight() ) ||
        r.bottom() > ( page + 1 ) * static_cast<int>( doc->ptPaperHeight() );
}

void KWCanvas::drawContents( QPainter *painter, int cx, int cy, int cw, int ch )
{
    bool focus = hasFocus() || viewport()->hasFocus();
#if 0
    // Can't see what this is good for
    if ( contentsY() == 0 ) {
        QColorGroup g = colorGroup();
	painter->fillRect( contentsX(), contentsY(), visibleWidth(), m_currentFrameSet->textDocument()->y(),
                           g.brush( QColorGroup::Base ) );
    }
#endif
    painter->setBrushOrigin( -contentsX(), -contentsY() );
    // Note: this is drawContents. The painter is already translated correctly.

    // Looks like this excludes one pixel on the left and one on the right
    QRect crect( cx, cy, cw, ch );
    QRegion cr( crect );
    QRegion r = cr;
    if ( contentsX() == 0 ) {
        r = QRegion( QRect( 1, 0, doc->ptPaperWidth() - 2, visibleHeight() ) );
        r = r.intersect( cr );
    }
    bool _erase = drawAll; // ### To be checked. Old KWord had stuff like if (_scrolled) erase=true; etc.
    drawBorders( painter, crect, _erase, &r );

    // Draw all framesets
    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( frameset->isVisible() )
        {
            //if ( redrawOnlyCurrFrameset && frameset != m_currentFrameSet )
            //    continue;

            QColorGroup gb=QApplication::palette().active();
            if ( focus && frameset == m_currentFrameSet && m_currentFrameSetEdit )
                m_currentFrameSetEdit->drawContents( painter, cx, cy, cw, ch, gb, !drawAll );
            else
                frameset->drawContents( painter, cx, cy, cw, ch, gb, !drawAll );
        }
    }

#if 0
    if ( contentsHeight() < visibleHeight() && ( !m_currentFrameSet->textDocument()->lastParag() ||
						 m_currentFrameSet->textDocument()->lastParag()->isValid() ) && drawAll )
	painter->fillRect( 0, contentsHeight(), visibleWidth(),
                           visibleHeight() - contentsHeight(), colorGroup().brush( QColorGroup::Base ) );
#endif
}

void KWCanvas::drawBorders( QPainter *painter, QRect v_area, bool drawBack, QRegion *region )
{
    painter->save();
    painter->setBrush( NoBrush );

    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameset = fit.current();
        if ( !frameset->isVisible() )
            continue;

        QListIterator<KWFrame> frameIt = frameset->frameIterator();
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            QRect frameRect( frame->x() - 1, frame->y() - 1,
                             frame->width() + 2, frame->height() + 2 );
            if ( !v_area.intersects( frameRect ) )
                continue;

            painter->setBrush( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) ?
                               frameset->getFrame( 0 )->getBackgroundColor() : frame->getBackgroundColor() );
            painter->setPen( lightGray );
            bool should_draw = TRUE;
            if ( frameset->getGroupManager() ) {
                if ( doc->getViewTableGrid() )
                    painter->setPen( QPen( black, 1, DotLine ) );
                else
                    painter->setPen( NoPen );
            }
            if ( FALSE )//static_cast<int>( i ) == hiliteFrameSet )
                painter->setPen( blue );
            else if ( !doc->getViewFrameBorders() )
                should_draw = FALSE;

            if ( v_area.intersects( frameRect ) && should_draw && !frameset->getGroupManager() ) {
                if ( region )
                    *region = region->subtract( frameRect );
                //if ( redrawOnlyCurrFrameset && m_currentFrameSet != frameset )
                //    ;
                /*else*/ {
                    if ( !drawBack )
                        painter->setBrush( Qt::NoBrush );
                    painter->drawRect( frameRect );
                }
            }

            painter->setBrush( Qt::NoBrush );
            if ( v_area.intersects( frameRect ) && frameset->getGroupManager() ) {
                if ( region )
                    *region = region->subtract( frameRect );
                //if ( redrawOnlyCurrFrameset && m_currentFrameSet != frameset )
                //    ;
                /*else*/ {
                    painter->fillRect( frameRect, frame->getBackgroundColor() );
                    painter->drawLine( frame->right() + 1, frame->y() - 1,
                                       frame->right() + 1, frame->bottom()  + 1 );
                    painter->drawLine( frame->x() - 1, frame->bottom() + 1,
                                       frame->right() + 1, frame->bottom() + 1 );
                    uint row = frameset->getGroupManager()->getCell( frameset )->row;
                    uint col = frameset->getGroupManager()->getCell( frameset )->col;
                    if ( row == 0 )
                        painter->drawLine( frame->x() - 1, frame->y() - contentsY() - 1,
                                           frame->right() + 1, frame->y() - contentsY() - 1 );
                    if ( col == 0 )
                        painter->drawLine( frame->x() - 1, frame->y() - 1,
                                           frame->x() - 1, frame->bottom() + 1 );
                }
            }

            //if ( redrawOnlyCurrFrameset && m_currentFrameSet != frameset )
            //    continue;

            // Draw frame borders

            if ( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) )
                frame = frameset->getFrame( 0 );

            if ( frame->getLeftBorder().ptWidth > 0 && frame->getLeftBorder().color !=
                 frame->getBackgroundColor().color() ) {
                QPen p( Border::borderPen( frame->getLeftBorder() ) );
                painter->setPen( p );
                painter->drawLine( frameRect.x() + frame->getLeftBorder().ptWidth / 2, frameRect.y(),
                                   frameRect.x() + frame->getLeftBorder().ptWidth / 2, frameRect.bottom() + 1 );
            }
            if ( frame->getRightBorder().ptWidth > 0 && frame->getRightBorder().color !=
                 frame->getBackgroundColor().color() ) {
                QPen p( Border::borderPen( frame->getRightBorder() ) );
                painter->setPen( p );
                int w = frame->getRightBorder().ptWidth;
                if ( ( w / 2 ) * 2 == w ) w--;
                w /= 2;
                painter->drawLine( frameRect.right() - w, frameRect.y(),
                                   frameRect.right() - w, frameRect.bottom() + 1 );
            }
            if ( frame->getTopBorder().ptWidth > 0 && frame->getTopBorder().color !=
                 frame->getBackgroundColor().color() ) {
                QPen p( Border::borderPen( frame->getTopBorder() ) );
                painter->setPen( p );
                painter->drawLine( frameRect.x(), frameRect.y() + frame->getTopBorder().ptWidth / 2,
                                   frameRect.right() + 1,
                                   frameRect.y() + frame->getTopBorder().ptWidth / 2 );
            }
            if ( frame->getBottomBorder().ptWidth > 0 && frame->getBottomBorder().color !=
                 frame->getBackgroundColor().color() ) {
                int w = frame->getBottomBorder().ptWidth;
                if ( ( w / 2 ) * 2 == w ) w--;
                w /= 2;
                QPen p( Border::borderPen( frame->getBottomBorder() ) );
                painter->setPen( p );
                painter->drawLine( frameRect.x(), frameRect.bottom() - w,
                                   frameRect.right() + 1,
                                   frameRect.bottom() - w );
            }
        }
    }

/*    if ( region ) {
        // clear empty space
        painter->save();
        painter->setClipRegion( *region );
        painter->fillRect( region->boundingRect(), Qt::white );
        painter->restore();
    }
*/
    // Draw page borders (red)
    painter->setPen( red );
    painter->setBrush( Qt::NoBrush );

    for ( int k = 0; k < doc->getPages(); k++ ) {
        QRect pageRect( 0, ( k * doc->ptPaperHeight() ),
                        doc->ptPaperWidth(), doc->ptPaperHeight() );
        if ( v_area.intersects( pageRect ) )
            painter->drawRect( pageRect );
    }

    painter->restore();
}

void KWCanvas::keyPressEvent( QKeyEvent *e )
{
    if( !doc->isReadWrite()) {
        switch( e->key() ) {
            case Key_Down:
                setContentsPos( contentsX(), contentsY() + 10 );
                break;
            case Key_Up:
                setContentsPos( contentsX(), contentsY() - 10 );
                break;
            case Key_Left:
                setContentsPos( contentsX() - 10, contentsY() );
                break;
            case Key_Right:
                setContentsPos( contentsX() + 10, contentsY() );
                break;
            case Key_PageUp:
                setContentsPos( contentsX(), contentsY() - visibleHeight() );
                break;
            case Key_PageDown:
                setContentsPos( contentsX(), contentsY() + visibleHeight() );
                break;
            case Key_Home:
                setContentsPos( contentsX(), 0 );
                break;
            case Key_End:
                setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
                break;
            default:
                break;
        }
        return;
    }
    if ( m_currentFrameSetEdit && m_mouseMode==MM_EDIT )
        m_currentFrameSetEdit->keyPressEvent( e );
}

void KWCanvas::mpEditFrame( QMouseEvent *e, int mx, int my ) // mouse press in edit-frame mode
{
    mousePressed = true;
    frameResized = false;
    if ( e ) {
        // only simulate selection - we do real selection below
        int currentSelection = doc->selectFrame( mx, my, TRUE );

        KWFrameSet *fs = doc->getFrameSet( mx, my );
        if ( currentSelection != 0 && ( e->state() & ShiftButton ) && fs->getGroupManager() ) { // is table and we hold shift
            fs->getGroupManager()->selectUntil( fs );
            curTable = fs->getGroupManager();
        } else if ( currentSelection == 0 ) { // none selected
            selectAllFrames( FALSE );
        } else if ( currentSelection == 1 ) { // 1 selected
            if ( !( e->state() & ControlButton || e->state() & ShiftButton ) )
                selectAllFrames( FALSE );
            selectFrame( mx, my, TRUE );
            curTable = fs->getGroupManager();
        } else if ( currentSelection == 2 ) { // was already selected
            if ( e->state() & ControlButton || e->state() & ShiftButton ) {
                selectFrame( mx, my, FALSE );
                curTable = fs->getGroupManager();
            } else if ( viewport()->cursor().shape() != SizeAllCursor ) {
                selectAllFrames( FALSE );
                selectFrame( mx, my, TRUE );
                curTable = fs->getGroupManager();
            }
        }
    }

    viewport()->setCursor( doc->getMouseCursor( mx, my ) );

    deleteMovingRect = FALSE;
    mx = ( mx / doc->gridX() ) * doc->gridX();
    oldMx = mx;
    my = ( my / doc->gridX() ) * doc->gridY();
    oldMy = my;
}

void KWCanvas::mpCreate( int mx, int my )
{
    mx = ( mx / doc->gridX() ) * doc->gridX();
    oldMx = mx;
    my = ( my / doc->gridX() ) * doc->gridY();
    oldMy = my;
    m_insRect = QRect( mx, my, 0, 0 );
    deleteMovingRect = FALSE;
}

void KWCanvas::mpCreatePixmap( int mx, int my )
{
    if ( !m_PixmapName.isEmpty() ) {
        QPixmap _pix( m_PixmapName );
        mx = ( mx / doc->gridX() ) * doc->gridX();
        oldMx = mx;
        my = ( my / doc->gridX() ) * doc->gridY();
        oldMy = my;
        m_insRect = QRect( mx, my, 0, 0 );
        deleteMovingRect = FALSE;
        //doRaster = FALSE;
        QCursor::setPos( viewport()->mapToGlobal( QPoint( mx + _pix.width(), my + _pix.height() ) ) );
    }
}

void KWCanvas::contentsMousePressEvent( QMouseEvent *e )
{
    if(!doc->isReadWrite())
        return;
    int mx = e->pos().x();
    int my = e->pos().y();
    if ( e->button() == RightButton ) {
        // rmb menu
        switch ( m_mouseMode )
            {
            case MM_EDIT:
                m_gui->getView()->openPopupMenuEditText( QCursor::pos() );
                break;
            case MM_EDIT_FRAME:
            case MM_CREATE_TEXT:
            case MM_CREATE_PART:
            case MM_CREATE_TABLE:
            case MM_CREATE_FORMULA:// case MM_CREATE_KSPREAD_TABLE:
            case MM_CREATE_PIX:
                // Select the frame first
                mpEditFrame( e, mx, my );
                if (!doc->getFrameSet( e->pos().x(), e->pos().y() ))
                    m_gui->getView()->openPopupMenuChangeAction( QCursor::pos() );
                else
                    {
                        //todo create a popupMenu for frame (delete - properties etc...)
                        KWFrame *frame=doc->getFirstSelectedFrame();
                        // if a header/footer etc. Dont show the popup.
                        if((frame->getFrameSet() && frame->getFrameSet()->getFrameInfo() != FI_BODY))
                            {
                                mousePressed = false;
                                return;
                            }
                        m_gui->getView()->openPopupMenuEditFrame( QCursor::pos() );
                    }
                break;
            default: break;
            }
        mousePressed = false;
        return;
    }

    if ( e->button() == LeftButton ) {
	mousePressed = TRUE;

        switch ( m_mouseMode ) {
        case MM_EDIT:
            {
                KWFrameSet *fs = doc->getFrameSet( mx, my );
                if ( m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() != fs )
                    {
                        // Terminate edition of that frameset
                        delete m_currentFrameSetEdit;
                        m_currentFrameSetEdit = 0L;
                    }
                // Edit the frameset under the mouse, if any
                if ( !m_currentFrameSetEdit && fs )
                    m_currentFrameSetEdit = fs->createFrameSetEdit( this );

                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mousePressEvent( e );
            }
            break;
        case MM_EDIT_FRAME:
            mpEditFrame( e, mx, my );
            break;
        case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE:
        case MM_CREATE_FORMULA:// case MM_CREATE_KSPREAD_TABLE:
            mpCreate( mx, my );
            break;
        case MM_CREATE_PIX:
            mpCreatePixmap( mx, my );
            break;
        default: break;
        }
        // The "edit" modes don't modify the doc (on mouse-press). The "create" ones do.
        if ( m_mouseMode != MM_EDIT && m_mouseMode != MM_EDIT_FRAME )
            doc->setModified( TRUE );

    } else if ( e->button() == MidButton ) {
        if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT)
            m_currentFrameSetEdit->paste();
    }
}

/*================================================================*/
void KWCanvas::mmEditFrameResize( bool top, bool bottom, bool left, bool right )
{
    // This one is called by KWResizeHandle

    // Can't resize the main frame of a WP document
    KWFrame *frame = doc->getFirstSelectedFrame();
    if ( doc->processingType() == KWDocument::WP  && frame->getFrameSet() == doc->getFrameSet(0))
        return;

    // Get the mouse position from QCursor. Trying to get it from KWResizeHandle's
    // mouseMoveEvent leads to the frame 'jumping' because the events are received async.
    QPoint mousep = mapFromGlobal(QCursor::pos());
    int mx = mousep.x() + contentsX();
    int my = mousep.y() + contentsY();

    // Apply the grid
    KWDocument * doc = frame->getFrameSet()->kWordDocument();
    int rastX = doc->gridX();
    int rastY = doc->gridY();
    mx = ( mx / rastX ) * rastX;
    my = ( my / rastY ) * rastY;
    //kdDebug() << "KWCanvas::mmEditFrameResize after raster mx=" << mx << " my=" << my << endl;

    // Calculate new frame coordinates, using minimum sizes, and keeping it in the bounds of the page
    int newLeft = frame->left();
    int newTop = frame->top();
    int newRight = frame->right();
    int newBottom = frame->bottom();

    FrameInfo frInfo=frame->getFrameSet()->getFrameInfo();

    if (top && newTop != my) {
        bool move=true;
        if (isAFooter(frInfo))
            move=false;
        if (newBottom-my < (int)(minFrameHeight+5))
            my=newBottom-minFrameHeight-5;
        if (my < (int)( frame->getPageNum() * doc->ptPaperHeight()))
            my = frame->getPageNum() * doc->ptPaperHeight();

        if (move) newTop=my;
    } else if (bottom && newBottom != my) {
        bool move=true;
        if (isAHeader(frInfo))
            move=false;
        if (my-newTop < (int)(minFrameHeight+5))
            my=newTop+minFrameHeight+5;
        if (my >= (int)((frame->getPageNum()+1) * doc->ptPaperHeight()))
        {
            my = (frame->getPageNum()+1) * doc->ptPaperHeight();
        }

        if (move) newBottom=my;
    }

    if (left && newLeft != mx) {
        bool move=true;
        if (isAHeader(frInfo))
            move=false;
        if (isAFooter(frInfo))
            move=false;
        if (newRight-mx < (int)minFrameWidth)
            mx=newRight-minFrameWidth-5;
        if (mx <= 0)
            mx=0;
        if (move)
            newLeft=mx;
    } else if (right && newRight != mx) {
        bool move=true;
        if (isAHeader(frInfo)) move=false;
        if (isAFooter(frInfo)) move=false;
        if (mx-newLeft < (int)minFrameWidth)
            mx=newLeft+minFrameWidth+5;
        if (mx >  (int)doc->ptPaperWidth())
            mx = doc->ptPaperWidth();

        if (move) newRight=mx;
    }

    // Keep copy of old rectangle, for repaint()
    QRect oldRect( frame->x() - 1, frame->y() - 1,
                   frame->width() + 2, frame->height() + 2 );

    frame->setLeft(newLeft);
    frame->setTop(newTop);
    frame->setRight(newRight);
    frame->setBottom(newBottom);

#if 0
    int drawX, drawWidth, drawY, drawHeight;
    drawX=frame->left();
    drawWidth=frame->width();
    drawY=frame->top();
    drawHeight=frame->height();
    if (frame->getFrameSet()->getGroupManager()) { // is table
        if (!(top || bottom)) { /// full height.
            drawY=frame->getFrameSet()->getGroupManager()->getBoundingRect().y();
            drawHeight=frame->getFrameSet()->getGroupManager()->getBoundingRect().height();
        } else if (!(left || right)) { // full width.
            drawX=frame->getFrameSet()->getGroupManager()->getBoundingRect().x();
            drawWidth=frame->getFrameSet()->getGroupManager()->getBoundingRect().width();
        }
    }
    //p.drawRect( drawX, drawY, drawWidth, drawHeight );
    //p.end();
#endif

    // Move resize handles to new position
    frame->updateResizeHandles();
    // Calculate new rectangle for this frame
    QRect newRect( frame->x() - 1, frame->y() - 1,
                   frame->width() + 2, frame->height() + 2 );
    // Repaing only the changed rects (oldRect U newRect)
    repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );

    oldMy = my;
    oldMx = mx;
    //doRaster = TRUE;
    frameResized = TRUE;
}

void KWCanvas::mmEditFrameMove( int mx, int my )
{
    if ( mx != oldMx || my != oldMy ) {
        QList<KWGroupManager> undos, updates;
        undos.setAutoDelete( FALSE );
        updates.setAutoDelete( FALSE );
        bool bFirst = true;

        QListIterator<KWFrameSet> framesetIt( doc->framesetsIterator() );
        for (; framesetIt.current(); ++framesetIt, bFirst=false )
        {
            KWFrameSet *frameset = framesetIt.current();
            if ( doc->processingType() == KWDocument::WP && bFirst ||
                 frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() != FI_BODY ) continue;

            QListIterator<KWFrame> frameIt( frameset->frameIterator() );
            for ( ; frameIt.current(); ++frameIt )
            {
                KWFrame *frame = frameIt.current();
                if ( frame->isSelected() ) {
                    if ( frameset->getGroupManager() ) {
                        if ( updates.findRef( frameset->getGroupManager() ) == -1 )
                            updates.append( frameset->getGroupManager() );
                    } else {
                        QRect oldRect( frame->x() - 1, frame->y() - 1,
                                       frame->width() + 2, frame->height() + 2 );
                        int page = doc->getPageOfRect( *frame );
                        // Move the frame
                        frame->moveBy( mx - oldMx, my - oldMy );
                        // But not out of the page it was on initially
                        if ( isOutOfPage( *frame, page ) )
                            frame->moveBy( oldMx - mx, oldMy - my );
                        // Calculate new rectangle for this frame
                        QRect newRect( frame->x() - 1, frame->y() - 1,
                                       frame->width() + 2, frame->height() + 2 );
                        // Repaing only the changed rects (oldRect U newRect)
                        repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
                    }
                    // Move resize handles to new position
                    frame->updateResizeHandles();
                }
            }
        }

        if ( !updates.isEmpty() ) {
            kdDebug() << "KWCanvas::mmEditFrameMove UPDATES" << endl;
            for ( unsigned int i = 0; i < updates.count(); i++ ) {
                KWGroupManager *grpMgr = updates.at( i );
                for ( unsigned k = 0; k < grpMgr->getNumCells(); k++ ) {
                    KWFrame * frame = grpMgr->getCell( k )->frameSet->getFrame( 0 );
                    QRect oldRect( frame->x() - 1, frame->y() - 1,
                                   frame->width() + 2, frame->height() + 2 );
                    frame->moveBy( mx - oldMx, my - oldMy );
                    if ( frame->x() < 0 || frame->right() > static_cast<int>( doc->ptPaperWidth() ) || frame->y() < 0 ) {
                        if ( undos.findRef( grpMgr ) == -1 )
                            undos.append( grpMgr );
                    }
                    // Calculate new rectangle for this frame
                    QRect newRect( frame->x() - 1, frame->y() - 1,
                                   frame->width() + 2, frame->height() + 2 );
                    // Repaing only the changed rects (oldRect U newRect)
                    repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
                    // Move resize handles to new position
                    frame->updateResizeHandles();
                }
            }
        }

        frameResized = true; // mrEditFrame() expects us to make this one true

#if 0 // TODO
        if ( !undos.isEmpty() ) {
            for ( unsigned int i = 0; i < undos.count(); i++ ) {
                undos.at( i )->drawAllRects( p, contentsX(), contentsY() );
                undos.at( i )->moveBy( oldMx - mx, oldMy - my );
                undos.at( i )->drawAllRects( p, contentsX(), contentsY() );
            }
        }
#endif

    }
}

void KWCanvas::mmCreate( int mx, int my ) // Mouse move when creating a frame
{
    //mx -= contentsX();
    //my -= contentsY();

    /*if ( doRaster )*/ {
        mx = ( mx / doc->gridX() ) * doc->gridX();
        my = ( my / doc->gridY() ) * doc->gridY();
    }

    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );

    if ( deleteMovingRect ) {
#if 0
        if ( useAnchor ) {
            p.drawLine( anchor->getOrigin(), m_insRect.topLeft() );
        }
#endif
        p.drawRect( m_insRect );
    }
    else {
#if 0
        if ( useAnchor ) {
            KWGroupManager *grpMgr;
            grpMgr = new KWGroupManager( doc );
            insertAnchor( grpMgr );
            anchor = grpMgr;
        }
#endif
    }
    // Resize the rectangle
    m_insRect.setWidth( m_insRect.width() + mx - oldMx );
    m_insRect.setHeight( m_insRect.height() + my - oldMy );

    // But not out of the page
    QRect r = m_insRect.normalize();
    if ( isOutOfPage( r, doc->getPageOfRect( r ) ) )
    {
        m_insRect.setWidth( m_insRect.width() - ( mx - oldMx ) );
        m_insRect.setHeight( m_insRect.height() - ( my - oldMy ) );
    }
#if 0
    if ( useAnchor ) {
        p.drawLine( anchor->getOrigin(), m_insRect.topLeft() );
    }
#endif
    p.drawRect( m_insRect );
    p.end();
    oldMx = mx;
    oldMy = my;
    deleteMovingRect = TRUE;
}

void KWCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
    int mx = e->pos().x();
    int my = e->pos().y();
    if ( mousePressed ) {
        // We need doAutoScroll() (not in KWTextFrameSet), because when moving frames etc. it could be handy
	doAutoScroll();

        switch ( m_mouseMode ) {
            case MM_EDIT:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseMoveEvent( e );
                break;
            case MM_EDIT_FRAME: {
                mx = ( mx / doc->gridX() ) * doc->gridX();
                my = ( my / doc->gridY() ) * doc->gridY();

                if ( viewport()->cursor().shape() == SizeAllCursor )
                    mmEditFrameMove( mx, my );
                deleteMovingRect = TRUE;
                oldMx = mx; oldMy = my;
            } break;
            case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
            case MM_CREATE_TABLE: case MM_CREATE_FORMULA:// case MM_CREATE_KSPREAD_TABLE:
                mmCreate( mx, my );
            default: break;
        }
    } else {
        switch ( m_mouseMode ) {
            case MM_EDIT_FRAME:
                viewport()->setCursor( doc->getMouseCursor( mx, my ) );
                break;
            default: break;
        }
    }
}

/*================================================================*/

void KWCanvas::mrEditFrame()
{
    KWFrame *frame= doc->getFirstSelectedFrame();
    if (!frame) return;
#if 0
    if ( doc->processingType() == KWDocument::DTP )
        setRuler2Frame( frame );
#endif
    m_gui->getHorzRuler()->setFrameStart( frame->x() );

    doc->layout();

    if ( frameResized ) {
        doc->setModified( true );
        doc->recalcFrames();
        doc->updateAllFrames();
        KWGroupManager *grpMgr = frame->getFrameSet()->getGroupManager();
        if (grpMgr) {
            grpMgr->recalcCols();
            grpMgr->recalcRows();
            grpMgr->updateTempHeaders();
            //repaintTableHeaders( grpMgr );
        }
        //recalcAll = TRUE;
        //recalcText();
        //recalcCursor();
        repaintAll( true );
        //recalcAll = FALSE;
    } else {
        // Frame not resized - or moved
        doc->updateAllViews( m_gui->getView() );
    }
}

void KWCanvas::mrCreateText()
{
    repaintChanged();
    KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );

    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        KWFrameDia * frameDia = new KWFrameDia( this, frame, doc, FT_TEXT);
        connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
        frameDia->setCaption(i18n("Connect frame"));
        frameDia->show();
        delete frameDia;
    }
}

void KWCanvas::mrCreatePixmap()
{
    repaintChanged();

    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() && !m_PixmapName.isEmpty() ) {
        KWPictureFrameSet *frameset = new KWPictureFrameSet( doc );
        frameset->setFileName( m_PixmapName, m_insRect.size() );
        m_insRect = m_insRect.normalize();
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(),
                                     m_insRect.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        repaintChanged();
    }
    setMouseMode( MM_EDIT );
}

void KWCanvas::mrCreatePart() // mouse release, when creating part or kspread table
{
    repaintChanged();

    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        doc->insertObject( m_insRect, m_partEntry, contentsX(), contentsY() );
        repaintAll();
    }
    setMouseMode( MM_EDIT );
}

void KWCanvas::mrCreateFormula()
{
    repaintChanged();

    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( doc, this );
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        repaintChanged();
    }
    setMouseMode( MM_EDIT );
}

void KWCanvas::mrCreateTable()
{
    repaintChanged();

    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        if ( tcols * minFrameWidth + m_insRect.x() > doc->ptPaperWidth() )
            {
                KMessageBox::sorry(0, i18n("KWord is unable to insert the table because there\n"
                                           "is not enough space available."));
            }
        else {
            KWGroupManager *grpMgr;

            /*if ( useAnchor ) {
                grpMgr = static_cast<KWGroupManager *>(anchor);
                } else*/
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
                if (!same) found=false;
                numGroupManagers++;
            }
            grpMgr->setName( _name );
            doc->addGroupManager( grpMgr );
            for ( unsigned int i = 0; i < trows; i++ ) {
                for ( unsigned int j = 0; j < tcols; j++ ) {
                    KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
                    KWFrame *frame = new KWFrame(_frameSet, m_insRect.x(), m_insRect.y(), (doc->ptPaperWidth() - m_insRect.x())/tcols, m_insRect.height() );
                    _frameSet->addFrame( frame );
                    frame->setFrameBehaviour(AutoExtendFrame);
                    frame->setNewFrameBehaviour(NoFollowup);
                    _frameSet->setGroupManager( grpMgr );
                    grpMgr->addFrameSet( _frameSet, i, j );
                }
            }
            grpMgr->init( m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height(),
                          twid, thei );
            grpMgr->recalcRows();
        }
        //recalcWholeText( TRUE );
        doc->updateAllFrames();
    }
    setMouseMode( MM_EDIT );
    useAnchor = false;

}

void KWCanvas::contentsMouseReleaseEvent( QMouseEvent * e )
{
    if ( scrollTimer->isActive() )
	scrollTimer->stop();
    if ( mousePressed ) {
        switch ( m_mouseMode ) {
            case MM_EDIT:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseReleaseEvent( e );
                break;
            case MM_EDIT_FRAME:
                mrEditFrame();
                break;
            case MM_CREATE_TEXT:
                mrCreateText();
                break;
            case MM_CREATE_PIX:
                mrCreatePixmap();
                break;
            case MM_CREATE_PART: //case MM_CREATE_KSPREAD_TABLE:
                mrCreatePart();
                break;
            case MM_CREATE_TABLE:
                mrCreateTable();
                break;
            case MM_CREATE_FORMULA:
                mrCreateFormula();
                break;
            default:
                repaintChanged();
        }

	mousePressed = FALSE;
    }
}

void KWCanvas::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
    switch ( m_mouseMode ) {
        case MM_EDIT:
            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->mouseDoubleClickEvent( e );
            break;
        case MM_EDIT_FRAME:
            // TODO activate part/formula/... ?
            // As easy as 1) create framesetedit for this frameset, 2) mouseDoubleClickEvent
            // But then we are in the wrong mousemode, no ?
            break;
        default:
            break;
    }

    repaintChanged();
    mousePressed = TRUE;
}

/*================================================================*/
void KWCanvas::setLeftFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    QList<FrameBorderTypeStruct> tmpBorderList;
    QList<FrameIndex> frameindexList;

    if (!_b)
        _frmBrd.ptWidth=0;


    KWFrame *frame=0L;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getLeftBorder();
        tmp->m_EFrameType= FBLeft;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (_b)
        {
            if (_frmBrd!=frame->getLeftBorder())
                frame->setLeftBorder(_frmBrd);
        }
        else
            frame->setLeftBorder(_frmBrd);
    }

    KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Left Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
    doc->addCommand(cmd);

    repaintChanged();
}

/*================================================================*/
void KWCanvas::setRightFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    QList<FrameBorderTypeStruct> tmpBorderList;
    QList<FrameIndex> frameindexList;
    KWFrame *frame=0L;
    if (!_b)
        _frmBrd.ptWidth=0;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getRightBorder();
        tmp->m_EFrameType= FBRight;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());

        tmpBorderList.append(tmp);
        frameindexList.append(index);

        if (_b)
        {
            if (_frmBrd!=frame->getRightBorder())
                frame->setRightBorder(_frmBrd);
        }
        else
            frame->setRightBorder(_frmBrd);
    }

    KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Right Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
    doc->addCommand(cmd);

    repaintChanged();
}

/*================================================================*/
void KWCanvas::setTopFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;

    QList<FrameBorderTypeStruct> tmpBorderList;
    QList<FrameIndex> frameindexList;

    KWFrame *frame=0L;
    if (!_b)
        _frmBrd.ptWidth=0;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getTopBorder();
        tmp->m_EFrameType= FBTop;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (_b)
        {
            if (_frmBrd!=frame->getTopBorder())
                frame->setTopBorder(_frmBrd);
        }
        else
            frame->setTopBorder(_frmBrd);
    }
    KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Top Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
    doc->addCommand(cmd);

    repaintChanged();
}

/*================================================================*/
void KWCanvas::setBottomFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;

    QList<FrameBorderTypeStruct> tmpBorderList;
    QList<FrameIndex> frameindexList;
    KWFrame *frame=0L;
    if (!_b)
        _frmBrd.ptWidth=0;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getBottomBorder();
        tmp->m_EFrameType= FBBottom;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (_b)
        {
            if (_frmBrd!=frame->getBottomBorder())
                frame->setBottomBorder(_frmBrd);
        }
        else
            frame->setBottomBorder(_frmBrd);
    }

    KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Bottom Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
    doc->addCommand(cmd);

    repaintChanged();
}

/*================================================================*/
void KWCanvas::setOutlineFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    KWFrame *frame=0L;
    if (!_b)
        _frmBrd.ptWidth=0;

    QList<FrameBorderTypeStruct> tmpBorderListRight;
    QList<FrameBorderTypeStruct> tmpBorderListTop;
    QList<FrameBorderTypeStruct> tmpBorderListLeft;
    QList<FrameBorderTypeStruct> tmpBorderListBottom;
    QList<FrameIndex> frameindexListRight;
    QList<FrameIndex> frameindexListLeft;
    QList<FrameIndex> frameindexListTop;
    QList<FrameIndex> frameindexListBottom;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getBottomBorder();
        tmp->m_EFrameType= FBBottom;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());
        tmpBorderListBottom.append(tmp);
        frameindexListBottom.append(index);

        index=new FrameIndex;
        tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getTopBorder();
        tmp->m_EFrameType= FBTop;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());
        tmpBorderListTop.append(tmp);
        frameindexListTop.append(index);

        index=new FrameIndex;
        tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getRightBorder();
        tmp->m_EFrameType= FBRight;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());
        tmpBorderListRight.append(tmp);
        frameindexListRight.append(index);

        index=new FrameIndex;
        tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getLeftBorder();
        tmp->m_EFrameType= FBLeft;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());
        tmpBorderListLeft.append(tmp);
        frameindexListLeft.append(index);

        if (_b)
        {
            if (_frmBrd!=frame->getBottomBorder())
                frame->setBottomBorder(_frmBrd);
            if (_frmBrd!=frame->getTopBorder())
                frame->setTopBorder(_frmBrd);
            if (_frmBrd!=frame->getLeftBorder())
                frame->setLeftBorder(_frmBrd);
            if (_frmBrd!=frame->getRightBorder())
                frame->setRightBorder(_frmBrd);
        }
        else
        {
            frame->setTopBorder(_frmBrd);
            frame->setBottomBorder(_frmBrd);
            frame->setLeftBorder(_frmBrd);
            frame->setRightBorder(_frmBrd);
        }
    }
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Change Outline Border") );
    KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Bottom Border frame"),doc,frameindexListBottom,tmpBorderListBottom,_frmBrd);
    macroCmd->addCommand(cmd);

    cmd=new KWFrameBorderCommand(i18n("Change Left Border frame"),doc,frameindexListLeft,tmpBorderListLeft,_frmBrd);
    macroCmd->addCommand(cmd);

    cmd=new KWFrameBorderCommand(i18n("Change Top Border frame"),doc,frameindexListTop,tmpBorderListTop,_frmBrd);
    macroCmd->addCommand(cmd);

    cmd=new KWFrameBorderCommand(i18n("Change Right Border frame"),doc,frameindexListRight,tmpBorderListRight,_frmBrd);
    macroCmd->addCommand(cmd);

    doc->addCommand(macroCmd);
    repaintChanged();
}

/*================================================================*/
void KWCanvas::setFrameBackgroundColor( const QBrush &_backColor )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    KWFrame *frame=0L;

    QList<FrameIndex> frameindexList;
    QList<QBrush> oldColor;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());
        QBrush *_color=new QBrush(frame->getBackgroundColor());
        frameindexList.append(index);
        oldColor.append(_color);
        if (_backColor!=frame->getBackgroundColor())
            frame->setBackgroundColor(_backColor);
    }
    KWFrameBackGroundColorCommand *cmd=new KWFrameBackGroundColorCommand(i18n("Change Frame BackGroundColor"),doc,frameindexList,oldColor,_backColor);
    doc->addCommand(cmd);
    repaintChanged();
}


/*================================================================*/
void KWCanvas::editFrameProperties()
{
    KWFrame *frame=doc->getFirstSelectedFrame();
    if ( isAHeader(frame->getFrameSet()->getFrameInfo()) )
    {
        KMessageBox::sorry( this, i18n( "This is a Header frame, it can not be edited."), i18n( "Frame Properties"  ) );
        return;
    }
    if ( isAFooter(frame->getFrameSet()->getFrameInfo()) )
    {
        KMessageBox::sorry( this, i18n( "This is a Footer frame, it can not be edited."),i18n( "Frame Properties"  ) );
        return;
    }

    if (frame)
    {
        KWFrameDia * frameDia = new KWFrameDia( this, frame);
        connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
        frameDia->setCaption(i18n("Frame Properties"));
        frameDia->show();
        delete frameDia;
    }
}

/*================================================================*/
void KWCanvas::frameDiaClosed()
{
    //todo refresh text cursor etc...
#if 0
    hiliteFrameSet = -1;
    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    recalcAll = FALSE;
    repaintScreen (true);
#endif
    doc->updateAllFrames();
}

void KWCanvas::updateFrameFormat()
{
    KWFrame *frame= doc->getFirstSelectedFrame();
    if (frame)
        m_gui->getView()->showFrameBorders( frame->getLeftBorder(), frame->getRightBorder(), frame->getBottomBorder(), frame->getTopBorder() );
}

void KWCanvas::selectAllFrames( bool select )
{
    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        QListIterator<KWFrame> frameIt = fs->frameIterator();
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            if ( frame->isSelected() != select )
                frame->setSelected( select );
        }
    }
}

void KWCanvas::selectFrame( int mx, int my, bool select )
{
    KWFrameSet *frameset = doc->getFrameSet( mx, my );
    if ( frameset )
    {
        KWFrame *frame = frameset->getFrame( mx, my );
        if ( frame->isSelected() != select )
            frame->setSelected( select );
	updateFrameFormat();
        m_gui->getView()->updatePopupMenuChangeAction();
    }
}

KWGroupManager *KWCanvas::getTable()
{
    KWGroupManager *tmpGroupManager=0L;
    if( !m_currentFrameSetEdit)
        return tmpGroupManager;
    if( m_currentFrameSetEdit->frameSet()->getGroupManager()!=0)
        return m_currentFrameSetEdit->frameSet()->getGroupManager();
    else
        return tmpGroupManager;
}


/*================================================================*/
void KWCanvas::deleteFrame()
{
    if ( m_mouseMode != MM_EDIT_FRAME ) {
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
        if ( doc->processingType() == KWDocument::WP && doc->getFrameSetNum( theFrame->getFrameSet() ) == 0 )
            return;
#if 0L
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
        #endif
    }
#if 0
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();
#endif
    // do the actual delete.
    if ( theFrame->getFrameSet()->getNumFrames() > 1 )
        theFrame->getFrameSet()->delFrame( theFrame );
    else
        doc->delFrameSet( theFrame->getFrameSet() );

#if 0
    // set FC to new frameset
    fc->setFrameSet( 1 );
    fc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getFirstParag() );
#endif
    doc->recalcFrames();
    doc->updateAllFrames();
#if 0
    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    repaintScreen( TRUE );
    recalcAll = FALSE;
#endif
    repaintAll();
}

/*================================================================*/
void KWCanvas::deleteTable( KWGroupManager *groupManager )
{
    if ( !groupManager )
        return;
#if 0
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();
#endif
    doc->delGroupManager( groupManager );
    doc->recalcFrames();
    doc->updateAllFrames();

#if 0

    fc->setFrameSet( 0 );
    fc->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getFirstParag() );

    recalcAll = TRUE;
    recalcText();
    recalcCursor();
    repaintScreen( TRUE );
    recalcAll = FALSE;

    if ( blinking )
        startBlinkCursor();
#endif

    repaintAll();
}


void KWCanvas::setMouseMode( MouseMode _mm )
{
    if ( m_mouseMode != _mm )
    {
        selectAllFrames( FALSE );
        if ( _mm != MM_EDIT )
        {
            // Terminate edition of current frameset
            delete m_currentFrameSetEdit;
            m_currentFrameSetEdit = 0L;
        }
    }

    m_mouseMode = _mm;
    //mmUncheckAll();
    m_gui->getView()->setTool( m_mouseMode );

    switch ( m_mouseMode ) {
        case MM_EDIT: {
            viewport()->setCursor( ibeamCursor );
            //mm_menu->setItemChecked( mm_edit, TRUE );
#if 0
            // Update UI
            if ( !inKeyEvent ) {
                setRulerFirstIndent( m_gui->getHorzRuler(), fc->getParag()->getParagLayout()->getFirstLineLeftIndent() );
                setRulerLeftIndent( m_gui->getHorzRuler(), fc->getParag()->getParagLayout()->getLeftIndent() );
                m_gui->getHorzRuler()->setFrameStart( doc->getFrameSet( fc->getFrameSet() - 1 )->
                                                    getFrame( fc->getFrame() - 1 )->x() );
                m_gui->getHorzRuler()->setTabList( fc->getParag()->getParagLayout()->getTabList() );
                if ( doc->processingType() == KWDocument::DTP )
                    setRuler2Frame( fc->getFrameSet() - 1, fc->getFrame() - 1 );
            }
#endif
        } break;
        case MM_EDIT_FRAME: {
            viewport()->setCursor( arrowCursor );
            //mm_menu->setItemChecked( mm_edit_frame, TRUE );
        } break;
        case MM_CREATE_TEXT: {
            viewport()->setCursor( crossCursor );
            //mm_menu->setItemChecked( mm_create_text, TRUE );
        } break;
        case MM_CREATE_PIX: {
            viewport()->setCursor( crossCursor );
            //mm_menu->setItemChecked( mm_create_pix, TRUE );
        } break;
        case MM_CREATE_TABLE: {
            viewport()->setCursor( crossCursor );
            //mm_menu->setItemChecked( mm_create_table, TRUE );
        } break;
        /*case MM_CREATE_KSPREAD_TABLE: {
            viewport()->setCursor( crossCursor );
            //mm_menu->setItemChecked( mm_create_kspread_table, TRUE );
        } break;*/
        case MM_CREATE_FORMULA: {
            viewport()->setCursor( crossCursor );
            //mm_menu->setItemChecked( mm_create_formula, TRUE );
        } break;
        case MM_CREATE_PART: {
            viewport()->setCursor( crossCursor );
            //mm_menu->setItemChecked( mm_create_part, TRUE );
        } break;
    }

    repaintChanged();
}

void KWCanvas::contentsDragEnterEvent( QDragEnterEvent *e )
{
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->dragEnterEvent( e );
}

void KWCanvas::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->dragMoveEvent( e );
}

void KWCanvas::contentsDragLeaveEvent( QDragLeaveEvent *e )
{
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->dragLeaveEvent( e );
}

void KWCanvas::contentsDropEvent( QDropEvent *e )
{
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->dropEvent( e );
    mousePressed = false;
}

void KWCanvas::doAutoScroll()
{
    if ( !mousePressed )
	return;

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->doAutoScroll( viewportToContents( pos ) );

    if ( !scrollTimer->isActive() && pos.y() < 0 || pos.y() > height() )
	scrollTimer->start( 100, FALSE );
    else if ( scrollTimer->isActive() && pos.y() >= 0 && pos.y() <= height() )
	scrollTimer->stop();
}

void KWCanvas::slotContentsMoving( int, int cy )
{
    //kdDebug() << "KWCanvas::slotContentsMoving " << cy << endl;
    // Update our "formatted paragraphs needs" in the text framesets
    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>(fit.current());
        if ( fs )
        {
            fs->updateViewArea( this, cy + visibleHeight() );
        }
    }
}

bool KWCanvas::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return TRUE;

    if ( o == this || o == viewport() ) {
	if ( e->type() == QEvent::FocusIn ) {
            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->focusInEvent();
	    return TRUE;
	} else if ( e->type() == QEvent::FocusOut ) {
            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->focusOutEvent();
	    return TRUE;
	}
    }

    return QScrollView::eventFilter( o, e );
}

bool KWCanvas::focusNextPrevChild( bool )
{
    return TRUE; // Don't allow to go out of the canvas widget by pressing "Tab"
}

void KWCanvas::updateCurrentFormat()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit);
    if ( edit )
        edit->updateUI();
}

void KWCanvas::printRTDebug()
{
    KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet *>(m_currentFrameSet);
    if ( !textfs )
        return;
    static const char * dm[] = { "DisplayBlock", "DisplayInline", "DisplayListItem", "DisplayNone" };
    QTextDocument * textdoc = textfs->textDocument();
    for (QTextParag * parag = textdoc->firstParag(); parag ; parag = parag->next())
    {
        kdDebug() << "Paragraph " << parag << "------------------ " << endl;
        QVector<QStyleSheetItem> vec = parag->styleSheetItems();
        for ( uint i = 0 ; i < vec.size() ; ++i )
        {
            QStyleSheetItem * item = vec[i];
            kdDebug() << "  StyleSheet Item " << item << " '" << item->name() << "'" << endl;
            kdDebug() << "        italic=" << item->fontItalic() << " underline=" << item->fontUnderline() << " fontSize=" << item->fontSize() << endl;
            kdDebug() << "        align=" << item->alignment() << " leftMargin=" << item->margin(QStyleSheetItem::MarginLeft) << " rightMargin=" << item->margin(QStyleSheetItem::MarginRight) << " topMargin=" << item->margin(QStyleSheetItem::MarginTop) << " bottomMargin=" << item->margin(QStyleSheetItem::MarginBottom) << endl;
            kdDebug() << "        displaymode=" << dm[item->displayMode()] << endl;
        }
        kdDebug() << "  Text: " << parag->richText() << endl;
        KWTextParag * p = static_cast<KWTextParag *>(parag);
        if ( p->counter() )
            kdDebug() << "  Counter style=" << p->counter()->style() << " depth=" << p->counter()->depth() << endl;

/*        kdDebug() << "  Paragraph format=" << p->paragFormat() << endl;
        QTextString * s = parag->string();
        for ( int i = 0 ; i < s->length() ; ++i )
            kdDebug() << i << ": '" << QString(s->at(i).c) << "'" << s->at(i).format() << endl;
*/
    }
}

#include "kwcanvas.moc"
