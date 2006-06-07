/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2006 David Faure <faure@kde.org>
   Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "KWCanvas.h"
#include "KWTableFrameSet.h"
#include "KWPartFrameSet.h"
#include "KWFormulaFrameSet.h"
#include "KWDocument.h"
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
#include "KWGUI.h"
#include "KWView.h"

#include <QTimer>
#include <q3progressdialog.h>
#include <QApplication>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <Q3ValueList>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <Q3PtrList>

#include <KoStore.h>
#include <KoStoreDrag.h>
#include <KoPictureCollection.h>
#include <KoGfxEvent.h>
#include <KoShapeManager.h>
#include <KoCreateShapesTool.h>

#include <ktempfile.h>
#include <klocale.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <k3urldrag.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kprinter.h>

#include <KoRectangleShape.h>
#include <QBrush>

KWCanvas::KWCanvas(const QString& viewMode, KWDocument *d, KWGUI *lGui)
    : QWidget(), m_doc( d )
{
    m_frameViewManager = new KWFrameViewManager(d);
    m_tool = 0;
    m_gui = lGui;
    m_shapeManager = new KoShapeManager(this);
KoRectangleShape *rs = new KoRectangleShape();
rs->setBackground(QBrush(Qt::red));
m_shapeManager->add(rs);
    m_currentFrameSetEdit = 0L;
    m_mouseMeaning = MEANING_NONE;
    m_mousePressed = false;
    m_imageDrag = false;
    m_frameInline = false;
    m_overwriteMode = false;

    //used by insert picture dialogbox
    m_picture.pictureInline = false;
    m_picture.keepRatio = true;



    m_frameInlineType = FT_TABLE;
    m_viewMode = KWViewMode::create( viewMode, m_doc, this );

#if 0
    // Default table parameters.
    m_table.rows = 3;
    m_table.cols = 2;
    m_table.width = KWTableFrameSet::TblAuto;
    m_table.height = KWTableFrameSet::TblAuto;
    m_table.floating = true;
    m_table.tableTemplateName=QString::null;
    m_table.format=31;
#endif

    m_footEndNote.noteType = FootNote;
    m_footEndNote.numberingType = KWFootNoteVariable::Auto;


    m_currentTable = 0L;
    m_printing = false;
    m_deleteMovingRect = false;
    m_resizedFrameInitialMinHeight = 0;
    m_temporaryStatusBarTextShown = false;

    setBackgroundMode( Qt::PaletteBase );
    setAcceptDrops( true );

    setKeyCompression( true );
    setMouseTracking( true );

    m_scrollTimer = new QTimer( this );
    connect( m_scrollTimer, SIGNAL( timeout() ),
             this, SLOT( doAutoScroll() ) );

    setFocusPolicy( Qt::WheelFocus );
    setFocus();
    setInputMethodEnabled( true );
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

#if 0
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
#endif
    m_doc->deleteInitialEditingInfo();
    updateSize();

    connect(frameViewManager(), SIGNAL(sigFrameResized(const Q3ValueList<KWFrame*>&)),
        m_doc, SLOT(framesChanged(const Q3ValueList<KWFrame*>&)));
    connect(frameViewManager(), SIGNAL(sigFrameMoved(const Q3ValueList<KWFrame*>&)),
        m_doc, SLOT(framesChanged(const Q3ValueList<KWFrame*>&)));
}

KWCanvas::~KWCanvas()
{
    delete m_currentFrameSetEdit;
    m_currentFrameSetEdit = 0;
    delete m_viewMode;
    m_viewMode = 0;
    disconnect(frameViewManager(), SIGNAL(sigFrameResized(const Q3ValueList<KWFrame*>&)),
        m_doc, SLOT(framesChanged(const Q3ValueList<KWFrame*>&)));
    disconnect(frameViewManager(), SIGNAL(sigFrameMoved(const Q3ValueList<KWFrame*>&)),
        m_doc, SLOT(framesChanged(const Q3ValueList<KWFrame*>&)));
    delete m_frameViewManager;
    m_frameViewManager = 0;
    delete m_shapeManager;
    m_shapeManager = 0;
}

void KWCanvas::repaintChanged( KWFrameSet * fs, bool resetChanged )
{
kDebug() << "KWCanvas::repaintChanged" << endl;
#if 0
    assert(fs); // the new code can't support fs being 0L here. Mail me if it happens (DF)
    //kDebug(32002) << "KWCanvas::repaintChanged this=" << this << " fs=" << fs << endl;
    QPainter p( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setBrushOrigin( -contentsX(), -contentsY() );
    QRect crect( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    drawFrameSet( fs, &p, crect, true, resetChanged, m_viewMode );
    // ###### This repaints the whole grid every time. Ouch.
    // We should return a QRegion from drawFrameSet.... Tricky.
    if ( m_doc->showGrid() )
      drawGrid( p, crect );
#endif
}

void KWCanvas::repaintAll( bool erase /* = false */ )
{
    update();
}

#if 0
void KWCanvas::viewportResizeEvent( QResizeEvent * )
{
    viewport()->update();
}
#endif

void KWCanvas::print( QPainter *painter, KPrinter *printer )
{
#if 0
    // Prevent cursor drawing and editing
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->focusOutEvent();
    m_printing = true;
    KWViewMode *viewMode = new KWViewModePrint( m_doc, this );
    // Use page list specified in kdeprint dialogbox
    QList<int> pageList = printer->pageList();
    Q3ProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              pageList.count() + 1, this, 0, false, 0 );
    int j = 0;
    progress.setProgress( 0 );
    QList<int>::Iterator it = pageList.begin();
    for ( ; it != pageList.end(); ++it )
    {
        progress.setProgress( ++j );
        qApp->processEvents();

        if ( progress.wasCanceled() )
            break;

        if ( it != pageList.begin() )
            printer->newPage();

        painter->save();
        int pgNum = (*it);
        int yOffset = m_doc->zoomItYOld( m_doc->pageManager()->topOfPage( pgNum ) );
        kDebug(32001) << "printing page " << pgNum << " yOffset=" << yOffset << endl;
        QRect pageRect = m_doc->pageManager()->page(pgNum)->zoomedRect(m_doc);
        painter->fillRect( pageRect, Qt::white );

        painter->translate( 0, -yOffset );
        painter->setBrushOrigin( 0, -yOffset );
        drawDocument( painter, pageRect, viewMode );
        qApp->processEvents();
        painter->restore();
    }
    if ( m_currentFrameSetEdit )
        m_currentFrameSetEdit->focusInEvent();
    m_printing = false;
    delete viewMode;
#endif
}

/*
void KWCanvas::drawContents( QPainter *painter, int cx, int cy, int cw, int ch )
{
  if ( isUpdatesEnabled() )
  {
    // Note: in drawContents, the painter is already translated to the contents coordinates
    painter->setBrushOrigin( -contentsX(), -contentsY() );
    drawDocument( painter, QRect( cx, cy, cw, ch ), m_viewMode );
    if ( m_doc->showGrid() )
      drawGrid( *painter, QRect( cx, cy, cw, ch ) );
    else if ( m_doc->snapToGrid() && ( m_interactionPolicy && m_interactionPolicy->gotDragEvents()
                || m_mouseMode != MM_EDIT ) )
      drawGrid( *painter, QRect(contentsX(), contentsY(), visibleWidth(), visibleHeight()) );
  }
}*/

void KWCanvas::drawDocument( QPainter *painter, const QRect &crect, KWViewMode* viewMode )
{
    //kDebug(32002) << "KWCanvas::drawDocument crect: " << crect << endl;

    // Draw the outside of the pages (shadow, gray area)
    // and the empty area first (in case of transparent frames)
    if ( painter->device()->devType() != QInternal::Printer ) // except when printing
    {
        QRegion emptySpaceRegion( crect );
        m_doc->createEmptyRegion( crect, emptySpaceRegion, viewMode );
        viewMode->drawPageBorders( painter, crect, emptySpaceRegion );
    }

    // Draw all framesets contents
    Q3PtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
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

    bool focus = hasFocus();
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
#if 0
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
#endif
    m_tool->keyPressEvent(e);
}

void KWCanvas::switchViewMode( const QString& newViewMode )
{
    delete m_viewMode;
    m_viewMode = KWViewMode::create( newViewMode, m_doc, this );
}

void KWCanvas::mpCreate( const QPoint& normalPoint, bool noGrid )
{
#if 0
    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
    if ( m_doc->snapToGrid() && !noGrid )
        applyGrid( docPoint );
    m_insRect.setCoords( docPoint.x(), docPoint.y(), 0, 0 );
    m_deleteMovingRect = false;
#endif
}

void KWCanvas::mpCreatePixmap( const QPoint& normalPoint, bool noGrid  )
{
#if 0
    if ( !m_kopicture.isNull() )
    {
        // Apply grid for the first corner only
        KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
        if ( m_doc->snapToGrid() && ! noGrid )
            applyGrid( docPoint );
        int pageNum = m_doc->pageManager()->pageNumber( docPoint );
        m_insRect.setRect( docPoint.x(), docPoint.y(), 0, 0 );
        m_deleteMovingRect = false;

        if ( !m_pixmapSize.isEmpty() )
        {
            // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
            uint width = qRound( (double)m_pixmapSize.width() * m_doc->zoomedResolutionX() / POINT_TO_INCH( KoGlobal::dpiX() ) );
            uint height = qRound( (double)m_pixmapSize.height() * m_doc->zoomedResolutionY() / POINT_TO_INCH( KoGlobal::dpiY() ) );
            m_insRect.setWidth( m_doc->unzoomItXOld( width ) );
            m_insRect.setHeight( m_doc->unzoomItYOld( height ) );
            // Apply reasonable limits
            width = qMin( width, m_doc->paperWidth(pageNum) - normalPoint.x() - 5 );
            height = qMin( height, m_doc->paperHeight(pageNum) - normalPoint.y() - 5 );
            // And apply aspect-ratio if set
            if ( m_keepRatio )
            {
                double ratio = ((double) m_pixmapSize.width()) / ((double) m_pixmapSize.height());
                applyAspectRatio( ratio, m_insRect );
            }

            QPoint nPoint( normalPoint.x() + m_doc->zoomItXOld( m_insRect.width() ),
                           normalPoint.y() + m_doc->zoomItYOld( m_insRect.height() ) );
            QPoint vPoint = m_viewMode->normalToView( nPoint );
            vPoint = contentsToViewport( vPoint );
            QRect viewportRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
            if ( viewportRect.contains( vPoint ) ) // Don't move the mouse out of the viewport
                QCursor::setPos( viewport()->mapToGlobal( vPoint ) );
        }
        emit docStructChanged(Pictures);
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
          repaintContents( false ); //draw the grid over the whole canvas
    }
#endif
}

#if 0
void KWCanvas::contentsMousePressEvent( QMouseEvent *e )
{
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );


    if ( e->button() == Qt::LeftButton )
    {
      m_mousePressed = true;
    }

    // Only edit-mode (and only LMB) allowed on read-only documents (to select text)
    if ( !m_doc->isReadWrite() && ( m_mouseMode != MM_EDIT || e->button() != Qt::LeftButton ) )
        return;
    if ( m_printing )
        return;

    // This code here is common to all mouse buttons, so that RMB and MMB place the cursor (or select the frame) too
    switch ( m_mouseMode ) {
    case MM_EDIT:
    {
        if(! viewMode()->hasFrames() ) { // for the text mode we just forward the click to the text
            // first, make clicks left of text be on the border;
            docPoint = KoPoint(qMax(0.0, docPoint.x()), qMax(0.0, docPoint.y()));
            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->mousePressEvent( e, normalPoint, docPoint );
            break;
        }
        // See if we clicked on a frame's border
        m_mouseMeaning = m_frameViewManager->mouseMeaning( docPoint, e->state());

        //kDebug(32001) << "contentsMousePressEvent meaning=" << m_mouseMeaning << endl;
        switch ( m_mouseMeaning )  {
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
            if ( ! ( e->button() == Qt::RightButton && view && view->selected() ) )
                selectAllFrames( false );

            KWFrame * frame = view ? view->frame() : 0L;
            KWFrameSet * fs = frame ? frame->frameSet() : 0L;
            if ( fs && fs->groupmanager() )
                fs = fs->groupmanager();
            bool emitChanged = false;
            if ( fs )
            {
                // Clicked inside a frame - start editing it
                emitChanged = checkCurrentEdit( fs );
            }

            if ( m_currentFrameSetEdit )
                m_currentFrameSetEdit->mousePressEvent( e, normalPoint, docPoint );

            if ( emitChanged ) { // emitted after mousePressEvent [for tables]
                emit currentFrameSetEditChanged();
                emit updateRuler();
            }

            if ( m_frameInline )
            {
                bool inlineCreated = true;
                if(m_frameInlineType == FT_TABLE)
                    inlineCreated = insertInlineTable();
                else if(m_frameInlineType == FT_PICTURE)
                    inlineCreated = insertInlinePicture();
                if (!inlineCreated)
                    KMessageBox::information(0L, i18n("Read-only content cannot be changed. No modifications will be accepted."));
            }
            break;
        }
        case MEANING_RESIZE_COLUMN:
        case MEANING_RESIZE_ROW:
        {
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
                    kDebug(32002) << "resizing row/col edge. m_rowColResized=" << m_rowColResized << endl;
                }
            }
            break;
        }
        default:
            m_mousePressed = true;
            m_deleteMovingRect = false;

            delete m_interactionPolicy;
            m_interactionPolicy = InteractionPolicy::createPolicy(this, m_mouseMeaning, docPoint, e->button(), e->state());
            if(m_interactionPolicy)
                terminateCurrentEdit();
            viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, e->state() ) );
        }
    }
    break;
    case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE:
    case MM_CREATE_FORMULA:
        if ( e->button() == Qt::LeftButton )
            mpCreate( normalPoint, e->state() & Qt::ShiftModifier);
        break;
    case MM_CREATE_PIX:
        if ( e->button() == Qt::LeftButton )
            mpCreatePixmap( normalPoint, e->state() & Qt::ShiftModifier );
        break;
    default: break;
    }
    m_scrollTimer->start( 50 );

    if ( e->button() == Qt::MidButton ) {
        if ( m_doc->isReadWrite() && m_currentFrameSetEdit && m_mouseMode == MM_EDIT )
        {
            m_currentFrameSetEdit->paste();
        }
    }
    else if ( e->button() == Qt::RightButton ) {
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
#endif

// Called by KWTableDia
void KWCanvas::createTable( unsigned int rows, unsigned int cols,
                            int wid, int hei,
                            bool isFloating,
                            KWTableTemplate *tt, int format )
{
#if 0
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
#endif
}

bool KWCanvas::insertInlinePicture() // also called by DCOP
{
    bool inserted = m_gui->getView()->insertInlinePicture();
    if ( inserted )
        m_frameInline = false;
    return inserted;
}

bool KWCanvas::insertInlineTable()
{
#if 0
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
    m_gui->getView()->updateFrameStatusBarItem();
    m_frameInline = false;
    return true;
#endif
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

  //FIXME It doesn't work in preview mode - apply viewmode
}

void KWCanvas::drawGrid( QPainter &p, const QRect& rect )
{
// TODO reenable this after converting to QPointF and using the zoomHandler
#if 0
    // Grid-positioning makes no sense in text view mode
    if ( !m_viewMode->hasFrames() )
        return;
    QPen pen = QPen( Qt::black, 6, Qt::DotLine );
    QPen oldPen = p.pen();
    p.setPen( pen );

    const double offsetX = m_doc->gridX();
    const double offsetY = m_doc->gridY();

    // per page, this is needed to paint correctly on preview mode as well as making sure
    // we start the grid from the topleft of each page.
    for ( int pgNum = m_doc->startPage() ; pgNum <= m_doc->lastPage() ; ++pgNum) {
        const QRect pageRect = m_viewMode->viewPageRect( pgNum );
        const QRect crect = pageRect & rect;
        if ( crect.isEmpty() )
            continue;

        // To convert back to view coordinates later we can calculate the offset for each
        // point again, or we can do it one time for the page and re-use the answer.
        KoPoint pageTopLeft (0, m_doc->pageManager()->topOfPage(pgNum) + 2); // +2 to get around rounding problems..
        QPoint pageTopLeftView = m_viewMode->normalToView( m_doc->zoomPointOld(pageTopLeft) );

        // docRect is the part of the document that needs to be painted.. Coordinates in pt
        const KoRect docRect = m_doc->unzoomRectOld( m_viewMode->viewToNormal( crect ) );
        // kDebug() << "drawGrid page " << pgNum << " pageRect=" << pageRect << " crect=" << crect << " docRect=" << docRect << endl;

        // the following is effectively a docRect.y() modulo offsetY
        const double firstOffsetY = pageTopLeft.y() - offsetY * static_cast<int>( docRect.y() / offsetY );
        const double bottom = docRect.bottom() - pageTopLeft.y();

        for ( double x = 0; x <= docRect.right(); x += offsetX ) {
            const int zoomedX = m_doc->zoomItXOld( x ) + pageTopLeftView.x();
            for ( double y = firstOffsetY; y <= bottom; y += offsetY ) {
                const int zoomedY = m_doc->zoomItYOld( y ) + pageTopLeftView.y();
                p.drawPoint( zoomedX, zoomedY );
            }
        }
    }

    p.setPen( oldPen );
#endif
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
    //kDebug() << "KWCanvas::applyAspectRatio: width=" << width << " height=" << height << " insRect=" << DEBUGRECT(insRect) << endl;
}

void KWCanvas::mmCreate( const QPoint& normalPoint, bool noGrid ) // Mouse move when creating a frame
{
#if 0
    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    //p.setRasterOp( NotROP );
    p.setPen( Qt::black );
    p.setBrush( Qt::NoBrush );

    if ( m_deleteMovingRect )
        drawMovingRect( p );

    int page = m_doc->pageManager()->pageNumber( m_insRect );
    if( page == -1) {
        return;
    }
    KoRect oldRect = m_insRect;

    // Resize the rectangle
    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
    if ( m_doc->snapToGrid() && m_mouseMode != MM_CREATE_PIX && !noGrid )
        applyGrid( docPoint );

    m_insRect.setRight( docPoint.x() );
    m_insRect.setBottom( docPoint.y() );

    // But not out of the page
    KoRect r = m_insRect.normalize();
    if ( !m_doc->pageManager()->page(page)->rect().contains(r) )
    {
        // Even better would be to go to the limit of the page boundary, so that the result,
        // when moving the mouse of the page, doesn't depend on how fast we're moving it...
        m_insRect = oldRect;
        // #### QCursor::setPos( viewport()->mapToGlobal( zoomPointOld( m_insRect.bottomRight() ) ) );
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
#endif
}

void KWCanvas::drawMovingRect( QPainter & p )
{
#if 0
    p.setPen( Qt::black );
    p.drawRect( m_viewMode->normalToView( m_doc->zoomRectOld( m_insRect ) ) );
#endif
}

void KWCanvas::deleteMovingRect()
{
#if 0
    Q_ASSERT( m_deleteMovingRect );
    QPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    //p.setRasterOp( NotROP );
    p.setPen( Qt::black );
    p.setBrush( Qt::NoBrush );
    drawMovingRect( p );
    m_deleteMovingRect = false;
    p.end();
#endif
}

#if 0
void KWCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
    if ( m_printing )
        return;
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    if(normalPoint.x() == -1) // e->pos() is an exceptional state...
        return;
    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );

    if ( m_mousePressed ) {
        //doAutoScroll();

        switch ( m_mouseMode ) {
            case MM_EDIT:
            {
                if ( m_currentFrameSetEdit )
                    m_currentFrameSetEdit->mouseMoveEvent( e, normalPoint, docPoint );
                if( ! m_doc->isReadWrite() )
                    break;
                if ( m_mouseMeaning == MEANING_RESIZE_COLUMN || m_mouseMeaning == MEANING_RESIZE_ROW )
                {
                    // TODO undo/redo support (esp. in mouse-release)
                    QRect oldRect( m_viewMode->normalToView( m_doc->zoomRectOld( m_currentTable->boundingRect() ) ) );
                    if ( m_mouseMeaning == MEANING_RESIZE_ROW )
                        m_currentTable->resizeRow( m_rowColResized, docPoint.y() );
                    else
                        m_currentTable->resizeColumn( m_rowColResized, docPoint.x() );
                    // Repaint only the changed rects (oldRect U newRect)
                    QRect newRect( m_viewMode->normalToView( m_doc->zoomRectOld( m_currentTable->boundingRect() ) ) );
                    repaintContents( QRegion(oldRect).unite(newRect).boundingRect(), false );
                }
                else if (m_interactionPolicy) {
                    m_interactionPolicy->handleMouseMove(e->state(),
                            m_doc->unzoomPointOld( normalPoint ));
                    m_interactionPolicy->hadDragEvents();
                }
            } break;
            case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART:
            case MM_CREATE_TABLE: case MM_CREATE_FORMULA:
                mmCreate( normalPoint, e->state() & Qt::ShiftModifier );
            default:
                break;
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
            if(viewMode()->hasFrames())
                viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, e->state() ) );
            else
                viewport()->setCursor( Qt::IBeamCursor );
        }
    }
}
#endif

void KWCanvas::mrEditFrame() {
#if 0
    //kDebug() << "KWCanvas::mrEditFrame" << endl;
    if(m_interactionPolicy) {
        m_interactionPolicy->finishInteraction();
        KCommand *cmd = m_interactionPolicy->createCommand();
        if(cmd)
            addCommand(cmd, false);
        delete(m_interactionPolicy);
        m_interactionPolicy = 0;
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
            repaintContents();
    }
    m_mousePressed = false;
#endif
}

KCommand *KWCanvas::createTextBox( const KoRect & rect )
{
#if 0
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
    return 0;
#endif
}

void KWCanvas::mrCreateText()
{
#if 0
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        KWFrame *frame = new KWFrame(0L, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frame->setMinimumFrameHeight( frame->height() ); // so that AutoExtendFrame doesn't resize it down right away
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
#endif
}

void KWCanvas::mrCreatePixmap()
{
#if 0
    // kDebug() << "KWCanvas::mrCreatePixmap m_insRect=" << DEBUGRECT(m_insRect) << endl;
    Q_ASSERT(m_insRect.width() > 0 && m_insRect.height() > 0);
    // Make sure the pic is completely in document.
    double ratio = m_insRect.width() / m_insRect.height();
    KoRect picRect(m_doc->pageManager()->clipToDocument(m_insRect.topLeft()),
            m_doc->pageManager()->clipToDocument(m_insRect.bottomRight()) );
    picRect = picRect.normalize();

    // Make sure it's completely on page.
    KWPage *page = m_doc->pageManager()->page( picRect.bottom() );
    KoRect pageRect = page->rect();
    picRect = pageRect.intersect(picRect);

    // make sure we keep ratio!
    double height = picRect.width() / ratio ;
    if(picRect.height() > height)
        picRect.setBottom(picRect.top() + height);
    else // moving bottom would make it bigger, so alter width
        picRect.setRight(picRect.left() + ratio * picRect.height());

    setMouseMode( MM_EDIT );
    if ( !m_kopicture.isNull() ) {
        KWPictureFrameSet *fs = new KWPictureFrameSet( m_doc, QString::null /*automatic name*/ );
        fs->insertPicture( m_kopicture );
        fs->setKeepAspectRatio( m_keepRatio );
        KWFrame *frame = new KWFrame(fs, picRect.x(), picRect.y(), picRect.width(),
                picRect.height() );
        frame->setZOrder( m_doc->maxZOrder( page->pageNumber() ) +1 ); // make sure it's on top
        fs->addFrame( frame, false );
        m_doc->addFrameSet( fs );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Insert Picture"), frame );
        addCommand(cmd, false);
        m_doc->frameChanged( frame );
        frameViewManager()->view(frame)->setSelected(true);
    }
    emit docStructChanged(Pictures);
#endif
}

void KWCanvas::mrCreatePart() // mouse release, when creating part
{
#if 0
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        KWPartFrameSet *fs = m_doc->insertObject( m_insRect, m_partEntry, this );
Q_ASSERT(viewMode()->canvas());
        if(fs)
            fs->updateChildGeometry(); // set initial coordinates of 'ch' correctly
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(Embedded);
#endif
}

void KWCanvas::mrCreateFormula()
{
#if 0
    m_insRect = m_insRect.normalize();
    if ( !m_doc->snapToGrid() || ( m_insRect.width() > m_doc->gridX() && m_insRect.height() > m_doc->gridY() ) ) {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( m_doc, QString::null );
        KWFrame *frame = new KWFrame(frameset, m_insRect.x(), m_insRect.y(), m_insRect.width(), m_insRect.height() );
        frame->setZOrder( m_doc->maxZOrder( frame->pageNumber(m_doc) ) + 1 ); // make sure it's on top
        frameset->addFrame( frame, false );
        m_doc->addFrameSet( frameset );
        KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Formula Frame"), frame );
        addCommand(cmd, false);
        m_doc->frameChanged( frame );
    }
    setMouseMode( MM_EDIT );
    emit docStructChanged(FormulaFrames);
#endif
}

void KWCanvas::mrCreateTable()
{
#if 0
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
            addCommand(macroCmd, false);
            macroCmd->execute();

            emit docStructChanged(Tables);
        }
        m_doc->updateAllFrames();
        m_doc->layout();
        repaintAll();

    }
    setMouseMode( MM_EDIT );
#endif
}

KWTableFrameSet * KWCanvas::createTable() // uses m_insRect and m_table to create the table
{
#if 0
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
#endif
}

#if 0
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
        KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );

        if(m_insRect.bottom()==0 && m_insRect.right()==0) {
            // if the user did not drag, just click; make a 200x150 square for him.
            int page = m_doc->pageManager()->pageNumber(docPoint);
            if(page == -1)
                return;
            KoPageLayout pageLayout = m_doc->pageManager()->pageLayout(page);
            m_insRect.setLeft(qMin(m_insRect.left(), pageLayout.ptWidth - 200));
            m_insRect.setTop(qMin(m_insRect.top(), pageLayout.ptHeight - 150));
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
                    addCommand(cmd, false);
                    cmd->execute();
                  }
                  else if ( m_mouseMeaning == MEANING_RESIZE_ROW )
                  {
                    KWResizeRowCommand *cmd = new KWResizeRowCommand( m_currentTable, m_rowColResized, m_previousTableSize, docPoint.y() );
                    addCommand(cmd, false);
                    cmd->execute();
                  }
                  else
                    mrEditFrame();
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
          repaintContents( false ); //draw the grid over the whole canvas
        m_mousePressed = false;
    }
}

void KWCanvas::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
    if ( m_printing )
        return;
    QPoint normalPoint = m_viewMode->viewToNormal( e->pos() );
    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
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
                // KWDocumentChild::hitTest and KWView::slotChildActivated take care of embedded objects
                m_mousePressed = false;
            }
            break;
        default:
            break;
    }
}
#endif

void KWCanvas::setFrameBackgroundColor( const QBrush &backColor )
{
    Q3ValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
    if (selectedFrames.isEmpty())
        return;
    bool colorChanged=false;
    Q3PtrList<FrameIndex> frameindexList;
    Q3PtrList<QBrush> oldColor;

    Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    while(framesIterator != selectedFrames.end()) {
        KWFrame *frame = KWFrameSet::settingsFrame( (*framesIterator)->frame() );
        FrameIndex *index=new FrameIndex( frame );
        frameindexList.append(index);

        QBrush *_color=new QBrush(frame->background());
        oldColor.append(_color);

        if (frame->frameSet() && frame->frameSet()->type()!=FT_PICTURE && frame->frameSet()->type()!=FT_PART &&  backColor!=frame->background())
        {
            colorChanged=true;
            frame->setBackground(backColor);
        }
        ++framesIterator;
    }
    if(colorChanged)
    {
        KWFrameBackGroundColorCommand *cmd=new KWFrameBackGroundColorCommand(i18n("Change Frame Background Color"),frameindexList,oldColor,backColor);
        addCommand(cmd, false);
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
    Q3ValueList<KWFrameView*> selectedFrames = m_frameViewManager->selectedFrames();
    if(selectedFrames.count()==0) return;

    KWFrameDia *frameDia;
    if(selectedFrames.count()==1)
        frameDia = new KWFrameDia( this, selectedFrames[0]->frame());
    else { // multi frame dia.
        Q3PtrList<KWFrame> frames;
        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        for(;framesIterator != selectedFrames.end(); ++framesIterator)
            frames.append( (*framesIterator)->frame() );
        frameDia = new KWFrameDia( this, frames );
    }
    frameDia->exec();
    delete frameDia;
}

void KWCanvas::selectAllFrames( bool select ) {
    Q3ValueList<KWFrameView*> frameViews = m_frameViewManager->frameViewsIterator();
    Q3ValueList<KWFrameView*>::iterator frames = frameViews.begin();
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
    terminateCurrentEdit();
    m_frameViewManager->view(cell->frame(0))->setSelected(true);
    m_currentTable = table;
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
#if 0
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
        KWTextFrameSetEdit *textedit = currentTextEdit();
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
#endif
}

void KWCanvas::ensureCursorVisible()
{
    Q_ASSERT( m_currentFrameSetEdit );
    KWTextFrameSetEdit *textedit = currentTextEdit();
    if ( textedit )
        textedit->ensureCursorVisible();
}

bool KWCanvas::checkCurrentEdit( KWFrameSet * fs , bool onlyText )
{
#if 0
    bool emitChanged = false;
    if ( fs && m_currentFrameSetEdit && m_currentFrameSetEdit->frameSet() != fs )
    {
        KWTextFrameSet * tmp = dynamic_cast<KWTextFrameSet *>(fs );
        if ( tmp && tmp->protectContent() && !m_doc->cursorInProtectedArea() )
            return false;

        KWTextFrameSetEdit *edit = currentTextEdit();
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
#if 0
            if ( m_currentTable ) {
                m_currentFrameSetEdit = m_currentTable->createFrameSetEdit( this );
                static_cast<KWTableFrameSetEdit *>( m_currentFrameSetEdit )->setCurrentCell( fs );
            }
            else
                m_currentFrameSetEdit = fs->createFrameSetEdit( this );
#endif

            if ( m_currentFrameSetEdit ) {
                KWTextFrameSetEdit *edit = currentTextEdit();
                if ( edit )
                    edit->setOverwriteMode( m_overwriteMode );
            }
        }
        emitChanged = true;
    }
    return emitChanged;
#endif
}

void KWCanvas::terminateCurrentEdit()
{
    if(m_currentFrameSetEdit == 0)
        return;
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
    Q3PtrListIterator<KWFrame> frameIt = fs->frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        m_frameViewManager->view(frameIt.current())->setSelected(false);
}

KWTextFrameSetEdit* KWCanvas::currentTextEdit() const
{
#if 0
    if ( m_currentFrameSetEdit )
        return dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit->currentTextEdit());
    return 0;
#endif
}

void KWCanvas::setMouseMode( MouseMode newMouseMode )
{
#if 0
    if ( m_mouseMode != newMouseMode )
    {
        selectAllFrames( false );

        if ( newMouseMode != MM_EDIT )
            terminateCurrentEdit();

        m_mouseMode = newMouseMode;
        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
          repaintContents( false ); //draw the grid over the whole canvas
    }
    else
        m_mouseMode = newMouseMode;
    emit currentMouseModeChanged(m_mouseMode);

    switch ( m_mouseMode ) {
    case MM_EDIT: {
        QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
        QPoint normalPoint = m_viewMode->viewToNormal( mousep );
        KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
        viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, 0 ) );
        m_frameInline = false;
    } break;
    case MM_CREATE_TEXT:
    case MM_CREATE_PIX:
    case MM_CREATE_TABLE:
    case MM_CREATE_FORMULA:
    case MM_CREATE_PART:
        viewport()->setCursor( Qt::CrossCursor );
        break;
    }
#endif
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

void KWCanvas::insertPictureDirect( const KoPicture& picture, const KoPoint& pos, const QSize& sz )
{
    // Prepare things for mrCreatePixmap
    m_pixmapSize = sz.isEmpty() ? picture.getOriginalSize() : sz;
    m_kopicture = picture;
    m_insRect = KoRect( pos.x(), pos.y(), m_doc->unzoomItXOld( m_pixmapSize.width() ), m_doc->unzoomItYOld( m_pixmapSize.height() ) );
    m_keepRatio = true;
    mrCreatePixmap();
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

#if 0
void KWCanvas::contentsDragEnterEvent( QDragEnterEvent *e )
{
    const QMimeData* mimeData = e->mimeData();
    const int provides = KWView::checkClipboard( mimeData );
    if ( ( provides & KWView::ProvidesImage ) || KUrl::List::canDecode( mimeData ) )
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
        KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
        KWFrameView *view = m_frameViewManager->view(docPoint, KWFrameViewManager::frameOnTop);
        KWFrame *frame = view ? view->frame() : 0;
        KWFrameSet * fs = frame ? frame->frameSet() : 0L;
        bool emitChanged = false;
        if ( fs )
        {
            //kDebug()<<"table :"<<table<<endl;
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
    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );

    const QMimeData* mimeData = e->mimeData();
    if ( mimeData->hasImage() ) {
        pasteImage( mimeData, docPoint );
    } else if ( KUrl::List::canDecode( mimeData ) ) {

        // TODO ask (with a popupmenu) between inserting a link and inserting the contents
        // TODO fix khtml to export images when dragging an image+link (as it does when using "Copy")

        const KUrl::List lst = KUrl::List::fromMimeData( mimeData );

        KUrl::List::ConstIterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            const KUrl &url( *it );

            QString filename;
            if ( !KIO::NetAccess::download( url, filename, this ) )
                continue;

            KMimeType::Ptr res = KMimeType::findByFileContent( filename );

            if ( res && res->isValid() ) {
                QString mimetype = res->name();
                if ( mimetype.contains( "image" ) ) {
                    KoPictureKey key;
                    key.setKeyFromFile( filename );
                    KoPicture newKoPicture;
                    newKoPicture.setKey( key );
                    newKoPicture.loadFromFile( filename );
                    insertPictureDirect( newKoPicture, docPoint );
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
            m_gui->getView()->pasteData( mimeData, true );
    }
    m_mousePressed = false;
    m_imageDrag = false;
}
#endif

void KWCanvas::pasteImage( const QMimeData *mimeData, const KoPoint &docPoint )
{
    QImage i = mimeData->imageData().value<QImage>();
    KTempFile tmpFile( QString::null, ".png");
    tmpFile.setAutoDelete( true );
    if ( !i.save(tmpFile.name(), "PNG") ) {
        kWarning() << "Couldn't save image to " << tmpFile.name() << endl;
        return;
    }

    m_pixmapSize = i.size();
    // Prepare things for mrCreatePixmap
    KoPictureKey key;
    key.setKeyFromFile( tmpFile.name() );
    KoPicture newKoPicture;
    newKoPicture.setKey( key );
    newKoPicture.loadFromFile( tmpFile.name() );
    m_kopicture = newKoPicture;
    m_insRect = KoRect( docPoint.x(), docPoint.y(), m_doc->unzoomItXOld( i.width() ), m_doc->unzoomItYOld( i.height() ) );
    m_keepRatio = true;
    mrCreatePixmap();
}

void KWCanvas::doAutoScroll()
{
#if 0
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
#endif
}

void KWCanvas::slotContentsMoving( int cx, int cy )
{
#if 0
    //QPoint nPointTop = m_viewMode->viewToNormal( QPoint( cx, cy ) );
    QPoint nPointBottom = m_viewMode->viewToNormal( QPoint( cx + visibleWidth(), cy + visibleHeight() ) );
    //kDebug() << "KWCanvas::slotContentsMoving cx=" << cx << " cy=" << cy << endl;
    //kDebug() << " visibleWidth()=" << visibleWidth() << " visibleHeight()=" << visibleHeight() << endl;
    // Update our "formatted paragraphs needs" in all the text framesets
    Q3PtrList<KWTextFrameSet> textFrameSets = m_doc->allTextFramesets( false );
    Q3PtrListIterator<KWTextFrameSet> fit( textFrameSets );
    for ( ; fit.current() ; ++fit )
    {
        if(! fit.current()->isVisible()) continue;
        fit.current()->updateViewArea( this, m_viewMode, nPointBottom );
    }
    // cx and cy contain the future values for contentsx and contentsy, so we need to
    // pass them to updateRulerOffsets.
    updateRulerOffsets( cx, cy );

    // Tell KoView that the view transformations have changed (e.g. the centering of the page, or the scrolling offsets)
    // so that it will reposition any active embedded object.
    // This needs to be delayed since contents moving is emitted -before- moving,
    // and from resizeEvent it's too early too.
    QTimer::singleShot( 0, this, SIGNAL( viewTransformationsChanged() ) );
#endif
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
#if 0
    QSize size = m_viewMode->contentsSize();
    if ( size != QSize( contentsWidth(), contentsHeight() ) )
    {
        //kDebug() << "KWCanvas::slotNewContentsSize " << size.width() << "x" << size.height() << endl;
        resizeContents( size.width(), size.height() );
    }
#endif
}

void KWCanvas::resizeEvent( QResizeEvent *e )
{
#if 0
    slotContentsMoving( contentsX(), contentsY() );
    Q3ScrollView::resizeEvent( e );
#endif
}

void KWCanvas::scrollToOffset( const KoPoint & d )
{
    //kDebug() << "KWCanvas::scrollToOffset " << d.x() << "," << d.y() << endl;
#if 0
    QPoint nPoint = m_doc->zoomPointOld( d );
    QPoint cPoint = m_viewMode->normalToView( nPoint );
    setContentsPos( cPoint.x(), cPoint.y() );
#endif
}

void KWCanvas::updateRulerOffsets( int cx, int cy )
{
#if 0
    if ( cx == -1 && cy == -1 )
    {
        cx = contentsX();
        cy = contentsY();
    }
    // The offset is usually just the scrollview offset
    // But we also need to offset to the current page, for the graduations
    QPoint pc = m_viewMode->pageCorner();
    //kDebug() << "KWCanvas::updateRulerOffsets contentsX=" << cx << ", contentsY=" << cy << endl;
    m_gui->getHorzRuler()->setOffset( cx - pc.x(), 0 );
    m_gui->getVertRuler()->setOffset( 0, cy - pc.y() );

#endif
}

bool KWCanvas::eventFilter( QObject *o, QEvent *e )
{
#if 0
    if ( o == this || o == viewport() ) {

        if(m_currentFrameSetEdit && o == this )
        {
            // Pass event to auto-hide-cursor code (see kcursor.h for details)
            KCursor::autoHideEventFilter( o, e );
        }

        switch ( e->type() ) {
            case QEvent::FocusIn:
                //  kDebug() << "KWCanvas::eventFilter QEvent::FocusIn" << endl;
                if ( m_currentFrameSetEdit && !m_printing )
                    m_currentFrameSetEdit->focusInEvent();
                break;
            case QEvent::FocusOut:
                //  kDebug() << "KWCanvas::eventFilter QEvent::FocusOut" << endl;
                if ( m_currentFrameSetEdit && !m_printing )
                    m_currentFrameSetEdit->focusOutEvent();
                if ( m_scrollTimer->isActive() )
                    m_scrollTimer->stop();
                m_mousePressed = false;
                break;
            case QEvent::ShortcutOverride: // was part of KeyPress - changed due to kdelibs BUG!
            {
                //  kDebug() << " KeyPress m_currentFrameSetEdit=" << m_currentFrameSetEdit << " isRW="<<m_doc->isReadWrite() << endl;
                //  kDebug() << " m_printing=" << m_printing << " mousemode=" << m_mouseMode << " (MM_EDIT=" << MM_EDIT<<")"<<endl;
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
#ifndef NDEBUG
                // Debug keys
                if ( ( keyev->state() & Qt::ControlModifier ) && ( keyev->state() & Qt::ShiftModifier ) )
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
                            kDebug(32002) << "Current framesetedit: " << m_currentFrameSetEdit << " " <<
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
                                kDebug(32002) << "Developer mark: " << dtMark.toString("yyyy-MM-dd hh:mm:ss,zzz") << endl;
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
                //  kDebug() << " KeyPress m_currentFrameSetEdit=" << m_currentFrameSetEdit << " isRW="<<m_doc->isReadWrite() << endl;
                //  kDebug() << " m_printing=" << m_printing << " mousemode=" << m_mouseMode << " (MM_EDIT=" << MM_EDIT<<")"<<endl;
                QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
                // By default PgUp and PgDown move the scrollbars and not the caret anymore - this is done here
                if ( !m_doc->pgUpDownMovesCaret() && ( (keyev->state() & Qt::ShiftModifier) == 0 )
                     && ( keyev->key() == Qt::Key_PageUp || keyev->key() == Qt::Key_PageDown ) )
                {
                    viewportScroll( keyev->key() == Qt::Key_PageUp );
                }
                // Activate this code (and in focusNextPreviousChild() to allow Shift+Tab
                // out of document window.  Disabled because it conflicts with Shift+Tab inside a table.
                // else if ( keyev->key() == Qt::Key_Backtab )
                //    return false;
                else if ( keyev->key() == KGlobalSettings::contextMenuKey() ) {
                    // The popups are not available in readonly mode, since the GUI isn't built...
                    if(!m_doc->isReadWrite()) return true;
                    if (m_mouseMode != MM_EDIT) return true;
                    KoPoint docPoint = m_doc->unzoomPointOld( QCursor::pos() );

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
                    if (m_lastCaretPos.isNull()) return true;
                    int page = m_doc->pageManager()->pageNumber(m_lastCaretPos);
                    if(page == -1) return true;
                    QPoint normalPoint = m_doc->zoomPointOld(m_lastCaretPos);
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
                else if ( keyev->key() == Qt::Key_Escape  ) {
                    if ( m_mouseMode != MM_EDIT ) // Abort frame creation
                        setMouseMode( MM_EDIT );
                    else if(m_interactionPolicy) {
                        m_interactionPolicy->cancelInteraction();
                        delete(m_interactionPolicy);
                        m_interactionPolicy = 0;
                        m_mousePressed = false;

                        // reset cursor
                        QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                        QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                        KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
                        viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, keyev->stateAfter() ) );
                        if ( !m_doc->showGrid() && m_doc->snapToGrid() )
                            repaintContents();
                    }
                }
                else if ( keyev->key() == Qt::Key_Insert && keyev->state() == 0 ) {
                    m_overwriteMode = !m_overwriteMode;
                    KWTextFrameSetEdit *edit = currentTextEdit();
                    if ( edit ) {
                        edit->setOverwriteMode( m_overwriteMode );
                        emit overwriteModeChanged( m_overwriteMode );
                    }
                    kDebug()<<"Insert is pressed, overwrite mode: "<< m_overwriteMode << endl;
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
                    return true;
                }

                // Because of the dependency on the control key, we need to update the mouse cursor here
                if ( keyev->key() == Qt::Key_Control )
                {
                    QPoint mousep = mapFromGlobal(QCursor::pos()) + QPoint( contentsX(), contentsY() );
                    QPoint normalPoint = m_viewMode->viewToNormal( mousep );
                    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
                    viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, keyev->stateAfter() ) );
                }
                else if ( (keyev->key() == Qt::Key_Delete || keyev->key() ==Qt::Key_Backspace )
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
                    KoPoint docPoint = m_doc->unzoomPointOld( normalPoint );
                    viewport()->setCursor( m_frameViewManager->mouseCursor( docPoint, keyev->stateAfter() ) );
                }

                if ( m_currentFrameSetEdit && m_mouseMode == MM_EDIT && m_doc->isReadWrite() && !m_printing )
                {
                    m_currentFrameSetEdit->keyReleaseEvent( keyev );
                    return true;
                }
            }
                break;
            case QEvent::InputMethod:
            {
                QInputMethodEvent * imev = static_cast<QInputMethodEvent *>(e);
                m_currentFrameSetEdit->inputMethodEvent( imev );
            }
                break;
            default:
                break;
        }
    }
    return Q3ScrollView::eventFilter( o, e );
#endif
}

bool KWCanvas::focusNextPrevChild( bool next)
{
    Q_UNUSED(next);
    return true; // Don't allow to go out of the canvas widget by pressing "Tab"
    // Don't allow to go out of the canvas widget by pressing Tab, but do allow Shift+Tab.
    // if (next) return true;
    // return QWidget::focusNextPrevChild( next );
}

void KWCanvas::updateCurrentFormat()
{
#if 0
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_currentFrameSetEdit);
    if ( edit )
        edit->updateUI( true, true );
#endif
}

#ifndef NDEBUG
void KWCanvas::printRTDebug( int info )
{
#if 0
    KWTextFrameSet * textfs = 0L;
    if ( m_currentFrameSetEdit ) {
        KWTextFrameSetEdit* edit = currentTextEdit();
        if ( edit ) {
            textfs = dynamic_cast<KWTextFrameSet *>( edit->frameSet() );
            Q_ASSERT( textfs );
        }
    }
    if ( !textfs )
        textfs = dynamic_cast<KWTextFrameSet *>(m_doc->frameSet( 0 ));
    if ( textfs )
        textfs->textObject()->printRTDebug( info );
#endif
}
#endif

void KWCanvas::setXimPosition( int x, int y, int w, int h )
{
#if 0 // TODO Qt4 PORTING
    This functionality has been replaced by subclassing
    QWidget::inputMethodQuery, and then calling QWidget::updateMicroFocus() when you
    would previously have called QWidget::setMicroFocusHint( ... )

    QWidget::setMicroFocusHint( x - contentsX(), y - contentsY(), w, h );
#endif
}

void KWCanvas::inlinePictureStarted()
{
    m_frameInline=true;
    m_frameInlineType=FT_PICTURE;
}

int KWCanvas::currentTableRow() const
{
#if 0
    if ( !m_currentFrameSetEdit )
        return -1;
    KWTextFrameSetEdit *edit = currentTextEdit();
    if ( !edit )
        return -1;
    KWTextFrameSet* textfs = edit->textFrameSet();
    if ( textfs && textfs->groupmanager() )
        return static_cast<KWTableFrameSet::Cell *>(textfs)->firstRow();
    return -1;
#endif
}

int KWCanvas::currentTableCol() const
{
#if 0
    if ( !m_currentFrameSetEdit )
        return -1;
    KWTextFrameSetEdit *edit = currentTextEdit();
    if ( !edit )
        return -1;
    KWTextFrameSet* textfs = edit->textFrameSet();
    if ( textfs && textfs->groupmanager() )
        return static_cast<KWTableFrameSet::Cell *>(textfs)->firstColumn();
    return -1;
#endif
}

void KWCanvas::viewportScroll( bool up )
{
#if 0
    if ( up )
        setContentsPos( contentsX(), contentsY() - visibleHeight() );
    else
        setContentsPos( contentsX(), contentsY() + visibleHeight() );
#endif
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
#if 0
    if (!m_currentFrameSetEdit) return KoPoint();
    KWTextFrameSetEdit* textEdit = currentTextEdit();
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
    KoPoint docP = m_doc->unzoomPointOld(normalP);
    return docP;
#endif
}

void KWCanvas::gridSize(double *horizontal, double *vertical) const {
    *horizontal = m_doc->gridX();
    *vertical = m_doc->gridY();
}

bool KWCanvas::snapToGrid() const {
    return m_doc->snapToGrid();
}

void KWCanvas::addCommand(KCommand *command, bool execute) {
    m_doc->addCommand(command, execute);
}

void KWCanvas::updateCanvas(const QRectF& rc) {
    QRect clipRect(viewConverter()->normalToView(rc).toRect());
    clipRect.adjust(-2, -2, 2, 2); // grow for to anti-aliasing
    update(clipRect);
}

KoViewConverter *KWCanvas::viewConverter() {
    return m_gui->getView()->viewConverter();
}

void KWCanvas::mouseMoveEvent(QMouseEvent *e) {
    KoGfxEvent ev(e, QPointF( viewConverter()->viewToNormal(e->pos()) ));

    m_tool->mouseMoveEvent( &ev );
    setCursor( m_tool->cursor( ev.point ) );
}

void KWCanvas::mousePressEvent(QMouseEvent *e) {
    KoGfxEvent ev(e, QPointF( viewConverter()->viewToNormal(e->pos()) ));

    m_tool->mousePressEvent( &ev );
    setCursor( m_tool->cursor( ev.point ) );
}

void KWCanvas::mouseReleaseEvent(QMouseEvent *e) {
    KoGfxEvent ev(e, QPointF( viewConverter()->viewToNormal(e->pos()) ));

    m_tool->mouseReleaseEvent( &ev );
    setCursor( m_tool->cursor( ev.point ) );
}

void KWCanvas::keyReleaseEvent (QKeyEvent *e) {
    m_tool->keyReleaseEvent(e);
}

void KWCanvas::paintEvent(QPaintEvent * ev) {
    QPainter painter( this );
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipRect(ev->rect());

    m_shapeManager->paint( painter, *(viewConverter()), false );
    m_tool->paint( painter, *m_doc );

    painter.end();
}

void KWCanvas::updateSize() {
    QSize size = viewMode()->contentsSize();
    setMinimumSize(size.width(), size.height());
}

// ************* KWShapeController ***************
KWShapeController::KWShapeController(KWDocument *document)
: m_document(document)
{
    m_type = TextFrameType;
}

void KWShapeController::addShape( KoShape* shape ) {
kDebug() << "KWShapeController::addShape" << endl;
    foreach(KWView *view, m_document->getAllViews())
        view->getGUI()->canvasWidget()->shapeManager()->add(shape);
}

void KWShapeController::removeShape( KoShape* shape ) {
    foreach(KWView *view, m_document->getAllViews())
        view->getGUI()->canvasWidget()->shapeManager()->remove(shape);
}

KoShape* KWShapeController::createShape(const QRectF &outline) const {
    KoShape *rect = new KoRectangleShape();
    rect->setBackground(QColor(Qt::blue));
    rect->setPosition(outline.topLeft());
    rect->resize(outline.size());
    return rect;
}

#include "KWCanvas.moc"
