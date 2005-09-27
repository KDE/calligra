/* This file is part of the KDE project

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
   Copyright 1999 Michael Reiher <michael.reiher.gmx.de>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <assert.h>
#include <float.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qbuffer.h>
#include <qdrawutil.h>
#include <qlabel.h>
#include <qpoint.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <krun.h>

#include "kspread_doc.h"
#include "kspread_editors.h"
#include "kspread_global.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_selection.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "kspread_view.h"
#include "kspread_selection.h"

#include "kspread_canvas.h"


class CanvasPrivate
{
  public:
  
    KSpreadComboboxLocationEditWidget *posWidget;
    KSpreadEditWidget *editWidget;
    KSpreadCellEditor *cellEditor;
    
    bool choose_visible;
    int  length_namecell;
    int  length_text;

    KSpreadView *view;
    QTimer* scrollTimer;

    // Non visible range left from current screen
    // Example: If the first visible column is 'E', then xOffset stores
    // the width of the invisible columns 'A' to 'D'.
    double xOffset;

    // Non visible range on top of the current screen
    // Example: If the first visible row is '5', then yOffset stores
    // the height of the invisible rows '1' to '4'.
    double yOffset;

    // Used to draw the grey grid that is usually only visible on the
    // screen, but not by printing on paper.
    QPen defaultGridPen;

    // see setLastEditorWithFocus, lastEditorWithFocus
    KSpreadCanvas::EditorType focusEditorType;

    QLabel *validationInfo;

    // true if the user is to choose a cell.
    bool chooseCell;
    
    // if a choose selection is started (@ref #startChoose) the current
    KSpreadSheet* chooseStartSheet;
    
    // True when the mouse button is pressed
    bool mousePressed;

    // If the user is dragging around with the mouse then this tells us what he is doing.
    // The user may want to mark cells or he started in the lower right corner
    // of the marker which is something special. The values for the 2 above
    // methods are called 'Mark' and 'ResizeCell' or 'AutoFill' depending
    // on the mouse button used. By default this variable holds
    // the value 'NoAction'.
    KSpreadCanvas::MouseActions mouseAction;

    // If we use the lower right corner of the marker to start autofilling, then this
    // rectangle conatins all cells that were already marker when the user started
    // to mark the rectangle which he wants to become autofilled.
    QRect autoFillSource;

    // Start coordinates for drag and drop
    QPoint dragStart;
    bool dragging;

    // Used to indicate whether the user started drawing a rubber band rectangle
    bool rubberBandStarted;
    QPoint rubberBandStart;
    QPoint rubberBandEnd;

    // If the mouse is over some anchor ( in the sense of HTML anchors )
    QString anchor;
    
    //Stores information about cells referenced in the formula currently being edited,
    //Used to highlight relevant cells
    std::vector<HighlightRange>* highlightedRanges;
    
    //The highlighted range being resized by the user (using the size grip, or null if the user
    //is not resizing a range.
    HighlightRange* sizingHighlightRange;
    
   // bool mouseOverHighlightRangeSizeGrip;
};



/****************************************************************
 *
 * KSpreadCanvas
 *
 ****************************************************************/

KSpreadCanvas::KSpreadCanvas (KSpreadView *_view)
  : QWidget( _view, "", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase )
{
  d = new CanvasPrivate;


  d->length_namecell = 0;
  d->chooseStartSheet = NULL;
  d->cellEditor = 0;
  d->chooseCell = false;
  d->validationInfo = 0L;
  d->highlightedRanges = 0;
  d->sizingHighlightRange = 0;
 // d->mouseOverHighlightRangeSizeGrip = false;

  QWidget::setFocusPolicy( QWidget::StrongFocus );

  d->dragStart = QPoint( -1, -1 );
  d->dragging = false;
  
  
  d->defaultGridPen.setColor( lightGray );
  d->defaultGridPen.setWidth( 1 );
  d->defaultGridPen.setStyle( SolidLine );

  d->xOffset = 0.0;
  d->yOffset = 0.0;
  d->view = _view;
  // m_eAction = DefaultAction;
  d->mouseAction = NoAction;
  d->rubberBandStarted = false;
  // m_bEditDirtyFlag = false;

  //Now built afterwards(David)
  //d->editWidget = d->view->editWidget();
  d->posWidget = d->view->posWidget();

  setBackgroundMode( PaletteBase );

  setMouseTracking( true );
  d->mousePressed = false;

  d->scrollTimer = new QTimer( this );
  connect (d->scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );

  d->choose_visible = false;
  setFocus();
  installEventFilter( this );
  (void)new KSpreadToolTip( this );
  setAcceptDrops( true );
  setInputMethodEnabled( true ); // ensure using the InputMethod
}

KSpreadCanvas::~KSpreadCanvas()
{
  delete d->scrollTimer;
  delete d->validationInfo;
  delete d->highlightedRanges;
  delete d;
}

KSpreadView* KSpreadCanvas::view()
{
  return d->view;
}

KSpreadDoc* KSpreadCanvas::doc()
{
  return d->view->doc();
}

void KSpreadCanvas::setEditWidget( KSpreadEditWidget * ew ) 
{ 
  d->editWidget = ew; 
}

KSpreadEditWidget* KSpreadCanvas::editWidget() const 
{ 
  return d->editWidget; 
}

KSpreadCellEditor* KSpreadCanvas::editor() const 
{ 
  return d->cellEditor; 
}

double KSpreadCanvas::xOffset() const 
{ 
  return d->xOffset; 
}

double KSpreadCanvas::yOffset() const 
{ 
  return d->yOffset; 
}

void KSpreadCanvas::setXOffset( double _xOffset )
{
  d->xOffset = _xOffset;
  kdDebug(36001) << "setXOffset(): XOffset before scrollToCell: " 
		 << d->xOffset << endl;
  scrollToCell( marker() );
  kdDebug(36001) << "setXOffset(): XOffset after scrollToCell: " 
		 << d->xOffset << endl;
}

void KSpreadCanvas::setYOffset( double _yOffset )
{
  d->yOffset = _yOffset;
  kdDebug(36001) << "setyOffset(): YOffset before scrollToCell: " 
		 << d->yOffset << endl;
  scrollToCell( marker() );
  kdDebug(36001) << "setYOffset(): YOffset after scrollToCell: " 
		 << d->yOffset << endl;
}

const QPen& KSpreadCanvas::defaultGridPen() const 
{ 
  return d->defaultGridPen; 
}

void KSpreadCanvas::setLastEditorWithFocus( KSpreadCanvas::EditorType type )
{ 
  d->focusEditorType = type; 
}

KSpreadCanvas::EditorType KSpreadCanvas::lastEditorWithFocus() const
{ 
  return d->focusEditorType; 
}

  
bool KSpreadCanvas::eventFilter( QObject *o, QEvent *e )
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
    if ((keyev->key()==Key_Tab) || (keyev->key()==Key_Backtab))
    {
      keyPressEvent ( keyev );
      return true;
    }
    break;
  }
  case QEvent::IMStart:
  case QEvent::IMCompose:
  case QEvent::IMEnd:
  {
      QIMEvent * imev = static_cast<QIMEvent *>(e);
      processIMEvent( imev );
      break;
  }
  default:
    break;
  }
  return false;
}

bool KSpreadCanvas::focusNextPrevChild( bool )
{
    return true; // Don't allow to go out of the canvas widget by pressing "Tab"
}

int KSpreadCanvas::chooseTextLen() const
{
  return d->length_namecell;
}

KSpreadSelection* KSpreadCanvas::selectionInfo() const
{
  return d->view->selectionInfo();
}

QRect KSpreadCanvas::selection() const
{
  return d->view->selectionInfo()->selection();
}

QPoint KSpreadCanvas::marker() const
{
    return d->view->selectionInfo()->marker();
}

int KSpreadCanvas::markerColumn() const
{
    return d->view->selectionInfo()->marker().x();
}

int KSpreadCanvas::markerRow() const
{
    return d->view->selectionInfo()->marker().y();
}

double KSpreadCanvas::zoom() const
{
  return d->view->zoom();
}

bool KSpreadCanvas::chooseMode() const
{ 
  return d->chooseCell; 
}

void KSpreadCanvas::startChoose()
{
  if ( d->chooseCell )
    return;

  updateChooseRect(QPoint(0,0), QPoint(0,0));

  // It is important to enable this AFTER we set the rect!
  d->chooseCell = true;
  d->chooseStartSheet = activeSheet();
}

void KSpreadCanvas::startChoose( const QRect& rect )
{
  if (d->chooseCell)
    return;

  updateChooseRect(rect.bottomRight(), rect.topLeft());

  // It is important to enable this AFTER we set the rect!
  d->chooseCell = true;
  d->chooseStartSheet = activeSheet();
}

void KSpreadCanvas::endChoose()
{
  if ( !d->chooseCell )
    return;

  updateChooseRect(QPoint(0,0), QPoint(0,0));

  d->length_namecell = 0;
  d->chooseCell = false;

  KSpreadSheet *sheet=d->view->doc()->map()->findSheet(d->chooseStartSheet->sheetName());
  if (sheet)
    d->view->setActiveSheet(sheet);

  d->chooseStartSheet = 0;
}

KSpreadHBorder* KSpreadCanvas::hBorderWidget() const
{
  return d->view->hBorderWidget();
}

KSpreadVBorder* KSpreadCanvas::vBorderWidget() const
{
  return d->view->vBorderWidget();
}

QScrollBar* KSpreadCanvas::horzScrollBar() const
{
  return d->view->horzScrollBar();
}

QScrollBar* KSpreadCanvas::vertScrollBar() const
{
  return d->view->vertScrollBar();
}

KSpreadSheet* KSpreadCanvas::findSheet( const QString& _name ) const
{
  return d->view->doc()->map()->findSheet( _name );
}

KSpreadSheet* KSpreadCanvas::activeSheet() const
{
  return d->view->activeSheet();
}

bool KSpreadCanvas::gotoLocation( const KSpreadRange & _range )
{
  if ( !_range.isValid() )
  {
    KMessageBox::error( this, i18n( "Invalid cell reference" ) );
    return false;
  }
  KSpreadSheet * sheet = activeSheet();
  if ( _range.isSheetKnown() )
    sheet = _range.sheet;
  if ( !sheet )
  {
    KMessageBox::error( this, i18n("Unknown table name %1" ).arg( _range.sheetName ) );
    return false;
  }

  gotoLocation( _range.range.topLeft(), sheet, false );
  gotoLocation( _range.range.bottomRight(), sheet, true );
  return true;
}


bool KSpreadCanvas::gotoLocation( const KSpreadPoint& _cell )
{
  if ( !_cell.isValid() )
  {
    KMessageBox::error( this, i18n("Invalid cell reference") );
    return false;
  }

  KSpreadSheet* sheet = activeSheet();
  if ( _cell.isSheetKnown() )
    sheet = _cell.sheet;
  if ( !sheet )
  {
    KMessageBox::error( this, i18n("Unknown table name %1").arg( _cell.sheetName ) );
    return false;
  }

  gotoLocation( _cell.pos, sheet );
  return true;
}

void KSpreadCanvas::gotoLocation( QPoint const & location, KSpreadSheet* sheet,
                                  bool extendSelection)
{
    //  kdDebug() << "GotoLocation: " << location.x() << ", " << location.x() << endl;

    if ( sheet && (sheet != activeSheet() ))
        d->view->setActiveSheet(sheet);
    else
        sheet = activeSheet();

    if (extendSelection)
    {
        extendCurrentSelection(location);
    }
    else
    {
        QPoint topLeft(location);
        KSpreadCell* cell = sheet->cellAt(location);
        if ( cell->isObscured() && cell->isObscuringForced() )
        {
            cell = cell->obscuringCells().first();
            topLeft = QPoint(cell->column(), cell->row());
        }

        if (d->chooseCell)
        {
            updateChooseRect(topLeft, topLeft);
            if( d->cellEditor )
            {
                if( d->chooseStartSheet != sheet )
                    d->cellEditor->hide();
                else
                    d->cellEditor->show();
            }
        }
        else
        {
            /* anchor and marker should be on the same cell here */
            selectionInfo()->setSelection(topLeft, topLeft, sheet);
        }
    }
    scrollToCell(location);

    // Perhaps the user is entering a value in the cell.
    // In this case we may not touch the EditWidget
    if ( !d->cellEditor && !d->chooseCell )
        d->view->updateEditWidgetOnPress();

    if ( selectionInfo()->singleCellSelection() )
    {
        int col = selectionInfo()->marker().x();
        int row = selectionInfo()->marker().y();
        KSpreadCell * cell = sheet->cellAt( col,row );
        if ( cell && cell->getValidity(0) && cell->getValidity()->displayValidationInformation)
        {
            QString title = cell->getValidity(0)->titleInfo;
            QString message = cell->getValidity(0)->messageInfo;
            if ( title.isEmpty() && message.isEmpty() )
                return;

            if ( !d->validationInfo )
                d->validationInfo = new QLabel(  this );
            kdDebug()<<" display info validation\n";
            double u = cell->dblWidth( col );
            double v = cell->dblHeight( row );
            double xpos = sheet->dblColumnPos( markerColumn() ) - xOffset();
            double ypos = sheet->dblRowPos( markerRow() ) - yOffset();
            // Special treatment for obscured cells.
            if ( cell->isObscured() && cell->isObscuringForced() )
            {
                cell = cell->obscuringCells().first();
                int moveX = cell->column();
                int moveY = cell->row();

                // Use the obscuring cells dimensions
                u = cell->dblWidth( moveX );
                v = cell->dblHeight( moveY );
                xpos = sheet->dblColumnPos( moveX );
                ypos = sheet->dblRowPos( moveY );
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
                resultText = title + "\n";
            }
            if ( !message.isEmpty() )
            {
                int i = 0;
                int pos = 0;
                QString t;
                do
                {
                    i = message.find( "\n", pos );
                    if ( i == -1 )
                        t = message.mid( pos, message.length() - pos );
                    else
                    {
                        t = message.mid( pos, i - pos );
                        pos = i + 1;
                    }
                    hei += painter.fontMetrics().height();
                    len = QMAX( len, painter.fontMetrics().width( t ) );
                }
                while ( i != -1 );
                resultText += message;
            }
            painter.end();
            d->validationInfo->setText( resultText );

            KoRect unzoomedMarker( xpos - xOffset()+u,
                                   ypos - yOffset()+v,
                                   len,
                                   hei );
            QRect marker( d->view->doc()->zoomRect( unzoomedMarker ) );

            d->validationInfo->setGeometry( marker );
            d->validationInfo->show();
        }
        else
        {
            delete d->validationInfo;
            d->validationInfo = 0L;
        }
    }
    else
    {
        delete d->validationInfo;
        d->validationInfo = 0L;
    }
    updatePosWidget();
}


void KSpreadCanvas::scrollToCell(QPoint location)
{
  KSpreadSheet* sheet = activeSheet();
  if (sheet == NULL)
    return;

  kdDebug(36001) << "------------------------------------------------" << endl;
  kdDebug(36001) << "scrollToCell(): at location [" << location.x() << ","
  		 << location.y() << "]" << endl;

  /* we don't need this cell ptr, but this call is necessary to update the
     scroll bar correctly.  I don't like having that as part of the cellAt function
     but I suppose that's ok for now.
  */
  KSpreadCell* cell = sheet->cellAt(location.x(), location.y(), true);
  Q_UNUSED(cell);

  double  unzoomedWidth  = d->view->doc()->unzoomItX( width() );
  double  unzoomedHeight = d->view->doc()->unzoomItY( height() );
  
  kdDebug(36001) << "Unzoomed view size: [" << unzoomedWidth << "," 
		 << unzoomedHeight << "]" << endl;

  // xpos is the position of the cell in the current window in unzoomed
  // document coordinates.
  double xpos;
  if ( sheet->layoutDirection()==KSpreadSheet::LeftToRight )
    xpos = sheet->dblColumnPos( location.x() ) - xOffset();
  else
    xpos = unzoomedWidth - sheet->dblColumnPos( location.x() ) + xOffset();
  double ypos = sheet->dblRowPos( location.y() ) - yOffset();

  kdDebug(36001) << "Position: [" << xpos << "," << ypos << "]" << endl;

  double minY = 40.0;
  double maxY = unzoomedHeight - 40.0;
  kdDebug(36001) << "KSpreadCanvas::gotoLocation : height=" << height() << endl;
  kdDebug(36001) << "KSpreadCanvas::gotoLocation : width=" << width() << endl;

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft ) {
    // Right to left sheet.

    double minX = unzoomedWidth - 100.0; // less than that, we scroll
    double maxX = 100.0; // more than that, we scroll

    // kdDebug() << "rtl2: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << ", Offset: " << xOffset() << endl;

    // Do we need to scroll left?
    if ( xpos > minX )
      horzScrollBar()->setValue( horzScrollBar()->maxValue() -
                                  d->view->doc()->zoomItX( xOffset() - xpos + minX ) );

    // Do we need to scroll right?
    else if ( xpos < maxX )
    {
      double horzScrollBarValue = xOffset() - xpos + maxX;
      double horzScrollBarValueMax = sheet->sizeMaxX() - unzoomedWidth;

      //We don't want to display any area > KS_colMax widths
      if ( horzScrollBarValue > horzScrollBarValueMax )
        horzScrollBarValue = horzScrollBarValueMax;

      horzScrollBar()->setValue( horzScrollBar()->maxValue() -
                                  d->view->doc()->zoomItX( horzScrollBarValue ) );
    }
  }
  else {
    // Left to right sheet.

    double minX = 100.0; // less than that, we scroll
    double maxX = unzoomedWidth - 100.0; // more than that, we scroll

    kdDebug() << "ltr: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << endl;

    // Do we need to scroll left?
    if ( xpos < minX )
      horzScrollBar()->setValue( d->view->doc()->zoomItX( xOffset() + xpos - minX ) );

    // Do we need to scroll right?
    else if ( xpos > maxX )
    {
      double horzScrollBarValue = xOffset() + xpos - maxX;
      double horzScrollBarValueMax = sheet->sizeMaxX() - unzoomedWidth;

      //We don't want to display any area > KS_colMax widths
      if ( horzScrollBarValue > horzScrollBarValueMax )
        horzScrollBarValue = horzScrollBarValueMax;

      horzScrollBar()->setValue( d->view->doc()->zoomItX( horzScrollBarValue ) );
    }
  }

  // do we need to scroll up
  if ( ypos < minY )
    vertScrollBar()->setValue( d->view->doc()->zoomItY( yOffset() + ypos - minY ) );

  // do we need to scroll down
  else if ( ypos > maxY )
  {
    double vertScrollBarValue = yOffset() + ypos - maxY;
    double vertScrollBarValueMax = sheet->sizeMaxY() - unzoomedHeight;

    //We don't want to display any area > KS_rowMax heights
    if ( vertScrollBarValue > vertScrollBarValueMax )
      vertScrollBarValue = vertScrollBarValueMax;

    vertScrollBar()->setValue( d->view->doc()->zoomItY( vertScrollBarValue ) );
  }
}

void KSpreadCanvas::slotScrollHorz( int _value )
{
  KSpreadSheet * sheet = activeSheet();

  if ( sheet == 0L )
    return;

  kdDebug(36001) << "slotScrollHorz: value = " << _value << endl;
  //kdDebug(36001) << kdBacktrace() << endl;

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    _value = horzScrollBar()->maxValue() - _value;

  double unzoomedValue = d->view->doc()->unzoomItX( _value );
  double dwidth = d->view->doc()->unzoomItX( width() );

  d->view->doc()->emitBeginOperation(false);

  if ( unzoomedValue < 0.0 ) {
    kdDebug (36001)
      << "KSpreadCanvas::slotScrollHorz: value out of range (unzoomedValue: " 
      << unzoomedValue << ")" << endl;
    unzoomedValue = 0.0;
  }

  double xpos = sheet->dblColumnPos( QMIN( KS_colMax, d->view->activeSheet()->maxColumn()+10 ) ) - d->xOffset;
  if ( unzoomedValue > ( xpos + d->xOffset ) )
    unzoomedValue = xpos + d->xOffset;

  sheet->enableScrollBarUpdates( false );

  // Relative movement
  int dx = d->view->doc()->zoomItX( d->xOffset - unzoomedValue );


  /* what cells will need painted now? */
  QRect area = visibleCells();
  double tmp;
  if (dx > 0)
  {
    area.setRight( area.left() );
    area.setLeft( sheet->leftColumn( unzoomedValue, tmp ) );
  }
  else
  {
    area.setLeft( area.right() );
    area.setRight( sheet->rightColumn( dwidth  + unzoomedValue ) );
  }

  sheet->setRegionPaintDirty(area);

  // New absolute position
  kdDebug(36001) << "slotScrollHorz(): XOffset before setting: " 
		 << d->xOffset << endl;
  d->xOffset = unzoomedValue;
  kdDebug(36001) << "slotScrollHorz(): XOffset after setting: " 
		 << d->xOffset << endl;

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    dx = -dx;

  scroll( dx, 0 );

  hBorderWidget()->scroll( dx, 0 );

  sheet->enableScrollBarUpdates( true );

  d->view->doc()->emitEndOperation( sheet->visibleRect( this ) );
}

void KSpreadCanvas::slotScrollVert( int _value )
{
  if ( activeSheet() == 0L )
    return;

  d->view->doc()->emitBeginOperation(false);
  double unzoomedValue = d->view->doc()->unzoomItY( _value );

  if ( unzoomedValue < 0 )
  {
    unzoomedValue = 0;
    kdDebug (36001) << "KSpreadCanvas::slotScrollVert: value out of range (unzoomedValue: " <<
                       unzoomedValue << ")" << endl;
  }

  double ypos = activeSheet()->dblRowPos( QMIN( KS_rowMax, d->view->activeSheet()->maxRow()+10 ) );
  if ( unzoomedValue > ypos )
      unzoomedValue = ypos;

  activeSheet()->enableScrollBarUpdates( false );

  // Relative movement
  int dy = d->view->doc()->zoomItY( d->yOffset - unzoomedValue );


  /* what cells will need painted now? */
  QRect area = visibleCells();
  double tmp;
  if (dy > 0)
  {
    area.setBottom(area.top());
    area.setTop(activeSheet()->topRow(unzoomedValue, tmp));
  }
  else
  {
    area.setTop(area.bottom());
    area.setBottom(activeSheet()->bottomRow(d->view->doc()->unzoomItY(height()) +
                                            unzoomedValue));
  }

  activeSheet()->setRegionPaintDirty( area );

  // New absolute position
  d->yOffset = unzoomedValue;
  scroll( 0, dy );
  vBorderWidget()->scroll( 0, dy );

  activeSheet()->enableScrollBarUpdates( true );

  d->view->doc()->emitEndOperation( activeSheet()->visibleRect( this ) );
}

void KSpreadCanvas::slotMaxColumn( int _max_column )
{
  int oldValue = horzScrollBar()->maxValue() - horzScrollBar()->value();
  double xpos = activeSheet()->dblColumnPos( QMIN( KS_colMax, _max_column + 10 ) ) - xOffset();
  double unzoomWidth = d->view->doc()->unzoomItX( width() );

  //Don't go beyond the maximum column range (KS_colMax)
  double sizeMaxX = activeSheet()->sizeMaxX();
  if ( xpos > sizeMaxX - xOffset() - unzoomWidth )
    xpos = sizeMaxX - xOffset() - unzoomWidth;

  horzScrollBar()->setRange( 0, d->view->doc()->zoomItX( xpos + xOffset() ) );

  if ( activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft )
    horzScrollBar()->setValue( horzScrollBar()->maxValue() - oldValue );
}

void KSpreadCanvas::slotMaxRow( int _max_row )
{
  double ypos = activeSheet()->dblRowPos( QMIN( KS_rowMax, _max_row + 10 ) ) - yOffset();
  double unzoomHeight = d->view->doc()->unzoomItY( height() );

  //Don't go beyond the maximum row range (KS_rowMax)
  double sizeMaxY = activeSheet()->sizeMaxY();
  if ( ypos > sizeMaxY - yOffset() - unzoomHeight )
    ypos = sizeMaxY - yOffset() - unzoomHeight;

  vertScrollBar()->setRange( 0, d->view->doc()->zoomItY( ypos + yOffset() ) );
}

bool KSpreadCanvas::getHighlightedRangesAt(const int col, const int row, std::vector<HighlightRange*>& ranges)
{
	if (!d->highlightedRanges)
		return false;
	
	bool result=false;
	
	std::vector<HighlightRange>::iterator iter;
		
	for (iter=d->highlightedRanges->begin();iter != d->highlightedRanges->end();iter++)
	{
		KSpreadRange rg;
		iter->getRange(rg);
		KSpreadPoint pt;
		pt.setRow(row);
		pt.setColumn(col);
		pt.sheet=activeSheet();
		
		if (rg.contains(pt))
		{ 
			ranges.push_back( &(*iter) );
			result=true;
		} 
	}
	
	return result;
}

void KSpreadCanvas::resizeHighlightedRange(HighlightRange* range, const QRect& newArea)
{
	if (!range) return;
	if (!d->cellEditor) return;
	if (!d->editWidget) return;
	
	QRect normArea=newArea.normalize();
	
	if ( (normArea.right() > KS_colMax) || (normArea.bottom() > KS_rowMax) )
		return;
	 
	
	KSpreadRange rg;
	range->getRange(rg);
	
	if (rg.range == normArea) //Don't update if no change to area
		return;
	
	QString oldRangeRef=rg.toString();
	
	int sheetNameEndPos=oldRangeRef.find("!");

	if (sheetNameEndPos == -1) return;
	
	oldRangeRef.remove(0,sheetNameEndPos+1); //Remove the sheet name (eg. Sheet1!A1 -> A1)
	
	int refSeparator=oldRangeRef.find(":");
	
	QString oldPointRef=oldRangeRef.mid(refSeparator+1);
	//if ( (rg.range.width()==1) && (rg.range.height()==1) )
	//	oldPointRef=oldRangeRef.mid(refSeparator+1);
	
	//Adjust range to fit new area selected by user
	rg.setRange(normArea);
	QString newRangeRef=rg.toString();
	
	newRangeRef.remove(0,sheetNameEndPos+1); //Remove sheet name, will be same name as for old reference
	
	if ( (rg.range.width()==1) && (rg.range.height()==1) )
		newRangeRef.remove(0,newRangeRef.find(":")+1);
	
	kdDebug() << "Old Range Ref - " << oldRangeRef << " -- New Range Ref - " << newRangeRef << endl;
	
	QString formulaText=d->cellEditor->text();
	
	int pos=formulaText.find(oldRangeRef);
	int pointPos=formulaText.find(oldPointRef);
	
	while ( (pos != -1) || (pointPos != -1))
	{ 
		bool validRef=true;
		
		if (pos != -1)
		{
			//Check that this is not part of a longer reference (eg. if we are looking for A1:A1, don't find A1:A12) 
			if (formulaText.at ( pos - 1).isLetterOrNumber())
				validRef=false;
			
			if (formulaText.at ( pos + oldRangeRef.length() ).isLetterOrNumber() )
				validRef=false;
		}
		else
		{
			if (formulaText.at (pointPos - 1).isLetterOrNumber())
				validRef=false;
			
			QChar ch=formulaText.at(pointPos + oldPointRef.length());
			
			if ( (ch.isLetterOrNumber()) || (ch == ':') )
				validRef=false;
		}
			
		if (validRef) 
		{ 
			if (pos != -1)
				formulaText=formulaText.replace(pos,oldRangeRef.length(),newRangeRef);
			else 
				formulaText=formulaText.replace(pointPos,oldPointRef.length(),newRangeRef);	
		} 
		
		kdDebug() << "Formula Text : " << formulaText << endl;
		
		if (pos != -1) 
		{
			pos = formulaText.find(oldRangeRef,pos+newRangeRef.length());
			pointPos = formulaText.find(oldPointRef,pos+newRangeRef.length());
		}
		else
		{
			pos = formulaText.find(oldRangeRef,pointPos+1+newRangeRef.length());
			pointPos = formulaText.find(oldPointRef,pointPos+1+newRangeRef.length());
		}
	}
	
	d->cellEditor->setText(formulaText);
	d->editWidget->setText(formulaText);
}

void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
  // Dont allow modifications if document is readonly. Selecting is no modification
  if ( (!d->view->koDocument()->isReadWrite()) && (d->mouseAction!=Mark))
    return;

  if ( d->dragging )
    return;
  
  if ( d->dragStart.x() != -1 )
  {
    QPoint p ( (int) _ev->pos().x() + (int) xOffset(),
               (int) _ev->pos().y() + (int) yOffset() );

    if ( ( d->dragStart - p ).manhattanLength() > 4 )
    {
      d->dragging = true;
      startTheDrag();
      d->dragStart.setX( -1 );
    }
    d->dragging = false;
    return;
  }

  // Working on this sheet ?
  KSpreadSheet *sheet = activeSheet();
  if ( !sheet )
    return;
 
  double dwidth = d->view->doc()->unzoomItX( width() );
  double ev_PosX;
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  double xpos;
  double ypos;
  int col = sheet->leftColumn( ev_PosX, xpos );
  int row  = sheet->topRow( ev_PosY, ypos );

  
  if ( !(_ev->state() & Qt::LeftButton) )
	  d->sizingHighlightRange=0;
  
  if (d->sizingHighlightRange)
  {
	  KSpreadRange rg;
	  d->sizingHighlightRange->getRange(rg);
	  QRect newRange;
	  newRange.setCoords(rg.range.left(),rg.range.top(),col,row);
	  
	  resizeHighlightedRange(d->sizingHighlightRange,newRange);
	  
	  setCursor(Qt::CrossCursor);
	  
	  return;
  }
  
 /* kdDebug() << "ev_PosX " << ev_PosX << endl;
  kdDebug() << "ev_PosY " << ev_PosY << endl;
  kdDebug() << "xOffset " << xOffset() << endl;
  kdDebug() << "yOffset " << yOffset() << endl;*/
  
  //Check for a highlight range size grip and show the user a visual cue if found.
  if (highlightRangeSizeGripAt(ev_PosX,ev_PosY))
  { 
	  setCursor(Qt::CrossCursor);
	  return;	  
  }
  
   // Special handling for choose mode.
  if ( d->chooseCell )
  {
	  chooseMouseMoveEvent( _ev );
	  return;
  }
  
  if ( col > KS_colMax || row > KS_rowMax )
  {
	  return;
  }

  QRect rct( selectionInfo()->selection() );

  QRect r1;
  QRect r2;

  double lx = sheet->dblColumnPos( rct.left() );
  double rx = sheet->dblColumnPos( rct.right() + 1 );
  double ty = sheet->dblRowPos( rct.top() );
  double by = sheet->dblRowPos( rct.bottom() + 1 );

  r1.setLeft( (int) (lx - 1) );
  r1.setTop( (int) (ty - 1) );
  r1.setRight( (int) (rx + 1) );
  r1.setBottom( (int) (by + 1) );

  r2.setLeft( (int) (lx + 1) );
  r2.setTop( (int) (ty + 1) );
  r2.setRight( (int) (rx - 1) );
  r2.setBottom( (int) (by - 1) );

  QRect selectionHandle = d->view->selectionInfo()->selectionHandleArea();

  // Test whether the mouse is over some anchor
  {
    KSpreadCell *cell = sheet->visibleCellAt( col, row );
    QString anchor;
    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
      anchor = cell->testAnchor( d->view->doc()->zoomItX( cell->dblWidth() - ev_PosX +
                               xpos ), d->view->doc()->zoomItY( ev_PosY - ypos ) );
    else
      anchor = cell->testAnchor( d->view->doc()->zoomItX( ev_PosX - xpos ),
                                       d->view->doc()->zoomItY( ev_PosY - ypos ) );
    if ( !anchor.isEmpty() && anchor != d->anchor )
      setCursor( KCursor::handCursor() );

    d->anchor = anchor;
  }

  if ( selectionHandle.contains( QPoint( d->view->doc()->zoomItX( ev_PosX ),
                                         d->view->doc()->zoomItY( ev_PosY ) ) ) )
  {
    //If the cursor is over the hanlde, than it might be already on the next cell.
    //Recalculate the cell!
    col  = sheet->leftColumn( ev_PosX - d->view->doc()->unzoomItX( 2 ), xpos );
    row  = sheet->topRow( ev_PosY - d->view->doc()->unzoomItY( 2 ), ypos );
     
    if ( !sheet->isProtected() )
    {
      if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
        setCursor( sizeBDiagCursor );
      else
        setCursor( sizeFDiagCursor );
    }
  }
  else if ( !d->anchor.isEmpty() )
  {
    if ( !sheet->isProtected() )
      setCursor( KCursor::handCursor() );
  }
  else if ( r1.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) )
            && !r2.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) ) )
    setCursor( KCursor::handCursor() );
  else
    setCursor( arrowCursor );

  // No marking, selecting etc. in progess? Then quit here.
  if ( d->mouseAction == NoAction )
    return;

  // Set the new extent of the selection
  gotoLocation( QPoint( col, row ), sheet, true );
}

void KSpreadCanvas::mouseReleaseEvent( QMouseEvent* _ev )
{
  if ( d->scrollTimer->isActive() )
    d->scrollTimer->stop();

  d->mousePressed = false;

  if ( d->chooseCell )
  {
    chooseMouseReleaseEvent( _ev );
    return;
  }
  

  KSpreadSheet *sheet = activeSheet();
  if ( !sheet )
    return;

  KSpreadSelection* selectionInfo = d->view->selectionInfo();
  QRect s( selection() );

  if ( selectionInfo->singleCellSelection() )
  {
    KSpreadCell* cell = sheet->cellAt( selectionInfo->marker() );
    cell->clicked( this );
  }

  // The user started the drag in the lower right corner of the marker ?
  if ( d->mouseAction == ResizeCell && !sheet->isProtected() )
  {
    QPoint selectionAnchor = selectionInfo->selectionAnchor();
    int x = selectionAnchor.x();
    int y = selectionAnchor.y();
    if ( x > s.left())
        x = s.left();
    if ( y > s.top() )
        y = s.top();
    KSpreadCell *cell = sheet->nonDefaultCell( x, y );
    if ( !d->view->doc()->undoLocked() )
    {
        KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( d->view->doc(),
                        sheet, x, y, cell->extraXCells(), cell->extraYCells() );
        d->view->doc()->addCommand( undo );
    }
    cell->forceExtraCells( x, y,
                           abs( s.right() - s.left() ),
                           abs( s.bottom() - s.top() ) );

    d->view->updateEditWidget();
    if ( sheet->getAutoCalc() ) sheet->recalc();
  }
  else if ( d->mouseAction == AutoFill && !sheet->isProtected() )
  {
    QRect dest = s;
    sheet->autofill( d->autoFillSource, dest );

    d->view->updateEditWidget();
  }
  // The user started the drag in the middle of a cell ?
  else if ( d->mouseAction == Mark )
  {
    d->view->updateEditWidget();
  }

  d->mouseAction = NoAction;
  d->dragging = false;
  d->dragStart.setX( -1 );
}

void KSpreadCanvas::processClickSelectionHandle( QMouseEvent *event )
{
  // Auto fill ? That is done using the left mouse button.
  if ( event->button() == LeftButton )
  {
    d->mouseAction = AutoFill;
    d->autoFillSource = selection();
  }
  // Resize a cell (done with the right mouse button) ?
  // But for that to work there must not be a selection.
  else if ( event->button() == MidButton && selectionInfo()->singleCellSelection())
  {
    d->mouseAction = ResizeCell;
  }

  return;
}


void KSpreadCanvas::extendCurrentSelection( QPoint cell )
{
  KSpreadSheet* sheet = activeSheet();
  QPoint chooseAnchor = selectionInfo()->getChooseAnchor();
//  KSpreadCell* destinationCell = sheet->cellAt(cell);

  if ( d->chooseCell )
  {
    if ( chooseAnchor.x() == 0 )
    {
      updateChooseRect( cell, cell );
    }
    else
    {
      updateChooseRect( cell, chooseAnchor );
    }
  }
  else
  {

    /* the selection simply becomes a box with the anchor and given cell as
       opposite corners
    */
    selectionInfo()->setSelection( cell, selectionInfo()->selectionAnchor(),
                                   sheet );
  }
}

void KSpreadCanvas::processLeftClickAnchor()
{
    bool isRefLink = localReferenceAnchor( d->anchor );
    bool isLocalLink = (d->anchor.find("file:") == 0);
    if ( !isRefLink )
    {
        QString question = i18n("Do you want to open this link to '%1'?\n").arg(d->anchor);
        if ( isLocalLink )
        {
            question += i18n("Note that opening a link to a local file may "
                             "compromise your system's security.");
        }

        // this will also start local programs, so adding a "don't warn again"
        // checkbox will probably be too dangerous
        int choice = KMessageBox::warningYesNo(this, question, i18n("Open Link?"));
        if ( choice == KMessageBox::Yes )
        {
            (void) new KRun( d->anchor );
        }
    }
    else
    {
        gotoLocation( KSpreadPoint( d->anchor, d->view->doc()->map() ) );
    }
}

HighlightRange* KSpreadCanvas::highlightRangeSizeGripAt(double x, double y) 
{
	if (!d->highlightedRanges)
		return 0;
	
	double xpos;
	double ypos;
	int col  = activeSheet()->leftColumn( x, xpos );
	int row  = activeSheet()->topRow( y, ypos );
	
	std::vector<HighlightRange*> highlightedRangesUnderMouse;

	if (getHighlightedRangesAt(col,row,highlightedRangesUnderMouse))
	{
		std::vector<HighlightRange*>::iterator iter;
	  
		for (iter=highlightedRangesUnderMouse.begin();iter != highlightedRangesUnderMouse.end();iter++)
		{
			HighlightRange* highlight=*iter;
		  
	  	//Is the mouse over the size grip at the bottom-right hand corner of the range?
			KSpreadRange rg;
			highlight->getRange(rg);
	  
			KoRect visibleRect;
			sheetAreaToRect(QRect(rg.startCol(),rg.startRow(),
					       rg.endCol()-rg.startCol()+1,rg.endRow()-rg.startRow()+1),visibleRect);

			double distFromSizeGripCorner=( sqrt( (visibleRect.right()-x) +
						(visibleRect.bottom()-y) ) );
	  
			if (distFromSizeGripCorner < 5)
			{
				return highlight; 
			}
		}
	} 
	
	return 0;
}
void KSpreadCanvas::mousePressEvent( QMouseEvent * _ev )
{
  if ( _ev->button() == LeftButton )
    d->mousePressed = true;

  // If in choose mode, we handle the mouse differently.
  if ( d->chooseCell )
  {
    chooseMousePressEvent( _ev );
    return;
  }
  
  //Get info about where the event occurred - this is duplicated in ::mouseMoveEvent, needs to be separated into one function
  KSpreadSheet *sheet = activeSheet();

  if ( !sheet )
	  return;

  double dwidth = 0.0;
  double ev_PosX;
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
	  dwidth = d->view->doc()->unzoomItX( width() );
	  ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  }
  else
	  ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();
  
  // In which cell did the user click ?
  double xpos;
  double ypos;
  int col  = sheet->leftColumn( ev_PosX, xpos );
  int row  = sheet->topRow( ev_PosY, ypos );
  
  //Check region to see if we are over a highlight range size grip
 
  d->sizingHighlightRange=highlightRangeSizeGripAt(ev_PosX,ev_PosY);

  if (d->sizingHighlightRange)
		  return;
  

  // We were editing a cell -> save value and get out of editing mode
  if ( d->cellEditor )
  {
    deleteEditor( true ); // save changes
  }

  d->scrollTimer->start( 50 );

  // Remember current values.
  QRect s( selection() );

  // Did we click in the lower right corner of the marker/marked-area ?
  if ( selectionInfo()->selectionHandleArea().contains( QPoint( d->view->doc()->zoomItX( ev_PosX ),
                                                                d->view->doc()->zoomItY( ev_PosY ) ) ) )
  {
    processClickSelectionHandle( _ev );
    return;
  }

  

  {
    // start drag ?
    QRect rct( selectionInfo()->selection() );

    QRect r1;
    QRect r2;
    {
      double lx = sheet->dblColumnPos( rct.left() );
      double rx = sheet->dblColumnPos( rct.right() + 1 );
      double ty = sheet->dblRowPos( rct.top() );
      double by = sheet->dblRowPos( rct.bottom() + 1 );

      r1.setLeft( (int) (lx - 1) );
      r1.setTop( (int) (ty - 1) );
      r1.setRight( (int) (rx + 1) );
      r1.setBottom( (int) (by + 1) );

      r2.setLeft( (int) (lx + 1) );
      r2.setTop( (int) (ty + 1) );
      r2.setRight( (int) (rx - 1) );
      r2.setBottom( (int) (by - 1) );
    }

    d->dragStart.setX( -1 );

    if ( r1.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) )
         && !r2.contains( QPoint( (int) ev_PosX, (int) ev_PosY ) ) )
    {
      d->dragStart.setX( (int) ev_PosX );
      d->dragStart.setY( (int) ev_PosY );

      return;
    }
  }

  //  kdDebug() << "Clicked in cell " << col << ", " << row << endl;

  //you cannot move marker when col > KS_colMax or row > KS_rowMax
  if ( col > KS_colMax || row > KS_rowMax)
  {
    kdDebug(36001) << "KSpreadCanvas::mousePressEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  // Extending an existing selection with the shift button ?
  if ( d->view->koDocument()->isReadWrite() && s.right() != KS_colMax &&
       s.bottom() != KS_rowMax && _ev->state() & ShiftButton )
  {
    gotoLocation( QPoint( col, row ), activeSheet(), true );
    return;
  }

  KSpreadCell *cell = sheet->cellAt( col, row );

  // Go to the upper left corner of the obscuring object if cells are merged
  if (cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    col = cell->column();
    row = cell->row();
  }

  // Start a marking action ?
  if ( !d->anchor.isEmpty() && _ev->button() == LeftButton )
  {
    processLeftClickAnchor();
    updatePosWidget();
  }
  else if ( _ev->button() == LeftButton )
  {
    d->mouseAction = Mark;
    gotoLocation( QPoint( col, row ), activeSheet(), false );
  }
  else if ( _ev->button() == RightButton &&
            !s.contains( QPoint( col, row ) ) )
  {
    // No selection or the mouse press was outside of an existing selection ?
    gotoLocation( QPoint( col, row ), activeSheet(), false );
  }

  // Paste operation with the middle button ?
  if ( _ev->button() == MidButton )
  {
    if ( d->view->koDocument()->isReadWrite() && !sheet->isProtected() )
    {
      selectionInfo()->setMarker( QPoint( col, row ), sheet );
      sheet->paste( QRect(marker(), marker()) );
      sheet->setRegionPaintDirty(QRect(marker(), marker()));
    }
    updatePosWidget();
  }

  // Update the edit box
  d->view->updateEditWidgetOnPress();

  // Context menu ?
  if ( _ev->button() == RightButton )
  {
    updatePosWidget();
    // TODO: Handle anchor
    QPoint p = mapToGlobal( _ev->pos() );
    d->view->openPopupMenu( p );
  }
}

void KSpreadCanvas::startTheDrag()
{
  KSpreadSheet * sheet = activeSheet();
  if ( !sheet )
    return;

  // right area for start dragging
  KSpreadTextDrag * d = new KSpreadTextDrag( this );
  setCursor( KCursor::handCursor() );

  QRect rct( selectionInfo()->selection() );
  QDomDocument doc = sheet->saveCellRect( rct );

  // Save to buffer
  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  QTextStream str( &buffer );
  str.setEncoding( QTextStream::UnicodeUTF8 );
  str << doc;
  buffer.close();

  d->setPlain( sheet->copyAsText( selectionInfo() ) );
  d->setKSpread( buffer.buffer() );

  d->dragCopy();
  setCursor( KCursor::arrowCursor() );
}

void KSpreadCanvas::chooseMouseMoveEvent( QMouseEvent * _ev )
{
  if ( !d->mousePressed )
    return;

  KSpreadSheet * sheet = activeSheet();
  if ( !sheet )
    return;

  double tmp;
  double ev_PosX;
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    double dwidth = d->view->doc()->unzoomItX( width() );
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() );
  }
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() );

  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() );
  int col = sheet->leftColumn( (ev_PosX + xOffset()), tmp );
  int row = sheet->topRow( (ev_PosY + yOffset()), tmp );

  if ( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  QPoint chooseMarker = selectionInfo()->getChooseMarker();

  // Nothing changed ?
  if ( row == chooseMarker.y() && col == chooseMarker.x() )
  {
    return;
  }

  gotoLocation( QPoint( col, row ), sheet, ( d->mouseAction != NoAction ) );
}

void KSpreadCanvas::chooseMouseReleaseEvent( QMouseEvent* )
{
    // gets done in mouseReleaseEvent
    //  d->mousePressed = false;
  d->mouseAction = NoAction;
}

void KSpreadCanvas::chooseMousePressEvent( QMouseEvent * _ev )
{
  KSpreadSheet *sheet = activeSheet();
  if ( !sheet )
    return;


  double ev_PosX;
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    double dwidth = d->view->doc()->unzoomItX( width() );
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() );
  }
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() );

  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() );
  double ypos, xpos;
  int col = sheet->leftColumn( (ev_PosX + xOffset()), xpos );
  int row = sheet->topRow( (ev_PosY + yOffset()), ypos );

  if ( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  bool extend = ( ( ( !util_isColumnSelected(selection() ) ) &&
                    ( !util_isRowSelected(selection() ) ) ) &&
                  ( _ev->state() & ShiftButton ) );

  gotoLocation( QPoint( col, row ), activeSheet(), extend );

  if ( _ev->button() == LeftButton )
  {
    d->mouseAction = Mark;
  }
  return;
}

void KSpreadCanvas::mouseDoubleClickEvent( QMouseEvent*  )
{
  if ( d->view->koDocument()->isReadWrite() && activeSheet() )
    createEditor();
}

void KSpreadCanvas::wheelEvent( QWheelEvent* _ev )
{
  if ( _ev->orientation() == Qt::Vertical )
  {
    if ( vertScrollBar() )
      QApplication::sendEvent( vertScrollBar(), _ev );
  }
  else if ( horzScrollBar() )
  {
    QApplication::sendEvent( horzScrollBar(), _ev );
  }
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
  if ( d->view->doc()->isLoading() )
    return;

  KSpreadSheet* sheet = activeSheet();
  if ( !sheet )
    return;

  // ElapsedTime et( "KSpreadCanvas::paintEvent" );

  double dwidth = d->view->doc()->unzoomItX( width() );
  KoRect rect = d->view->doc()->unzoomRect( _ev->rect() & QWidget::rect() );
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    rect.moveBy( -xOffset(), yOffset() );
  else
    rect.moveBy( xOffset(), yOffset() );

  KoPoint tl = rect.topLeft();
  KoPoint br = rect.bottomRight();

  double tmp;
  int left_col;
  int right_col;
  //Philipp: I don't know why we need the +1, but otherwise we don't get it correctly
  //Testcase: Move a dialog slowly up left. Sometimes the top/left most points are not painted
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    right_col = sheet->leftColumn( dwidth - tl.x(), tmp );
    left_col  = sheet->rightColumn( dwidth - br.x() + 1.0 );
  }
  else
  {
    left_col  = sheet->leftColumn( tl.x(), tmp );
    right_col = sheet->rightColumn( br.x() + 1.0 );
  }
  int top_row = sheet->topRow( tl.y(), tmp );
  int bottom_row = sheet->bottomRow( br.y() + 1.0 );

  QRect vr( QPoint(left_col, top_row),
            QPoint(right_col, bottom_row) );
  d->view->doc()->emitBeginOperation( false );
  sheet->setRegionPaintDirty( vr );
  d->view->doc()->emitEndOperation( vr );
}

void KSpreadCanvas::focusInEvent( QFocusEvent* )
{
  if ( !d->cellEditor )
    return;

  //kdDebug(36001) << "d->chooseCell : " << ( d->chooseCell ? "true" : "false" ) << endl;
  // If we are in editing mode, we redirect the
  // focus to the CellEditor or EditWidget
  // And we know which, using lastEditorWithFocus.
  // This screws up <Tab> though (David)
  if ( lastEditorWithFocus() == EditWidget )
  {
    d->view->editWidget()->setFocus();
    //kdDebug(36001) << "Focus to EditWidget" << endl;
    return;
  }

  //kdDebug(36001) << "Redirecting focus to editor" << endl;
  d->cellEditor->setFocus();
}

void KSpreadCanvas::focusOutEvent( QFocusEvent* )
{
    if ( d->scrollTimer->isActive() )
        d->scrollTimer->stop();
    d->mousePressed = false;
}

void KSpreadCanvas::dragMoveEvent( QDragMoveEvent * _ev )
{
  KSpreadSheet * sheet = activeSheet();
  if ( !sheet )
  {
    _ev->ignore();
    return;
  }

  _ev->accept( KSpreadTextDrag::canDecode( _ev ) );

  double dwidth = d->view->doc()->unzoomItX( width() );
  double xpos = sheet->dblColumnPos( selectionInfo()->selection().left() );
  double ypos = sheet->dblRowPos( selectionInfo()->selection().top() );
  double width  = sheet->columnFormat( selectionInfo()->selection().left() )->dblWidth( this );
  double height = sheet->rowFormat( selectionInfo()->selection().top() )->dblHeight( this );

  QRect r1 ((int) xpos - 1, (int) ypos - 1, (int) width + 3, (int) height + 3);

  double ev_PosX;
  if (sheet->layoutDirection()==KSpreadSheet::RightToLeft)
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();

  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  if ( r1.contains( QPoint ((int) ev_PosX, (int) ev_PosY) ) )
    _ev->ignore( r1 );
}

void KSpreadCanvas::dragLeaveEvent( QDragLeaveEvent * )
{
  if ( d->scrollTimer->isActive() )
    d->scrollTimer->stop();
}

void KSpreadCanvas::dropEvent( QDropEvent * _ev )
{
  d->dragging = false;
  KSpreadSheet * sheet = activeSheet();
  if ( !sheet || sheet->isProtected() )
  {
    _ev->ignore();
    return;
  }

  double dwidth = d->view->doc()->unzoomItX( width() );
  double xpos = sheet->dblColumnPos( selectionInfo()->selection().left() );
  double ypos = sheet->dblRowPos( selectionInfo()->selection().top() );
  double width  = sheet->columnFormat( selectionInfo()->selection().left() )->dblWidth( this );
  double height = sheet->rowFormat( selectionInfo()->selection().top() )->dblHeight( this );

  QRect r1 ((int) xpos - 1, (int) ypos - 1, (int) width + 3, (int) height + 3);

  double ev_PosX;
  if (sheet->layoutDirection()==KSpreadSheet::RightToLeft)
    ev_PosX = dwidth - d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  else
    ev_PosX = d->view->doc()->unzoomItX( _ev->pos().x() ) + xOffset();

  double ev_PosY = d->view->doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  if ( r1.contains( QPoint ((int) ev_PosX, (int) ev_PosY) ) )
  {
    _ev->ignore( );
    return;
  }
  else
    _ev->accept( );

  double tmp;
  int col = sheet->leftColumn( ev_PosX, tmp );
  int row = sheet->topRow( ev_PosY, tmp );

  if ( !KSpreadTextDrag::canDecode( _ev ) )
  {
    _ev->ignore();
    return;
  }

  QByteArray b;

  bool makeUndo = true;

  if ( _ev->provides( KSpreadTextDrag::selectionMimeType() ) )
  {
    if ( KSpreadTextDrag::target() == _ev->source() )
    {
      if ( !d->view->doc()->undoLocked() )
      {
        KSpreadUndoDragDrop * undo
          = new KSpreadUndoDragDrop( d->view->doc(), sheet, selectionInfo()->selection(),
                                     QRect( col, row, selectionInfo()->selection().width(),
                                            selectionInfo()->selection().height() ) );
        d->view->doc()->addCommand( undo );
        makeUndo = false;
      }
      sheet->deleteSelection( selectionInfo(), false );
    }


    b = _ev->encodedData( KSpreadTextDrag::selectionMimeType() );
    sheet->paste( b, QRect( col, row, 1, 1 ), makeUndo );

    if ( _ev->source() == this )
      _ev->acceptAction();
    _ev->accept();
  }
  else
  {
    QString text;
    if ( !QTextDrag::decode( _ev, text ) )
    {
      _ev->ignore();
      return;
    }
    //    if ( KSpreadTextDrag::target() == _ev->source() )
    //      sheet->deleteSelection( selectionInfo() );

    sheet->pasteTextPlain( text, QRect( col, row, 1, 1 ) );
    _ev->accept();
    if ( _ev->source() == this )
      _ev->acceptAction();

    return;
  }
}

void KSpreadCanvas::resizeEvent( QResizeEvent* _ev )
{
    double ev_Width = d->view->doc()->unzoomItX( _ev->size().width() );
    double ev_Height = d->view->doc()->unzoomItY( _ev->size().height() );

    // workaround to allow horizontal resizing and zoom changing when sheet
    // direction and interface direction don't match (e.g. an RTL sheet on an
    // LTR interface)
    if ( activeSheet() && activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft && !QApplication::reverseLayout() )
    {
        int dx = _ev->size().width() - _ev->oldSize().width();
        scroll(dx, 0);
    }
    else if ( activeSheet() && activeSheet()->layoutDirection()==KSpreadSheet::LeftToRight && QApplication::reverseLayout() )
    {
        int dx = _ev->size().width() - _ev->oldSize().width();
        scroll(-dx, 0);
    }

    // If we rise horizontally, then check if we are still within the valid area (KS_colMax)
    if ( _ev->size().width() > _ev->oldSize().width() )
    {
        int oldValue = horzScrollBar()->maxValue() - horzScrollBar()->value();

        if ( ( xOffset() + ev_Width ) >
               d->view->doc()->zoomItX( activeSheet()->sizeMaxX() ) )
        {
            horzScrollBar()->setRange( 0, d->view->doc()->zoomItX( activeSheet()->sizeMaxX() - ev_Width ) );
            if ( activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft )
                horzScrollBar()->setValue( horzScrollBar()->maxValue() - oldValue );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().width() < _ev->oldSize().width() )
    {
        int oldValue = horzScrollBar()->maxValue() - horzScrollBar()->value();

        if ( horzScrollBar()->maxValue() ==
             int( d->view->doc()->zoomItX( activeSheet()->sizeMaxX() ) - ev_Width ) )
        {
            horzScrollBar()->setRange( 0, d->view->doc()->zoomItX( activeSheet()->sizeMaxX() - ev_Width ) );
            if ( activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft )
                horzScrollBar()->setValue( horzScrollBar()->maxValue() - oldValue );
        }
    }

    // If we rise vertically, then check if we are still within the valid area (KS_rowMax)
    if ( _ev->size().height() > _ev->oldSize().height() )
    {
        if ( ( yOffset() + ev_Height ) >
             d->view->doc()->zoomItY( activeSheet()->sizeMaxY() ) )
        {
            vertScrollBar()->setRange( 0, d->view->doc()->zoomItY( activeSheet()->sizeMaxY() - ev_Height ) );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().height() < _ev->oldSize().height() )
    {
        if ( vertScrollBar()->maxValue() ==
             int( d->view->doc()->zoomItY( activeSheet()->sizeMaxY() ) - ev_Height ) )
        {
            vertScrollBar()->setRange( 0, d->view->doc()->zoomItY( activeSheet()->sizeMaxY() - ev_Height ) );
        }
    }
}

QPoint KSpreadCanvas::cursorPos ()
{
  QPoint cursor;
  if (d->chooseCell)
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  return cursor;
}

QRect KSpreadCanvas::moveDirection( KSpread::MoveTo direction, bool extendSelection )
{
  QPoint destination;
  QPoint cursor = cursorPos ();

  QPoint cellCorner = cursor;
  KSpreadCell* cell = activeSheet()->cellAt(cursor.x(), cursor.y());

  /* cell is either the same as the marker, or the cell that is forced obscuring
     the marker cell
  */
  if (cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    cellCorner = QPoint(cell->column(), cell->row());
  }

  /* how many cells must we move to get to the next cell? */
  int offset = 0;
  RowFormat *rl = NULL;
  ColumnFormat *cl = NULL;
  switch (direction)
    /* for each case, figure out how far away the next cell is and then keep
       going one row/col at a time after that until a visible row/col is found

       NEVER use cell->column() or cell->row() -- it might be a default cell
    */
  {
    case KSpread::Bottom:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeSheet()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        offset++;
        rl = activeSheet()->rowFormat( cursor.y() + offset );
      }

      destination = QPoint(cursor.x(), QMIN(cursor.y() + offset, KS_rowMax));
      break;
    case KSpread::Top:
      offset = (cellCorner.y() - cursor.y()) - 1;
      rl = activeSheet()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) >= 1) && rl->isHide())
      {
        offset--;
        rl = activeSheet()->rowFormat( cursor.y() + offset );
      }
      destination = QPoint(cursor.x(), QMAX(cursor.y() + offset, 1));
      break;
    case KSpread::Left:
      offset = (cellCorner.x() - cursor.x()) - 1;
      cl = activeSheet()->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) >= 1) && cl->isHide())
      {
        offset--;
        cl = activeSheet()->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(QMAX(cursor.x() + offset, 1), cursor.y());
      break;
    case KSpread::Right:
      offset = cell->mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
      cl = activeSheet()->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) <= KS_colMax) && cl->isHide())
      {
        offset++;
        cl = activeSheet()->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(QMIN(cursor.x() + offset, KS_colMax), cursor.y());
      break;
    case KSpread::BottomFirst:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeSheet()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        ++offset;
        rl = activeSheet()->rowFormat( cursor.y() + offset );
      }

      destination = QPoint( 1, QMIN( cursor.y() + offset, KS_rowMax ) );
      break;
  }

  gotoLocation(destination, activeSheet(), extendSelection);
  d->view->updateEditWidget();

  return QRect( cursor, destination );
}

void KSpreadCanvas::processEnterKey(QKeyEvent* event)
{
  // array is true, if ctrl+alt are pressed
  bool array = (event->state() & Qt::AltButton) &&
      (event->state() & Qt::ControlButton);

  /* save changes to the current editor */
  if (!d->chooseCell)
  {
    deleteEditor(true, array);
  }

  /* use the configuration setting to see which direction we're supposed to move
     when enter is pressed.
  */
  KSpread::MoveTo direction = d->view->doc()->getMoveToValue();

  //if shift Button clicked inverse move direction
  if (event->state() & Qt::ShiftButton)
  {
    switch( direction )
    {
     case KSpread::Bottom:
      direction = KSpread::Top;
      break;
     case KSpread::Top:
      direction = KSpread::Bottom;
      break;
     case KSpread::Left:
      direction = KSpread::Right;
      break;
     case KSpread::Right:
      direction = KSpread::Left;
      break;
     case KSpread::BottomFirst:
      direction = KSpread::BottomFirst;
      break;
    }
  }

  /* never extend a selection with the enter key -- the shift key reverses
     direction, not extends the selection
  */
  QRect r( moveDirection( direction, false ) );
  d->view->doc()->emitEndOperation( r );
}

void KSpreadCanvas::processArrowKey( QKeyEvent *event)
{
  /* NOTE:  hitting the tab key also calls this function.  Don't forget
     to account for it
  */

  /* save changes to the current editor */
  if (!d->chooseCell)
  {
    deleteEditor( true );
  }

  KSpread::MoveTo direction = KSpread::Bottom;
  bool makingSelection = event->state() & ShiftButton;

  switch (event->key())
  {
  case Key_Down:
    direction = KSpread::Bottom;
    break;
  case Key_Up:
    direction = KSpread::Top;
    break;
  case Key_Left:
    if (activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft)
      direction = KSpread::Right;
    else
      direction = KSpread::Left;
    break;
  case Key_Right:
    if (activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft)
      direction = KSpread::Left;
    else
      direction = KSpread::Right;
    break;
  case Key_Tab:
      direction = KSpread::Right;
      break;
  case Key_Backtab:
      //Shift+Tab moves to the left
      direction = KSpread::Left;
      makingSelection = false;
      break;
  default:
    Q_ASSERT(false);
    break;
  }

  QRect r( moveDirection( direction, makingSelection ) );
  d->view->doc()->emitEndOperation( r );
}

void KSpreadCanvas::processEscapeKey(QKeyEvent * event)
{
  if ( d->cellEditor )
    deleteEditor( false );

  event->accept(); // ?
  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
}

bool KSpreadCanvas::processHomeKey(QKeyEvent* event)
{
  bool makingSelection = event->state() & ShiftButton;
  KSpreadSheet* sheet = activeSheet();

  if ( d->cellEditor )
  // We are in edit mode -> go beginning of line
  {
    // (David) Do this for text editor only, not formula editor...
    // Don't know how to avoid this hack (member var for editor type ?)
    if ( d->cellEditor->inherits("KSpreadTextEditor") )
      QApplication::sendEvent( d->editWidget, event );
    // What to do for a formula editor ?

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

    if (event->state() & ControlButton)
    {
      /* ctrl + Home will always just send us to location (1,1) */
      destination = QPoint( 1, 1 );
    }
    else
    {
      QPoint marker = d->chooseCell ?
        selectionInfo()->getChooseMarker() : selectionInfo()->marker();

      KSpreadCell * cell = sheet->getFirstCellRow(marker.y());
      while (cell != NULL && cell->column() < marker.x() && cell->isEmpty())
      {
        cell = sheet->getNextCellRight(cell->column(), cell->row());
      }

      int col = ( cell ? cell->column() : 1 );
      if ( col == marker.x())
        col = 1;
      destination = QPoint(col, marker.y());
    }

    if ( selectionInfo()->marker() == destination )
    {
      d->view->doc()->emitEndOperation( QRect( destination, destination ) );
      return false;
    }

    gotoLocation( destination, activeSheet(), makingSelection );
  }
  return true;
}

bool KSpreadCanvas::processEndKey( QKeyEvent *event )
{
  bool makingSelection = event->state() & ShiftButton;
  KSpreadSheet* sheet = activeSheet();
  KSpreadCell* cell = NULL;
  QPoint marker = d->chooseCell ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();


  // move to the last used cell in the row
  // We are in edit mode -> go beginning of line
  if ( d->cellEditor )
  {
    // (David) Do this for text editor only, not formula editor...
    // Don't know how to avoid this hack (member var for editor type ?)
    if ( d->cellEditor->inherits("KSpreadTextEditor") )
      QApplication::sendEvent( d->editWidget, event );
    // TODO: What to do for a formula editor ?
    d->view->doc()->emitEndOperation( QRect( marker, marker ) );
    return false;
  }
  else
  {
    int col = 1;

    cell = sheet->getLastCellRow(marker.y());
    while (cell != NULL && cell->column() > markerColumn() && cell->isEmpty())
    {
      cell = sheet->getNextCellLeft(cell->column(), cell->row());
    }

    col = (cell == NULL) ? KS_colMax : cell->column();

    QPoint destination( col, marker.y() );
    if ( destination == marker )
    {
      d->view->doc()->emitEndOperation( QRect( destination, destination ) );
      return false;
    }

    gotoLocation( destination, activeSheet(), makingSelection );
  }
  return true;
}

bool KSpreadCanvas::processPriorKey(QKeyEvent *event)
{
  bool makingSelection = event->state() & ShiftButton;
  if (!d->chooseCell)
  {
    deleteEditor( true );
  }

  QPoint marker = d->chooseCell ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();

  QPoint destination(marker.x(), QMAX(1, marker.y() - 10));
  if ( destination == marker )
  {
    d->view->doc()->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  gotoLocation(destination, activeSheet(), makingSelection);

  return true;
}

bool KSpreadCanvas::processNextKey(QKeyEvent *event)
{
  bool makingSelection = event->state() & ShiftButton;

  if (!d->chooseCell)
  {
    deleteEditor( true /*save changes*/ );
  }

  QPoint marker = d->chooseCell ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();
  QPoint destination(marker.x(), QMAX(1, marker.y() + 10));

  if ( marker == destination )
  {
    d->view->doc()->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  gotoLocation(destination, activeSheet(), makingSelection);

  return true;
}

void KSpreadCanvas::processDeleteKey(QKeyEvent* /* event */)
{
  activeSheet()->clearTextSelection( selectionInfo() );
  d->view->editWidget()->setText( "" );

  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void KSpreadCanvas::processF2Key(QKeyEvent* /* event */)
{
  d->view->editWidget()->setFocus();
  if ( d->cellEditor )
    d->view->editWidget()->setCursorPosition( d->cellEditor->cursorPosition() - 1 );
  d->view->editWidget()->cursorForward( false );


  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void KSpreadCanvas::processF4Key(QKeyEvent* event)
{
  /* passes F4 to the editor (if any), which will process it
   */
  if ( d->cellEditor )
  {
    d->cellEditor->handleKeyPressEvent( event );
//    d->view->editWidget()->setFocus();
    d->view->editWidget()->setCursorPosition( d->cellEditor->cursorPosition() );
  }
  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void KSpreadCanvas::processOtherKey(QKeyEvent *event)
{
  // No null character ...
  if ( event->text().isEmpty() || !d->view->koDocument()->isReadWrite()
       || !activeSheet() || activeSheet()->isProtected() )
  {
    event->accept();
  }
  else
  {
    if ( !d->cellEditor && !d->chooseCell )
    {
      // Switch to editing mode
      createEditor( CellEditor );
      d->cellEditor->handleKeyPressEvent( event );
    }
    else if ( d->cellEditor )
      d->cellEditor->handleKeyPressEvent( event );
  }

  QPoint cursor = cursorPos();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );

  return;
}

bool KSpreadCanvas::processControlArrowKey( QKeyEvent *event )
{
  bool makingSelection = event->state() & ShiftButton;

  KSpreadSheet* sheet = activeSheet();
  KSpreadCell* cell = NULL;
  KSpreadCell* lastCell;
  QPoint destination;
  bool searchThroughEmpty = true;
  int row;
  int col;

  QPoint marker = d->chooseCell ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();

  /* here, we want to move to the first or last cell in the given direction that is
     actually being used.  Ignore empty cells and cells on hidden rows/columns */
  switch ( event->key() )
  {
    //Ctrl+Key_Up
   case Key_Up:

    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != 1))
    {
      lastCell = cell;
      row = marker.y()-1;
      cell = sheet->cellAt(cell->column(), row);
      while ((cell != NULL) && (row > 0) && (!cell->isEmpty()) )
      {
        if (!(sheet->rowFormat(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        row--;
        if ( row > 0 )
          cell = sheet->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellUp(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->rowFormat(cell->row())->isHide())))
      {
        cell = sheet->getNextCellUp(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = 1;
    else
      row = cell->row();

    while ( sheet->rowFormat(row)->isHide() )
    {
      row++;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

    //Ctrl+Key_Down
   case Key_Down:

    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != KS_rowMax))
    {
      lastCell = cell;
      row = marker.y()+1;
      cell = sheet->cellAt(cell->column(), row);
      while ((cell != NULL) && (row < KS_rowMax) && (!cell->isEmpty()) )
      {
        if (!(sheet->rowFormat(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        row++;
        cell = sheet->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellDown(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->rowFormat(cell->row())->isHide())))
      {
        cell = sheet->getNextCellDown(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = marker.y();
    else
      row = cell->row();

    while ( sheet->rowFormat(row)->isHide() )
    {
      row--;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

  //Ctrl+Key_Left
  case Key_Left:

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col++;
        cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellRight(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = marker.x();
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
  else
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col > 0) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col--;
        if ( col > 0 )
            cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellLeft(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
    break;

  //Ctrl+Key_Right
  case Key_Right:

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col > 0) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col--;
        if ( col > 0 )
            cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellLeft(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
  else
  {
    cell = sheet->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = sheet->cellAt(col, cell->row());
      while ((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if (!(sheet->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = false;
        }
        col++;
        cell = sheet->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = sheet->getNextCellRight(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (sheet->columnFormat(cell->column())->isHide())))
      {
        cell = sheet->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = marker.x();
    else
      col = cell->column();

    while ( sheet->columnFormat(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
    break;

  }

  if ( marker == destination )
  {
    d->view->doc()->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  gotoLocation( destination, sheet, makingSelection );
  return true;
}


void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
  KSpreadSheet * sheet = activeSheet();

  if ( !sheet || formatKeyPress( _ev ))
    return;

  // Dont handle the remaining special keys.
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) &&
       (_ev->key() != Key_Down) &&
       (_ev->key() != Key_Up) &&
       (_ev->key() != Key_Right) &&
       (_ev->key() != Key_Left) &&
       (_ev->key() != Key_Home) &&
       (_ev->key() != Key_Enter) &&
       (_ev->key() != Key_Return) )
  {
    QWidget::keyPressEvent( _ev );
    return;
  }

  // Always accept so that events are not
  // passed to the parent.
  _ev->accept();

  d->view->doc()->emitBeginOperation(false);
  switch( _ev->key() )
  {
   case Key_Return:
   case Key_Enter:
    processEnterKey( _ev );
    return;
    break;
   case Key_Down:
   case Key_Up:
   case Key_Left:
   case Key_Right:
   case Key_Tab: /* a tab behaves just like a right/left arrow */
   case Key_Backtab:  /* and so does Shift+Tab */
    if (_ev->state() & ControlButton)
    {
      if ( !processControlArrowKey( _ev ) )
        return;
    }
    else
    {
      processArrowKey( _ev );
      return;
    }
    break;

   case Key_Escape:
    processEscapeKey( _ev );
    return;
    break;

   case Key_Home:
    if ( !processHomeKey( _ev ) )
      return;
    break;

   case Key_End:
    if ( !processEndKey( _ev ) )
      return;
    break;

   case Key_Prior:  /* Page Up */
    if ( !processPriorKey( _ev ) )
      return;
    break;

   case Key_Next:   /* Page Down */
    if ( !processNextKey( _ev ) )
      return;
    break;

   case Key_Delete:
    processDeleteKey( _ev );
    return;
    break;

   case Key_F2:
    processF2Key( _ev );
    return;
    break;

   case Key_F4:
    processF4Key( _ev );
    return;
    break;

   default:
    processOtherKey( _ev );
    return;
    break;
  }

  //most process*Key methods call emitEndOperation, this only gets called in some situations
  // (after some move operations)
  d->view->doc()->emitEndOperation( sheet->visibleRect( this ) );
  return;
}

void KSpreadCanvas::processIMEvent( QIMEvent * event )
{
  d->view->doc()->emitBeginOperation( false );
  if ( !d->cellEditor && !d->chooseCell )
  {
    // Switch to editing mode
    createEditor( CellEditor );
    d->cellEditor->handleIMEvent( event );
  }

  QPoint cursor;

  if ( d->chooseCell )
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  d->view->doc()->emitEndOperation( QRect( cursor, cursor ) );
}

bool KSpreadCanvas::formatKeyPress( QKeyEvent * _ev )
{
  if (!(_ev->state() & ControlButton ))
    return false;

  int key = _ev->key();
  if ( key != Key_Exclam && key != Key_At && key != Key_Ampersand
       && key != Key_Dollar && key != Key_Percent && key != Key_AsciiCircum
       && key != Key_NumberSign )
    return false;

  KSpreadCell  * cell = 0L;
  KSpreadSheet * sheet = activeSheet();
  QRect rect = selection();

  d->view->doc()->emitBeginOperation(false);
  sheet->setRegionPaintDirty( rect );
  int right  = rect.right();
  int bottom = rect.bottom();

  if ( !d->view->doc()->undoLocked() )
  {
    QString dummy;
    KSpreadUndoCellFormat * undo = new KSpreadUndoCellFormat( d->view->doc(), sheet, rect, dummy );
    d->view->doc()->addCommand( undo );
  }

  if ( util_isRowSelected(selection()) )
  {
    for ( int r = rect.top(); r <= bottom; ++r )
    {
      cell = sheet->getFirstCellRow( r );
      while ( cell )
      {
        if ( cell->isObscuringForced() )
        {
          cell = sheet->getNextCellRight( cell->column(), r );
          continue;
        }

        formatCellByKey (cell, _ev->key(), rect);

        cell = sheet->getNextCellRight( cell->column(), r );
      } // while (cell)
      RowFormat * rw = sheet->nonDefaultRowFormat( r );
      QPen pen;
      switch ( _ev->key() )
      {
       case Key_Exclam:
        rw->setFormatType (Number_format);
        rw->setPrecision( 2 );
        break;

       case Key_Dollar:
        rw->setFormatType (Money_format);
        rw->setPrecision( d->view->doc()->locale()->fracDigits() );
        break;

       case Key_Percent:
        rw->setFormatType (Percentage_format);
        break;

       case Key_At:
        rw->setFormatType( SecondeTime_format );
        break;

       case Key_NumberSign:
        rw->setFormatType( ShortDate_format );
        break;

       case Key_AsciiCircum:
        rw->setFormatType( Scientific_format );
        break;

       case Key_Ampersand:
        if ( r == rect.top() )
        {
          pen = QPen( d->view->borderColor(), 1, SolidLine);
          rw->setTopBorderPen( pen );
        }
        if ( r == rect.bottom() )
        {
          pen = QPen( d->view->borderColor(), 1, SolidLine);
          rw->setBottomBorderPen( pen );
        }
        break;

       default:
         d->view->doc()->emitEndOperation( rect );
        return false;
      }
      sheet->emit_updateRow( rw, r );
    }

    d->view->doc()->emitEndOperation( rect );
    return true;
  }

  if ( util_isColumnSelected(selection()) )
  {
    for ( int c = rect.left(); c <= right; ++c )
    {
      cell = sheet->getFirstCellColumn( c );
      while ( cell )
      {
        if ( cell->isObscuringForced() )
        {
          cell = sheet->getNextCellDown( c, cell->row() );
          continue;
        }

        formatCellByKey (cell, _ev->key(), rect);

        cell = sheet->getNextCellDown( c, cell->row() );
      }

      ColumnFormat * cw = sheet->nonDefaultColumnFormat( c );
      QPen pen;
      switch ( _ev->key() )
      {
       case Key_Exclam:
        cw->setFormatType( Number_format );
        cw->setPrecision( 2 );
        break;

       case Key_Dollar:
        cw->setFormatType( Money_format );
        cw->setPrecision( d->view->doc()->locale()->fracDigits() );
        break;

       case Key_Percent:
        cw->setFormatType( Percentage_format );
        break;

       case Key_At:
        cw->setFormatType( SecondeTime_format );
        break;

       case Key_NumberSign:
        cw->setFormatType( ShortDate_format );
        break;

       case Key_AsciiCircum:
        cw->setFormatType( Scientific_format );
        break;

       case Key_Ampersand:
        if ( c == rect.left() )
        {
          pen = QPen( d->view->borderColor(), 1, SolidLine);
          cw->setLeftBorderPen( pen );
        }
        if ( c == rect.right() )
        {
          pen = QPen( d->view->borderColor(), 1, SolidLine);
          cw->setRightBorderPen( pen );
        }
        break;

       default:
         d->view->doc()->emitEndOperation( rect );
         return false;
      }
      sheet->emit_updateColumn( cw, c );
    }
    d->view->doc()->emitEndOperation( rect );
    return true;
  }

  for ( int row = rect.top(); row <= bottom; ++row )
  {
    for ( int col = rect.left(); col <= right; ++ col )
    {
      cell = sheet->nonDefaultCell( col, row );

      if ( cell->isObscuringForced() )
        continue;

      formatCellByKey (cell, _ev->key(), rect);
    } // for left .. right
  } // for top .. bottom
  _ev->accept();

  d->view->doc()->emitEndOperation( rect );
  return true;
}

bool KSpreadCanvas::formatCellByKey (KSpreadCell *cell, int key, const QRect &rect)
{
  QPen pen;
  switch (key)
  {
    case Key_Exclam:
    cell->convertToDouble ();
    cell->setFormatType (Number_format);
    cell->setPrecision( 2 );
    break;

    case Key_Dollar:
    cell->convertToMoney ();
    break;

    case Key_Percent:
    cell->convertToPercent ();
    break;

    case Key_At:
    cell->convertToTime ();
    break;

    case Key_NumberSign:
    cell->convertToDate ();
    break;

    case Key_AsciiCircum:
    cell->setFormatType (Scientific_format);
    cell->convertToDouble ();
    break;

    case Key_Ampersand:
    if ( cell->row() == rect.top() )
    {
      pen = QPen( d->view->borderColor(), 1, SolidLine);
      cell->setTopBorderPen( pen );
    }
    if ( cell->row() == rect.bottom() )
    {
      pen = QPen( d->view->borderColor(), 1, SolidLine);
      cell->setBottomBorderPen( pen );
    }
    if ( cell->column() == rect.left() )
    {
      pen = QPen( d->view->borderColor(), 1, SolidLine);
      cell->setLeftBorderPen( pen );
    }
    if ( cell->column() == rect.right() )
    {
      pen = QPen( d->view->borderColor(), 1, SolidLine);
      cell->setRightBorderPen( pen );
    }
    break;
  } // switch

  return true;
}

void KSpreadCanvas::doAutoScroll()
{
    if ( !d->mousePressed )
    {
        d->scrollTimer->stop();
        return;
    }

    bool select = false;
    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    //Provide progressive scrolling depending on the mouse position
    if ( pos.y() < 0 )
    {
        vertScrollBar()->setValue ((int) (vertScrollBar()->value() -
                                   autoScrollAccelerationY( - pos.y())));
        select = true;
    }
    else if ( pos.y() > height() )
    {
        vertScrollBar()->setValue ((int) (vertScrollBar()->value() +
                                   autoScrollAccelerationY (pos.y() - height())));
        select = true;
    }

    if ( pos.x() < 0 )
    {
        horzScrollBar()->setValue ((int) (horzScrollBar()->value() -
                                   autoScrollAccelerationX( - pos.x() )));
        select = true;
    }
    else if ( pos.x() > width() )
    {
        horzScrollBar()->setValue ((int) (horzScrollBar()->value() +
                                 autoScrollAccelerationX( pos.x() - width())));
        select = true;
    }

    if ( select )
    {
        QMouseEvent * event = new QMouseEvent(QEvent::MouseMove, pos, 0, 0);
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    d->scrollTimer->start( 50 );
}

double KSpreadCanvas::autoScrollAccelerationX( int offset )
{
    switch( static_cast<int>( offset / 20 ) )
    {
        case 0: return 5.0;
        case 1: return 20.0;
        case 2: return d->view->doc()->unzoomItX( width() );
        case 3: return d->view->doc()->unzoomItX( width() );
        default: return d->view->doc()->unzoomItX( (int) (width() * 5.0) );
    }
}

double KSpreadCanvas::autoScrollAccelerationY( int offset )
{
    switch( static_cast<int>( offset / 20 ) )
    {
        case 0: return 5.0;
        case 1: return 20.0;
        case 2: return d->view->doc()->unzoomItY( height() );
        case 3: return d->view->doc()->unzoomItY( height() );
        default: return d->view->doc()->unzoomItY( (int) (height() * 5.0) );
    }
}

void KSpreadCanvas::deleteEditor (bool saveChanges, bool array)
{
  if ( !d->cellEditor )
    return;
  // We need to set the line-edit out of edit mode,
  // but only if we are using it (text editor)
  // A bit of a hack - perhaps we should store the editor mode ?
  bool textEditor = true;
  if ( d->cellEditor->inherits("KSpreadTextEditor") )
      d->editWidget->setEditMode( false );
  else
      textEditor = false;

  QString t = d->cellEditor->text();
  // Delete the cell editor first and after that update the document.
  // That means we get a synchronous repaint after the cell editor
  // widget is gone. Otherwise we may get painting errors.
  delete d->cellEditor;
  d->cellEditor = 0;

  if ( saveChanges && textEditor )
  {
      if ( t.at(0)=='=' )
      {
          //a formula
          int openParenthese = t.contains('(' );
          int closeParenthese = t.contains(')' );
          int diff = QABS( openParenthese - closeParenthese );
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
    d->view->updateEditWidget();

  setHighlightedRanges(0);
  setFocus();
}

void KSpreadCanvas::createEditor()
{
  KSpreadCell * cell = activeSheet()->nonDefaultCell( markerColumn(), markerRow(), false );

  if ( !createEditor( CellEditor ) )
      return;
  if ( cell )
      d->cellEditor->setText( cell->text() );
}

bool KSpreadCanvas::createEditor( EditorType ed, bool addFocus )
{
  KSpreadSheet * sheet = activeSheet();
  if ( !d->cellEditor )
  {
    KSpreadCell * cell = sheet->nonDefaultCell( marker().x(), marker().y(), false );

    if ( sheet->isProtected() && !cell->notProtected( marker().x(), marker().y() ) )
      return false;

    if ( ed == CellEditor )
    {
      d->editWidget->setEditMode( true );

      d->cellEditor = new KSpreadTextEditor( cell, this );
    }

    double w, h;
    double min_w = cell->dblWidth( markerColumn() );
    double min_h = cell->dblHeight( markerRow() );
    if ( cell->isDefault() )
    {
      w = min_w;
      h = min_h;
      //kdDebug(36001) << "DEFAULT" << endl;
    }
    else
    {
      w = cell->extraWidth();
      h = cell->extraHeight();
      //kdDebug(36001) << "HEIGHT=" << min_h << " EXTRA=" << h << endl;
    }

    double xpos = sheet->dblColumnPos( markerColumn() ) - xOffset();

    KSpreadSheet::LayoutDirection sheetDir = sheet->layoutDirection();
    bool rtlText = cell->strOutText().isRightToLeft();

    // if sheet and cell direction don't match, then the editor's location
    // needs to be shifted backwards so that it's right above the cell's text
    if ( w > 0 && ( ( sheetDir == KSpreadSheet::RightToLeft && !rtlText ) ||
                    ( sheetDir == KSpreadSheet::LeftToRight && rtlText  ) ) )
      xpos -= w - min_w;

    // paint editor above correct cell if sheet direction is RTL
    if ( sheetDir == KSpreadSheet::RightToLeft )
    {
      double dwidth = d->view->doc()->unzoomItX( width() );
      double w2 = QMAX( w, min_w );
      xpos = dwidth - w2 - xpos;
    }
         
    double ypos = sheet->dblRowPos( markerRow() ) - yOffset();
    QPalette p = d->cellEditor->palette();
    QColorGroup g( p.active() );

    QColor color = cell->textColor( markerColumn(), markerRow() );
    if ( !color.isValid() )
        color = QApplication::palette().active().text();
    g.setColor( QColorGroup::Text, color);

    color = cell->bgColor( markerColumn(), markerRow() );
    if ( !color.isValid() )
        color = g.base();
    g.setColor( QColorGroup::Background, color );

    d->cellEditor->setPalette( QPalette( g, p.disabled(), g ) );
    QFont tmpFont = cell->textFont( markerColumn(), markerRow() );
    tmpFont.setPointSizeFloat( 0.01 * d->view->doc()->zoom() * tmpFont.pointSizeFloat() );
    d->cellEditor->setFont( tmpFont );

    KoRect rect( xpos, ypos, w, h ); //needed to circumvent rounding issue with height/width

	
    QRect zoomedRect=d->view->doc()->zoomRect( rect );
	/*zoomedRect.setLeft(zoomedRect.left()-2);
	zoomedRect.setRight(zoomedRect.right()+4);
	zoomedRect.setTop(zoomedRect.top()-1);
	zoomedRect.setBottom(zoomedRect.bottom()+2);*/

    d->cellEditor->setGeometry( zoomedRect );
    d->cellEditor->setMinimumSize( QSize( d->view->doc()->zoomItX( min_w ), d->view->doc()->zoomItY( min_h ) ) );
    d->cellEditor->show();
    //kdDebug(36001) << "FOCUS1" << endl;
    //Laurent 2001-12-05
    //Don't add focus when we create a new editor and
    //we select text in edit widget otherwise we don't delete
    //selected text.
  //  startChoose();

    if ( addFocus )
        d->cellEditor->setFocus();

	setSelectionChangePaintDirty(sheet,selection(),QRect(0,0,0,0));
	paintUpdates();
  }

  return true;
}

void KSpreadCanvas::closeEditor()
{
  if ( d->chooseCell )
    return;

  if ( d->cellEditor )
  {
    deleteEditor( true ); // save changes
  }
}


void KSpreadCanvas::updateChooseRect(const QPoint &newMarker, const QPoint &newAnchor)
{
  if( !d->chooseCell )
    return;

  KSpreadSheet* sheet = activeSheet();

  if ( ! sheet )
      return;

  QPoint oldAnchor = selectionInfo()->getChooseAnchor();
  QPoint oldMarker = selectionInfo()->getChooseMarker();
  QPoint chooseCursor = selectionInfo()->getChooseCursor();
  QRect oldChooseRect = selectionInfo()->getChooseRect();


  if ( newMarker == oldMarker && newAnchor == oldAnchor )
  {
    return;
  }

  selectionInfo()->setChooseMarker(newMarker);
  selectionInfo()->setChooseAnchor(newAnchor);

  QRect newChooseRect = selectionInfo()->getChooseRect();

  /* keep the choose cursor updated.  If you don't know what the 'cursor' is
     supposed to represent, check the comments of the regular selection cursor
     in kspread_selection.h (KSpreadSelection::m_cursorPosition).  It's the
     same thing here except for the choose selection.
  */
  if ( !newChooseRect.contains(chooseCursor) )
  {
    selectionInfo()->setChooseCursor(sheet, newMarker);
  }

  d->view->doc()->emitBeginOperation();
  setSelectionChangePaintDirty(sheet, oldChooseRect, newChooseRect);
  repaint();
  d->view->doc()->emitEndOperation();

  /* this signal is used in the formula editor to update the text display */
  emit d->view->sig_chooseSelectionChanged(activeSheet(), newChooseRect);

  if ( !d->cellEditor )
  {
    d->length_namecell = 0;
    return;
  }

  /* the rest of this function updates the text showing the choose rect */
  /***** TODO - should this be here? */

  if (newMarker.x() != 0 && newMarker.y() != 0)
    /* don't update the text if we are removing the marker */
  {
    QString name_cell;

    if ( d->chooseStartSheet != sheet )
    {
      if ( newMarker == newAnchor )
        name_cell = KSpreadCell::fullName( sheet, newChooseRect.left(), newChooseRect.top() );
      else
        name_cell = util_rangeName( sheet, newChooseRect );
    }
    else
    {
      if ( newMarker == newAnchor )
        name_cell = KSpreadCell::name( newChooseRect.left(), newChooseRect.top() );
      else
        name_cell = util_rangeName( newChooseRect );
    }

    int old = d->length_namecell;
    d->length_namecell= name_cell.length();
    d->length_text = d->cellEditor->text().length();
    //kdDebug(36001) << "updateChooseMarker2 len=" << d->length_namecell << endl;

    QString text = d->cellEditor->text();
    QString res = text.left( d->cellEditor->cursorPosition() - old ) + name_cell + text.right( text.length() - d->cellEditor->cursorPosition() );
    int pos = d->cellEditor->cursorPosition() - old;

    ((KSpreadTextEditor*)d->cellEditor)->blockCheckChoose( true );
    d->cellEditor->setText( res );
    ((KSpreadTextEditor*)d->cellEditor)->blockCheckChoose( false );
    d->cellEditor->setCursorPosition( pos + d->length_namecell );
    d->editWidget->setText( res );
    //kdDebug(36001) << "old=" << old << " len=" << d->length_namecell << " pos=" << pos << endl;
  }
}


void KSpreadCanvas::setSelectionChangePaintDirty(KSpreadSheet* sheet,
                                                 QRect area1, QRect area2)
{
  QValueList<QRect> cellRegions;

  /* first of all, let's not get confused by an unset region at 0,0,0,0
     Just reset to region to something ridiculous that will be ignored by a paint call
   */
  if (area1.contains(QPoint(0,0)))
  {
    area1.setLeft(-100);
    area1.setRight(-100);
  }

  if (area2.contains(QPoint(0,0)))
  {
    area2.setLeft(-50);
    area2.setRight(-50);
  }

  /* let's try to only paint where the selection is actually changing*/
  bool newLeft   = area1.left() != area2.left();
  bool newTop    = area1.top() != area2.top();
  bool newRight  = area1.right() != area2.right();
  bool newBottom = area1.bottom() != area2.bottom();
  bool topLeftSame = !newLeft && !newTop;
  bool topRightSame = !newTop && !newRight;
  bool bottomLeftSame = !newLeft && !newBottom;
  bool bottomRightSame = !newBottom && !newRight;

  if (!topLeftSame && !topRightSame && !bottomLeftSame && !bottomRightSame)
  {
    /* the two areas are not related. */
    /* since the marker/selection border extends into neighboring cells, we
       want to calculate all the cells bordering these regions.
    */
    ExtendRectBorder(area1);
    ExtendRectBorder(area2);
    cellRegions.append(area1);
    cellRegions.append(area2);
  }
  else
  {
    /* at least one corner is the same -- let's only paint the extension
       on corners that are not the same
    */

    /* first, calculate some numbers that we'll use a few times */
    int farLeft = QMIN(area1.left(), area2.left());
    if (farLeft != 1) farLeft--;
    int innerLeft = QMAX(area1.left(), area2.left());
    if (innerLeft != KS_colMax) innerLeft++;

    int farTop = QMIN(area1.top(), area2.top());
    if (farTop != 1) farTop--;
    int innerTop = QMAX(area1.top(), area2.top());
    if (innerTop != KS_rowMax) innerTop++;

    int farRight = QMAX(area1.right(), area2.right());
    if (farRight != KS_colMax) farRight++;
    int innerRight = QMIN(area1.right(), area2.right());
    if (innerRight != 1) innerRight--;

    int farBottom = QMAX(area1.bottom(), area2.bottom());
    if (farBottom!= KS_rowMax) farBottom++;
    int innerBottom = QMIN(area1.bottom(), area2.bottom());
    if (innerBottom != 1) innerBottom--;

    if (newLeft)
    {
      cellRegions.append(QRect(QPoint(farLeft, farTop),
                               QPoint(innerLeft, farBottom)));
    }

    if (newTop)
    {
      cellRegions.append(QRect(QPoint(farLeft, farTop),
                               QPoint(farRight, innerTop)));
    }

    if (newRight)
    {
      cellRegions.append(QRect(QPoint(innerRight, farTop),
                               QPoint(farRight, farBottom)));
    }

    if (newBottom)
    {
      cellRegions.append(QRect(QPoint(farLeft, innerBottom),
                               QPoint(farRight, farBottom)));
    }
  }

  QValueList<QRect>::iterator it = cellRegions.begin();

  while (it != cellRegions.end())
  {
    sheet->setRegionPaintDirty(*it);
    it++;
  }
}

void KSpreadCanvas::ExtendRectBorder(QRect& area)
{
  ColumnFormat *cl;
  RowFormat *rl;
  //look at if column is hiding.
  //if it's hiding refreshing column+1 (or column -1 )
  int left = area.left();
  int right = area.right();
  int top = area.top();
  int bottom = area.bottom();

  //Maybe the case for ridiculous settings, see setSelectionChangePaintDirty
  //No need to extend then, avoids warnings
  if ( left < 1 && right < 1 )
      return;

  if ( right < KS_colMax )
  {
    do
    {
      right++;
      cl = activeSheet()->nonDefaultColumnFormat( right );
    } while ( cl->isHide() && right != KS_colMax );
  }
  if ( left > 1 )
  {
    do
    {
      left--;
      cl = activeSheet()->nonDefaultColumnFormat( left );
    } while ( cl->isHide() && left != 1);
  }

  if ( bottom < KS_rowMax )
  {
    do
    {
      bottom++;
      rl = activeSheet()->nonDefaultRowFormat( bottom );
    } while ( rl->isHide() && bottom != KS_rowMax );
  }

  if ( top > 1 )
  {
    do
    {
      top--;
      rl = activeSheet()->nonDefaultRowFormat( top );
    } while ( rl->isHide() && top != 1);
  }

  area.setLeft(left);
  area.setRight(right);
  area.setTop(top);
  area.setBottom(bottom);
}


void KSpreadCanvas::updatePosWidget()
{
    QString buffer;
    // No selection, or only one cell merged selected
    if ( selectionInfo()->singleCellSelection() )
    {
        if (activeSheet()->getLcMode())
        {
            buffer = "L" + QString::number( markerRow() ) +
		"C" + QString::number( markerColumn() );
        }
        else
        {
            buffer = KSpreadCell::columnName( markerColumn() ) +
		QString::number( markerRow() );
        }
    }
    else
    {
        if (activeSheet()->getLcMode())
        {
            buffer = QString::number( (selection().bottom()-selection().top()+1) )+"Lx";
            if ( util_isRowSelected( selection() ) )
                buffer+=QString::number((KS_colMax-selection().left()+1))+"C";
            else
                buffer+=QString::number((selection().right()-selection().left()+1))+"C";
        }
        else
        {
                //encodeColumnLabelText return @@@@ when column >KS_colMax
                //=> it's not a good display
                //=> for the moment I display pos of marker
                buffer=KSpreadCell::columnName( selection().left() ) +
		    QString::number(selection().top()) + ":" +
		    KSpreadCell::columnName( QMIN( KS_colMax, selection().right() ) ) +
		    QString::number(selection().bottom());
                //buffer=activeSheet()->columnLabel( m_iMarkerColumn );
                //buffer+=tmp.setNum(m_iMarkerRow);
        }
  }

    if (buffer != d->posWidget->lineEdit()->text())
      d->posWidget->lineEdit()->setText(buffer);
}




void KSpreadCanvas::adjustArea(bool makeUndo)
{
  QRect s( selection() );
  if (activeSheet()->areaIsEmpty(s))
        return;

  if (makeUndo)
  {
        if ( !d->view->doc()->undoLocked() )
        {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( d->view->doc(),activeSheet() , s );
                d->view->doc()->addCommand( undo );
        }
  }
  // Columns selected
  if ( util_isColumnSelected(s) )
  {
    for (int x=s.left(); x <= s.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
  }
  // Rows selected
  else if ( util_isRowSelected(s) )
  {
    for(int y = s.top(); y <= s.bottom(); y++ )
    {
      vBorderWidget()->adjustRow(y,false);
    }
  }
  // No selection
  // Selection of a rectangular area
  else
  {
    for (int x=s.left(); x <= s.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
    for(int y = s.top(); y <= s.bottom(); y++ )
    {
      vBorderWidget()->adjustRow(y,false);
    }
  }
}

void KSpreadCanvas::equalizeRow()
{
  QRect s( selection() );
  RowFormat *rl = d->view->activeSheet()->rowFormat(s.top());
  int size=rl->height(this);
  if ( s.top() == s.bottom() )
      return;
  for(int i=s.top()+1;i<=s.bottom();i++)
  {
      KSpreadSheet *sheet = activeSheet();
      if ( !sheet )
          return;
      size=QMAX(d->view->activeSheet()->rowFormat(i)->height(this),size);
  }
  d->view->vBorderWidget()->equalizeRow(size);
}

void KSpreadCanvas::equalizeColumn()
{
  QRect s( selection() );
  ColumnFormat *cl = d->view->activeSheet()->columnFormat(s.left());
  int size=cl->width(this);
  if ( s.left() == s.right() )
      return;

  for(int i=s.left()+1;i<=s.right();i++)
  {
    size=QMAX(d->view->activeSheet()->columnFormat(i)->width(this),size);
  }
  d->view->hBorderWidget()->equalizeColumn(size);
}

QRect KSpreadCanvas::visibleCells()
{
  KoRect unzoomedRect = d->view->doc()->unzoomRect( QRect( 0, 0, width(), height() ) );
  unzoomedRect.moveBy( xOffset(), yOffset() );

  double tmp;
  int left_col = activeSheet()->leftColumn( unzoomedRect.left(), tmp );
  int right_col = activeSheet()->rightColumn( unzoomedRect.right() );
  int top_row = activeSheet()->topRow( unzoomedRect.top(), tmp );
  int bottom_row = activeSheet()->bottomRow( unzoomedRect.bottom() );

  return QRect( left_col, top_row,
                right_col - left_col + 1, bottom_row - top_row + 1 );
}


//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadCanvas::paintUpdates()
{
  if (activeSheet() == NULL)
    return;

  QPainter painter(this);

  //Save clip region
  QRegion rgnComplete( painter.clipRegion() );
  QWMatrix matrix;
  if ( d->view )
  {
    matrix = d->view->matrix();
  }
  else
  {
    matrix = painter.worldMatrix();
  }

  painter.save();
  clipoutChildren( painter, matrix );

  KoRect unzoomedRect = d->view->doc()->unzoomRect( QRect( 0, 0, width(), height() ) );
  // unzoomedRect.moveBy( xOffset(), yOffset() );


  /* paint any visible cell that has the paintDirty flag */
  QRect range = visibleCells();
  KSpreadCell* cell = NULL;

  double topPos = activeSheet()->dblRowPos(range.top());
  double leftPos = activeSheet()->dblColumnPos(range.left());

  KoPoint dblCorner( leftPos - xOffset(), topPos - yOffset() );

  int x;
  int y;

  int right  = range.right();
  int bottom = range.bottom();
  KSpreadSheet * sheet = activeSheet();

#if 0
  kdDebug(36001) 
    << "================================================================"
    << endl;
  kdDebug(36001) << "painting dirty cells " << endl;
#endif

  for ( x = range.left(); x <= right; ++x )
  {
    for ( y = range.top(); y <= bottom; ++y )
    {
      if ( sheet->cellIsPaintDirty( QPoint( x, y ) ) )
      {
        cell = sheet->cellAt( x, y );

        // recalc and relayout only for non default cells
        if( !cell->isDefault() )
        {
          cell->calc();
          cell->makeLayout( painter, x, y );
        }

       /* bool paintBordersBottom = false;
        bool paintBordersRight = false;
        bool paintBordersLeft = false;
	bool paintBordersTop = false; */
	
	int paintBorder=KSpreadCell::Border_None;

        QPen bottomPen( cell->effBottomBorderPen( x, y ) );
        QPen rightPen( cell->effRightBorderPen( x, y ) );
        QPen leftPen( cell->effLeftBorderPen( x, y ) );
        QPen topPen( cell->effTopBorderPen( x, y ) );

        // paint right border if rightmost cell or if the pen is more "worth" than the left border pen
        // of the cell on the left or if the cell on the right is not painted. In the latter case get
        // the pen that is of more "worth"
        if ( x >= KS_colMax )
         // paintBordersRight = true;
		paintBorder |= KSpreadCell::Border_Right;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x + 1, y ) ) )
          {
            //paintBordersRight = true;
		  paintBorder |= KSpreadCell::Border_Right;
            if ( cell->effRightBorderValue( x, y ) < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
              rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
          }
        else
        {
         // paintBordersRight = true;
		paintBorder |= KSpreadCell::Border_Right;
          if ( cell->effRightBorderValue( x, y ) < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
            rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
        }

        // similiar for other borders...
        // bottom border:
        if ( y >= KS_rowMax )
         // paintBordersBottom = true;
		paintBorder |= KSpreadCell::Border_Bottom;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x, y + 1 ) ) )
          {
            if ( cell->effBottomBorderValue( x, y ) > sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1 ) )
             // paintBordersBottom = true;
		    paintBorder |= KSpreadCell::Border_Bottom;
          }
        else
        {
          //paintBordersBottom = true;
		paintBorder |= KSpreadCell::Border_Bottom;
          if ( cell->effBottomBorderValue( x, y ) < sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1 ) )
            bottomPen = sheet->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
        }

        // left border:
        if ( x == 1 )
         // paintBordersLeft = true;
		paintBorder |= KSpreadCell::Border_Left;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x - 1, y ) ) )
          {
           // paintBordersLeft = true;
		  paintBorder |= KSpreadCell::Border_Left;
            if ( cell->effLeftBorderValue( x, y ) < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
              leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
          }
        else
        {
		paintBorder |= KSpreadCell::Border_Left;
          if ( cell->effLeftBorderValue( x, y ) < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
            leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
        }

        // top border:
        if ( y == 1 )
        //  paintBordersTop = true;
		paintBorder |= KSpreadCell::Border_Top;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x, y - 1 ) ) )
          {
          //  paintBordersTop = true;
		  paintBorder |= KSpreadCell::Border_Top;
            if ( cell->effTopBorderValue( x, y ) < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
              topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
          }
        else
        {
        //  paintBordersTop = true;
		paintBorder |= KSpreadCell::Border_Top;
          if ( cell->effTopBorderValue( x, y ) < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
            topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
        }
	
	
	cell->paintCell( unzoomedRect, painter, d->view, dblCorner, QPoint( x, y), paintBorder,
				 rightPen,bottomPen,leftPen,topPen);
	

      }
      dblCorner.setY( dblCorner.y() + sheet->rowFormat( y )->dblHeight( ) );
    }
    dblCorner.setY( topPos - yOffset() );
    dblCorner.setX( dblCorner.x() + sheet->columnFormat( x )->dblWidth( ) );
  }   

  /* now paint the selection */
  //Nb.  No longer necessary to paint choose selection here as the cell reference highlight
  //stuff takes care of this anyway
  
 // paintChooseRect(painter, unzoomedRect);
  paintHighlightedRanges(painter, unzoomedRect);
  paintNormalMarker(painter, unzoomedRect);

  //restore clip region with children area
  painter.restore();
  painter.setClipRegion( rgnComplete );
  paintChildren( painter, matrix );
}



void KSpreadCanvas::clipoutChildren( QPainter& painter, QWMatrix& matrix )
{
  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, width(), height() ) );

  QPtrListIterator<KoDocumentChild> itChild( d->view->doc()->children() );
  for( ; itChild.current(); ++itChild )
  {
//    if ( ((KSpreadChild*)it.current())->sheet() == sheet &&
//         !d->view->hasDocumentInWindow( it.current()->document() ) )
    if ( ( ( KSpreadChild*)itChild.current() )->sheet() == activeSheet() )
    {
      rgn -= itChild.current()->region( matrix );
    }
  }
  painter.setClipRegion( rgn );
}

void KSpreadCanvas::paintChildren( QPainter& painter, QWMatrix& matrix )
{
  painter.setWorldMatrix( matrix );
  QPtrListIterator<KoDocumentChild> itChild( d->view->doc()->children() );
  itChild.toFirst();
  for( ; itChild.current(); ++itChild )
  {
    if ( ( ( KSpreadChild*)itChild.current() )->sheet() == activeSheet() &&
         ( d->view && !d->view->hasDocumentInWindow( itChild.current()->document() ) ) )
    {
      // #### todo: paint only if child is visible inside rect
      painter.save();
      d->view->doc()->paintChild( itChild.current(), painter, d->view,
        d->view->doc()->zoomedResolutionX(), d->view->doc()->zoomedResolutionY() );
      painter.restore();
    }
  }
}

	
void KSpreadCanvas::paintChooseRect(QPainter& painter, const KoRect &viewRect)
{
	 
  double positions[4];
  bool paintSides[4];

  QRect chooseRect = d->view->selectionInfo()->getChooseRect();

  if ( chooseRect.left() != 0 )
  {
    QPen pen;
    pen.setWidth( 2 );
    pen.setStyle( DashLine );

    retrieveMarkerInfo( chooseRect, viewRect, positions, paintSides );

    double left =   positions[0];
    double top =    positions[1];
    double right =  positions[2];
    double bottom = positions[3];

    bool paintLeft =   paintSides[0];
    bool paintTop =    paintSides[1];
    bool paintRight =  paintSides[2];
    bool paintBottom = paintSides[3];

    RasterOp rop = painter.rasterOp();
    painter.setRasterOp( NotROP );
    painter.setPen( pen );

    if ( paintTop )
    {
      painter.drawLine( d->view->doc()->zoomItX( left ),  d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ) );
    }
    if ( paintLeft )
    {
      painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) );
    }
    if ( paintRight )
    {
      painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) );
    }
    if ( paintBottom )
    {
      painter.drawLine( d->view->doc()->zoomItX( left ),  d->view->doc()->zoomItY( bottom ),
                        d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) );
    }

    /* restore the old raster mode */
    painter.setRasterOp( rop );
  }
  return;
}
    
    
void KSpreadCanvas::paintHighlightedRanges(QPainter& painter, const KoRect& viewRect)
{
	if (!d->highlightedRanges)
		return;
	
	std::vector<HighlightRange>::iterator iter;
	
	QBrush nullBrush;

	for (iter=d->highlightedRanges->begin();iter != d->highlightedRanges->end();iter++)
	{ 
		//Only paint ranges or cells on the current sheet
		if (iter->firstCell->sheet != activeSheet())
			continue;
		
		QRect region;
		
		region.setTop(iter->firstCell->row());
		region.setLeft(iter->firstCell->column());
		
		if (iter->lastCell) //is this a range (lastCell != 0) or a single cell (lastCell == 0) ?
		{
			region.setBottom(iter->lastCell->row());
			region.setRight(iter->lastCell->column());	
		}
		else
		{
			region.setBottom(region.top());
			region.setRight(region.left());
		}
		
		//Sanity-check: If the top-left corner is lower and/or further right than the bottom-right corner,
		//the user has entered an invalid region and we should go to the next range.
		if (!region.isValid())
			continue;
		
		//double positions[4];
		//bool paintSides[4];
		KoRect unzoomedRect;
		
		sheetAreaToVisibleRect(region,unzoomedRect);
		//Convert region from sheet coordinates to canvas coordinates for use with the painter
		//retrieveMarkerInfo(region,viewRect,positions,paintSides);
		
		QPen highlightPen(iter->color);
		painter.setPen(highlightPen);
		
		//Adjust the canvas coordinate - rect to take account of zoom level
		
		QRect zoomedRect;
		
		zoomedRect.setCoords (	d->view->doc()->zoomItX(unzoomedRect.left()),
					d->view->doc()->zoomItY(unzoomedRect.top()),
					d->view->doc()->zoomItX(unzoomedRect.right()),
					d->view->doc()->zoomItY(unzoomedRect.bottom()) );
		
		//Now adjust the highlight rectangle is slightly inside the cell borders (this means that multiple highlighted cells 
		//look nicer together as the borders do not clash)
		
		zoomedRect.setLeft(zoomedRect.left()+2);
		zoomedRect.setTop(zoomedRect.top()+2);
		zoomedRect.setRight(zoomedRect.right()-2);
		zoomedRect.setBottom(zoomedRect.bottom()-1);
		
		painter.setBrush(nullBrush);
		painter.drawRect(zoomedRect);
		
		//Now draw the size grip (the little rectangle on the bottom right-hand corner of the range which the user can 
		//click and drag to resize the region)
		
		
		QBrush sizeGripBrush(iter->color);
		QPen   sizeGripPen(Qt::white);
		
		painter.setPen(sizeGripPen);
		painter.setBrush(sizeGripBrush);
		
		painter.drawRect(zoomedRect.right()-3,zoomedRect.bottom()-3,6,6);
		
	}
}
    
void KSpreadCanvas::paintNormalMarker(QPainter& painter, const KoRect &viewRect)
{

  if( d->chooseCell )
    	return;

  if (d->cellEditor)
	return;

  double positions[4];
  bool paintSides[4];

  QRect marker = selection();

  QPen pen( Qt::black, 3 );
  painter.setPen( pen );

  retrieveMarkerInfo( marker, viewRect, positions, paintSides );

  painter.setPen( pen );

  double left =   positions[0];
  double top =    positions[1];
  double right =  positions[2];
  double bottom = positions[3];

  bool paintLeft =   paintSides[0];
  bool paintTop =    paintSides[1];
  bool paintRight =  paintSides[2];
  bool paintBottom = paintSides[3];

  /* the extra '-1's thrown in here account for the thickness of the pen.
     want to look like this:                     not this:
                            * * * * * *                     * * * *
                            *         *                   *         *
     .                      *         *                   *         *
  */
  int l = 1;

  if ( paintTop )
  {
    painter.drawLine( d->view->doc()->zoomItX( left ) - l,      d->view->doc()->zoomItY( top ),
                      d->view->doc()->zoomItX( right ) + 2 * l, d->view->doc()->zoomItY( top ) );
  }
  if ( activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    if ( paintRight )
    {
      painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) );
    }
    if ( paintLeft && paintBottom )
    {
      /* then the 'handle' in the bottom left corner is visible. */
      painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) - 3 );
      painter.drawLine( d->view->doc()->zoomItX( left ) + 4,  d->view->doc()->zoomItY( bottom ),
                        d->view->doc()->zoomItX( right ) + l + 1, d->view->doc()->zoomItY( bottom ) );
      painter.fillRect( d->view->doc()->zoomItX( left ) - 2, d->view->doc()->zoomItY( bottom ) -2, 5, 5,
                        painter.pen().color() );
    }
    else
    {
      if ( paintLeft )
      {
        painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                          d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) );
      }
      if ( paintBottom )
      {
        painter.drawLine( d->view->doc()->zoomItX( left ) - l,  d->view->doc()->zoomItY( bottom ),
                          d->view->doc()->zoomItX( right ) + l + 1, d->view->doc()->zoomItY( bottom ));
      }
    }
  }
  else
  {
    if ( paintLeft )
    {
      painter.drawLine( d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( left ), d->view->doc()->zoomItY( bottom ) );
    }
    if ( paintRight && paintBottom )
    {
      /* then the 'handle' in the bottom right corner is visible. */
      painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                        d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) - 3 );
      painter.drawLine( d->view->doc()->zoomItX( left ) - l,  d->view->doc()->zoomItY( bottom ),
                        d->view->doc()->zoomItX( right ) - 3, d->view->doc()->zoomItY( bottom ) );
      painter.fillRect( d->view->doc()->zoomItX( right ) - 2, d->view->doc()->zoomItY( bottom ) - 2, 5, 5,
                        painter.pen().color() );
    }
    else
    {
      if ( paintRight )
      {
        painter.drawLine( d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( top ),
                          d->view->doc()->zoomItX( right ), d->view->doc()->zoomItY( bottom ) );
      }
      if ( paintBottom )
      {
        painter.drawLine( d->view->doc()->zoomItX( left ) - l,  d->view->doc()->zoomItY( bottom ),
                          d->view->doc()->zoomItX( right ) + l, d->view->doc()->zoomItY( bottom ) );
      }
    }
  }
}

void KSpreadCanvas::sheetAreaToRect(const QRect& sheetArea, KoRect& rect) 
{ 
	KSpreadSheet* sheet=activeSheet();
		
	if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
	{ 
		rect.setLeft(sheet->dblColumnPos( sheetArea.right()+1 ) );
		rect.setRight(sheet->dblColumnPos( sheetArea.left() ));
	} 
	else
	{ 
		rect.setLeft(sheet->dblColumnPos( sheetArea.left() ));
		rect.setRight(sheet->dblColumnPos( sheetArea.right()+1 ));
	}

	rect.setTop(sheet->dblRowPos(sheetArea.top()));
	rect.setBottom(sheet->dblRowPos(sheetArea.bottom()+1));
	
}

void KSpreadCanvas::sheetAreaToVisibleRect( const QRect& sheetArea,
					    KoRect& visibleRect ) 
{
	KSpreadSheet* sheet=activeSheet();
	
	if (!sheet)
		return;
	
	double dwidth=d->view->doc()->unzoomItX(width());
	double xpos;
	double x;
	
	if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
	{
		xpos = dwidth - sheet->dblColumnPos( sheetArea.right() ) + xOffset();
		x    = dwidth - sheet->dblColumnPos( sheetArea.left() ) + xOffset();
	}
	else
	{
		xpos = sheet->dblColumnPos( sheetArea.left() ) - xOffset();
		x    = sheet->dblColumnPos( sheetArea.right() ) - xOffset();
	}
	
	double ypos = sheet->dblRowPos(sheetArea.top())-yOffset();
	
	const ColumnFormat *columnFormat = sheet->columnFormat( sheetArea.right() );
	double tw = columnFormat->dblWidth( );
	double w = x - xpos + tw;

	double y = sheet->dblRowPos( sheetArea.bottom() ) - yOffset();
	const RowFormat* rowFormat = sheet->rowFormat( sheetArea.bottom() );
	double th = rowFormat->dblHeight( );
	double h = ( y - ypos ) + th;

	/* left, top, right, bottom */
	if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
	{
		visibleRect.setLeft(xpos - tw );
		visibleRect.setRight(xpos - tw + w );
	}
	else
	{
		visibleRect.setLeft(xpos );
		visibleRect.setRight(xpos + w );
	}
	visibleRect.setTop(ypos);
	visibleRect.setBottom(ypos + h);	
}

void KSpreadCanvas::retrieveMarkerInfo( const QRect &marker,
                                        const KoRect &viewRect,
                                        double positions[],
                                        bool paintSides[] )
{ 
	
	KSpreadSheet* sheet=activeSheet();
	
	if (!sheet) return;
	
	KoRect visibleRect;
	sheetAreaToVisibleRect(marker,visibleRect);
	
	
 /* KSpreadSheet * sheet = activeSheet();
  if ( !sheet )
    return;

  double dWidth = d->view->doc()->unzoomItX( width() );

  double xpos;
  double x;
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    xpos = dWidth - sheet->dblColumnPos( marker.right() ) + xOffset();
    x    = dWidth - sheet->dblColumnPos( marker.left() ) + xOffset();
  }
  else
  {
    xpos = sheet->dblColumnPos( marker.left() ) - xOffset();
    x    = sheet->dblColumnPos( marker.right() ) - xOffset();
  }
  double ypos = sheet->dblRowPos( marker.top() ) - yOffset();

  const ColumnFormat *columnFormat = sheet->columnFormat( marker.right() );
  double tw = columnFormat->dblWidth( );
  double w = x - xpos + tw;

  double y = sheet->dblRowPos( marker.bottom() ) - yOffset();
  const RowFormat* rowFormat = sheet->rowFormat( marker.bottom() );
  double th = rowFormat->dblHeight( );
  double h = ( y - ypos ) + th;

	//left, top, right, bottom 
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    positions[0] = xpos - tw;
    positions[2] = xpos - tw + w;
  }
  else
  {
    positions[0] = xpos;
    positions[2] = xpos + w;
  }
  positions[1] = ypos;
	positions[3] = ypos + h;*/

  /* these vars are used for clarity, the array for simpler function arguments  */
	double left = visibleRect.left();
	double top = visibleRect.top();
	double right = visibleRect.right();
	double bottom = visibleRect.bottom();

  /* left, top, right, bottom */
  paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) &&
                (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom())
               && (right >= viewRect.left()) && (left <= viewRect.right());
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    paintSides[2] = (viewRect.left() <= right ) &&
                    (right - 1 <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  else
    paintSides[2] = (viewRect.left() <= right ) &&
                    (right <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom())
               && (right >= viewRect.left()) && (left <= viewRect.right());

  positions[0] = QMAX( left,   viewRect.left() );
  positions[1] = QMAX( top,    viewRect.top() );
  positions[2] = QMIN( right,  viewRect.right() );
  positions[3] = QMIN( bottom, viewRect.bottom() );
}


/****************************************************************
 *
 * KSpreadVBorder
 *
 ****************************************************************/

KSpreadVBorder::KSpreadVBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view)
    : QWidget( _parent, "", /*WNorthWestGravity*/WStaticContents | WResizeNoErase | WRepaintNoErase )
{
  m_pView = _view;
  m_pCanvas = _canvas;
  m_lSize = 0L;

  setBackgroundMode( PaletteButton );
  setMouseTracking( true );
  m_bResize = false;
  m_bSelection = false;
  m_iSelectionAnchor=1;
  m_bMousePressed = false;

  m_scrollTimer = new QTimer( this );
  connect (m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
}


KSpreadVBorder::~KSpreadVBorder()
{
    delete m_scrollTimer;
}

QSize KSpreadVBorder::sizeHint() const
{
  return QSize( 40, 10 );
}


void KSpreadVBorder::mousePressEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  if ( _ev->button() == LeftButton )
    m_bMousePressed = true;

  const KSpreadSheet *sheet = m_pCanvas->activeSheet();
  assert( sheet );

  double ev_PosY = m_pCanvas->d->view->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();
  double dHeight = m_pCanvas->d->view->doc()->unzoomItY( height() );
  m_bResize = false;
  m_bSelection = false;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  // Find the first visible row and the y position of this row.
  double y;
  int row = sheet->topRow( m_pCanvas->yOffset(), y );

  // Did the user click between two rows?
  while ( y < ( dHeight + m_pCanvas->yOffset() ) && ( !m_bResize ) )
  {
    double h = sheet->rowFormat( row )->dblHeight();
    row++;
    if ( row > KS_rowMax )
      row = KS_rowMax;
    if ( ( ev_PosY >= y + h - 2 ) &&
         ( ev_PosY <= y + h + 1 ) &&
         !( sheet->rowFormat( row )->isHide() && row == 1 ) )
      m_bResize = true;
    y += h;
  }

  //if row is hide and it's the first row
  //you mustn't resize it.
  double tmp2;
  int tmpRow = sheet->topRow( ev_PosY - 1, tmp2 );
  if ( sheet->rowFormat( tmpRow )->isHide() && tmpRow == 1 )
      m_bResize = false;

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine row to resize
    double tmp;
    m_iResizedRow = sheet->topRow( ev_PosY - 1, tmp );
    if ( !sheet->isProtected() )
      paintSizeIndicator( _ev->pos().y(), true );
  }
  else
  {
    m_bSelection = true;

    double tmp;
    int hit_row = sheet->topRow( ev_PosY, tmp );
    if ( hit_row > KS_rowMax )
        return;

    m_iSelectionAnchor = hit_row;

    QRect rect = m_pView->selection();
    if ( !rect.contains( QPoint(1, hit_row) ) ||
        !( _ev->button() == RightButton ) ||
        ( !util_isRowSelected( rect ) ) )
    {
      QPoint newMarker( 1, hit_row );
      QPoint newAnchor( KS_colMax, hit_row );
      m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                              m_pView->activeSheet() );
    }

    if ( _ev->button() == RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupRowMenu( p );
      m_bSelection = false;
    }
    m_pView->updateEditWidget();
  }
}

void KSpreadVBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();

    m_bMousePressed = false;

    if ( !m_pView->koDocument()->isReadWrite() )
        return;

    KSpreadSheet *sheet = m_pCanvas->activeSheet();
    assert( sheet );

    double ev_PosY = m_pCanvas->d->view->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();

    if ( m_bResize )
    {
        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setRasterOp( NotROP );
        painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );
        painter.end();

        int start = m_iResizedRow;
        int end = m_iResizedRow;
        QRect rect;
        rect.setCoords( 1, m_iResizedRow, KS_colMax, m_iResizedRow );
        if ( util_isRowSelected( m_pView->selection() ) )
        {
            if ( m_pView->selection().contains( QPoint( 1, m_iResizedRow ) ) )
            {
                start = m_pView->selection().top();
                end = m_pView->selection().bottom();
                rect = m_pView->selection();
            }
        }

        double height = 0.0;
        double y = sheet->dblRowPos( m_iResizedRow );
        if ( ev_PosY - y <= 0.0 )
            height = 0.0;
        else
            height = ev_PosY - y;

        if ( !sheet->isProtected() )
        {
          if ( !m_pCanvas->d->view->doc()->undoLocked() )
          {
            //just resize
            if ( height != 0.0 )
            {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
                m_pCanvas->d->view->doc()->addCommand( undo );
            }
            else
            {
                //hide row
                KSpreadUndoHideRow *undo = new KSpreadUndoHideRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(),
                                                                   rect.top(), ( rect.bottom() - rect.top() ) );
                m_pCanvas->d->view->doc()->addCommand( undo );
            }
          }

          for( int i = start; i <= end; i++ )
          {
            RowFormat *rl = sheet->nonDefaultRowFormat( i );
            if ( height != 0.0 )
            {
              if ( !rl->isHide() )
                rl->setDblHeight( height );
            }
            else
              rl->setHide( true );
          }

          if ( height == 0.0 )
            sheet->emitHideColumn();

          delete m_lSize;
          m_lSize = 0;
        }
    }
    else if ( m_bSelection )
    {
        QRect rect = m_pView->selection();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kdDebug(36001) << "selected: T " << rect.top() << " B " << rect.bottom() << endl;

            int i;
            RowFormat * row;
            QValueList<int>hiddenRows;

            for ( i = rect.top(); i <= rect.bottom(); ++i )
            {
                row = m_pView->activeSheet()->rowFormat( i );
                if ( row->isHide() )
                {
                    hiddenRows.append(i);
                }
            }

            if ( hiddenRows.count() > 0 )
                m_pView->activeSheet()->showRow( 0, -1, hiddenRows );
        }
    }

    m_bSelection = false;
    m_bResize = false;
}

void KSpreadVBorder::adjustRow( int _row, bool makeUndo )
{
    double adjust = -1.0;
    int select;
    if ( _row == -1 )
    {
        adjust = m_pCanvas->activeSheet()->adjustRow( m_pView->selectionInfo() );
        select = m_iSelectionAnchor;
    }
    else
    {
        adjust = m_pCanvas->activeSheet()->adjustRow( m_pView->selectionInfo(), _row );
        select = _row;
    }

    if ( adjust != -1.0 )
    {
        KSpreadSheet * sheet = m_pCanvas->activeSheet();
        assert( sheet );
        if ( _row == -1 )
        {
            RowFormat * rl = sheet->nonDefaultRowFormat( select );

            if ( kAbs( rl->dblHeight() - adjust ) < DBL_EPSILON )
                return;
        }

        if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
        {
            QRect rect;
            rect.setCoords( 1, select, KS_colMax, select);
            KSpreadUndoResizeColRow * undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(),
                                                                          m_pCanvas->activeSheet(), rect );
            m_pCanvas->d->view->doc()->addCommand( undo );
        }
        RowFormat * rl = sheet->nonDefaultRowFormat( select );
        rl->setDblHeight( QMAX( 2.0, adjust ) );
    }
}

void KSpreadVBorder::equalizeRow( double resize )
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  Q_ASSERT( sheet );

  QRect selection( m_pView->selection() );
  if ( !m_pCanvas->d->view->doc()->undoLocked() )
  {
     KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), selection );
     m_pCanvas->d->view->doc()->addCommand( undo );
  }
  RowFormat *rl;
  for ( int i = selection.top(); i <= selection.bottom(); i++ )
  {
     rl = sheet->nonDefaultRowFormat( i );
     resize = QMAX( 2.0, resize);
     rl->setDblHeight( resize );
  }
}

void KSpreadVBorder::resizeRow( double resize, int nb, bool makeUndo )
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  Q_ASSERT( sheet );

  if ( nb == -1 ) // I don't know, where this is the case
  {
    if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
    {
        QRect rect;
        rect.setCoords( 1, m_iSelectionAnchor, KS_colMax, m_iSelectionAnchor );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
        m_pCanvas->d->view->doc()->addCommand( undo );
    }
    RowFormat *rl = sheet->nonDefaultRowFormat( m_iSelectionAnchor );
    rl->setDblHeight( QMAX( 2.0, resize ) );
  }
  else
  {
    QRect selection( m_pView->selection() );
    if ( m_pView->selectionInfo()->singleCellSelection() )
    {
      if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
      {
        QRect rect;
        rect.setCoords( 1, m_pCanvas->markerRow(), KS_colMax, m_pCanvas->markerRow() );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
        m_pCanvas->d->view->doc()->addCommand( undo );
      }
      RowFormat *rl = sheet->nonDefaultRowFormat( m_pCanvas->markerRow() );
      rl->setDblHeight( QMAX( 2.0, resize ) );
    }
    else
    {
      if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
      {
          KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), selection );
          m_pCanvas->d->view->doc()->addCommand( undo );
      }
      RowFormat *rl;
      for ( int i = selection.top(); i<=selection.bottom(); i++ )
      {
        rl = sheet->nonDefaultRowFormat( i );
        rl->setDblHeight( QMAX( 2.0, resize ) );
      }
    }
  }
}


void KSpreadVBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  assert( sheet );

  if ( !m_pView->koDocument()->isReadWrite() || sheet->isProtected() )
    return;

  adjustRow();
}


void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  assert( sheet );

  double ev_PosY = m_pCanvas->d->view->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();
  double dHeight = m_pCanvas->d->view->doc()->unzoomItY( height() );

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    if ( !sheet->isProtected() )
      paintSizeIndicator( _ev->pos().y(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double y;
    int row = sheet->topRow( ev_PosY, y );
    if ( row > KS_rowMax )
      return;

    QPoint newAnchor = m_pView->selectionInfo()->selectionAnchor();
    QPoint newMarker = m_pView->selectionInfo()->marker();
    newMarker.setY( row );
    newAnchor.setY( m_iSelectionAnchor );

    m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                            m_pView->activeSheet() );

    if ( _ev->pos().y() < 0 )
      m_pCanvas->vertScrollBar()->setValue( m_pCanvas->d->view->doc()->zoomItY( ev_PosY ) );
    else if ( _ev->pos().y() > m_pCanvas->height() )
    {
      if ( row < KS_rowMax )
      {
        RowFormat *rl = sheet->rowFormat( row + 1 );
        y = sheet->dblRowPos( row + 1 );
        m_pCanvas->vertScrollBar()->setValue ((int) (m_pCanvas->d->view->doc()->zoomItY
              (ev_PosY + rl->dblHeight()) - dHeight));
      }
    }
  }
  // No button is pressed and the mouse is just moved
  else
  {

     //What is the internal size of 1 pixel
    const double unzoomedPixel = m_pCanvas->d->view->doc()->unzoomItY( 1 );
    double y;
    int tmpRow = sheet->topRow( m_pCanvas->yOffset(), y );

    while ( y < m_pCanvas->d->view->doc()->unzoomItY( height() ) + m_pCanvas->yOffset() )
    {
      double h = sheet->rowFormat( tmpRow )->dblHeight();
      //if col is hide and it's the first column
      //you mustn't resize it.
      if ( ev_PosY >= y + h - 2 * unzoomedPixel &&
           ev_PosY <= y + h + unzoomedPixel &&
           !( sheet->rowFormat( tmpRow )->isHide() && tmpRow == 1 ) )
      {
        setCursor( splitVCursor );
        return;
      }
      y += h;
      tmpRow++;
    }
    setCursor( arrowCursor );
  }
}

void KSpreadVBorder::doAutoScroll()
{
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( pos.y() < 0 || pos.y() > height() )
    {
        QMouseEvent * event = new QMouseEvent( QEvent::MouseMove, pos, 0, 0 );
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
}

void KSpreadVBorder::wheelEvent( QWheelEvent* _ev )
{
  if ( m_pCanvas->vertScrollBar() )
    QApplication::sendEvent( m_pCanvas->vertScrollBar(), _ev );
}


void KSpreadVBorder::paintSizeIndicator( int mouseY, bool firstTime )
{
    KSpreadSheet *sheet = m_pCanvas->activeSheet();
    assert( sheet );

    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );

    if ( !firstTime )
      painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    m_iResizePos = mouseY;

    // Dont make the row have a height < 2 pixel.
    int y = m_pCanvas->d->view->doc()->zoomItY( sheet->dblRowPos( m_iResizedRow ) - m_pCanvas->yOffset() );
    if ( m_iResizePos < y + 2 )
        m_iResizePos = y;

    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    painter.end();

    QString tmpSize;
    if ( m_iResizePos != y )
        tmpSize = i18n("Height: %1 %2").arg( KoUnit::toUserValue( m_pCanvas->doc()->unzoomItY( m_iResizePos - y ),
                                                               m_pView->doc()->getUnit() ) )
                                       .arg( m_pView->doc()->getUnitName() );
    else
        tmpSize = i18n( "Hide Row" );

    painter.begin( this );
    int len = painter.fontMetrics().width( tmpSize );
    int hei = painter.fontMetrics().height();
    painter.end();

    if ( !m_lSize )
    {
          m_lSize = new QLabel( m_pCanvas );

          if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
            m_lSize->setGeometry( m_pCanvas->width() - len - 5,
                                                  y + 3, len + 2, hei + 2 );
          else
            m_lSize->setGeometry( 3, y + 3, len + 2,hei + 2 );

          m_lSize->setAlignment( Qt::AlignVCenter );
          m_lSize->setText( tmpSize );
          m_lSize->setPalette( QToolTip::palette() );
          m_lSize->show();
    }
    else
    {
          if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
            m_lSize->setGeometry( m_pCanvas->width() - len - 5,
                                                  y + 3, len + 2, hei + 2 );
          else
            m_lSize->setGeometry( 3, y + 3, len + 2,hei + 2 );

          m_lSize->setText( tmpSize );
    }
}

void KSpreadVBorder::updateRows( int from, int to )
{
    KSpreadSheet *sheet = m_pCanvas->activeSheet();
    if ( !sheet )
        return;

    int y0 = sheet->rowPos( from, m_pCanvas );
    int y1 = sheet->rowPos( to+1, m_pCanvas );
    update( 0, y0, width(), y1-y0 );
}

void KSpreadVBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  if ( !sheet )
    return;

  QPainter painter( this );
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );
  // painter.setBackgroundColor( colorGroup().base() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  painter.setClipRect( _ev->rect() );

  double yPos;
  //Get the top row and the current y-position
  int y = sheet->topRow( (m_pCanvas->d->view->doc()->unzoomItY( _ev->rect().y() ) + m_pCanvas->yOffset()), yPos );
  //Align to the offset
  yPos = yPos - m_pCanvas->yOffset();
  int width = m_pCanvas->d->view->doc()->zoomItX( YBORDER_WIDTH );

  QFont normalFont = painter.font();
  if ( m_pCanvas->d->view->doc()->zoom() < 100 )
  {
    normalFont.setPointSizeFloat( 0.01 * m_pCanvas->d->view->doc()->zoom() *
                                  normalFont.pointSizeFloat() );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( true );

  //several cells selected but not just a cell merged
  bool area = !( m_pView->selectionInfo()->singleCellSelection() );

  //Loop through the rows, until we are out of range
  while ( yPos <= m_pCanvas->d->view->doc()->unzoomItY( _ev->rect().bottom() ) )
  {
    bool highlighted = ( area && y >= m_pView->selection().top() &&
                         y <= m_pView->selection().bottom() );
    bool selected = ( highlighted && (util_isRowSelected(m_pView->selection())) );
    bool current  = ( !highlighted && y == m_pView->selection().top() );

    const RowFormat *row_lay = sheet->rowFormat( y );
    int zoomedYPos = m_pCanvas->d->view->doc()->zoomItY( yPos );
    int height = m_pCanvas->d->view->doc()->zoomItY( yPos + row_lay->dblHeight() ) - zoomedYPos;

    if ( selected || current )
    {
      QColor c = colorGroup().highlight().light();
      QBrush fillSelected( c );
      qDrawPlainRect ( &painter, 0, zoomedYPos, width, height+1, c.dark(150),
           1, &fillSelected );
    }
    else if ( highlighted )
    {
      QColor c = colorGroup().highlight().light();
      QBrush fillHighlighted( c );
      qDrawPlainRect ( &painter, 0, zoomedYPos, width, height+1, c.dark(150),
           1, &fillHighlighted );
    }
    else
    {
      QColor c = colorGroup().background();
      QBrush fill( c );
      qDrawPlainRect ( &painter, 0, zoomedYPos, width, height+1, c.dark(150),
           1, &fill );
    }

    QString rowText = QString::number( y );

    // Reset painter
    painter.setFont( normalFont );
    painter.setPen( colorGroup().text() );

    if ( selected )
      painter.setPen( colorGroup().highlightedText() );
    else if ( highlighted || current )
      painter.setFont( boldFont );

    int len = painter.fontMetrics().width( rowText );
    if (!row_lay->isHide())
        painter.drawText( ( width-len )/2, zoomedYPos +
                          ( height + painter.fontMetrics().ascent() -
                            painter.fontMetrics().descent() ) / 2, rowText );

    yPos += row_lay->dblHeight();
    y++;
  }
}


void KSpreadVBorder::focusOutEvent( QFocusEvent* )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
}


/****************************************************************
 *
 * KSpreadHBorder
 *
 ****************************************************************/

KSpreadHBorder::KSpreadHBorder( QWidget *_parent, KSpreadCanvas *_canvas,KSpreadView *_view )
    : QWidget( _parent, "", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase )
{
  m_pView = _view;
  m_pCanvas = _canvas;
  m_lSize = 0L;
  setBackgroundMode( PaletteButton );
  setMouseTracking( true );
  m_bResize = false;
  m_bSelection = false;
  m_iSelectionAnchor=1;
  m_bMousePressed = false;

  m_scrollTimer = new QTimer( this );
  connect ( m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
}


KSpreadHBorder::~KSpreadHBorder()
{
    delete m_scrollTimer;
}

QSize KSpreadHBorder::sizeHint() const
{
  return QSize( 40, 10 );
}

void KSpreadHBorder::mousePressEvent( QMouseEvent * _ev )
{
  if (!m_pView->koDocument()->isReadWrite())
    return;

  if ( _ev->button() == LeftButton )
    m_bMousePressed = true;

  const KSpreadSheet *sheet = m_pCanvas->activeSheet();
  assert( sheet );

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
      m_pCanvas->deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  double ev_PosX;
  double dWidth = m_pCanvas->d->view->doc()->unzoomItX( width() );
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    ev_PosX = dWidth - m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  else
    ev_PosX = m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  m_bResize = false;
  m_bSelection = false;

  // Find the first visible column and the x position of this column.
  double x;

  const double unzoomedPixel = m_pCanvas->d->view->doc()->unzoomItX( 1 );
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    int tmpCol = sheet->leftColumn( m_pCanvas->xOffset(), x );

    kdDebug() << "evPos: " << ev_PosX << ", x: " << x << ", COL: " << tmpCol << endl;
    while ( ev_PosX > x && ( !m_bResize ) )
    {
      double w = sheet->columnFormat( tmpCol )->dblWidth();

      kdDebug() << "evPos: " << ev_PosX << ", x: " << x << ", w: " << w << ", COL: " << tmpCol << endl;

      ++tmpCol;
      if ( tmpCol > KS_colMax )
        tmpCol = KS_colMax;
      //if col is hide and it's the first column
      //you mustn't resize it.

      if ( ev_PosX >= x + w - unzoomedPixel &&
           ev_PosX <= x + w + unzoomedPixel &&
           !( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 1 ) )
      {
        m_bResize = true;
      }
      x += w;
    }

    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
    tmpCol = sheet->leftColumn( dWidth - ev_PosX + 1, tmp2 );
    if ( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 0 )
    {
      kdDebug() << "No resize: " << tmpCol << ", " << sheet->columnFormat( tmpCol )->isHide() << endl;
      m_bResize = false;
    }

    kdDebug() << "Resize: " << m_bResize << endl;
  }
  else
  {
    int col = sheet->leftColumn( m_pCanvas->xOffset(), x );

    // Did the user click between two columns?
    while ( x < ( dWidth + m_pCanvas->xOffset() ) && ( !m_bResize ) )
    {
      double w = sheet->columnFormat( col )->dblWidth();
      col++;
      if ( col > KS_colMax )
        col = KS_colMax;
      if ( ( ev_PosX >= x + w - unzoomedPixel ) &&
         ( ev_PosX <= x + w + unzoomedPixel ) &&
           !( sheet->columnFormat( col )->isHide() && col == 1 ) )
        m_bResize = true;
      x += w;
    }

    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
    int tmpCol = sheet->leftColumn( ev_PosX - 1, tmp2 );
    if ( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 1 )
      m_bResize = false;
  }

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine the column to resize
    double tmp;
    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      m_iResizedColumn = sheet->leftColumn( ev_PosX - 1, tmp );
      // kdDebug() << "RColumn: " << m_iResizedColumn << ", PosX: " << ev_PosX << endl;

      if ( !sheet->isProtected() )
        paintSizeIndicator( _ev->pos().x(), true );
    }
    else
    {
      m_iResizedColumn = sheet->leftColumn( ev_PosX - 1, tmp );

      if ( !sheet->isProtected() )
        paintSizeIndicator( _ev->pos().x(), true );
    }

    // kdDebug() << "Column: " << m_iResizedColumn << endl;
  }
  else
  {
    m_bSelection = true;

    double tmp;
    int hit_col = sheet->leftColumn( ev_PosX, tmp );
    if ( hit_col > KS_colMax )
        return;

    m_iSelectionAnchor = hit_col;

    QRect rect = m_pView->selection();
    if ( !rect.contains( QPoint( hit_col, 1 ) ) ||
        !( _ev->button() == RightButton ) ||
        ( !util_isColumnSelected( rect ) ) )
    {
      QPoint newMarker( hit_col, 1 );
      QPoint newAnchor( hit_col, KS_rowMax );
      m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                              m_pView->activeSheet() );
    }

    if ( _ev->button() == RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupColumnMenu( p );
      m_bSelection = false;
    }
    m_pView->updateEditWidget();
  }
}

void KSpreadHBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();

    m_bMousePressed = false;

    if ( !m_pView->koDocument()->isReadWrite() )
      return;

    KSpreadSheet * sheet = m_pCanvas->activeSheet();
    assert( sheet );

    if ( m_bResize )
    {
        double dWidth = m_pCanvas->d->view->doc()->unzoomItX( width() );
        double ev_PosX;

        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setRasterOp( NotROP );
        painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
        painter.end();

        int start = m_iResizedColumn;
        int end   = m_iResizedColumn;
        QRect rect;
        rect.setCoords( m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax );
        if ( util_isColumnSelected(m_pView->selection()) )
        {
            if ( m_pView->selection().contains( QPoint( m_iResizedColumn, 1 ) ) )
            {
                start = m_pView->selection().left();
                end   = m_pView->selection().right();
                rect  = m_pView->selection();
            }
        }

        double width = 0.0;
        double x;

        if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
          ev_PosX = dWidth - m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
        else
          ev_PosX = m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();

        x = sheet->dblColumnPos( m_iResizedColumn );

        if ( ev_PosX - x <= 0.0 )
          width = 0.0;
        else
          width = ev_PosX - x;

        if ( !sheet->isProtected() )
        {
          if ( !m_pCanvas->d->view->doc()->undoLocked() )
          {
            //just resize
            if ( width != 0.0 )
            {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
                m_pCanvas->d->view->doc()->addCommand( undo );
            }
            else
            {//hide column
                KSpreadUndoHideColumn *undo = new KSpreadUndoHideColumn( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect.left(), (rect.right()-rect.left()));
                m_pCanvas->d->view->doc()->addCommand( undo );
            }
          }

          for( int i = start; i <= end; i++ )
          {
            ColumnFormat *cl = sheet->nonDefaultColumnFormat( i );
            if ( width != 0.0 )
            {
                if ( !cl->isHide() )
                    cl->setDblWidth( width );
            }
            else
                cl->setHide( true );
          }

          if ( width == 0.0 )
            sheet->emitHideRow();

          delete m_lSize;
          m_lSize = 0;
        }
    }
    else if ( m_bSelection )
    {
        QRect rect = m_pView->selection();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kdDebug(36001) << "selected: L " << rect.left() << " R " << rect.right() << endl;

            int i;
            ColumnFormat * col;
            QValueList<int>hiddenCols;

            for ( i = rect.left(); i <= rect.right(); ++i )
            {
                col = m_pView->activeSheet()->columnFormat( i );
                if ( col->isHide() )
                {
                    hiddenCols.append(i);
                }
            }

            if ( hiddenCols.count() > 0 )
                m_pView->activeSheet()->showColumn( 0, -1, hiddenCols );
        }
    }

    m_bSelection = false;
    m_bResize = false;
}

void KSpreadHBorder::adjustColumn( int _col, bool makeUndo )
{
  double adjust = -1.0;
  int select;

  if ( _col == -1 )
  {
    adjust = m_pCanvas->activeSheet()->adjustColumn( m_pView->selectionInfo() );
    select = m_iSelectionAnchor;
  }
  else
  {
    adjust = m_pCanvas->activeSheet()->adjustColumn( m_pView->selectionInfo(), _col );
    select = _col;
  }

  if ( adjust != -1.0 )
  {
    KSpreadSheet * sheet = m_pCanvas->activeSheet();
    assert( sheet );

    if ( _col == -1 )
    {
        ColumnFormat * cl = sheet->nonDefaultColumnFormat( select );
        if ( kAbs( cl->dblWidth() - adjust ) < DBL_EPSILON )
            return;

    }
    if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
    {
        QRect rect;
        rect.setCoords( select, 1, select, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(),
                                                                     m_pCanvas->activeSheet(), rect );
        m_pCanvas->d->view->doc()->addCommand( undo );
    }

    ColumnFormat * cl = sheet->nonDefaultColumnFormat( select );
    cl->setDblWidth( QMAX( 2.0, adjust ) );
  }
}

void KSpreadHBorder::equalizeColumn( double resize )
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  Q_ASSERT( sheet );

  QRect selection( m_pView->selection() );
  if ( !m_pCanvas->d->view->doc()->undoLocked() )
  {
      KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), selection );
      m_pCanvas->d->view->doc()->addCommand( undo );
  }
  ColumnFormat *cl;
  for ( int i = selection.left(); i <= selection.right(); i++ )
  {
      cl = sheet->nonDefaultColumnFormat( i );
      resize = QMAX( 2.0, resize );
      cl->setDblWidth( resize );
  }

}

void KSpreadHBorder::resizeColumn( double resize, int nb, bool makeUndo )
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  Q_ASSERT( sheet );

  if ( nb == -1 )
  {
    if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
    {
        QRect rect;
        rect.setCoords( m_iSelectionAnchor, 1, m_iSelectionAnchor, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
        m_pCanvas->d->view->doc()->addCommand( undo );
    }
    ColumnFormat *cl = sheet->nonDefaultColumnFormat( m_iSelectionAnchor );
    cl->setDblWidth( QMAX( 2.0, resize ) );
  }
  else
  {
    QRect selection( m_pView->selection() );
    if ( m_pView->selectionInfo()->singleCellSelection() )
    {
      if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
      {
        QRect rect;
        rect.setCoords( m_iSelectionAnchor, 1, m_iSelectionAnchor, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), rect );
        m_pCanvas->d->view->doc()->addCommand( undo );
      }

      ColumnFormat *cl = sheet->nonDefaultColumnFormat( m_pCanvas->markerColumn() );
      cl->setDblWidth( QMAX( 2.0, resize ) );
    }
    else
    {
      if ( makeUndo && !m_pCanvas->d->view->doc()->undoLocked() )
      {
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->d->view->doc(), m_pCanvas->activeSheet(), selection );
        m_pCanvas->d->view->doc()->addCommand( undo );
      }
      ColumnFormat *cl;
      for ( int i = selection.left(); i <= selection.right(); i++ )
      {
        cl = sheet->nonDefaultColumnFormat( i );
        cl->setDblWidth( QMAX( 2.0, resize ) );
      }
    }
  }
}

void KSpreadHBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  assert( sheet );

  if ( !m_pView->koDocument()->isReadWrite() || sheet->isProtected() )
    return;

  adjustColumn();
}

void KSpreadHBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  KSpreadSheet *sheet = m_pCanvas->activeSheet();
  assert( sheet );

  double dWidth = m_pCanvas->d->view->doc()->unzoomItX( width() );
  double ev_PosX;
  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    ev_PosX = dWidth - m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  else
    ev_PosX = m_pCanvas->d->view->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    if ( !sheet->isProtected() )
        paintSizeIndicator( _ev->pos().x(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double x;
    int col = sheet->leftColumn( ev_PosX, x );

    if ( col > KS_colMax )
      return;

    QPoint newMarker = m_pView->selectionInfo()->marker();
    QPoint newAnchor = m_pView->selectionInfo()->selectionAnchor();

    newMarker.setX( col );
    newAnchor.setX( m_iSelectionAnchor );

    m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                            m_pView->activeSheet() );

    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      if ( _ev->pos().x() < width() - m_pCanvas->width() )
      {
        ColumnFormat *cl = sheet->columnFormat( col + 1 );
        x = sheet->dblColumnPos( col + 1 );
        m_pCanvas->horzScrollBar()->setValue ( m_pCanvas->horzScrollBar()->maxValue() - (int)
            (m_pCanvas->d->view->doc()->zoomItX (ev_PosX + cl->dblWidth()) - m_pCanvas->d->view->doc()->unzoomItX( m_pCanvas->width() )));
      }
      else if ( _ev->pos().x() > width() )
        m_pCanvas->horzScrollBar()->setValue( m_pCanvas->horzScrollBar()->maxValue() - m_pCanvas->d->view->doc()->zoomItX( ev_PosX - dWidth + m_pCanvas->d->view->doc()->unzoomItX( m_pCanvas->width() ) ) );
    }
    else
    {
      if ( _ev->pos().x() < 0 )
        m_pCanvas->horzScrollBar()->setValue( m_pCanvas->d->view->doc()->zoomItX( ev_PosX ) );
      else if ( _ev->pos().x() > m_pCanvas->width() )
      {
        if ( col < KS_colMax )
        {
          ColumnFormat *cl = sheet->columnFormat( col + 1 );
          x = sheet->dblColumnPos( col + 1 );
          m_pCanvas->horzScrollBar()->setValue ((int)
              (m_pCanvas->d->view->doc()->zoomItX (ev_PosX + cl->dblWidth()) - dWidth));
        }
      }
    }

  }
  // No button is pressed and the mouse is just moved
  else
  {
     //What is the internal size of 1 pixel
    const double unzoomedPixel = m_pCanvas->d->view->doc()->unzoomItX( 1 );
    double x;

    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      int tmpCol = sheet->leftColumn( m_pCanvas->xOffset(), x );

      while ( ev_PosX > x )
      {
        double w = sheet->columnFormat( tmpCol )->dblWidth();
        ++tmpCol;

        //if col is hide and it's the first column
        //you mustn't resize it.
        if ( ev_PosX >= x + w - unzoomedPixel &&
             ev_PosX <= x + w + unzoomedPixel &&
             !( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 0 ) )
        {
          setCursor( splitHCursor );
          return;
        }
        x += w;
      }
      setCursor( arrowCursor );
    }
    else
    {
      int tmpCol = sheet->leftColumn( m_pCanvas->xOffset(), x );

      while ( x < m_pCanvas->d->view->doc()->unzoomItY( width() ) + m_pCanvas->xOffset() )
      {
        double w = sheet->columnFormat( tmpCol )->dblWidth();
        //if col is hide and it's the first column
        //you mustn't resize it.
        if ( ev_PosX >= x + w - unzoomedPixel &&
             ev_PosX <= x + w + unzoomedPixel &&
             !( sheet->columnFormat( tmpCol )->isHide() && tmpCol == 1 ) )
        {
          setCursor( splitHCursor );
          return;
        }
        x += w;
        tmpCol++;
      }
      setCursor( arrowCursor );
    }
  }
}

void KSpreadHBorder::doAutoScroll()
{
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( pos.x() < 0 || pos.x() > width() )
    {
        QMouseEvent * event = new QMouseEvent( QEvent::MouseMove, pos, 0, 0 );
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
}

void KSpreadHBorder::wheelEvent( QWheelEvent* _ev )
{
  if ( m_pCanvas->horzScrollBar() )
    QApplication::sendEvent( m_pCanvas->horzScrollBar(), _ev );
}

void KSpreadHBorder::resizeEvent( QResizeEvent* _ev )
{
  // workaround to allow horizontal resizing and zoom changing when sheet
  // direction and interface direction don't match (e.g. an RTL sheet on an
  // LTR interface)
  if ( m_pCanvas->activeSheet() && m_pCanvas->activeSheet()->layoutDirection()==KSpreadSheet::RightToLeft && !QApplication::reverseLayout() )
  {
    int dx = _ev->size().width() - _ev->oldSize().width();
    scroll(dx, 0);
  }
  else if ( m_pCanvas->activeSheet() && m_pCanvas->activeSheet()->layoutDirection()==KSpreadSheet::LeftToRight && QApplication::reverseLayout() )
  {
    int dx = _ev->size().width() - _ev->oldSize().width();
    scroll(-dx, 0);
  }
}

void KSpreadHBorder::paintSizeIndicator( int mouseX, bool firstTime )
{
    KSpreadSheet *sheet = m_pCanvas->activeSheet();
    assert( sheet );

    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );

    if ( !firstTime )
      painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
      m_iResizePos = mouseX + m_pCanvas->width() - width();
    else
      m_iResizePos = mouseX;

    // Dont make the column have a width < 2 pixels.
    int x = m_pCanvas->d->view->doc()->zoomItX( sheet->dblColumnPos( m_iResizedColumn ) - m_pCanvas->xOffset() );

    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      x = m_pCanvas->width() - x;

      if ( m_iResizePos > x - 2 )
          m_iResizePos = x;
    }
    else
    {
      if ( m_iResizePos < x + 2 )
          m_iResizePos = x;
    }

    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    painter.end();

    QString tmpSize;
    if ( m_iResizePos != x )
        tmpSize = i18n("Width: %1 %2")
                  .arg( KGlobal::locale()->formatNumber( KoUnit::toUserValue( m_pCanvas->doc()->unzoomItX( (sheet->layoutDirection()==KSpreadSheet::RightToLeft) ? x - m_iResizePos : m_iResizePos - x ),
                                                                           m_pView->doc()->getUnit() )))
                  .arg( m_pView->doc()->getUnitName() );
    else
        tmpSize = i18n( "Hide Column" );

    painter.begin( this );
    int len = painter.fontMetrics().width( tmpSize );
    int hei = painter.fontMetrics().height();
    painter.end();

    if ( !m_lSize )
    {
        m_lSize = new QLabel( m_pCanvas );

        if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
          m_lSize->setGeometry( x - len - 5, 3, len + 2, hei + 2 );
        else
          m_lSize->setGeometry( x + 3, 3, len + 2, hei + 2 );

        m_lSize->setAlignment( Qt::AlignVCenter );
        m_lSize->setText( tmpSize );
        m_lSize->setPalette( QToolTip::palette() );
        m_lSize->show();
    }
    else
    {
        if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
          m_lSize->setGeometry( x - len - 5, 3, len + 2, hei + 2 );
        else
          m_lSize->setGeometry( x + 3, 3, len + 2, hei + 2 );

        m_lSize->setText( tmpSize );
    }
}

void KSpreadHBorder::updateColumns( int from, int to )
{
    KSpreadSheet *sheet = m_pCanvas->activeSheet();
    if ( !sheet )
        return;

    int x0 = sheet->columnPos( from, m_pCanvas );
    int x1 = sheet->columnPos( to+1, m_pCanvas );
    update( x0, 0, x1-x0, height() );
}

void KSpreadHBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadSheet * sheet = m_pCanvas->activeSheet();
  if ( !sheet )
    return;

  QPainter painter( this );
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  painter.setClipRect( _ev->rect() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  double xPos;
  int x;

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    //Get the left column and the current x-position
    x = sheet->leftColumn( int( m_pCanvas->d->view->doc()->unzoomItX( width() ) - m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().x() ) + m_pCanvas->xOffset() ), xPos );
    //Align to the offset
    xPos = m_pCanvas->d->view->doc()->unzoomItX( width() ) - xPos + m_pCanvas->xOffset();
  }
  else
  {
    //Get the left column and the current x-position
    x = sheet->leftColumn( int( m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().x() ) + m_pCanvas->xOffset() ), xPos );
    //Align to the offset
    xPos = xPos - m_pCanvas->xOffset();
  }

  int height = m_pCanvas->d->view->doc()->zoomItY( KSpreadFormat::globalRowHeight() + 2 );

  QFont normalFont = painter.font();
  if ( m_pCanvas->d->view->doc()->zoom() < 100 )
  {
    normalFont.setPointSizeFloat( 0.01 * m_pCanvas->d->view->doc()->zoom() *
                                  normalFont.pointSizeFloat() );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( true );

  KSpreadCell *cell = sheet->cellAt( m_pView->marker() );
  QRect extraCell;
  extraCell.setCoords( m_pCanvas->markerColumn(),
                       m_pCanvas->markerRow(),
                       m_pCanvas->markerColumn() + cell->extraXCells(),
                       m_pCanvas->markerRow() + cell->extraYCells());

  //several cells selected but not just a cell merged
  bool area = ( m_pView->selection().left()!=0 && extraCell != m_pView->selection() );

  if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    if ( x > KS_colMax )
      x = KS_colMax;

    xPos -= sheet->columnFormat( x )->dblWidth();

    //Loop through the columns, until we are out of range
    while ( xPos <= m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().right() ) )
    {
      bool highlighted = ( area && x >= m_pView->selection().left() && x <= m_pView->selection().right());
      bool selected = ( highlighted && util_isColumnSelected( m_pView->selection() ) &&
                        ( !util_isRowSelected( m_pView->selection() ) ) );
      bool current = ( !highlighted && x == m_pView->selection().left() );

      const ColumnFormat * col_lay = sheet->columnFormat( x );
      int zoomedXPos = m_pCanvas->d->view->doc()->zoomItX( xPos );
      int width = m_pCanvas->d->view->doc()->zoomItX( xPos + col_lay->dblWidth() ) - zoomedXPos;

      if ( selected || current )
      {
        QColor c = colorGroup().highlight().light();
        QBrush fillSelected( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(150),
           1, &fillSelected );
      }
      else if ( highlighted )
      {
        QColor c = colorGroup().highlight().light();
        QBrush fillHighlighted( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(150),
           1, &fillHighlighted );
      }
      else
      {
        QColor c = colorGroup().background();
        QBrush fill( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(150),
           1, &fill );
      }

      // Reset painter
      painter.setFont( normalFont );
      painter.setPen( colorGroup().text() );

      if ( selected )
        painter.setPen( colorGroup().highlightedText() );
      else if ( highlighted || current )
        painter.setFont( boldFont );
      if ( !m_pView->activeSheet()->getShowColumnNumber() )
      {
        QString colText = KSpreadCell::columnName( x );
        int len = painter.fontMetrics().width( colText );
        if ( !col_lay->isHide() )
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2, colText );
      }
      else
      {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2,
                            tmp.setNum(x) );
      }
      xPos += col_lay->dblWidth();
      --x;
    }
  }
  else
  {
    //Loop through the columns, until we are out of range
    while ( xPos <= m_pCanvas->d->view->doc()->unzoomItX( _ev->rect().right() ) )
    {
      bool highlighted = ( area && x >= m_pView->selection().left() && x <= m_pView->selection().right());
      bool selected = ( highlighted && util_isColumnSelected( m_pView->selection() ) &&
                        ( !util_isRowSelected( m_pView->selection() ) ) );
      bool current = ( !highlighted && x == m_pView->selection().left() );

      const ColumnFormat *col_lay = sheet->columnFormat( x );
      int zoomedXPos = m_pCanvas->d->view->doc()->zoomItX( xPos );
      int width = m_pCanvas->d->view->doc()->zoomItX( xPos + col_lay->dblWidth() ) - zoomedXPos;

      if ( selected || current )
      {
        QColor c = colorGroup().highlight().light();
        QBrush fillSelected( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(),
           1, &fillSelected );
      }
      else if ( highlighted )
      {
        QColor c = colorGroup().highlight().light();
        QBrush fillHighlighted( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(),
           1, &fillHighlighted );
      }
      else
      {
        QColor c = colorGroup().background();
        QBrush fill( c );
        qDrawPlainRect ( &painter, zoomedXPos, 0, width+1, height, c.dark(150),
           1, &fill );
      }

      // Reset painter
      painter.setFont( normalFont );
      painter.setPen( colorGroup().text() );

      if ( selected )
        painter.setPen( colorGroup().highlightedText() );
      else if ( highlighted || current )
        painter.setFont( boldFont );
      if ( !m_pView->activeSheet()->getShowColumnNumber() )
      {
        QString colText = KSpreadCell::columnName( x );
        int len = painter.fontMetrics().width( colText );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2, colText );
      }
      else
      {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2,
                            tmp.setNum(x) );
      }
      xPos += col_lay->dblWidth();
      ++x;
    }
  }
}


void KSpreadHBorder::focusOutEvent( QFocusEvent* )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
}

/****************************************************************
 *
 * KSpreadToolTip
 *
 ****************************************************************/

KSpreadToolTip::KSpreadToolTip( KSpreadCanvas* canvas )
    : QToolTip( canvas ), m_canvas( canvas )
{
}

void KSpreadToolTip::maybeTip( const QPoint& p )
{
    KSpreadSheet *sheet = m_canvas->activeSheet();
    if ( !sheet )
        return;

    // Over which cell is the mouse ?
    double ypos, xpos;
    double dwidth = m_canvas->doc()->unzoomItX( m_canvas->width() );
    int col;
    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
      col = sheet->leftColumn( (dwidth - m_canvas->doc()->unzoomItX( p.x() ) +
                                              m_canvas->xOffset()), xpos );
    else
      col = sheet->leftColumn( (m_canvas->doc()->unzoomItX( p.x() ) +
                                     m_canvas->xOffset()), xpos );


    int row = sheet->topRow( (m_canvas->doc()->unzoomItY( p.y() ) +
                                   m_canvas->yOffset()), ypos );

    KSpreadCell* cell = sheet->visibleCellAt( col, row );
    if ( !cell )
        return;

    // Quick cut
    if( cell->strOutText().isEmpty() )
        return;

    // displayed tool tip, which has the following priorities:
    //  - cell content if the cell dimension is too small
    //  - cell comment
    //  - hyperlink
    QString tipText;

    // If cell is too small, show the content
    if ( cell->testFlag( KSpreadCell::Flag_CellTooShortX ) ||
         cell->testFlag( KSpreadCell::Flag_CellTooShortY ) )
    {
        tipText = cell->strOutText();

        //Add 2 extra lines and a text, when both should be in the tooltip
        QString comment = cell->comment( col, row );
        if ( !comment.isEmpty() )
            comment = "\n\n" + i18n("Comment:") + "\n" + comment;

        tipText += comment;
    }

    // Show comment, if any
    if( tipText.isEmpty() )
    {
        tipText = cell->comment( col, row );
    }

    // Show hyperlink, if any
    if( tipText.isEmpty() )
    {
        tipText = cell->link();
    }

    // Nothing to display, bail out
    if( tipText.isEmpty() )
      return;

    // Determine position and width of the current cell.
    cell = sheet->cellAt( col, row );
    double u = cell->dblWidth( col );
    double v = cell->dblHeight( row );

    // Special treatment for obscured cells.
    if ( cell->isObscured() && cell->isObscuringForced() )
    {
      cell = cell->obscuringCells().first();
      int moveX = cell->column();
      int moveY = cell->row();

      // Use the obscuring cells dimensions
      u = cell->dblWidth( moveX );
      v = cell->dblHeight( moveY );
      xpos = sheet->dblColumnPos( moveX );
      ypos = sheet->dblRowPos( moveY );
    }

    // Get the cell dimensions
    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      KoRect unzoomedMarker( dwidth - u - xpos + m_canvas->xOffset(),
                             ypos - m_canvas->yOffset(),
                             u,
                             v );

      QRect marker( m_canvas->doc()->zoomRect( unzoomedMarker ) );
      if ( marker.contains( p ) )
      {
          tip( marker, tipText );
      }
    }
    else
    {
      KoRect unzoomedMarker( xpos - m_canvas->xOffset(),
                             ypos - m_canvas->yOffset(),
                             u,
                             v );

      QRect marker( m_canvas->doc()->zoomRect( unzoomedMarker ) );
      if ( marker.contains( p ) )
      {
          tip( marker, tipText );
      }
    }
}

void KSpreadCanvas::setHighlightedRanges(std::vector<HighlightRange>* cells)
{
	//Clear existing highlighted ranges
		
	if (d->highlightedRanges)
	{
		std::vector<HighlightRange>::iterator iter;
		
		for (iter=d->highlightedRanges->begin();iter != d->highlightedRanges->end();iter++)
		{
			if (iter->lastCell)
			{
				KSpreadRange rg(*(iter->firstCell),*(iter->lastCell));
			
				
				rg.sheet->setRegionPaintDirty(rg.range);
			}
			else
			{
				QRect cellRect(iter->firstCell->column(),iter->firstCell->row(),
				       1,1);
			
				iter->firstCell->sheet->setRegionPaintDirty(cellRect);
			}
		}
					
		delete d->highlightedRanges;
	}
		
	if (cells==0)
		d->highlightedRanges=0;
	else
	{
		//Set the highlight flag for the specified cells, set the highlight color and add them to the dirty list for 
		//later repainting
		d->highlightedRanges=new std::vector<HighlightRange>(*cells);
		
		/*std::vector<HighlightRange>::iterator iter;
		
		for (iter=d->highlightedRanges->begin();iter != d->highlightedRanges->end();iter++)
		{
			if (iter->lastCell)
			{
				
				KSpreadRange rg(*(iter->firstCell),*(iter->lastCell));
				/*QRect range=rg.range;
				range.setWidth(range.width()+1);
				range.setHeight(range.height()+1);*/
				/*rg.sheet->setRegionPaintDirty(rg.range);
			}
			else
			{
				QRect cellRect(iter->firstCell->column(),iter->firstCell->row(),
				       1,1);
			
				iter->firstCell->sheet->setRegionPaintDirty(cellRect);
			}
			//iter->cell->setFlag(KSpreadCell::Flag_DisplayDirty);
		}
		
				*/	
	}
	
	//activeSheet()->setRegionPaintDirty(visibleCells());
	
	paintUpdates();
	
	
	
	
}



#include "kspread_canvas.moc"
