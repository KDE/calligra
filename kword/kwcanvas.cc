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
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwview.h"
#include "kwtextdocument.h"
#include "kwanchor.h"
#include "framedia.h"
#include "kwcommand.h"

#include <qtimer.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qprogressdialog.h>
#include <kaction.h>
#include <kdebug.h>
#include <kapp.h>
#include <kmessagebox.h>
#include <config.h>

KWCanvas::KWCanvas(QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent, "canvas", WNorthWestGravity | WResizeNoErase /*| WRepaintNoErase*/ ), doc( d )
// it looks like QRT doesn't paint all pixels (e.g. between paragraphs) so we can't use WRepaintNoErase
{
    m_gui = lGui;
    m_currentFrameSetEdit = 0L;
    mousePressed = false;

    cmdMoveFrame=0L;

    // Default table parameters.
    m_table.rows = 3;
    m_table.cols = 2;
    m_table.width = TblAuto;
    m_table.height = TblAuto;
    m_table.floating = true;

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
    resizeContents( doc->paperWidth(), doc->pageTop( doc->getPages() ) );

    // Create the current frameset-edit last, to have everything ready for it
    m_mouseMode = (MouseMode) -1;
    setMouseMode( MM_EDIT );
}

KWCanvas::~KWCanvas()
{
    if ( !m_gui->getView()->documentDeleted() )
        selectAllFrames( false ); // destroy resize handles properly (they are our children at the Qt level!)
    delete m_currentFrameSetEdit;
    m_currentFrameSetEdit = 0L;
}

void KWCanvas::repaintChanged( KWFrameSet * fs, bool resetChanged )
{
    assert(fs); // the new code can't support fs being 0L here. Mail me if it happens (DF)
    //kdDebug(32002) << "KWCanvas::repaintChanged this=" << this << " fs=" << fs << endl;
    QPainter p( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setBrushOrigin( -contentsX(), -contentsY() );
    QRect crect( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    drawFrameSet( fs, &p, crect, true, resetChanged );
}

void KWCanvas::repaintAll( bool erase /* = false */ )
{
    //kdDebug() << "KWCanvas::repaintAll erase=" << erase << endl;
    viewport()->repaint( erase );
}

void KWCanvas::print( QPainter *painter, KPrinter *printer )
{
    QValueList<int> pageList;
#ifndef HAVE_KDEPRINT
    int from = printer->fromPage();
    int to = printer->toPage();
    kdDebug(32001) << "KWCanvas::print from=" << from << " to=" << to << endl;
    if ( !from && !to ) // 0, 0 means everything
    {
        from = printer->minPage();
        to = printer->maxPage();
    }
    for ( int i = from; i <= to; i++ )
        pageList.append( i );
#else
    pageList = printer->pageList();
#endif
    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              pageList.count() + 1, this );
    int j = 0;
    progress.setProgress( 0 );
    QValueList<int>::Iterator it = pageList.begin();
    for ( ; it != pageList.end(); ++it )
    {
        progress.setProgress( ++j );
        kapp->processEvents();

        if ( progress.wasCancelled() )
            break;

        if ( it != pageList.begin() )
            printer->newPage();

        painter->save();
        int pgNum = (*it) - 1;
        int yOffset = doc->pageTop( pgNum );
        kdDebug(32001) << "printing page " << pgNum << " yOffset=" << yOffset << endl;
        QRect pageRect( 0, yOffset, doc->paperWidth(), doc->paperHeight() );
        painter->fillRect( pageRect, white );

        painter->translate( 0, -yOffset );
        painter->setBrushOrigin( 0, -yOffset );
        drawDocument( painter, pageRect );
        kapp->processEvents();
        painter->restore();
    }
}

void KWCanvas::drawContents( QPainter *painter, int cx, int cy, int cw, int ch )
{
    if ( isUpdatesEnabled() )
    {
        // Note: in drawContents, the painter is already to the contents coordinates
        painter->setBrushOrigin( -contentsX(), -contentsY() );
        drawDocument( painter, QRect( cx, cy, cw, ch ) );
    }
}

void KWCanvas::drawDocument( QPainter *painter, const QRect &crect )
{
    //kdDebug(32002) << "KWCanvas::drawDocument crect: " << DEBUGRECT( crect ) << endl;

    // Draw all framesets, and borders
    doc->drawBorders( painter, crect );

    QListIterator<KWFrameSet> fit = doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( frameset->isVisible() && !frameset->isFloating() )
            drawFrameSet( frameset, painter, crect, false, false );
    }
}

void KWCanvas::drawFrameSet( KWFrameSet * frameset, QPainter * painter,
                             const QRect & crect, bool onlyChanged, bool resetChanged )
{
    bool focus = hasFocus() || viewport()->hasFocus();
    if ( painter->device()->devType() == QInternal::Printer )
        focus = false;

    QColorGroup gb = QApplication::palette().active();
    if ( focus && m_currentFrameSetEdit && frameset == m_currentFrameSetEdit->frameSet() )
        // Currently edited frameset
        m_currentFrameSetEdit->drawContents( painter, crect, gb, onlyChanged, resetChanged );
    else
        frameset->drawContents( painter, crect, gb, onlyChanged, resetChanged );
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
        // missing: Copy ... well selection isn't possible either currently
        default:
            break;
            m_gui->getVertRuler()->setOffset( 0, -getVertRulerPos() );
        }
    }
    // The key events in read-write mode are handled by eventFilter(), otherwise
    // we don't get <Tab> key presses.
}

void KWCanvas::mpEditFrame( QMouseEvent *e, int mx, int my ) // mouse press in edit-frame mode
// This can be called by KWResizeHandle::mousePressEvent
{
    double x = mx / doc->zoomedResolutionX();
    double y = my / doc->zoomedResolutionY();
    mousePressed = true;
    frameMoved = false;
    frameResized = false;
    if ( e ) {
        // only simulate selection - we do real selection below
        int currentSelection = doc->selectFrame( x, y, TRUE );

        KWFrameSet *fs = doc->getFrameSet( x, y );
        if ( currentSelection != 0 && ( e->state() & ShiftButton ) && fs->getFrameType() == FT_TABLE ) { // is table and we hold shift
            curTable = static_cast<KWTableFrameSet *> (fs);
            curTable->selectUntil( x,y );
        } else if ( currentSelection == 0 ) { // none selected
            selectAllFrames( FALSE );
        } else if ( currentSelection == 1 ) { // 1 selected
            if ( !( e->state() & ControlButton || e->state() & ShiftButton ) )
                selectAllFrames( FALSE );
            selectFrame( x, y, TRUE );
            if(fs->getFrameType() == FT_TABLE) curTable = static_cast<KWTableFrameSet *> (fs);
            else curTable = 0L;
        } else if ( currentSelection == 2 ) { // was already selected
            if ( e->state() & ControlButton || e->state() & ShiftButton ) {
                selectFrame( x, y, FALSE );
                if(fs->getFrameType() == FT_TABLE) curTable = static_cast<KWTableFrameSet *> (fs);
                else curTable = 0L;
            } else if ( viewport()->cursor().shape() != SizeAllCursor ) {
                selectAllFrames( FALSE );
                selectFrame( x, y, TRUE );
                if(fs->getFrameType() == FT_TABLE) curTable = static_cast<KWTableFrameSet *> (fs);
                else curTable = 0L;
            }
        }
        emit frameSelectedChanged();
    }
    // At least one frame selected ?
    if( doc->getFirstSelectedFrame() )
    {
        KWFrame * frame = doc->getFirstSelectedFrame();
        m_resizedFrameInitialSize = frame->normalize();
    }

    QList<KWFrame> selectedFrames = doc->getSelectedFrames();
    QList<FrameIndex> frameindexList;
    QList<FrameResizeStruct> frameindexMove;
    KWFrame *frame=0L;
    // When moving many frames, we look at the bounding rect.
    // It's the one that will be checked against the limits, etc.
    m_boundingRect = KoRect();
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        KWFrameSet * fs = frame->getFrameSet();
        // If one cell belongs to a table, we are in fact moving the whole table
        KWTableFrameSet *table = fs->getGroupManager();
        // We'll have to do better in the long run
         if ( table )
            m_boundingRect |= table->boundingRect();
        else
            m_boundingRect |= *frame;
        FrameIndex *index=new FrameIndex;
        FrameResizeStruct *move=new FrameResizeStruct;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=fs->getFrameFromPtr(frame);

        move->sizeOfBegin=frame->normalize();
        move->sizeOfEnd=KoRect();
        frameindexList.append(index);
        frameindexMove.append(move);
    }
    m_hotSpot = KoPoint( x,y ) - m_boundingRect.topLeft();
    if(frameindexMove.count()!=0)
        cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"),doc,frameindexList, frameindexMove ) ;

    viewport()->setCursor( doc->getMouseCursor( x, y ) );

    deleteMovingRect = FALSE;
}

void KWCanvas::mpCreate( int mx, int my )
{
    mx = ( mx / doc->gridX() ) * doc->gridX();
    my = ( my / doc->gridX() ) * doc->gridY();
    double x = mx / doc->zoomedResolutionX();
    double y = my / doc->zoomedResolutionY();
    m_insRect.setCoords( x, y, 0, 0 );
    deleteMovingRect = FALSE;
}

void KWCanvas::mpCreatePixmap( int mx, int my )
{
    if ( !m_PixmapName.isEmpty() ) {
        QPixmap _pix( m_PixmapName );
        mx = ( mx / doc->gridX() ) * doc->gridX();
        my = ( my / doc->gridX() ) * doc->gridY();
        double x = mx / doc->zoomedResolutionX();
        double y = my / doc->zoomedResolutionY();
        m_insRect.setCoords( x, y, 0, 0 );
        deleteMovingRect = FALSE;
        //doRaster = FALSE;
        // TODO same zoom stuff as KWImage (for 1x1 at 100%)
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
            case MM_CREATE_FORMULA:
            case MM_CREATE_PIX:
            {
                // Select the frame first
                mpEditFrame( e, mx, my );
                double x = mx / doc->zoomedResolutionX();
                double y = my / doc->zoomedResolutionY();
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
                double x = mx / doc->zoomedResolutionX();
                double y = my / doc->zoomedResolutionY();
                KWFrameSet *fs = doc->getFrameSet( x, y );
                bool emitChanged = false; // to emit only once
                if ( fs && m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() != fs )
                {
                    // Terminate edition of that frameset
                    m_currentFrameSetEdit->terminate();
                    delete m_currentFrameSetEdit;
                    m_currentFrameSetEdit = 0L;
                    emitChanged = true;
                    repaintAll();
                }
                // Edit the frameset under the mouse, if any
                if ( fs && !m_currentFrameSetEdit )
                {
                    m_currentFrameSetEdit = fs->createFrameSetEdit( this );
                    emitChanged = true;
                }

                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mousePressEvent( e );

                if ( emitChanged )
                    emit currentFrameSetEditChanged();

            }
            break;
        case MM_EDIT_FRAME:
            mpEditFrame( e, mx, my );
            break;
        case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE:
        case MM_CREATE_FORMULA:
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

// Called by KWTableDia
void KWCanvas::createTable( unsigned int rows, unsigned int cols,
                            KWTblCellSize wid, KWTblCellSize hei,
                            bool isFloating )
{
    // Remember for next time in any case
    m_table.rows = rows;
    m_table.cols = cols;
    m_table.width = wid;
    m_table.height = hei;
    m_table.floating = isFloating;

    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit);

    if ( isFloating && edit )
    {
        m_insRect = KoRect( 0, 0, rows * 30, doc->ptPaperWidth()-50 /*10*/ ); // ## check those values
        KWTableFrameSet * table = createTable();
        edit->insertFloatingFrameSet( table, i18n("Insert Floating Table") );
        doc->addFrameSet( table ); // last since it triggers a redraw
        doc->updateAllFrames();
    }
    else
        setMouseMode( MM_CREATE_TABLE );
}

/*================================================================*/
void KWCanvas::mmEditFrameResize( bool top, bool bottom, bool left, bool right )
{
    // This one is called by KWResizeHandle

    // Can't resize the main frame of a WP document
    KWFrame *frame = doc->getFirstSelectedFrame();
    KWFrameSet *fs = frame->getFrameSet();
    if ( doc->processingType() == KWDocument::WP && fs == doc->getFrameSet(0))
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
    double x = mx / doc->zoomedResolutionX();
    double y = my / doc->zoomedResolutionY();

    // Calculate new frame coordinates, using minimum sizes, and keeping it in the bounds of the page
    double newLeft = frame->left();
    double newTop = frame->top();
    double newRight = frame->right();
    double newBottom = frame->bottom();

    if ( top && newTop != y && !fs->isAFooter() )
    {
        if (newBottom - y < minFrameHeight+5)
            y = newBottom - minFrameHeight - 5;
        y = QMAX( y, doc->ptPageTop( frame->pageNum() ) );
        newTop = y;
    } else if ( bottom && newBottom != y && !fs->isAHeader() )
    {
        if (y - newTop < minFrameHeight+5)
            y = newTop + minFrameHeight + 5;
        y = QMIN( y, doc->ptPageTop( frame->pageNum() + 1 ) );
        newBottom = y;
    }

    if ( left && newLeft != x && !fs->isAHeader() && !fs->isAFooter() )
    {
        if (newRight - x < minFrameWidth)
            x = newRight - minFrameWidth - 5;
        x = QMAX( x, 0 );
        newLeft = x;
    } else if ( right && newRight != x && !fs->isAHeader() && !fs->isAFooter() )
    {
        if (x - newLeft < minFrameWidth)
            x = newLeft + minFrameWidth + 5; // why +5 ?
        x = QMIN( x, doc->ptPaperWidth() );
        newRight = x;
    }

    // Keep copy of old rectangle, for repaint()
    QRect oldRect = frame->outerRect();

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
    QRect newRect( frame->outerRect() );
    // Repaing only the changed rects (oldRect U newRect)
    repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );

    //doRaster = TRUE;
    frameResized = TRUE;
}

void KWCanvas::mmEditFrameMove( int mx, int my )
{
    bool adjustPosNeeded = false;
    double cx = mx / doc->zoomedResolutionX();
    double cy = my / doc->zoomedResolutionY();
    // Move the bounding rect containing all the selected frames
    KoRect oldBoundingRect = m_boundingRect;
    int page = doc->getPageOfRect( m_boundingRect );
    //kdDebug() << "KWCanvas::mmEditFrameMove cx=" << cx << " page=" << page
    //          << "  boundingrect=" << DEBUGRECT(m_boundingRect) << endl;

    // (x and y separately for a better behaviour at limit of page)
    KoPoint p( m_boundingRect.topLeft() );
    //kdDebug() << "KWCanvas::mmEditFrameMove hotspot.x=" << m_hotSpot.x() << " cx=" << cx << endl;
    p.setX( cx - m_hotSpot.x() );
    //kdDebug() << "KWCanvas::mmEditFrameMove p.x is now " << p.x() << endl;
    m_boundingRect.moveTopLeft( p );
    //kdDebug() << "KWCanvas::mmEditFrameMove boundingrect now " << DEBUGRECT(m_boundingRect) << endl;
    // But not out of the margins
    if ( m_boundingRect.left() < 0 )
    {
        m_boundingRect.moveBy( -m_boundingRect.left(), 0 );
        adjustPosNeeded = true;
    }
    else if ( m_boundingRect.right() > doc->ptPaperWidth() )
    {
        m_boundingRect.moveBy( doc->ptPaperWidth() - m_boundingRect.right(), 0 );
        adjustPosNeeded = true;
    }
    // Now try Y
    p = m_boundingRect.topLeft();
    p.setY( cy - m_hotSpot.y() );
    m_boundingRect.moveTopLeft( p );
    // -- Don't limit to the current page. Let the user move a frame between pages --
    // But we still want to limit to 0 - lastPage
    if ( m_boundingRect.top() < 0 )
    {
        m_boundingRect.moveBy( 0, 0 - m_boundingRect.top() );
        adjustPosNeeded = true;
    }
    else if ( m_boundingRect.bottom() > ( doc->getPages()+1 ) * doc->ptPaperHeight() )
    {
        m_boundingRect.moveBy( 0, ( doc->getPages()+1 ) * doc->ptPaperHeight() - m_boundingRect.bottom() );
        adjustPosNeeded = true;
    }

    /*kdDebug() << "boundingRect moved by " << m_boundingRect.left() - oldBoundingRect.left() << ","
              << m_boundingRect.top() - oldBoundingRect.top() << endl;
    kdDebug() << " boundingX+hotspotX=" << m_boundingRect.left() + m_hotSpot.x() << endl;
    kdDebug() << " cx=" << cx << endl;*/

    QList<KWTableFrameSet> tablesMoved;
    tablesMoved.setAutoDelete( FALSE );
    bool bFirst = true;

    QListIterator<KWFrameSet> framesetIt( doc->framesetsIterator() );
    for ( ; framesetIt.current(); ++framesetIt, bFirst=false )
    {
        KWFrameSet *frameset = framesetIt.current();
        // Can't move main frameset of a WP document
        if ( doc->processingType() == KWDocument::WP && bFirst ||
             frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() != FI_BODY )
            continue;
        // Can't move frame of floating frameset
        if ( frameset->isFloating() ) continue;

        frameMoved = true;

        QListIterator<KWFrame> frameIt( frameset->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame *frame = frameIt.current();
            if ( frame->isSelected() ) {
                if ( frameset->getFrameType() == FT_TABLE ) {
                    if ( tablesMoved.findRef( static_cast<KWTableFrameSet *> (frameset) ) == -1 )
                        tablesMoved.append( static_cast<KWTableFrameSet *> (frameset));
                } else {
                    QRect oldRect( frame->outerRect() );
                    // Move the frame
                    frame->moveTopLeft( frame->topLeft() + m_boundingRect.topLeft() - oldBoundingRect.topLeft() );
                    // Calculate new rectangle for this frame
                    QRect newRect( frame->outerRect() );
                    // Repaing only the changed rects (oldRect U newRect)
                    repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
                    // Move resize handles to new position
                    frame->updateResizeHandles();
                }
            }
        }
    }

    if ( !tablesMoved.isEmpty() ) {
        //kdDebug() << "KWCanvas::mmEditFrameMove TABLESMOVED" << endl;
        for ( unsigned int i = 0; i < tablesMoved.count(); i++ ) {
            KWTableFrameSet *table = tablesMoved.at( i );
            for ( unsigned k = 0; k < table->getNumCells(); k++ ) {
                KWFrame * frame = table->getCell( k )->getFrame( 0 );
                QRect oldRect( frame->outerRect() );
                frame->moveTopLeft( frame->topLeft() + m_boundingRect.topLeft() - oldBoundingRect.topLeft() );
                // Calculate new rectangle for this frame
                QRect newRect( frame->outerRect() );
                // Repaing only the changed rects (oldRect U newRect)
                repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
                // Move resize handles to new position
                frame->updateResizeHandles();
            }
        }
    }

    // Doesn't work ! It makes the cursor jump.
    // I have tried every combination of contentsToViewport and viewport()->mapToGlobal etc., no luck
    /*if ( adjustPosNeeded )
    {
        QPoint pos = mapToGlobal( doc->zoomPoint( m_boundingRect.topLeft() + m_hotSpot ) );
        kdDebug() << "ADJUSTING ptcoordX=" << m_boundingRect.left()+m_hotSpot.x()
                  << " globalCoordX=" << pos.x() << " currentGlobalX=" << QCursor::pos().x() << endl;
        QCursor::setPos( pos );
    }*/
}

void KWCanvas::mmCreate( int mx, int my ) // Mouse move when creating a frame
{
    /*if ( doRaster ) {*/
        mx = ( mx / doc->gridX() ) * doc->gridX();
        my = ( my / doc->gridY() ) * doc->gridY();
    //}

    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );

    if ( deleteMovingRect )
        drawMovingRect( p );

    int page = doc->getPageOfRect( m_insRect );
    KoRect oldRect = m_insRect;

    // Resize the rectangle
    m_insRect.setRight( mx / doc->zoomedResolutionX() );
    m_insRect.setBottom( my / doc->zoomedResolutionY() );

    // But not out of the page
    KoRect r = m_insRect.normalize();
    if ( doc->isOutOfPage( r, page ) )
    {
        m_insRect = oldRect;
        // #### QCursor::setPos( viewport()->mapToGlobal( m_insRect.bottomRight() ) );
    }

    drawMovingRect( p );
    p.end();
    deleteMovingRect = true;
}

void KWCanvas::drawMovingRect( QPainter & p )
{
    p.setPen( black );
    p.drawRect( doc->zoomRect( m_insRect ) );
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

                if ( viewport()->cursor().shape() == SizeAllCursor /*&& ( mx != oldMx || my != oldMy )*/ )
                    mmEditFrameMove( mx, my );
                deleteMovingRect = TRUE;
            } break;
            case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
            case MM_CREATE_TABLE: case MM_CREATE_FORMULA:
                mmCreate( mx, my );
            default: break;
        }
    } else {
        double x = mx / doc->zoomedResolutionX();
        double y = my / doc->zoomedResolutionY();
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
    m_gui->getHorzRuler()->setFrameStart( doc->zoomItX( firstFrame->x() ) );
    // Why not the same with Y ?

    if ( frameMoved || frameResized )
    {
        KWTableFrameSet *table = firstFrame->getFrameSet()->getGroupManager();
        if (table) {
            table->recalcCols();
            table->recalcRows();
            table->updateTempHeaders();
            if(frameResized)
                table->refreshSelectedCell();
            //repaintTableHeaders( table );
        }

        // Create command
        if ( frameResized )
        {
            KWFrame *frame = doc->getFirstSelectedFrame();
            ASSERT( frame );
            if ( frame )
            {
                FrameIndex index;
                FrameResizeStruct tmpResize;
                tmpResize.sizeOfBegin = m_resizedFrameInitialSize;
                tmpResize.sizeOfEnd = frame->normalize();

                index.m_pFrameSet=frame->getFrameSet();
                index.m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);

                KWFrameResizeCommand *cmd = new KWFrameResizeCommand( i18n("Resize Frame"), doc, index, tmpResize ) ;
                doc->addCommand(cmd);

                doc->frameChanged( frame, m_gui->getView() ); // repaint etc.
            }
            delete cmdMoveFrame; // Unused after all
            cmdMoveFrame = 0L;
        }
        else
        {
            ASSERT( cmdMoveFrame ); // has been created by mpEditFrame
            if( cmdMoveFrame )
            {
                // Store final positions
                QList<KWFrame> selectedFrames = doc->getSelectedFrames();
                int i = 0;
                for(KWFrame * frame=selectedFrames.first(); frame; frame=selectedFrames.next(), ++i )
                    cmdMoveFrame->listFrameMoved().at(i)->sizeOfEnd = frame->normalize();

                doc->addCommand(cmdMoveFrame);
                doc->framesChanged( selectedFrames, m_gui->getView() ); // repaint etc.

                cmdMoveFrame = 0L;
            }
        }
    }
    doc->repaintAllViews();
    mousePressed = false;
}

void KWCanvas::mrCreateText()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        KWFrameDia frameDia( this, frame, doc, FT_TEXT );
        frameDia.setCaption(i18n("Connect frame"));
        frameDia.show();
    }
    setMouseMode( MM_EDIT );
    doc->repaintAllViews();
    emit docStructChanged(TextFrames);
}

void KWCanvas::mrCreatePixmap()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() && !m_PixmapName.isEmpty() ) {
        KWPictureFrameSet *frameset = new KWPictureFrameSet( doc, QString::null /*automatic name*/ );
        frameset->setFileName( m_PixmapName, doc->zoomRect( m_insRect ).size() );
        m_insRect = m_insRect.normalize();
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(),
                                     m_insRect.height() );
        frameset->addFrame( frame, false );
        doc->addFrameSet( frameset );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a picture frame"), doc,  frame) ;
        doc->addCommand(cmd);
        doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Pictures);
}

void KWCanvas::mrCreatePart() // mouse release, when creating part
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        doc->insertObject( m_insRect, m_partEntry );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Embedded);
}

void KWCanvas::mrCreateFormula()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( doc, QString::null );
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frameset->addFrame( frame, false );
        doc->addFrameSet( frameset );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a formula frame"), doc,  frame) ;
        doc->addCommand(cmd);
        doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(FormulaFrames);
}

void KWCanvas::mrCreateTable()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > doc->gridX() && m_insRect.height() > doc->gridY() ) {
        if ( m_table.cols * minFrameWidth + m_insRect.x() > doc->ptPaperWidth() )
        {
            KMessageBox::sorry(0, i18n("KWord is unable to insert the table because there\n"
                                       "is not enough space available."));
        }
        else {
            KWTableFrameSet * table = createTable();
            // Done at the end so that finalize is called
            doc->addFrameSet( table );
            // ## TODO undo/redo support. KWCreateFrameCommand won't do it, we need a new command.
            // Only here, not in createTable() (anchored tables are handled differently)
            emit docStructChanged(Tables);
        }
        doc->updateAllFrames();
        doc->layout();
        repaintAll();

    }
    setMouseMode( MM_EDIT );
}

KWTableFrameSet * KWCanvas::createTable() // uses m_insRect and m_table to create the table
{
    KWTableFrameSet *table = new KWTableFrameSet( doc, QString::null /*automatic name*/ );

    // Create a set of cells with random-size frames.
    for ( unsigned int i = 0; i < m_table.rows; i++ ) {
        for ( unsigned int j = 0; j < m_table.cols; j++ ) {
            KWTableFrameSet::Cell *cell = new KWTableFrameSet::Cell( table, i, j, QString::null /*automatic name*/ );
            KWFrame *frame = new KWFrame(cell, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height(), RA_NO );
            cell->addFrame( frame, false );
            frame->setFrameBehaviour(AutoExtendFrame);
            frame->setNewFrameBehaviour(NoFollowup);
        }
    }
    table->setHeightMode( m_table.height );
    table->setWidthMode( m_table.width );
    table->setBoundingRect( m_insRect );
    return table;
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
            case MM_CREATE_PART:
                mrCreatePart();
                break;
            case MM_CREATE_TABLE:
                mrCreateTable();
                break;
            case MM_CREATE_FORMULA:
                mrCreateFormula();
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
    bool leftFrameBorderChanged=false;

    if (!_b)
        _frmBrd.ptWidth=0;


    KWFrame *frame=0L;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getLeftBorder();
        tmp->m_EFrameType= FBLeft;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);


        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (_b)
        {
            if (_frmBrd!=frame->getLeftBorder())
            {
                leftFrameBorderChanged=true;
                frame->setLeftBorder(_frmBrd);
            }
        }
        else
        {
            if(frame->getLeftBorder().ptWidth!=0)
            {
                leftFrameBorderChanged=true;
                frame->setLeftBorder(_frmBrd);
            }
        }
    }
    if(leftFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Left Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
        doc->addCommand(cmd);
        doc->repaintAllViews();
    }
}

/*================================================================*/
void KWCanvas::setRightFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    QList<FrameBorderTypeStruct> tmpBorderList;
    QList<FrameIndex> frameindexList;
    bool rightFrameBorderChanged=false;
    KWFrame *frame=0L;
    if (!_b)
        _frmBrd.ptWidth=0;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getRightBorder();
        tmp->m_EFrameType= FBRight;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);

        tmpBorderList.append(tmp);
        frameindexList.append(index);

        if (_b)
        {
            if (_frmBrd!=frame->getRightBorder())
            {
                rightFrameBorderChanged=true;
                frame->setRightBorder(_frmBrd);
            }
        }
        else
        {
            if(frame->getRightBorder().ptWidth!=0)
            {
                frame->setRightBorder(_frmBrd);
                rightFrameBorderChanged=true;
            }
        }
    }
    if( rightFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Right Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
        doc->addCommand(cmd);
        doc->repaintAllViews();
    }
}

/*================================================================*/
void KWCanvas::setTopFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;

    QList<FrameBorderTypeStruct> tmpBorderList;
    QList<FrameIndex> frameindexList;
    bool topFrameBorderChanged=false;

    KWFrame *frame=0L;
    if (!_b)
        _frmBrd.ptWidth=0;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getTopBorder();
        tmp->m_EFrameType= FBTop;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (_b)
        {
            if (_frmBrd!=frame->getTopBorder())
            {
                topFrameBorderChanged=true;
                frame->setTopBorder(_frmBrd);
            }
        }
        else
        {
            if(frame->getTopBorder().ptWidth!=0)
            {
                topFrameBorderChanged=true;
                frame->setTopBorder(_frmBrd);
            }
        }
    }
    if(topFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Top Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
        doc->addCommand(cmd);
        doc->repaintAllViews();
    }
}

/*================================================================*/
void KWCanvas::setBottomFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    bool bottomFrameBorderChanged=false;
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

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);


        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (_b)
        {
            if (_frmBrd!=frame->getBottomBorder())
            {
                bottomFrameBorderChanged=true;
                frame->setBottomBorder(_frmBrd);
            }
        }
        else
        {
            if(frame->getBottomBorder().ptWidth!=0)
            {
                bottomFrameBorderChanged=true;
                frame->setBottomBorder(_frmBrd);
            }
        }
    }
    if(bottomFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Bottom Border frame"),doc,frameindexList,tmpBorderList,_frmBrd);
        doc->addCommand(cmd);
        doc->repaintAllViews();
    }
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

    int m_IindexFrame;
    int m_IindexFrameSet;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        FrameIndex *index=new FrameIndex;
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getBottomBorder();
        tmp->m_EFrameType= FBBottom;


        m_IindexFrame=frame->getFrameSet()->getFrameFromPtr(frame);

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=m_IindexFrame;

        tmpBorderListBottom.append(tmp);
        frameindexListBottom.append(index);

        index=new FrameIndex;
        tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getTopBorder();
        tmp->m_EFrameType= FBTop;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=m_IindexFrame;


        tmpBorderListTop.append(tmp);
        frameindexListTop.append(index);

        index=new FrameIndex;
        tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getRightBorder();
        tmp->m_EFrameType= FBRight;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=m_IindexFrame;

        tmpBorderListRight.append(tmp);
        frameindexListRight.append(index);

        index=new FrameIndex;
        tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getLeftBorder();
        tmp->m_EFrameType= FBLeft;

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=m_IindexFrame;

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

        index->m_pFrameSet=frame->getFrameSet();
        index->m_iFrameIndex=frame->getFrameSet()->getFrameFromPtr(frame);


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
    frameDia->setCaption(i18n("Frame Properties"));
    frameDia->show();
    delete frameDia;
}

void KWCanvas::updateFrameFormat()
{
    doc->refreshFrameBorderButton();
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

void KWCanvas::selectFrame( double mx, double my, bool select )
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

KWTableFrameSet *KWCanvas::getTable()
{
    if( !m_currentFrameSetEdit)
        return 0L;

    if(m_currentFrameSetEdit->frameSet()->getFrameType() == FT_TABLE)
        return static_cast<KWTableFrameSet *> (m_currentFrameSetEdit->frameSet());

    return 0L;
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
    case FT_TABLE:
    case FT_BASE:
        ASSERT( 0 );
        break;
    }
    KWDeleteFrameCommand *cmd = new KWDeleteFrameCommand( cmdName, doc, frame );
    doc->addCommand( cmd );
    cmd->execute();
}

void KWCanvas::deleteTable( KWTableFrameSet *table )
{
    if ( !table )
        return;
    deleteFrameSetEditTable( table );
    // ## TODO undo/redo support
    doc->delFrameSet( table );
    doc->updateAllFrames();
    doc->layout();
    doc->repaintAllViews();
}

void KWCanvas::deleteFrameSetEditTable( KWTableFrameSet *table )
{
    if ( m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() == table )
    {
        // Terminate edition of that frameset
        m_currentFrameSetEdit->terminate();
        delete m_currentFrameSetEdit;
        m_currentFrameSetEdit = 0L;
        repaintAll();
    }
}

void KWCanvas::setMouseMode( MouseMode newMouseMode )
{
    if ( m_mouseMode != newMouseMode )
    {
        KWFrame * frame = doc->getFirstSelectedFrame();
        selectAllFrames( false );

        if ( newMouseMode != MM_EDIT )
        {
            // Terminate edition of current frameset
            if ( m_currentFrameSetEdit )
            {
                m_currentFrameSetEdit->terminate();
                delete m_currentFrameSetEdit;
                m_currentFrameSetEdit = 0L;
                emit currentFrameSetEditChanged();
                repaintAll();
            }
        } else if ( doc->isReadWrite() )
        {
            ASSERT( !m_currentFrameSetEdit );
            // When switching to edit mode, start edition of main frameset
            // If a frame was selected, we edit that one instead - ## well, its frameset at least.
            KWFrameSet * fs = frame ? frame->getFrameSet() : doc->getFrameSet( 0 );
            ASSERT( fs );
            KWTableFrameSet *table = fs->getGroupManager();
            if ( fs )
            {
                //kdDebug() << "KWCanvas::setMouseMode editing " << fs << endl;
                if(table)
                {
                    m_currentFrameSetEdit=table->createFrameSetEdit(this);
                (static_cast<KWTableFrameSetEdit *>(m_currentFrameSetEdit))->setCurrentCell( fs );
                }
                else
                    m_currentFrameSetEdit = fs->createFrameSetEdit( this );
                emit currentFrameSetEditChanged();
            }
        }
    }

    m_mouseMode = newMouseMode;
    //mmUncheckAll();
    //m_gui->getView()->setTool( m_mouseMode );
    emit currentMouseModeChanged(m_mouseMode);

    switch ( m_mouseMode ) {
        case MM_EDIT: {
            viewport()->setCursor( ibeamCursor );
            //mm_menu->setItemChecked( mm_edit, TRUE );
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
    kdDebug() << "KWCanvas::scrollToOffset " << _x << "," << _y << endl;
#if 0
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();
#endif

    setContentsPos( doc->zoomItX( _x ), doc->zoomItY( _y ) );

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
        pageNum = m_currentFrameSetEdit->currentFrame()->pageNum() + 1;
    return ( -(y==-1 ? contentsY() : y) + doc->pageTop(pageNum - 1) );
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
	switch ( e->type() ) {
            case QEvent::FocusIn:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->focusInEvent();
                return TRUE;
            case QEvent::FocusOut:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->focusOutEvent();
                return TRUE;
            case QEvent::KeyPress:
            {
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
#ifndef NDEBUG
                // Debug keys
                if ( ( keyev->state() & ControlButton ) && ( keyev->state() & ShiftButton ) )
                {
                    switch ( keyev->key() ) {
                        case Key_P: // 'P' -> paragraph debug
                            printRTDebug( 0 );
                            break;
                        case Key_V: // 'V' -> verbose parag debug
                            printRTDebug( 1 );
                            break;
                        case Key_F: // 'F' -> frames debug
                            doc->printDebug();
                            break;
                        default:
                            break;
                    };
                    // For some reason 'T' doesn't work (maybe kxkb)
                }
#endif
                if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT )
                {
                    m_currentFrameSetEdit->keyPressEvent( keyev );
                    return TRUE;
                }
            } break;
            default:
                break;
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

#ifndef NDEBUG
void KWCanvas::printRTDebug( int info )
{
    KWTextFrameSet * textfs = 0L;
    if ( m_currentFrameSetEdit )
        textfs = dynamic_cast<KWTextFrameSet *>(m_currentFrameSetEdit->frameSet());
    if ( !textfs )
        textfs = dynamic_cast<KWTextFrameSet *>(doc->getFrameSet( 0 ) );
    if ( textfs )
        textfs->printRTDebug( info );
}
#endif

#include "kwcanvas.moc"
