/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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


#include "KWCanvas.h"
#include "KWTableFrameSet.h"
#include "KWPartFrameSet.h"
#include "KWFormulaFrameSet.h"
#include "KWDocument.h"
#include "KWView.h"
#include "KWViewMode.h"
#include "KWFrameDia.h"
#include "KWCommand.h"
#include "KWTableTemplate.h"
#include "KWTextDocument.h"
#include "KWFrameList.h"
#include "KWPageManager.h"
#include "KWPage.h"

#include <qbuffer.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qobjectlist.h>
#include <qwhatsthis.h>

#include <koStore.h>
#include <koStoreDrag.h>
#include <koPictureCollection.h>

#include <ktempfile.h>
#include <klocale.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kmultipledrag.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kio/netaccess.h>
#include <kmimetype.h>

#include <assert.h>

KWCanvas::KWCanvas(KWViewMode* viewMode, QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent, "canvas", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase ), m_doc( d )
{
    m_gui = lGui;
    m_currentFrameSetEdit = 0L;
    m_mouseMeaning = MEANING_NONE;
    m_mousePressed = false;
    m_imageDrag = false;
    m_frameInline = false;

    //used by insert picture dialogbox
    m_picture.pictureInline = false;
    m_picture.keepRatio = true;



    m_frameInlineType=FT_TABLE;
    m_viewMode = viewMode;
    cmdMoveFrame=0L;

    // Default table parameters.
    m_table.rows = 3;
    m_table.cols = 2;
    m_table.width = KWTableFrameSet::TblAuto;
    m_table.height = KWTableFrameSet::TblAuto;
    m_table.floating = true;
    m_table.tableTemplateName=QString::null;
    m_table.format=31;

    m_tableSplit.nbRows=1;
    m_tableSplit.nbCols=1;

    m_footEndNote.noteType = FootNote;
    m_footEndNote.numberingType = KWFootNoteVariable::Auto;


    curTable = 0L;
    m_printing = false;
    m_deleteMovingRect = false;
    m_resizedFrameInitialMinHeight = 0;
    m_temporaryStatusBarTextShown = false;

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setAcceptDrops( TRUE );

    setKeyCompression( TRUE );
    viewport()->setMouseTracking( TRUE );

    m_scrollTimer = new QTimer( this );
    connect( m_scrollTimer, SIGNAL( timeout() ),
             this, SLOT( doAutoScroll() ) );

    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    setFocus();
    setInputMethodEnabled( true );
    viewport()->installEventFilter( this );
    installEventFilter( this );
    KCursor::setAutoHideCursor( this, true, true );

    connect( this, SIGNAL(contentsMoving( int, int )),
             this, SLOT(slotContentsMoving( int, int )) );

    connect( m_doc, SIGNAL( newContentsSize() ),
             this, SLOT( slotNewContentsSize() ) );

    connect( m_doc, SIGNAL( mainTextHeightChanged() ),
             this, SLOT( slotMainTextHeightChanged() ) );

    connect( m_doc, SIGNAL( sig_terminateEditing( KWFrameSet * ) ),
             this, SLOT( terminateEditing( KWFrameSet * ) ) );

    slotNewContentsSize();

    m_mouseMode = MM_EDIT; // avoid UMR in setMouseMode
    setMouseMode( MM_EDIT );

    // Create the current frameset-edit last, to have everything ready for it
    KWFrameSet * fs = 0L;
    QString fsName = m_doc->initialFrameSet();
    if ( !fsName.isEmpty() )
        fs = m_doc->frameSetByName( fsName );
    if ( !fs )
        fs = m_doc->frameSet( 0 );
    Q_ASSERT( fs );
    if ( fs && fs->isVisible( m_viewMode ) ) {
        checkCurrentEdit( fs );
        KWTextFrameSetEdit* textedit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit);
        if ( textedit ) {
            int paragId = m_doc->initialCursorParag();
            int index = m_doc->initialCursorIndex();
            if ( paragId != 0 || index != 0 ) {
                KoTextParag* parag = textedit->textDocument()->paragAt( paragId );
                if ( parag )
                    textedit->setCursor( parag, index );
            }
        }
    }
    m_doc->deleteInitialEditingInfo();
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
    delete cmdMoveFrame;
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
    drawFrameSet( fs, &p, crect, true, resetChanged, m_viewMode );
    if ( m_doc->showGrid() )
      drawGrid( p, crect );
}

void KWCanvas::repaintAll( bool erase /* = false */ )
{
    //kdDebug(32002) << "KWCanvas::repaintAll erase=" << erase << endl;
    viewport()->repaint( erase );
}

void KWCanvas::viewportResizeEvent( QResizeEvent * )
{
    // repaint only in preview mode
    if ( m_viewMode->type() == "ModePreview" )
    {
        m_doc->updateResizeHandles();
        viewport()->repaint( false );
    }
}

void KWCanvas::print( QPainter *painter, KPrinter *printer )
{
    // Prevent cursor drawing and editing
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->focusOutEvent();
    m_printing = true;
    KWViewMode *viewMode = new KWViewModePrint( m_doc );
    // Use page list specified in kdeprint dialogbox
    QValueList<int> pageList = printer->pageList();
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
        int yOffset = m_doc->zoomItY( m_doc->pageManager()->topOfPage( pgNum ) );
        kdDebug(32001) << "printing page " << pgNum << " yOffset=" << yOffset << endl;
        QRect pageRect = m_doc->pageManager()->page(pgNum)->zoomedRect(m_doc);
        painter->fillRect( pageRect, white );

        painter->translate( 0, -yOffset );
        painter->setBrushOrigin( 0, -yOffset );
        drawDocument( painter, pageRect, viewMode );
        kapp->processEvents();
        painter->restore();
    }
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->focusInEvent();
    m_printing = false;
    delete viewMode;
}

void KWCanvas::drawContents( QPainter *painter, int cx, int cy, int cw, int ch )
{
  if ( isUpdatesEnabled() )
  {
        // Note: in drawContents, the painter is already translated to the contents coordinates
    painter->setBrushOrigin( -contentsX(), -contentsY() );
    drawDocument( painter, QRect( cx, cy, cw, ch ), m_viewMode );
    if ( m_doc->showGrid() )
      drawGrid( *painter, QRect( cx, cy, cw, ch ) );
  }
}

void KWCanvas::drawDocument( QPainter *painter, const QRect &crect, KWViewMode* viewMode )
{
    //kdDebug(32002) << "KWCanvas::drawDocument crect: " << DEBUGRECT( crect ) << endl;

    // set the viewmode canvas
    // (all views share the same viewmode instance)
    viewMode->setCanvas( this );

    // Draw the outside of the pages (shadow, gray area)
    // and the empty area first (in case of transparent frames)
    if ( painter->device()->devType() != QInternal::Printer ) // except when printing
    {
        QRegion emptySpaceRegion( crect );
        m_doc->createEmptyRegion( crect, emptySpaceRegion, viewMode );
        viewMode->drawPageBorders( painter, crect, emptySpaceRegion );
    }

    // Draw all framesets contents
    QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if(! frameset->isVisible()) continue;
        drawFrameSet( frameset, painter, crect, false, true, viewMode );
    }

    m_doc->maybeDeleteDoubleBufferPixmap();
}

void KWCanvas::drawFrameSet( KWFrameSet * frameset, QPainter * painter,
                             const QRect & crect, bool onlyChanged, bool resetChanged, KWViewMode* viewMode )
{
    if ( !frameset->isVisible( viewMode ) )
        return;
    if ( !onlyChanged && frameset->isFloating() )
        return;

    bool focus = hasFocus() || viewport()->hasFocus();
    if ( painter->device()->devType() == QInternal::Printer )
        focus = false;

    QColorGroup gb = QApplication::palette().active();
    if ( focus && m_currentFrameSetEdit && frameset == m_currentFrameSetEdit->frameSet() )
        // Currently edited frameset
        m_currentFrameSetEdit->drawContents( painter, crect, gb, onlyChanged, resetChanged, viewMode );
    else
        frameset->drawContents( painter, crect, gb, onlyChanged, resetChanged, 0L, viewMode );
}

void KWCanvas::keyPressEvent( QKeyEvent *e )
{
    if( !m_doc->isReadWrite()) {
        switch( e->key() ) {
        case Qt::Key_Down:
            setContentsPos( contentsX(), contentsY() + 10 );
            break;
        case Qt::Key_Up:
            setContentsPos( contentsX(), contentsY() - 10 );
            break;
        case Qt::Key_Left:
            setContentsPos( contentsX() - 10, contentsY() );
            break;
        case Qt::Key_Right:
            setContentsPos( contentsX() + 10, contentsY() );
            break;
        case Qt::Key_PageUp:
            setContentsPos( contentsX(), contentsY() - visibleHeight() );
            break;
        case Qt::Key_PageDown:
            setContentsPos( contentsX(), contentsY() + visibleHeight() );
            break;
        case Qt::Key_Home:
            setContentsPos( contentsX(), 0 );
            break;
        case Qt::Key_End:
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
    m_viewMode = newViewMode;
}

void KWCanvas::mpEditFrame( QMouseEvent *e, const QPoint &nPoint, MouseMeaning meaning ) // mouse press in edit-frame mode
// This can be called by KWResizeHandle::mousePressEvent
{
    KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
    m_mouseMeaning = meaning;
    m_mousePressed = true;
    m_frameMoved = false;
    m_frameResized = false;
    m_ctrlClickOnSelectedFrame = false;

    if ( e )
    {
        KWFrame * frame = m_doc->frameUnderMouse( nPoint );
        KWFrameSet *fs = frame ? frame->frameSet() : 0;
        KWTableFrameSet *table= fs ? fs->getGroupManager() : 0;
        KWDocument::TableToSelectPosition posn;

        if ( fs && ( e->state() & ShiftButton ) && table ) { // is table and we hold shift
            KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
            table->selectUntil( docPoint.x(), docPoint.y() );
        }
        else if ( (posn = m_doc->positionToSelectRowcolTable( nPoint, &table))
            != KWDocument::TABLE_POSITION_NONE)  // we are in position to select full row/cells of a table + hold shift
        {
            if( e->state() & ShiftButton ) {
                KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
                if(posn == KWDocument::TABLE_POSITION_RIGHT)
                    table->selectUntil( table->boundingRect().right(), docPoint.y() );
                else
                    table->selectUntil( docPoint.x(), table->boundingRect().bottom() );
            }
        }
        else if ( frame && !frame->isSelected() ) // clicked on a frame that wasn't selected
        {
            if ( ! ( e->state() & ShiftButton || e->state() & ControlButton ) )
                selectAllFrames( FALSE ); // if we don't hold shift or control, destroy old frame selection
            KWFrame *f = m_doc->frameUnderMouse(nPoint, 0L, true);
            if (f == frame) {
                if (e->state() & ShiftButton)
                selectAllFrames( FALSE ); // shift deselects everything.
                selectFrame( frame, TRUE ); // select the frame.
            }
            else
                m_ctrlClickOnSelectedFrame = true;

        }
        else if(frame)  // clicked on a frame that was already selected
        {
            if ( e->state() & ControlButton )
                m_ctrlClickOnSelectedFrame = true;
            else
            {
                if ( e->state() & ShiftButton )
                    selectFrame( frame, FALSE );
                else {
                    // Resizing?
                    if ( m_mouseMeaning >= MEANING_TOPLEFT /*hack*/ ) {
                        // We can only resize one frame at a time
                        selectAllFrames( FALSE );
                        selectFrame( frame, TRUE );
                    }
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
        if ( frame->frameSet()->isHeaderOrFooter() )
            frame = frame->frameSet()->frame( 0 );
        m_resizedFrameInitialSize = frame->normalize();
        m_resizedFrameInitialMinHeight = frame->minFrameHeight();
    }

    QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
    QValueList<FrameIndex> frameindexList;
    QValueList<FrameMoveStruct> frameindexMove;
    KWFrame *frame=0L;
    // When moving many frames, we look at the bounding rect.
    // It's the one that will be checked against the limits, etc.
    m_boundingRect = KoRect();
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        KWFrameSet * fs = frame->frameSet();
        if ( !(m_doc->processingType() == KWDocument::WP && m_doc->frameSetNum( fs ) == 0 )&& !fs->isAHeader() && !fs->isAFooter()  )
        {
            // If one cell belongs to a table, we are in fact moving the whole table
            KWTableFrameSet *table = fs->getGroupManager();
            // We'll have to do better in the long run
            if ( table ) {
                KWTableFrameSet::Cell *theCell=static_cast<KWTableFrameSet::Cell *>(fs);
                for(unsigned int col=0; col < table->getCols(); col++) {
                    KWTableFrameSet::Cell *c = table->cell(theCell->firstRow(), col);
                    m_boundingRect |= *c->frame(0);
                }
            } else
            {
                m_boundingRect |= frame->outerKoRect();
            }
            frameindexList.append( FrameIndex( frame ) );
            // The final position will only be known afterwards
            frameindexMove.append( FrameMoveStruct( frame->topLeft(), KoPoint() ) );
        }

    }
    m_hotSpot = docPoint - m_boundingRect.topLeft();
    if(frameindexMove.count()!=0)
    {
        delete cmdMoveFrame;
        cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"), frameindexList, frameindexMove );
    }

    viewport()->setCursor( m_doc->getMouseCursor( nPoint, e ? e->state() : 0 ) );

    m_deleteMovingRect = false;
}

void KWCanvas::mpCreate( const QPoint& normalPoint )
{
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    if ( m_doc->snapToGrid() )
        applyGrid( docPoint );
    m_insRect.setCoords( docPoint.x(), docPoint.y(), 0, 0 );
    m_deleteMovingRect = false;
}

void KWCanvas::mpCreatePixmap( const QPoint& normalPoint )
{
    if ( !m_kopicture.isNull() )
    {
        // Apply grid for the first corner only
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        int pageNum = m_doc->pageManager()->pageNumber( &docPoint );
        if ( m_doc->snapToGrid() )
            applyGrid( docPoint );
        m_insRect.setRect( docPoint.x(), docPoint.y(), 0, 0 );
        m_deleteMovingRect = false;

        if ( !m_pixmapSize.isEmpty() )
        {
            // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
            uint width = qRound( (double)m_pixmapSize.width() * m_doc->zoomedResolutionX() / POINT_TO_INCH( KoGlobal::dpiX() ) );
            uint height = qRound( (double)m_pixmapSize.height() * m_doc->zoomedResolutionY() / POINT_TO_INCH( KoGlobal::dpiY() ) );
            m_insRect.setWidth( m_doc->unzoomItX( width ) );
            m_insRect.setHeight( m_doc->unzoomItY( height ) );
            // Apply reasonable limits
            width = kMin( width, m_doc->paperWidth(pageNum) - normalPoint.x() - 5 );
            height = kMin( height, m_doc->paperHeight(pageNum) - normalPoint.y() - 5 );
            // And apply aspect-ratio if set
            if ( m_keepRatio )
            {
                double ratio = ((double) m_pixmapSize.width()) / ((double) m_pixmapSize.height());
                applyAspectRatio( ratio, m_insRect );
            }

            QPoint nPoint( normalPoint.x() + m_doc->zoomItX( m_insRect.width() ),
                           normalPoint.y() + m_doc->zoomItY( m_insRect.height() ) );
            QPoint vPoint = m_viewMode->normalToView( nPoint );
            vPoint = contentsToViewport( vPoint );
            QRect viewportRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
            if ( viewportRect.contains( vPoint ) ) // Don't move the mouse out of the viewport
                QCursor::setPos( viewport()->mapToGlobal( vPoint ) );
        }
        emit docStructChanged(Pictures);
    }
}

void KWCanvas::contentsMousePressEvent( QMouseEvent *e )
{
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );


    if ( e->button() == LeftButton )
    {
      m_mousePressed = true;
    }

    // Only edit-mode (and only LMB) allowed on read-only documents (to select text)
    if ( !m_doc->isReadWrite() && ( m_mouseMode != MM_EDIT || e->button() != LeftButton ) )
        return;
    if ( m_printing )
        return;

    // This code here is common to all mouse buttons, so that RMB and MMB place the cursor (or select the frame) too
    switch ( m_mouseMode ) {
    case MM_EDIT:
    {
        // See if we clicked on a frame's border
        KWFrame* frame;
        m_mouseMeaning = m_doc->getMouseMeaning( normalPoint, e->state(), &frame );
        //kdDebug(32001) << "contentsMousePressEvent meaning=" << m_mouseMeaning << endl;
        switch ( m_mouseMeaning )  {
        case MEANING_MOUSE_MOVE:
        case MEANING_MOUSE_SELECT:
        {
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();
            mpEditFrame( e, normalPoint, m_mouseMeaning );

            KWTableFrameSet *table = 0L;
            KWDocument::TableToSelectPosition ePositionTable = m_doc->positionToSelectRowcolTable( normalPoint, &table);
            // are we in the situation to select row/cols of a table?
            if (ePositionTable != KWDocument::TABLE_POSITION_NONE)
            {   // YES => select row/col
                if (ePositionTable == KWDocument::TABLE_POSITION_RIGHT)
                {  // in position to select a ROW
                    // here the cursor is on the left of the table. the y is OK, but the x is not,
                    // hence finding a proper x with the table object
                    KWTableFrameSet::Cell *cell = table->cell( table->leftWithoutBorder(), m_doc->unzoomItY(normalPoint.y())  );
                    if (cell)
                    {
                        table->selectRow( cell->firstRow() );
                        curTable = table;
                        emit frameSelectedChanged();
                    }
                }
                else
                { // in position to select a COLUMN
                    // here the cursor is on top of the table. the x is ok, but the y is not.
                    KWTableFrameSet::Cell *cell = table->cell( m_doc->unzoomItX(normalPoint.x()), table->topWithoutBorder()  );
                    if (cell)
                    {
                        table->selectCol( cell->firstCol() );
                        curTable = table;
                        emit frameSelectedChanged();
                    }
                }
            } // end select row/col
            break;
        }
        case MEANING_MOUSE_INSIDE:
        case MEANING_MOUSE_OVER_LINK:
        case MEANING_MOUSE_OVER_FOOTNOTE:
        case MEANING_MOUSE_INSIDE_TEXT:
        case MEANING_ACTIVATE_PART:
        {
            // LMB/MMB inside a frame always unselects all frames
            // RMB inside a frame unselects too, except when
            //     right-clicking on a selected frame
            if ( ! ( e->button() == RightButton && frame && frame->isSelected() ) )
                if ( selectAllFrames( false ) )
                    emit frameSelectedChanged();

            KWFrameSet * fs = frame ? frame->frameSet() : 0L;
            bool emitChanged = false;
            if ( fs )
            {
                // Clicked inside a frame - start editing it
                emitChanged = checkCurrentEdit( fs );
            }

            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->mousePressEvent( e, normalPoint, docPoint );

            if ( emitChanged ) // emitted after mousePressEvent [for tables]
                emit currentFrameSetEditChanged();
            emit updateRuler();

            if ( m_frameInline )
            {
                bool inlineCreated = true;
                if(m_frameInlineType==FT_TABLE)
                    inlineCreated = insertInlineTable();
                else if(m_frameInlineType==FT_PICTURE)
                    inlineCreated = m_gui->getView()->insertInlinePicture();
                if (inlineCreated)
                    m_frameInline=false;
                else
                    KMessageBox::information(0L, i18n("Read-only content cannot be changed. No modifications will be accepted."));
            }
            break;
        }
        case MEANING_RESIZE_COLUMN:
        case MEANING_RESIZE_ROW:
        {
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();

            // We know we're resizing a row or column, but we don't know which one yet...
            // We're between two rows (or columns) so frameUnderMouse is a bit unprecise...
            // We need it to find out which table we clicked on, though.
            bool border = false;
            KWFrame * frame = m_doc->frameUnderMouse( normalPoint, &border );
            if (frame)
            {
                KWTableFrameSet::Cell* cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
                if ( cell )
                {
                    KWTableFrameSet* table = cell->getGroupManager();
                    if ( m_mouseMeaning == MEANING_RESIZE_COLUMN )
                    {
                        m_rowColResized = table->columnEdgeAt( docPoint.x() );
                        m_previousTableSize = table->columnSize( m_rowColResized );
                    }
                    else
                    {
                        m_rowColResized = table->rowEdgeAt( docPoint.y() );
                        m_previousTableSize = table->rowSize( m_rowColResized );
                    }
                    curTable = table;
                    kdDebug(32002) << "resizing row/col edge. m_rowColResized=" << m_rowColResized << endl;
                }
            }
            break;
        }
        case MEANING_TOPLEFT:
        case MEANING_TOP:
        case MEANING_TOPRIGHT:
        case MEANING_RIGHT:
        case MEANING_BOTTOMRIGHT:
        case MEANING_BOTTOM:
        case MEANING_BOTTOMLEFT:
        case MEANING_LEFT:
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();
            // select frame
            mpEditFrame( e, normalPoint, m_mouseMeaning );
            //mmEditFrameResize( bool top, bool bottom, bool left, bool right, e->state() & ShiftButton );
            break;
        case MEANING_NONE:
            break;
        }
        m_scrollTimer->start( 50 );
    }
    break;
    case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE:
    case MM_CREATE_FORMULA:
        if ( e->button() == LeftButton )
            mpCreate( normalPoint );
        break;
    case MM_CREATE_PIX:
        if ( e->button() == LeftButton )
            mpCreatePixmap( normalPoint );
        break;
    default: break;
    }

    if ( e->button() == MidButton ) {
        if ( m_doc->isReadWrite() && m_currentFrameSetEdit && m_mouseMode == MM_EDIT )
        {
            QApplication::clipboard()->setSelectionMode( true );
            m_currentFrameSetEdit->paste();
            QApplication::clipboard()->setSelectionMode( false );
        }
    }
    else if ( e->button() == RightButton ) {
        if(!m_doc->isReadWrite()) // The popups are not available in readonly mode, since the GUI isn't built...
            return;
        if ( m_deleteMovingRect )
            deleteMovingRect();
        // rmb menu
        switch ( m_mouseMode )
        {
        case MM_EDIT:
        {
            if ( viewMode()->type()=="ModeText")
                m_gui->getView()->openPopupMenuInsideFrame( m_doc->frameSet( 0 )->frame(0), QCursor::pos() );
            else
            {
                // See if we clicked on a frame's border
                bool border = false;
                KWFrame * frame = m_doc->frameUnderMouse( normalPoint, &border );
                if ( ( frame && ( border || frame->isSelected() ) )
                       || e->state() & ControlButton )
                {
                    m_gui->getView()->openPopupMenuEditFrame( QCursor::pos() );
                }
                else
                {
                    if ( frame )
                        m_gui->getView()->openPopupMenuInsideFrame( frame, QCursor::pos() );
                    else
                        m_gui->getView()->openPopupMenuChangeAction( QCursor::pos() );
                }
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
                            bool isFloating,
                            KWTableTemplate *tt, int format )
{
    // Remember for next time in any case
    m_table.rows = rows;
    m_table.cols = cols;
    m_table.width = wid;
    m_table.height = hei;
    m_table.floating = isFloating;
    m_table.format = format;

    m_table.tableTemplateName = tt ? tt->displayName():QString::null;
    m_table.tt = tt;

    if ( isFloating  )
    {
        m_frameInline=true;
        m_frameInlineType=FT_TABLE;
        m_gui->getView()->displayFrameInlineInfo();
    }
    else
    {
        m_frameInline=false;
        setMouseMode( MM_CREATE_TABLE );
    }
}

bool KWCanvas::insertInlineTable()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit);
    if(edit)
    {
        if ( edit->textFrameSet()->textObject()->protectContent() )
            return false;
        m_insRect = KoRect( 0, 0, edit->frameSet()->frame(0)->width(), 10 );

        KWTableFrameSet * table = createTable();
        m_doc->addFrameSet( table, false );
        edit->insertFloatingFrameSet( table, i18n("Insert Inline Table") );
        table->finalize();

        if (m_table.tt) {
            KWTableTemplateCommand *ttCmd=new KWTableTemplateCommand( "Apply template to inline table", table, m_table.tt );
            m_doc->addCommand(ttCmd);
            ttCmd->execute();
        }

        m_doc->updateAllFrames();
        m_doc->refreshDocStructure(Tables);
    }
    else
    {
        m_frameInline=false;
    }
    m_gui->getView()->updateFrameStatusBarItem();
    return true;
}

void KWCanvas::mmEditFrameResize( bool top, bool bottom, bool left, bool right, bool noGrid )
{
    // This one is also called by KWResizeHandle
    KWFrame *frame = m_doc->getFirstSelectedFrame();
    if (!frame) { // can't happen, but never say never
        kdWarning(32001) << "KWCanvas::mmEditFrameResize: no frame selected!" << endl;
        return;
    }
    //kdDebug(32002) << "KWCanvas::mmEditFrameResize top,bottom,left,right: "
    //          << top << "," << bottom << "," << left << "," << right << endl;

    // Get the mouse position from QCursor. Trying to get it from KWResizeHandle's
    // mouseMoveEvent leads to the frame 'jumping' because the events are received async.
    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
    mousep = m_viewMode->viewToNormal( mousep );

    KoPoint docPoint = m_doc->unzoomPoint( mousep );
    noGrid = ( noGrid && m_doc->snapToGrid() ) || ( !noGrid && !m_doc->snapToGrid() );
    // Apply the grid, unless Shift is pressed
    if ( !noGrid )
        applyGrid( docPoint );
    double x = docPoint.x();
    double y = docPoint.y();
    int page = m_doc->pageManager()->pageNumber(&docPoint);
    int oldPage = m_doc->pageManager()->pageNumber(frame);

    // Calculate new frame coordinates, using minimum sizes, and keeping it in the bounds of the page
    double newLeft = frame->left();
    double newTop = frame->top();
    double newRight = frame->right();
    double newBottom = frame->bottom();
    KWFrameSet* frameSet = frame->frameSet();
    if ( page == oldPage )
    {
        //kdDebug() << "KWCanvas::mmEditFrameResize old rect " << DEBUGRECT( *frame ) << endl;
        int minHeight = s_minFrameHeight + static_cast<int>(frame->paddingTop() + frame->paddingBottom());
        int minWidth = s_minFrameWidth + static_cast<int>(frame->paddingLeft() + frame->paddingRight());
        if ( top && newTop != y )
        {
            if (newBottom - y < minHeight+5)
                y = newBottom - minHeight - 5;
            y = QMAX( y, m_doc->pageManager()->topOfPage( oldPage ) );
            newTop = y;
        } else if ( bottom && newBottom != y )
        {
            if (y - newTop < minHeight+5)
                y = newTop + minHeight + 5;
            y = QMIN( y, m_doc->pageManager()->topOfPage( oldPage + 1 ) );
            newBottom = y;
        }

        if ( left && newLeft != x )
        {
            if (newRight - x < minWidth)
                x = newRight - minWidth - 5;
            x = QMAX( x, 0 );
            newLeft = x;
        } else if ( right && newRight != x )
        {
            if (x - newLeft < minWidth)
                x = newLeft + minWidth + 5; // why +5 ?
            x = QMIN( x, m_doc->pageManager()->pageLayout(page).ptWidth );
            newRight = x;
        }

        // Keep Aspect Ratio feature
        if ( frameSet->type() == FT_PICTURE &&
             static_cast<KWPictureFrameSet *>( frameSet )->keepAspectRatio()
              )
        {
            double resizedFrameRatio = m_resizedFrameInitialSize.width() / m_resizedFrameInitialSize.height();
            double width = newRight - newLeft;
            double height = newBottom - newTop;

            if ( ( top || bottom ) && ( left || right ) ) // resizing by a corner
                if ( width < height )
                    width = height * resizedFrameRatio;
                else
                    height = width / resizedFrameRatio;
            else  // resizing by a border
                if ( top || bottom )
                    width = height * resizedFrameRatio;
                else
                    height = width / resizedFrameRatio;
            //kdDebug() << "KWCanvas::mmEditFrameResize after aspect ratio: width=" << width << " height=" << height << endl;
            if ( left )
                newLeft = frame->right() - width;
            else
                newRight = frame->left() + width;
            if ( top )
                newTop = frame->bottom() - height;
            else
                newBottom = frame->top() + height;
            //kdDebug() << "KWCanvas::mmEditFrameResize after: newRight=" << newRight << " newBottom=" << newBottom << endl;
            }
        }
    // Check if frame was really resized because otherwise no repaint is needed
    if( newLeft != frame->left() || newRight != frame->right() || newTop != frame->top() || newBottom != frame->bottom() )
    {
        // Keep copy of old rectangle, for repaint()
        QRect oldRect = m_viewMode->normalToView( frame->outerRect(m_viewMode) );
        if ( !m_frameResized )
          m_boundingRect = frame->rect();

        frameSet->resizeFrameSetCoords( frame, newLeft, newTop, newRight, newBottom, false /*not final*/ );
        /*frame->setLeft(newLeft);
        frame->setTop(newTop);
        frame->setRight(newRight);
        frame->setBottom(newBottom);*/

        //kdDebug() << "KWCanvas::mmEditFrameResize newTop=" << newTop << " newBottom=" << newBottom << " height=" << frame->height() << endl;

        // If header/footer, resize the first frame
        if ( frameSet->isHeaderOrFooter() )
        {
            KWFrame * origFrame = frameSet->frame( 0 );
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
        if (frameSet->getGroupManager()) { // is table
            if (!(top || bottom)) { /// full height.
                drawY=frameSet->getGroupManager()->getBoundingRect().y();
                drawHeight=frameSet->getGroupManager()->getBoundingRect().height();
            } else if (!(left || right)) { // full width.
                drawX=frameSet->getGroupManager()->getBoundingRect().x();
                drawWidth=frameSet->getGroupManager()->getBoundingRect().width();
            }
        }
        //p.drawRect( drawX, drawY, drawWidth, drawHeight );
        //p.end();
#endif

        // Move resize handles to new position
        frame->updateResizeHandles();
        // Calculate new rectangle for this frame
        QRect newRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
        // Repaint only the changed rects (oldRect U newRect)
        repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), FALSE );
        m_frameResized = true;

        m_gui->getView()->updateFrameStatusBarItem();
    }
}

void KWCanvas::applyGrid( KoPoint &p )
{
  if ( m_viewMode->type() != "ModeNormal" )
    return;
  // The 1e-10 here is a workaround for some weird division problem.
  // 360.00062366 / 2.83465058 gives 127 'exactly' when shown as a double,
  // but when casting into an int, we get 126. In fact it's 127 - 5.64e-15 !

  // This is a problem when calling applyGrid twice, we get 1 less than the time before.
  p.setX( static_cast<int>( p.x() / m_doc->gridX() + 1e-10 ) * m_doc->gridX() );
  p.setY( static_cast<int>( p.y() / m_doc->gridY() + 1e-10 ) * m_doc->gridY() );
  
  //FIXME It doesn't work in preview mode
}

void KWCanvas::drawGrid( QPainter &p, const QRect& rect )
{
  if ( m_viewMode->type() != "ModeNormal" )
    return;
  QPen _pen = QPen( /*m_doc->gridColor()*/QColor("black"), 6, Qt::DotLine  );
  p.save();
  p.setPen( _pen );

//   Enable this when applyGrid works in preview mode. 
//   if ( m_viewMode->pagesPerRow() > 0 ) //preview mode
//   {
//     double const x = dynamic_cast<KWViewModePreview*>(m_viewMode)->leftSpacing();
//     double const y = dynamic_cast<KWViewModePreview*>(m_viewMode)->topSpacing();
// 
//     int const paperWidth = m_doc->paperWidth();
//     int const paperHeight = m_doc->paperHeight();
// 
//     int zoomedX,  zoomedY;
//     double offsetX = m_doc->gridX();
//     double offsetY = m_doc->gridY();
// 
//     for ( int page = 0; page < m_doc->numPages(); page++ )
//     {
//       int row = page / m_viewMode->pagesPerRow();
//       int col = page % m_viewMode->pagesPerRow();
// 
//       QRect pageRect( x + col * ( paperWidth + 10 ),
//                       y + row * ( paperHeight + 10 ),
//                       paperWidth, paperHeight );
// 
//       for ( double i = offsetX; ( zoomedX = m_doc->zoomItX( i )+pageRect.left() ) < pageRect.right() && offsetX < rect.right(); i += offsetX )
//         for ( double j = offsetY; ( zoomedY = m_doc->zoomItY( j )+pageRect.top() ) < pageRect.bottom() && offsetY < rect.bottom(); j += offsetY )
//           if( rect.contains( zoomedX, zoomedY ) )
//             p.drawPoint( zoomedX, zoomedY );
//     }
// 
//     p.restore();
//     return;
//   }
  QRect pageRect( QPoint(0,0), m_viewMode->contentsSize() );

  int zoomedX,  zoomedY;
  double offsetX = m_doc->gridX();
  double offsetY = m_doc->gridY();

  for ( double i = offsetX; ( zoomedX = m_doc->zoomItX( i )+pageRect.left() ) < pageRect.right(); i += offsetX )
    for ( double j = offsetY; ( zoomedY = m_doc->zoomItY( j )+pageRect.top() ) < pageRect.bottom(); j += offsetY )
      if( rect.contains( zoomedX, zoomedY ) )
        p.drawPoint( zoomedX, zoomedY );

  p.restore();
}

void KWCanvas::applyAspectRatio( double ratio, KoRect& insRect )
{
    double width = insRect.width();
    double height = insRect.height();
    if ( width < height ) // the biggest border is the one in control
        width = height * ratio;
    else
        height = width / ratio;
    insRect.setRight( insRect.left() + width );
    insRect.setBottom( insRect.top() + height );
    //kdDebug() << "KWCanvas::applyAspectRatio: width=" << width << " height=" << height << " insRect=" << DEBUGRECT(insRect) << endl;
}

void KWCanvas::mmEditFrameMove( const QPoint &normalPoint, bool shiftPressed )
{
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    KWPageManager *pageManager = m_doc->pageManager();
    int page = pageManager->pageNumber(&docPoint);
    // Move the bounding rect containing all the selected frames
    KoRect oldBoundingRect = m_boundingRect;
    //int page = m_doc->getPageOfRect( m_boundingRect );
    //kdDebug() << "KWCanvas::mmEditFrameMove docPoint.x=" << docPoint.x()
    //          << "  boundingrect=" << DEBUGRECT(m_boundingRect) << endl;

    // (x and y separately for a better behaviour at limit of page)
    KoPoint p( m_boundingRect.topLeft() );
    //kdDebug() << "KWCanvas::mmEditFrameMove hotspot.x=" << m_hotSpot.x() << endl;
    p.setX( docPoint.x() - m_hotSpot.x() );
    //kdDebug() << "mmEditFrameMove: x (pixel)=" << DEBUGDOUBLE( normalPoint.x() )
    //          << " docPoint.x()=" << DEBUGDOUBLE( docPoint.x() )
    //          << " m_hotSpot.x()=" << DEBUGDOUBLE( m_hotSpot.x() ) << endl;
    //          << " p.x=" << DEBUGDOUBLE( p.x() ) << endl;
    shiftPressed = ( shiftPressed && m_doc->snapToGrid() ) || ( !shiftPressed && !m_doc->snapToGrid() );
    if ( !shiftPressed ) // Shift disables the grid
        applyGrid( p );
    //kdDebug() << "KWCanvas::mmEditFrameMove p.x is now " << DEBUGDOUBLE( p.x() )
    //          << " (" << DEBUGDOUBLE( KWUnit::toMM( p.x() ) ) << " mm)" << endl;
    m_boundingRect.moveTopLeft( p );
    //kdDebug() << "KWCanvas::mmEditFrameMove boundingrect now " << DEBUGRECT(m_boundingRect) << endl;
    // But not out of the margins
    if ( m_boundingRect.left() < 1 ) // 1 pt margin to avoid drawing problems
    {
        p.setX( 1 );
        m_boundingRect.moveTopLeft( p );
    }
    else if ( m_boundingRect.right() > pageManager->pageLayout( page ).ptWidth - 1 )
    {
        p.setX( pageManager->pageLayout( page ).ptWidth - m_boundingRect.width() - 2 );
        m_boundingRect.moveTopLeft( p );
    }
    // Now try Y
    p = m_boundingRect.topLeft();
    p.setY( docPoint.y()- m_hotSpot.y() );
    if ( !shiftPressed ) // Shift disables the grid
        applyGrid( p );
    //kdDebug() << "       (grid again) p.x is now " << DEBUGDOUBLE( p.x() )
    //          << " (" << DEBUGDOUBLE( KWUnit::toMM( p.x() ) ) << " mm)" << endl;
    m_boundingRect.moveTopLeft( p );
    // -- Don't limit to the current page. Let the user move a frame between pages --
    // But we still want to limit to 0 - lastPage
    if ( m_boundingRect.top() < 1 ) // 1 pt margin to avoid drawing problems
    {
        p.setY( 1 );
        m_boundingRect.moveTopLeft( p );
    }
    else if ( m_boundingRect.bottom() >= pageManager->bottomOfPage(pageManager->lastPageNumber() ) )
    {
        //kdDebug() << "KWCanvas::mmEditFrameMove limiting to last page" << endl;
        p.setY( pageManager->bottomOfPage(pageManager->lastPageNumber() ) - 2);
        m_boundingRect.moveTopLeft( p );
    }
    // Another annoying case is if the top and bottom points are not in the same page....
    int topPage = pageManager->pageNumber( &m_boundingRect.topLeft() );
    int bottomPage = pageManager->pageNumber( &m_boundingRect.bottomRight() );
    //kdDebug() << "KWCanvas::mmEditFrameMove topPage=" << topPage << " bottomPage=" << bottomPage << endl;
    if ( topPage != bottomPage )
    {
        // Choose the closest page...
        Q_ASSERT( topPage + 1 == bottomPage ); // Not too sure what to do otherwise
        double topPart = m_boundingRect.bottom() - pageManager->bottomOfPage(topPage);
        if ( topPart < m_boundingRect.height() / 2 )
            // Most of the rect is in the top page
            p.setY( pageManager->bottomOfPage(topPage) - m_boundingRect.height() - 1 );
        else {
            // Most of the rect is in the bottom page
            p.setY( pageManager->topOfPage(bottomPage) + 5 /* grmbl, resize handles.... */ );
            topPage = bottomPage;
        }
        //kdDebug() << "KWCanvas::mmEditFrameMove y set to " << p.y() << endl;

        m_boundingRect.moveTopLeft( p );
    }

    if( m_boundingRect.topLeft() == oldBoundingRect.topLeft() )
        return; // nothing happened (probably due to the grid)

    /*kdDebug() << "boundingRect moved by " << m_boundingRect.left() - oldBoundingRect.left() << ","
      << m_boundingRect.top() - oldBoundingRect.top() << endl;
      kdDebug() << " boundingX+hotspotX=" << m_boundingRect.left() + m_hotSpot.x() << endl;
      kdDebug() << " docPoint.x()=" << docPoint.x() << endl;*/

    QPtrList<KWTableFrameSet> tablesMoved;
    tablesMoved.setAutoDelete( FALSE );
    bool bFirst = true;
    QRegion repaintRegion;
    KoPoint _move=m_boundingRect.topLeft() - oldBoundingRect.topLeft();
    QPtrListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    for ( ; framesetIt.current(); ++framesetIt, bFirst=false )
    {
        KWFrameSet *frameset = framesetIt.current();
        if(! frameset->isVisible()) continue;
        // Can't move main frameset of a WP document
        if ( m_doc->processingType() == KWDocument::WP && bFirst ||
             frameset->type() == FT_TEXT && frameset->frameSetInfo() != KWFrameSet::FI_BODY )
            continue;
        // Can't move frame of floating frameset
        if ( frameset->isFloating() ) continue;
        if ( frameset->isProtectSize() ) continue;

        m_frameMoved = true;
        QPtrListIterator<KWFrame> frameIt( frameset->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame *frame = frameIt.current();
            if ( frame->isSelected() ) {
                if ( frameset->type() == FT_TABLE ) {
                    if ( tablesMoved.findRef( static_cast<KWTableFrameSet *> (frameset) ) == -1 )
                        tablesMoved.append( static_cast<KWTableFrameSet *> (frameset));
                } else {
                    QRect oldRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
                    // Move the frame
                    frame->moveTopLeft( frame->topLeft() + _move );
                    // Calculate new rectangle for this frame
                    QRect newRect( frame->outerRect(m_viewMode) );

                    QRect frameRect( m_viewMode->normalToView( newRect ) );
                    // Repaint only the changed rects (oldRect U newRect)
                    repaintRegion += QRegion(oldRect).unite(frameRect).boundingRect();
                    // Move resize handles to new position
                    frame->updateResizeHandles();
                    if(frame->frameStack())
                    frame->frameStack()->update();
                }
            }
        }
    }

    if ( !tablesMoved.isEmpty() ) {
        //kdDebug() << "KWCanvas::mmEditFrameMove TABLESMOVED" << endl;
        for ( unsigned int i = 0; i < tablesMoved.count(); i++ ) {
            KWTableFrameSet *table = tablesMoved.at( i );
            for ( KWTableFrameSet::TableIter k(table) ; k ; ++k ) {
                KWFrame * frame = k->frame( 0 );
                QRect oldRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
                frame->moveTopLeft( frame->topLeft() + _move );
                // Calculate new rectangle for this frame
                QRect newRect( frame->outerRect(m_viewMode) );
                QRect frameRect( m_viewMode->normalToView( newRect ) );
                // Repaing only the changed rects (oldRect U newRect)
                repaintRegion += QRegion(oldRect).unite(frameRect).boundingRect();
                // Move resize handles to new position
                frame->updateResizeHandles();
            }
        }
    }

    repaintContents( repaintRegion.boundingRect(), FALSE );

    m_gui->getView()->updateFrameStatusBarItem();
}

void KWCanvas::mmCreate( const QPoint& normalPoint, bool shiftPressed ) // Mouse move when creating a frame
{
    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );

    if ( m_deleteMovingRect )
        drawMovingRect( p );

    int page = m_doc->pageManager()->pageNumber( &m_insRect );
    KoRect oldRect = m_insRect;

    // Resize the rectangle
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    shiftPressed = ( shiftPressed && m_doc->snapToGrid() ) || ( !shiftPressed && !m_doc->snapToGrid() );
    if ( m_mouseMode != MM_CREATE_PIX && !shiftPressed )
        applyGrid( docPoint );

    m_insRect.setRight( docPoint.x() );
    m_insRect.setBottom( docPoint.y() );

    // But not out of the page
    KoRect r = m_insRect.normalize();
    if ( !m_doc->pageManager()->page(page)->rect().contains(r) )
    {
        m_insRect = oldRect;
        // #### QCursor::setPos( viewport()->mapToGlobal( zoomPoint( m_insRect.bottomRight() ) ) );
    }

    // Apply keep-aspect-ratio feature
    if ( m_mouseMode == MM_CREATE_PIX && m_keepRatio )
    {
        double ratio = (double)m_pixmapSize.width() / (double)m_pixmapSize.height();
        applyAspectRatio( ratio, m_insRect );
    }

    drawMovingRect( p );
    p.end();
    m_deleteMovingRect = true;
}

void KWCanvas::drawMovingRect( QPainter & p )
{
    p.setPen( black );
    p.drawRect( m_viewMode->normalToView( m_doc->zoomRect( m_insRect ) ) );
}

void KWCanvas::deleteMovingRect()
{
    Q_ASSERT( m_deleteMovingRect );
    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setRasterOp( NotROP );
    p.setPen( black );
    p.setBrush( NoBrush );
    drawMovingRect( p );
    m_deleteMovingRect = false;
    p.end();
}

void KWCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
    if ( m_printing )
        return;
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );

    if ( m_mousePressed ) {
        //doAutoScroll();

        switch ( m_mouseMode ) {
            case MM_EDIT:
            {
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseMoveEvent( e, normalPoint, docPoint );
                else if ( m_doc->isReadWrite() )
                {
                    if ( m_mouseMeaning == MEANING_MOUSE_MOVE )
                        mmEditFrameMove( normalPoint, e->state() & ShiftButton );
                    else if ( m_mouseMeaning == MEANING_RESIZE_COLUMN || m_mouseMeaning == MEANING_RESIZE_ROW )
                    {
                        // TODO undo/redo support (esp. in mouse-release)
                        QRect oldRect( m_viewMode->normalToView( m_doc->zoomRect( curTable->boundingRect() ) ) );
                        if ( m_mouseMeaning == MEANING_RESIZE_ROW )
                            curTable->resizeRow( m_rowColResized, docPoint.y() );
                        else
                            curTable->resizeColumn( m_rowColResized, docPoint.x() );
                        // Repaint only the changed rects (oldRect U newRect)
                        QRect newRect( m_viewMode->normalToView( m_doc->zoomRect( curTable->boundingRect() ) ) );
                        repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), FALSE );
                    }
                    // mousemove during frame-resizing is handled by the resizehandles directly
                    // (since they are widgets. The canvas doesn't get the event).
                    // ... but that's not really true since the active area is 6pt
                    // (and the resize handle widget is 6 pixels...)
                }
            } break;
            case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
            case MM_CREATE_TABLE: case MM_CREATE_FORMULA:
                mmCreate( normalPoint, e->state() & ShiftButton );
            default: break;
        }
    } else {
        if ( m_mouseMode == MM_EDIT )
        {
            MouseMeaning meaning = m_doc->getMouseMeaning( normalPoint, e->state() );
            switch ( meaning ) {
             case MEANING_MOUSE_OVER_FOOTNOTE:
             {
                 KWFrame* frame = m_doc->frameUnderMouse( normalPoint );
                 KWFrameSet * fs = frame ? frame->frameSet() : 0L;
                 if (fs && fs->type() == FT_TEXT)
                 {
                    KoVariable* var = static_cast<KWTextFrameSet *>(fs)->variableUnderMouse(docPoint);
                    if ( var )
                    {
                        KWFootNoteVariable * footNoteVar = dynamic_cast<KWFootNoteVariable *>( var );
                        if ( footNoteVar )
                        {
                            // show the content of the footnote in the status bar
                            gui()->getView()->setTemporaryStatusBarText( footNoteVar->frameSet()->textDocument()->firstParag()->string()->toString() );
                            m_temporaryStatusBarTextShown = true;
                        }
                    }

                 }
                 break;
             }
            case MEANING_MOUSE_OVER_LINK:
            {
                KWFrame* frame = m_doc->frameUnderMouse( normalPoint );
                KWFrameSet * fs = frame ? frame->frameSet() : 0L;
                if (fs && fs->type() == FT_TEXT)
                {
                    KWTextFrameSet *frameset = static_cast<KWTextFrameSet *>(fs);
                    //show the link target in the status bar
                    QString link = frameset->linkVariableUnderMouse(docPoint)->url();
                    if ( link.startsWith("bkm://") )
                        link.replace( 0, 6, i18n("Bookmark target: ") );
                    gui()->getView()->setTemporaryStatusBarText( link );
                    m_temporaryStatusBarTextShown = true;
                }
                break;
            }
            default:
                resetStatusBarText();
                break;
            }
            viewport()->setCursor( m_doc->getMouseCursor( normalPoint, e->state() ) );
        }
    }
}

void KWCanvas::mrEditFrame( QMouseEvent *e, const QPoint &nPoint ) // Can be called from KWCanvas and from KWResizeHandle's mouseReleaseEvents
{
    //kdDebug() << "KWCanvas::mrEditFrame" << endl;
    KWFrame *firstFrame = m_doc->getFirstSelectedFrame();
    //kdDebug() << "KWCanvas::mrEditFrame m_frameMoved=" << m_frameMoved << " m_frameResized=" << m_frameResized << endl;
    if ( firstFrame && ( m_frameMoved || m_frameResized ) )
    {
        KWTableFrameSet *table = firstFrame->frameSet()->getGroupManager();
        if (table) {
            table->recalcCols();
            table->recalcRows();
            if(m_frameResized)
                table->refreshSelectedCell();
            //repaintTableHeaders( table );
        }

        // Create command
        if ( m_frameResized )
        {
            KWFrame *frame = m_doc->getFirstSelectedFrame();
            KWFrameSet *fs = frame->frameSet();
            // If header/footer, resize the first frame
            if ( fs->isHeaderOrFooter() )
                frame = fs->frame( 0 );
            Q_ASSERT( frame );
            if ( frame )
            {
                    FrameIndex index( frame );
                    kdDebug() << "mrEditFrame: initial minframeheight = " << m_resizedFrameInitialMinHeight << endl;
                    FrameResizeStruct tmpResize( m_resizedFrameInitialSize, m_resizedFrameInitialMinHeight,
                                                frame->normalize() );

                    KWFrameResizeCommand *cmd = new KWFrameResizeCommand( i18n("Resize Frame"), index, tmpResize );
                    m_doc->addCommand(cmd);

                    m_doc->frameChanged( frame, m_gui->getView() ); // repaint etc.
                    if(fs->isAHeader() || fs->isAFooter())
                    {
                        m_doc->recalcFrames();
                        frame->updateResizeHandles();
                    }
                    // Especially useful for EPS images: set final size
                    fs->resizeFrame( frame, frame->width(), frame->height(), true );
                    if ( frame && fs->type() == FT_PART )
                        static_cast<KWPartFrameSet *>( fs )->updateChildGeometry( viewMode() );
            }
            delete cmdMoveFrame; // Unused after all
            cmdMoveFrame = 0L;
        }
        else
        {
            Q_ASSERT( cmdMoveFrame ); // has been created by mpEditFrame
            if( cmdMoveFrame )
            {
                // Store final positions
                QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
                QValueList<FrameMoveStruct>::Iterator it = cmdMoveFrame->listFrameMoved().begin();
                for(KWFrame * frame=selectedFrames.first(); frame && it != cmdMoveFrame->listFrameMoved().end();
                    frame=selectedFrames.next() )
                {
                    KWFrameSet * fs = frame->frameSet();
                    if ( !(m_doc->processingType() == KWDocument::WP && m_doc->frameSetNum( fs ) == 0 )&& !fs->isAHeader() && !fs->isAFooter()  )
                    {

                        (*it).newPos = frame->topLeft();
                        ++it;
                    }
                    // Needed for evaluatable formulas
                    fs->moveFrame( frame );
                    if ( frame && fs->type() == FT_PART )
                        static_cast<KWPartFrameSet *>( fs )->updateChildGeometry( viewMode() );
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
        if ( e->state() & ControlButton )
        {
            //KWFrame * frame = m_doc->frameUnderMouse( nPoint );
            if ( m_ctrlClickOnSelectedFrame /* && frame->isSelected() */ ) // kervel: why the && ?
            {
                KWFrame *f = m_doc->frameUnderMouse( nPoint,0L,true );
                if (e->state() & ShiftButton)
                    selectAllFrames( false );
                if (f)
                    selectFrame(f,true);
                emit frameSelectedChanged();
            }
        }
    }
    m_mousePressed = false;
    m_ctrlClickOnSelectedFrame = false;
}

KCommand *KWCanvas::createTextBox( const KoRect & rect )
{
    if ( !m_doc->snapToGrid() || ( rect.width() > m_doc->gridX() && rect.height() > m_doc->gridY() ) ) {
        KWFrame *frame = new KWFrame(0L, rect.x(), rect.y(), rect.width(), rect.height() );
        frame->setNewFrameBehavior(KWFrame::Reconnect);
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top

        QString name = m_doc->generateFramesetName( i18n( "Text Frameset %1" ) );
        KWTextFrameSet *_frameSet = new KWTextFrameSet(m_doc, name );
        _frameSet->addFrame( frame );
        m_doc->addFrameSet( _frameSet );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Text Frame"), frame );
        if ( checkCurrentEdit(frame->frameSet(), true) )
            emit currentFrameSetEditChanged();
        return cmd;
    }
    return 0L;
}

void KWCanvas::mrCreateText()
{
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frame->setMinFrameHeight( frame->height() ); // so that AutoExtendFrame doesn't resize it down right away
        frame->setNewFrameBehavior(KWFrame::Reconnect);
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        KWFrameDia frameDia( this, frame, m_doc, FT_TEXT );
        frameDia.setCaption(i18n("Connect Frame"));
        frameDia.exec();
        if ( checkCurrentEdit(frame->frameSet(), true) )
            emit currentFrameSetEditChanged();
    }
    setMouseMode( MM_EDIT );
    m_doc->repaintAllViews();
    emit docStructChanged(TextFrames);
    emit currentFrameSetEditChanged();
}

void KWCanvas::mrCreatePixmap()
{
    kdDebug() << "KWCanvas::mrCreatePixmap m_insRect=" << DEBUGRECT(m_insRect) << endl;
    // Make sure it's completely on page.
    KoRect picRect( kMin(m_insRect.left(), m_insRect.right()), kMin( m_insRect.top(), m_insRect.bottom()), kAbs( m_insRect.width()), kAbs(m_insRect.height()));
    int page = m_doc->pageManager()->pageNumber( &picRect );
    double pageWidth = m_doc->pageManager()->pageLayout(page).ptWidth;
    if(picRect.right() > pageWidth) {
        double width = picRect.width();

        m_insRect.setLeft(pageWidth - width);
        m_insRect.setRight(pageWidth);
    }

    double pageBottom = m_doc->pageManager()->bottomOfPage(page);
    if(picRect.bottom() >  pageBottom) {
        double height = picRect.height();
        picRect.setTop(pageBottom - height);
        picRect.setBottom(pageBottom);
    }

    if ( picRect.width() > 0 /*m_doc->gridX()*/ &&picRect.height() > 0 /*m_doc->gridY()*/ && !m_kopicture.isNull() )
    {
        KWFrameSet * fs = 0L;
        KWPictureFrameSet *frameset = new KWPictureFrameSet( m_doc, QString::null /*automatic name*/ );
        frameset->insertPicture( m_kopicture );
        frameset->setKeepAspectRatio( m_keepRatio );
        fs = frameset;
        picRect = picRect.normalize();
        KWFrame *frame = new KWFrame(fs, picRect.x(), picRect.y(), picRect.width(),
                                     picRect.height() );
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        frame->setSelected(TRUE);
        fs->addFrame( frame, false );
        m_doc->addFrameSet( fs );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Picture Frame"), frame );
        m_doc->addCommand(cmd);
        m_doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Pictures);
}

void KWCanvas::mrCreatePart() // mouse release, when creating part
{
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        m_doc->insertObject( m_insRect, m_partEntry );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Embedded);
}

void KWCanvas::mrCreateFormula()
{
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( m_doc, QString::null );
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        frameset->addFrame( frame, false );
        m_doc->addFrameSet( frameset );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Formula Frame"), frame );
        m_doc->addCommand(cmd);
        m_doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(FormulaFrames);
}

void KWCanvas::mrCreateTable()
{
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        if ( m_table.cols * s_minFrameWidth + m_insRect.x() > m_doc->pageManager()->pageLayout(0).ptWidth )
        {
            KMessageBox::sorry(0, i18n("KWord is unable to insert the table because there "
                                       "is not enough space available."));
        }
        else {
            KWTableFrameSet * table = createTable();
            KMacroCommand *macroCmd = new KMacroCommand( i18n("Create Table") );

            KWCreateTableCommand *cmd=new KWCreateTableCommand( "Create table", table );
            macroCmd->addCommand(cmd);
            if (m_table.tt) {
                KWTableTemplateCommand *ttCmd=new KWTableTemplateCommand( "Apply template to table", table, m_table.tt );
                macroCmd->addCommand(ttCmd);
            }
            m_doc->addCommand(macroCmd);
            macroCmd->execute();

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
    int pageNum = m_doc->pageManager()->pageNumber(&m_insRect.topLeft());

    // Create a set of cells with random-size frames.
    for ( unsigned int i = 0; i < m_table.rows; i++ ) {
        for ( unsigned int j = 0; j < m_table.cols; j++ ) {
            KWTableFrameSet::Cell *cell = new KWTableFrameSet::Cell( table, i, j, QString::null /*automatic name*/ );
            KWFrame *frame = new KWFrame(cell, 0, 0, 0, 0, KWFrame::RA_BOUNDINGRECT ); // pos and size will be set in setBoundingRect
            frame->setZOrder( m_doc->maxZOrder( pageNum ) + 1 ); // make sure it's on top
            cell->addFrame( frame, false );
            frame->setFrameBehavior(KWFrame::AutoExtendFrame);
            frame->setNewFrameBehavior(KWFrame::NoFollowup);
        }
    }
    KWTableFrameSet::CellSize w;
    w=static_cast<KWTableFrameSet::CellSize>( m_table.width );
    if(m_frameInline) w=KWTableFrameSet::TblManual;
    table->setBoundingRect( m_insRect , w, static_cast<KWTableFrameSet::CellSize>( m_table.height ));
    return table;
}

void KWCanvas::contentsMouseReleaseEvent( QMouseEvent * e )
{
    if ( m_printing )
        return;
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    if ( m_mousePressed ) {
        if ( m_deleteMovingRect )
            deleteMovingRect();

        QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );

        if(m_insRect.bottom()==0 && m_insRect.right()==0) {
            // if the user did not drag, just click; make a 200x150 square for him.
            int page = m_doc->pageManager()->pageNumber(&docPoint);
            KoPageLayout pageLayout = m_doc->pageManager()->pageLayout(page);
            m_insRect.setLeft(QMIN(m_insRect.left(), pageLayout.ptWidth - 200));
            m_insRect.setTop(QMIN(m_insRect.top(), pageLayout.ptHeight - 150));
            m_insRect.setBottom(m_insRect.top()+150);
            m_insRect.setRight(m_insRect.left()+200);
        }
        switch ( m_mouseMode ) {
            case MM_EDIT:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseReleaseEvent( e, normalPoint, docPoint );
                else {
                  if ( m_mouseMeaning == MEANING_RESIZE_COLUMN )
                  {
                    KWResizeColumnCommand *cmd = new KWResizeColumnCommand( curTable, m_rowColResized, m_previousTableSize, docPoint.x() );
                    m_doc->addCommand(cmd);
                    cmd->execute();
                  }
                  else if ( m_mouseMeaning == MEANING_RESIZE_ROW )
                  {
                    KWResizeRowCommand *cmd = new KWResizeRowCommand( curTable, m_rowColResized, m_previousTableSize, docPoint.y() );
                    m_doc->addCommand(cmd);
                    cmd->execute();
                  }
                  else
                    mrEditFrame( e, normalPoint );
                  m_mouseMeaning = MEANING_NONE;
                }
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
    if ( m_printing )
        return;
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    switch ( m_mouseMode ) {
        case MM_EDIT:
            if ( m_currentFrameSetEdit )
            {
                m_mousePressed = true; // needed for the dbl-click + move feature.
                m_scrollTimer->start( 50 );
                m_currentFrameSetEdit->mouseDoubleClickEvent( e, normalPoint, docPoint );
            }
            else
            {
                // Double-click on an embedded object should edit it, not pop up the frame dialog
                // So we have to test for that.
                QPtrList<KWFrame> frames = m_doc->getSelectedFrames();
                bool isPartFrameSet = frames.count() == 1 && frames.first()->frameSet()->type() == FT_PART;

                if ( !isPartFrameSet )
                    editFrameProperties();
                // KWChild::hitTest and KWView::slotChildActivated take care of embedded objects
                m_mousePressed = false;
            }
            break;
        default:
            break;
    }
}

KCommand *KWCanvas::setLeftFrameBorder( KoBorder newBorder, bool on )
{
    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return 0L;
    QPtrList<FrameBorderTypeStruct> tmpBorderList;
    QPtrList<FrameIndex> frameindexList;
    bool leftFrameBorderChanged=false;

    if (!on)
        newBorder.setPenWidth(0);

    QMap<KWTableFrameSet *, KWFrame*> tables;

    KWFrame *frame=0L;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {  // do all selected frames
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->leftBorder();
        tmp->m_EFrameType= FBLeft;

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (newBorder!=frame->leftBorder()) // only commit when it has actually changed
        {
            leftFrameBorderChanged=true;
            KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
            if(cell!=0L) // is a table cell
                tables[cell->getGroupManager()]=frame;
            else
                frame->setLeftBorder(newBorder);
        }
        frame->updateResizeHandles();
        frame->frameBordersChanged();
    }
    QMap<KWTableFrameSet *, KWFrame*>::Iterator it;
    for ( it = tables.begin(); it != tables.end(); ++it )
        it.key()->setLeftBorder(newBorder);

    if(leftFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Left Border Frame"),frameindexList,tmpBorderList,newBorder);
        m_doc->repaintAllViews();
        return cmd;
    }
    else
    {
        tmpBorderList.setAutoDelete(true);
        frameindexList.setAutoDelete(true);
    }
    return 0L;
}

KCommand *KWCanvas::setRightFrameBorder( KoBorder newBorder, bool on )
{
    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return 0L;
    QPtrList<FrameBorderTypeStruct> tmpBorderList;
    QPtrList<FrameIndex> frameindexList;
    bool rightFrameBorderChanged=false;
    KWFrame *frame=0L;
    if (!on)
        newBorder.setPenWidth(0);

    QMap<KWTableFrameSet *, KWFrame*> tables;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    { // do all selected frames
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->rightBorder();
        tmp->m_EFrameType= FBRight;

        tmpBorderList.append(tmp);
        frameindexList.append(index);

        if (newBorder!=frame->rightBorder())
        {
            rightFrameBorderChanged=true;
            KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
            if(cell!=0L) // is a table cell
                tables[cell->getGroupManager()]=frame;
            else
                frame->setRightBorder(newBorder);
        }
        frame->updateResizeHandles();
        frame->frameBordersChanged();
    }
    QMap<KWTableFrameSet *, KWFrame*>::Iterator it;
    for ( it = tables.begin(); it != tables.end(); ++it )
        it.key()->setRightBorder(newBorder);

    if( rightFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Right Border Frame"),frameindexList,tmpBorderList,newBorder);
        m_doc->repaintAllViews();
        return cmd;
    }
    else
    {
        tmpBorderList.setAutoDelete(true);
        frameindexList.setAutoDelete(true);
    }
    return 0L;
}

KCommand *KWCanvas::setTopFrameBorder( KoBorder newBorder, bool on )
{
    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return 0L;

    QPtrList<FrameBorderTypeStruct> tmpBorderList;
    QPtrList<FrameIndex> frameindexList;
    bool topFrameBorderChanged=false;

    KWFrame *frame=0L;
    if (!on)
        newBorder.setPenWidth(0);

    QMap<KWTableFrameSet *, KWFrame*> tables;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    { // do all selected frames
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->topBorder();
        tmp->m_EFrameType= FBTop;

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (newBorder!=frame->topBorder())
        {
            topFrameBorderChanged=true;
            KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
            if(cell!=0L) // is a table cell
                tables[cell->getGroupManager()]=frame;
            else
                frame->setTopBorder(newBorder);
        }
        frame->updateResizeHandles();
        frame->frameBordersChanged();
    }
    QMap<KWTableFrameSet *, KWFrame*>::Iterator it;
    for ( it = tables.begin(); it != tables.end(); ++it )
        it.key()->setTopBorder(newBorder);

    if(topFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Top Border Frame"),frameindexList,tmpBorderList,newBorder);
        m_doc->repaintAllViews();
        return cmd;
    }
    else
    {
        tmpBorderList.setAutoDelete(true);
        frameindexList.setAutoDelete(true);
    }

    return 0L;
}

KCommand *KWCanvas::setBottomFrameBorder( KoBorder newBorder, bool on )
{
    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.count() == 0)
        return 0L;
    bool bottomFrameBorderChanged=false;
    QPtrList<FrameBorderTypeStruct> tmpBorderList;
    QPtrList<FrameIndex> frameindexList;
    KWFrame *frame=0L;
    if (!on)
        newBorder.setPenWidth(0);

    QMap<KWTableFrameSet *, KWFrame*> tables;

    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    { // do all selected frames
        frame=KWFrameSet::settingsFrame(frame);
        FrameIndex *index=new FrameIndex( frame );
        FrameBorderTypeStruct *tmp=new FrameBorderTypeStruct;
        tmp->m_OldBorder=frame->bottomBorder();
        tmp->m_EFrameType= FBBottom;

        tmpBorderList.append(tmp);
        frameindexList.append(index);
        if (newBorder!=frame->bottomBorder())
        {
            bottomFrameBorderChanged=true;
            KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>(frame->frameSet());
            if(cell!=0L) // is a table cell
                tables[cell->getGroupManager()]=frame;
            else
                frame->setBottomBorder(newBorder);
        }
        frame->updateResizeHandles();
        frame->frameBordersChanged();
    }
    QMap<KWTableFrameSet *, KWFrame*>::Iterator it;
    for ( it = tables.begin(); it != tables.end(); ++it )
        it.key()->setBottomBorder(newBorder);

    if(bottomFrameBorderChanged)
    {
        KWFrameBorderCommand *cmd=new KWFrameBorderCommand(i18n("Change Bottom Border Frame"),frameindexList,tmpBorderList,newBorder);
        m_doc->repaintAllViews();
        return cmd;
    }
    else
    {
        tmpBorderList.setAutoDelete(true);
        frameindexList.setAutoDelete(true);
        tmpBorderList.clear();
        frameindexList.clear();

    }
    return 0L;
}

void KWCanvas::setFrameBackgroundColor( const QBrush &_backColor )
{
    QPtrList <KWFrame> selectedFrames = m_doc->getSelectedFrames();
    if (selectedFrames.isEmpty())
        return;
    bool colorChanged=false;
    KWFrame *frame=0L;
    QPtrList<FrameIndex> frameindexList;
    QPtrList<QBrush> oldColor;
    for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
    {
        frame=KWFrameSet::settingsFrame(frame);

        FrameIndex *index=new FrameIndex( frame );
        frameindexList.append(index);

        QBrush *_color=new QBrush(frame->backgroundColor());
        oldColor.append(_color);

        if (frame->frameSet() && frame->frameSet()->type()!=FT_PICTURE && frame->frameSet()->type()!=FT_PART &&  _backColor!=frame->backgroundColor())
        {
            colorChanged=true;
            frame->setBackgroundColor(_backColor);
        }
    }
    if(colorChanged)
    {
        KWFrameBackGroundColorCommand *cmd=new KWFrameBackGroundColorCommand(i18n("Change Frame Background Color"),frameindexList,oldColor,_backColor);
        m_doc->addCommand(cmd);
        m_doc->repaintAllViews();
    }
    else
    {
        frameindexList.setAutoDelete(true);
        oldColor.setAutoDelete(true);
    }
}

void KWCanvas::editFrameProperties( KWFrameSet * frameset )
{
    KWFrameDia *frameDia;
    KWFrame *frame = frameset->frame(0);
    frameDia = new KWFrameDia( this, frame );
    frameDia->exec();
    delete frameDia;
}

void KWCanvas::editFrameProperties()
{
    QPtrList<KWFrame> frames=m_doc->getSelectedFrames();
    if(frames.count()==0) return;
    KWFrameDia *frameDia;
    if(frames.count()==1) {
        KWFrame *frame = frames.first();
        frameDia = new KWFrameDia( this, frame );
    } else { // multi frame dia.
        frameDia = new KWFrameDia( this, frames );
    }
    frameDia->exec();
    delete frameDia;
}

bool KWCanvas::selectAllFrames( bool select )
{
    bool ret = false;
    QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        if ( !fs->isVisible() ) continue;
        if ( select && fs->isMainFrameset() ) continue; // "select all frames" shouldn't select the page
        QPtrListIterator<KWFrame> frameIt = fs->frameIterator();
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

void KWCanvas::tableSelectCell(KWTableFrameSet *table, KWFrameSet *cell)
{
    if ( m_currentFrameSetEdit )
        terminateCurrentEdit();
    selectFrame( cell->frame(0), TRUE ); // select the frame.
    curTable = table;
    emit frameSelectedChanged();
}

void KWCanvas::selectFrame( KWFrame * frame, bool select )
{
    if ( frame->isSelected() != select )
        frame->setSelected( select );
}

void KWCanvas::cutSelectedFrames()
{
    copySelectedFrames();
    m_gui->getView()->deleteFrame(false);
}

void KWCanvas::copySelectedFrames()
{
    QDomDocument domDoc( "SELECTION" );
    QDomElement topElem = domDoc.createElement( "SELECTION" );
    domDoc.appendChild( topElem );
    bool foundOne = false;
    QPtrList<KoDocumentChild> embeddedObjects;

    KoStoreDrag *kd = new KoStoreDrag( "application/x-kword", 0L );
    QDragObject* dragObject = kd;
    QByteArray arr;
    QBuffer buffer(arr);
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, "application/x-kword" );

    QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        KWFrameSet * fs = fit.current();
        if ( fs->isVisible() && fs->type() == FT_PART
             && fs->frameIterator().getFirst()->isSelected() ) {
            foundOne = true;
            embeddedObjects.append( static_cast<KWPartFrameSet *>(fs)->getChild() );
        }
    }

    // Save internal embedded objects first, since it might change their URL
    int i = 0;
    QValueList<KoPictureKey> savePictures;
    QPtrListIterator<KoDocumentChild> chl( embeddedObjects );
    for( ; chl.current(); ++chl ) {
        KoDocument* childDoc = chl.current()->document();
        if ( childDoc && !childDoc->isStoredExtern() )
            (void) childDoc->saveToStore( store, QString::number( i++ ) );
    }

    // We really need a selected-frames-list !
    fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        if ( fs->isVisible() )
        {
            bool isTable = ( fs->type() == FT_TABLE );
            if ( fs->type() == FT_PART )
                continue;
            QPtrListIterator<KWFrame> frameIt = fs->frameIterator();
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
                    if ( frame == firstFrame || isTable )
                    {
                        parentElem = fs->toXML( parentElem, isTable ? true : false );
                        // We'll save the frame inside that frameset tag
                    }
                    if ( !isTable )
                    {
                        // Save the frame information
                        QDomElement frameElem = parentElem.ownerDocument().createElement( "FRAME" );
                        parentElem.appendChild( frameElem );
                        frame->save( frameElem );
                        if ( frame != firstFrame )
                        {
                            // Frame saved alone -> remember which frameset it's part of
                            frameElem.setAttribute( "parentFrameset", fs->getName() );
                        }
                        if ( fs->type() == FT_PICTURE ) {
                            KoPictureKey key = static_cast<KWPictureFrameSet *>( fs )->key();
                            if ( !savePictures.contains( key ) )
                                savePictures.append( key );
                        }
                    }
                    foundOne = true;
                    if ( isTable ) // Copy tables only once, even if they have many cells selected
                        break;
                }
            }
        }
    }

    if ( !foundOne ) {
        delete store;
        delete kd;
        return;
    }

    if ( !embeddedObjects.isEmpty() )
        m_doc->saveEmbeddedObjects( topElem, embeddedObjects );
    if ( !savePictures.isEmpty() ) {
        // Save picture list at the end of the main XML
        topElem.appendChild( m_doc->pictureCollection()->saveXML( KoPictureCollection::CollectionPicture, domDoc, savePictures ) );
        // Save the actual picture data into the store
        m_doc->pictureCollection()->saveToStore( KoPictureCollection::CollectionPicture, store, savePictures );
        // Single image -> put it in dragobject too
        if ( savePictures.count() == 1 )
        {
            KoPicture pic = m_doc->pictureCollection()->findPicture( savePictures.first() );
            QDragObject* picDrag = pic.dragObject( 0L );
            kdDebug() << k_funcinfo << "picDrag=" << picDrag << endl;
            if ( picDrag ) {
                KMultipleDrag* multipleDrag = new KMultipleDrag( 0L );
                multipleDrag->addDragObject( kd );
                multipleDrag->addDragObject( picDrag );
                dragObject = multipleDrag;
                kdDebug() << k_funcinfo << "multiple" << endl;
            }
        }
    }

    if ( store->open( "root" ) )
    {
        QCString s = domDoc.toCString(); // this is already Utf8!
        kdDebug(32001) << "KWCanvas::copySelectedFrames: " << s << endl;
        (void)store->write( s.data(), s.size()-1 );
        store->close();
    }

    // Maybe we need to also save styles, framestyles and tablestyles.

    delete store;
    kd->setEncodedData( arr );
    QApplication::clipboard()->setData( dragObject );
}

// ####### obsolete
void KWCanvas::pasteFrames()
{
    QMimeSource *data = QApplication::clipboard()->data();
    QByteArray arr = data->encodedData( KoStoreDrag::mimeType( "application/x-kword" ) );
    if ( !arr.size() )
        return;
    QBuffer buffer( arr );
    KoStore* store = KoStore::createStore( &buffer, KoStore::Read );
    if ( !store->bad() )
    {
        if ( store->open( "root" ) )
        {
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument domDoc;
            if ( !domDoc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn ) )
            {
                kdError (30003) << "Parsing Error! Aborting! (in KWCanvas::pasteFrames)" << endl
                                << "  Line: " << errorLine << " Column: " << errorColumn << endl
                                << "  Message: " << errorMsg << endl;
                delete store;
                return;
            }
            kdDebug() << domDoc.toCString() << endl;
            QDomElement topElem = domDoc.documentElement();

            KMacroCommand * macroCmd = new KMacroCommand( i18n( "Paste Frames" ) );
            m_doc->pasteFrames( topElem, macroCmd, false, false, true /*select frames*/ );
            m_doc->loadPictureMap( topElem );
            store->close();
            m_doc->loadImagesFromStore( store );
            m_doc->insertEmbedded( store, topElem, macroCmd, 20.0 );
            m_doc->completePasting();
            m_doc->addCommand( macroCmd );
        }
    }
    delete store;
}

// Superseded by getCurrentTable
/*KWTableFrameSet *KWCanvas::getTable()
{
    if( !m_currentFrameSetEdit)
        return 0L;

    if(m_currentFrameSetEdit->frameSet()->type() == FT_TABLE)
        return static_cast<KWTableFrameSet *> (m_currentFrameSetEdit->frameSet());

    return 0L;
}*/

void KWCanvas::editFrameSet( KWFrameSet * frameSet, bool onlyText /*=false*/ )
{
    if ( selectAllFrames( false ) )
        emit frameSelectedChanged();

    bool emitChanged = checkCurrentEdit( frameSet, onlyText );

    if ( emitChanged ) // emitted after mousePressEvent [for tables]
        emit currentFrameSetEditChanged();
    emit updateRuler();
}

void KWCanvas::editTextFrameSet( KWFrameSet * fs, KoTextParag* parag, int index )
{
    if ( selectAllFrames( false ) )
        emit frameSelectedChanged();
    //active header/footer when it's possible
    // DF: what is this code doing here?
    if ( fs->isAHeader() && !m_doc->isHeaderVisible() && !(viewMode()->type()=="ModeText"))
        m_doc->setHeaderVisible( true );
    if ( fs->isAFooter() && !m_doc->isFooterVisible() && !(viewMode()->type()=="ModeText"))
        m_doc->setFooterVisible( true );

    if ( !fs->isVisible( viewMode() ) )
        return;
    setMouseMode( MM_EDIT );
    bool emitChanged = checkCurrentEdit( fs );

    if ( m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet()->type()==FT_TEXT ) {
        if ( !parag )
        {
            KWTextDocument *tmp = static_cast<KWTextFrameSet*>(m_currentFrameSetEdit->frameSet())->kwTextDocument();
            parag = tmp->firstParag();
        }
        // The _new_ cursor position must be visible.
        KWTextFrameSetEdit *textedit=dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
        if ( textedit ) {
            textedit->hideCursor();
            textedit->setCursor( parag, index );
            textedit->showCursor();
            textedit->ensureCursorVisible();
        }
    }
    if ( emitChanged )
        emit currentFrameSetEditChanged();
    emit updateRuler();
}

void KWCanvas::ensureCursorVisible()
{
    Q_ASSERT( m_currentFrameSetEdit );
    if ( !m_currentFrameSetEdit )
        return;
    KWTextFrameSetEdit *textedit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
    textedit->ensureCursorVisible();
}

bool KWCanvas::checkCurrentEdit( KWFrameSet * fs , bool onlyText )
{
    bool emitChanged = false;
    if ( fs && m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() != fs )
    {
        KWTextFrameSet * tmp = dynamic_cast<KWTextFrameSet *>(fs );
        if ( tmp && tmp->protectContent() && !m_doc->cursorInProtectedArea() )
            return false;

        KWTextFrameSetEdit *edit=dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
        if(edit && onlyText)
        {
            // Don't use terminateCurrentEdit here, we want to emit changed only once
            //don't remove selection in dnd
            m_currentFrameSetEdit->terminate(false);
        }
        else
            m_currentFrameSetEdit->terminate();
        delete m_currentFrameSetEdit;
        m_currentFrameSetEdit = 0L;
        emitChanged = true;

    }

    // Edit the frameset "fs"
    if ( fs && !m_currentFrameSetEdit )
    {
        KWTextFrameSet * tmp = dynamic_cast<KWTextFrameSet *>(fs );
        if ( tmp && tmp->protectContent() && !m_doc->cursorInProtectedArea() )
            return false;
        // test for "text frameset only" if requested
        if(fs->type()==FT_TABLE || fs->type()==FT_TEXT || !onlyText)
        {
            if ( fs->type() == FT_TABLE )
                curTable = static_cast<KWTableFrameSet *>(fs);
            else if ( fs->type() == FT_TEXT )
                curTable = static_cast<KWTextFrameSet *>(fs)->getGroupManager();
            else
                curTable = 0L;
            if ( curTable ) {
                m_currentFrameSetEdit = curTable->createFrameSetEdit( this );
                static_cast<KWTableFrameSetEdit *>( m_currentFrameSetEdit )->setCurrentCell( fs );
            }
            else
                m_currentFrameSetEdit = fs->createFrameSetEdit( this );
        }
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
    QPtrListIterator<KWFrame> frameIt = fs->frameIterator();
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
        viewport()->setCursor( m_doc->getMouseCursor( normalPoint, 0 /*....*/ ) );
        m_frameInline = false;
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

void KWCanvas::insertPicture( const KoPicture& newPicture, QSize pixmapSize, bool _keepRatio )
{
    setMouseMode( MM_CREATE_PIX );
    m_kopicture = newPicture;
    m_pixmapSize = pixmapSize;
    if ( pixmapSize.isEmpty() )
        m_pixmapSize = newPicture.getOriginalSize();
    m_keepRatio = _keepRatio;
}

void KWCanvas::insertPart( const KoDocumentEntry &entry )
{
    m_partEntry = entry;
    if ( m_partEntry.isEmpty() )
    {
        setMouseMode( MM_EDIT );
        return;
    }
    setMouseMode( MM_CREATE_PART );
}

void KWCanvas::contentsDragEnterEvent( QDragEnterEvent *e )
{
    int provides = KWView::checkClipboard( e );
    if ( ( provides & KWView::ProvidesImage ) || KURLDrag::canDecode( e ) )
    {
        m_imageDrag = true;
        e->acceptAction();
    }
    else
    {
        m_imageDrag = false;
        if ( m_currentFrameSetEdit )
            m_currentFrameSetEdit->dragEnterEvent( e );
    }
}

void KWCanvas::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( !m_imageDrag /*&& m_currentFrameSetEdit*/ )
    {
        QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        KWFrame * frame = m_doc->frameUnderMouse( normalPoint );
        KWFrameSet * fs = frame ? frame->frameSet() : 0L;
        bool emitChanged = false;
        if ( fs )
        {
            //kdDebug()<<"table :"<<table<<endl;
            emitChanged = checkCurrentEdit( fs, true );
        }
        if ( m_currentFrameSetEdit )
        {
            m_currentFrameSetEdit->dragMoveEvent( e, normalPoint, docPoint );

            if ( emitChanged ) // emitted after mousePressEvent [for tables]
                emit currentFrameSetEditChanged();
        }
    }
}

void KWCanvas::contentsDragLeaveEvent( QDragLeaveEvent *e )
{
    if ( !m_imageDrag && m_currentFrameSetEdit )
        m_currentFrameSetEdit->dragLeaveEvent( e );
}

void KWCanvas::contentsDropEvent( QDropEvent *e )
{
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );

    if ( QImageDrag::canDecode( e ) ) {
        pasteImage( e, docPoint );
    } else if ( KURLDrag::canDecode( e ) ) {

        // TODO ask (with a popupmenu) between inserting a link and inserting the contents

        KURL::List lst;
        KURLDrag::decode( e, lst );

        KURL::List::ConstIterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            const KURL &url( *it );

            QString filename;
            if ( !KIO::NetAccess::download( url, filename, this ) )
                continue;

            KMimeType::Ptr res = KMimeType::findByFileContent( filename );

            if ( res && res->isValid() ) {
                QString mimetype = res->name();
                if ( mimetype.contains( "image" ) ) {
                    QImage i( filename );
                    m_pixmapSize = i.size();
                    // Prepare things for mrCreatePixmap
                    KoPictureKey key;
                    key.setKeyFromFile( filename );
                    KoPicture newKoPicture;
                    newKoPicture.setKey( key );
                    newKoPicture.loadFromFile( filename );
                    m_kopicture = newKoPicture;
                    m_insRect = KoRect( docPoint.x(), docPoint.y(), m_doc->unzoomItX( i.width() ), m_doc->unzoomItY( i.height() ) );
                    m_keepRatio = true;
                    mrCreatePixmap();
                }
            }
            KIO::NetAccess::removeTempFile( filename );
        }
    }
    else
    {
        if ( m_currentFrameSetEdit )
            m_currentFrameSetEdit->dropEvent( e, normalPoint, docPoint, m_gui->getView() );
        else
            m_gui->getView()->pasteData( e );
    }
    m_mousePressed = false;
    m_imageDrag = false;
}

void KWCanvas::pasteImage( QMimeSource *e, const KoPoint &docPoint )
{
    QImage i;
    QImageDrag::decode(e, i);
    KTempFile tmpFile( QString::null, ".png");
    tmpFile.setAutoDelete( true );
    i.save(tmpFile.name(), "PNG");
    m_pixmapSize = i.size();
    // Prepare things for mrCreatePixmap
    KoPictureKey key;
    key.setKeyFromFile( tmpFile.name() );
    KoPicture newKoPicture;
    newKoPicture.setKey( key );
    newKoPicture.loadFromFile( tmpFile.name() );
    m_kopicture = newKoPicture;
    m_insRect = KoRect( docPoint.x(), docPoint.y(), m_doc->unzoomItX( i.width() ), m_doc->unzoomItY( i.height() ) );
    m_keepRatio = true;
    mrCreatePixmap();
}

void KWCanvas::doAutoScroll()
{
    if ( !m_mousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    // This code comes from khtml
    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    pos = QPoint(pos.x() - viewport()->x(), pos.y() - viewport()->y());
    if ( (pos.y() < 0) || (pos.y() > visibleHeight()) ||
         (pos.x() < 0) || (pos.x() > visibleWidth()) )
    {
        int xm, ym;
        viewportToContents(pos.x(), pos.y(), xm, ym);
        if ( m_currentFrameSetEdit )
            m_currentFrameSetEdit->focusOutEvent(); // Hide cursor
        if ( m_deleteMovingRect )
            deleteMovingRect();
        ensureVisible( xm, ym, 0, 5 );
        if ( m_currentFrameSetEdit )
            m_currentFrameSetEdit->focusInEvent(); // Show cursor
    }
}

void KWCanvas::slotContentsMoving( int cx, int cy )
{
    //QPoint nPointTop = m_viewMode->viewToNormal( QPoint( cx, cy ) );
    QPoint nPointBottom = m_viewMode->viewToNormal( QPoint( cx + visibleWidth(), cy + visibleHeight() ) );
    //kdDebug() << "KWCanvas::slotContentsMoving cx=" << cx << " cy=" << cy << endl;
    //kdDebug() << " visibleWidth()=" << visibleWidth() << " visibleHeight()=" << visibleHeight() << endl;
    // Update our "formatted paragraphs needs" in all the text framesets
    QPtrList<KWTextFrameSet> textFrameSets = m_doc->allTextFramesets( false );
    QPtrListIterator<KWTextFrameSet> fit( textFrameSets );
    for ( ; fit.current() ; ++fit )
    {
        if(! fit.current()->isVisible()) continue;
        fit.current()->updateViewArea( this, m_viewMode, nPointBottom );
    }
    // cx and cy contain the future values for contentsx and contentsy, so we need to
    // pass them to updateRulerOffsets.
    updateRulerOffsets( cx, cy );
}

void KWCanvas::slotMainTextHeightChanged()
{
    // Check that the viewmode is a KWViewModeText, and that the rulers have been built already
    if ( dynamic_cast<KWViewModeText *>(m_viewMode) && m_gui->getHorzRuler() )
    {
        slotNewContentsSize();
        m_viewMode->setPageLayout( m_gui->getHorzRuler(), m_gui->getVertRuler(), KoPageLayout() /*unused*/ );
        emit updateRuler();
    }
}

void KWCanvas::slotNewContentsSize()
{
    QSize size = m_viewMode->contentsSize();
    if ( size != QSize( contentsWidth(), contentsHeight() ) )
    {
        //kdDebug() << "KWCanvas::slotNewContentsSize " << size.width() << "x" << size.height() << endl;
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

void KWCanvas::updateRulerOffsets( int cx, int cy )
{
    if ( cx == -1 && cy == -1 )
    {
        cx = contentsX();
        cy = contentsY();
    }
    // The offset is usually just the scrollview offset
    // But we also need to offset to the current page, for the graduations
    QPoint pc = m_viewMode->pageCorner(this);
    //kdDebug() << "KWCanvas::updateRulerOffsets contentsX=" << cx << ", contentsY=" << cy << endl;
    m_gui->getHorzRuler()->setOffset( cx - pc.x(), 0 );
    m_gui->getVertRuler()->setOffset( 0, cy - pc.y() );

}

bool KWCanvas::eventFilter( QObject *o, QEvent *e )
{
    if ( o == this || o == viewport() ) {

        if(m_currentFrameSetEdit && o == this )
        {
            // Pass event to auto-hide-cursor code (see kcursor.h for details)
            KCursor::autoHideEventFilter( o, e );
        }
        
        switch ( e->type() ) {
            case QEvent::FocusIn:
                //  kdDebug() << "KWCanvas::eventFilter QEvent::FocusIn" << endl;
                if ( m_currentFrameSetEdit && !m_printing )
                    m_currentFrameSetEdit->focusInEvent();
                break;
            case QEvent::FocusOut:
                //  kdDebug() << "KWCanvas::eventFilter QEvent::FocusOut" << endl;
                if ( m_currentFrameSetEdit && !m_printing )
                    m_currentFrameSetEdit->focusOutEvent();
                if ( m_scrollTimer->isActive() )
                    m_scrollTimer->stop();
                m_mousePressed = false;
                break;
            case QEvent::AccelOverride: // was part of KeyPress - changed due to kdelibs BUG!
            {
                //  kdDebug() << " KeyPress m_currentFrameSetEdit=" << m_currentFrameSetEdit << " isRW="<<m_doc->isReadWrite() << endl;
                //  kdDebug() << " m_printing=" << m_printing << " mousemode=" << m_mouseMode << " (MM_EDIT=" << MM_EDIT<<")"<<endl;
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
#ifndef NDEBUG
                // Debug keys
                if ( ( keyev->state() & ControlButton ) && ( keyev->state() & ShiftButton ) )
                {
                    switch ( keyev->key() ) {
                        case Qt::Key_P: // 'P' -> paragraph debug
                            printRTDebug( 0 );
                            keyev->accept();
                            break;
                        case Qt::Key_V: // 'V' -> verbose parag debug
                            printRTDebug( 1 );
                            keyev->accept();
                            break;
                        case Qt::Key_F: // 'F' -> frames debug
                            m_doc->printDebug();
                            kdDebug(32002) << "Current framesetedit: " << m_currentFrameSetEdit << " " <<
                                ( m_currentFrameSetEdit ? m_currentFrameSetEdit->frameSet()->className() : "" ) << endl;
                            keyev->accept();
                            break;
                        case Qt::Key_S: // 'S' -> styles debug
                            m_doc->printStyleDebug();
                            keyev->accept();
                            break;
                        case Qt::Key_M: // 'M' -> mark debug output
                            {
                                const QDateTime dtMark ( QDateTime::currentDateTime() );
                                kdDebug(32002) << "Developer mark: " << dtMark.toString("yyyy-MM-dd hh:mm:ss,zzz") << endl;
                                keyev->accept();
                                break;
                            }
                        default:
                            break;
                    };
                    // For some reason 'T' doesn't work (maybe kxkb)
                }
#endif
            }
            break;
            case QEvent::KeyPress:
            {
                //  kdDebug() << " KeyPress m_currentFrameSetEdit=" << m_currentFrameSetEdit << " isRW="<<m_doc->isReadWrite() << endl;
                //  kdDebug() << " m_printing=" << m_printing << " mousemode=" << m_mouseMode << " (MM_EDIT=" << MM_EDIT<<")"<<endl;
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
                // By default PgUp and PgDown move the scrollbars and not the caret anymore - this is done here
                if ( !m_doc->pgUpDownMovesCaret() && ( (keyev->state() & ShiftButton) == 0 )
                     && ( keyev->key() == Qt::Key_PageUp || keyev->key() == Key_PageDown ) )
                {
                    viewportScroll( keyev->key() == Qt::Key_PageUp );
                }
                else if ( keyev->key() == Qt::Key_Escape  )
                {
                    if ( m_mouseMode != MM_EDIT )
                        // Abort frame creation
                        setMouseMode( MM_EDIT );
                    else
                    {
                      if ( m_frameMoved && cmdMoveFrame)
                      {
                        cmdMoveFrame->unexecute();
                        delete cmdMoveFrame;
                        cmdMoveFrame = 0;
                        m_frameMoved = false;
                        m_mousePressed = false; //we don't want things to happen in KWCanvas::contentsMouseReleaseEvent
                        m_ctrlClickOnSelectedFrame = false;
                        m_gui->getView()->updateFrameStatusBarItem();
                        setMouseMode( MM_EDIT );
                      }
                      else if ( m_frameResized )
                      {
                        KWFrame *frame = m_doc->getFirstSelectedFrame();
                        if (!frame) { // can't happen, but never say never
                          kdWarning(32001) << "KWCanvas::eventFilter: no frame selected!" << endl;
                          return TRUE;
                        }
                        KWFrameSet* frameSet = frame->frameSet();
                        QRect oldRect = m_viewMode->normalToView( frame->outerRect(m_viewMode) );
                        frameSet->resizeFrameSetCoords( frame, m_boundingRect.left(), m_boundingRect.top(), m_boundingRect.right(), m_boundingRect.bottom(), false /*not final*/ );
                        delete cmdMoveFrame; // Unused after all
                        cmdMoveFrame = 0L;
                        m_frameResized = false;
                        m_mousePressed = false; //we don't want things to happen in KWCanvas::contentsMouseReleaseEvent
                        m_ctrlClickOnSelectedFrame = false;
                        // Move resize handles to new position
                        frame->updateResizeHandles();
                        // Calculate new rectangle for this frame
                        QRect newRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
                        // Repaint only the changed rects (oldRect U newRect)
                        repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), FALSE );
                        m_gui->getView()->updateFrameStatusBarItem();
                        setMouseMode( MM_EDIT );
                      }
                      else
                        selectAllFrames( false );
                    }
                }
                else // normal key processing
                    if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT && m_doc->isReadWrite() && !m_printing )
                {
                    KWTextFrameSetEdit *edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit );
                    if ( edit )
                    {
                        if ( !edit->textFrameSet()->textObject()->protectContent() || (keyev->text().length() == 0))
                            m_currentFrameSetEdit->keyPressEvent( keyev );
                        else if(keyev->text().length() > 0)
                            KMessageBox::information(this, i18n("Read-only content cannot be changed. No modifications will be accepted."));
                    }
                    else
                        m_currentFrameSetEdit->keyPressEvent( keyev );
                    return TRUE;
                }

                // Because of the dependency on the control key, we need to update the mouse cursor here
                if ( keyev->key() == Qt::Key_Control )
                {
                    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                    QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                    viewport()->setCursor( m_doc->getMouseCursor( normalPoint, keyev->stateAfter() ) );
                }
                else if ( (keyev->key() == Qt::Key_Delete || keyev->key() ==Key_Backspace )
                          && m_doc->getFirstSelectedFrame() && !m_printing )
                {
                    m_gui->getView()->editDeleteFrame();
                }
            } break;
            case QEvent::KeyRelease:
            {
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
                if ( keyev->key() == Qt::Key_Control )
                {
                    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                    QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                    viewport()->setCursor( m_doc->getMouseCursor( normalPoint, keyev->stateAfter() ) );
                }

                if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT && m_doc->isReadWrite() && !m_printing )
                {
                    m_currentFrameSetEdit->keyReleaseEvent( keyev );
                    return TRUE;
                }
            }
            break;
        case QEvent::IMStart:
        {
            QIMEvent * imev = static_cast<QIMEvent *>(e);
            m_currentFrameSetEdit->imStartEvent( imev );
        }
        break;
        case QEvent::IMCompose:
        {
            QIMEvent * imev = static_cast<QIMEvent *>(e);
            m_currentFrameSetEdit->imComposeEvent( imev );
        }
        break;
        case QEvent::IMEnd:
        {
            QIMEvent * imev = static_cast<QIMEvent *>(e);
            m_currentFrameSetEdit->imEndEvent( imev );
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
        textfs = dynamic_cast<KWTextFrameSet *>(m_currentFrameSetEdit->currentTextEdit()->frameSet());
    if ( !textfs )
        textfs = dynamic_cast<KWTextFrameSet *>(m_doc->frameSet( 0 ));
    if ( textfs )
        textfs->textObject()->printRTDebug( info );
}
#endif

void KWCanvas::setXimPosition( int x, int y, int w, int h )
{
    QWidget::setMicroFocusHint( x - contentsX(), y - contentsY(), w, h );
}

void KWCanvas::inlinePictureStarted()
{
    m_frameInline=true;
    m_frameInlineType=FT_PICTURE;
}

int KWCanvas::currentTableRow() const
{
    if ( !m_currentFrameSetEdit )
        return -1;
    KWTextFrameSetEdit *edit=dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
    if ( !edit )
        return -1;
    KWTextFrameSet* textfs = edit->textFrameSet();
    if ( textfs && textfs->getGroupManager() )
        return static_cast<KWTableFrameSet::Cell *>(textfs)->firstRow();
    return -1;
}

int KWCanvas::currentTableCol() const
{
    if ( !m_currentFrameSetEdit )
        return -1;
    KWTextFrameSetEdit *edit=dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
    if ( !edit )
        return -1;
    KWTextFrameSet* textfs = edit->textFrameSet();
    if ( textfs && textfs->getGroupManager() )
        return static_cast<KWTableFrameSet::Cell *>(textfs)->firstCol();
    return -1;
}

void KWCanvas::viewportScroll( bool up )
{
    if ( up )
        setContentsPos( contentsX(), contentsY() - visibleHeight() );
    else
        setContentsPos( contentsX(), contentsY() + visibleHeight() );
}

void KWCanvas::resetStatusBarText()
{
    if ( m_temporaryStatusBarTextShown )
    {
        gui()->getView()->updateFrameStatusBarItem();
        m_temporaryStatusBarTextShown = false;
    }
}

#include "KWCanvas.moc"
