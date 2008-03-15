/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

// Local
#include "Canvas.h"
#include "Canvas_p.h"

// std
#include <assert.h>
#include <float.h>
#include <stdlib.h>

// Qt
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTextStream>
#include <QToolTip>
#include <QWidget>

// KDE
#include <kcursor.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <krun.h>
#include <kmimetype.h>
#include <ksharedptr.h>
#include <kwordwrap.h>

// KOffice
#include <KoCanvasController.h>
#include <KoDocumentChild.h>
#include <KoShapeManager.h>
#include <KoStore.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

// KSpread
#include "Border.h"
#include "CellStorage.h"
#include "CellView.h"
#include "Doc.h"
#include "Editors.h"
#include "Global.h"
#include "Localization.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetView.h"
#include "Util.h"
#include "Validity.h"
#include "View.h"

// commands
#include "commands/DeleteCommand.h"
#include "commands/EmbeddedObjectCommands.h"
#include "commands/StyleCommand.h"
#include "commands/Undo.h"

#define MIN_SIZE 10

using namespace KSpread;

/****************************************************************
 *
 * Canvas
 *
 ****************************************************************/

Canvas::Canvas(View *view)
    : QWidget( view )
    , KoCanvasBase( view->doc() )
    , d( new Private )
{
  setAttribute( Qt::WA_OpaquePaintEvent );
  setAttribute( Qt::WA_StaticContents );
  setBackgroundRole(QPalette::Base);

  d->cellEditor = 0;
  d->chooseCell = false;
  d->validationInfo = 0;

  QWidget::setFocusPolicy( Qt::StrongFocus );


  d->defaultGridPen.setColor( Qt::lightGray );
  d->defaultGridPen.setWidth( 1 );
  d->defaultGridPen.setStyle( Qt::SolidLine );

  d->offset = QPointF( 0.0, 0.0 );

  d->view = view;

  //Now built afterwards(David)
  //d->editWidget = d->view->editWidget();
  d->posWidget = d->view->posWidget();

  setMouseTracking( true );
  d->mousePressed = false;
  d->dragging = false;
  d->mouseSelectedObject = false;
  d->drawContour = false;
  d->modType = MT_NONE;

  d->m_resizeObject = 0;
  d->m_ratio = 0.0;
  d->m_isMoving = false;
  d->m_objectDisplayAbove = false;
  d->m_isResizing = false;

  d->prevSpokenPointerRow = -1;
  d->prevSpokenPointerCol = -1;
  d->prevSpokenFocusRow = -1;
  d->prevSpokenFocusCol = -1;
  d->prevSpokenRow = -1;
  d->prevSpokenCol = -1;

    connect( d->view, SIGNAL( autoScroll( const QPoint & )),
             this, SLOT( slotAutoScroll( const QPoint &)));

  setFocus();
  installEventFilter( this ); // for TAB key processing, otherwise focus change
  setAcceptDrops( true );
  setAttribute(Qt::WA_InputMethodEnabled, true); // ensure using the InputMethod

    // flake
    d->shapeManager = new KoShapeManager( this );
    d->toolProxy = new KoToolProxy( this );
}

Canvas::~Canvas()
{
    delete d->shapeManager;
    delete d->toolProxy;
    delete d->validationInfo;
    delete d;
}

View* Canvas::view() const
{
  return d->view;
}

Doc* Canvas::doc() const
{
  return d->view->doc();
}

void Canvas::gridSize( double* horizontal, double* vertical ) const
{
    *horizontal = doc()->defaultColumnFormat()->width();
    *vertical = doc()->defaultRowFormat()->height();
}

bool Canvas::snapToGrid() const
{
    return false; // FIXME
}

void Canvas::addCommand( QUndoCommand* command )
{
    doc()->addCommand( command );
}

KoShapeManager* Canvas::shapeManager() const
{
    return d->shapeManager;
}

void Canvas::updateCanvas( const QRectF& rc )
{
    QRect clipRect( viewConverter()->documentToView( rc.translated( -offset() ) ).toRect() );
    clipRect.adjust( -2, -2, 2, 2 ); // Resize to fit anti-aliasing
    update( clipRect );
}

const KoViewConverter* Canvas::viewConverter() const
{
    return view()->zoomHandler();
}

KoUnit Canvas::unit() const
{
    return doc()->unit();
}

KoToolProxy* Canvas::toolProxy() const
{
    return d->toolProxy;
}

void Canvas::setEditWidget( KSpread::EditWidget * ew )
{
  d->editWidget = ew;
}

KSpread::EditWidget* Canvas::editWidget() const
{
  return d->editWidget;
}

CellEditor* Canvas::editor() const
{
  return d->cellEditor;
}

QPointF Canvas::offset() const
{
    return d->offset;
}

double Canvas::xOffset() const
{
    return d->offset.x();
}

double Canvas::yOffset() const
{
    return d->offset.y();
}

const QPen& Canvas::defaultGridPen() const
{
  return d->defaultGridPen;
}

void Canvas::setLastEditorWithFocus( Canvas::EditorType type )
{
  d->focusEditorType = type;
}

Canvas::EditorType Canvas::lastEditorWithFocus() const
{
  return d->focusEditorType;
}


bool Canvas::eventFilter( QObject *o, QEvent *e )
{
  /* this canvas event filter acts on events sent to the line edit as well
     as events to this filter itself.
  */
  if ( !o || !e )
    return true;
  switch ( e->type() )
  {
  case QEvent::KeyPress:
  {
    QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
    if ((keyev->key()==Qt::Key_Tab) || (keyev->key()==Qt::Key_Backtab))
    {
      keyPressEvent ( keyev );
      return true;
    }
    break;
  }
  case QEvent::InputMethod:
  {
      //QIMEvent * imev = static_cast<QIMEvent *>(e);
      //processIMEvent( imev );
      //break;
  }
  case QEvent::ToolTip:
  {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>( e );
    showToolTip( helpEvent->pos() );
  }
  default:
    break;
  }
  return false;
}

bool Canvas::focusNextPrevChild( bool )
{
    return true; // Don't allow to go out of the canvas widget by pressing "Tab"
}

Selection* Canvas::selection() const
{
  return d->view->selection();
}

Selection* Canvas::choice() const
{
  return d->view->choice();
}

double Canvas::zoom() const
{
  return d->view->zoom();
}

void Canvas::setChooseMode(bool state)
{
  d->chooseCell = state;
}

bool Canvas::chooseMode() const
{
  return d->chooseCell;
}

void Canvas::startChoose()
{
    if ( d->chooseCell )
        return;

    choice()->clear();
    choice()->setActiveSheet( activeSheet() );
    choice()->setOriginSheet( activeSheet() );

    // It is important to enable this AFTER we set the rect!
    d->chooseCell = true;
}

void Canvas::startChoose( const QRect& rect )
{
    if ( d->chooseCell )
        return;

    choice()->setActiveSheet( activeSheet() );
    choice()->initialize( rect );
    choice()->setOriginSheet( activeSheet() );

    // It is important to enable this AFTER we set the rect!
    d->chooseCell = true;
}

void Canvas::endChoose()
{
    // While entering a formula the choose mode is turned on and off.
    // Clear the choice even if we are not in choose mode. Otherwise,
    // cell references will stay highlighted.
    if ( !choice()->isEmpty() )
    {
        d->view->markSelectionAsDirty();
        choice()->clear();
    }

    if ( !d->chooseCell )
        return;

    d->chooseCell = false;

    if ( choice()->originSheet() )
        d->view->setActiveSheet( choice()->originSheet() );
}

HBorder* Canvas::hBorderWidget() const
{
  return d->view->hBorderWidget();
}

VBorder* Canvas::vBorderWidget() const
{
  return d->view->vBorderWidget();
}

QScrollBar* Canvas::horzScrollBar() const
{
  return d->view->horzScrollBar();
}

QScrollBar* Canvas::vertScrollBar() const
{
  return d->view->vertScrollBar();
}

Sheet* Canvas::findSheet( const QString& _name ) const
{
  return d->view->doc()->map()->findSheet( _name );
}

Sheet* Canvas::activeSheet() const
{
  return d->view->activeSheet();
}

void Canvas::validateSelection()
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

    if ( selection()->isSingular() )
    {
        int col = selection()->marker().x();
        int row = selection()->marker().y();
        Cell cell( sheet, col,row );
        Validity validity = cell.validity();
        if ( validity.displayValidationInformation() )
        {
            QString title = validity.titleInfo();
            QString message = validity.messageInfo();
            if ( title.isEmpty() && message.isEmpty() )
                return;

            if ( !d->validationInfo )
                d->validationInfo = new QLabel(  this );
            kDebug(36001)<<" display info validation";
            double u = cell.width();
            double v = cell.height();
            double xpos = sheet->columnPosition( selection()->marker().x() ) - xOffset();
            double ypos = sheet->rowPosition( selection()->marker().y() ) - yOffset();
            // Special treatment for obscured cells.
            if ( cell.isPartOfMerged() )
            {
                cell = cell.masterCell();
                int moveX = cell.column();
                int moveY = cell.row();

                // Use the obscuring cells dimensions
                u = cell.width();
                v = cell.height();
                xpos = sheet->columnPosition( moveX );
                ypos = sheet->rowPosition( moveY );
            }
            //d->validationInfo->setGeometry( 3, y + 3, len + 2, hei + 2 );
            d->validationInfo->setAlignment( Qt::AlignVCenter );
            QPainter painter;
            painter.begin( this );
            int len = 0;
            int hei = 0;
            QString resultText;
            if ( !title.isEmpty() )
            {
                len = painter.fontMetrics().width( title );
                hei = painter.fontMetrics().height();
                resultText = title + '\n';
            }
            if ( !message.isEmpty() )
            {
                int i = 0;
                int pos = 0;
                QString t;
                do
                {
                    i = message.indexOf( "\n", pos );
                    if ( i == -1 )
                        t = message.mid( pos, message.length() - pos );
                    else
                    {
                        t = message.mid( pos, i - pos );
                        pos = i + 1;
                    }
                    hei += painter.fontMetrics().height();
                    len = qMax( len, painter.fontMetrics().width( t ) );
                }
                while ( i != -1 );
                resultText += message;
            }
            painter.end();
            d->validationInfo->setText( resultText );

            QRectF unzoomedMarker( xpos - xOffset()+u,
                                   ypos - yOffset()+v,
                                   len,
                                   hei );
            QRectF marker( viewConverter()->documentToView( unzoomedMarker ) );

            d->validationInfo->setGeometry( marker.toRect() );
            d->validationInfo->show();
        }
        else
        {
            delete d->validationInfo;
            d->validationInfo = 0;
        }
    }
    else
    {
        delete d->validationInfo;
        d->validationInfo = 0;
    }
}


void Canvas::scrollToCell(const QPoint& location) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;
    if (d->view->isLoading())
        return;

    // Adjust the maximum accessed column and row for the scrollbars.
    view()->sheetView(sheet)->updateAccessedCellRange(location);

    // The cell geometry expanded by the size of one column or one row, resp., in each direction.
    const Cell cell = Cell(sheet, location).masterCell();
    const double xpos = sheet->columnPosition(cell.cellPosition().x());
    const double ypos = sheet->rowPosition(cell.cellPosition().y());
    const double width = sheet->doc()->defaultColumnFormat()->width();
    const double height = sheet->doc()->defaultRowFormat()->height();
    QRectF rect(xpos, ypos, cell.width(), cell.height());
    rect.adjust(-width-2, -height-2, width+2, height+2);
    rect = rect & QRectF(QPointF(0.0, 0.0), sheet->documentSize());

    d->view->canvasController()->ensureVisible(rect, true);
}

void Canvas::setDocumentOffset( const QPoint& offset )
{
    const QPoint delta = offset - viewConverter()->documentToView( d->offset ).toPoint();
    d->offset = viewConverter()->viewToDocument( offset );

    //2008-03-15 sebsauer; crashes with Qt4.4 reproducable on loading with;
    //#5  0x00002b53a0d4e107 in QWidget::testAttribute (this=0x0, attribute=Qt::WA_UpdatesDisabled) at ../../include/QtGui/../../src/gui/kernel/qwidget.h:990
    //#6  0x00002b53a0d83ddc in QWidget::updatesEnabled (this=0x0) at ../../include/QtGui/../../src/gui/kernel/qwidget.h:946
    //hBorderWidget()->scroll( delta.x(), 0 );
    //vBorderWidget()->scroll( 0, delta.y() );
}

void Canvas::setDocumentSize( const QSizeF& size )
{
    const QSize s = viewConverter()->documentToView( size ).toSize();
    emit documentSizeChanged( s );
}

#if 0
void Canvas::slotScrollHorz( int _value )
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    kDebug(36005) <<"slotScrollHorz: value =" << _value;
    //kDebug(36005) << kBacktrace();

    if ( sheet->layoutDirection() == Qt::RightToLeft )
        _value = horzScrollBar()->maximum() - _value;

    if ( _value < 0 ) {
        kDebug (36001)
                << "Canvas::slotScrollHorz: value out of range (_value: "
                << _value << ')' << endl;
        _value = 0;
    }

    double xpos = sheet->columnPosition( qMin( KS_colMax, sheet->maxColumn()+10 ) ) - d->xOffset;
    if ( _value > ( xpos + d->xOffset ) )
        _value = (int) ( xpos + d->xOffset );

    // Relative movement
    // NOTE Stefan: Always scroll by whole pixels, otherwise we'll get offsets.
    int dx = qRound( viewConverter()->documentToViewX( d->xOffset - _value ) );

    // New absolute position
    // NOTE Stefan: Always store whole pixels, otherwise we'll get offsets.
    d->xOffset -=  viewConverter()->viewToDocumentX( dx );
    if ( d->xOffset < 0.05 )
        d->xOffset = 0.0;

    // scrolling the widgets in the right direction
    if ( sheet->layoutDirection() == Qt::RightToLeft )
        dx = -dx;
    scroll( dx, 0 );
    hBorderWidget()->scroll( dx, 0 );
}

void Canvas::slotScrollVert( int _value )
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    if ( _value < 0 )
    {
        _value = 0;
        kDebug (36001) <<"Canvas::slotScrollVert: value out of range (_value:" <<
                _value << ')' << endl;
    }

    double ypos = sheet->rowPosition( qMin( KS_rowMax, sheet->maxRow()+10 ) );
    if ( _value > ypos )
        _value = (int) ypos;

    // Relative movement
    // NOTE Stefan: Always scroll by whole pixels, otherwise we'll get offsets.
    int dy = qRound( viewConverter()->documentToViewY( d->yOffset - _value ) );
    scroll( 0, dy );
    vBorderWidget()->scroll( 0, dy );

    // New absolute position
    // NOTE Stefan: Always store whole pixels, otherwise we'll get offsets.
    d->yOffset -= viewConverter()->viewToDocumentY( dy );
    if ( d->yOffset < 0.05 )
        d->yOffset = 0.0;
}

void Canvas::slotMaxColumn( int _max_column )
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  int oldValue = horzScrollBar()->maximum() - horzScrollBar()->value();
  double xpos = sheet->columnPosition( qMin( KS_colMax, _max_column + 10 ) ) - xOffset();
  double unzoomWidth = viewConverter()->viewToDocumentX( width() );

  //Don't go beyond the maximum column range (KS_colMax)
  double sizeMaxX = sheet->documentSize().width();
  if ( xpos > sizeMaxX - xOffset() - unzoomWidth )
    xpos = sizeMaxX - xOffset() - unzoomWidth;

  horzScrollBar()->setRange( 0, (int) ( xpos + xOffset() ) );

  if ( sheet->layoutDirection() == Qt::RightToLeft )
    horzScrollBar()->setValue( horzScrollBar()->maximum() - oldValue );
}

void Canvas::slotMaxRow( int _max_row )
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  double ypos = sheet->rowPosition( qMin( KS_rowMax, _max_row + 10 ) ) - yOffset();
  double unzoomHeight = viewConverter()->viewToDocumentY( height() );

  //Don't go beyond the maximum row range (KS_rowMax)
  double sizeMaxY = sheet->documentSize().height();
  if ( ypos > sizeMaxY - yOffset() - unzoomHeight )
    ypos = sizeMaxY - yOffset() - unzoomHeight;

  vertScrollBar()->setRange( 0, (int) ( ypos + yOffset() ) );
}
#endif

void Canvas::mousePressEvent( QMouseEvent* event )
{
    // flake
    d->toolProxy->mousePressEvent( event, viewConverter()->viewToDocument( event->pos() ) + offset() );
}

void Canvas::mouseReleaseEvent( QMouseEvent* event )
{
    // flake
    d->toolProxy->mouseReleaseEvent( event, viewConverter()->viewToDocument( event->pos() ) + offset() );
}

void Canvas::mouseMoveEvent( QMouseEvent* event )
{
    // flake
    d->toolProxy->mouseMoveEvent( event, viewConverter()->viewToDocument( event->pos() ) + offset() );
}

void Canvas::mouseDoubleClickEvent( QMouseEvent* event )
{
    // flake
    d->toolProxy->mouseDoubleClickEvent( event, viewConverter()->viewToDocument( event->pos() ) + offset() );
}

void Canvas::keyPressEvent ( QKeyEvent* event )
{
    // flake
    d->toolProxy->keyPressEvent( event );
    if ( event->isAccepted() )
        return;
    // Don't eat accelerators
    if ( !( event->modifiers() & ( Qt::AltModifier | Qt::ControlModifier ) ) )
        event->accept();
}

bool Canvas::highlightRangeSizeGripAt(double x, double y)
{
  if ( !d->chooseCell )
    return false;

  Region::ConstIterator end = choice()->constEnd();
  for (Region::ConstIterator it = choice()->constBegin(); it != end; ++it)
  {
    // TODO Stefan: adapt to Selection::selectionHandleArea
    QRectF visibleRect = cellCoordinatesToDocument( (*it)->rect() );

    QPoint bottomRight((int) visibleRect.right(), (int) visibleRect.bottom());
    QRect handle( ( (int) bottomRight.x() - 6 ),
                  ( (int) bottomRight.y() - 6 ),
                  ( 6 ),
                  ( 6 ) );

    if (handle.contains(QPoint((int) x,(int) y)))
			{
				return true;
			}
	}

	return false;
}

void Canvas::startTheDrag()
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  // right area for start dragging
  setCursor( Qt::PointingHandCursor );

  QDomDocument doc = sheet->saveCellRegion(*selection(), true);

  // Save to buffer
  QBuffer buffer;
  buffer.open( QIODevice::WriteOnly );
  QTextStream str( &buffer );
  str.setCodec( "UTF-8" );
  str << doc;
  buffer.close();

  QMimeData* mimeData = new QMimeData();
  mimeData->setText( sheet->copyAsText( selection() ) );
  mimeData->setData( "application/x-kspread-snippet", buffer.buffer() );

  QDrag *drag = new QDrag(this);
  drag->setMimeData( mimeData );
  drag->start();

  setCursor( Qt::ArrowCursor );
}

void Canvas::paintEvent( QPaintEvent* event )
{
    if ( d->view->doc()->isLoading() || d->view->isLoading() )
        return;

    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    ElapsedTime et( "Painting cells", ElapsedTime::PrintOnlyTime );

    QPainter painter(this);
    painter.save();
    painter.setClipRegion( event->region() );

#if 0 // KSPREAD_KOPART_EMBEDDING
    //Save clip region
    QMatrix matrix;
    if ( d->view )
        matrix = d->view->matrix();
    else
        matrix = painter.matrix();

    paintChildren( painter, matrix );

//     painter.save();
    clipoutChildren( painter );
#endif // KSPREAD_KOPART_EMBEDDING

    painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );
    painter.scale( d->view->zoomHandler()->zoomedResolutionX(), d->view->zoomHandler()->zoomedResolutionY() );

    // erase background
    const QRectF paintRect = viewConverter()->viewToDocument(rect());
    painter.fillRect(paintRect, painter.background());

    // paint visible cells
    const QRect visibleRect = visibleCells();
    const QPointF topLeft( sheet->columnPosition(visibleRect.left()) - xOffset(),
                           sheet->rowPosition(visibleRect.top()) - yOffset() );
    view()->sheetView( sheet )->setPaintCellRange( visibleRect );
    view()->sheetView( sheet )->paintCells( this, painter, paintRect, topLeft );

    // flake
    painter.restore();
    painter.translate( -viewConverter()->documentToView( offset() ) );
    painter.setClipRegion( event->region().translated( viewConverter()->documentToView( offset() ).toPoint() ) );
    d->shapeManager->paint( painter, *viewConverter(), false );
    d->toolProxy->paint( painter, *viewConverter() );

    event->accept();
}

void Canvas::focusInEvent( QFocusEvent* )
{
  if ( !d->cellEditor )
    return;

  //kDebug(36005) <<"d->chooseCell :" << ( d->chooseCell ?"true" :"false" );
  // If we are in editing mode, we redirect the
  // focus to the CellEditor or EditWidget
  // And we know which, using lastEditorWithFocus.
  // This screws up <Tab> though (David)
  if ( lastEditorWithFocus() == EditWidget )
  {
    d->editWidget->setFocus();
    //kDebug(36005) <<"Focus to EditWidget";
    return;
  }

  //kDebug(36005) <<"Redirecting focus to editor";
  d->cellEditor->setFocus();
}

void Canvas::focusOutEvent( QFocusEvent* )
{
    d->mousePressed = false;
    d->view->disableAutoScroll();
}

void Canvas::dragEnterEvent( QDragEnterEvent* event )
{
  const QMimeData* mimeData = event->mimeData();
  if ( mimeData->hasText() ||
       mimeData->hasFormat( "application/x-kspread-snippet" ) )
  {
    event->acceptProposedAction();
  }
}

void Canvas::dragMoveEvent( QDragMoveEvent* event )
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
  {
    event->ignore();
    return;
  }

  const QMimeData* mimeData = event->mimeData();
  if ( mimeData->hasText() || mimeData->hasFormat( "application/x-kspread-snippet" ) )
  {
    event->acceptProposedAction();
  }
  else
  {
    event->ignore();
    return;
  }
#if 0 // TODO Stefan: implement drag marking rectangle
  QRect dragMarkingRect;
  if ( mimeData->hasFormat( "application/x-kspread-snippet" ) )
  {
    if ( event->source() == this  )
    {
      kDebug(36005) <<"source == this";
      dragMarkingRect = selection()->boundingRect();
    }
    else
    {
      kDebug(36005) <<"source != this";
      QByteArray data = mimeData->data( "application/x-kspread-snippet" );
      QString errorMsg;
      int errorLine;
      int errorColumn;
      QDomDocument doc;
      if ( !doc.setContent( data, false, &errorMsg, &errorLine, &errorColumn ) )
      {
        // an error occurred
        kDebug(36005) <<"Canvas::daragMoveEvent: an error occurred" << endl
                 << "line: " << errorLine << " col: " << errorColumn
                 << ' ' << errorMsg << endl;
        dragMarkingRect = QRect(1,1,1,1);
      }
      else
      {
        QDomElement root = doc.documentElement(); // "spreadsheet-snippet"
        dragMarkingRect = QRect(1,1,
                                root.attribute( "columns" ).toInt(),
                                root.attribute( "rows" ).toInt());
      }
    }
  }
  else // if ( mimeData->hasText() )
  {
    kDebug(36005) <<"has text";
    dragMarkingRect = QRect(1,1,1,1);
  }
#endif
  const QPoint dragAnchor = selection()->boundingRect().topLeft();
  double xpos = sheet->columnPosition( dragAnchor.x() );
  double ypos = sheet->rowPosition( dragAnchor.y() );
  double width  = sheet->columnFormat( dragAnchor.x() )->width();
  double height = sheet->rowFormat( dragAnchor.y() )->height();

  // consider also the selection rectangle
  const QRectF noGoArea( xpos - 1, ypos - 1, width + 3, height + 3 );

  // determine the current position
  double eventPosX;
  if (sheet->layoutDirection() == Qt::RightToLeft)
  {
    eventPosX = this->width() - viewConverter()->viewToDocumentX( event->pos().x() ) + xOffset();
  }
  else
  {
    eventPosX = viewConverter()->viewToDocumentX( event->pos().x() ) + xOffset();
  }
  double eventPosY = viewConverter()->viewToDocumentY( event->pos().y() ) + yOffset();

  if ( noGoArea.contains( QPointF( eventPosX, eventPosY ) ) )
  {
    event->ignore( noGoArea.toRect() );
    return;
  }

#if 0 // TODO Stefan: implement drag marking rectangle
  // determine the cell position under the mouse
  double tmp;
  const int col = sheet->leftColumn( eventPosX, tmp );
  const int row = sheet->topRow( eventPosY, tmp );
  dragMarkingRect.moveTo( QPoint( col, row ) );
  kDebug(36005) <<"MARKING RECT =" << dragMarkingRect;
#endif
}

void Canvas::dragLeaveEvent( QDragLeaveEvent * )
{
}

void Canvas::dropEvent( QDropEvent * _ev )
{
  d->dragging = false;
  d->view->disableAutoScroll();
  register Sheet * const sheet = activeSheet();
  if ( !sheet || sheet->isProtected() )
  {
    _ev->ignore();
    return;
  }

  double xpos = sheet->columnPosition( selection()->lastRange().left() );
  double ypos = sheet->rowPosition( selection()->lastRange().top() );
  double width  = sheet->columnFormat( selection()->lastRange().left() )->width();
  double height = sheet->rowFormat( selection()->lastRange().top() )->height();

  const QRectF noGoArea( xpos - 1, ypos - 1, width + 3, height + 3 );

  double ev_PosX;
  if ( sheet->layoutDirection() == Qt::RightToLeft )
    ev_PosX = this->width() - viewConverter()->viewToDocumentX( _ev->pos().x() ) + xOffset();
  else
    ev_PosX = viewConverter()->viewToDocumentX( _ev->pos().x() ) + xOffset();

  double ev_PosY = viewConverter()->viewToDocumentY( _ev->pos().y() ) + yOffset();

  if ( noGoArea.contains( QPointF( ev_PosX, ev_PosY ) ) )
  {
    _ev->ignore();
    return;
  }
  else
    _ev->setAccepted(true);

  double tmp;
  int col = sheet->leftColumn( ev_PosX, tmp );
  int row = sheet->topRow( ev_PosY, tmp );

  const QMimeData* mimeData = _ev->mimeData();
  if ( !mimeData->hasText() && !mimeData->hasFormat( "application/x-kspread-snippet" ) )
  {
    _ev->ignore();
    return;
  }

  QByteArray b;

  bool makeUndo = true;

  if ( mimeData->hasFormat( "application/x-kspread-snippet" ) )
  {
    if ( _ev->source() == this  )
    {
      if ( !d->view->doc()->undoLocked() )
      {
        UndoDragDrop * undo
          = new UndoDragDrop(d->view->doc(), sheet, *selection(),
                             Region(QRect(col, row,
                                   selection()->boundingRect().width(),
                                   selection()->boundingRect().height())));
        d->view->doc()->addCommand( undo );
        makeUndo = false;
      }
      DeleteCommand* command = new DeleteCommand();
      command->setSheet( activeSheet() );
      command->add( *selection() );
      command->setRegisterUndo( false );
      command->execute();
    }


    b = mimeData->data( "application/x-kspread-snippet" );
    sheet->paste( b, QRect( col, row, 1, 1 ), makeUndo );

    _ev->setAccepted(true);
  }
  else
  {
    QString text = mimeData->text();
    sheet->pasteTextPlain( text, QRect( col, row, 1, 1 ) );
    _ev->setAccepted(true);
    return;
  }
}

QPoint Canvas::cursorPos()
{
  QPoint cursor;
  if (d->chooseCell && !choice()->isEmpty())
    cursor = choice()->cursor();
  else
    cursor = selection()->cursor();

  return cursor;
}

QRect Canvas::moveDirection( KSpread::MoveTo direction, bool extendSelection )
{
  kDebug(36005) <<"Canvas::moveDirection";

  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return QRect();

  QPoint destination;
  QPoint cursor = cursorPos();

  QPoint cellCorner = cursor;
  Cell cell( sheet,cursor.x(), cursor.y());

  /* cell is either the same as the marker, or the cell that is forced obscuring
     the marker cell
  */
  if (cell.isPartOfMerged())
  {
    cell = cell.masterCell();
    cellCorner = QPoint(cell.column(), cell.row());
  }

  /* how many cells must we move to get to the next cell? */
  int offset = 0;
  const RowFormat *rl = 0;
  const ColumnFormat *cl = 0;
  switch (direction)
    /* for each case, figure out how far away the next cell is and then keep
       going one row/col at a time after that until a visible row/col is found

       NEVER use cell.column() or cell.row() -- it might be a default cell
    */
  {
    case Bottom:
      offset = cell.mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = sheet->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHiddenOrFiltered())
      {
        offset++;
        rl = sheet->rowFormat( cursor.y() + offset );
      }

      destination = QPoint(cursor.x(), qMin(cursor.y() + offset, KS_rowMax));
      break;
    case Top:
      offset = (cellCorner.y() - cursor.y()) - 1;
      rl = sheet->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) >= 1) && rl->isHiddenOrFiltered())
      {
        offset--;
        rl = sheet->rowFormat( cursor.y() + offset );
      }
      destination = QPoint(cursor.x(), qMax(cursor.y() + offset, 1));
      break;
    case Left:
      offset = (cellCorner.x() - cursor.x()) - 1;
      cl = sheet->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) >= 1) && cl->isHiddenOrFiltered())
      {
        offset--;
        cl = sheet->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(qMax(cursor.x() + offset, 1), cursor.y());
      break;
    case Right:
      offset = cell.mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
      cl = sheet->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) <= KS_colMax) && cl->isHiddenOrFiltered())
      {
        offset++;
        cl = sheet->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(qMin(cursor.x() + offset, KS_colMax), cursor.y());
      break;
    case BottomFirst:
      offset = cell.mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = sheet->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHiddenOrFiltered())
      {
        ++offset;
        rl = sheet->rowFormat( cursor.y() + offset );
      }

      destination = QPoint( 1, qMin( cursor.y() + offset, KS_rowMax ) );
      break;
  }

  if (extendSelection)
  {
    (d->chooseCell ? choice() : selection())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selection())->initialize(destination, sheet);
  }
  d->view->updateEditWidget();

  return QRect( cursor, destination );
}

void Canvas::processEnterKey(QKeyEvent* event)
{
  // array is true, if ctrl+alt are pressed
  bool array = (event->modifiers() & Qt::AltModifier) &&
      (event->modifiers() & Qt::ControlModifier);

  /* save changes to the current editor */
  if (!d->chooseCell)
  {
    deleteEditor(true, array);
  }

  /* use the configuration setting to see which direction we're supposed to move
     when enter is pressed.
  */
  KSpread::MoveTo direction = d->view->doc()->moveToValue();

  //if shift Button clicked inverse move direction
  if (event->modifiers() & Qt::ShiftModifier)
  {
    switch( direction )
    {
     case Bottom:
      direction = Top;
      break;
     case Top:
      direction = Bottom;
      break;
     case Left:
      direction = Right;
      break;
     case Right:
      direction = Left;
      break;
     case BottomFirst:
      direction = BottomFirst;
      break;
    }
  }

  /* never extend a selection with the enter key -- the shift key reverses
     direction, not extends the selection
  */
  QRect r( moveDirection( direction, false ) );
}

void Canvas::processArrowKey( QKeyEvent *event)
{
  /* NOTE:  hitting the tab key also calls this function.  Don't forget
     to account for it
  */
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  /* save changes to the current editor */
  if (!d->chooseCell)
  {
    deleteEditor( true );
  }

  KSpread::MoveTo direction = Bottom;
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  switch (event->key())
  {
  case Qt::Key_Down:
    direction = Bottom;
    break;
  case Qt::Key_Up:
    direction = Top;
    break;
  case Qt::Key_Left:
    if (sheet->layoutDirection() == Qt::RightToLeft)
      direction = Right;
    else
      direction = Left;
    break;
  case Qt::Key_Right:
    if (sheet->layoutDirection() == Qt::RightToLeft)
      direction = Left;
    else
      direction = Right;
    break;
  case Qt::Key_Tab:
      direction = Right;
      break;
  case Qt::Key_Backtab:
      //Shift+Tab moves to the left
      direction = Left;
      makingSelection = false;
      break;
  default:
    Q_ASSERT(false);
    break;
  }

  QRect r( moveDirection( direction, makingSelection ) );
}

void Canvas::processEscapeKey(QKeyEvent * event)
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  if ( d->cellEditor )
    deleteEditor( false );

#if 0 // KSPREAD_KOPART_EMBEDDING
  if ( view()->isInsertingObject() )
  {
    view()->resetInsertHandle();
    setCursor( Qt::ArrowCursor );
    return;
  }
#endif // KSPREAD_KOPART_EMBEDDING

  event->setAccepted(true); // ?
  QPoint cursor = cursorPos();

#if 0 // KSPREAD_KOPART_EMBEDDING
  if ( d->mousePressed /*&& toolEditMode == TEM_MOUSE */)
  {
    switch (d->modType)
    {
      case MT_RESIZE_UP:
      case MT_RESIZE_DN:
      case MT_RESIZE_LF:
      case MT_RESIZE_RT:
      case MT_RESIZE_LU:
      case MT_RESIZE_LD:
      case MT_RESIZE_RU:
      case MT_RESIZE_RD:
      {
        QRect oldBoundingRect = d->view->zoomHandler()->zoomRectOld( d->m_resizeObject->geometry()/*getRepaintRect()*/);
        d->m_resizeObject->setGeometry( d->m_rectBeforeResize );
        oldBoundingRect.translate( (int)( -xOffset()*d->view->zoomHandler()->zoomedResolutionX() ) ,
                            (int)( -yOffset() * d->view->zoomHandler()->zoomedResolutionY()) );

        repaint( oldBoundingRect );
        repaintObject( d->m_resizeObject );
        d->m_ratio = 0.0;
        d->m_resizeObject = 0;
        d->m_isResizing = false;
        view()->disableAutoScroll();
        d->mousePressed = false;
        d->modType = MT_NONE;
        break;
      }
      case MT_MOVE:
      {
        if ( d->m_isMoving )
        {
          QPointF move( d->m_moveStartPoint - objectRect( false ).topLeft() );
          sheet->moveObject( view(), move, false );
          view()->disableAutoScroll();
          d->mousePressed = false;
          d->modType = MT_NONE;
          d->m_isMoving = false;
          update();
        }
        break;
      }
      default:
        break;
    }
  }
#endif // KSPREAD_KOPART_EMBEDDING
}

bool Canvas::processHomeKey(QKeyEvent* event)
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return false;

  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  if ( d->cellEditor )
  // We are in edit mode -> go beginning of line
  {
    QApplication::sendEvent( d->editWidget, event );
    return false;
  }
  else
  {
    QPoint destination;
    /* start at the first used cell in the row and cycle through the right until
       we find a cell that has some output text.  But don't look past the current
       marker.
       The end result we want is to move to the left to the first cell with text,
       or just to the first column if there is no more text to the left.

       But why?  In excel, home key sends you to the first column always.
       We might want to change to that behavior.
    */

    if (event->modifiers() & Qt::ControlModifier)
    {
      /* ctrl + Home will always just send us to location (1,1) */
      destination = QPoint( 1, 1 );
    }
    else
    {
      QPoint marker = d->chooseCell ? choice()->marker() : selection()->marker();

      Cell cell = sheet->cellStorage()->firstInRow(marker.y(), CellStorage::VisitContent);
      while (!cell.isNull() && cell.column() < marker.x() && cell.isEmpty())
      {
        cell = sheet->cellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
      }

      int col = ( !cell.isNull() ? cell.column() : 1 );
      if ( col == marker.x())
        col = 1;
      destination = QPoint(col, marker.y());
    }

    if ( selection()->marker() == destination )
      return false;

    if (makingSelection)
    {
      (d->chooseCell ? choice() : selection())->update(destination);
    }
    else
    {
      (d->chooseCell ? choice() : selection())->initialize(destination, sheet);
    }
  }
  return true;
}

bool Canvas::processEndKey( QKeyEvent *event )
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return false;

  bool makingSelection = event->modifiers() & Qt::ShiftModifier;
  Cell cell;
  QPoint marker = d->chooseCell ? choice()->marker() : selection()->marker();

  // move to the last used cell in the row
  // We are in edit mode -> go beginning of line
  if ( d->cellEditor )
  {
    QApplication::sendEvent( d->editWidget, event );
    return false;
  }
  else
  {
    int col = 1;

    cell = sheet->cellStorage()->lastInRow(marker.y(), CellStorage::VisitContent);
    while (!cell.isNull() && cell.column() > selection()->marker().x() && cell.isEmpty())
    {
      cell = sheet->cellStorage()->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
    }

    col = (cell.isNull()) ? KS_colMax : cell.column();

    QPoint destination( col, marker.y() );
    if ( destination == marker )
      return false;

    if (makingSelection)
    {
      (d->chooseCell ? choice() : selection())->update(destination);
    }
    else
    {
      (d->chooseCell ? choice() : selection())->initialize(destination, sheet);
    }
  }
  return true;
}

bool Canvas::processPriorKey(QKeyEvent *event)
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;
  if (!d->chooseCell)
  {
    deleteEditor( true );
  }

  QPoint marker = d->chooseCell ? choice()->marker() : selection()->marker();

  QPoint destination(marker.x(), qMax(1, marker.y() - 10));
  if ( destination == marker )
    return false;

  if (makingSelection)
  {
    (d->chooseCell ? choice() : selection())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selection())->initialize(destination, activeSheet());
  }
  return true;
}

bool Canvas::processNextKey(QKeyEvent *event)
{
  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  if (!d->chooseCell)
  {
    deleteEditor( true /*save changes*/ );
  }

  QPoint marker = d->chooseCell ? choice()->marker() : selection()->marker();
  QPoint destination(marker.x(), qMax(1, marker.y() + 10));

  if ( marker == destination )
    return false;

  if (makingSelection)
  {
    (d->chooseCell ? choice() : selection())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selection())->initialize(destination, activeSheet());
  }
  return true;
}

void Canvas::processDeleteKey(QKeyEvent* event)
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

#if 0 // KSPREAD_KOPART_EMBEDDING
  if ( isObjectSelected() )
  {
    d->view->deleteSelectedObjects();
    return;
  }
#endif // KSPREAD_KOPART_EMBEDDING

  // Delete is also a valid editing key, process accordingly
  if ( !d->cellEditor && !d->chooseCell )
  {
      // Switch to editing mode
      createEditor();
      d->cellEditor->handleKeyPressEvent( event );
  }
  else if ( d->cellEditor )
      d->cellEditor->handleKeyPressEvent( event );

  d->view->clearContents();
  d->editWidget->setText( "" );
}

void Canvas::processF2Key(QKeyEvent* /* event */)
{
  d->editWidget->setFocus();
  if ( d->cellEditor )
    d->editWidget->setCursorPosition( d->cellEditor->cursorPosition() - 1 );
  d->editWidget->cursorForward( false );
}

void Canvas::processF4Key(QKeyEvent* event)
{
  /* passes F4 to the editor (if any), which will process it
   */
  if ( d->cellEditor )
  {
    d->cellEditor->handleKeyPressEvent( event );
//    d->editWidget->setFocus();
    d->editWidget->setCursorPosition( d->cellEditor->cursorPosition() );
  }
}

void Canvas::processOtherKey(QKeyEvent *event)
{
    register Sheet * const sheet = activeSheet();

    // No null character ...
    if ( event->text().isEmpty() || !d->view->koDocument()->isReadWrite() ||
         !sheet || sheet->isProtected() )
    {
        event->setAccepted(true);
    }
    else
    {
        if ( !d->cellEditor && !d->chooseCell )
        {
            // Switch to editing mode
            createEditor();
            d->cellEditor->handleKeyPressEvent( event );
        }
        else if ( d->cellEditor )
            d->cellEditor->handleKeyPressEvent( event );
    }
}

bool Canvas::processControlArrowKey( QKeyEvent *event )
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return false;

  bool makingSelection = event->modifiers() & Qt::ShiftModifier;

  Cell cell;
  Cell lastCell;
  QPoint destination;
  bool searchThroughEmpty = true;
  int row;
  int col;

  QPoint marker = d->chooseCell ? choice()->marker() : selection()->marker();

  /* here, we want to move to the first or last cell in the given direction that is
     actually being used.  Ignore empty cells and cells on hidden rows/columns */
  switch ( event->key() )
  {
    //Ctrl+Qt::Key_Up
   case Qt::Key_Up:

    cell = Cell( sheet, marker.x(), marker.y() );
    if ( (!cell.isNull()) && (!cell.isEmpty()) && (marker.y() != 1))
    {
      lastCell = cell;
      row = marker.y()-1;
      cell = Cell( sheet,cell.column(), row);
      while ((!cell.isNull()) && (row > 0) && (!cell.isEmpty()) )
      {
        if (!sheet->rowFormat(cell.row())->isHiddenOrFiltered())
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        row--;
        if ( row > 0 )
          cell = Cell( sheet,cell.column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->cellStorage()->prevInColumn(marker.x(), marker.y(), CellStorage::VisitContent);

      while ((!cell.isNull()) &&
            (cell.isEmpty() || (sheet->rowFormat(cell.row())->isHiddenOrFiltered())))
      {
        cell = sheet->cellStorage()->prevInColumn(cell.column(), cell.row(), CellStorage::VisitContent);
      }
    }

    if (cell.isNull())
      row = 1;
    else
      row = cell.row();

    while ( sheet->rowFormat(row)->isHiddenOrFiltered() )
    {
      row++;
    }

    destination.setX(qBound(1, marker.x(), KS_colMax));
    destination.setY(qBound(1, row, KS_rowMax));
    break;

    //Ctrl+Qt::Key_Down
   case Qt::Key_Down:

    cell = Cell( sheet, marker.x(), marker.y() );
    if ( (!cell.isNull()) && (!cell.isEmpty()) && (marker.y() != KS_rowMax))
    {
      lastCell = cell;
      row = marker.y()+1;
      cell = Cell( sheet,cell.column(), row);
      while ((!cell.isNull()) && (row < KS_rowMax) && (!cell.isEmpty()) )
      {
        if (!(sheet->rowFormat(cell.row())->isHiddenOrFiltered()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        row++;
        cell = Cell( sheet,cell.column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->cellStorage()->nextInColumn(marker.x(), marker.y(), CellStorage::VisitContent);

      while ((!cell.isNull()) &&
            (cell.isEmpty() || (sheet->rowFormat(cell.row())->isHiddenOrFiltered())))
      {
        cell = sheet->cellStorage()->nextInColumn(cell.column(), cell.row(), CellStorage::VisitContent);
      }
    }

    if (cell.isNull())
      row = marker.y();
    else
      row = cell.row();

    while ( sheet->rowFormat(row)->isHiddenOrFiltered() )
    {
      row--;
    }

    destination.setX(qBound(1, marker.x(), KS_colMax));
    destination.setY(qBound(1, row, KS_rowMax));
    break;

  //Ctrl+Qt::Key_Left
  case Qt::Key_Left:

  if ( sheet->layoutDirection() == Qt::RightToLeft )
  {
    cell = Cell( sheet, marker.x(), marker.y() );
    if ( (!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = Cell( sheet,col, cell.row());
      while ((!cell.isNull()) && (col < KS_colMax) && (!cell.isEmpty()) )
      {
        if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col++;
        cell = Cell( sheet,col, cell.row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->cellStorage()->nextInRow(marker.x(), marker.y(), CellStorage::VisitContent);

      while ((!cell.isNull()) &&
            (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered())))
      {
        cell = sheet->cellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
      }
    }

    if (cell.isNull())
      col = marker.x();
    else
      col = cell.column();

    while ( sheet->columnFormat(col)->isHiddenOrFiltered() )
    {
      col--;
    }

    destination.setX(qBound(1, col, KS_colMax));
    destination.setY(qBound(1, marker.y(), KS_rowMax));
  }
  else
  {
    cell = Cell( sheet, marker.x(), marker.y() );
    if ( (!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = Cell( sheet,col, cell.row());
      while ((!cell.isNull()) && (col > 0) && (!cell.isEmpty()) )
      {
        if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col--;
        if ( col > 0 )
            cell = Cell( sheet,col, cell.row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->cellStorage()->prevInRow(marker.x(), marker.y(), CellStorage::VisitContent);

      while ((!cell.isNull()) &&
            (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered())))
      {
        cell = sheet->cellStorage()->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
      }
    }

    if (cell.isNull())
      col = 1;
    else
      col = cell.column();

    while ( sheet->columnFormat(col)->isHiddenOrFiltered() )
    {
      col++;
    }

    destination.setX(qBound(1, col, KS_colMax));
    destination.setY(qBound(1, marker.y(), KS_rowMax));
  }
    break;

  //Ctrl+Qt::Key_Right
  case Qt::Key_Right:

  if ( sheet->layoutDirection() == Qt::RightToLeft )
  {
    cell = Cell( sheet, marker.x(), marker.y() );
    if ( (!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = Cell( sheet,col, cell.row());
      while ((!cell.isNull()) && (col > 0) && (!cell.isEmpty()) )
      {
        if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col--;
        if ( col > 0 )
            cell = Cell( sheet,col, cell.row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->cellStorage()->prevInRow(marker.x(), marker.y(), CellStorage::VisitContent);

      while ((!cell.isNull()) &&
            (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered())))
      {
        cell = sheet->cellStorage()->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
      }
    }

    if (cell.isNull())
      col = 1;
    else
      col = cell.column();

    while ( sheet->columnFormat(col)->isHiddenOrFiltered() )
    {
      col++;
    }

    destination.setX(qBound(1, col, KS_colMax));
    destination.setY(qBound(1, marker.y(), KS_rowMax));
  }
  else
  {
    cell = Cell( sheet, marker.x(), marker.y() );
    if ( (!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = Cell( sheet,col, cell.row());
      while ((!cell.isNull()) && (col < KS_colMax) && (!cell.isEmpty()) )
      {
        if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col++;
        cell = Cell( sheet,col, cell.row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->cellStorage()->nextInRow(marker.x(), marker.y(), CellStorage::VisitContent);

      while ((!cell.isNull()) &&
            (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered())))
      {
        cell = sheet->cellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
      }
    }

    if (cell.isNull())
      col = marker.x();
    else
      col = cell.column();

    while ( sheet->columnFormat(col)->isHiddenOrFiltered() )
    {
      col--;
    }

    destination.setX(qBound(1, col, KS_colMax));
    destination.setY(qBound(1, marker.y(), KS_rowMax));
  }
    break;

  }

  if ( marker == destination )
    return false;

  if (makingSelection)
  {
    (d->chooseCell ? choice() : selection())->update(destination);
  }
  else
  {
    (d->chooseCell ? choice() : selection())->initialize(destination, sheet);
  }
  return true;
}

#if 0
void Canvas::processIMEvent( QIMEvent * event )
{
  if ( !d->cellEditor && !d->chooseCell )
  {
    // Switch to editing mode
    createEditor();
    d->cellEditor->handleIMEvent( event );
  }

  QPoint cursor;

  if ( d->chooseCell )
  {
    cursor = choice()->cursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = choice()->cursor();
  }
  else
    cursor = selection()->cursor();
}
#endif

bool Canvas::formatKeyPress( QKeyEvent * _ev )
{
    if (!(_ev->modifiers() & Qt::ControlModifier ))
        return false;

    int key = _ev->key();
    if ( key != Qt::Key_Exclam && key != Qt::Key_At &&
         key != Qt::Key_Ampersand && key != Qt::Key_Dollar &&
         key != Qt::Key_Percent && key != Qt::Key_AsciiCircum &&
         key != Qt::Key_NumberSign )
        return false;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( activeSheet() );

    switch ( _ev->key() )
    {
        case Qt::Key_Exclam:
            manipulator->setText( i18n("Number Format") );
            manipulator->setFormatType( Format::Number );
            manipulator->setPrecision( 2 );
            break;

        case Qt::Key_Dollar:
            manipulator->setText( i18n("Currency Format") );
            manipulator->setFormatType( Format::Money );
            manipulator->setPrecision( d->view->doc()->locale()->fracDigits() );
            break;

        case Qt::Key_Percent:
            manipulator->setText( i18n("Percentage Format") );
            manipulator->setFormatType( Format::Percentage );
            break;

        case Qt::Key_At:
            manipulator->setText( i18n("Time Format") );
            manipulator->setFormatType( Format::SecondeTime );
            break;

        case Qt::Key_NumberSign:
            manipulator->setText( i18n("Date Format") );
            manipulator->setFormatType( Format::ShortDate );
            break;

        case Qt::Key_AsciiCircum:
            manipulator->setText( i18n("Scientific Format") );
            manipulator->setFormatType( Format::Scientific );
            break;

        case Qt::Key_Ampersand:
            manipulator->setText( i18n("Change Border") );
            manipulator->setTopBorderPen( QPen( d->view->borderColor(), 1, Qt::SolidLine) );
            manipulator->setBottomBorderPen( QPen( d->view->borderColor(), 1, Qt::SolidLine) );
            manipulator->setLeftBorderPen( QPen( d->view->borderColor(), 1, Qt::SolidLine) );
            manipulator->setRightBorderPen( QPen( d->view->borderColor(), 1, Qt::SolidLine) );
            break;

        default:
            delete manipulator;
            return false;
    }

    manipulator->add( *selection() );
    manipulator->execute();
    _ev->setAccepted(true);

    return true;
}

void Canvas::slotAutoScroll(const QPoint &scrollDistance)
{
  // NOTE Stefan: This slot is triggered by the same signal as
  //              HBorder::slotAutoScroll and VBorder::slotAutoScroll.
  //              Therefore, nothing has to be done except the scrolling was
  //              initiated in the canvas.
  if (!d->mousePressed)
    return;
  d->view->canvasController()->scrollContentsBy(scrollDistance.x(), scrollDistance.y());
}

#if 0 // KSPREAD_KOPART_EMBEDDING
KSpread::EmbeddedObject *Canvas::getObject( const QPoint &pos, Sheet *_sheet )
{
  QPoint const p ( (int) pos.x() ,
              (int) pos.y() );

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( object->sheet() == _sheet )
    {
        QRectF const bound = ( object )->geometry();
        QRect zoomedBound = d->view->zoomHandler()->zoomRectOld( QRectF(bound.left(), bound.top(),
                                bound.width(),
                                bound.height() ) );
        zoomedBound.translate( (int)( -xOffset() * d->view->zoomHandler()->zoomedResolutionX() ), (int)( -yOffset() * d->view->zoomHandler()->zoomedResolutionY() ) );
         if ( zoomedBound.contains( p ) )
              return object;
    }
  }
  return 0;
}

void Canvas::selectObject( EmbeddedObject *obj )
{
  if ( obj->sheet() != activeSheet() || obj->isSelected() )
    return;
  obj->setSelected( true );
  repaintObject( obj );

  d->mouseSelectedObject = true;
  emit objectSelectedChanged();
  deleteEditor( true );
}

void Canvas::deselectObject( EmbeddedObject *obj )
{
  if ( obj->sheet() != activeSheet() || !obj->isSelected() )
    return;
  obj->setSelected( false );
  repaintObject( obj );

  d->mouseSelectedObject = false;
  emit objectSelectedChanged();
}

void Canvas::selectAllObjects()
{
  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( object->sheet() == activeSheet() )
      object->setSelected( true );
  }

   d->mouseSelectedObject = true;
//   emit objectSelectedChanged();
}

void Canvas::deselectAllObjects()
{
  if( activeSheet()->numberSelectedObjects() == 0 )
    return;

  //lowerObject();

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
      deselectObject( object );

   d->mouseSelectedObject = false;
//   emit objectSelectedChanged();
}



void Canvas::setMouseSelectedObject(bool b)
{
  d->mouseSelectedObject = b;
  emit objectSelectedChanged();
}

bool Canvas::isObjectSelected()
{
  return d->mouseSelectedObject;
}


void Canvas::moveObjectsByMouse( QPointF &pos, bool keepXorYunchanged )
{
  QRectF rect( objectRect( false ) );
  QPointF move( 0, 0 );
  double diffx = pos.x() - d->m_origMousePos.x();
  double diffy = pos.y() - d->m_origMousePos.y();

  move = QPointF( diffx, diffy );
  d->m_origMousePos = pos;

    // unwind last snapping
  QRectF movedRect( rect );
  movedRect.translate( diffx, diffy );

    // don't move object off canvas
  QPointF diffDueToBorders(0,0);
//   KoRect pageRect( m_activePage->getPageRect() );
  if ( rect.left() + move.x() < 0/*pageRect.left()*/ )
    diffDueToBorders.setX( -rect.left() - move.x() );
//   else if ( rect.right() + move.x() > pageRect.right() )
//     diffDueToBorders.setX( pageRect.right() - (rect.right() + move.x()) );


  //kDebug(36005) <<"rect.top() + move.y():" << rect.top() + move.y();
  if ( rect.top() + move.y() < 0 )
    diffDueToBorders.setY( -rect.top() - move.y() );
//   else if ( rect.bottom() + move.y() > pageRect.bottom() )
//     diffDueToBorders.setY( pageRect.bottom() - (rect.bottom() + move.y()) );

//   m_moveSnapDiff += diffDueToBorders;
  move += diffDueToBorders;

//   movedRect.moveBy( m_moveSnapDiff.x(), m_moveSnapDiff.y() );
  if ( keepXorYunchanged )
  {
    QPointF diff( d->m_moveStartPosMouse - movedRect.topLeft() );
    if ( fabs( diff.x() ) > fabs( diff.y() ) )
    {
//       m_moveSnapDiff.setY( /*m_moveSnapDiff.y() + */m_moveStartPosMouse.y() - movedRect.y() );
      movedRect.moveTopLeft( QPointF( movedRect.x(), d->m_moveStartPosMouse.y() ) );
      move.setY( movedRect.y() - rect.y() );
    }
    else
    {
//       m_moveSnapDiff.setX( /*m_moveSnapDiff.x() + */m_moveStartPosMouse.x() - movedRect.x() );
      movedRect.moveTopLeft( QPointF( d->m_moveStartPosMouse.x(), movedRect.y() ) );
      move.setX( movedRect.x() - rect.x() );
    }
  }

  if ( move != QPointF( 0, 0 ) )
  {
        //kDebug(33001) <<"moveObjectsByMouse move =" << move;
    activeSheet()->moveObject( view(), move, false );
  }
}


void Canvas::resizeObject( ModifyType _modType, const QPointF & point, bool keepRatio )
{
    EmbeddedObject *obj = d->m_resizeObject;

    QRectF objRect = obj->geometry();
    /*objRect.moveBy( -xOffset(), -yOffset() );*/
    objRect.translate(-xOffset(), -yOffset() );
    QRect oldBoundingRect( d->view->zoomHandler()->zoomRectOld( objRect ) );

    bool left = false;
    bool right = false;
    bool top = false;
    bool bottom = false;
    if ( _modType == MT_RESIZE_UP || _modType == MT_RESIZE_LU || _modType == MT_RESIZE_RU )
    {
        top = true;
//         snapStatus |= KoGuides::SNAP_HORIZ;
    }
    if ( _modType == MT_RESIZE_DN || _modType == MT_RESIZE_LD || _modType == MT_RESIZE_RD )
    {
        bottom = true;
//         snapStatus |= KoGuides::SNAP_HORIZ;
    }
    if ( _modType == MT_RESIZE_LF || _modType == MT_RESIZE_LU || _modType == MT_RESIZE_LD )
    {
        left = true;
//         snapStatus |= KoGuides::SNAP_VERT;
    }
    if ( _modType == MT_RESIZE_RT || _modType == MT_RESIZE_RU || _modType == MT_RESIZE_RD )
    {
        right = true;
//         snapStatus |= KoGuides::SNAP_VERT;
    }

    double newLeft = objRect.left();
    double newRight = objRect.right();
    double newTop = objRect.top();
    double newBottom = objRect.bottom();
    if ( top )
    {
        if ( point.y() < objRect.bottom() - MIN_SIZE )
        {
            newTop = point.y();
        }
        else
        {
            newTop = objRect.bottom() - MIN_SIZE;
        }
    }
    if ( bottom )
    {
        if ( point.y() > objRect.top() + MIN_SIZE )
        {
            newBottom = point.y();
        }
        else
        {
            newBottom = objRect.top() + MIN_SIZE;
        }
    }
    if ( left )
    {
        if ( point.x() < objRect.right() - MIN_SIZE )
        {
            newLeft = point.x();
        }
        else
        {
            newLeft = objRect.right() - MIN_SIZE;
        }
    }
    if ( right )
    {
        if ( point.x() > objRect.left() + MIN_SIZE )
        {
            newRight = point.x();
        }
        else
        {
            newRight = objRect.left() + MIN_SIZE;
        }
    }

  double width = newRight - newLeft;
  double height = newBottom - newTop;

  if ( keepRatio && d->m_ratio != 0 )
  {
    if ( ( top || bottom ) && ( right || left ) )
    {
      if ( height * height * d->m_ratio > width * width / d->m_ratio )
      {
        width = height * d->m_ratio;
      }
      else
      {
        height = width / d->m_ratio;
      }
    }
    else if ( top || bottom )
    {
      width = height * d->m_ratio;
    }
    else
    {
      height = width / d->m_ratio;
    }

    if ( top )
    {
      newTop = objRect.bottom() - height;
    }
    else
    {
      newBottom = objRect.top() + height;
    }
    if ( left )
    {
      newLeft = objRect.right() - width;
    }
    else
    {
      newRight = objRect.right() + width;
    }
  }

  if ( newLeft != objRect.left() || newRight != objRect.right() || newTop != objRect.top() || newBottom != objRect.bottom() )
  {
        // resizeBy and moveBy have to been used to make it work with rotated objects
        obj->resizeBy( width - objRect.width(), height - objRect.height() );

        if ( objRect.left() != newLeft || objRect.top() != newTop )
        {
            obj->moveBy( QPointF( newLeft - objRect.left(), newTop - objRect.top() ) );
        }

//     if ( doc()->showGuideLines() && !m_disableSnapping )
//     {
//       QRectF rect( obj->getRealRect() );
//       QPointF sp( rect.topLeft() );
//       if ( right )
//       {
//         sp.setX( rect.right() );
//       }
//       if ( bottom )
//       {
//         sp.setY( rect.bottom() );
//       }
//       m_gl.repaintSnapping( sp, snapStatus );
//     }

    repaint( oldBoundingRect );
    repaintObject( obj );
    emit objectSizeChanged();
  }
}


void Canvas::finishResizeObject( const QString &/*name*/, bool /*layout*/ )
{
  if ( d->m_resizeObject )
  {
    QPointF move = QPointF( d->m_resizeObject->geometry().x() - d->m_rectBeforeResize.x(),
                            d->m_resizeObject->geometry().y() - d->m_rectBeforeResize.y() );
    QSizeF size = QSizeF( d->m_resizeObject->geometry().width() - d->m_rectBeforeResize.width(),
                          d->m_resizeObject->geometry().height() - d->m_rectBeforeResize.height() );

    if ( ( d->m_resizeObject->geometry() ) != d->m_rectBeforeResize )
    {
        ChangeObjectGeometryCommand *resizeCmd = new ChangeObjectGeometryCommand( d->m_resizeObject, move, size );
        // the command is not executed as the object is already resized.
        doc()->addCommand( resizeCmd );
    }

//     if ( layout )
//       doc()->layout( m_resizeObject );

    d->m_ratio = 0.0;
    d->m_isResizing = false;
    repaintObject( d->m_resizeObject );
    d->m_resizeObject = 0;
  }
}

void Canvas::raiseObject( EmbeddedObject *object )
{
    if ( doc()->embeddedObjects().count() <= 1 )
        return;

    if ( d->m_objectDisplayAbove == 0 )
    {
        if ( activeSheet()->numberSelectedObjects() == 1 )
        {
            d->m_objectDisplayAbove = object;
        }
    }
}

void Canvas::lowerObject()
{
    d->m_objectDisplayAbove = 0;
}

void Canvas::displayObjectList( QList<EmbeddedObject*> &list )
{
  list = doc()->embeddedObjects();

    if ( d->m_objectDisplayAbove )
    {
        // it can happen that the object is no longer there e.g. when
        // the insert of the object is undone
        int pos = doc()->embeddedObjects().indexOf( d->m_objectDisplayAbove );
        if ( pos != -1 && d->m_objectDisplayAbove->isSelected() )
        {
            list.removeAt( pos );
            list.append( d->m_objectDisplayAbove );
        }
        else
        {
            //tz not possible due to const. should const be removed?
            //m_objectDisplayAbove = 0;
        }
    }
}


QRectF Canvas::objectRect( bool all ) const
{
  return activeSheet()->getRealRect( all );
}
#endif // KSPREAD_KOPART_EMBEDDING

void Canvas::deleteEditor (bool saveChanges, bool array)
{
  if ( !d->cellEditor )
    return;

  //There may be highlighted areas on the sheet which will need to be erased
  d->view->markSelectionAsDirty();

  d->editWidget->setEditMode( false );

  QString t = d->cellEditor->text();
  d->cellEditor->hide();
  // Delete the cell editor first and after that update the document.
  // That means we get a synchronous repaint after the cell editor
  // widget is gone. Otherwise we may get painting errors.
  delete d->cellEditor;
  d->cellEditor = 0;

  if ( saveChanges )
  {
      if ( (!t.isEmpty()) && (t.at(0)=='=') )
      {
          //a formula
          int openParenthese = t.count( '(' );
          int closeParenthese = t.count( ')' );
          int diff = qAbs( openParenthese - closeParenthese );
          if ( openParenthese > closeParenthese )
          {
              for (int i=0; i < diff;i++)
              {
                  t=t+')';
              }
          }
      }
    d->view->setText (t, array);
  }
  else
  {
    d->view->updateEditWidget();
  }

  setFocus();
}


bool Canvas::createEditor( bool clear,  bool focus )
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return false;

    Cell cell( sheet, selection()->marker() );

    if ( sheet->isProtected() && !cell.style().notProtected() )
        return false;

    // Set the starting sheet of the choice.
    choice()->setActiveSheet( sheet );
    choice()->setOriginSheet( sheet );

    if ( !d->cellEditor )
    {
        d->editWidget->setEditMode( true );
        d->cellEditor = new KSpread::CellEditor( cell, this, doc()->captureAllArrowKeys() );

        double w = cell.width();
        double h = cell.height();
        double min_w = cell.width();
        double min_h = cell.height();

        double xpos = sheet->columnPosition( selection()->marker().x() ) - xOffset();

        Qt::LayoutDirection sheetDir = sheet->layoutDirection();
        bool rtlText = cell.displayText().isRightToLeft();

        // if sheet and cell direction don't match, then the editor's location
        // needs to be shifted backwards so that it's right above the cell's text
        if ( w > 0 && ( ( sheetDir == Qt::RightToLeft && !rtlText ) ||
             ( sheetDir == Qt::LeftToRight && rtlText  ) ) )
            xpos -= w - min_w;

        // paint editor above correct cell if sheet direction is RTL
        if ( sheetDir == Qt::RightToLeft )
        {
            double dwidth = viewConverter()->viewToDocumentX( width() );
            double w2 = qMax( w, min_w );
            xpos = dwidth - w2 - xpos;
        }

        double ypos = sheet->rowPosition( selection()->marker().y() ) - yOffset();
        QPalette editorPalette( d->cellEditor->palette() );

        QColor color = cell.style().fontColor();

        if ( !color.isValid() )
            color = palette().text().color();
        editorPalette.setColor( QPalette::Text, color );

        color = Cell( sheet, selection()->marker() ).effectiveStyle().backgroundColor();
        if ( !color.isValid() )
            color = editorPalette.base().color();
        editorPalette.setColor( QPalette::Background, color );

        d->cellEditor->setPalette( editorPalette );

        QFont tmpFont = cell.style().font();

        tmpFont.setPointSizeF( 0.01 * d->view->zoomHandler()->zoomInPercent() * tmpFont.pointSizeF() );
        d->cellEditor->setFont( tmpFont );

        QRectF rect( xpos, ypos, w, h ); //needed to circumvent rounding issue with height/width


        QRect zoomedRect = viewConverter()->documentToView( rect ).toRect();
        /*zoomedRect.setLeft(zoomedRect.left()-2);
        zoomedRect.setRight(zoomedRect.right()+4);
        zoomedRect.setTop(zoomedRect.top()-1);
        zoomedRect.setBottom(zoomedRect.bottom()+2);*/

        d->cellEditor->setGeometry( zoomedRect );
        d->cellEditor->setMinimumSize( QSize( (int)d->view->zoomHandler()->documentToViewX( min_w ),
                                              (int)d->view->zoomHandler()->documentToViewY( min_h ) ) );
        d->cellEditor->show();

        // Laurent 2001-12-05
        // Don't add focus when we create a new editor and
        // we select text in edit widget otherwise we don't delete
        // selected text.
        if ( focus )
            d->cellEditor->setFocus();

        d->view->markSelectionAsDirty();
    }

    if ( !clear && !cell.isNull() )
        d->cellEditor->setText( cell.userInput() );

    return true;
}

#if 0 // KSPREAD_KOPART_EMBEDDING
void Canvas::repaintObject( EmbeddedObject *obj )
{
	//Calculate where the object appears on the canvas widget and then repaint that part of the widget
	QRect canvasRelativeGeometry = d->view->zoomHandler()->zoomRectOld( obj->geometry() );
	canvasRelativeGeometry.translate( (int)( -xOffset()*d->view->zoomHandler()->zoomedResolutionX() ) ,
			   			(int)( -yOffset() * d->view->zoomHandler()->zoomedResolutionY()) );

    update( canvasRelativeGeometry );

 /* if ( !obj->isSelected() )
  {
    QRectF g = obj->geometry();
    g.moveBy( -xOffset(), -yOffset() );
    QRect geometry( d->view->zoomHandler()->zoomRectOld( g ) );

    update( geometry );
  }
  else
  {
    QPainter p(this);
    p.translate( -xOffset() * d->view->zoomHandler()->zoomedResolutionX() , -yOffset() * d->view->zoomHandler()->zoomedResolutionY() );
    obj->draw(&p); //this goes faster than calling repaint
    p.end();
  }*/
}

void Canvas::copyOasisObjects()
{
    // We'll create a store (ZIP format) in memory
    QBuffer buffer;
    QByteArray mimeType = "application/vnd.oasis.opendocument.spreadsheet";
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, mimeType );
    Q_ASSERT( store );
    Q_ASSERT( !store->bad() );
    KoOasisStore oasisStore( store );

    KoXmlWriter* manifestWriter = oasisStore.manifestWriter( mimeType );

    QString plainText;
    KoPicture picture;
    if ( !doc()->saveOasisHelper( store, manifestWriter, Doc::SaveSelected, &plainText, &picture )
         || !oasisStore.closeManifestWriter() )
    {
        delete store;
        return;
    }
    delete store;
    QMimeData* mimeData = new QMimeData();
    if ( !plainText.isEmpty() )
        mimeData->setText( plainText );
#ifdef __GNUC__
#warning TODO KDE4 portage: Drag'n'drop of KoPicture
#endif
#if 0
    if ( !picture.isNull() )
        multiDrag->setMimeData( picture.dragObject( 0 ) );
#endif
    kDebug(36003) <<"setting zip data:" << buffer.buffer().size() <<" bytes.";
    mimeData->setData( mimeType, buffer.buffer() );

    //save the objects as pictures too so that other programs can access them
    foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
    {
      QRectF kr = objectRect(false);
      QRect r( kr.toRect() );
      QPixmap pixmap( r.width(), r.height() );
      pixmap.fill( "white" );
      QPainter p(&pixmap);
      if ( object->isSelected() )
      {
          p.drawPixmap( object->geometry().toRect().left() - r.left(), object->geometry().toRect().top() - r.top(), object->toPixmap() );
      }
      p.end();
      if (!pixmap.isNull())
      {
        mimeData->setImageData( pixmap.toImage() );
      }
    }

    QApplication::clipboard()->setMimeData( mimeData, QClipboard::Clipboard );
}
#endif // KSPREAD_KOPART_EMBEDDING

void Canvas::closeEditor()
{
  if ( d->chooseCell )
    return;

  if ( d->cellEditor )
  {
    deleteEditor( true ); // save changes
  }
}

void Canvas::updateEditor()
{
    if ( !d->chooseCell )
        return;

    register Sheet * const sheet = activeSheet();
    Q_ASSERT( sheet );

    if ( d->cellEditor )
    {
        if ( choice()->originSheet() != sheet )
            d->cellEditor->hide();
        else
            d->cellEditor->show();
        d->cellEditor->updateChoice();
    }
}

void Canvas::updatePosWidget()
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

    QString buffer;
    // No selection, or only one cell merged selected
    if ( selection()->isSingular() )
    {
        if (sheet->getLcMode())
        {
            buffer = 'L' + QString::number( selection()->marker().y() ) +
		'C' + QString::number( selection()->marker().x() );
        }
        else
        {
            buffer = Cell::columnName( selection()->marker().x() ) +
		QString::number( selection()->marker().y() );
        }
    }
    else
    {
        if (sheet->getLcMode())
        {
          buffer = QString::number( (selection()->lastRange().bottom()-selection()->lastRange().top()+1) )+"Lx";
          if ( Region::Range( selection()->lastRange() ).isRow() )
            buffer+=QString::number((KS_colMax-selection()->lastRange().left()+1))+'C';
            else
              buffer+=QString::number((selection()->lastRange().right()-selection()->lastRange().left()+1))+'C';
        }
        else
        {
                //encodeColumnLabelText return @@@@ when column >KS_colMax
                //=> it's not a good display
                //=> for the moment I display pos of marker
          buffer=Cell::columnName( selection()->lastRange().left() ) +
                    QString::number(selection()->lastRange().top()) + ':' +
                    Cell::columnName( qMin( KS_colMax, selection()->lastRange().right() ) ) +
                    QString::number(selection()->lastRange().bottom());
                //buffer=sheet->columnLabel( m_iMarkerColumn );
                //buffer+=tmp.setNum(m_iMarkerRow);
        }
  }

    if (buffer != d->posWidget->lineEdit()->text())
      d->posWidget->lineEdit()->setText(buffer);
}

void Canvas::equalizeRow()
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  QRect s( selection()->lastRange() );
  const RowFormat* rowFormat = sheet->rowFormat(s.top());
  double size = rowFormat->height();
  if ( s.top() == s.bottom() )
      return;
  for ( int i = s.top() + 1; i <= s.bottom(); i++ )
  {
      size = qMax( sheet->rowFormat(i)->height(), size );
  }
  d->view->vBorderWidget()->equalizeRow(size);
}

void Canvas::equalizeColumn()
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  QRect s( selection()->lastRange() );
  const ColumnFormat* columnFormat = sheet->columnFormat(s.left());
  double size = columnFormat->width();
  if ( s.left() == s.right() )
      return;

  for(int i=s.left()+1;i<=s.right();i++)
  {
    size = qMax( sheet->columnFormat(i)->width(), size );
  }
  d->view->hBorderWidget()->equalizeColumn(size);
}

QRect Canvas::viewToCellCoordinates( const QRectF& viewRect ) const
{
  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return QRect();

  const QRectF rect = d->view->zoomHandler()->viewToDocument( viewRect ).translated( offset() );

  double tmp;
  const int left = sheet->leftColumn( rect.left(), tmp );
  const int right = sheet->rightColumn( rect.right() );
  const int top = sheet->topRow( rect.top(), tmp );
  const int bottom = sheet->bottomRow( rect.bottom() );

  return QRect( left, top, right - left + 1, bottom - top + 1 );
}

QRect Canvas::visibleCells() const
{
  return viewToCellCoordinates( rect() );
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void Canvas::clipoutChildren( QPainter& painter ) const
{
  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, width(), height() ) );

  const double horizontalOffset = -xOffset() * d->view->zoomHandler()->zoomedResolutionX();
  const double verticalOffset = -yOffset() * d->view->zoomHandler()->zoomedResolutionY();

#if 0 // KSPREAD_KOPART_EMBEDDING
  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( ( object )->sheet() == activeSheet() )
    {
	QRect childGeometry = d->view->zoomHandler()->zoomRectOld( object->geometry());

	//The clipping region is given in device coordinates
	//so subtract the current offset (scroll position) of the canvas
	childGeometry.translate( (int)horizontalOffset , (int)verticalOffset );

	if (painter.window().intersects(childGeometry))
		rgn -= childGeometry;

      //painter.fillRect( d->view->zoomHandler()->zoomRectOld( object->geometry() ), QColor("red" ) );
    }
  }
#endif // KSPREAD_KOPART_EMBEDDING

  painter.setClipRegion( rgn );
}

QRect Canvas::painterWindowGeometry( const QPainter& painter ) const
{
  QRect zoomedWindowGeometry = painter.window();

  zoomedWindowGeometry.translate( (int)( xOffset() * d->view->zoomHandler()->zoomedResolutionX() ) , (int)( yOffset() * d->view->zoomHandler()->zoomedResolutionY() ) );

	return zoomedWindowGeometry;
}

#if 0 // KSPREAD_KOPART_EMBEDDING
void Canvas::paintChildren( QPainter& painter, QMatrix& /*matrix*/ )
{
  if ( doc()->embeddedObjects().isEmpty() )
    return;

  register Sheet * const sheet = activeSheet();
  if (!sheet)
    return;

  painter.save();
  painter.translate( -xOffset() * d->view->zoomHandler()->zoomedResolutionX() , -yOffset() * d->view->zoomHandler()->zoomedResolutionY() );

  const QRect zoomedWindowGeometry = painterWindowGeometry( painter );

  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    QRect const zoomedObjectGeometry = d->view->zoomHandler()->zoomRectOld( object->geometry() );
    if ( ( object )->sheet() == sheet &&
           zoomedWindowGeometry.intersects( zoomedObjectGeometry ) )
    {
	    //To prevent unnecessary redrawing of the embedded object, we only repaint
	    //if one or more of the cells underneath the object has been marked as 'dirty'.

	   QRect canvasRelativeGeometry = zoomedObjectGeometry;
	   canvasRelativeGeometry.translate( (int)( -xOffset()*d->view->zoomHandler()->zoomedResolutionX() ) ,
			   			(int)( -yOffset() * d->view->zoomHandler()->zoomedResolutionY()) );

	   const QRect cellsUnderObject = viewToCellCoordinates( canvasRelativeGeometry );
	   bool redraw=false;

      Region paintDirtyList = sheet->paintDirtyData();
      Region::ConstIterator end(paintDirtyList.constEnd());
      for (Region::ConstIterator it(paintDirtyList.constBegin()); it != end; ++it)
      {
        QRect range = (*it)->rect() & cellsUnderObject;
        int right = range.right();
        for (int x = range.left(); x <= right; ++x)
        {
          int bottom = range.bottom();
          for (int y = range.top(); y <= bottom; ++y)
          {
            redraw=true;
            break;
          }
        }
      }
      if ( redraw )
        object->draw( &painter );
    }
  }
  painter.restore();
}
#endif // KSPREAD_KOPART_EMBEDDING

void Canvas::paintHighlightedRanges(QPainter& painter, const QRectF& /*viewRect*/)
{
    // save painter state
    painter.save();
    painter.translate( offset() );

  const QList<QColor> colors = choice()->colors();
  int index = 0;
  Region::ConstIterator end(choice()->constEnd());
  for (Region::ConstIterator it = choice()->constBegin(); it != end; ++it)
  {
    //Only paint ranges or cells on the current sheet
    if ((*it)->sheet() != activeSheet())
    {
      index++;
      continue;
    }

    const QRect range = choice()->extendToMergedAreas( (*it)->rect() );

    QRectF unzoomedRect = cellCoordinatesToDocument( range ).translated( -xOffset(), -yOffset() );

    //Convert region from sheet coordinates to canvas coordinates for use with the painter
    //retrieveMarkerInfo(region,viewRect,positions,paintSides);

    painter.setPen( colors[(index) % colors.size()] );

    //Now adjust the highlight rectangle is slightly inside the cell borders (this means that multiple highlighted cells
    //look nicer together as the borders do not clash)
    const double unzoomedXPixel = d->view->zoomHandler()->unzoomItX( 1.0 );
    const double unzoomedYPixel = d->view->zoomHandler()->unzoomItY( 1.0 );

    unzoomedRect.adjust( unzoomedXPixel, unzoomedYPixel, -unzoomedXPixel, -unzoomedYPixel );

    painter.setBrush( QBrush() );
    painter.drawRect( unzoomedRect );

    //Now draw the size grip (the little rectangle on the bottom right-hand corner of the range which the user can
    //click and drag to resize the region)

    painter.setPen( Qt::white );
    painter.setBrush( colors[(index) % colors.size()] );

    painter.drawRect( QRectF( unzoomedRect.right() - 3 * unzoomedXPixel,
                              unzoomedRect.bottom() - 3 * unzoomedYPixel,
                              6 * unzoomedXPixel,
                              6 * unzoomedYPixel ) );
    index++;
  }

    // restore painter state
    painter.restore();
}

void Canvas::paintNormalMarker(QPainter& painter, const QRectF &viewRect)
{
    if( d->chooseCell )
        return;
    if (d->cellEditor)
        return;

    // save the painter state
    painter.save();
    painter.translate( offset() );
    // disable antialiasing
    painter.setRenderHint( QPainter::Antialiasing, false );
    // Extend the clip rect by one in each direction to avoid artefacts caused by rounding errors.
    // TODO Stefan: This unites the region's rects. May be bad. Check!
    painter.setClipRegion( painter.clipRegion().boundingRect().adjusted(-1,-1,1,1) );

    QLineF line;
    QPen pen( Qt::black, d->view->zoomHandler()->unzoomItX( 2 ) );
    painter.setPen( pen );

    const KSpread::Selection* selection = this->selection();
    const QRect currentRange = selection->extendToMergedAreas(QRect(selection->anchor(), selection->marker()));
    const QRect effMarker = selection->extendToMergedAreas( QRect( selection->marker(), selection->marker() ) );
    const QRectF markerRegion = viewConverter()->viewToDocument( cellCoordinatesToView( effMarker ) );
    Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it)
    {
        const QRect range = (*it)->isAll() ? (*it)->rect() : selection->extendToMergedAreas( (*it)->rect() );

        // Only the active element (the one with the anchor) will be drawn with a border
        const bool current = (currentRange == range);

        double positions[4];
        bool paintSides[4];
        retrieveMarkerInfo( range, viewRect, positions, paintSides );

        double left =   positions[0];
        double top =    positions[1];
        double right =  positions[2];
        double bottom = positions[3];

        bool paintLeft =   paintSides[0];
        bool paintTop =    paintSides[1];
        bool paintRight =  paintSides[2];
        bool paintBottom = paintSides[3];

        // get the transparent selection color
        QColor selectionColor( QApplication::palette().highlight().color() );
        selectionColor.setAlpha( 127 );
        if ( current )
        {
            // save old clip region
            const QRegion clipRegion = painter.clipRegion();
            // clip out the marker region
            painter.setClipRegion( clipRegion.subtracted( markerRegion.toRect() ) );
            // draw the transparent selection background
            painter.fillRect( QRectF( left, top, right - left, bottom - top ), selectionColor );
            // restore clip region
            painter.setClipRegion( clipRegion );
        }
        else
            // draw the transparent selection background
            painter.fillRect( QRectF( left, top, right - left, bottom - top ), selectionColor );

        if ( paintTop )
        {
            line = QLineF( left, top, right, top );
            painter.drawLine( line );
        }
        if ( activeSheet()->layoutDirection() == Qt::RightToLeft )
        {
            if ( paintRight )
            {
                line = QLineF( right, top, right, bottom );
                painter.drawLine( line );
            }
            if ( paintLeft && paintBottom && current )
            {
                /* then the 'handle' in the bottom left corner is visible. */
                line = QLineF( left, top, left, bottom - d->view->zoomHandler()->unzoomItY( 3 ) );
                painter.drawLine( line );
                line = QLineF( left + d->view->zoomHandler()->unzoomItX( 4 ),  bottom, right + d->view->zoomHandler()->unzoomItY( 1 ), bottom );
                painter.drawLine( line );
                painter.fillRect( QRectF( left - d->view->zoomHandler()->unzoomItX( 2 ), bottom - d->view->zoomHandler()->unzoomItY( 2 ),
                                  d->view->zoomHandler()->unzoomItX( 5 ), d->view->zoomHandler()->unzoomItY( 5 ) ), painter.pen().color() );
            }
            else
            {
                if ( paintLeft )
                {
                    line = QLineF( left, top, left, bottom );
                    painter.drawLine( line );
                }
                if ( paintBottom )
                {
                    line = QLineF( left, bottom, right, bottom );
                    painter.drawLine( line );
                }
            }
        }
        else // activeSheet()->layoutDirection() == Qt::LeftToRight
        {
            if ( paintLeft )
            {
                line = QLineF( left, top, left, bottom );
                painter.drawLine( line );
            }
            if ( paintRight && paintBottom && current )
            {
                /* then the 'handle' in the bottom right corner is visible. */
                line = QLineF( right, top, right, bottom - d->view->zoomHandler()->unzoomItY( 3 ) );
                painter.drawLine( line );
                line = QLineF( left, bottom, right - d->view->zoomHandler()->unzoomItX( 3 ), bottom );
                painter.drawLine( line );
                painter.fillRect( QRectF( right - d->view->zoomHandler()->unzoomItX( 2 ), bottom - d->view->zoomHandler()->unzoomItY( 2 ),
                                  d->view->zoomHandler()->unzoomItX( 5 ), d->view->zoomHandler()->unzoomItY( 5 ) ), painter.pen().color() );
            }
            else
            {
                if ( paintRight )
                {
                    line = QLineF( right, top, right, bottom );
                    painter.drawLine( line );
                }
                if ( paintBottom )
                {
                    line = QLineF( left, bottom, right, bottom );
                    painter.drawLine( line );
                }
            }
        }
    }
    // restore painter state
    painter.restore();
}

QRectF Canvas::cellCoordinatesToDocument( const QRect& cellRange ) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return QRectF();

    QRectF rect;
    rect.setLeft  ( sheet->columnPosition( cellRange.left() ) );
    rect.setRight ( sheet->columnPosition(cellRange.right()) + sheet->columnFormat(cellRange.right())->width());
    rect.setTop   ( sheet->rowPosition( cellRange.top() ) );
    rect.setBottom( sheet->rowPosition(cellRange.bottom()) + sheet->rowFormat(cellRange.bottom())->height());
    return rect;
}

QRectF Canvas::cellCoordinatesToView( const QRect& cellRange ) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return QRectF();

    QRectF rect = cellCoordinatesToDocument( cellRange );
    // apply scrolling offset
    rect.translate( -xOffset(), -yOffset() );
    // convert it to view coordinates
    rect = d->view->zoomHandler()->documentToView( rect );
    // apply layout direction
    if ( sheet->layoutDirection() == Qt::RightToLeft )
    {
        const double left = rect.left();
        const double right = rect.right();
        rect.setLeft( width() - right );
        rect.setRight( width() - left );
    }
    return rect;
}

void Canvas::retrieveMarkerInfo( const QRect &cellRange,
                                 const QRectF &viewRect,
                                 double positions[],
                                 bool paintSides[] )
{
    const Sheet* sheet = activeSheet();
    const QRectF visibleRect = cellCoordinatesToDocument( cellRange ).translated( -xOffset(), -yOffset() );

    /* these vars are used for clarity, the array for simpler function arguments  */
    double left = visibleRect.left();
    double top = visibleRect.top();
    double right = visibleRect.right();
    double bottom = visibleRect.bottom();
    if ( sheet->layoutDirection() == Qt::RightToLeft )
    {
        const double docWidth = d->view->zoomHandler()->unzoomItX( width() );
        left = docWidth - visibleRect.right();
        right = docWidth - visibleRect.left();
    }

    /* left, top, right, bottom */
    paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
    paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom()) &&
                    (right >= viewRect.left()) && (left <= viewRect.right());
    paintSides[2] = (viewRect.left() <= right ) && (right <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
    paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom()) &&
                    (right >= viewRect.left()) && (left <= viewRect.right());

    positions[0] = qMax( left,   viewRect.left() );
    positions[1] = qMax( top,    viewRect.top() );
    positions[2] = qMin( right,  viewRect.right() );
    positions[3] = qMin( bottom, viewRect.bottom() );
}

void Canvas::showToolTip( const QPoint& p )
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    // Over which cell is the mouse ?
    double ypos, xpos;
    double dwidth = d->view->zoomHandler()->viewToDocumentX( width() );
    int col;
    if ( sheet->layoutDirection() == Qt::RightToLeft )
      col = sheet->leftColumn( (dwidth - d->view->zoomHandler()->viewToDocumentX( p.x() ) +
                                              xOffset()), xpos );
    else
      col = sheet->leftColumn( (d->view->zoomHandler()->viewToDocumentX( p.x() ) +
                                     xOffset()), xpos );


    int row = sheet->topRow( (d->view->zoomHandler()->viewToDocumentY( p.y() ) +
                                   yOffset()), ypos );

    Cell cell = Cell(sheet, col, row).masterCell();
    CellView cellView = view()->sheetView(sheet)->cellView(cell.column(), cell.row());
    if (cellView.isObscured())
    {
        cell = Cell(sheet, cellView.obscuringCell());
        cellView = view()->sheetView(sheet)->cellView(cellView.obscuringCell().x(), cellView.obscuringCell().y());
    }

    // displayed tool tip, which has the following priorities:
    //  - cell content if the cell dimension is too small
    //  - cell comment
    //  - hyperlink
    // Ensure that it is plain text.
    // Not funny if (intentional or not) <a> appears as hyperlink.
    QString tipText;
    // If cell is too small, show the content
    if (!cellView.dimensionFits())
        tipText = cell.displayText().replace('<', "&lt;");

    // Show hyperlink, if any
    if ( tipText.isEmpty() )
        tipText = cell.link().replace('<', "&lt;");

    // Nothing to display, bail out
    if (tipText.isEmpty() && cell.comment().isEmpty())
      return;

    // Cut if the tip is ridiculously long
    const int maxLen = 256;
    if ( tipText.length() > maxLen )
        tipText = tipText.left(maxLen).append("...");

    // Determine position and width of the current cell.
    const double cellWidth = cellView.cellWidth();
    const double cellHeight = cellView.cellHeight();

    // Get the cell dimensions
    QRect cellRect;
    bool insideCellRect = false;
    if ( sheet->layoutDirection() == Qt::RightToLeft )
    {
        const QRectF rect(dwidth - cellWidth - xpos + xOffset(), ypos - yOffset(), cellWidth, cellHeight);
        cellRect = viewConverter()->documentToView(rect).toRect();
        insideCellRect = cellRect.contains( p );
    }
    else
    {
        QRectF rect(xpos - xOffset(), ypos - yOffset(), cellWidth, cellHeight);
        cellRect = viewConverter()->documentToView(rect).toRect();
        insideCellRect = cellRect.contains( p );
    }

    // No use if mouse is somewhere else
    if ( !insideCellRect )
        return;

    // Show comment, if any.
    if (tipText.isEmpty())
        tipText = cell.comment().replace('<', "&lt;");
    else if (!cell.comment().isEmpty())
        tipText += "</p><h4>" + i18n("Comment:") + "</h4><p>" + cell.comment().replace('<', "&lt;");

    // Now we show the tip
    QToolTip::showText(mapToGlobal(cellRect.bottomRight()),
                       "<p>" + tipText.replace('\n', "<br>") + "</p>",
                       this, cellRect.translated(-mapToGlobal(cellRect.topLeft())));
}

int Canvas::metric( PaintDeviceMetric metric ) const
{
    switch( metric )
    {
        case QPaintDevice::PdmDpiX:
        case QPaintDevice::PdmDpiY:
        case QPaintDevice::PdmPhysicalDpiX:
        case QPaintDevice::PdmPhysicalDpiY:
            return 72;
        default:
            break;
    }
    return QWidget::metric( metric );
}

void Canvas::updateInputMethodInfo() {
    updateMicroFocus();
}

#include "Canvas.moc"
