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
#include <qprogressdialog.h>
#include <kaction.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <config.h>

KWCanvas::KWCanvas(QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent, "canvas", WNorthWestGravity | WResizeNoErase ), doc( d )
{
    m_gui = lGui;
    m_currentFrameSetEdit = doc->getFrameSet( 0 )->createFrameSetEdit( this );
    mousePressed = false;
    setMouseMode( MM_EDIT );

    cmdMoveFrame=0L;

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
    resizeContents( doc->paperWidth(), doc->paperHeight() * doc->getPages() );

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

void KWCanvas::repaintChanged( KWFrameSet * fs, bool resetChanged )
{
    //kdDebug(32002) << "KWCanvas::repaintChanged this=" << this << " fs=" << fs << endl;
    QPainter p( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setBrushOrigin( -contentsX(), -contentsY() );
    drawDocument( fs, &p, contentsX(), contentsY(), visibleWidth(), visibleHeight(), fs && resetChanged );
}

void KWCanvas::repaintAll( bool erase /* = false */ )
{
    //kdDebug() << "KWCanvas::repaintAll erase=" << erase << endl;
    viewport()->repaint( erase );
}

void KWCanvas::print( QPainter *painter, KPrinter *printer )
{
    int from = printer->fromPage();
    int to = printer->toPage();
    kdDebug(32001) << "KWCanvas::print from=" << from << " to=" << to << endl;
    if ( !from && !to ) // 0, 0 means everything
    {
        from = printer->minPage();
        to = printer->maxPage();
    }
#ifdef HAVE_KDEPRINT
    if (  printer->currentPage() )
    {
        from = m_gui->getView()->currentPage() + 1;
        to = from;
        kdDebug() << "KWCanvas::print currentpage set -> printing " << from << endl;
    }
#endif
    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              to - from + 2, this );
    int j = 0;
    progress.setProgress( 0 );
        for ( int i = from; i <= to; i++ )
    {
        progress.setProgress( ++j );
        kapp->processEvents();

        if ( progress.wasCancelled() )
            break;

        if ( i > from ) printer->newPage();

        painter->save();
        int pgNum = i - 1;
        int yOffset = pgNum * doc->paperHeight();
        kdDebug(32001) << "printing page " << pgNum << " yOffset=" << yOffset << endl;
        QRect pageRect( 0, yOffset, doc->paperWidth(), doc->paperHeight() );
        painter->fillRect( pageRect, white );

        painter->translate( 0, -yOffset );
        painter->setBrushOrigin( 0, -yOffset );
        drawDocument( 0L, painter, pageRect.x(), pageRect.y(), pageRect.width(), pageRect.height() );
        kapp->processEvents();
        painter->restore();
    }
}

void KWCanvas::drawContents( QPainter *painter, int cx, int cy, int cw, int ch )
{
    // Note: in drawContents, the painter is already to the contents coordinates
    painter->setBrushOrigin( -contentsX(), -contentsY() );
    drawDocument( 0L, painter, cx, cy, cw, ch );
}

void KWCanvas::drawDocument( KWFrameSet * onlyFrameset, QPainter *painter, int cx, int cy, int cw, int ch, bool resetChanged )
{
    QRect crect( cx, cy, cw, ch );
    //kdDebug(32002) << "KWCanvas::drawDocument onlyFrameset=" << onlyFrameset << " crect: " << DEBUGRECT( crect ) << endl;
    bool focus = hasFocus() || viewport()->hasFocus();
    if ( painter->device()->devType() == QInternal::Printer )
        focus = false;

    bool onlyChanged = (onlyFrameset != 0L);

    if ( !onlyFrameset )      // no need for borders if we're only repainting the text
        drawBorders( 0L /*onlyFrameset*/, painter, crect );

    // Draw all framesets
    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( !onlyFrameset || (frameset == onlyFrameset) ) // Draw all, or draw only one
        {
            if ( frameset->isVisible() )
            {
                QColorGroup gb = QApplication::palette().active();
                if ( focus && m_currentFrameSetEdit && frameset == m_currentFrameSetEdit->frameSet() )     // Currently edited frameset
                    m_currentFrameSetEdit->drawContents( painter, crect, gb, onlyChanged, resetChanged );
                else
                    frameset->drawContents( painter, crect, gb, onlyChanged, resetChanged );
            }
        }
    }
}

void KWCanvas::drawBorders( KWFrameSet * onlyFrameset, QPainter *painter, const QRect & crect )
{
    bool clearEmptySpace = onlyFrameset == 0L;
    bool drawBack = onlyFrameset == 0L;
    QRegion region( crect );
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
            QRect frameRect( doc->zoomRect(  *frame ) );
            frameRect.rLeft() -= 1;
            frameRect.rTop() -= 1;
            frameRect.rRight() += 1;
            frameRect.rBottom() += 1;
            //kdDebug(32002) << "KWCanvas::drawBorders frameRect: " << DEBUGRECT( frameRect ) << endl;
            if ( !crect.intersects( frameRect ) )
                continue;

            // set the background color from the main frameset (why?)
            painter->setBrush( ( frameset->isAHeader() || frameset->isAFooter() ) ?
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

            if ( should_draw && !frameset->getGroupManager() ) {
                if ( clearEmptySpace )
                    region = region.subtract( frameRect );
                if ( !onlyFrameset || frameset == onlyFrameset )
                {
                    if ( !drawBack )
                        painter->setBrush( Qt::NoBrush );
                    painter->drawRect( frameRect );
                }
            }

            // Draw cell borders
            painter->setBrush( Qt::NoBrush );
            if ( frameset->getGroupManager() ) {
                if ( clearEmptySpace )
                    region = region.subtract( frameRect );
                if ( !onlyFrameset || frameset == onlyFrameset )
                {
                    if ( drawBack )
                        painter->fillRect( frameRect, frame->getBackgroundColor() );
                    // Always draw right and bottom
                    painter->drawLine( frameRect.right() + 1, frameRect.y() - 1,
                                       frameRect.right() + 1, frameRect.bottom()  + 1 );
                    painter->drawLine( frameRect.x() - 1, frameRect.bottom() + 1,
                                       frameRect.right() + 1, frameRect.bottom() + 1 );
                    uint row = frameset->getGroupManager()->getCell( frameset )->row;
                    uint col = frameset->getGroupManager()->getCell( frameset )->col;
                    if ( row == 0 ) // draw top only for 1st row
                        painter->drawLine( frameRect.x() - 1, frameRect.y() - 1,
                                           frameRect.right() + 1, frameRect.y() - 1 );
                    if ( col == 0 ) // draw left only for 1st column
                        painter->drawLine( frameRect.x() - 1, frameRect.y() - 1,
                                           frameRect.x() - 1, frameRect.bottom() + 1 );
                }
            }

            if ( !onlyFrameset || frameset == onlyFrameset )
            {
                // Draw frame borders

                if ( frameset->isAHeader() || frameset->isAFooter() )
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
    }

    // Draw page borders (red), unless when printing
    if ( painter->device()->devType() != QInternal::Printer )
    {
        painter->setPen( red );
        painter->setBrush( Qt::NoBrush );

        QRegion pageContentsReg;

        for ( int k = 0; k < doc->getPages(); k++ ) {
            QRect pageRect( 0, ( k * doc->paperHeight() ),
                            doc->paperWidth(), doc->paperHeight() );
            if ( crect.intersects( pageRect ) )
                painter->drawRect( pageRect );
            // Exclude red border line
            pageRect.rLeft() += 1;
            pageRect.rTop() += 1;
            pageRect.rRight() -= 1;
            pageRect.rBottom() -= 1;
            pageContentsReg += pageRect; // unite
        }

        if ( clearEmptySpace ) {
            // Clear empty space. This is also disabled when printing because
            // 1 - it needs pageContentsReg,  2 - not needed, this is probably only
            // needed when moving/resizing frames.
            painter->save();

            region &= pageContentsReg; // intersect

            // Region, but in device coordinates
            // ( ARGL why on earth isn't QPainter::setClipRegion in transformed coordinate system ?? )
            QRegion devReg;
            QArray<QRect>rs = region.rects();
            rs.detach();
            for ( uint i = 0 ; i < rs.size() ; ++i )
                rs[i] = painter->xForm( rs[i] );
            devReg.setRects( rs.data(), rs.size() );
            painter->setClipRegion( devReg );

            //kdDebug() << "KWCanvas::drawBorders clearEmptySpace in " << DEBUGRECT( region.boundingRect() ) << endl;
            painter->fillRect( region.boundingRect(), colorGroup().brush( QColorGroup::Base ) );
            painter->restore();
        }
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
            m_gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
        }
        return;
    }
    if ( m_currentFrameSetEdit && m_mouseMode==MM_EDIT )
        {
            m_currentFrameSetEdit->keyPressEvent( e );
            m_gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
        }
}

void KWCanvas::mpEditFrame( QMouseEvent *e, int mx, int my ) // mouse press in edit-frame mode
// This can be called by KWResizeHandle::mousePressEvent
{
    int x = static_cast<int>( mx / doc->zoomedResolutionX() );
    int y = static_cast<int>( my / doc->zoomedResolutionY() );
    mousePressed = true;
    frameMoved = false;
    frameResized = false;
    if ( e ) {
        // only simulate selection - we do real selection below
        int currentSelection = doc->selectFrame( x, y, TRUE );

        KWFrameSet *fs = doc->getFrameSet( x, y );
        if ( currentSelection != 0 && ( e->state() & ShiftButton ) && fs->getGroupManager() ) { // is table and we hold shift
            fs->getGroupManager()->selectUntil( fs );
            curTable = fs->getGroupManager();
        } else if ( currentSelection == 0 ) { // none selected
            selectAllFrames( FALSE );
        } else if ( currentSelection == 1 ) { // 1 selected
            if ( !( e->state() & ControlButton || e->state() & ShiftButton ) )
                selectAllFrames( FALSE );
            selectFrame( x, y, TRUE );
            curTable = fs->getGroupManager();
        } else if ( currentSelection == 2 ) { // was already selected
            if ( e->state() & ControlButton || e->state() & ShiftButton ) {
                selectFrame( x, y, FALSE );
                curTable = fs->getGroupManager();
            } else if ( viewport()->cursor().shape() != SizeAllCursor ) {
                selectAllFrames( FALSE );
                selectFrame( x, y, TRUE );
                curTable = fs->getGroupManager();
            }
        }
    }
    if( doc->getFirstSelectedFrame() )
        m_resizedFrameInitialSize = doc->getFirstSelectedFrame()->normalize();

    QList<KWFrame> selectedFrames = doc->getSelectedFrames();
    QList<FrameIndex> frameindexList;
    QList<FrameResizeStruct> frameindexMove;
    KWFrame *frame=0L;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameResizeStruct *move=new FrameResizeStruct;
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);
        index->m_iFrameSetIndex=doc->getFrameSetNum(frame->getFrameSet());
        move->sizeOfBegin=frame->normalize();
        move->sizeOfEnd=QRect();
        frameindexList.append(index);
        frameindexMove.append(move);
    }
    if(frameindexMove.count()!=0)
        cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"),doc,frameindexList, frameindexMove ) ;


    viewport()->setCursor( doc->getMouseCursor( x, y ) );

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
    int x = static_cast<int>( mx / doc->zoomedResolutionX() );
    int y = static_cast<int>( my / doc->zoomedResolutionY() );
    m_insRect = QRect( x, y, 0, 0 );
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
        int x = static_cast<int>( mx / doc->zoomedResolutionX() );
        int y = static_cast<int>( my / doc->zoomedResolutionY() );
        m_insRect = QRect( x, y, 0, 0 );
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
            {
                // Select the frame first
                mpEditFrame( e, mx, my );
                int x = static_cast<int>( mx / doc->zoomedResolutionX() );
                int y = static_cast<int>( my / doc->zoomedResolutionY() );
                if (!doc->getFrameSet( x, y ))
                    m_gui->getView()->openPopupMenuChangeAction( QCursor::pos() );
                else
                {
                    //todo create a popupMenu for frame (delete - properties etc...)
                    KWFrame *frame = doc->getFirstSelectedFrame();
                    // if a header/footer etc. Dont show the popup.
                    if((frame->getFrameSet() && frame->getFrameSet()->getFrameInfo() != FI_BODY))
                    {
                        mousePressed = false;
                        return;
                    }
                    m_gui->getView()->openPopupMenuEditFrame( QCursor::pos() );
                }
            }
            break;
            default: break;
        }
        mousePressed = false;
        return;
    }

    if ( e->button() == LeftButton ) {
	mousePressed = true;

        switch ( m_mouseMode ) {
        case MM_EDIT:
            {
                int x = static_cast<int>( mx / doc->zoomedResolutionX() );
                int y = static_cast<int>( my / doc->zoomedResolutionY() );
                KWFrameSet *fs = doc->getFrameSet( x, y );
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
    KWFrameSet *fs = frame->getFrameSet();
    if ( doc->processingType() == KWDocument::WP  && fs == doc->getFrameSet(0))
        return;

    // Get the mouse position from QCursor. Trying to get it from KWResizeHandle's
    // mouseMoveEvent leads to the frame 'jumping' because the events are received async.
    QPoint mousep = mapFromGlobal(QCursor::pos());
    int mx = mousep.x() + contentsX();
    int my = mousep.y() + contentsY();

    // Apply the grid
    int rastX = doc->gridX();
    int rastY = doc->gridY();
    mx = ( mx / rastX ) * rastX ;
    my = ( my / rastY ) * rastY;
    int x = static_cast<int>( mx / doc->zoomedResolutionX() );
    int y = static_cast<int>( my / doc->zoomedResolutionY() );

    // Calculate new frame coordinates, using minimum sizes, and keeping it in the bounds of the page
    int newLeft = frame->left();
    int newTop = frame->top();
    int newRight = frame->right();
    int newBottom = frame->bottom();

    if (top && newTop != y) {
        bool move=true;
        if (fs->isAFooter())
            move=false;
        if (newBottom-y < (int)(minFrameHeight+5))
            y=newBottom-minFrameHeight-5;
        if (y < static_cast<int>( frame->pageNum() * doc->ptPaperHeight()))
            y = static_cast<int>( frame->pageNum() * doc->ptPaperHeight() );

        if (move) newTop=y;
    } else if (bottom && newBottom != y) {
        bool move=true;
        if (fs->isAHeader())
            move=false;
        if (y-newTop < (int)(minFrameHeight+5))
            y=newTop+minFrameHeight+5;
        if (y >= static_cast<int>((frame->pageNum()+1) * doc->ptPaperHeight()))
        {
            y = static_cast<int>((frame->pageNum()+1) * doc->ptPaperHeight());
        }

        if (move) newBottom=y;
    }

    if (left && newLeft != x) {
        bool move=true;
        if (fs->isAHeader())
            move=false;
        if (fs->isAFooter())
            move=false;
        if (newRight-x < (int)minFrameWidth)
            x=newRight-minFrameWidth-5;
        if (x <= 0)
            x=0;
        if (move)
            newLeft=x;
    } else if (right && newRight != x) {
        bool move=true;
        if (fs->isAHeader()) move=false;
        if (fs->isAFooter()) move=false;
        if (x-newLeft < (int)minFrameWidth)
            x=newLeft+minFrameWidth+5;
        if (x > static_cast<int>(doc->ptPaperWidth()))
            x = static_cast<int>(doc->ptPaperWidth());

        if (move) newRight=x;
    }

    // Keep copy of old rectangle, for repaint()
    QRect oldRect( *frame );

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
    QRect newRect( *frame );
    // Repaing only the changed rects (oldRect U newRect)
    oldRect = doc->zoomRect(oldRect);
    oldRect.rLeft() -= 1;
    oldRect.rTop() -= 1;
    oldRect.rRight() += 1;
    oldRect.rBottom() += 1;
    newRect = doc->zoomRect(newRect);
    newRect.rLeft() -= 1;
    newRect.rTop() -= 1;
    newRect.rRight() += 1;
    newRect.rBottom() += 1;
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
                        QRect oldRect( *frame );
                        int page = doc->getPageOfRect( *frame );
                        // Move the frame
                        frame->moveBy( static_cast<int>((mx - oldMx) / doc->zoomedResolutionX()),
                                       static_cast<int>((my - oldMy) / doc->zoomedResolutionY()) );
                        // But not out of the page it was on initially
                        if ( doc->isOutOfPage( *frame, page ) )
                            frame->setRect( oldRect.x(), oldRect.y(), oldRect.width(), oldRect.height() );
                        // Calculate new rectangle for this frame
                        QRect newRect( *frame );
                        // Repaing only the changed rects (oldRect U newRect)
                        oldRect = doc->zoomRect(oldRect);
                        oldRect.rLeft() -= 1;
                        oldRect.rTop() -= 1;
                        oldRect.rRight() += 1;
                        oldRect.rBottom() += 1;
                        newRect = doc->zoomRect(newRect);
                        newRect.rLeft() -= 1;
                        newRect.rTop() -= 1;
                        newRect.rRight() += 1;
                        newRect.rBottom() += 1;
                        repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
                    }
                    // Move resize handles to new position
                    frame->updateResizeHandles();
                }
            }
        }

        if ( !updates.isEmpty() ) {
            //kdDebug() << "KWCanvas::mmEditFrameMove UPDATES" << endl;
            for ( unsigned int i = 0; i < updates.count(); i++ ) {
                KWGroupManager *grpMgr = updates.at( i );
                for ( unsigned k = 0; k < grpMgr->getNumCells(); k++ ) {
                    KWFrame * frame = grpMgr->getCell( k )->frameSet->getFrame( 0 );
                    QRect oldRect( *frame );
                    frame->moveBy( mx - oldMx, my - oldMy );
                    if ( frame->x() < 0 || frame->right() > static_cast<int>( doc->paperWidth() ) || frame->y() < 0 ) {
                        if ( undos.findRef( grpMgr ) == -1 )
                            undos.append( grpMgr );
                    }
                    // Calculate new rectangle for this frame
                    QRect newRect( *frame );
                    // Repaing only the changed rects (oldRect U newRect)
                    oldRect = doc->zoomRect(oldRect);
                    oldRect.rLeft() -= 1;
                    oldRect.rTop() -= 1;
                    oldRect.rRight() += 1;
                    oldRect.rBottom() += 1;
                    newRect = doc->zoomRect(newRect);
                    newRect.rLeft() -= 1;
                    newRect.rTop() -= 1;
                    newRect.rRight() += 1;
                    newRect.rBottom() += 1;
                    repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
                    // Move resize handles to new position
                    frame->updateResizeHandles();
                }
            }
        }

        frameMoved = true;

#if 0 // TODO
        if ( !undos.isEmpty() ) {
            for ( unsigned int i = 0; i < undos.count(); i++ ) {
                undos.at( i )->drawAllRects( p, contentsX(), contentsY() );
                undos.at( i )->moveBy( static_cast<int>((mx - oldMx) / doc->zoomedResolutionX()),
                                       static_cast<int>((my - oldMy) / doc->zoomedResolutionY()) );
                undos.at( i )->drawAllRects( p, contentsX(), contentsY() );
            }
        }
#endif

    }
}

void KWCanvas::mmCreate( int mx, int my ) // Mouse move when creating a frame
{
    /*if ( doRaster )*/ {
        mx = ( mx / doc->gridX() ) * doc->gridX();
        my = ( my / doc->gridY() ) * doc->gridY();
    }

    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );

    if ( deleteMovingRect ) {
#if 0
        if ( useAnchor ) {
            p.drawLine( anchor->getOrigin(), m_insRect.topLeft() );
        }
#endif
        p.drawRect( doc->zoomRect( m_insRect ) );
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
    m_insRect.setWidth( m_insRect.width() + (mx - oldMx) / doc->zoomedResolutionX() );
    m_insRect.setHeight( m_insRect.height() + (my - oldMy) / doc->zoomedResolutionY() );

    // But not out of the page
    QRect r = m_insRect.normalize();
    if ( doc->isOutOfPage( r, doc->getPageOfRect( r ) ) )
    {
        m_insRect.setWidth( m_insRect.width() - (mx - oldMx) / doc->zoomedResolutionX() );
        m_insRect.setHeight( m_insRect.height() - (my - oldMy) / doc->zoomedResolutionY() );
    }
#if 0
    if ( useAnchor ) {
        p.drawLine( anchor->getOrigin(), m_insRect.topLeft() );
    }
#endif
    p.drawRect( doc->zoomRect( m_insRect ) );
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
        int x = static_cast<int>( mx / doc->zoomedResolutionX() );
        int y = static_cast<int>( my / doc->zoomedResolutionY() );
        switch ( m_mouseMode ) {
            case MM_EDIT_FRAME:
                viewport()->setCursor( doc->getMouseCursor( x, y ) );
                break;
            default: break;
        }
    }
}

/*================================================================*/

void KWCanvas::mrEditFrame() // Can be called from KWCanvas and from KWResizeHandle's mouseReleaseEvents
{
    KWFrame *firstFrame = doc->getFirstSelectedFrame();
    if (!firstFrame) return;
#if 0
    if ( doc->processingType() == KWDocument::DTP ) // ?
        setRuler2Frame( firstFrame );
#endif
    m_gui->getHorzRuler()->setFrameStart( firstFrame->x() ); // does this need zoomItX() ?
    // Why not the same with Y ?

    if ( frameMoved || frameResized )
    {
        KWGroupManager *grpMgr = firstFrame->getFrameSet()->getGroupManager();
        if (grpMgr) {
            grpMgr->recalcCols();
            grpMgr->recalcRows();
            grpMgr->updateTempHeaders();
            //repaintTableHeaders( grpMgr );
        }

        // Create command
        if ( frameResized )
        {
            KWFrame *frame = doc->getFirstSelectedFrame();
            FrameIndex index;
            FrameResizeStruct tmpResize;
            tmpResize.sizeOfBegin = m_resizedFrameInitialSize;
            tmpResize.sizeOfEnd = frame->normalize();

            index.m_iFrameIndex = frame->getFrameSet()->getFrameFromPtr(frame);
            index.m_iFrameSetIndex = doc->getFrameSetNum(frame->getFrameSet());

            KWFrameResizeCommand *cmd = new KWFrameResizeCommand( i18n("Resize Frame"), doc, index, tmpResize ) ;
            doc->addCommand(cmd);

            doc->frameChanged( frame, m_gui->getView() ); // repaint etc.
            delete cmdMoveFrame; // Unused after all
            cmdMoveFrame = 0L;
        }
        else
        {
            ASSERT( cmdMoveFrame ); // has been created by mpEditFrame
            if( cmdMoveFrame )
            {
                QList<KWFrame> selectedFrames = doc->getSelectedFrames();
                QList<FrameResizeStruct> tmpListFrameMoved = cmdMoveFrame->getListFrameMoved();
                int i = 0;
                for(KWFrame * frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next(), ++i )
                    tmpListFrameMoved.at(i)->sizeOfEnd = frame->normalize();

                cmdMoveFrame->setListFrameMoved(tmpListFrameMoved);
                doc->addCommand(cmdMoveFrame);
                doc->framesChanged( selectedFrames, m_gui->getView() ); // repaint etc.

                cmdMoveFrame=0L;
            }
        }

    } else {
        // Frame not resized nor moved
        // doc->repaintAllViewsExcept( m_gui->getView() ); // Do we need this ?
    }
    doc->repaintAllViews();
    mousePressed = false;
}

void KWCanvas::mrCreateText()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        KWFrameDia * frameDia = new KWFrameDia( this, frame, doc, FT_TEXT);
        connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
        frameDia->setCaption(i18n("Connect frame"));
        frameDia->show();
        delete frameDia;
    }
}

void KWCanvas::mrCreatePixmap()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() && !m_PixmapName.isEmpty() ) {
        KWPictureFrameSet *frameset = new KWPictureFrameSet( doc );
        frameset->setFileName( m_PixmapName, m_insRect.size() );
        m_insRect = m_insRect.normalize();
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(),
                                     m_insRect.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
}

void KWCanvas::mrCreatePart() // mouse release, when creating part or kspread table
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        doc->insertObject( m_insRect, m_partEntry, contentsX(), contentsY() );
    }
    setMouseMode( MM_EDIT );
}

void KWCanvas::mrCreateFormula()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( doc, this );
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frameset->addFrame( frame );
        doc->addFrameSet( frameset );
        doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
}

void KWCanvas::mrCreateTable()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        if ( tcols * minFrameWidth + m_insRect.x() > doc->paperWidth() )
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
                    KWFrame *frame = new KWFrame(_frameSet, m_insRect.x(), m_insRect.y(), (doc->paperWidth() - m_insRect.x())/tcols, m_insRect.height() );
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
        doc->updateAllFrames();
        doc->layout();
        repaintAll();
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
                //repaintAll(); ?
                break;
        }

	mousePressed = false;
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

    repaintAll();
    mousePressed = false;
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
    doc->repaintAllViews();
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
    doc->repaintAllViews();
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
    doc->repaintAllViews();
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
    doc->repaintAllViews();
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
    doc->repaintAllViews();
}

/*================================================================*/
void KWCanvas::setFrameBackgroundColor( const QBrush &_backColor )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    bool colorChanged=false;
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
        {
            colorChanged=true;
            frame->setBackgroundColor(_backColor);
        }
    }
    if(colorChanged)
    {
        KWFrameBackGroundColorCommand *cmd=new KWFrameBackGroundColorCommand(i18n("Change Frame BackGroundColor"),doc,frameindexList,oldColor,_backColor);
        doc->addCommand(cmd);
        doc->repaintAllViews();
    }
}


/*================================================================*/
void KWCanvas::editFrameProperties()
{
    KWFrame *frame = doc->getFirstSelectedFrame();
    if (!frame)
        return;
    KWFrameSet *fs = frame->getFrameSet();
    if ( fs->isAHeader() )
    {
        KMessageBox::sorry( this, i18n( "This is a Header frame, it can not be edited."), i18n( "Frame Properties"  ) );
        return;
    }
    if ( fs->isAFooter() )
    {
        KMessageBox::sorry( this, i18n( "This is a Footer frame, it can not be edited."),i18n( "Frame Properties"  ) );
        return;
    }

    KWFrameDia * frameDia = new KWFrameDia( this, frame);
    connect( frameDia, SIGNAL( changed() ), this, SLOT( frameDiaClosed() ) );
    frameDia->setCaption(i18n("Frame Properties"));
    frameDia->show();
    delete frameDia;
}

void KWCanvas::frameDiaClosed() // get rid of this?
{
    doc->repaintAllViews();
#if 0
    doc->updateAllFrames(); // already done in framedia
#endif
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
    if( !m_currentFrameSetEdit)
        return 0L;
    return m_currentFrameSetEdit->frameSet()->getGroupManager();
}

void KWCanvas::deleteFrame( KWFrame * frame )
{
    KWFrameSet * fs = frame->getFrameSet();
    QString cmdName;
    switch (fs->getFrameType() ) {
    case FT_TEXT:
        cmdName=i18n("Delete text frame");
        break;
    case FT_FORMULA:
        cmdName=i18n("Delete formula frame");
        break;
    case FT_PICTURE:
        cmdName=i18n("Delete picture frame");
        break;
    case FT_PART:
        cmdName=i18n("Delete object frame");
        break;
    case FT_BASE:
        ASSERT( 0 );
        break;
    }
    KWDeleteFrameCommand *cmd = new KWDeleteFrameCommand( cmdName, doc, frame );
    doc->addCommand( cmd );
    cmd->execute();
}

void KWCanvas::deleteTable( KWGroupManager *groupManager )
{
    if ( !groupManager )
        return;
    // ## TODO undo/redo support
    doc->delGroupManager( groupManager );
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWCanvas::setMouseMode( MouseMode _mm )
{
    if ( m_mouseMode != _mm )
    {
        selectAllFrames( false );
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

    //repaintAll(); ?
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

void KWCanvas::slotContentsMoving( int cx, int cy )
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
    m_gui->getVertRuler()->setOffset( 0, -getVertRulerPos(cy) );
    m_gui->getHorzRuler()->setOffset( -getHorzRulerPos(cx), 0 );
}

void KWCanvas::resizeEvent( QResizeEvent *e )
{
    slotContentsMoving( contentsX(), contentsY() );
    QScrollView::resizeEvent( e );
}

/*================================================================*/
void KWCanvas::scrollToOffset( int _x, int _y )
{
#if 0
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();
#endif

    setContentsPos( _x*doc->zoomedResolutionX(), _y*doc->zoomedResolutionY() );

#if 0
    if ( blinking )
        startBlinkCursor();
#endif
}

/*================================================================*/
void KWCanvas::setContentsPos( int x, int y )
{
    QScrollView::setContentsPos( x, y );
    //calcVisiblePages();
}


/*================================================================*/
int KWCanvas::getVertRulerPos(int y)
{
    int pageNum=1;
    if( m_currentFrameSetEdit )
        pageNum=m_currentFrameSetEdit->currentFrame()->pageNum() + 1;
    return ( -(y==-1 ? contentsY() : y) + (pageNum - 1) * doc->paperHeight() );
}

/*================================================================*/
int KWCanvas::getHorzRulerPos(int x)
{
    return ( -(x==-1 ? contentsX() : x) );
}

/*================================================================*/
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
    KWTextFrameSet * textfs = 0L;
    if ( m_currentFrameSetEdit )
        textfs = dynamic_cast<KWTextFrameSet *>(m_currentFrameSetEdit->frameSet());
    if ( !textfs )
        textfs = dynamic_cast<KWTextFrameSet *>(doc->getFrameSet( 0 ) );
    if ( !textfs )
        return;
    QTextDocument * textdoc = textfs->textDocument();
    for (QTextParag * parag = textdoc->firstParag(); parag ; parag = parag->next())
    {
        KWTextParag * p = static_cast<KWTextParag *>(parag);
        p->printRTDebug();
    }
}

#include "kwcanvas.moc"
