#include "kspread_canvas.h"
#include "kspread_table.h"
#include "kspread_cell.h"
#include "kspread_util.h"
#include "kspread_editors.h"

// Hack
#include "kspread_view.h"
#include "kspread_doc.h"

#include <klocale.h>
#include <kcursor.h>

#include <qlabel.h>
#include <qpainter.h>

/****************************************************************
 *
 * KSpreadEditWidget
 *
 ****************************************************************/

KSpreadEditWidget::KSpreadEditWidget( QWidget *_parent, KSpreadView *_view ) : QLineEdit( _parent, "KSpreadEditWidget" )
{
  m_pView = _view;
}

void KSpreadEditWidget::publicKeyPressEvent ( QKeyEvent* _ev )
{
  // QLineEdit::keyPressEvent( _ev );
  keyPressEvent( _ev );
}

void KSpreadEditWidget::slotAbortEdit()
{
  m_pView->canvasWidget()->setFocus();
  setText( "" );
}

void KSpreadEditWidget::slotDoneEdit()
{
  if ( !m_pView->activeTable() )
    return;

  m_pView->canvasWidget()->setFocus();
  m_pView->setText( text() );
  setText( "" );
}

void KSpreadEditWidget::keyPressEvent ( QKeyEvent* _ev )
{
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
  {
    QLineEdit::keyPressEvent( _ev );
    return;
  }

  switch ( _ev->key() )
  {
    case Key_Down:
    case Key_Up:
    case Key_Return:
    case Key_Enter:

	if ( m_pView->activeTable() != 0L )
	{
	    m_pView->canvasWidget()->setFocus();
	    m_pView->setText( text() );
	    setText( "" );
	    QApplication::sendEvent( m_pView->canvasWidget(), _ev );
	}
	else
	    _ev->accept();
	
	break;
	
    default:
	QLineEdit::keyPressEvent( _ev );
    }
}

/****************************************************************
 *
 * KSpreadCanvas
 *
 ****************************************************************/

KSpreadCanvas::KSpreadCanvas( QWidget *_parent, KSpreadView *_view, KSpreadDoc* _doc ) : QWidget( _parent )
{
  m_pHorzScrollBar = 0;
  m_pVertScrollBar = 0;
  m_pHBorderWidget = 0;
  m_pVBorderWidget = 0;
  m_pEditWidget = 0;
  m_pEditor = 0;
  m_pPosWidget = 0;

  m_iMarkerColumn = 1;
  m_iMarkerRow = 1;
  m_iMarkerVisible = 1;

  QWidget::setFocusPolicy( QWidget::StrongFocus );

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  m_iXOffset = 0;
  m_iYOffset = 0;
  m_fZoom = 1.0;
  m_pView = _view;
  m_pDoc = _doc;
  m_eAction = DefaultAction;
  m_eMouseAction = NoAction;
  m_bGeometryStarted = false;
  m_bEditDirtyFlag = false;

  setBackgroundColor( white );
  setMouseTracking( TRUE );

  setBackgroundMode( NoBackground );
}


void KSpreadCanvas::init()
{
  m_pHorzScrollBar = m_pView->horzScrollBar();
  m_pVertScrollBar = m_pView->vertScrollBar();
  m_pHBorderWidget = m_pView->hBorderWidget();
  m_pVBorderWidget = m_pView->vBorderWidget();
  m_pEditWidget = m_pView->editWidget();
  m_pPosWidget = m_pView->posWidget();
}

KSpreadTable* KSpreadCanvas::findTable( const QString& _name )
{
  KSpreadTable *t;

  for ( t = m_pDoc->map()->firstTable(); t != 0L; t = m_pDoc->map()->nextTable() )
  {
    if ( t->name() == _name )
      return t;
  }

  return 0L;
}

KSpreadTable* KSpreadCanvas::activeTable()
{
  return m_pView->activeTable();
}

void KSpreadCanvas::gotoLocation( const KSpreadPoint& _cell )
{
  if ( !_cell.isValid() )
  {
    QMessageBox::critical( this, i18n("KSpread Error"), i18n("Invalid cell reference"), i18n("Ok") );
    return;
  }

  KSpreadTable* table = activeTable();
  if ( _cell.isTableKnown() )
    table = _cell.table;
  if ( !table )
  {
    QMessageBox::critical( this, i18n("KSpread Error"),
			   i18n("Unknown table name %1").arg( _cell.tableName ), i18n("Ok" ) );
    return;
  }

  m_pView->setActiveTable( table );

  int xpos = table->columnPos( _cell.pos.x(), this );
  int ypos = table->rowPos( _cell.pos.y(), this );

  if ( xpos < 0 || xpos > width() - 100 * zoom() )
  {
    slotScrollHorz( xOffset() + xpos + width()/2 );
  }

  if ( ypos < 0 || ypos > height() - 50 * zoom() )
  {
    slotScrollVert( yOffset() + ypos + height()/2 );
  }

  setMarkerColumn( _cell.pos.x() );
  setMarkerRow( _cell.pos.y() );
}

void KSpreadCanvas::slotScrollHorz( int _value )
{
  if ( activeTable() == 0L )
    return;

  if ( _value < 0 )
    _value = 0;

  activeTable()->enableScrollBarUpdates( false );

  hideMarker();

  int dx = xOffset() - _value;
  m_iXOffset = _value;
  scroll( dx, 0 );
  m_pHBorderWidget->scroll( dx, 0 );

  showMarker();

  activeTable()->enableScrollBarUpdates( true );
}

void KSpreadCanvas::slotScrollVert( int _value )
{
  if ( activeTable() == 0L )
    return;

  if ( _value < 0 )
    _value = 0;

  activeTable()->enableScrollBarUpdates( false );

  hideMarker();

  int dy = yOffset() - _value;
  m_iYOffset = _value;
  scroll( 0, dy );
  m_pVBorderWidget->scroll( 0, dy );

  showMarker();

  activeTable()->enableScrollBarUpdates( true );
}

void KSpreadCanvas::slotMaxColumn( int _max_column )
{
  int xpos = activeTable()->columnPos( _max_column + 10, this );

  m_pHorzScrollBar->setRange( 0, xpos + xOffset() );
}

void KSpreadCanvas::slotMaxRow( int _max_row )
{
  int ypos = activeTable()->rowPos( _max_row + 10, this );

  m_pVertScrollBar->setRange( 0, ypos + yOffset() );
}

void KSpreadCanvas::setAction( Actions _act )
{
  QRect selection( activeTable()->selectionRect() );

  if ( _act == InsertChart )
  {
    // Something must be selected
    if ( selection.right() == 0x7fff || selection.bottom() == 0x7fff || selection.left() == 0 )
    {
      QMessageBox::critical( this, i18n("KSpread Error" ), i18n("You must first select the cells\n"
								"which contain the data." ),
			     i18n( "Ok" ) );
      return;
    }
  }

  m_eAction = _act;
}

void KSpreadCanvas::setAction( Actions _act, KoDocumentEntry& _e )
{
  m_actionArgument = _e;
  setAction( _act );
}

void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  QRect selection( table->selectionRect() );

  if ( m_eMouseAction == ChildGeometry )
  {
    QPainter painter;
    painter.begin( this );
    painter.setRasterOp( NotROP );

    QPen pen;
    pen.setStyle( DashLine );
    painter.setPen( pen );

    if ( m_bGeometryStarted )
    {
      int x = m_ptGeometryStart.x();
      int y = m_ptGeometryStart.y();
      if ( x > m_ptGeometryEnd.x() )
	x = m_ptGeometryEnd.x();
      if ( y > m_ptGeometryEnd.y() )
	    y = m_ptGeometryEnd.y();
      int w = m_ptGeometryEnd.x() - m_ptGeometryStart.x();
      if ( w < 0 ) w *= -1;
      int h = m_ptGeometryEnd.y() - m_ptGeometryStart.y();
	if ( h < 0 ) h *= -1;
	
	painter.drawRect( x, y, w, h );
    }
    else
      m_bGeometryStarted = TRUE;

    m_ptGeometryEnd = _ev->pos();

    int x = m_ptGeometryStart.x();
    int y = m_ptGeometryStart.y();
    if ( x > m_ptGeometryEnd.x() )
      x = m_ptGeometryEnd.x();
    if ( y > m_ptGeometryEnd.y() )
      y = m_ptGeometryEnd.y();
    int w = m_ptGeometryEnd.x() - m_ptGeometryStart.x();
    if ( w < 0 ) w *= -1;
    int h = m_ptGeometryEnd.y() - m_ptGeometryStart.y();
    if ( h < 0 ) h *= -1;

    painter.drawRect( x, y, w, h );
    painter.end();

    return;
  }

  int ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  // Test wether the mouse is over some anchor
  KSpreadCell* cell = table->visibleCellAt( col, row );
  if ( cell )
    m_strAnchor = cell->testAnchor( _ev->pos().x() - xpos, _ev->pos().y() - ypos, this );

  // Test wether we are in the lower right corner of the marker
  // if so => change the cursor
  {
    int xpos;
    int ypos;
    int w, h;
    // No selection or just complete rows/columns ?
    if ( selection.left() == 0 || selection.right() == 0x7fff || selection.bottom() == 0x7fff )
    {
      xpos = table->columnPos( markerColumn(), this );
      ypos = table->rowPos( markerRow(), this );
      KSpreadCell *cell = table->cellAt( markerColumn(), markerRow() );
      w = cell->width( markerColumn(), this );
      h = cell->height( markerRow(), this );
    }
    else // if we have a rectangular selection ( not complete rows or columns )
    {
      xpos = table->columnPos( selection.left(), this );
      ypos = table->rowPos( selection.top(),  this );
      int x = table->columnPos( selection.right(), this );
      KSpreadCell *cell = table->cellAt( selection.right(), selection.top() );
      int tw = cell->width( selection.right(), this );
      w = ( x - xpos ) + tw;
      cell = table->cellAt( selection.left(), selection.bottom() );
      int y = table->rowPos( selection.bottom(), this );
      int th = cell->height( selection.bottom(), this );
      h = ( y - ypos ) + th;
    }

    if ( _ev->pos().x() >= xpos + w - 2 && _ev->pos().x() <= xpos + w + 3 &&
	 _ev->pos().y() >= ypos + h - 1 && _ev->pos().y() <= ypos + h + 4 )
      setCursor( sizeAllCursor );
    else if ( !m_strAnchor.isEmpty() )
      setCursor( KCursor::handCursor() );
    else
      setCursor( arrowCursor );
  }

  if ( m_eMouseAction == NoAction )
    return;

  if ( col < m_iMouseStartColumn )
	col = m_iMouseStartColumn;
  if ( row < m_iMouseStartRow )
    row = m_iMouseStartRow;

  if ( row == selection.bottom() && col == selection.right() )
    return;

  hideMarker();

  // Set the new lower right corner of the selection
  selection.setRight( col );
  selection.setBottom( row );
  table->setSelection( selection, this );

  // Scroll the table if neccessary
  if ( _ev->pos().x() < 0 )
    m_pHorzScrollBar->setValue( xOffset() + xpos );
  else if ( _ev->pos().x() > width() )
  {
    ColumnLayout *cl = table->columnLayout( col + 1 );
    xpos = table->columnPos( col + 1, this );
    m_pHorzScrollBar->setValue( xOffset() + ( xpos + cl->width( this ) - width() ) );
  }
  if ( _ev->pos().y() < 0 )
    m_pVertScrollBar->setValue( yOffset() + ypos );
  else if ( _ev->pos().y() > height() )
  {
    RowLayout *rl = table->rowLayout( row + 1 );
    ypos = table->rowPos( row + 1, this );
    m_pVertScrollBar->setValue( yOffset() + ( ypos + rl->height( this ) - height() ) );
  }

  showMarker();

  // All visible cells
  /* int left_col = table->leftColumn( 0, xpos, m_pView );
  int right_col = table->rightColumn( width(), m_pView );
  int top_row = table->topRow( 0, ypos, m_pView );
  int bottom_row = table->bottomRow( height(), m_pView ); */

  // Redraw all cells that changed their marking mode.
  /* for ( int x = left_col; x <= right_col; x++ )
    for ( int y = top_row; y <= bottom_row; y++ )
    {
      bool b1 = ( x >= r.left() && x <= r.right() );
      bool b2 = ( x >= selection.left() && x <= selection.right() );
      bool b3 = ( y >= r.top() && y <= r.bottom() );
      bool b4 = ( y >= selection.top() && y <= selection.bottom() );

      if ( ( b1 && !b2 ) || ( !b1 && b2 ) || ( b3 && !b4 ) || ( !b3 && b4 ) )
      {
	KSpreadCell *cell = table->cellAt( x, y );
	m_pView->drawCell( cell, x, y );
	m_bMouseMadeSelection = TRUE;
      }
    } */

  m_bMouseMadeSelection = true;
}

void KSpreadCanvas::mouseReleaseEvent( QMouseEvent *_ev )
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  if ( m_eMouseAction == ChildGeometry )
  {
    if ( !m_bGeometryStarted )
      return;

    m_bGeometryStarted = false;

    m_ptGeometryEnd = _ev->pos();

    int x = m_ptGeometryStart.x();
    int y = m_ptGeometryStart.y();
    if ( x > m_ptGeometryEnd.x() )
      x = m_ptGeometryEnd.x();
    if ( y > m_ptGeometryEnd.y() )
      y = m_ptGeometryEnd.y();
    int w = m_ptGeometryEnd.x() - m_ptGeometryStart.x();
    if ( w < 0 ) w *= -1;
    int h = m_ptGeometryEnd.y() - m_ptGeometryStart.y();
    if ( h < 0 ) h *= -1;

    QPainter painter;
    painter.begin( this );

    QPen pen;
    pen.setStyle( DashLine );
    painter.setPen( pen );

    painter.setRasterOp( NotROP );
    painter.drawRect( x, y, w, h );
    painter.end();

    QRect r( x, y, w, h );
    if ( m_eAction == InsertChart )
      m_pView->insertChart( r );
    else if ( m_eAction == InsertChild )
      m_pView->insertChild( r, m_actionArgument );
	
    m_eMouseAction = NoAction;
    m_eAction = DefaultAction;
    return;
  }

  hideMarker();

  QRect selection( table->selectionRect() );

  // The user started the drag in the lower right corner of the marker ?
  if ( m_eMouseAction == ResizeCell )
  {
    KSpreadCell *cell = table->nonDefaultCell( m_iMouseStartColumn, m_iMouseStartRow );
    cell->forceExtraCells( m_iMouseStartColumn, m_iMouseStartRow,
			   selection.right() - selection.left(),
			   selection.bottom() - selection.top() );
	
    selection.setCoords( 0, 0, 0, 0 );
    table->setSelection( selection, this );
    // m_pView->doc()->setModified( TRUE );
  }
  else if ( m_eMouseAction == AutoFill )
  {
    table->autofill( m_rctAutoFillSrc, table->selectionRect() );
	
    // m_pView->doc()->setModified( TRUE );
    selection.setCoords( 0, 0, 0, 0 );
    table->setSelection( selection, this );
  }
  // The user started the drag in the middle of a cell ?
  else if ( m_eMouseAction == Mark )
  {
    // Get the object in the lower right corner
    KSpreadCell *cell = table->cellAt( m_iMouseStartColumn, m_iMouseStartRow );
    // Did we mark only a single cell ?
    // Take care: One cell may obscure other cells ( extra size! ).
    if ( selection.left() + cell->extraXCells() == selection.right() &&
	 selection.top() + cell->extraYCells() == selection.bottom() )
    {
      // Delete the selection
      selection.setCoords( 0, 0, 0, 0 );
      table->setSelection( selection, this );
    }
  }

  m_eMouseAction = NoAction;
  if ( m_bMouseMadeSelection )
    drawVisibleCells();
  m_bMouseMadeSelection = FALSE;

  showMarker();
}

void KSpreadCanvas::mousePressEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  if ( m_pEditor )
  {
    setFocus();
    delete m_pEditor;
    m_pEditor = 0;
  }

  // Do we have to create a new chart at this position ?
  if ( m_eAction == InsertChart || m_eAction == InsertChild )
  {
    m_ptGeometryStart = _ev->pos();
    m_ptGeometryEnd = _ev->pos();
    m_bGeometryStarted = FALSE;
    m_eMouseAction = ChildGeometry;
    return;
  }

#ifdef USE_PICTURE
  if ( _ev->button() == LeftButton )
  {
    QPoint p( _ev->pos().x() + m_pView->xOffset(), _ev->pos().y() + m_pView->yOffset() );

    QListIterator<KSpreadChildPicture> it = m_pView->pictures();
    for( ; it != 0L; ++it )
    {
      if ( it.current()->geometry().contains( p ) )
      {
	m_pView->markChildPicture( it.current() );
	return;
      }
    }
  }
#endif

  QRect selection( table->selectionRect() );

  // Check wether we clicked in the little marker in the lower right
  // corner of a cell or a marked area.
  {
    // Get the position and size of the marker/marked-area
    int xpos;
    int ypos;
    int w, h;
    // No selection or complete rows/columns are selected
    if ( selection.left() == 0 ||
	 selection.right() == 0x7fff || selection.bottom() == 0x7fff )
    {
      xpos = table->columnPos( markerColumn(), this );
      ypos = table->rowPos( markerRow(), this );
      KSpreadCell *cell = table->cellAt( markerColumn(), markerRow() );
      w = cell->width( markerColumn() );
      h = cell->height( markerRow() );
    }
    else // if we have a rectangular selection ( not complete rows or columns )
    {
      xpos = table->columnPos( selection.left(), this );
      ypos = table->rowPos( selection.top(), this );
      int x = table->columnPos( selection.right(), this );
      KSpreadCell *cell = table->cellAt( selection.right(), selection.top() );
      int tw = cell->width( selection.right(), this );
      w = ( x - xpos ) + tw;
      cell = table->cellAt( selection.left(), selection.bottom() );
      int y = table->rowPos( selection.bottom(), this );
      int th = cell->height( selection.bottom(), this );
      h = ( y - ypos ) + th;
    }

    // Did we click in the lower right corner of the marker/marked-area ?
    if ( _ev->pos().x() >= xpos + w - 2 && _ev->pos().x() <= xpos + w + 3 &&
	 _ev->pos().y() >= ypos + h - 1 && _ev->pos().y() <= ypos + h + 4 )
    {
      // Auto fill ?
      if ( _ev->button() == LeftButton )
      {
	m_eMouseAction = AutoFill;
	// Do we have a selection already ?
	if ( selection.left() != 0 && selection.right() != 0x7fff && selection.bottom() != 0x7fff )
	{ /* Selection is ok */
	  m_rctAutoFillSrc = selection;
	}	
	else // Select the current cell
	{
	  KSpreadCell *cell = table->cellAt( markerColumn(), markerRow() );
	  selection.setCoords( markerColumn(), markerRow(),
			       markerColumn() + cell->extraXCells(),
			       markerRow() + cell->extraYCells() );
	  m_rctAutoFillSrc.setCoords( markerColumn(), markerRow(),
				      markerColumn(), markerRow() );
	}
	
	m_iMouseStartColumn = markerColumn();
	m_iMouseStartRow = markerRow();
      }
      // Resize a cell ?
      else if ( _ev->button() == MidButton && selection.left() == 0 )
      {
	m_eMouseAction = ResizeCell;
	KSpreadCell *cell = table->cellAt( markerColumn(), markerRow() );
	selection.setCoords( markerColumn(), markerRow(),
			     markerColumn() + cell->extraXCells(),
			     markerRow() + cell->extraYCells() );
	m_iMouseStartColumn = markerColumn();
	m_iMouseStartRow = markerRow();
      }

      table->setSelection( selection, this );
      return;
    }
  }

  hideMarker();

  int xpos, ypos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  if ( _ev->button() == LeftButton || !selection.contains( QPoint( col, row ) ) )
    table->unselect();

  setMarkerColumn( col );
  setMarkerRow( row );

  KSpreadCell *cell = table->cellAt( markerColumn(), markerRow() );

  // Go to the upper left corner of the obscuring object
  if ( cell->isObscured() )
  {
    setMarkerRow( cell->obscuringCellsRow() );
    setMarkerColumn( cell->obscuringCellsColumn() );
    cell = table->cellAt( markerColumn(), markerRow() );
  }

  // Test wether the mouse is over some anchor
  {
    KSpreadCell *cell = table->visibleCellAt( markerColumn(), markerRow() );
    QString anchor = cell->testAnchor( _ev->pos().x() - xpos,
				       _ev->pos().y() - ypos, this );
    if ( !anchor.isEmpty() && anchor != m_strAnchor )
      setCursor( KCursor::handCursor() );
    m_strAnchor = anchor;
  }

  // Start a marking action ?
  if ( m_strAnchor.isEmpty() && _ev->button() == LeftButton )
  {
    m_eMouseAction = Mark;
    selection.setCoords( markerColumn(), markerRow(),
			 markerColumn() + cell->extraXCells(),
			 markerRow() + cell->extraYCells() );

    // if ( old_selection.left() != 0 || cell->extraXCells() != 0 || cell->extraYCells() != 0 )
    table->setSelection( selection, this );
    m_iMouseStartColumn = markerColumn();
    m_iMouseStartRow = markerRow();
  }

  // Update the edit box
  if ( cell->text() != 0L )
    m_pEditWidget->setText( cell->text() );
  else
    m_pEditWidget->setText( "" );

  if ( !m_strAnchor.isEmpty() )
  {
    debug("ANCHOR=%s",m_strAnchor.ascii() );
    gotoLocation( KSpreadPoint( m_strAnchor, m_pDoc->map() ) );
  }

  showMarker();

  // Context menu ?
  if ( _ev->button() == RightButton )
  {
    // TODO: Handle anchor
    QPoint p = mapToGlobal( _ev->pos() );
    m_pView->openPopupMenu( p );
  }
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
  if ( m_pDoc->isLoading() )
    return;

  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  hideMarker();

  QPainter painter;
  painter.begin( this );
  painter.save();

  // printf("PAINT CANVAS %i/%i %i/%i\n",_ev->rect().left(),_ev->rect().top(),_ev->rect().right(),_ev->rect().bottom());

  int xpos;
  int ypos;
  int left_col = table->leftColumn( _ev->rect().x(), xpos, this );
  int right_col = table->rightColumn( _ev->rect().right(), this );
  int top_row = table->topRow( _ev->rect().y(), ypos, this );
  int bottom_row = table->bottomRow( _ev->rect().bottom(), this );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );

  QRect r;

  int left = xpos;
  for ( int y = top_row; y <= bottom_row; y++ )
  {
    RowLayout *row_lay = table->rowLayout( y );
    xpos = left;

    for ( int x = left_col; x <= right_col; x++ )
    {
      ColumnLayout *col_lay = table->columnLayout( x );
	
      KSpreadCell *cell = table->cellAt( x, y );
      cell->paintEvent( this, _ev->rect(), painter, xpos, ypos, x, y, col_lay, row_lay, &r );
	
      xpos += col_lay->width( this );
    }

    ypos += row_lay->height( this );
  }

#ifdef USE_PICTURES
  QListIterator<KSpreadChildPicture> it = m_pView->pictures();
  for( ; it != 0L; ++it )
  {
    // if ( it.current()->geometry().intersects( _ev->rect() ) )
    {
      painter.translate( it.current()->geometry().left() - xOffset(),
			 it.current()->geometry().top() - yOffset() );
      painter.drawPicture( *(it.current()->picture() ) );
      painter.translate( - it.current()->geometry().left() - xOffset(),
			 - it.current()->geometry().top() - yOffset() );
    }
  }
#endif

  painter.end();

  showMarker();
}

void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  if ( ( _ev->state() & Qt::ControlButton ) && _ev->key() == Key_E )
  {
    KSpreadCell* cell = activeTable()->cellAt( marker() );
    if ( cell->content() == KSpreadCell::Text )
      m_pEditWidget->setFocus();
    // TODO: Handle Formula and QML here, too
    return;
  }

  // Dont handle accelerators
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
  {
    QWidget::keyPressEvent( _ev );
    return;
  }

  switch( _ev->key() )
    {
      /**
       * Handle in KSpreadView event handler
       */
    case Key_Return:
    case Key_Enter:
    case Key_Down:
	if ( markerRow() == 0xFFFF )
	  return;

	if ( m_pEditor )
	{
	  setFocus();
	  m_pView->setText( m_pEditor->text() );
	  delete m_pEditor;
	  m_pEditor = 0;
	}
	break;
	
    case Key_Up:
	if ( markerRow() == 1 )
	    return;

	if ( m_pEditor )
	{
	  setFocus();
	  m_pView->setText( m_pEditor->text() );
	  delete m_pEditor;
	  m_pEditor = 0;
	}
	break;
	
    case Key_Left:
	if ( markerColumn() == 1 )
	    return;

	if ( m_pEditor )
	{
	  setFocus();
	  m_pView->setText( m_pEditor->text() );
	  delete m_pEditor;
	  m_pEditor = 0;
	}
	break;

    case Key_Right:
	if ( markerColumn() == 0xFFFF )
	    return;

	if ( m_pEditor )
	{
	  setFocus();
	  m_pView->setText( m_pEditor->text() );
	  delete m_pEditor;
	  m_pEditor = 0;
	}
	break;

	/**
	 * Handle here
	 */
    case Key_Escape:
      _ev->accept();
      if ( m_pEditor )
      {
	KSpreadCell* cell = table->cellAt( markerColumn(), markerRow() );
	if ( cell->text() != 0L )
	  m_pView->editWidget()->setText( cell->text() );
	else
	  m_pView->editWidget()->setText( "" );
	setFocus();

	delete m_pEditor;
	m_pEditor = 0;
      }
      return;

    default:
      // No null character ...
      if ( _ev->ascii() == 0 )
      {
	_ev->accept();
	return;
      }

      if ( !m_pEditor )
      {
	KSpreadCell* cell = activeTable()->cellAt( marker() );
	if ( _ev->ascii() == '*' )
	  m_pEditor = new KSpreadFormulaEditor( cell, this );
	else
	// TODO: Choose the correct editor here!
	  m_pEditor = new KSpreadTextEditor( cell, this );

	int w = cell->width( m_iMarkerColumn, this );
	int h = cell->height( m_iMarkerRow, this );
	int xpos = table->columnPos( markerColumn(), this );
	int ypos = table->rowPos( markerRow(), this );
	QPalette p = m_pEditor->palette();
	QColorGroup g( p.normal() );
	g.setColor( QColorGroup::Text, cell->textPen().color() );
	g.setColor( QColorGroup::Background, cell->KSpreadLayout::bgColor() );
	m_pEditor->setPalette( QPalette( g, p.disabled(), g ) );
	m_pEditor->setFont( cell->textFont() );
	m_pEditor->setGeometry( xpos, ypos, w, h );
	m_pEditor->setMinimumSize( QSize( w, h ) );
	m_pEditor->show();
	m_pEditor->setFocus();

	if ( _ev->ascii() != '*' )
	  m_pEditor->handleKeyPressEvent( _ev );	
      }
      else
	m_pEditor->handleKeyPressEvent( _ev );	
      return;
    }

  /**
   * Tell the KSpreadView event handler and enable
   * makro recording by the way.
   */
  _ev->accept();

  KSpread::EventKeyPressed event;
  event.key = _ev->key();
  event.state = _ev->state();
  event.ascii = _ev->ascii();
  EMIT_EVENT( m_pView, KSpread::eventKeyPressed, event );
}

void KSpreadCanvas::updateCellRect( const QRect &_rect )
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  QRect param( _rect );
  if ( param.left() <= 0 && param.top() <= 0 && param.right() <= 0 && param.bottom() <= 0 )
    return;

  // We want to repaint the border too => enlarge the rect
  if ( param.left() > 1 )
    param.setLeft( param.left() - 1 );
  if ( param.right() < 0x7fff )
    param.setRight( param.right() + 1 );
  if ( param.top() > 1 )
    param.setTop( param.top() - 1 );
  if ( param.bottom() < 0x7fff )
    param.setBottom( param.bottom() + 1 );

  if ( param.left() <= 0 )
    param.setLeft( 1 );
  if ( param.top() <= 0 )
    param.setTop( 1 );
  if ( param.right() < param.left() )
    return;
  if ( param.bottom() < param.top() )
    return;

  hideMarker();

  QPainter painter;
  painter.begin( this );
  painter.save();

  int xpos = table->columnPos( param.left(), this );
  int ypos = table->rowPos( param.top(), this );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );

  QRect r;

  int left = xpos;
  for ( int y = param.top(); y <= param.bottom(); y++ )
  {
    RowLayout *row_lay = table->rowLayout( y );
    xpos = left;

    for ( int x = param.left(); x <= param.right(); x++ )
    {
      ColumnLayout *col_lay = table->columnLayout( x );
	
      KSpreadCell *cell = table->cellAt( x, y );
      cell->paintEvent( this, rect(), painter, xpos, ypos, x, y, col_lay, row_lay, &r );
	
      xpos += col_lay->width( this );

      if ( xpos > width() )
	break;
    }

    ypos += row_lay->height( this );
    if ( ypos > height() )
      break;
  }

  painter.end();

  showMarker();
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadCanvas::drawVisibleCells()
{
  hideMarker();

  QPainter painter;
  painter.begin( this );
  painter.save();

  int xpos;
  int ypos;
  int left_col = activeTable()->leftColumn( 0, xpos, this );
  int right_col = activeTable()->rightColumn( width(), this );
  int top_row = activeTable()->topRow( 0, ypos, this );
  int bottom_row = activeTable()->bottomRow( height(), this );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );

  QRect rect( 0, 0, width(), height() );

  int left = xpos;
  for ( int y = top_row; y <= bottom_row; y++ )
  {
    RowLayout *row_lay = activeTable()->rowLayout( y );
    xpos = left;

    for ( int x = left_col; x <= right_col; x++ )
    {
      ColumnLayout *col_lay = activeTable()->columnLayout( x );
      KSpreadCell *cell = activeTable()->cellAt( x, y );
      cell->paintEvent( this, rect, painter, xpos, ypos, x, y, col_lay, row_lay );
      xpos += col_lay->width( this );
    }

    ypos += row_lay->height( this );
  }

  painter.end();

  showMarker();
}

void KSpreadCanvas::drawMarker( QPainter * _painter )
{
  bool own_painter = FALSE;

  if ( _painter == 0L )
  {
    _painter = new QPainter();
    _painter->begin( this );
    own_painter = TRUE;
  }

  int xpos;
  int ypos;
  int w, h;
  QRect selection( activeTable()->selectionRect() );
    // printf("selection: %i %i %i\n",selection.left(), selection.right(), selection.bottom() );

  if ( selection.left() == 0 || selection.right() == 0x7fff || selection.bottom() == 0x7fff )
  {
    xpos = activeTable()->columnPos( m_iMarkerColumn, this );
    ypos = activeTable()->rowPos( m_iMarkerRow, this );
    KSpreadCell *cell = activeTable()->cellAt( m_iMarkerColumn, m_iMarkerRow );
    w = cell->width( m_iMarkerColumn, this );
    h = cell->height( m_iMarkerRow, this );
  }
  else
  {
    xpos = activeTable()->columnPos( selection.left(), this );
    ypos = activeTable()->rowPos( selection.top(), this );
    int x = activeTable()->columnPos( selection.right(), this );
    KSpreadCell *cell = activeTable()->cellAt( selection.right(), selection.top() );
    int tw = cell->width( selection.right(), this );
    w = ( x - xpos ) + tw;
    cell = activeTable()->cellAt( selection.left(), selection.bottom() );
    int y = activeTable()->rowPos( selection.bottom(), this );
    int th = cell->height( selection.bottom(), this );
    h = ( y - ypos ) + th;
  }

  RasterOp rop = _painter->rasterOp();

  _painter->setRasterOp( NotROP );
  QPen pen;
  pen.setWidth( 3 );
  _painter->setPen( pen );

    // _painter->drawRect( xpos - 1, ypos - 1, w + 2, h + 2 );
  _painter->drawLine( xpos - 2, ypos - 1, xpos + w + 2, ypos - 1 );
  _painter->drawLine( xpos - 1, ypos + 1, xpos - 1, ypos + h + 3 );
  _painter->drawLine( xpos + 1, ypos + h + 1, xpos + w - 3, ypos + h + 1 );
  _painter->drawLine( xpos + w, ypos + 1, xpos + w, ypos + h - 2 );
  _painter->fillRect( xpos + w - 2, ypos + h - 1, 5, 5, black );
  _painter->setRasterOp( rop );

  if ( own_painter )
  {
    _painter->end();
    delete _painter;
  }

  char buffer[ 20 ];
  sprintf( buffer, "%s%d", activeTable()->columnLabel( m_iMarkerColumn ), m_iMarkerRow );

  m_pPosWidget->setText(buffer);
}

void KSpreadCanvas::hideMarker( QPainter& _painter )
{
  if ( m_iMarkerVisible == 1 )
    drawMarker( &_painter );
  m_iMarkerVisible--;
}

void KSpreadCanvas::showMarker( QPainter& _painter)
{
  if ( m_iMarkerVisible == 1 )
    return;
  m_iMarkerVisible++;
  if ( m_iMarkerVisible == 1 )
    drawMarker( &_painter );
}

void KSpreadCanvas::drawCell( KSpreadCell *_cell, int _col, int _row )
{
    QPainter painter;
    painter.begin( this );

    drawCell( painter, _cell, _col, _row );

    painter.end();
}

void KSpreadCanvas::drawCell( QPainter &painter, KSpreadCell *_cell, int _col, int _row )
{
    painter.save();

    hideMarker( painter );

    QRect rect( 0, 0, width(), height() );

    QRect r;
    _cell->paintEvent( this, rect, painter, _col, _row, &r );

    painter.restore();

    showMarker( painter );
}

/****************************************************************
 *
 * KSpreadVBorder
 *
 ****************************************************************/

KSpreadVBorder::KSpreadVBorder( QWidget *_parent, KSpreadCanvas *_canvas ) : QWidget( _parent )
{
  m_pCanvas = _canvas;

  setBackgroundColor( lightGray );
  setMouseTracking( TRUE );
  m_bResize = FALSE;
  m_bSelection = FALSE;
}

void KSpreadVBorder::mousePressEvent( QMouseEvent * _ev )
{
  m_bResize = FALSE;
  m_bSelection = FALSE;

  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  // Find the first visible row and the y position of this row.
  int y = 0;
  int row = table->topRow( 0, y, m_pCanvas );

  // Did the user click between two rows ?
  while ( y < height() )
  {
    int h = table->rowLayout( row )->height( m_pCanvas );
    row++;
    if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1 )
      m_bResize = TRUE;
    y += h;
  }

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    painter.drawLine( 0, _ev->pos().y(), m_pCanvas->width(), _ev->pos().y() );
    painter.end();

    int tmp;
    m_iResizeAnchor = table->topRow( _ev->pos().y() - 3, tmp, m_pCanvas );
    m_iResizePos = _ev->pos().y();
  }
  else
  {
    m_bSelection = TRUE;
	
    table->unselect();
    int tmp;
    int hit_row = table->topRow( _ev->pos().y(), tmp, m_pCanvas );
    m_iSelectionAnchor = hit_row;
    QRect selection( table->selectionRect() );
    selection.setCoords( 1, hit_row, 0x7FFF, hit_row );
    update();
    table->setSelection( selection, m_pCanvas );
  }
}

void KSpreadVBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );
    painter.end();
	
    RowLayout *rl = table->nonDefaultRowLayout( m_iResizeAnchor );
    int y = table->rowPos( m_iResizeAnchor, m_pCanvas );
    if ( _ev->pos().y() < 20 )
      rl->setHeight( 20, m_pCanvas );
    else
      rl->setHeight( _ev->pos().y() - y, m_pCanvas );
  }

  m_bSelection = FALSE;
  m_bResize = FALSE;
}

void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    m_iResizePos = _ev->pos().y();
    // Dont make the row have a height < 20 pixel.
    int twenty = (int)( 20.0 * m_pCanvas->zoom() );
    int y = table->rowPos( m_iResizeAnchor, m_pCanvas );
    if ( m_iResizePos < y + twenty )
      m_iResizePos = y + twenty;
    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );
    painter.end();
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    int y = 0;
    int row = table->topRow( _ev->pos().y(), y, m_pCanvas );
    QRect selection = table->selectionRect();

    if ( row < m_iSelectionAnchor )
    {
      selection.setTop( row );
      selection.setBottom( m_iSelectionAnchor );
    }
    else
    {
      selection.setBottom( row );
      selection.setTop( m_iSelectionAnchor );
    }
    table->setSelection( selection, m_pCanvas );

    if ( _ev->pos().y() < 0 )
      m_pCanvas->vertScrollBar()->setValue( m_pCanvas->yOffset() + y );
    else if ( _ev->pos().y() > m_pCanvas->height() )
    {
      RowLayout *rl = table->rowLayout( row + 1 );
      y = table->rowPos( row + 1, m_pCanvas );
      m_pCanvas->vertScrollBar()->setValue( m_pCanvas->yOffset()
						  + y + rl->height( m_pCanvas )
						  - m_pCanvas->height() );
    }	
  }
  // No button is pressed and the mouse is just moved
  else
  {
    int y = 0;
    int row = table->topRow( 0, y, m_pCanvas );
	
    while ( y < height() )
    {
      int h = table->rowLayout( row )->height( m_pCanvas );
      row++;
      if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1 )
      {
	setCursor( sizeAllCursor );
	return;
      }
      y += h;
    }
	
    setCursor( arrowCursor );
  }
}

void KSpreadVBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  if ( !table )
    return;

  QPainter painter;
  painter.begin( this );
  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  painter.setClipRect( _ev->rect() );

  int ypos;
  int top_row = table->topRow( _ev->rect().y(), ypos, m_pCanvas );
  int bottom_row = table->bottomRow( _ev->rect().bottom(), m_pCanvas );

  QRect selection( table->selectionRect() );

  for ( int y = top_row; y <= bottom_row; y++ )
  {
    bool selected = ( selection.left() != 0 && selection.right() == 0x7FFF &&
		      y >= selection.top() && y <= selection.bottom() );

    RowLayout *row_lay = table->rowLayout( y );

    if ( selected )
    {
      static QColorGroup g2( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill2( black );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pCanvas ), g2, FALSE, 1, &fill2 );
    }
    else
    {
      static QColorGroup g( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill( lightGray );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pCanvas ), g, FALSE, 1, &fill );
    }
	
    char buffer[ 20 ];
    sprintf( buffer, "%i", y );

    if ( selected )
      painter.setPen( white );
    else
      painter.setPen( black );

    painter.drawText( 3, ypos + ( row_lay->height( m_pCanvas ) + painter.fontMetrics().ascent() - painter.fontMetrics().descent() ) / 2, buffer );

    ypos += row_lay->height( m_pCanvas );
  }

  painter.end();
}

/****************************************************************
 *
 * KSpreadHBorder
 *
 ****************************************************************/

KSpreadHBorder::KSpreadHBorder( QWidget *_parent, KSpreadCanvas *_canvas ) : QWidget( _parent )
{
  m_pCanvas = _canvas;

  setBackgroundColor( lightGray );
  setMouseTracking( TRUE );
  m_bResize = FALSE;
  m_bSelection = FALSE;
}

void KSpreadHBorder::mousePressEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  m_bResize = FALSE;
  m_bSelection = FALSE;

  int x = 0;
  int col = table->leftColumn( 0, x, m_pCanvas );

  while ( x < width() && !m_bResize )
  {
    int w = table->columnLayout( col )->width( m_pCanvas );
    col++;
    if ( _ev->pos().x() >= x + w - 1 && _ev->pos().x() <= x + w + 1 )
      m_bResize = TRUE;
    x += w;
  }

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    painter.drawLine( _ev->pos().x(), 0, _ev->pos().x(), m_pCanvas->height() );
    painter.end();

    int tmp;
    m_iResizeAnchor = table->leftColumn( _ev->pos().x() - 3, tmp, m_pCanvas );
    m_iResizePos = _ev->pos().x();
  }
  else
  {
    m_bSelection = TRUE;

    table->unselect();
    int tmp;
    int hit_col = table->leftColumn( _ev->pos().x(), tmp, m_pCanvas );
    m_iSelectionAnchor = hit_col;	
    QRect r;
    r.setCoords( hit_col, 1, hit_col, 0x7FFF );
    table->setSelection( r, m_pCanvas );
  }
}

void KSpreadHBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
    painter.end();
	
    ColumnLayout *cl = table->nonDefaultColumnLayout( m_iResizeAnchor );
    int x = table->columnPos( m_iResizeAnchor, m_pCanvas );
    if ( ( m_pCanvas->zoom() * (float)( _ev->pos().x() - x ) ) < 20.0 )
      cl->setWidth( 20, m_pCanvas );	
    else
      cl->setWidth( _ev->pos().x() - x, m_pCanvas );
  }

  m_bSelection = FALSE;
  m_bResize = FALSE;
}

void KSpreadHBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    m_iResizePos = _ev->pos().x();
    int twenty = (int)( 20.0 * m_pCanvas->zoom() );
    // Dont make the column have a width < 20 pixels.
    int x = table->columnPos( m_iResizeAnchor, m_pCanvas );
    if ( m_iResizePos < x + twenty )
      m_iResizePos = x + twenty;
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
    painter.end();
  }
  else if ( m_bSelection )
  {
    int x = 0;
    int col = table->leftColumn( _ev->pos().x(), x, m_pCanvas );
    QRect r = table->selectionRect();

    if ( col < m_iSelectionAnchor )
    {
      r.setLeft( col );
      r.setRight( m_iSelectionAnchor );
    }
    else
    {
      r.setRight( col );
      r.setLeft( m_iSelectionAnchor );
    }
    table->setSelection( r, m_pCanvas );

    if ( _ev->pos().x() < 0 )
      m_pCanvas->horzScrollBar()->setValue( m_pCanvas->xOffset() + x );
    else if ( _ev->pos().x() > m_pCanvas->width() )
    {
      ColumnLayout *cl = table->columnLayout( col + 1 );
      x = table->columnPos( col + 1, m_pCanvas );
      m_pCanvas->horzScrollBar()->setValue( m_pCanvas->xOffset() +
					  ( x + cl->width( m_pCanvas ) - m_pCanvas->width() ) );
    }
  }
  // Perhaps we have to modify the cursor
  else
  {
    int x = 0;
    int col = table->leftColumn( 0, x, m_pCanvas );

    while ( x < width() )
    {
      int w = table->columnLayout( col )->width( m_pCanvas );
      col++;
      if ( _ev->pos().x() >= x + w - 1 && _ev->pos().x() <= x + w + 1 )
      {
	setCursor( sizeAllCursor );
	return;
      }
      x += w;
    }
    setCursor( arrowCursor );
  }
}

void KSpreadHBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();

  if (!table )
	return;

  QPainter painter;
  painter.begin( this );
  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  int xpos;
  int left_col = table->leftColumn( _ev->rect().x(), xpos, m_pCanvas );
  int right_col = table->rightColumn( _ev->rect().right(), m_pCanvas );

  QRect selection( table->selectionRect() );

  for ( int x = left_col; x <= right_col; x++ )
  {
    bool selected = ( selection.left() != 0 && selection.bottom() == 0x7FFF &&
		      x >= selection.left() && x <= selection.right() );
	
    ColumnLayout *col_lay = table->columnLayout( x );

    if ( selected )
    {
      static QColorGroup g2( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill2( black );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ), XBORDER_HEIGHT, g2, FALSE, 1, &fill2 );
    }
    else
    {
      static QColorGroup g( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill( lightGray );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ), XBORDER_HEIGHT, g, FALSE, 1, &fill );
    }

    int len = painter.fontMetrics().width( table->columnLabel(x) );

    if ( selected )
      painter.setPen( white );
    else
      painter.setPen( black );
	
    painter.drawText( xpos + ( col_lay->width( m_pCanvas ) - len ) / 2,
		      ( XBORDER_HEIGHT + painter.fontMetrics().ascent() - painter.fontMetrics().descent() ) / 2,
		      table->columnLabel(x) );

    xpos += col_lay->width( m_pCanvas );
	}

  painter.end();
}

#include "kspread_canvas.moc"
