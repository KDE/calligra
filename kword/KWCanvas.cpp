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
#include "KWPictureFrameSet.h"
#include "KWFrameView.h"
#include "KWFrameViewManager.h"

#include <qbuffer.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qobjectlist.h>
#include <qwhatsthis.h>

#include <KoStore.h>
#include <KoStoreDrag.h>
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

KWCanvas::KWCanvas(const QString& viewMode, QWidget *parent, KWDocument *d, KWGUI *lGui)
    : QScrollView( parent, "canvas", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase ), m_doc( d )
{
    m_frameViewManager = new KWFrameViewManager(d);
    m_gui = lGui;
    m_currentFrameSetEdit = 0L;
    m_mouseMeaning = MEANING_NONE;
    m_mousePressed = false;
    m_imageDrag = false;
    m_frameInline = false;

    //used by insert picture dialogbox
    m_picture.pictureInline = false;
    m_picture.keepRatio = true;



    m_frameInlineType = FT_TABLE;
    m_viewMode = KWViewMode::create( viewMode, m_doc, this );
    m_moveFrameCommand = 0;

    // Default table parameters.
    m_table.rows = 3;
    m_table.cols = 2;
    m_table.width = KWTableFrameSet::TblAuto;
    m_table.height = KWTableFrameSet::TblAuto;
    m_table.floating = true;
    m_table.tableTemplateName=QString::null;
    m_table.format=31;

    m_footEndNote.noteType = FootNote;
    m_footEndNote.numberingType = KWFootNoteVariable::Auto;


    m_currentTable = 0L;
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
    delete m_moveFrameCommand;
    delete m_currentFrameSetEdit;
    m_currentFrameSetEdit = 0;
    delete m_viewMode;
    m_viewMode = 0;
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
    viewport()->update();
}

void KWCanvas::print( QPainter *painter, KPrinter *printer )
{
    // Prevent cursor drawing and editing
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->focusOutEvent();
    m_printing = true;
    KWViewMode *viewMode = new KWViewModePrint( m_doc, this );
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
        int pgNum = (*it);
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
    else if ( m_doc->snapToGrid() && ( m_frameMoved || m_frameResized || m_mouseMode != MM_EDIT ) )
      drawGrid( *painter, rect() );
  }
}

void KWCanvas::drawDocument( QPainter *painter, const QRect &crect, KWViewMode* viewMode )
{
    kdDebug(32002) << "KWCanvas::drawDocument crect: " << crect << endl;

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
        m_currentFrameSetEdit->drawContents( painter, crect, gb, onlyChanged, resetChanged, viewMode, m_frameViewManager );
    else
        frameset->drawContents( painter, crect, gb, onlyChanged, resetChanged, 0L, viewMode, m_frameViewManager );
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

void KWCanvas::switchViewMode( const QString& newViewMode )
{
    delete m_viewMode;
    m_viewMode = KWViewMode::create( newViewMode, m_doc, this );
}

void KWCanvas::mpEditFrame( const QPoint &nPoint, MouseMeaning meaning, QMouseEvent *event ) // mouse press in edit-frame mode
{
    KoPoint docPoint( m_doc->unzoomPoint( nPoint ) );
    m_mouseMeaning = meaning;
    m_mousePressed = true;
    m_frameMoved = false;
    m_frameResized = false;
    m_ctrlClickOnSelectedFrame = false;

    if ( event && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) )
        m_frameViewManager->selectFrames(docPoint, event->state(), event->button() == Qt::LeftButton );

    // At least one frame selected ?
    KWFrameView *view = m_frameViewManager->selectedFrame();
    if( view )
    {
        KWFrame *frame = view->frame();
        // If header/footer, resize the first frame
        if ( frame->frameSet()->isHeaderOrFooter() )
            frame = frame->frameSet()->frame( 0 );
        m_resizedFrameInitialSize = frame->normalize();
        m_resizedFrameInitialMinHeight = frame->minFrameHeight();
    }

    QValueList<FrameIndex> frameindexList;
    QValueList<FrameMoveStruct> frameindexMove;
    QValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
    QValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    m_boundingRect = KoRect();
    // When moving many frames, we look at the bounding rect.
    // It's the one that will be checked against the limits, etc.
    while(framesIterator != selectedFrames.end()) {
        KWFrame *frame=(*framesIterator)->frame();
        KWFrameSet * fs = frame->frameSet();
        if ( !(m_doc->processingType() == KWDocument::WP && m_doc->frameSetNum( fs ) == 0 )&& !fs->isAHeader() && !fs->isAFooter()  )
        {
            // If one cell belongs to a table, we are in fact moving the whole table
            KWTableFrameSet *table = fs->groupmanager();
            // We'll have to do better in the long run
            if ( table ) {
                KWTableFrameSet::Cell *theCell=static_cast<KWTableFrameSet::Cell *>(fs);
                for(unsigned int col=0; col < table->getColumns(); col++) {
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
        ++framesIterator;
    }
    m_hotSpot = docPoint - m_boundingRect.topLeft();
    if(frameindexMove.count()!=0)
    {
        delete m_moveFrameCommand;
        m_moveFrameCommand = new KWFrameMoveCommand( i18n("Move Frame"), frameindexList, frameindexMove );
    }

    viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, event ? event->state() : 0 ) );

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
        if ( m_doc->snapToGrid() )
            applyGrid( docPoint );
        int pageNum = m_doc->pageManager()->pageNumber( docPoint );
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
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
          repaintContents( FALSE ); //draw the grid over the whole canvas
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
        m_mouseMeaning = m_frameViewManager->mouseMeaning( docPoint, e->state());
        //kdDebug(32001) << "contentsMousePressEvent meaning=" << m_mouseMeaning << endl;
        switch ( m_mouseMeaning )  {
        case MEANING_MOUSE_MOVE:
        case MEANING_SELECT_ROW:
        case MEANING_SELECT_RANGE:
        case MEANING_SELECT_COLUMN:
        case MEANING_FORBIDDEN:
        case MEANING_MOUSE_SELECT:
        {
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();
            mpEditFrame( normalPoint, m_mouseMeaning, e );
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
            KWFrameView *view = m_frameViewManager->view( docPoint, KWFrameViewManager::frameOnTop );
            if ( ! ( e->button() == RightButton && view && view->selected() ) )
                selectAllFrames( false );

            KWFrame * frame = view ? view->frame() : 0L;
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
            KWFrameView *view = m_frameViewManager->view(docPoint, KWFrameViewManager::frameOnTop);
            if (view)
            {
                KWTableFrameSet::Cell* cell = dynamic_cast<KWTableFrameSet::Cell *>(view->frame()->frameSet());
                if ( cell )
                {
                    KWTableFrameSet* table = cell->groupmanager();
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
                    m_currentTable = table;
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
            mpEditFrame( normalPoint, m_mouseMeaning, e );
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
            if ( !viewMode()->hasFrames() ) { // text view mode
                KWFrameView *view = m_frameViewManager->view(m_doc->frameSet( 0 )->frame(0));
                view->showPopup(docPoint, m_gui->getView(), QCursor::pos());
            }
            else
                m_frameViewManager->showPopup(docPoint, m_gui->getView(), e->state(), QCursor::pos());
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
    kdDebug() << "mmEditFrameResize " << top << " " << bottom << " " << left << " " << right << endl;
    KWFrameView *view = m_frameViewManager->selectedFrame();
    KWFrame *frame = view == 0 ? 0 : view->frame();
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
    int page = m_doc->pageManager()->pageNumber(docPoint);
    int oldPage = m_doc->pageManager()->pageNumber(*frame);

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
        if (frameSet->groupmanager()) { // is table
            if (!(top || bottom)) { /// full height.
                drawY=frameSet->groupmanager()->getBoundingRect().y();
                drawHeight=frameSet->groupmanager()->getBoundingRect().height();
            } else if (!(left || right)) { // full width.
                drawX=frameSet->groupmanager()->getBoundingRect().x();
                drawWidth=frameSet->groupmanager()->getBoundingRect().width();
            }
        }
        //p.drawRect( drawX, drawY, drawWidth, drawHeight );
        //p.end();
#endif

        // Move resize handles to new position
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
          repaintContents( FALSE ); //draw the grid over the whole canvas
        else
        {
          // Calculate new rectangle for this frame
          QRect newRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
          // Repaint only the changed rects (oldRect U newRect)
          repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), FALSE );
        }
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
    int page = pageManager->pageNumber(docPoint);
    if(page == -1) // moving left or right out of the page.
        return;
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
    //          << " (" << DEBUGDOUBLE( KoUnit::toMM( p.x() ) ) << " mm)" << endl;
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
    //          << " (" << DEBUGDOUBLE( KoUnit::toMM( p.x() ) ) << " mm)" << endl;
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
    int topPage = pageManager->pageNumber( m_boundingRect.topLeft() );
    int bottomPage = pageManager->pageNumber( m_boundingRect.bottomRight() );
    //kdDebug() << "KWCanvas::mmEditFrameMove topPage=" << topPage << " bottomPage=" << bottomPage << endl;
    if ( topPage != bottomPage )
    {
        // Choose the closest page...
        Q_ASSERT( bottomPage == -1 || topPage + 1 == bottomPage ); // Not too sure what to do otherwise
        double topPart = m_boundingRect.bottom() - pageManager->bottomOfPage(topPage);
        if ( bottomPage == -1 || topPart < m_boundingRect.height() / 2 )
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
    QRegion repaintRegion;
    KoPoint _move=m_boundingRect.topLeft() - oldBoundingRect.topLeft();

    QValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
    QValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    for(; framesIterator != selectedFrames.end(); ++framesIterator) {
        KWFrame *frame = (*framesIterator)->frame();
        KWFrameSet *fs = frame->frameSet();
        if(!fs->isVisible()) continue;
        if(fs->isMainFrameset() ) continue;
        if(fs->isFloating() ) continue;
        if(fs->isProtectSize() ) continue;
        if(fs->type() == FT_TEXT && fs->frameSetInfo() != KWFrameSet::FI_BODY ) continue;

        m_frameMoved = true;
        if ( fs->type() == FT_TABLE ) {
            if ( tablesMoved.findRef( static_cast<KWTableFrameSet *> (fs) ) == -1 )
                tablesMoved.append( static_cast<KWTableFrameSet *> (fs));
        }
        else {
            QRect oldRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
            // Move the frame
            frame->moveTopLeft( frame->topLeft() + _move );
            // Calculate new rectangle for this frame
            QRect newRect( frame->outerRect(m_viewMode) );

            QRect frameRect( m_viewMode->normalToView( newRect ) );
            // Repaint only the changed rects (oldRect U newRect)
            repaintRegion += QRegion(oldRect).unite(frameRect).boundingRect();
            // Move resize handles to new position
            if(frame->frameStack())
                frame->frameStack()->update();
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
            }
        }
    }

    if ( !m_doc->showGrid() && m_doc->snapToGrid() )
      repaintContents( FALSE ); //draw the grid over the whole canvas
    else
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

    int page = m_doc->pageManager()->pageNumber( m_insRect );
    if( page == -1)
        return;
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
                        QRect oldRect( m_viewMode->normalToView( m_doc->zoomRect( m_currentTable->boundingRect() ) ) );
                        if ( m_mouseMeaning == MEANING_RESIZE_ROW )
                            m_currentTable->resizeRow( m_rowColResized, docPoint.y() );
                        else
                            m_currentTable->resizeColumn( m_rowColResized, docPoint.x() );
                        // Repaint only the changed rects (oldRect U newRect)
                        QRect newRect( m_viewMode->normalToView( m_doc->zoomRect( m_currentTable->boundingRect() ) ) );
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
    }
    else {
        if ( m_mouseMode == MM_EDIT )
        {
            MouseMeaning meaning = m_frameViewManager->mouseMeaning( docPoint, e->state() );
            switch ( meaning ) {
             case MEANING_MOUSE_OVER_FOOTNOTE:
             {
                 KWFrameView *view = m_frameViewManager->view(docPoint, KWFrameViewManager::frameOnTop);
                 KWFrame* frame = view ? view->frame() : 0;
                 KWFrameSet * fs = frame ? frame->frameSet() : 0;
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
                KWFrameView *view = m_frameViewManager->view(docPoint, KWFrameViewManager::frameOnTop);
                KWFrame* frame = view ? view->frame() : 0;
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
            viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, e->state() ) );
        }
    }
}

void KWCanvas::mrEditFrame( QMouseEvent *e, const QPoint &nPoint ) // Can be called from KWCanvas and from KWResizeHandle's mouseReleaseEvents
{
    //kdDebug() << "KWCanvas::mrEditFrame" << endl;
    KWFrameView *view = m_frameViewManager->selectedFrame();
    KWFrame *firstFrame = view == 0 ? 0 : view->frame();
    //kdDebug() << "KWCanvas::mrEditFrame m_frameMoved=" << m_frameMoved << " m_frameResized=" << m_frameResized << endl;
    if ( firstFrame && ( m_frameMoved || m_frameResized ) )
    {
        KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell*>(firstFrame->frameSet());
        if (cell) {
            KWTableFrameSet *table = cell->groupmanager();
            table->recalcCols( cell->firstColumn(), 0 );
            table->recalcRows( 0, cell->firstRow() );
        }

        // Create command
        if ( m_frameResized )
        {
            KWFrameView *view = m_frameViewManager->selectedFrame();
            KWFrame *frame = view == 0 ? 0 : view->frame();
            Q_ASSERT( frame );
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
                    }
                    // Especially useful for EPS images: set final size
                    fs->resizeFrame( frame, frame->width(), frame->height(), true );
                    if ( frame && fs->type() == FT_PART )
                        static_cast<KWPartFrameSet *>( fs )->updateChildGeometry( viewMode() );
            }
            delete m_moveFrameCommand; // Unused after all
            m_moveFrameCommand = 0L;
        }
        else
        {
            Q_ASSERT( m_moveFrameCommand ); // has been created by mpEditFrame
            if( m_moveFrameCommand )
            {
                // Store final positions
                QValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
                QValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
                QValueList<FrameMoveStruct>::Iterator it = m_moveFrameCommand->listFrameMoved().begin();
                for(; framesIterator != selectedFrames.end() &&
                        it != m_moveFrameCommand->listFrameMoved().end();
                        ++framesIterator ) {
                    KWFrame *frame= (*framesIterator)->frame();
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
                m_doc->addCommand(m_moveFrameCommand);

                // build list for framesChanged call
                QPtrList<KWFrame> framesChanged;
                framesIterator = selectedFrames.begin();
                for(;framesIterator != selectedFrames.end(); ++framesIterator)
                    framesChanged.append( (*framesIterator)->frame() );
                m_doc->framesChanged( framesChanged, m_gui->getView() ); // repaint etc.

                m_moveFrameCommand = 0L;
            }
        }
        m_doc->repaintAllViews();
        m_frameMoved = m_frameResized = false;
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
            repaintContents();
    }
    else
    {
        // No frame was moved or resized.
        if ( e->state() & ControlButton )
        {
            if ( m_ctrlClickOnSelectedFrame ) {
                QPoint normalPoint = m_viewMode->viewToNormal( nPoint );
                KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
                KWFrameView *view = m_frameViewManager->view(docPoint, KWFrameViewManager::selected);
                KWFrame* f = view ? view->frame() : 0;
                if (e->state() & ShiftButton)
                    selectAllFrames( false );
                if (f)
                    selectFrame(f,true);
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
        frame->setZOrder( m_doc->maxZOrder( frame->pageNumber(m_doc) ) + 1 ); // make sure it's on top

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
        frame->setZOrder( m_doc->maxZOrder( frame->pageNumber(m_doc) ) + 1 ); // make sure it's on top
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
    int page = m_doc->pageManager()->pageNumber( picRect );
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

    setMouseMode( MM_EDIT );
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
        frame->setZOrder( m_doc->maxZOrder( page ) +1 ); // make sure it's on top
        fs->addFrame( frame, false );
        m_doc->addFrameSet( fs );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Picture Frame"), frame );
        m_doc->addCommand(cmd);
        m_doc->frameChanged( frame );
        frameViewManager()->view(frame)->setSelected(true);
    }
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
        frame->setZOrder( m_doc->maxZOrder( frame->pageNumber(m_doc) ) + 1 ); // make sure it's on top
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
    int pageNum = m_doc->pageManager()->pageNumber(m_insRect.topLeft());

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
            int page = m_doc->pageManager()->pageNumber(docPoint);
            if(page == -1)
                return;
            KoPageLayout pageLayout = m_doc->pageManager()->pageLayout(page);
            m_insRect.setLeft(QMIN(m_insRect.left(), pageLayout.ptWidth - 200));
            m_insRect.setTop(QMIN(m_insRect.top(), pageLayout.ptHeight - 150));
            m_insRect.setBottom(m_insRect.top()+150);
            m_insRect.setRight(m_insRect.left()+200);
        }
        MouseMode old_mouseMove = m_mouseMode;
        switch ( m_mouseMode ) {
            case MM_EDIT:
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseReleaseEvent( e, normalPoint, docPoint );
                else {
                  if ( m_mouseMeaning == MEANING_RESIZE_COLUMN )
                  {
                    KWResizeColumnCommand *cmd = new KWResizeColumnCommand( m_currentTable, m_rowColResized, m_previousTableSize, docPoint.x() );
                    m_doc->addCommand(cmd);
                    cmd->execute();
                  }
                  else if ( m_mouseMeaning == MEANING_RESIZE_ROW )
                  {
                    KWResizeRowCommand *cmd = new KWResizeRowCommand( m_currentTable, m_rowColResized, m_previousTableSize, docPoint.y() );
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

        if ( old_mouseMove != MM_EDIT && !m_doc->showGrid() && m_doc->snapToGrid() )
          repaintContents( FALSE ); //draw the grid over the whole canvas
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
                KWFrameView *view = m_frameViewManager->selectedFrame();
                bool isPartFrameSet = view && dynamic_cast<KWPartFrameSet*>(view->frame()->frameSet());
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

void KWCanvas::setFrameBackgroundColor( const QBrush &_backColor )
{
    QValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
    if (selectedFrames.isEmpty())
        return;
    bool colorChanged=false;
    QPtrList<FrameIndex> frameindexList;
    QPtrList<QBrush> oldColor;

    QValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    while(framesIterator != selectedFrames.end()) {
        KWFrame *frame = KWFrameSet::settingsFrame( (*framesIterator)->frame() );
        FrameIndex *index=new FrameIndex( frame );
        frameindexList.append(index);

        QBrush *_color=new QBrush(frame->backgroundColor());
        oldColor.append(_color);

        if (frame->frameSet() && frame->frameSet()->type()!=FT_PICTURE && frame->frameSet()->type()!=FT_PART &&  _backColor!=frame->backgroundColor())
        {
            colorChanged=true;
            frame->setBackgroundColor(_backColor);
        }
        ++framesIterator;
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
    QValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
    if(selectedFrames.count()==0) return;

    KWFrameDia *frameDia;
    if(selectedFrames.count()==1)
        frameDia = new KWFrameDia( this, selectedFrames[0]->frame());
    else { // multi frame dia.
        QPtrList<KWFrame> frames;
        QValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        for(;framesIterator != selectedFrames.end(); ++framesIterator)
            frames.append( (*framesIterator)->frame() );
        frameDia = new KWFrameDia( this, frames );
    }
    frameDia->exec();
    delete frameDia;
}

void KWCanvas::selectAllFrames( bool select ) {
    QValueList<KWFrameView*> frameViews = m_frameViewManager->frameViewsIterator();
    QValueList<KWFrameView*>::iterator frames = frameViews.begin();
    for(; frames != frameViews.end(); ++frames ) {
        KWFrameSet *fs = (*frames)->frame()->frameSet();
        if ( !fs->isVisible() ) continue;
        if ( select && fs->isMainFrameset() )
            continue; // "select all frames" shouldn't select the page
        (*frames)->setSelected(select);
    }
}

void KWCanvas::tableSelectCell(KWTableFrameSet *table, KWFrameSet *cell)
{
    if ( m_currentFrameSetEdit )
        terminateCurrentEdit();
    selectFrame( cell->frame(0), TRUE ); // select the frame.
    m_currentTable = table;
}

void KWCanvas::selectFrame( KWFrame * frame, bool select )
{
    m_frameViewManager->view(frame)->setSelected(select);
}

void KWCanvas::editFrameSet( KWFrameSet * frameSet, bool onlyText /*=false*/ )
{
    selectAllFrames( false );
    bool emitChanged = checkCurrentEdit( frameSet, onlyText );

    if ( emitChanged ) // emitted after mousePressEvent [for tables]
        emit currentFrameSetEditChanged();
    emit updateRuler();
}

void KWCanvas::editTextFrameSet( KWFrameSet * fs, KoTextParag* parag, int index )
{
    selectAllFrames( false );

#if 0
    //active header/footer when it's possible
    // DF: what is this code doing here?
    if ( fs->isAHeader() && !m_doc->isHeaderVisible() && !(viewMode()->type()=="ModeText"))
        m_doc->setHeaderVisible( true );
    if ( fs->isAFooter() && !m_doc->isFooterVisible() && !(viewMode()->type()=="ModeText"))
        m_doc->setFooterVisible( true );
#endif

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
                m_currentTable = static_cast<KWTableFrameSet *>(fs);
            else if ( fs->type() == FT_TEXT )
                m_currentTable = static_cast<KWTextFrameSet *>(fs)->groupmanager();
            else
                m_currentTable = 0L;
            if ( m_currentTable ) {
                m_currentFrameSetEdit = m_currentTable->createFrameSetEdit( this );
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
    m_lastCaretPos = caretPos();
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
        selectFrame(frameIt.current(), false);
}

void KWCanvas::setMouseMode( MouseMode newMouseMode )
{
    if ( m_mouseMode != newMouseMode )
    {
        selectAllFrames( false );

        if ( newMouseMode != MM_EDIT )
        {
            // Terminate edition of current frameset
            if ( m_currentFrameSetEdit )
                terminateCurrentEdit();
        }

        m_mouseMode = newMouseMode;
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
          repaintContents( FALSE ); //draw the grid over the whole canvas
    }
    else
        m_mouseMode = newMouseMode;
    emit currentMouseModeChanged(m_mouseMode);

    switch ( m_mouseMode ) {
    case MM_EDIT: {
        QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
        QPoint normalPoint = m_viewMode->viewToNormal( mousep );
        KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
        viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, 0 ) );
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
        KWFrameView *view = m_frameViewManager->view(docPoint, KWFrameViewManager::frameOnTop);
        KWFrame *frame = view ? view->frame() : 0;
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
                // Activate this code (and in focusNextPreviousChild() to allow Shift+Tab
                // out of document window.  Disabled because it conflicts with Shift+Tab inside a table.
                // else if ( keyev->key() == Qt::Key_BackTab )
                //    return FALSE;
                else if ( keyev->key() == KGlobalSettings::contextMenuKey() ) {
                    // The popups are not available in readonly mode, since the GUI isn't built...
                    if(!m_doc->isReadWrite()) return TRUE;
                    if (m_mouseMode != MM_EDIT) return TRUE;
                    KoPoint docPoint = m_doc->unzoomPoint( QCursor::pos() );

                    if ( viewMode()->type()=="ModeText") {
                        KWFrameView *view = m_frameViewManager->view(m_doc->frameSet( 0 )->frame(0));
                        view->showPopup(docPoint, m_gui->getView(), QCursor::pos());
                    }
                    else {
                        m_frameViewManager->showPopup( docPoint, m_gui->getView(), keyev->state(), pos());
                    }
                    return true;
                }
                else if ( keyev->key() == Qt::Key_Return && keyev->state() == 0
                    && (m_mouseMode != MM_EDIT || m_frameInline )) {
                    // When inserting an inline or non-line frame,
                    // simulate mouse press and release at caret position.
                    // In the case of a regular frame, the caret position was saved when
                    // they left edit mode.  In the case of an inline frame,
                    // get current caret position, since user can type and move caret
                    // around before they click or hit Enter.
                    if (m_frameInline)
                        m_lastCaretPos = caretPos();
                    if (m_lastCaretPos.isNull()) return TRUE;
                    int page = m_doc->pageManager()->pageNumber(m_lastCaretPos);
                    if(page == -1) return TRUE;
                    QPoint normalPoint = m_doc->zoomPoint(m_lastCaretPos);
                    // Coordinate is at the very top of the caret.  In the case of an
                    // inline frame, adjust slightly down and to the right in order
                    // to avoid "clicking" the frame border.
                    if (m_frameInline)
                        normalPoint += QPoint(2,2);
                    QPoint vP = m_viewMode->normalToView(normalPoint);
                    QPoint gP = mapToGlobal(vP);
                    QMouseEvent mevPress(QEvent::MouseButtonPress, vP,
                        gP, Qt::LeftButton, 0);
                    contentsMousePressEvent(&mevPress);
                    QMouseEvent mevRelease(QEvent::MouseButtonRelease, vP,
                        gP, Qt::LeftButton, 0);
                    contentsMouseReleaseEvent(&mevRelease);
                }
                else if ( keyev->key() == Qt::Key_Escape  )
                {
                    if ( m_mouseMode != MM_EDIT )
                        // Abort frame creation
                        setMouseMode( MM_EDIT );
                    else
                    {
                      if ( m_frameMoved && m_moveFrameCommand)
                      {
                        m_moveFrameCommand->unexecute();
                        delete m_moveFrameCommand;
                        m_moveFrameCommand = 0;
                        m_frameMoved = false;

                        m_mousePressed = false; //we don't want things to happen in KWCanvas::contentsMouseReleaseEvent
                        m_ctrlClickOnSelectedFrame = false;
                        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
                            repaintContents();
                        m_gui->getView()->updateFrameStatusBarItem();
                        setMouseMode( MM_EDIT );
                      }
                      else if ( m_frameResized )
                      {
                        KWFrameView *view = m_frameViewManager->selectedFrame();
                        KWFrame *frame = view == 0 ? 0 : view->frame();
                        if (!frame) { // can't happen, but never say never
                          kdWarning(32001) << "KWCanvas::eventFilter: no frame selected!" << endl;
                          return TRUE;
                        }
                        KWFrameSet* frameSet = frame->frameSet();
                        QRect oldRect = m_viewMode->normalToView( frame->outerRect(m_viewMode) );
                        frameSet->resizeFrameSetCoords( frame, m_boundingRect.left(), m_boundingRect.top(), m_boundingRect.right(), m_boundingRect.bottom(), false /*not final*/ );
                        delete m_moveFrameCommand; // Unused after all
                        m_moveFrameCommand = 0L;
                        m_frameResized = false;
                        m_mousePressed = false; //we don't want things to happen in KWCanvas::contentsMouseReleaseEvent
                        m_ctrlClickOnSelectedFrame = false;
                        // Move resize handles to new position
                        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
                          repaintContents();
                        else
                        {
                          // Calculate new rectangle for this frame
                          QRect newRect( m_viewMode->normalToView( frame->outerRect(m_viewMode) ) );
                          // Repaint only the changed rects (oldRect U newRect)
                          repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), FALSE );
                          m_gui->getView()->updateFrameStatusBarItem();
                        }
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
                    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
                    viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, keyev->stateAfter() ) );
                }
                else if ( (keyev->key() == Qt::Key_Delete || keyev->key() ==Key_Backspace )
                          && m_frameViewManager->selectedFrame() && !m_printing )
                    m_gui->getView()->editDeleteFrame();
            } break;
            case QEvent::KeyRelease:
            {
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
                if ( keyev->key() == Qt::Key_Control )
                {
                    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                    QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                    KoPoint docPoint = m_doc->unzoomPoint( normalPoint );
                    viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, keyev->stateAfter() ) );
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
            case QEvent::MouseMove:
                {
                    if(! m_mousePressed)
                        break;
                    if(! kWordDocument()->isReadWrite())
                        break;
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (e);
                    bool shiftPressed = mouseEvent->state() & ShiftButton;
                    switch (m_mouseMeaning) {
                        case MEANING_TOPLEFT:
                            mmEditFrameResize( true, false, true, false, shiftPressed );
                            break;
                        case MEANING_TOP:
                            mmEditFrameResize( true, false, false, false, shiftPressed );
                            break;
                        case MEANING_TOPRIGHT:
                            mmEditFrameResize( true, false, false, true, shiftPressed );
                            break;
                        case MEANING_RIGHT:
                            mmEditFrameResize( false, false, false, true, shiftPressed );
                            break;
                        case MEANING_BOTTOMRIGHT:
                            mmEditFrameResize( false, true, false, true, shiftPressed );
                            break;
                        case MEANING_BOTTOM:
                            mmEditFrameResize( false, true, false, false, shiftPressed );
                            break;
                        case MEANING_BOTTOMLEFT:
                            mmEditFrameResize( false, true, true, false, shiftPressed );
                            break;
                        case MEANING_LEFT:
                            mmEditFrameResize( false, false, true, false, shiftPressed );
                            break;
                        default: // do nothing for the rest.
                            break;
                    }
                }
                break;
            default:
                break;
        }
    }
    return QScrollView::eventFilter( o, e );
}

bool KWCanvas::focusNextPrevChild( bool next)
{
    Q_UNUSED(next);
    return TRUE; // Don't allow to go out of the canvas widget by pressing "Tab"
    // Don't allow to go out of the canvas widget by pressing Tab, but do allow Shift+Tab.
    // if (next) return TRUE;
    // return QWidget::focusNextPrevChild( next );
}

void KWCanvas::updateCurrentFormat()
{
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit);
    if ( edit )
        edit->updateUI( true, true );
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
    if ( textfs && textfs->groupmanager() )
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
    if ( textfs && textfs->groupmanager() )
        return static_cast<KWTableFrameSet::Cell *>(textfs)->firstColumn();
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


/* Returns the caret position in document coordinates.
   The current frame must be editable, i.e., a caret is possible. */
KoPoint KWCanvas::caretPos()
{
    if (!m_currentFrameSetEdit) return KoPoint();
    KWTextFrameSetEdit* textEdit =
        dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
    if (!textEdit) return KoPoint();
    KoTextCursor* cursor = textEdit->cursor();
    if (!cursor) return KoPoint();
    KWTextFrameSet* textFrameset =
        dynamic_cast<KWTextFrameSet *>(m_currentFrameSetEdit->frameSet());
    if (!textFrameset) return KoPoint();
    KWFrame* currentFrame = m_currentFrameSetEdit->currentFrame();
    if (!currentFrame) return KoPoint();

    QPoint viewP = textFrameset->cursorPos(cursor, this, currentFrame);
    viewP.rx() += contentsX();
    viewP.ry() += contentsY();
    QPoint normalP = m_viewMode->viewToNormal(viewP);
    KoPoint docP = m_doc->unzoomPoint(normalP);
    return docP;
}

#include "KWCanvas.moc"
