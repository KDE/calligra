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

#include "kwcanvas.h"
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "kwdrag.h"
#include "framedia.h"
#include "kwcommand.h"
#include "kwtabletemplate.h"

#include <qtimer.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qobjectlist.h>

#include <ktempfile.h>
#include <klocale.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <assert.h>
#include "kwtextdocument.h"

KWCanvas::KWCanvas(KWViewMode* viewMode, QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent, "canvas", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase ), m_doc( d )
{
    m_gui = lGui;
    m_currentFrameSetEdit = 0L;
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
        m_currentFrameSetEdit = fs->createFrameSetEdit( this );

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
    m_doc->initBookmarkList();
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
}

void KWCanvas::repaintAll( bool erase /* = false */ )
{
    //kdDebug() << "KWCanvas::repaintAll erase=" << erase << endl;
    viewport()->repaint( erase );
}

void KWCanvas::print( QPainter *painter, KPrinter *printer )
{
    // Prevent cursor drawing and editing
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->focusOutEvent();
    m_printing = true;
    KWViewMode *viewMode = new KWViewModePrint( m_doc );
    //use page specify in kdeprint dialogbox
    QValueList<int> pageList=printer->pageList();
    int from = printer->fromPage();
    int to = printer->toPage();
    kdDebug(32001) << "KWCanvas::print from=" << from << " to=" << to << endl;
    /*
    if ( !from && !to ) // 0, 0 means everything
    {
        from = printer->minPage();
        to = printer->maxPage();
    }
    for ( int i = from; i <= to; i++ )
        pageList.append( i );
    */
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
    }
}

void KWCanvas::drawDocument( QPainter *painter, const QRect &crect, KWViewMode* viewMode )
{
    //kdDebug(32002) << "KWCanvas::drawDocument crect: " << DEBUGRECT( crect ) << endl;

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
    m_viewMode = newViewMode;
}

void KWCanvas::mpEditFrame( QMouseEvent *e, const QPoint &nPoint ) // mouse press in edit-frame mode
// This can be called by KWResizeHandle::mousePressEvent
{
    KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
    m_mousePressed = true;
    m_frameMoved = false;
    m_frameResized = false;
    m_ctrlClickOnSelectedFrame = false;

    if ( e )
    {
        KWFrame * frame = m_doc->frameUnderMouse( nPoint );
        KWFrameSet *fs = frame ? frame->frameSet() : 0;
        KWTableFrameSet *table= fs ? fs->getGroupManager() : 0;

        if ( fs && ( e->state() & ShiftButton ) && table ) { // is table and we hold shift
            KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
            table->selectUntil( docPoint.x(), docPoint.y() );
        }
        else if ( ( e->state() & ShiftButton ) && (m_doc->positionToSelectRowcolTable( nPoint, &table) != KWDocument::TABLE_POSITION_NONE) ) // we are in position to select full row/cells of a table + hold shift
        {
            KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
            table->selectUntil( table->boundingRect().right(), docPoint.y() );
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
        if ( frame->frameSet()->isHeaderOrFooter() )
            frame = frame->frameSet()->frame( 0 );
        m_resizedFrameInitialSize = frame->normalize();
    }

    QPtrList<KWFrame> selectedFrames = m_doc->getSelectedFrames();
    QPtrList<FrameIndex> frameindexList;
    QPtrList<FrameResizeStruct> frameindexMove;
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
                    KWTableFrameSet::Cell *c = table->getCell(theCell->m_row, col);
                    m_boundingRect |= *c->frame(0);
                }
            } else
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

    m_deleteMovingRect = false;
}

void KWCanvas::mpCreate( const QPoint& normalPoint )
{
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    applyGrid( docPoint );
    m_insRect.setCoords( docPoint.x(), docPoint.y(), 0, 0 );
    m_deleteMovingRect = false;
}

void KWCanvas::mpCreatePixmap( const QPoint& normalPoint )
{
    if ( !m_pictureFilename.isEmpty() )
    {
        // Apply grid for the first corner only
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        applyGrid( docPoint );
        m_insRect.setRect( docPoint.x(), docPoint.y(), 0, 0 );
        m_deleteMovingRect = false;

        // ### TODO: remove the m_isClipart (it is the only place where it is needed)
        if ( !m_isClipart && !m_pixmapSize.isEmpty() )
        {
            // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
            uint width = qRound( (double)m_pixmapSize.width() * m_doc->zoomedResolutionX() / POINT_TO_INCH( QPaintDevice::x11AppDpiX() ) );
            uint height = qRound( (double)m_pixmapSize.height() * m_doc->zoomedResolutionY() / POINT_TO_INCH( QPaintDevice::x11AppDpiY() ) );
            m_insRect.setWidth( m_doc->unzoomItX( width ) );
            m_insRect.setHeight( m_doc->unzoomItY( height ) );
            // Apply reasonable limits
            width = QMIN( width, m_doc->paperWidth() - normalPoint.x() - 5 );
            height = QMIN( height, m_doc->paperHeight()- normalPoint.y() - 5 );
            // And apply aspect-ratio if set
            if ( m_keepRatio )
            {
                double ratio = ((double) m_pixmapSize.width()) / ((double) m_pixmapSize.height());
                applyAspectRatio( ratio, m_insRect );
            }

            QPoint nPoint( normalPoint.x() + m_doc->zoomItX( width ),
                           normalPoint.y() + m_doc->zoomItY( height ) );
            QPoint vPoint = m_viewMode->normalToView( nPoint );
            vPoint = contentsToViewport( vPoint );
            QRect viewportRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
            if ( viewportRect.contains( vPoint ) ) // Don't move the mouse out of the viewport
                QCursor::setPos( viewport()->mapToGlobal( vPoint ) );
        }
        emit docStructChanged(Pictures); // ### TODO: inside ot outside the "if"?
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
    if ( m_printing )
        return;

    // This code here is common to all mouse buttons, so that RMB and MMB place the cursor (or select the frame) too
    switch ( m_mouseMode ) {
    case MM_EDIT:
    {
        // See if we clicked on a frame's border
        bool border = false;
        KWFrame * frame = m_doc->frameUnderMouse( normalPoint, &border );
        bool selectedFrame = m_doc->getFirstSelectedFrame() != 0L;
        // Frame border, or pressing Control or pressing Shift and a frame has already been selected
        // [We must keep shift+click for selecting text, when no frame is selected]
        if ( ( frame && border )
             || e->state() & ControlButton
             || ( ( e->state() & ShiftButton ) && selectedFrame ) )
        {
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();
            mpEditFrame( e, normalPoint );
        }
        else
        {
            if ( selectAllFrames( false ) )
                emit frameSelectedChanged();

            KWFrameSet * fs = frame ? frame->frameSet() : 0L;
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

            if( m_frameInline)
            {
                bool inlineCreated = true;
                if(m_frameInlineType==FT_TABLE)
                    inlineCreated = insertInlineTable();
                else if(m_frameInlineType==FT_PICTURE)
                    inlineCreated = m_gui->getView()->insertInlinePicture();
                if ( inlineCreated)
                    m_frameInline=false;
                else
                    KMessageBox::information(0L, i18n("Read-only content cannot be changed. No modifications will be accepted"));
            }
        }

        KWTableFrameSet *table = 0L;
        KWDocument::TableToSelectPosition ePositionTable = m_doc->positionToSelectRowcolTable( normalPoint, &table);
        // are we in the situation to select row/cols of a table?
        if (ePositionTable != KWDocument::TABLE_POSITION_NONE)
        {   // YES => select row/col
            if (ePositionTable == KWDocument::TABLE_POSITION_RIGHT)
            {  // in position to select a ROW
                // here the cursor is on the left of the table. the y is OK, but the x is not,
                // hence finding a proper x with the table object
                KWTableFrameSet::Cell *cell = table->getCellByPos( table->leftWithoutBorder(), m_doc->unzoomItY(normalPoint.y())  );
                if (cell)
                {
                    table->selectRow( cell->getRow() );
                    curTable = table;
                    emit frameSelectedChanged();
                }
            }
            else
            { // in position to select a COLUMN
              // here the cursor is on top of the table. the x is ok, but the y is not.
                KWTableFrameSet::Cell *cell = table->getCellByPos( m_doc->unzoomItX(normalPoint.x()), table->topWithoutBorder()  );
                if (cell)
                {
                    table->selectCol( cell->getColumn() );
                    curTable = table;
                    emit frameSelectedChanged();
                }
            }
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
                if ( ( frame && border ) || e->state() & ControlButton )
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

    m_table.tableTemplateName = tt ? tt->translatedName():QString::null;
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
    // This one is called by KWResizeHandle
    KWFrame *frame = m_doc->getFirstSelectedFrame();
    if (!frame) { // can't happen, but never say never
        kdWarning(32001) << "KWCanvas::mmEditFrameResize: no frame selected!" << endl;
        return;
    }
    //kdDebug() << "KWCanvas::mmEditFrameResize top,bottom,left,right: "
    //          << top << "," << bottom << "," << left << "," << right << endl;

    // Get the mouse position from QCursor. Trying to get it from KWResizeHandle's
    // mouseMoveEvent leads to the frame 'jumping' because the events are received async.
    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
    mousep = m_viewMode->viewToNormal( mousep );

    KoPoint docPoint = m_doc->unzoomPoint( mousep );
    // Apply the grid, unless Shift is pressed
    if ( !noGrid )
        applyGrid( docPoint );
    double x = docPoint.x();
    double y = docPoint.y();
    int page = static_cast<int>( y / m_doc->ptPaperHeight() );
    int oldPage = static_cast<int>( frame->top() / m_doc->ptPaperHeight() );
    Q_ASSERT( oldPage == frame->pageNum() );

    // Calculate new frame coordinates, using minimum sizes, and keeping it in the bounds of the page
    double newLeft = frame->left();
    double newTop = frame->top();
    double newRight = frame->right();
    double newBottom = frame->bottom();
    KWFrameSet* frameSet = frame->frameSet();
    if ( page == oldPage )
    {
        //kdDebug() << "KWCanvas::mmEditFrameResize old rect " << DEBUGRECT( *frame ) << endl;

        if ( top && newTop != y )
        {
            if (newBottom - y < minFrameHeight+5)
                y = newBottom - minFrameHeight - 5;
            y = QMAX( y, m_doc->ptPageTop( oldPage ) );
            newTop = y;
        } else if ( bottom && newBottom != y )
        {
            if (y - newTop < minFrameHeight+5)
                y = newTop + minFrameHeight + 5;
            y = QMIN( y, m_doc->ptPageTop( oldPage + 1 ) );
            newBottom = y;
        }

        if ( left && newLeft != x )
        {
            if (newRight - x < minFrameWidth)
                x = newRight - minFrameWidth - 5;
            x = QMAX( x, 0 );
            newLeft = x;
        } else if ( right && newRight != x )
        {
            if (x - newLeft < minFrameWidth)
                x = newLeft + minFrameWidth + 5; // why +5 ?
            x = QMIN( x, m_doc->ptPaperWidth() );
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
        QRect oldRect = m_viewMode->normalToView( frame->outerRect() );

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
        QRect newRect( m_viewMode->normalToView( frame->outerRect() ) );
        // Repaint only the changed rects (oldRect U newRect)
        repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), FALSE );
        m_frameResized = true;

        m_gui->getView()->updateFrameStatusBarItem();
    }
}

void KWCanvas::applyGrid( KoPoint &p )
{
    // The 1e-10 here is a workaround for some weird division problem.
    // 360.00062366 / 2.83465058 gives 127 'exactly' when shown as a double,
    // but when casting into an int, we get 126. In fact it's 127 - 5.64e-15 !

    // This is a problem when calling applyGrid twice, we get 1 less than the time before.
    p.setX( static_cast<int>( p.x() / m_doc->gridX() + 1e-10 ) * m_doc->gridX() );
    p.setY( static_cast<int>( p.y() / m_doc->gridY() + 1e-10 ) * m_doc->gridY() );
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
    else if ( m_boundingRect.right() > m_doc->ptPaperWidth() - 1 )
    {
        p.setX( m_doc->ptPaperWidth() - m_boundingRect.width() - 2 );
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
    else if ( m_boundingRect.bottom() > m_doc->getPages() * m_doc->ptPaperHeight() - 1 )
    {
        //kdDebug() << "KWCanvas::mmEditFrameMove limiting to last page" << endl;
        p.setY( m_doc->getPages() * m_doc->ptPaperHeight() - m_boundingRect.height() - 2 );
        m_boundingRect.moveTopLeft( p );
    }
    // Another annoying case is if the top and bottom points are not in the same page....
    int topPage = static_cast<int>( m_boundingRect.top() / m_doc->ptPaperHeight() );
    int bottomPage = static_cast<int>( m_boundingRect.bottom() / m_doc->ptPaperHeight() );
    //kdDebug() << "KWCanvas::mmEditFrameMove topPage=" << topPage << " bottomPage=" << bottomPage << endl;
    if ( topPage != bottomPage )
    {
        // Choose the closest page...
        Q_ASSERT( topPage + 1 == bottomPage ); // Not too sure what to do otherwise
        double topPart = (bottomPage * m_doc->ptPaperHeight()) - m_boundingRect.top();
        if ( topPart > m_boundingRect.height() / 2 )
            // Most of the rect is in the top page
            p.setY( bottomPage * m_doc->ptPaperHeight() - m_boundingRect.height() - 1 );
        else
            // Most of the rect is in the bottom page
            p.setY( bottomPage * m_doc->ptPaperHeight() + 5 /* grmbl, resize handles.... */ );
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
                    QRect oldRect( m_viewMode->normalToView( frame->outerRect() ) );
                    // Move the frame
                    frame->moveTopLeft( frame->topLeft() + _move );
                    // Calculate new rectangle for this frame
                    QRect newRect( frame->outerRect() );

                    QRect frameRect( m_viewMode->normalToView( newRect ) );
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
                KWFrame * frame = table->getCell( k )->frame( 0 );
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
    //m_doc->updateAllFrames();
    // Not yet in fact. If we relayout the text everytime it's too slow.

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

    int page = m_doc->getPageOfRect( m_insRect );
    KoRect oldRect = m_insRect;

    // Resize the rectangle
    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
    if ( m_mouseMode != MM_CREATE_PIX && !shiftPressed )
        applyGrid( docPoint );

    m_insRect.setRight( docPoint.x() );
    m_insRect.setBottom( docPoint.y() );

    // But not out of the page
    KoRect r = m_insRect.normalize();
    if ( m_doc->isOutOfPage( r, page ) )
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
                    if ( viewport()->cursor().shape() == SizeAllCursor )
                        mmEditFrameMove( normalPoint, e->state() & ShiftButton );
                }
            } break;
            case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
            case MM_CREATE_TABLE: case MM_CREATE_FORMULA:
                mmCreate( normalPoint, e->state() & ShiftButton );
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
                FrameResizeStruct tmpResize;
                tmpResize.sizeOfBegin = m_resizedFrameInitialSize;
                tmpResize.sizeOfEnd = frame->normalize();

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
                int i = 0;
                for(KWFrame * frame=selectedFrames.first(); frame; frame=selectedFrames.next() )
                {
                    KWFrameSet * fs = frame->frameSet();
                    if ( !(m_doc->processingType() == KWDocument::WP && m_doc->frameSetNum( fs ) == 0 )&& !fs->isAHeader() && !fs->isAFooter()  )
                    {

                        cmdMoveFrame->listFrameMoved().at(i)->sizeOfEnd = frame->normalize();
                        i++;
                    }
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

void KWCanvas::mrCreateText()
{
    m_insRect = m_insRect.normalize();
    if ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) {
        KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frame->setNewFrameBehavior(KWFrame::Reconnect);
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        KWFrameDia frameDia( this, frame, m_doc, FT_TEXT );
        frameDia.setCaption(i18n("Connect Frame"));
        frameDia.exec();
	checkCurrentEdit(frame->frameSet(), true);
    }
    setMouseMode( MM_EDIT );
    m_doc->repaintAllViews();
    emit docStructChanged(TextFrames);
    emit currentFrameSetEditChanged();
}

void KWCanvas::mrCreatePixmap()
{
    //kdDebug() << "KWCanvas::mrCreatePixmap m_insRect=" << DEBUGRECT(m_insRect) << endl;
    // Make sure it's completely on page.
    if(m_insRect.right() > m_doc->ptPaperWidth()) {
        double width = m_insRect.width();
        m_insRect.setLeft(m_doc->ptPaperWidth() - width);
        m_insRect.setRight(m_doc->ptPaperWidth());
    }
    int page = static_cast<int>(m_insRect.top() / m_doc->ptPaperHeight()) + 1;
    //kdDebug() << "page: " << page << endl;

    if(m_insRect.bottom() > m_doc->ptPaperHeight() * page) {
        double height = m_insRect.height();
        m_insRect.setTop(m_doc->ptPaperHeight() * page - height);
        m_insRect.setBottom(m_doc->ptPaperHeight() * page);
    }

    if ( m_insRect.width() > 0 /*m_doc->gridX()*/ && m_insRect.height() > 0 /*m_doc->gridY()*/ && !m_pictureFilename.isEmpty() )
    {
        KWFrameSet * fs = 0L;
        KWPictureFrameSet *frameset = new KWPictureFrameSet( m_doc, QString::null /*automatic name*/ );
        frameset->loadPicture( m_pictureFilename );
        frameset->setKeepAspectRatio( m_keepRatio ); // For a clipart, this has almost no effect
        fs = frameset;
        m_insRect = m_insRect.normalize();
        KWFrame *frame = new KWFrame(fs, m_insRect.x(), m_insRect.y(), m_insRect.width(),
                                     m_insRect.height() );
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        fs->addFrame( frame, false );
        m_doc->addFrameSet( fs );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a Picture Frame"), frame );
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
        frame->setZOrder( m_doc->maxZOrder( frame->pageNum(m_doc) ) + 1 ); // make sure it's on top
        frameset->addFrame( frame, false );
        m_doc->addFrameSet( frameset );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a Formula Frame"), frame );
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
    int pageNum = static_cast<int>(m_insRect.y() / m_doc->ptPaperHeight());

    // Create a set of cells with random-size frames.
    for ( unsigned int i = 0; i < m_table.rows; i++ ) {
        for ( unsigned int j = 0; j < m_table.cols; j++ ) {
            KWTableFrameSet::Cell *cell = new KWTableFrameSet::Cell( table, i, j, QString::null /*automatic name*/ );
            KWFrame *frame = new KWFrame(cell, 0, 0, 0, 0, KWFrame::RA_NO ); // pos and size will be set in setBoundingRect
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
            m_insRect.setLeft(QMIN(m_insRect.left(), m_doc->ptPaperWidth() - 200));
            m_insRect.setTop(QMIN(m_insRect.top(), m_doc->ptPaperHeight() - 150));
            m_insRect.setBottom(m_insRect.top()+150);
            m_insRect.setRight(m_insRect.left()+200);
        }
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
    if ( m_printing )
        return;
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
    if (selectedFrames.count() == 0)
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

        if (_backColor!=frame->backgroundColor())
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


void KWCanvas::editFrameProperties()
{
    QPtrList<KWFrame> frames=m_doc->getSelectedFrames();
    if(frames.count()==0) return;
    KWFrameDia *frameDia;
    if(frames.count()==1) {
        KWFrame *frame = frames.first();
        frameDia = new KWFrameDia( this, frame );
        frameDia->setCaption(i18n("Frame Properties"));
    } else { // multi frame dia.
        frameDia = new KWFrameDia( this, frames );
        frameDia->setCaption(i18n("Frames Properties"));
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
        if(! fs->isVisible()) continue;
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

    // We really need a selected-frames-list !
    QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        if ( fs->isVisible() )
        {
            bool isTable = ( fs->type() == FT_TABLE );
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
                    }
                    foundOne = true;
                    if ( isTable ) // Copy tables only once, even if they have many cells selected
                        break;
                }
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
    m_doc->pasteFrames( topElem, macroCmd );
    m_doc->addCommand( macroCmd );
}

KWTableFrameSet *KWCanvas::getTable()
{
    if( !m_currentFrameSetEdit)
        return 0L;

    if(m_currentFrameSetEdit->frameSet()->type() == FT_TABLE)
        return static_cast<KWTableFrameSet *> (m_currentFrameSetEdit->frameSet());

    return 0L;
}

void KWCanvas::editFrameSet( KWFrameSet * frameSet, bool onlyText )
{
    if ( selectAllFrames( false ) )
        emit frameSelectedChanged();

    bool emitChanged = false;
    KWTableFrameSet *table = frameSet->getGroupManager();
    emitChanged = checkCurrentEdit( table ? table : frameSet, onlyText );

    if ( emitChanged ) // emitted after mousePressEvent [for tables]
        emit currentFrameSetEditChanged();
    emit updateRuler();
}

void KWCanvas::editTextFrameSet( KWFrameSet * fs, KoTextParag* parag, int index , bool forceEdit)
{
    if ( selectAllFrames( false ) )
        emit frameSelectedChanged();
    //active header/footer when it's possible
    if ( fs->isAHeader() && !m_doc->isHeaderVisible() && !(viewMode()->type()=="ModeText"))
        m_doc->setHeaderVisible( true );
    if ( fs->isAFooter() && !m_doc->isFooterVisible() && !(viewMode()->type()=="ModeText"))
        m_doc->setFooterVisible( true );

    if ( !fs->isVisible( viewMode() ) )
        return;
    KWTableFrameSet *table = fs->getGroupManager();
    bool emitChanged = checkCurrentEdit( table ? table : fs );

    if ( emitChanged || forceEdit) { // emitted after mousePressEvent [for tables]
        if ( m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet()->type()==FT_TEXT ) {
            if ( !parag )
            {
                KWTextDocument *tmp = static_cast<KWTextFrameSet*>(m_currentFrameSetEdit->frameSet())->kwTextDocument();
                parag = tmp->firstParag();
            }
            static_cast<KWTextFrameSetEdit*>( m_currentFrameSetEdit )->setCursor( parag, index );

            // The _new_ cursor position must be visible.
            KWTextFrameSetEdit *textedit=dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
            if ( textedit )
                textedit->ensureCursorVisible();
        }
        emit currentFrameSetEditChanged();
    }
    emit updateRuler();
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

        //just text frameset
        if(fs->type()==FT_TABLE || fs->type()==FT_TEXT || !onlyText)
        {
            m_currentFrameSetEdit = fs->createFrameSetEdit( this );
        }
        // Display page number in statusbar.
        gui()->getView()->updatePageInfo();
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

void KWCanvas::insertPicture( const QString & filename, bool isClipart, QSize pixmapSize, bool _keepRatio )
{
    setMouseMode( MM_CREATE_PIX );
    m_pictureFilename = filename;
    m_isClipart = isClipart;
    m_pixmapSize = pixmapSize;
    /// ### FIXME: how is the following code supposed to work with cliparts?
    if( pixmapSize.isEmpty()) m_pixmapSize = QPixmap( filename ).size();
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
    bool providesImage, providesKWordText, providesKWord, providesFormula;
    KWView::checkClipboard( e, providesImage, providesKWordText, providesKWord, providesFormula );
    if ( providesImage )
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
            KWTableFrameSet *table = fs->getGroupManager();
            //kdDebug()<<"table :"<<table<<endl;
            emitChanged = checkCurrentEdit( table ? table : fs,true );
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
    if ( m_imageDrag )
    {
        pasteImage( e, docPoint );
    }
    else if ( m_currentFrameSetEdit )
    {
        m_currentFrameSetEdit->dropEvent( e, normalPoint, docPoint );
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
    // Prepare things for mrCreatePixmap
    m_pictureFilename = tmpFile.name();
    m_isClipart = false;
    m_pixmapSize = i.size();
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
    // Update our "formatted paragraphs needs" in the text framesets
    ///////////////// TODO: use allTextFramesets for nested text framesets
    QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        if(! fit.current()->isVisible()) continue;
        KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>(fit.current());
        if ( fs )
        {
            fs->updateViewArea( this, m_viewMode, nPointBottom );
        }
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
    if ( !o || !e )
        return TRUE;

    if ( o == this || o == viewport() ) {

        if(m_currentFrameSetEdit)
        {
            // Pass event to auto-hide-cursor code (see kcursor.h for details)
            KCursor::autoHideEventFilter( o, e );
        }

        switch ( e->type() ) {
            case QEvent::FocusIn:
		//  kdDebug() << "KWCanvas::eventFilter QEvent::FocusIn" << endl;
                if ( m_currentFrameSetEdit && !m_printing )
                    m_currentFrameSetEdit->focusInEvent();
                return TRUE;
            case QEvent::FocusOut:
		//  kdDebug() << "KWCanvas::eventFilter QEvent::FocusOut" << endl;
                if ( m_currentFrameSetEdit && !m_printing )
                    m_currentFrameSetEdit->focusOutEvent();
                if ( m_scrollTimer->isActive() )
                    m_scrollTimer->stop();
                m_mousePressed = false;
                return TRUE;
            case QEvent::KeyPress:
            {
		//  kdDebug() << " KeyPress m_currentFrameSetEdit=" << m_currentFrameSetEdit << " isRW="<<m_doc->isReadWrite() << endl;
		//  kdDebug() << " m_printing=" << m_printing << " mousemode=" << m_mouseMode << " (MM_EDIT=" << MM_EDIT<<")"<<endl;
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
                            kdDebug(32002) << "Current framesetedit: " << m_currentFrameSetEdit << " " <<
                                ( m_currentFrameSetEdit ? m_currentFrameSetEdit->frameSet()->className() : "" ) << endl;
                            break;
                        case Key_S: // 'S' -> styles debug
                            m_doc->printStyleDebug();
                            break;
                        default:
                            break;
                    };
                    // For some reason 'T' doesn't work (maybe kxkb)
                }
#endif
                // By default PgUp and PgDown move the scrollbars and not the caret anymore - this is done here
                if ( !m_doc->pgUpDownMovesCaret() && ( (keyev->state() & ShiftButton) == 0 )
                     && ( keyev->key() == Key_PageUp || keyev->key() == Key_PageDown ) )
                {
                    if ( keyev->key() == Key_PageUp )
                        setContentsPos( contentsX(), contentsY() - visibleHeight() );
                    else // Key_PageDown
                        setContentsPos( contentsX(), contentsY() + visibleHeight() );
                }
                else if ( keyev->key() == Key_Escape && m_mouseMode != MM_EDIT )
                {
                    // Abort frame creation
                    setMouseMode( MM_EDIT );
                }
                else // normal key processing
                    if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT && m_doc->isReadWrite() && !m_printing )
                {
                    KWTextFrameSetEdit *edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit );
                    if ( edit )
                    {
                        if ( !edit->textFrameSet()->textObject()->protectContent() )
                            m_currentFrameSetEdit->keyPressEvent( keyev );
                        else if(keyev->text().length() > 0)
                            KMessageBox::information(this, i18n("Read-only content cannot be changed. No modifications will be accepted."));
                    }
                    else
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
                else if ( (keyev->key() == Key_Delete || keyev->key() ==Key_Backspace )
                          && m_doc->getFirstSelectedFrame() && !m_printing )
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

                if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT && m_doc->isReadWrite() && !m_printing )
                {
                    m_currentFrameSetEdit->keyReleaseEvent( keyev );
                    return TRUE;
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

#include "kwcanvas.moc"
