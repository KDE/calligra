#include "kspread_canvas.h"
#include "kspread_table.h"
#include "kspread_cell.h"
#include "kspread_util.h"
#include "kspread_editors.h"
#include "kspread_map.h"

// Hack
#include "kspread_view.h"
#include "kspread_doc.h"

#include "kspread_tabbar.h"
#include <klocale.h>
#include <kcursor.h>

#include <cassert>
#include <qlabel.h>
#include <qpainter.h>

// Fuck, that is bad programming style. Make it at least static ....

bool mousePressed;
/****************************************************************
 *
 * KSpreadEditWidget
 *
 ****************************************************************/

KSpreadEditWidget::KSpreadEditWidget( QWidget *_parent, KSpreadView *_view ) : QLineEdit( _parent, "KSpreadEditWidget" )
{
    m_pView = _view;
    if ( !m_pView->koDocument()->isReadWrite() )
      setEnabled( false );
}

void KSpreadEditWidget::slotAbortEdit()
{
    m_pView->canvasWidget()->deleteEditor();
    m_pView->canvasWidget()->setFocus();

    m_pView->updateEditWidget();
}

void KSpreadEditWidget::slotDoneEdit()
{
    m_pView->setText( text() );
    m_pView->canvasWidget()->deleteEditor();
    m_pView->canvasWidget()->setFocus();
}

void KSpreadEditWidget::keyPressEvent ( QKeyEvent* _ev )
{
    // Dont handle special keys and accelerators
    if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
    {
	QLineEdit::keyPressEvent( _ev );
	return;
    }

    if ( !m_pView->koDocument()->isReadWrite() )
      return;
    
    switch ( _ev->key() )
    {
    case Key_Down:
    case Key_Up:
    case Key_Return:
    case Key_Enter:
	
	slotDoneEdit();

	QApplication::sendEvent( m_pView->canvasWidget(), _ev );
	
	_ev->accept();
	break;

    default:

	QLineEdit::keyPressEvent( _ev );
	
	if ( !m_pView->canvasWidget()->editor() )
	    m_pView->canvasWidget()->createEditor( KSpreadCanvas::CellEditor );
	setFocus();
	((KSpreadTextEditor*)m_pView->canvasWidget()->editor())->blockCheckChoose( TRUE );
	m_pView->canvasWidget()->editor()->setText( text() );
	((KSpreadTextEditor*)m_pView->canvasWidget()->editor())->blockCheckChoose( FALSE );
	m_pView->canvasWidget()->editor()->setCursorPosition( cursorPosition() );
    }
}

void KSpreadEditWidget::focusOutEvent( QFocusEvent* ev )
{
    qDebug("************* EditWidget lost focus");
    m_pView->canvasWidget()->setLastEditorWithFocus( KSpreadCanvas::EditWidget );

    QLineEdit::focusOutEvent( ev );
}

void KSpreadEditWidget::setText( const QString& t )
{
    if ( t == text() )
	return;

    QLineEdit::setText( t );
}

/****************************************************************
 *
 * KSpreadCanvas
 *
 ****************************************************************/

KSpreadCanvas::KSpreadCanvas( QWidget *_parent, KSpreadView *_view, KSpreadDoc* _doc )
    : QWidget( _parent, "", WNorthWestGravity )
{
    length_namecell = 0;
    m_chooseStartTable = 0;
    m_pEditor = 0;
    m_bChoose = FALSE;

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
    // m_eAction = DefaultAction;
    m_eMouseAction = NoAction;
    m_bGeometryStarted = false;
    // m_bEditDirtyFlag = false;

    m_pEditWidget = m_pView->editWidget();
    m_pPosWidget = m_pView->posWidget();

    setBackgroundColor( white );
    setMouseTracking( TRUE );

    setBackgroundMode( NoBackground );

    mousePressed = false;

    choose_visible = false;
}

void KSpreadCanvas::startChoose()
{
    if ( m_bChoose )
	return;

    // Clear a selection if there is any
    m_i_chooseMarkerColumn = markerColumn();
    m_i_chooseMarkerRow = markerRow();
    activeTable()->setChooseRect( QRect( 0, 0, 0, 0 ) );

    // It is important to enable this AFTER we set the rect!
    m_bChoose = TRUE;
    m_chooseStartTable = activeTable();
}

void KSpreadCanvas::startChoose( const QRect& rect )
{
    activeTable()->setChooseRect( rect );

    m_i_chooseMarkerColumn = rect.right();
    m_i_chooseMarkerRow = rect.bottom();

    // It is important to enable this AFTER we set the rect!
    m_bChoose = TRUE;
    m_chooseStartTable = activeTable();
}

void KSpreadCanvas::endChoose()
{
    if ( !m_bChoose )
	return;

    activeTable()->setChooseRect( QRect( 0, 0, 0, 0 ) );
    m_pView->setActiveTable( m_chooseStartTable );
    qDebug("endChoose len=0");
    length_namecell = 0;
    m_bChoose = FALSE;
    m_chooseStartTable = 0;
}

KSpreadHBorder* KSpreadCanvas::hBorderWidget()
{
    return m_pView->hBorderWidget();
}

KSpreadVBorder* KSpreadCanvas::vBorderWidget()
{
    return m_pView->vBorderWidget();
}

QScrollBar* KSpreadCanvas::horzScrollBar()
{
    return m_pView->horzScrollBar();
}

QScrollBar* KSpreadCanvas::vertScrollBar()
{
    return m_pView->vertScrollBar();
}

KSpreadTable* KSpreadCanvas::findTable( const QString& _name )
{
    return m_pDoc->map()->findTable( _name );
}

void KSpreadCanvas::insertFormulaChar(int c)
{
    m_pEditor->insertFormulaChar(c);
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

    gotoLocation( _cell.pos.x(), _cell.pos.y(), table );
}

void KSpreadCanvas::gotoLocation( int x, int y, KSpreadTable* table, bool make_select )
{
    if ( table )
	m_pView->setActiveTable( table );
    else
	table = activeTable();

    KSpreadCell* cell = table->cellAt( x, y );
    if ( cell->isObscured() && cell->isObscuringForced() )
    {
	y = cell->obscuringCellsRow();
	x = cell->obscuringCellsColumn();
    }

    int xpos = table->columnPos( x, this );
    int ypos = table->rowPos( y, this );

    if ( xpos < 0 || xpos > width()-100 * zoom() )
	horzScrollBar()->setValue( xOffset() + xpos );

    if ( ypos < 0 || ypos > height() - 50 * zoom() )
	vertScrollBar()->setValue( yOffset() + ypos );

    QRect selection = activeTable()->selectionRect();
	
    if ( !make_select )
    {
	if ( selection.left() != 0 )
	    activeTable()->unselect();
	setMarker( QPoint( x, y ) );
    }
    else
    {
	if ( selection.left() == 0 )
	    selection.setCoords( markerColumn(), markerRow(), markerColumn(), markerRow() );
	
	if ( markerColumn() == selection.left() )
	    selection.setLeft( x );
	else
	    selection.setRight( x );

	if ( markerRow() == selection.top() )
	    selection.setTop( y );
	else
	    selection.setBottom( y );
	selection = selection.normalize();
	m_iMarkerColumn = x;
	m_iMarkerRow = y;
	activeTable()->setSelection( selection );
    }

    // Perhaps the user is entering a value in the cell.
    // In this case we may not touch the EditWidget
    if ( !m_pEditor )
	m_pView->updateEditWidget();
}

void KSpreadCanvas::chooseGotoLocation( int x, int y, KSpreadTable* table, bool make_select )
{
    if ( table )
	m_pView->setActiveTable( table );
    else
	table = activeTable();

    KSpreadCell* cell = table->cellAt( x, y );
    if ( cell->isObscured() && cell->isObscuringForced() )
    {
	x = cell->obscuringCellsRow();
	y = cell->obscuringCellsColumn();
    }

    int xpos = table->columnPos( x, this );
    int ypos = table->rowPos( y, this );

    if ( xpos < 0 || xpos > width()-100 * zoom() )
	horzScrollBar()->setValue( xOffset() + xpos );

    if ( ypos < 0 || ypos > height() - 50 * zoom() )
	vertScrollBar()->setValue( yOffset() + ypos );

    if ( !make_select )
	setChooseMarker( QPoint( x, y ) );
    else
    {
	QRect selection = activeTable()->chooseRect();
	if ( chooseMarkerColumn() == selection.left() )
	    selection.setLeft( x );
	else
	    selection.setRight( x );

	if ( chooseMarkerRow() == selection.top() )
	    selection.setTop( y );
	else
	    selection.setBottom( y );
	selection = selection.normalize();
	m_i_chooseMarkerColumn = x;
	m_i_chooseMarkerRow = y;
	activeTable()->setChooseRect( selection );
    }
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
  hBorderWidget()->scroll( dx, 0 );
  /* if(isgotohorz()==true)
  	{
  	setgotohorz(false);

  	m_pView->horzScrollBar()->setValue(_value);
  	
  	} */
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
  vBorderWidget()->scroll( 0, dy );

  /* if(isgotovert()==true)
  	{
  	setgotovert(false);
  	m_pView->vertScrollBar()->setValue(_value);
  	} */
  showMarker();

  activeTable()->enableScrollBarUpdates( true );
}

void KSpreadCanvas::slotMaxColumn( int _max_column )
{
  int xpos = activeTable()->columnPos( _max_column + 10, this );

  horzScrollBar()->setRange( 0, xpos + xOffset() );
}

void KSpreadCanvas::slotMaxRow( int _max_row )
{
  int ypos = activeTable()->rowPos( _max_row + 10, this );

  vertScrollBar()->setRange( 0, ypos + yOffset() );
}
/*
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
*/
void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
    if ( !m_pView->koDocument()->isReadWrite() )
      return;

    if( m_bChoose )
    {
	chooseMouseMoveEvent( _ev );
	return;
    }

    KSpreadTable *table = activeTable();
    if ( !table )
	return;

  QRect selection( table->selectionRect() );

  int ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  // Test whether the mouse is over some anchor
  KSpreadCell* cell = table->visibleCellAt( col, row );
  if ( cell )
    m_strAnchor = cell->testAnchor( _ev->pos().x() - xpos, _ev->pos().y() - ypos, this );

  // Test whether we are in the lower right corner of the marker
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
    horzScrollBar()->setValue( xOffset() + xpos );
  else if ( _ev->pos().x() > width() )
  {
    ColumnLayout *cl = table->columnLayout( col + 1 );
    xpos = table->columnPos( col + 1, this );
    horzScrollBar()->setValue( xOffset() + ( xpos + cl->width( this ) - width() ) );
  }
  if ( _ev->pos().y() < 0 )
    vertScrollBar()->setValue( yOffset() + ypos );
  else if ( _ev->pos().y() > height() )
  {
    RowLayout *rl = table->rowLayout( row + 1 );
    ypos = table->rowPos( row + 1, this );
    vertScrollBar()->setValue( yOffset() + ( ypos + rl->height( this ) - height() ) );
  }

  showMarker();

  m_bMouseMadeSelection = true;
}

void KSpreadCanvas::mouseReleaseEvent( QMouseEvent* _ev )
{
    if( m_bChoose )
    {
	chooseMouseReleaseEvent( _ev );
	return;
    }

    mousePressed = false;
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

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
    QRect dest = table->selectionRect();
    table->autofill( m_rctAutoFillSrc, dest );
	
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
  // if ( m_bMouseMadeSelection )
  // drawVisibleCells();
  m_bMouseMadeSelection = FALSE;

  showMarker();
}

void KSpreadCanvas::mousePressEvent( QMouseEvent * _ev )
{
    if( m_bChoose )
    {
	chooseMousePressEvent( _ev );
	return;
    }

    KSpreadTable *table = activeTable();
    mousePressed = true;
    if ( !table )
	return;

    else if ( m_pEditor )
    {
	setFocus();
	deleteEditor();
	m_pEditor = 0;
    }

    QRect selection( table->selectionRect() );

    int old_column = markerColumn();
    int old_row = markerRow();
  // Check whether we clicked in the little marker in the lower right
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

  if ((selection.right() != 0x7fff && selection.bottom() != 0x7fff)&& mousePressed && ( _ev->state() & ControlButton ))
  	{
  	if( (col!=old_column)||(row!=old_row))
  		{
  		if(old_column>col)
  			{
  			int tmp=col;
  			col=old_column;
  			old_column=tmp;
  			}
  		if(old_row>row)
  			{
  			int tmp=row;
  			row=old_row;
  			old_row=tmp;
  			}
  		if( selection.left()!=0&&selection.right()!=0
  			&&selection.top()!=0&&selection.bottom()!=0)
  			{
  			//if you have a selection and you go up in the table
  			//so bottom is the bottom of the selection
  			//not the markercolumn
  			if(selection.bottom()>row)
  				row=selection.bottom();
  			if(selection.right()>col)
  				col=selection.right();
  			}
  		selection.setLeft(old_column);
  		selection.setRight( col );
  		selection.setTop(old_row);
  		selection.setBottom( row );
  		table->setSelection( selection, this );
    		//always put Marker in top and left
    		//otherwise all functions don't work
    		setMarkerColumn( old_column );
  		setMarkerRow( old_row );
                showMarker();
                return;
      		}
  	}

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

  // Test whether the mouse is over some anchor
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
  m_pView->updateEditWidget();

  if ( !m_strAnchor.isEmpty() )
  {
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

void KSpreadCanvas::chooseMouseMoveEvent( QMouseEvent * _ev )
{
    if ( !mousePressed )
	return;

    KSpreadTable *table = activeTable();
    if ( !table )
	return;

    int ypos, xpos;
    int row = table->topRow( _ev->pos().y(), ypos, this );
    int col = table->leftColumn( _ev->pos().x(), xpos, this );

    if ( col < m_iMouseStartColumn )
	col = m_iMouseStartColumn;
    if ( row < m_iMouseStartRow )
	row = m_iMouseStartRow;

    // Noting changed ?
    QRect selection( table->chooseRect() );
    if ( row == selection.bottom() && col == selection.right() )
	return;

    // Set the new lower right corner of the selection
    selection.setRight( col );
    selection.setBottom( row );
    table->setChooseRect( selection );

    // Scroll the table if neccessary
    if ( _ev->pos().x() < 0 )
	horzScrollBar()->setValue( xOffset() + xpos );
    else if ( _ev->pos().x() > width() )
    {
	ColumnLayout *cl = table->columnLayout( col + 1 );
	xpos = table->columnPos( col + 1, this );
	horzScrollBar()->setValue( xOffset() + ( xpos + cl->width( this ) - width() ) );
    }

    if ( _ev->pos().y() < 0 )
	vertScrollBar()->setValue( yOffset() + ypos );
    else if ( _ev->pos().y() > height() )
    {
	RowLayout *rl = table->rowLayout( row + 1 );
	ypos = table->rowPos( row + 1, this );
	vertScrollBar()->setValue( yOffset() + ( ypos + rl->height( this ) - height() ) );
    }
}

void KSpreadCanvas::chooseMouseReleaseEvent( QMouseEvent* )
{
    mousePressed = FALSE;
}

void KSpreadCanvas::chooseMousePressEvent( QMouseEvent * _ev )
{
    mousePressed = TRUE;

    KSpreadTable *table = activeTable();
    if ( !table )
	return;
    QRect selection(table->chooseRect());

    int ypos, xpos;
    int row = table->topRow( _ev->pos().y(), ypos, this );
    int col = table->leftColumn( _ev->pos().x(), xpos, this );

    if ((selection.right() != 0x7fff && selection.bottom() != 0x7fff)&& ( _ev->state() & ControlButton ))
  	{
  	if( (col!=m_iMouseStartColumn)||(row!=m_iMouseStartRow))
  		{
  		if( selection.left()!=0&&selection.right()!=0
  			&&selection.top()!=0&&selection.bottom()!=0)
  			{
  			if ( col < m_iMouseStartColumn )
				col = m_iMouseStartColumn;
  			if ( row < m_iMouseStartRow )
    				row = m_iMouseStartRow;

  			if ( row == selection.bottom() && col == selection.right() )
    				return;
    			}
  		selection.setLeft(m_iMouseStartColumn);
  		selection.setRight( col );
  		selection.setTop(m_iMouseStartRow);
  		selection.setBottom( row );
  		table->setChooseRect( selection );
                return;
      		}
    }
    setChooseMarkerColumn( col );
    setChooseMarkerRow( row );	
    KSpreadCell *cell = table->cellAt( chooseMarkerColumn(), chooseMarkerRow() );

    // Go to the upper left corner of the obscuring object
    if ( cell->isObscured() )
    {
	setChooseMarkerRow( cell->obscuringCellsRow() );
	setChooseMarkerColumn( cell->obscuringCellsColumn() );
	cell = table->cellAt( chooseMarkerColumn(), chooseMarkerRow() );
    }


    selection.setCoords( chooseMarkerColumn(), chooseMarkerRow(),
			 chooseMarkerColumn() + cell->extraXCells(),
			 chooseMarkerRow() + cell->extraYCells() );

    table->setChooseRect( selection );
    m_iMouseStartColumn = chooseMarkerColumn();
    m_iMouseStartRow = chooseMarkerRow();
}

void KSpreadCanvas::mouseDoubleClickEvent( QMouseEvent*  )
{
  if ( m_pView->koDocument()->isReadWrite() ) 
    createEditor();
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
    if ( m_pDoc->isLoading() )
	return;

    if ( !activeTable() )
	return;

    qDebug("------------PAINT EVENT %i,%i %i|%i widget %i:%i", _ev->rect().x(), _ev->rect().y(),
	   _ev->rect().width(), _ev->rect().height(), width(), height() );

    hideMarker();

    QPainter painter;
    painter.begin( this );

    QWMatrix m = m_pView->matrix();
    painter.setWorldMatrix( m );
    m = m.invert();
    QPoint tl = m.map( _ev->rect().topLeft() );
    QPoint br = m.map( _ev->rect().bottomRight() );

    qDebug("Mapped topleft to %i:%i", tl.x(), tl.y() );

    painter.save();

    // Clip away children
    QRegion rgn = painter.clipRegion();
    if ( rgn.isEmpty() )
	rgn = QRegion( _ev->rect() );
    QListIterator<KoDocumentChild> it( m_pDoc->children() );
    for( ; it.current(); ++it )
    {
	if ( ((KSpreadChild*)it.current())->table() == activeTable() &&
	     !m_pView->hasDocumentInWindow( it.current()->document() ) )
        {
	    rgn -= it.current()->region( painter.worldMatrix() );
	}
    }
    painter.setClipRegion( rgn );

    // Draw content
    m_pDoc->paintContent( painter, QRect( tl, br ), FALSE, activeTable() );

    painter.restore();

    // Draw children
    // QListIterator<PartChild> it( m_pDoc->children() );
    it.toFirst();
    for( ; it.current(); ++it )
    {
	if ( ((KSpreadChild*)it.current())->table() == activeTable() &&
	     !m_pView->hasDocumentInWindow( it.current()->document() ) )
        {
	    // #### todo: paint only if child is visible inside rect
	    painter.save();
	    m_pDoc->paintChild( it.current(), painter, m_pView );
	    painter.restore();
	}
    }

    painter.end();

    showMarker();

    if( choose_visible )
  	drawChooseMarker( );
}

void KSpreadCanvas::focusInEvent( QFocusEvent* )
{
    // If we are in editing mode, we redirect the
    // focus to the CellEditor or EditWidget
    if ( !m_pEditor )
	return;

    // Strange focus bug. Lets fix it here!
    if ( lastEditorWithFocus() == EditWidget )
    {
	m_pView->editWidget()->setFocus();
	qDebug("Focus to EditWidget");
	return;
    }

    qDebug("Redirecting focus to editor");
    m_pEditor->setFocus();
}

void KSpreadCanvas::focusOutEvent( QFocusEvent* )
{
}

void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
    KSpreadTable *table = activeTable();
    QString tmp;
    if ( !table )
	return;

    // Dont handle accelerators
    if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
    {
	QWidget::keyPressEvent( _ev );
	return;
    }

    _ev->accept();
	
    // Find out about the current selection
    QRect selection;
    if ( m_bChoose )
	selection = activeTable()->chooseRect();
    else
	selection = activeTable()->selectionRect();

    // Are we making a selection right now ? Go thru this only if no selection is made
    // or if we neither selected complete rows nor columns.
    bool make_select = FALSE;
    if ( ( _ev->state() & ShiftButton ) == ShiftButton &&
	 ( _ev->key() == Key_Down || _ev->key() == Key_Up || _ev->key() == Key_Left || _ev->key() == Key_Right ||
	   _ev->key() == Key_Prior || _ev->key() == Key_Next || _ev->key() == Key_Home || _ev->key() == Key_End ) )
	make_select = TRUE;

    switch( _ev->key() )
    {
      /**
       * Handle in KSpreadView event handler
       */
    case Key_Return:
    case Key_Enter:
    case Key_Down:
    	
	if ( m_pEditor && !m_bChoose )
	{
	    // Delete the editor first and after that update the document.
	    // That means we get a syncron repaint after the editor
	    // widget is gone. Otherwise we may get painting errors.
	    QString t = m_pEditor->text();
	    setFocus();
	    deleteEditor();
	    m_pView->setText( t );
	}

	if ( !m_bChoose && markerRow() == 0xFFFF )
	  return;
	if ( m_bChoose && chooseMarkerRow() == 0xFFFF )
	  return;

	if ( m_bChoose )
	    chooseGotoLocation( chooseMarkerColumn(), QMIN( 0x7FFF, chooseMarkerRow() + 1 ), 0, make_select );
	else
	    gotoLocation( markerColumn(), QMIN( 0x7FFF, markerRow() + 1 ), 0, make_select );

	return;

    case Key_Up:

	if ( m_pEditor && !m_bChoose )
	{
	    QString t = m_pEditor->text();
	    setFocus();
	    deleteEditor();
	    m_pView->setText( t );
	}

	if ( !m_bChoose && markerRow() == 1 )
	    return;
	if ( m_bChoose && chooseMarkerRow() == 1 )
	    return;

	if ( m_bChoose )
	    chooseGotoLocation( chooseMarkerColumn(), QMAX( 1, chooseMarkerRow() - 1 ), 0, make_select );
	else
	    gotoLocation( markerColumn(), QMAX( 1, markerRow() - 1 ), 0, make_select );

	return;

    case Key_Right:
    	
	if ( m_pEditor && !m_bChoose )
	{
	    QString t = m_pEditor->text();
	    setFocus();
	    deleteEditor();
	    m_pView->setText( t );
	}

	if ( !m_bChoose && markerColumn() == 0xFFFF )
	    return;
	if ( m_bChoose && chooseMarkerColumn() == 0xFFFF )
	    return;

	if ( m_bChoose )
	    chooseGotoLocation( QMIN( 0x7FFF, chooseMarkerColumn() + 1 ), chooseMarkerRow(), 0, make_select );
	else
	    gotoLocation( QMIN( 0x7FFF, markerColumn() + 1 ), markerRow(), 0, make_select );

	return;
	
    case Key_Left:
    		
	if ( m_pEditor && !m_bChoose )
	{
	    QString t = m_pEditor->text();
	    setFocus();
	    deleteEditor();
	    m_pView->setText( t );
	}

	if ( !m_bChoose && markerColumn() == 1 )
	    return;
	if ( m_bChoose && chooseMarkerColumn() == 1 )
	    return;

	if ( m_bChoose )
	    chooseGotoLocation( QMAX( 1, chooseMarkerColumn() - 1 ), chooseMarkerRow(), 0, make_select );
	else
	    gotoLocation( QMAX( 1, markerColumn() - 1 ), markerRow(), 0, make_select );

	return;

	/**
	 * Handle here
	 */
    case Key_Escape:
    	
	_ev->accept();
	if ( m_pEditor )
        {
	    deleteEditor();
	
	    m_pView->updateEditWidget();
	}
	return;

      // We dont get that key while m_pEditor != 0
    case Key_Home:
	
	if ( m_bChoose )
	    chooseGotoLocation( 1, 1, 0, make_select );
	else
	    gotoLocation( 1, 1, 0, make_select );
    	return;
    	
    case Key_Prior:
	
    	if ( m_pEditor && !m_bChoose )
        {
	    QString t = m_pEditor->text();
	    setFocus();
	    deleteEditor();
	    m_pView->setText( t );
	}
	
    	if( !m_bChoose && markerRow() == 1 )
	    return;
    	if( m_bChoose && chooseMarkerRow() == 1 )
	    return;
    		
	if ( m_bChoose )
	    chooseGotoLocation( chooseMarkerColumn(), QMAX( 1, chooseMarkerRow() - 10 ), 0, make_select );
	else
	    gotoLocation( markerColumn(), QMAX( 1, markerRow() - 10 ), 0, make_select );

	return;
    	
    case Key_Next:
	
    	if ( m_pEditor && !m_bChoose )
        {
	    QString t = m_pEditor->text();
	    setFocus();
	    deleteEditor();
	    m_pView->setText( t );
	}

	if( !m_bChoose && markerRow() == 0x7FFF )
	    return;
	if( m_bChoose && chooseMarkerRow() == 0x7FFF )
	    return;

	if ( m_bChoose )
	    chooseGotoLocation( chooseMarkerColumn(), QMIN( 0x7FFF, chooseMarkerRow() + 10 ), 0, make_select );
	else
	    gotoLocation( markerColumn(), QMIN( 0x7FFF, markerRow() + 10 ), 0, make_select );

	return;

    default:
	
	// No null character ...
	if ( _ev->ascii() == 0 || !m_pView->koDocument()->isReadWrite() )
        {
	    _ev->accept();
	    return;
	}

	if ( !m_pEditor && !m_bChoose )
        {
	    if ( _ev->ascii() == '*' )
		createEditor( FormulaEditor );
	    else
	    {
		createEditor( CellEditor );
		m_pEditor->handleKeyPressEvent( _ev );	
	    }
	}
	else if ( m_pEditor )
	  m_pEditor->handleKeyPressEvent( _ev );	

      return;
    }

    /**
     * Tell the KSpreadView event handler and enable
     * makro recording by the way.
     */
    // _ev->accept();

    // m_pView->eventKeyPressed( _ev, m_bChoose );
}

void KSpreadCanvas::deleteEditor()
{
    KSpreadCellEditor* tmp = m_pEditor;
    m_pEditor = 0;
    delete tmp;
	
    setFocus();

    m_pView->enableFormulaToolBar( TRUE );
}

void KSpreadCanvas::createEditor()
{
    KSpreadCell* cell = activeTable()->cellAt( markerColumn(), markerRow() );

    if ( cell && cell->content() == KSpreadCell::VisualFormula )
    {
	QString tmp = cell->text();
	createEditor( FormulaEditor );
	m_pEditor->setText( tmp.right( tmp.length() - 1 ) );
	// m_pView->showFormulaToolBar( true );
    }
    else
    {
	createEditor( CellEditor );
	if ( cell )
        {
	    QString tmp = cell->text();
	    m_pEditor->setText(tmp);	
	}
    }
}

void KSpreadCanvas::createEditor( EditorType ed )
{
    KSpreadTable *table = activeTable();
    if ( !m_pEditor )
    {
      	KSpreadCell* cell = activeTable()->cellAt( marker() );
      	if ( ed == CellEditor )
	{
	    m_pView->enableFormulaToolBar( FALSE );
	
	    m_pEditor = new KSpreadTextEditor( cell, this );
	}
	else if( ed == FormulaEditor )
        {
	    m_pView->enableFormulaToolBar( TRUE );

	    m_pEditor = new KSpreadFormulaEditor( cell, this );
	}	
	
	int w, h;
	int min_w = cell->width( m_iMarkerColumn, this );
	int min_h = cell->height( m_iMarkerRow, this );
	if ( cell->isDefault() )
        {
	    w = min_w;
	    h = min_h;
	    qDebug("DEFAULT");
	}
	else
        {
	    w = cell->extraWidth() + 1;
	    h = cell->extraHeight() + 1;
	    qDebug("HEIGHT=%i EXTRA=%i", min_h, h );
	}
	int xpos = table->columnPos( markerColumn(), this );
	int ypos = table->rowPos( markerRow(), this );
	QPalette p = m_pEditor->palette();
	QColorGroup g( p.normal() );
	g.setColor( QColorGroup::Text, cell->textPen().color() );
	g.setColor( QColorGroup::Background, cell->KSpreadLayout::bgColor() );
	m_pEditor->setPalette( QPalette( g, p.disabled(), g ) );
	m_pEditor->setFont( cell->textFont() );
	m_pEditor->setGeometry( xpos, ypos, w, h );
	m_pEditor->setMinimumSize( QSize( min_w, min_h ) );
	m_pEditor->show();
	qDebug("FOCUS1");
	m_pEditor->setFocus();
	qDebug("FOCUS2");
    }
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadCanvas::updateCellRect( const QRect &_rect )
{
    qDebug("======================= UPDATE RECT %i,%i %i,%i==================",
	   _rect.x(), _rect.y(), _rect.width(), _rect.height() );

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

    int bottom, right;
    int left = table->columnPos( param.left() );
    int top = table->rowPos( param.top() );
    if ( param.right() < 0x7fff )
	right = left + table->columnPos( param.right() + 5 );
    else
	right = left + int( (double)width() / m_pView->xScaling() );
    if ( param.bottom() < 0x7fff )
	bottom = top + table->rowPos( param.bottom() + 5 );
    else
	bottom = top + int( (double)height() / m_pView->yScaling() );

    QPoint tl( left, top );
    QPoint br( right, bottom );
    QWMatrix m = m_pView->matrix();
    tl = m.map( tl );
    br = m.map( br );

    QPaintEvent event( QRect( tl, br ) );
    paintEvent( &event );
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

void KSpreadCanvas::updateChooseMarker( const QRect& _old, const QRect& _new )
{
    if ( isChooseMarkerVisible() )
    {
	drawChooseMarker( _old );
	drawChooseMarker( _new );
    }
    else
    {
	choose_visible = TRUE;
	drawChooseMarker( _new );
    }

    if ( _new.left() == 0 || !m_bChoose || !m_pEditor )
    {
	qDebug("updateChooseMarker len=0");
	length_namecell = 0;
	return;
    }

    KSpreadTable* table = activeTable();

    // ##### Torben: Clean up here!
    QString name_cell;

    kDebugInfo( "%s,%s", m_chooseStartTable->tableName().ascii(), table->tableName().ascii());
    if( m_chooseStartTable != table )
    {
	if ( _new.left() >= _new.right() && _new.top() >= _new.bottom() )
	    name_cell = util_cellName( table, _new.left(), _new.top() );
	else
	    name_cell = util_rangeName( table, _new );
    }
    else
    {
	if ( _new.left() >= _new.right() && _new.top() >= _new.bottom() )
	    name_cell = util_cellName( _new.left(), _new.top() );
	else
	    name_cell = util_rangeName( _new );
    }
  	
    int old = length_namecell;
    length_namecell= name_cell.length();
    length_text = m_pEditor->text().length();
    qDebug("updateChooseMarker2 len=%i", length_namecell);
	
    QString text = m_pEditor->text();
    QString res = text.left( m_pEditor->cursorPosition() - old ) + name_cell + text.right( text.length() - m_pEditor->cursorPosition() );
    int pos = m_pEditor->cursorPosition() - old;

    ((KSpreadTextEditor*)m_pEditor)->blockCheckChoose( TRUE );
    m_pEditor->setText( res );
    ((KSpreadTextEditor*)m_pEditor)->blockCheckChoose( FALSE );
    m_pEditor->setCursorPosition( pos + length_namecell );
    qDebug("old=%i len=%i pos=%i", old, length_namecell, pos );
    // m_pEditor->setText(m_pEditor->text().left(length_text-old)+ name_cell);
	
    /*
    //####### Torben: That looks too hacky
    if( m_pEditor->text().right(1)=="=")
    {
	m_pEditor->setText(m_pEditor->text()+ name_cell);
    }
    else
    {
	//######### TODO: TORBEN: There are more operators!!!
 	if( (m_pEditor->text().right(1)!="+") && (m_pEditor->text().right(1)!="-") &&
	    (m_pEditor->text().right(1)!="*") &&(m_pEditor->text().right(1)!="/"))
        {
	    m_pEditor->setText(m_pEditor->text().left(length_text-old)+ name_cell);
	}
	else
        {
	    m_pEditor->setText(m_pEditor->text()+ name_cell);
	}
    }
    */
    // m_pEditor->setFocus();
}

void KSpreadCanvas::drawChooseMarker()
{
    drawChooseMarker( activeTable()->chooseRect() );
}

void KSpreadCanvas::drawChooseMarker( const QRect& selection )
{
    // Draw nothing if the selection is empty
    if ( selection.left() == 0 )
	return;

    QPainter painter;
    painter.begin( this );

    int xpos;
    int ypos;
    int w, h;

    if ( selection.left() == 0 || selection.right() == 0x7fff || selection.bottom() == 0x7fff )
    {
	xpos = activeTable()->columnPos( chooseMarkerColumn(), this );
	ypos = activeTable()->rowPos( chooseMarkerRow(), this );
	KSpreadCell *cell = activeTable()->cellAt( chooseMarkerColumn(), chooseMarkerRow() );
	w = cell->width( chooseMarkerColumn(), this );
	h = cell->height( chooseMarkerRow(), this );
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

    RasterOp rop = painter.rasterOp();

    painter.setRasterOp( NotROP );
    QPen pen;
    pen.setWidth( 2 );
    pen.setStyle(DashLine);
    painter.setPen( pen );

    painter.drawLine( xpos - 2, ypos - 1, xpos + w + 2, ypos - 1 );
    painter.drawLine( xpos - 1, ypos + 1, xpos - 1, ypos + h + 3 );
    painter.drawLine( xpos + 1, ypos + h + 1, xpos + w - 3, ypos + h + 1 );
    painter.drawLine( xpos + w, ypos + 1, xpos + w, ypos + h - 2 );
    // painter.fillRect( xpos + w - 2, ypos + h - 1, 5, 5, black );
    painter.setRasterOp( rop );

    painter.end();
}

void KSpreadCanvas::setChooseMarker( const QPoint& p )
{
    if ( p.x() == m_i_chooseMarkerColumn && p.y() == m_i_chooseMarkerRow )
	return;

    m_i_chooseMarkerRow = p.y();
    m_i_chooseMarkerColumn = p.x();
    // This will trigger a redraw
    activeTable()->setChooseRect( QRect( p.x(), p.y(), 1, 1 ) );
}

void KSpreadCanvas::setMarker( const QPoint& p )
{
    if ( p.x() == m_iMarkerColumn && p.y() == m_iMarkerRow )
	return;

    if ( m_iMarkerVisible < 1 )
    {
	m_iMarkerRow = p.y();
	m_iMarkerColumn = p.x();
	return;
    }

    hideMarker();
    m_iMarkerRow = p.y();
    m_iMarkerColumn = p.x();
    showMarker();
}

void KSpreadCanvas::setMarkerColumn( int _c )
{
    if ( !m_iMarkerVisible < 1 )
    {
	m_iMarkerColumn = _c;
	return;
    }

    hideMarker();
    m_iMarkerColumn = _c;
    showMarker();
}

void KSpreadCanvas::setMarkerRow( int _r )
{
    if ( !m_iMarkerVisible < 1 )
    {
	m_iMarkerRow = _r;
	return;
    }

    hideMarker();
    m_iMarkerRow = _r;
    showMarker();
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

// Torben: ########### Not needed any more

void KSpreadCanvas::drawCell( KSpreadCell *_cell, int _col, int _row )
{
    int left = activeTable()->columnPos( _col ) - m_iXOffset;
    int top = activeTable()->rowPos( _row ) - m_iYOffset;
    int right = left + _cell->extraWidth() - m_iXOffset;
    int bottom = top + _cell->extraHeight() - m_iYOffset;

    qDebug("left=%i right=%i extra=%i", left, right, _cell->extraWidth() );

    QPoint tl( left, top );
    QPoint br( right, bottom );
    QWMatrix m = m_pView->matrix();
    tl = m.map( tl );
    br = m.map( br );

    QPaintEvent event( QRect( tl, br ) );
    paintEvent( &event );
}

void KSpreadCanvas::adjustArea()
{
    QRect selection( activeTable()->selectionRect() );
    // Columns selected
    if( selection.left() != 0 && selection.bottom() == 0x7FFF )
    {
	for (int x=selection.left(); x <= selection.right(); x++ )
        {
	    hBorderWidget()->adjustColumn(x);
	}
    }
    // Rows selected
    else if(selection.left() != 0 && selection.right() == 0x7FFF )
    {
	for(int y = selection.top(); y <= selection.bottom(); y++ )
        {
	    vBorderWidget()->adjustRow(y);
	}
    }
    // No selection
    else if( selection.left() == 0 || selection.top() == 0 ||
	     selection.bottom() == 0 || selection.right() == 0 )
    {
	vBorderWidget()->adjustRow(markerRow());
	hBorderWidget()->adjustColumn(markerColumn());
    }
    // Selection of a rectangualar area
    else
    {
	for (int x=selection.left(); x <= selection.right(); x++ )
        {
	    hBorderWidget()->adjustColumn(x);
	}
	for(int y = selection.top(); y <= selection.bottom(); y++ )
        {
	    vBorderWidget()->adjustRow(y);
	}
    }
}

/****************************************************************
 *
 * KSpreadVBorder
 *
 ****************************************************************/

KSpreadVBorder::KSpreadVBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view)
    : QWidget( _parent, "", WNorthWestGravity )
{

  m_pView = _view;
  m_pCanvas = _canvas;

  setBackgroundMode( PaletteBackground );
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

    if ( _ev->button() == RightButton )
  	{
	QPoint p = mapToGlobal( _ev->pos() );
    	m_pView->popupRowMenu( p );
  	m_bSelection=FALSE;
  	}
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
    if (( m_pCanvas->zoom() * (float)( _ev->pos().y() - y ) ) < 20.0 )
    //_ev->pos().y() < 20 )
      rl->setHeight( 20, m_pCanvas );
    else
      rl->setHeight( _ev->pos().y() - y, m_pCanvas );
  }

  m_bSelection = FALSE;
  m_bResize = FALSE;
}

void KSpreadVBorder::adjustRow(int _row)
{
int adjust;
int select;
if(_row==-1)
	{
	adjust=m_pCanvas->activeTable()->adjustRow(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
	select=m_iSelectionAnchor;
	}
else
	{
	adjust=m_pCanvas->activeTable()->adjustRow(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),_row);
	select=_row;
	}
if(adjust!=-1)
	{
	KSpreadTable *table = m_pCanvas->activeTable();
	assert( table );
	RowLayout *rl = table->nonDefaultRowLayout( select );
	adjust=QMAX(20,adjust);
	rl->setHeight(adjust,m_pCanvas);
	}
}


void KSpreadVBorder::resizeRow(int resize,int nb  )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    ASSERT( table );
if(nb==-1)
	{
    	RowLayout *rl = table->nonDefaultRowLayout( m_iSelectionAnchor );
    	resize = QMAX( 20, resize );
    	rl->setHeight( resize, m_pCanvas );
	}
else
	{
	QRect selection( table->selectionRect() );
	if(selection.bottom()==0 ||selection.top()==0 || selection.left()==0
	|| selection.right()==0)
		{
		RowLayout *rl = table->nonDefaultRowLayout( m_pCanvas->markerRow() );
		resize=QMAX(20, resize);
		rl->setHeight( resize, m_pCanvas );
		}
	else
		{
		RowLayout *rl;
		for (int i=selection.top();i<=selection.bottom();i++)
			{
			rl= table->nonDefaultRowLayout( i );
			resize=QMAX(20, resize);
			rl->setHeight( resize, m_pCanvas );
			}
		}
	}

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
//       static QColorGroup g2( black, white, white, darkGray, lightGray, black, black );
	static QBrush fill2( black );
	qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pCanvas ), colorGroup(), FALSE, 1, &fill2 );
    }
    else
    {
//       static QColorGroup g( black, white, white, darkGray, lightGray, black, black );
	static QBrush fill( colorGroup().brush( QColorGroup::Background ) );
	qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pCanvas ), colorGroup(), FALSE, 1, &fill );
    }
	
    char buffer[ 20 ];
    sprintf( buffer, "%i", y );

    if ( selected )
      painter.setPen( white );
    else
      painter.setPen( colorGroup().text() );

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

KSpreadHBorder::KSpreadHBorder( QWidget *_parent, KSpreadCanvas *_canvas,KSpreadView *_view )
    : QWidget( _parent, "", WNorthWestGravity )
{
  m_pView = _view;
  m_pCanvas = _canvas;

  setBackgroundMode( PaletteBackground );
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
    if ( _ev->button() == RightButton )
  	{
	QPoint p = mapToGlobal( _ev->pos() );
    	m_pView->popupColumnMenu( p );
  	m_bSelection=FALSE;
  	}
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

void KSpreadHBorder::adjustColumn(int _col)
{
    int adjust;
    int select;
    if( _col==-1 )
    {
	adjust = m_pCanvas->activeTable()->adjustColumn(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
	select=m_iSelectionAnchor;
    }
    else
    {
	adjust=m_pCanvas->activeTable()->adjustColumn(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),_col);
	select=_col;
    }

    if(adjust!=-1)
    {
	KSpreadTable *table = m_pCanvas->activeTable();
	assert( table );
	ColumnLayout *cl = table->nonDefaultColumnLayout( select );
	
	adjust = QMAX( 20, adjust );
    	cl->setWidth( adjust, m_pCanvas );
    }
}


void KSpreadHBorder::resizeColumn(int resize,int nb  )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    ASSERT( table );
    if( nb == -1 )
    {
    	ColumnLayout *cl = table->nonDefaultColumnLayout( m_iSelectionAnchor );
	resize = QMAX( 20, resize );
    	cl->setWidth( resize, m_pCanvas );
    }
    else
    {
	QRect selection( table->selectionRect() );
	if( selection.bottom() == 0 || selection.top() == 0 || selection.left() == 0 ||
	    selection.right() == 0 )
        {
	    ColumnLayout *cl = table->nonDefaultColumnLayout( m_pCanvas->markerColumn() );
		
	    resize = QMAX( 20, resize );
	    cl->setWidth( resize, m_pCanvas );
	}
	else
        {
	    ColumnLayout *cl;
	    for (int i=selection.left();i<=selection.right();i++)
	    {
		cl= table->nonDefaultColumnLayout( i );
			
		resize = QMAX( 20, resize );
		cl->setWidth( resize, m_pCanvas );
	    }
	}
    }
}

void KSpreadHBorder::mouseDoubleClickEvent( QMouseEvent * _ev )
{
	KSpreadTable *table = m_pCanvas->activeTable();
	assert( table );

	int x = 0;
	int col = table->leftColumn( 0, x, m_pCanvas );
	
	while ( x < width() )
	{
		int w = table->columnLayout( col )->width( m_pCanvas );
		col++;
		if ( _ev->pos().x() >= x + w - 1 && _ev->pos().x() <= x + w + 1 )
		{
			m_bSelection = TRUE;
			table->unselect();
			m_iSelectionAnchor = --col;	
			QRect r;
			r.setCoords( col, 1, col, 0x7FFF );
			table->setSelection( r, m_pCanvas );
			m_bSelection = FALSE;
			adjustColumn();
			return;
		}
		x += w;
    }
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
//       static QColorGroup g2( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill2( black );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ), XBORDER_HEIGHT, colorGroup(), FALSE, 1, &fill2 );
    }
    else
    {
//       static QColorGroup g( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ), XBORDER_HEIGHT, colorGroup(), FALSE, 1, &fill );
    }

    int len = painter.fontMetrics().width( table->columnLabel(x) );

    if ( selected )
      painter.setPen( white );
    else
      painter.setPen( colorGroup().text() );
	
    painter.drawText( xpos + ( col_lay->width( m_pCanvas ) - len ) / 2,
		      ( XBORDER_HEIGHT + painter.fontMetrics().ascent() - painter.fontMetrics().descent() ) / 2,
		      table->columnLabel(x) );

    xpos += col_lay->width( m_pCanvas );
	}

  painter.end();
}

#include "kspread_canvas.moc"
