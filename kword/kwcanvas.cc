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
#include "kwviewmode.h"
#include "kwtextdocument.h"
#include "kwanchor.h"
#include "kwdrag.h"
#include "framedia.h"
#include "kwcommand.h"

#include <qtimer.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qobjectlist.h>

#include <kaction.h>
#include <kdebug.h>
#include <kapp.h>
#include <kmessagebox.h>
#include <config.h>
#include <assert.h>

KWCanvas::KWCanvas(QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent, "canvas", WNorthWestGravity | WResizeNoErase | WRepaintNoErase ), m_doc( d )
{
    m_gui = lGui;
    m_currentFrameSetEdit = 0L;
    m_mousePressed = false;
    m_viewMode = new KWViewModeNormal( this ); // maybe pass as parameter, for initial value ( loaded from doc ) ?
    cmdMoveFrame=0L;

    // Default table parameters.
    m_table.rows = 3;
    m_table.cols = 2;
    m_table.width = KWTableFrameSet::TblAuto;
    m_table.height = KWTableFrameSet::TblAuto;
    m_table.floating = true;

    curTable = 0L;

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setAcceptDrops( TRUE );

    setKeyCompression( TRUE );
    viewport()->setMouseTracking( TRUE );

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

    connect( m_doc, SIGNAL( newContentsSize() ),
             this, SLOT( slotNewContentsSize() ) );

    connect( m_doc, SIGNAL( sig_terminateEditing( KWFrameSet * ) ),
             this, SLOT( terminateEditing( KWFrameSet * ) ) );

    slotNewContentsSize();

    setMouseMode( MM_EDIT );
    // Create the current frameset-edit last, to have everything ready for it
    KWFrameSet * fs = m_doc->getFrameSet( 0 );
    ASSERT( fs );
    if ( fs )
        m_currentFrameSetEdit = fs->createFrameSetEdit( this );
}

KWCanvas::~KWCanvas()
{
    // Let the frames destroy their resize handles themselves (atm they are our children at the Qt level!)
    // We can't call selectAllFrames since the doc my already be deleted (no frameset anymore etc.)
    // The real fix would be to create an object for 'selected frame' and store it in the view/canvas.
    // (and remove bool KWFrame::selected - so that a frame can be selected in a view and not in another)
    QObjectList *l = queryList( "KWResizeHandle" );
    QObjectListIt it( *l );
    for ( ; it.current() ; ++it )
    {
        QWidget * w = static_cast<QWidget*>(it.current());
        w->reparent(0L, QPoint(0,0)); // Yes, this is really an awful hack
        w->hide();
    }
    delete l;

    delete m_currentFrameSetEdit;
    m_currentFrameSetEdit = 0L;
    delete m_viewMode;
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
        int yOffset = m_doc->pageTop( pgNum );
        kdDebug(32001) << "printing page " << pgNum << " yOffset=" << yOffset << endl;
        QRect pageRect( 0, yOffset, m_doc->paperWidth(), m_doc->paperHeight() );
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

    // Draw all framesets contents
    QListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        drawFrameSet( frameset, painter, crect, false, false );
    }

    // Draw the outside of the pages (shadow, gray area)
    if ( painter->device()->devType() != QInternal::Printer ) // except when printing
    {
        QRegion emptySpaceRegion( crect );
        m_doc->createEmptyRegion( crect, emptySpaceRegion, m_viewMode );
        m_viewMode->drawPageBorders( painter, crect, emptySpaceRegion );
    }
}

void KWCanvas::drawFrameSet( KWFrameSet * frameset, QPainter * painter,
                             const QRect & crect, bool onlyChanged, bool resetChanged )
{
    if ( !frameset->isVisible() || frameset->isFloating() )
        return;

    bool focus = hasFocus() || viewport()->hasFocus();
    if ( painter->device()->devType() == QInternal::Printer )
        focus = false;

    QColorGroup gb = QApplication::palette().active();
    if ( focus && m_currentFrameSetEdit && frameset == m_currentFrameSetEdit->frameSet() )
        // Currently edited frameset
        m_currentFrameSetEdit->drawContents( painter, crect, gb, onlyChanged, resetChanged, m_viewMode );
    else
        frameset->drawContents( painter, crect, gb, onlyChanged, resetChanged, 0L, m_viewMode );
}

void KWCanvas::keyPressEvent( QKeyEvent *e )
{
    if( !m_doc->isReadWrite()) {
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
    }
    // The key events in read-write mode are handled by eventFilter(), otherwise
    // we don't get <Tab> key presses.
}

void KWCanvas::switchViewMode( KWViewMode * newViewMode )
{
    delete m_viewMode;
    m_viewMode = newViewMode;
    refreshViewMode();
    emit updateRuler();
}

void KWCanvas::refreshViewMode()
{
    slotNewContentsSize();
    m_doc->updateResizeHandles( );
    repaintAll( true );
}

void KWCanvas::mpEditFrame( QMouseEvent *e, const QPoint &nPoint ) // mouse press in edit-frame mode
// This can be called by KWResizeHandle::mousePressEvent
{
    KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
    double x = docPoint.x();
    double y = docPoint.y();
    m_mousePressed = true;
    frameMoved = false;
    frameResized = false;
    m_ctrlClickOnSelectedFrame = false;

    if ( e )
    {
        KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
        double x = docPoint.x();
        double y = docPoint.y();

        // Find the frame we clicked upon (try by border, fallback on frame's internal rect)
        KWFrame * frame = m_doc->frameByBorder( nPoint );
        if ( !frame )
            frame = m_doc->frameAtPos( x, y );

        KWFrameSet *fs = frame ? frame->getFrameSet() : 0;
        KWTableFrameSet *table= fs ? fs->getGroupManager() : 0;

        if ( fs && ( e->state() & ShiftButton ) && table ) { // is table and we hold shift
            table->selectUntil( x,y );
        }
        else if ( frame && !frame->isSelected() ) // clicked on a frame that wasn't selected
        {
            if ( ! ( e->state() & ShiftButton || e->state() & ControlButton ) )
                selectAllFrames( FALSE );
            selectFrame( frame, TRUE );
        }
        else if(frame)  // clicked on a frame that was already selected
        {
            if ( e->state() & ControlButton )
                m_ctrlClickOnSelectedFrame = true;
            else
            {
                if ( e->state() & ShiftButton )
                    selectFrame( frame, FALSE );
                else if ( viewport()->cursor().shape() != SizeAllCursor ) {
                    selectAllFrames( FALSE );
                    selectFrame( frame, TRUE );
                }
            }
        }
        curTable = table;
        emit frameSelectedChanged();
    }

    // At least one frame selected ?
    if( m_doc->getFirstSelectedFrame() )
    {
        KWFrame * frame = m_doc->getFirstSelectedFrame();
        // If header/footer, resize the first frame
        if ( frame->getFrameSet()->isHeaderOrFooter() )
            frame = frame->getFrameSet()->getFrame( 0 );
        m_resizedFrameInitialSize = frame->normalize();
    }

    QList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
    QList<FrameIndex> frameindexList;
    QList<FrameResizeStruct> frameindexMove;
    KWFrame *frame=0L;
    // When moving many frames, we look at the bounding rect.
    // It's the one that will be checked against the limits, etc.
    m_boundingRect = KoRect();
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        KWFrameSet * fs = frame->getFrameSet();
        if ( !(m_doc->processingType() == KWDocument::WP && m_doc->getFrameSetNum( fs ) == 0 )&& !fs->isAHeader() && !fs->isAFooter()  )
        {
            // If one cell belongs to a table, we are in fact moving the whole table
            KWTableFrameSet *table = fs->getGroupManager();
            // We'll have to do better in the long run
            if ( table )
                m_boundingRect |= table->boundingRect();
            else
                m_boundingRect |= *frame;

            FrameIndex *index=new FrameIndex( frame );
            FrameResizeStruct *move=new FrameResizeStruct;

            move->sizeOfBegin=frame->normalize();
            move->sizeOfEnd=KoRect();
            frameindexList.append(index);
            frameindexMove.append(move);
        }

    }
    m_hotSpot = docPoint - m_boundingRect.topLeft();
    if(frameindexMove.count()!=0)
        cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"), frameindexList, frameindexMove );

    viewport()->setCursor( m_doc->getMouseCursor( nPoint, e && e->state() & ControlButton ) );

    deleteMovingRect = FALSE;
}

void KWCanvas::mpCreate( int mx, int my )
{
    mx = ( mx / m_doc->gridX() ) * m_doc->gridX();
    my = ( my / m_doc->gridX() ) * m_doc->gridY();
    double x = mx / m_doc->zoomedResolutionX();
    double y = my / m_doc->zoomedResolutionY();
    m_insRect.setCoords( x, y, 0, 0 );
    deleteMovingRect = FALSE;
}

void KWCanvas::mpCreatePixmap( int mx, int my )
{
    if ( !m_pictureFilename.isEmpty() )
    {
        // Apply grid for the first corner only
        mx = ( mx / m_doc->gridX() ) * m_doc->gridX();
        my = ( my / m_doc->gridX() ) * m_doc->gridY();
        double x = mx / m_doc->zoomedResolutionX();
        double y = my / m_doc->zoomedResolutionY();
        m_insRect.setCoords( x, y, 0, 0 );
        deleteMovingRect = false;

        if ( !m_isClipart )
        {
            QPixmap pix( m_pictureFilename );
            // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
            int width = qRound( (double)pix.width() * m_doc->zoomedResolutionX() / POINT_TO_INCH( QPaintDevice::x11AppDpiX() ) );
            int height = qRound( (double)pix.height() * m_doc->zoomedResolutionY() / POINT_TO_INCH( QPaintDevice::x11AppDpiY() ) );
            // Apply reasonable limits
            width = QMIN( width, m_doc->paperWidth() );
            height = QMIN( height, m_doc->paperHeight() );

            QPoint nPoint( mx + width, my + height );
            QPoint vPoint = m_viewMode->normalToView( nPoint );
            QCursor::setPos( viewport()->mapToGlobal( contentsToViewport( vPoint ) ) );
        }
    }
}

void KWCanvas::contentsMousePressEvent( QMouseEvent *e )
{
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );

    if ( e->button() == LeftButton )
	m_mousePressed = true;

    // Only edit-mode (and only LMB) allowed on read-only documents (to select text)
    if ( !m_doc->isReadWrite() && ( m_mouseMode != MM_EDIT || e->button() != LeftButton ) )
       return;

    // This code here is common to all mouse buttons, so that RMB and MMB place the cursor (or select the frame) too
    switch ( m_mouseMode ) {
        case MM_EDIT:
        {
            // See if we clicked on a frame's border
            KWFrame * frame = m_doc->frameByBorder( normalPoint );
            bool selectedFrame = m_doc->getFirstSelectedFrame() != 0L;
            // Frame border, or pressing Control or pressing Shift and a frame has already been selected
            // [We must keep shift+click for selecting text, when no frame is selected]
            if ( frame || e->state() & ControlButton ||
                 ( ( e->state() & ShiftButton ) && selectedFrame ) )
            {
                if ( m_currentFrameSetEdit )
                    terminateCurrentEdit();
                mpEditFrame( e, normalPoint );
            }
            else
            {
                if ( selectAllFrames( false ) )
                    emit frameSelectedChanged();

                frame = m_doc->frameAtPos( docPoint.x(), docPoint.y() );
                KWFrameSet * fs = frame ? frame->getFrameSet() : 0L;
                bool emitChanged = false;
                if ( fs )
                {
                    KWTableFrameSet *table = fs->getGroupManager();
                    emitChanged = checkCurrentEdit( table ? table : fs );
                }

                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mousePressEvent( e, normalPoint, docPoint );

                if ( emitChanged ) // emitted after mousePressEvent [for tables]
                    emit currentFrameSetEditChanged();
                 emit updateRuler();
            }
        }
        break;
        case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE:
        case MM_CREATE_FORMULA:
            if ( e->button() == LeftButton )
                mpCreate( normalPoint.x(), normalPoint.y() );
            break;
        case MM_CREATE_PIX:
            if ( e->button() == LeftButton )
                mpCreatePixmap( normalPoint.x(), normalPoint.y() );
            break;
        default: break;
    }

    if ( e->button() == MidButton ) {
        if ( m_doc->isReadWrite() && m_currentFrameSetEdit && m_mouseMode == MM_EDIT )
            m_currentFrameSetEdit->paste();
    }
    else if ( e->button() == RightButton ) {
        if(!m_doc->isReadWrite()) // The popups are not available in readonly mode, since the GUI isn't built...
            return;
        // rmb menu
        switch ( m_mouseMode )
        {
            case MM_EDIT:
            {
                // See if we clicked on a frame's border
                KWFrame * frame = m_doc->frameByBorder( normalPoint );
                if ( frame || e->state() & ControlButton )
                {
                    KWFrame *frame = m_doc->getFirstSelectedFrame();
                    // if a header/footer etc. Dont show the popup.
                    if((frame->getFrameSet() && frame->getFrameSet()->frameSetInfo() != KWFrameSet::FI_BODY))
                    {
                        m_mousePressed = false;
                        return;
                    }
                    m_gui->getView()->openPopupMenuEditFrame( QCursor::pos() );
                }
                else
                {
                    if (m_doc->frameAtPos( docPoint.x(), docPoint.y() ))
                        m_gui->getView()->openPopupMenuEditText( QCursor::pos() );
                    else
                        m_gui->getView()->openPopupMenuChangeAction( QCursor::pos() );
                }
            }
            break;
            case MM_CREATE_TEXT:
            case MM_CREATE_PART:
            case MM_CREATE_TABLE:
            case MM_CREATE_FORMULA:
            case MM_CREATE_PIX:
                setMouseMode( MM_EDIT );
            default: break;
        }
        m_mousePressed = false;
    }
}

// Called by KWTableDia
void KWCanvas::createTable( unsigned int rows, unsigned int cols,
                            int wid, int hei,
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
        m_insRect = KoRect( 0, 0, edit->frameSet()->getFrame(0)->width()-10, rows * 30 ); // mostly unused anyway
        KWTableFrameSet * table = createTable();
        m_doc->addFrameSet( table, false );
        edit->insertFloatingFrameSet( table, i18n("Insert Inline Table") );
        table->finalize();
        m_doc->updateAllFrames();
    }
    else
        setMouseMode( MM_CREATE_TABLE );
}

void KWCanvas::mmEditFrameResize( bool top, bool bottom, bool left, bool right, bool noGrid )
{
    //kdDebug() << "KWCanvas::mmEditFrameResize top,bottom,left,right: "
    //          << top << "," << bottom << "," << left << "," << right << endl;
    // This one is called by KWResizeHandle

    // Can't resize the main frame of a WP document
    KWFrame *frame = m_doc->getFirstSelectedFrame();

    KWFrameSet *fs = frame->getFrameSet();
    if ( m_doc->processingType() == KWDocument::WP && fs == m_doc->getFrameSet(0))
        return;

    // Get the mouse position from QCursor. Trying to get it from KWResizeHandle's
    // mouseMoveEvent leads to the frame 'jumping' because the events are received async.
    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
    mousep = m_viewMode->viewToNormal( mousep );

    int mx = mousep.x();
    int my = mousep.y();
    // Apply the grid, unless Shift is pressed
    if ( !noGrid )
    {
        int rastX = m_doc->gridX();
        int rastY = m_doc->gridY();
        mx = ( mx / rastX ) * rastX ;
        my = ( my / rastY ) * rastY;
    }
    double x = mx / m_doc->zoomedResolutionX();
    double y = my / m_doc->zoomedResolutionY();
    int page = static_cast<int>( y / m_doc->ptPaperHeight() );
    int oldPage = static_cast<int>( frame->top() / m_doc->ptPaperHeight() );

    // Calculate new frame coordinates, using minimum sizes, and keeping it in the bounds of the page
    double newLeft = frame->left();
    double newTop = frame->top();
    double newRight = frame->right();
    double newBottom = frame->bottom();
    if ( page == oldPage )
    {

        //kdDebug() << "KWCanvas::mmEditFrameResize old rect " << DEBUGRECT( *frame ) << endl;

        if ( top && newTop != y && !fs->isAHeader()/*!fs->isAFooter()*/ )
        {
            if (newBottom - y < minFrameHeight+5)
                y = newBottom - minFrameHeight - 5;
            y = QMAX( y, m_doc->ptPageTop( frame->pageNum() ) );
            newTop = y;
        } else if ( bottom && newBottom != y && !fs->isAFooter()/*!fs->isAHeader()*/ )
        {
            if (y - newTop < minFrameHeight+5)
                y = newTop + minFrameHeight + 5;
            y = QMIN( y, m_doc->ptPageTop( frame->pageNum() + 1 ) );
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
            x = QMIN( x, m_doc->ptPaperWidth() );
            newRight = x;
        }
    }
    // Keep copy of old rectangle, for repaint()
    QRect oldRect = m_viewMode->normalToView( frame->outerRect() );

    frame->setLeft(newLeft);
    frame->setTop(newTop);
    frame->setRight(newRight);
    frame->setBottom(newBottom);

    // If header/footer, resize the first frame
    if ( frame->getFrameSet()->isHeaderOrFooter() )
    {
        KWFrame * origFrame = frame->getFrameSet()->getFrame( 0 );
        origFrame->setLeft(newLeft);
        origFrame->setTop(newTop);
        origFrame->setRight(newRight);
        origFrame->setBottom(newBottom);
    }

    //kdDebug() << "KWCanvas::mmEditFrameResize new rect " << DEBUGRECT( *frame ) << endl;

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
    QRect newRect( m_viewMode->normalToView( frame->outerRect() ) );
    // Repaing only the changed rects (oldRect U newRect)
    repaintContents( QRegion(oldRect).unite(newRect).boundingRect() );
    frameResized = true;

    m_gui->getView()->updateFrameStatusBarItem();
}

void KWCanvas::mmEditFrameMove( int mx, int my )
{
    bool adjustPosNeeded = false;
    double cx = mx / m_doc->zoomedResolutionX();
    double cy = my / m_doc->zoomedResolutionY();
    // Move the bounding rect containing all the selected frames
    KoRect oldBoundingRect = m_boundingRect;
    //int page = m_doc->getPageOfRect( m_boundingRect );
    //kdDebug() << "KWCanvas::mmEditFrameMove cx=" << cx
    //          << "  boundingrect=" << DEBUGRECT(m_boundingRect) << endl;

    // (x and y separately for a better behaviour at limit of page)
    KoPoint p( m_boundingRect.topLeft() );
    //kdDebug() << "KWCanvas::mmEditFrameMove hotspot.x=" << m_hotSpot.x() << " cx=" << cx << endl;
    p.setX( cx - m_hotSpot.x() );
    //kdDebug() << "KWCanvas::mmEditFrameMove p.x is now " << p.x() << endl;
    m_boundingRect.moveTopLeft( p );
    //kdDebug() << "KWCanvas::mmEditFrameMove boundingrect now " << DEBUGRECT(m_boundingRect) << endl;
    // But not out of the margins
    if ( m_boundingRect.left() < 1 ) // 1 pt margin to avoid drawing problems
    {
        p.setX( 1 );
        m_boundingRect.moveTopLeft( p );
        adjustPosNeeded = true;
    }
    else if ( m_boundingRect.right() > m_doc->ptPaperWidth() - 1 )
    {
        p.setX( m_doc->ptPaperWidth() - m_boundingRect.width() - 2 );
        m_boundingRect.moveTopLeft( p );
        adjustPosNeeded = true;
    }
    // Now try Y
    p = m_boundingRect.topLeft();
    p.setY( cy - m_hotSpot.y() );
    m_boundingRect.moveTopLeft( p );
    // -- Don't limit to the current page. Let the user move a frame between pages --
    // But we still want to limit to 0 - lastPage
    if ( m_boundingRect.top() < 1 ) // 1 pt margin to avoid drawing problems
    {
        p.setY( 1 );
        m_boundingRect.moveTopLeft( p );
        adjustPosNeeded = true;
    }
    else if ( m_boundingRect.bottom() > m_doc->getPages() * m_doc->ptPaperHeight() - 1 )
    {
        kdDebug() << "KWCanvas::mmEditFrameMove limiting to last page" << endl;
        p.setY( m_doc->getPages() * m_doc->ptPaperHeight() - m_boundingRect.height() - 2 );
        m_boundingRect.moveTopLeft( p );
        adjustPosNeeded = true;
    }
    // Another annoying case is if the top and bottom points are not in the same page....
    int topPage = static_cast<int>( m_boundingRect.top() / m_doc->ptPaperHeight() );
    int bottomPage = static_cast<int>( m_boundingRect.bottom() / m_doc->ptPaperHeight() );
    //kdDebug() << "KWCanvas::mmEditFrameMove topPage=" << topPage << " bottomPage=" << bottomPage << endl;
    if ( topPage != bottomPage )
    {
        // Choose the closest page...
        ASSERT( topPage + 1 == bottomPage ); // Not too sure what to do otherwise
        double topPart = (bottomPage * m_doc->ptPaperHeight()) - m_boundingRect.top();
        if ( topPart > m_boundingRect.height() / 2 )
            // Most of the rect is in the top page
            p.setY( bottomPage * m_doc->ptPaperHeight() - m_boundingRect.height() - 1 );
        else
            // Moost of the rect is in the bottom page
            p.setY( bottomPage * m_doc->ptPaperHeight() + 5 /* grmbl, resize handles.... */ );
        kdDebug() << "KWCanvas::mmEditFrameMove y set to " << p.y() << endl;

        m_boundingRect.moveTopLeft( p );
    }

    /*kdDebug() << "boundingRect moved by " << m_boundingRect.left() - oldBoundingRect.left() << ","
              << m_boundingRect.top() - oldBoundingRect.top() << endl;
    kdDebug() << " boundingX+hotspotX=" << m_boundingRect.left() + m_hotSpot.x() << endl;
    kdDebug() << " cx=" << cx << endl;*/

    QList<KWTableFrameSet> tablesMoved;
    tablesMoved.setAutoDelete( FALSE );
    bool bFirst = true;
    QRegion repaintRegion;
    KoPoint _move=m_boundingRect.topLeft() - oldBoundingRect.topLeft();
    QListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    for ( ; framesetIt.current(); ++framesetIt, bFirst=false )
    {
        KWFrameSet *frameset = framesetIt.current();
        // Can't move main frameset of a WP document
        if ( m_doc->processingType() == KWDocument::WP && bFirst ||
             frameset->type() == FT_TEXT && frameset->frameSetInfo() != KWFrameSet::FI_BODY )
            continue;
        // Can't move frame of floating frameset
        if ( frameset->isFloating() ) continue;

        frameMoved = true;
        QListIterator<KWFrame> frameIt( frameset->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame *frame = frameIt.current();
            if ( frame->isSelected() ) {
                if ( frameset->type() == FT_TABLE ) {
                    if ( tablesMoved.findRef( static_cast<KWTableFrameSet *> (frameset) ) == -1 )
                        tablesMoved.append( static_cast<KWTableFrameSet *> (frameset));
                } else {
                    QRect oldRect( m_viewMode->normalToView( frame->outerRect() ) );
                    // Move the frame
                    frame->moveTopLeft( frame->topLeft() + _move );
                    // Calculate new rectangle for this frame
                    QRect newRect( frame->outerRect() );

                    QRect frameRect( m_viewMode->normalToView( newRect ) );
                    /*ensureVisible( (frameRect.left()+frameRect.right()) / 2,  // point = center of the rect
                                   (frameRect.top()+frameRect.bottom()) / 2,
                                   (frameRect.right()-frameRect.left()) / 2,  // margin = half-width of the rect
                                   (frameRect.bottom()-frameRect.top()) / 2);*/
                    // This ensureVisible doesn't work as it should.
                    // With frames bigger than the viewport, we end scrolling up/down like nonsense.

                    // Repaint only the changed rects (oldRect U newRect)
                    repaintRegion += QRegion(oldRect).unite(frameRect).boundingRect();
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
                QRect oldRect( m_viewMode->normalToView( frame->outerRect() ) );
                frame->moveTopLeft( frame->topLeft() + _move );
                // Calculate new rectangle for this frame
                QRect newRect( frame->outerRect() );
                QRect frameRect( m_viewMode->normalToView( newRect ) );
                // Repaing only the changed rects (oldRect U newRect)
                repaintRegion += QRegion(oldRect).unite(frameRect).boundingRect();
                // Move resize handles to new position
                frame->updateResizeHandles();
            }
        }
    }

    // Frames have moved -> update the "frames on top" lists
    m_doc->updateAllFrames();
    repaintContents( repaintRegion.boundingRect() );

    m_gui->getView()->updateFrameStatusBarItem();

    // Doesn't work ! It makes the cursor jump.
    // I have tried every combination of contentsToViewport and viewport()->mapToGlobal etc., no luck
    /*if ( adjustPosNeeded )
    {
        QPoint pos = mapToGlobal( m_doc->zoomPoint( m_boundingRect.topLeft() + m_hotSpot ) );
        kdDebug() << "ADJUSTING ptcoordX=" << m_boundingRect.left()+m_hotSpot.x()
                  << " globalCoordX=" << pos.x() << " currentGlobalX=" << QCursor::pos().x() << endl;
        QCursor::setPos( pos );
    }*/
}

void KWCanvas::mmCreate( int mx, int my ) // Mouse move when creating a frame
{
    if ( m_mouseMode != MM_CREATE_PIX )
    {
        mx = ( mx / m_doc->gridX() ) * m_doc->gridX();
        my = ( my / m_doc->gridY() ) * m_doc->gridY();
    }

    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );

    if ( deleteMovingRect )
        drawMovingRect( p );

    int page = m_doc->getPageOfRect( m_insRect );
    KoRect oldRect = m_insRect;

    // Resize the rectangle
    m_insRect.setRight( mx / m_doc->zoomedResolutionX() );
    m_insRect.setBottom( my / m_doc->zoomedResolutionY() );

    // But not out of the page
    KoRect r = m_insRect.normalize();
    if ( m_doc->isOutOfPage( r, page ) )
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
    p.drawRect( m_viewMode->normalToView( m_doc->zoomRect( m_insRect ) ) );
}

void KWCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    if ( m_mousePressed ) {

	doAutoScroll();

        switch ( m_mouseMode ) {
            case MM_EDIT:
            {
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseMoveEvent( e, normalPoint, docPoint );
                else
                {
                    if ( viewport()->cursor().shape() == SizeAllCursor )
                    {
                        int mx = normalPoint.x();
                        int my = normalPoint.y();
                        if ( !( e->state() & ShiftButton ) ) // Shift disables the grid
                        {
                            mx = ( mx / m_doc->gridX() ) * m_doc->gridX();
                            my = ( my / m_doc->gridY() ) * m_doc->gridY();
                        }
                        mmEditFrameMove( mx, my );
                    }
                }
            } break;
            case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
            case MM_CREATE_TABLE: case MM_CREATE_FORMULA:
                mmCreate( normalPoint.x(), normalPoint.y() );
            default: break;
        }
    } else {
        if ( m_mouseMode == MM_EDIT )
            viewport()->setCursor( m_doc->getMouseCursor( normalPoint, e->state() & ControlButton ) );
    }
}

void KWCanvas::mrEditFrame( QMouseEvent *e, const QPoint &nPoint ) // Can be called from KWCanvas and from KWResizeHandle's mouseReleaseEvents
{
    //kdDebug() << "KWCanvas::mrEditFrame" << endl;
    KWFrame *firstFrame = m_doc->getFirstSelectedFrame();
    kdDebug() << "KWCanvas::mrEditFrame frameMoved=" << frameMoved << " frameResized=" << frameResized << endl;
    if ( firstFrame && ( frameMoved || frameResized ) )
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
            KWFrame *frame = m_doc->getFirstSelectedFrame();
            // If header/footer, resize the first frame
            if ( frame->getFrameSet()->isHeaderOrFooter() )
                frame = frame->getFrameSet()->getFrame( 0 );
            ASSERT( frame );
            if ( frame )
            {
                FrameIndex index( frame );
                FrameResizeStruct tmpResize;
                tmpResize.sizeOfBegin = m_resizedFrameInitialSize;
                tmpResize.sizeOfEnd = frame->normalize();

                KWFrameResizeCommand *cmd = new KWFrameResizeCommand( i18n("Resize Frame"), index, tmpResize );
                m_doc->addCommand(cmd);

                m_doc->frameChanged( frame, m_gui->getView() ); // repaint etc.
                if(frame->getFrameSet()->isAHeader() || frame->getFrameSet()->isAFooter())
                {
                    m_doc->recalcFrames();
                    frame->updateResizeHandles();
                }
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
                QList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
                int i = 0;
                for(KWFrame * frame=selectedFrames.first(); frame; frame=selectedFrames.next() )
                {
                    KWFrameSet * fs = frame->getFrameSet();
                    if ( !(m_doc->processingType() == KWDocument::WP && m_doc->getFrameSetNum( fs ) == 0 )&& !fs->isAHeader() && !fs->isAFooter()  )
                    {

                        cmdMoveFrame->listFrameMoved().at(i)->sizeOfEnd = frame->normalize();
                        i++;
                    }
                }
                m_doc->addCommand(cmdMoveFrame);
                m_doc->framesChanged( selectedFrames, m_gui->getView() ); // repaint etc.

                cmdMoveFrame = 0L;
            }
        }
        m_doc->repaintAllViews();
    }
    else
    {
        // No frame was moved or resized.
        // If CTRL+click on selected frame, unselect it
        if ( e->state() & ControlButton )
        {
            KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
            // Find the frame we clicked upon (try by border, fallback on frame's internal rect)
            KWFrame * frame = m_doc->frameByBorder( nPoint );
            if ( !frame )
                frame = m_doc->frameAtPos( docPoint.x(), docPoint.y() );
            if ( m_ctrlClickOnSelectedFrame && frame->isSelected() )
            {
                selectFrame( frame, false );
                emit frameSelectedChanged();
            }
        }
    }
    m_mousePressed = false;
    m_ctrlClickOnSelectedFrame = false;
}

void KWCanvas::mrCreateText()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) {
        KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        KWFrameDia frameDia( this, frame, m_doc, FT_TEXT );
        frameDia.setCaption(i18n("Connect frame"));
        frameDia.show();
    }
    setMouseMode( MM_EDIT );
    m_doc->repaintAllViews();
    emit docStructChanged(TextFrames);
}

void KWCanvas::mrCreatePixmap()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() && !m_pictureFilename.isEmpty() )
    {
        KWFrameSet * fs = 0L;
        if ( m_isClipart )
        {
            KWClipartFrameSet *frameset = new KWClipartFrameSet( m_doc, QString::null /*automatic name*/ );
            frameset->loadClipart( m_pictureFilename );
            fs = frameset;
        }
        else
        {
            KWPictureFrameSet *frameset = new KWPictureFrameSet( m_doc, QString::null /*automatic name*/ );
            frameset->loadImage( m_pictureFilename, m_doc->zoomRect( m_insRect ).size() );
            fs = frameset;
        }
        m_insRect = m_insRect.normalize();
        KWFrame *frame = new KWFrame(fs, m_insRect.x(), m_insRect.y(), m_insRect.width(),
                                     m_insRect.height() );
        fs->addFrame( frame, false );
        m_doc->addFrameSet( fs );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a picture frame"), frame );
        m_doc->addCommand(cmd);
        m_doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Pictures);
}

void KWCanvas::mrCreatePart() // mouse release, when creating part
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) {
        m_doc->insertObject( m_insRect, m_partEntry );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Embedded);
}

void KWCanvas::mrCreateFormula()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( m_doc, QString::null );
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frameset->addFrame( frame, false );
        m_doc->addFrameSet( frameset );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a formula frame"), frame );
        m_doc->addCommand(cmd);
        m_doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(FormulaFrames);
}

void KWCanvas::mrCreateTable()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) {
        if ( m_table.cols * minFrameWidth + m_insRect.x() > m_doc->ptPaperWidth() )
        {
            KMessageBox::sorry(0, i18n("KWord is unable to insert the table because there\n"
                                       "is not enough space available."));
        }
        else {
            KWTableFrameSet * table = createTable();
            KWCreateTableCommand *cmd=new KWCreateTableCommand( i18n("Create table"), table );
            m_doc->addCommand(cmd);
            cmd->execute();
            emit docStructChanged(Tables);
        }
        m_doc->updateAllFrames();
        m_doc->layout();
        repaintAll();

    }
    setMouseMode( MM_EDIT );
}

KWTableFrameSet * KWCanvas::createTable() // uses m_insRect and m_table to create the table
{
    KWTableFrameSet *table = new KWTableFrameSet( m_doc, QString::null /*automatic name*/ );

    // Create a set of cells with random-size frames.
    for ( unsigned int i = 0; i < m_table.rows; i++ ) {
        for ( unsigned int j = 0; j < m_table.cols; j++ ) {
            KWTableFrameSet::Cell *cell = new KWTableFrameSet::Cell( table, i, j, QString::null /*automatic name*/ );
            KWFrame *frame = new KWFrame(cell, 0, 0, 0, 0, KWFrame::RA_NO ); // pos and size will be set in setBoundingRect
            cell->addFrame( frame, false );
            frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
            frame->setNewFrameBehaviour(KWFrame::NoFollowup);
        }
    }
    table->setHeightMode( static_cast<KWTableFrameSet::CellSize>( m_table.height ) );
    table->setWidthMode( static_cast<KWTableFrameSet::CellSize>( m_table.width ) );
    table->setBoundingRect( m_insRect );
    return table;
}

void KWCanvas::contentsMouseReleaseEvent( QMouseEvent * e )
{
    if ( scrollTimer->isActive() )
	scrollTimer->stop();
    if ( m_mousePressed ) {
        if ( m_mouseMode == MM_CREATE_TEXT || m_mouseMode == MM_CREATE_PIX ||
             m_mouseMode == MM_CREATE_FORMULA || m_mouseMode == MM_CREATE_TABLE ||
             m_mouseMode == MM_CREATE_PART )
        {
            if ( deleteMovingRect )
            {
                QPainter p;
                p.begin( viewport() );
                p.translate( -contentsX(), -contentsY() );
                p.setRasterOp( NotROP );
                p.setPen( black );
                p.setBrush( NoBrush );
                drawMovingRect( p );
                deleteMovingRect = false;
            }
        }

        QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        switch ( m_mouseMode ) {
            case MM_EDIT:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseReleaseEvent( e, normalPoint, docPoint );
                else
                    mrEditFrame( e, normalPoint );
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

	m_mousePressed = false;
    }
}

void KWCanvas::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    switch ( m_mouseMode ) {
        case MM_EDIT:
            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->mouseDoubleClickEvent( e, normalPoint, docPoint );
            break;
        default:
            break;
    }

    m_mousePressed = true; // needed for the dbl-click + move feature.
}

void KWCanvas::setLeftFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
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
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getLeftBorder();
        tmp->m_EFrameType= FBLeft;

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
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Left Border frame"),frameindexList,tmpBorderList,_frmBrd);
        m_doc->addCommand(cmd);
        m_doc->repaintAllViews();
    }
}

void KWCanvas::setRightFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
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
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getRightBorder();
        tmp->m_EFrameType= FBRight;

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
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Right Border frame"),frameindexList,tmpBorderList,_frmBrd);
        m_doc->addCommand(cmd);
        m_doc->repaintAllViews();
    }
}

void KWCanvas::setTopFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
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
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getTopBorder();
        tmp->m_EFrameType= FBTop;

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
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Top Border frame"),frameindexList,tmpBorderList,_frmBrd);
        m_doc->addCommand(cmd);
        m_doc->repaintAllViews();
    }
}

void KWCanvas::setBottomFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
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
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->getBottomBorder();
        tmp->m_EFrameType= FBBottom;

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
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Bottom Border frame"),frameindexList,tmpBorderList,_frmBrd);
        m_doc->addCommand(cmd);
        m_doc->repaintAllViews();
    }
}

void KWCanvas::setOutlineFrameBorder( Border _frmBrd, bool _b )
{
    QList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
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
    //int m_IindexFrameSet;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        frame=KWFrameSet::settingsFrame(frame);
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
    KWFrameBorderCommand *cmd=new KWFrameBorderCommand(QString::null,frameindexListBottom,tmpBorderListBottom,_frmBrd);
    macroCmd->addCommand(cmd);

    cmd=new KWFrameBorderCommand(QString::null,frameindexListLeft,tmpBorderListLeft,_frmBrd);
    macroCmd->addCommand(cmd);

    cmd=new KWFrameBorderCommand(QString::null,frameindexListTop,tmpBorderListTop,_frmBrd);
    macroCmd->addCommand(cmd);

    cmd=new KWFrameBorderCommand(QString::null,frameindexListRight,tmpBorderListRight,_frmBrd);
    macroCmd->addCommand(cmd);

    m_doc->addCommand(macroCmd);
    m_doc->repaintAllViews();
}

void KWCanvas::setFrameBackgroundColor( const QBrush &_backColor )
{
    QList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return;
    bool colorChanged=false;
    KWFrame *frame=0L;
    QList<FrameIndex> frameindexList;
    QList<QBrush> oldColor;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        frame=KWFrameSet::settingsFrame(frame);

        FrameIndex *index=new FrameIndex( frame );
        frameindexList.append(index);

        QBrush *_color=new QBrush(frame->getBackgroundColor());
        oldColor.append(_color);

        if (_backColor!=frame->getBackgroundColor())
        {
            colorChanged=true;
            frame->setBackgroundColor(_backColor);
        }
    }
    if(colorChanged)
    {
        KWFrameBackGroundColorCommand *cmd=new KWFrameBackGroundColorCommand(i18n("Change Frame BackGroundColor"),frameindexList,oldColor,_backColor);
        m_doc->addCommand(cmd);
        m_doc->repaintAllViews();
    }
}


void KWCanvas::editFrameProperties()
{
    KWFrame *frame = m_doc->getFirstSelectedFrame();
    if (!frame)
        return;
    KWFrameSet *fs = frame->getFrameSet();
    if ( fs->isAHeader() )
    {
        KMessageBox::sorry( this, i18n( "This is a header frame. It can not be edited."), i18n( "Frame Properties"  ) );
        return;
    }
    if ( fs->isAFooter() )
    {
        KMessageBox::sorry( this, i18n( "This is a footer frame. It can not be edited."),i18n( "Frame Properties"  ) );
        return;
    }

    KWFrameDia * frameDia = new KWFrameDia( this, frame);
    frameDia->setCaption(i18n("Frame Properties"));
    frameDia->show();
    delete frameDia;
}

bool KWCanvas::selectAllFrames( bool select )
{
    bool ret = false;
    QListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        QListIterator<KWFrame> frameIt = fs->frameIterator();
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            if ( frame->isSelected() != select )
            {
                frame->setSelected( select );
                ret = true;
            }
        }
    }
    return ret;
}

void KWCanvas::selectFrame( KWFrame * frame, bool select )
{
    if ( frame->isSelected() != select )
        frame->setSelected( select );
}

void KWCanvas::copySelectedFrames()
{
    QDomDocument domDoc( "SELECTION" );
    QDomElement topElem = domDoc.createElement( "SELECTION" );
    domDoc.appendChild( topElem );
    bool foundOne = false;

    // We really need a selected-frames-list !
    QListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        QListIterator<KWFrame> frameIt = fs->frameIterator();
        KWFrame * firstFrame = frameIt.current();
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            if ( frame->isSelected() )
            {
                // Two cases to be distinguished here
                // If it's the first frame of a frameset, then copy the frameset (with that frame)
                // Otherwise copy only the frame information
                QDomElement parentElem = topElem;
                if ( frame == firstFrame )
                {
                    fs->save( parentElem, false );
                    // Get the last FRAMESET element, the one we just added
                    QDomNodeList listFramesets = parentElem.elementsByTagName( "FRAMESET" );
                    // Save the frame inside the frameset tag
                    parentElem = listFramesets.item( listFramesets.count() - 1 ).toElement();
                }
                // Save the frame information
                QDomElement frameElem = parentElem.ownerDocument().createElement( "FRAME" );
                parentElem.appendChild( frameElem );
                frame->save( frameElem );
                if ( frame != firstFrame )
                {
                    // Frame saved alone -> remember which frameset it's part of
                    frameElem.setAttribute( "parentFrameset", fs->getName() );
                }
                foundOne = true;
            }
        }
    }

    if ( !foundOne )
        return;

    KWDrag *kd = new KWDrag( 0L );
    kd->setKWord( domDoc.toCString() );
    kdDebug(32001) << "KWCanvas::copySelectedFrames: " << domDoc.toCString() << endl;
    QApplication::clipboard()->setData( kd );
}

void KWCanvas::pasteFrames()
{
    QMimeSource *data = QApplication::clipboard()->data();
    QByteArray arr = data->encodedData( KWDrag::selectionMimeType() );
    if ( !arr.size() )
        return;
    QDomDocument domDoc;
    domDoc.setContent( QCString( arr ) );
    QDomElement topElem = domDoc.documentElement();

    KMacroCommand * macroCmd = new KMacroCommand( i18n( "Paste Frames" ) );
    m_doc->addCommand( macroCmd );

    int ref=0;

    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
        QDomElement frameElem;
        KWFrameSet * fs = 0L;
        if ( elem.tagName() == "FRAME" )
        {
            QString frameSetName = frameElem.attribute( "parentFrameset" );
            fs = m_doc->getFrameSetByName( frameSetName );
            if ( !fs )
            {
                kdWarning(32001) << "pastFrames: Frameset '" << frameSetName << "' not found" << endl;
                continue;
            }
            frameElem = elem;
        }
        else if ( elem.tagName() == "FRAMESET" )
        {
            fs = m_doc->loadFrameSet( elem, false );
            frameElem = elem.namedItem( "FRAME" ).toElement();
        }
        else
            kdWarning(32001) << "Unsupported toplevel-element in KWCanvas::pasteFrames : '" << elem.tagName() << "'" << endl;

        if ( fs && !frameElem.isNull() )
        {
            double offs = 20.0;
            KoRect rect;
            rect.setLeft( KWDocument::getAttribute( frameElem, "left", 0.0 ) + offs );
            rect.setTop( KWDocument::getAttribute( frameElem, "top", 0.0 ) + offs );
            rect.setRight( KWDocument::getAttribute( frameElem, "right", 0.0 ) + offs );
            rect.setBottom( KWDocument::getAttribute( frameElem, "bottom", 0.0 ) + offs );
            KWFrame * frame = new KWFrame( fs, rect.x(), rect.y(), rect.width(), rect.height() );
            frame->load( frameElem, fs->isHeaderOrFooter(), KWDocument::CURRENT_SYNTAX_VERSION );
            QString newName=i18n("Copy-%1").arg(fs->getName());
            fs->setName(m_doc->generateFramesetName( (newName+" %1")));
            fs->addFrame( frame );

            KWCreateFrameCommand *cmd = new KWCreateFrameCommand( QString::null, frame );
            macroCmd->addCommand(cmd);

            fs->finalize();
            int type=0;
            switch(fs->type())
            {
                case FT_TEXT:
                    type=(int)TextFrames;
                    break;
                case FT_PICTURE:
                    type=(int)Pictures;
                    break;
                case FT_PART:
                    type=(int)Embedded;
                    break;
                case FT_FORMULA:
                    type=(int)FormulaFrames;
                    break;
                case FT_TABLE:
                    type=(int)Tables;
                    break;
                default:
                    type=(int)TextFrames;
            }
            ref|=type;
        }
    }
    m_doc->processImageRequests();
    m_doc->repaintAllViews();
    m_doc->refreshDocStructure(ref);
}

KWTableFrameSet *KWCanvas::getTable()
{
    if( !m_currentFrameSetEdit)
        return 0L;

    if(m_currentFrameSetEdit->frameSet()->type() == FT_TABLE)
        return static_cast<KWTableFrameSet *> (m_currentFrameSetEdit->frameSet());

    return 0L;
}

bool KWCanvas::checkCurrentEdit( KWFrameSet * fs )
{
    bool emitChanged = false;
    if ( fs && m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() != fs )
    {
        // Don't use terminateCurrentEdit here, we want to emit changed only once
        m_currentFrameSetEdit->terminate();
        delete m_currentFrameSetEdit;
        m_currentFrameSetEdit = 0L;
        emitChanged = true;
    }

    // Edit the frameset under the mouse, if any
    if ( fs && !m_currentFrameSetEdit )
    {
        m_currentFrameSetEdit = fs->createFrameSetEdit( this );
        emitChanged = true;
    }
    return emitChanged;
}

void KWCanvas::terminateCurrentEdit()
{
    m_currentFrameSetEdit->terminate();
    delete m_currentFrameSetEdit;
    m_currentFrameSetEdit = 0L;
    emit currentFrameSetEditChanged();
    repaintAll();
}

void KWCanvas::terminateEditing( KWFrameSet *fs )
{
    if ( m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() == fs )
        terminateCurrentEdit();
    // Also deselect the frames from this frameset
    QListIterator<KWFrame> frameIt = fs->frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->isSelected() )
            frameIt.current()->setSelected( false );
}

void KWCanvas::setMouseMode( MouseMode newMouseMode )
{
    if ( m_mouseMode != newMouseMode )
    {
        if ( selectAllFrames( false ) )
            emit frameSelectedChanged();

        if ( newMouseMode != MM_EDIT )
        {
            // Terminate edition of current frameset
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();
        }
    }

    m_mouseMode = newMouseMode;
    emit currentMouseModeChanged(m_mouseMode);

    switch ( m_mouseMode ) {
    case MM_EDIT: {
        QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
        QPoint normalPoint = m_viewMode->viewToNormal( mousep );
        viewport()->setCursor( m_doc->getMouseCursor( normalPoint, false /*....*/ ) );
    } break;
    case MM_CREATE_TEXT:
    case MM_CREATE_PIX:
    case MM_CREATE_TABLE:
    case MM_CREATE_FORMULA:
    case MM_CREATE_PART:
        viewport()->setCursor( crossCursor );
        break;
    }
}

void KWCanvas::insertPicture( const QString & filename, bool isClipart )
{
    setMouseMode( MM_CREATE_PIX );
    m_pictureFilename = filename;
    m_isClipart = isClipart;
}

void KWCanvas::contentsDragEnterEvent( QDragEnterEvent *e )
{
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->dragEnterEvent( e );
}

void KWCanvas::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( m_currentFrameSetEdit )
    {
        QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        m_currentFrameSetEdit->dragMoveEvent( e, normalPoint, docPoint );
    }
}

void KWCanvas::contentsDragLeaveEvent( QDragLeaveEvent *e )
{
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->dragLeaveEvent( e );
}

void KWCanvas::contentsDropEvent( QDropEvent *e )
{
    if ( m_currentFrameSetEdit )
    {
        QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        m_currentFrameSetEdit->dropEvent( e, normalPoint, docPoint );
    }
    m_mousePressed = false;
}

void KWCanvas::doAutoScroll()
{
    if ( !m_mousePressed || !m_currentFrameSetEdit )
	return;

    QPoint pos( mapFromGlobal( QCursor::pos() ) );
    pos = m_viewMode->viewToNormal( viewportToContents( pos ) );

    m_currentFrameSetEdit->doAutoScroll( pos );

    if ( !scrollTimer->isActive() && pos.y() < 0 || pos.y() > height() )
	scrollTimer->start( 100, FALSE );
    else if ( scrollTimer->isActive() && pos.y() >= 0 && pos.y() <= height() )
	scrollTimer->stop();
}

void KWCanvas::slotContentsMoving( int cx, int cy )
{
    //QPoint nPointTop = m_viewMode->viewToNormal( QPoint( cx, cy ) );
    QPoint nPointBottom = m_viewMode->viewToNormal( QPoint( cx + visibleWidth(), cy + visibleHeight() ) );
    //kdDebug() << "KWCanvas::slotContentsMoving cx=" << cx << " cy=" << cy
    //          << " visibleWidth()=" << visibleWidth() << " visibleHeight()=" << visibleHeight() << endl;
    // Update our "formatted paragraphs needs" in the text framesets
    QListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>(fit.current());
        if ( fs )
        {
            fs->updateViewArea( this, nPointBottom );
        }
    }
    QPoint rulerTopLeft = rulerPos( cx, cy );
    m_gui->getHorzRuler()->setOffset( rulerTopLeft.x(), 0 );
    m_gui->getVertRuler()->setOffset( 0, rulerTopLeft.y() );
}

void KWCanvas::slotNewContentsSize()
{
    QSize size = m_viewMode->contentsSize();
    if ( size != QSize( contentsWidth(), contentsHeight() ) )
    {
        kdDebug() << "KWCanvas::slotNewContentsSize " << size.width() << "x" << size.height() << endl;
        resizeContents( size.width(), size.height() );
    }
}

void KWCanvas::resizeEvent( QResizeEvent *e )
{
    slotContentsMoving( contentsX(), contentsY() );
    QScrollView::resizeEvent( e );
}

void KWCanvas::scrollToOffset( const KoPoint & d )
{
    kdDebug() << "KWCanvas::scrollToOffset " << d.x() << "," << d.y() << endl;
#if 0
    bool blinking = blinkTimer.isActive();
    if ( blinking )
        stopBlinkCursor();
#endif
    QPoint nPoint = m_doc->zoomPoint( d );
    QPoint cPoint = m_viewMode->normalToView( nPoint );
    setContentsPos( cPoint.x(), cPoint.y() );

#if 0
    if ( blinking )
        startBlinkCursor();
#endif
}

QPoint KWCanvas::rulerPos(int x, int y)
{
    int pageNum=1;
    if( m_currentFrameSetEdit && m_currentFrameSetEdit->currentFrame() )
        pageNum = m_currentFrameSetEdit->currentFrame()->pageNum() + 1;
    QPoint nPoint( 0, m_doc->pageTop(pageNum - 1) + 1 );
    QPoint cPoint( m_viewMode->normalToView( nPoint ) );
    QPoint p( (x==-1 ? contentsX() : x) - cPoint.x(),
              (y==-1 ? contentsY() : y) - cPoint.y() );
    //kdDebug() << "KWCanvas::rulerPos cPoint=" << cPoint.x() << "," << cPoint.y() << endl;
    //kdDebug() << "KWCanvas::rulerPos p=" << p.x() << "," << p.y() << endl;
    return p;
}

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
                m_mousePressed = false;
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
                            m_doc->printDebug();
                            kdDebug(32002) << "Current framesetedit: " << m_currentFrameSetEdit <<
                                ( m_currentFrameSetEdit ? m_currentFrameSetEdit->frameSet()->className() : "" ) << endl;
                            break;
                        default:
                            break;
                    };
                    // For some reason 'T' doesn't work (maybe kxkb)
                }
#endif
                if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT && m_doc->isReadWrite() )
                {
                    m_currentFrameSetEdit->keyPressEvent( keyev );
                    return TRUE;
                }

                // Because of the dependency on the control key, we need to update the mouse cursor here
                if ( keyev->key() == Key_Control )
                {
                    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                    QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                    viewport()->setCursor( m_doc->getMouseCursor( normalPoint, true ) );
                }
                else if ( (keyev->key() == Key_Delete ||keyev->key() ==Key_Backspace ) && m_doc->getFirstSelectedFrame() )
                {
                    m_gui->getView()->editDeleteFrame();
                }
            } break;
            case QEvent::KeyRelease:
            {
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
                if ( keyev->key() == Key_Control )
                {
                    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                    QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                    viewport()->setCursor( m_doc->getMouseCursor( normalPoint, false ) );
                }
            }
            break;
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
        edit->updateUI( true, true );
}


void KWCanvas::emitFrameSelectedChanged()
{
    emit frameSelectedChanged();
}

#ifndef NDEBUG
void KWCanvas::printRTDebug( int info )
{
    KWTextFrameSet * textfs = 0L;
    if ( m_currentFrameSetEdit )
        textfs = dynamic_cast<KWTextFrameSet *>(m_currentFrameSetEdit->frameSet());
    if ( !textfs )
        textfs = dynamic_cast<KWTextFrameSet *>(m_doc->getFrameSet( 0 ));
    if ( textfs )
        textfs->printRTDebug( info );
}
#endif

#include "kwcanvas.moc"
