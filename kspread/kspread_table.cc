/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <qmsgbox.h>
#include <qpainter.h>
#include <qdrawutl.h>
#include <qkeycode.h>
#include <qregexp.h>
#include <qpoint.h>
#include <qprinter.h>
#include <qcursor.h>
#include <qstack.h>
#include <qbuffer.h>
#include <qmsgbox.h>
#include <qclipbrd.h>
#include <qpicture.h>
#include <klocale.h>

#include "kspread_table.h"
#include "kspread_view.h"
#include "kspread_undo.h"
#include "kspread_map.h"
#include "kspread_util.h"
#include "kspread_corba_util.h"
#include "kspread_canvas.h"

#include <koStream.h>

#include <komlWriter.h>
#include <komlParser.h>
#include <komlStreamFeed.h>
#include <torben.h>

#include <strstream.h>
#include <kautoarray.h>

/*****************************************************************************
 *
 * CellBinding
 *
 *****************************************************************************/

CellBinding::CellBinding( KSpreadTable *_table, const QRect& _area )
{
  m_rctDataArea = _area;

  m_pTable = _table;
  m_pTable->addCellBinding( this );

  m_bIgnoreChanges = false;
}

CellBinding::~CellBinding()
{
  m_pTable->removeCellBinding( this );
}

void CellBinding::cellChanged( KSpreadCell *_cell )
{
  if ( m_bIgnoreChanges )
    return;

  emit changed( _cell );
}

bool CellBinding::contains( int _x, int _y )
{
  return m_rctDataArea.contains( QPoint( _x, _y ) );
}

/*****************************************************************************
 *
 * ChartBinding
 *
 *****************************************************************************/

ChartBinding::ChartBinding( KSpreadTable *_table, const QRect& _area, ChartChild *_child )
  : CellBinding( _table, _area )
{
  m_pChild = _child;
}

ChartBinding::~ChartBinding()
{
}

void ChartBinding::cellChanged( KSpreadCell* )
{
  cout << "######### void ChartBinding::cellChanged( KSpreadCell* )" << endl;

  if ( m_bIgnoreChanges || CORBA::is_nil( m_vChart ) )
    return;

  cout << "with=" << m_rctDataArea.width() << "  height=" << m_rctDataArea.height() << endl;

  Chart::Matrix matrix;
  matrix.columns = m_rctDataArea.width() - 1;
  matrix.rows = m_rctDataArea.height() - 1;

  matrix.matrix.length( ( m_rctDataArea.width() - 1 ) * ( m_rctDataArea.height() - 1 ) );
  for ( int y = 0; y < m_rctDataArea.height() - 1; y++ )
    for ( int x = 0; x < m_rctDataArea.width() - 1; x++ )
      matrix.matrix[ y * ( m_rctDataArea.width() - 1 ) + x ] =
	m_pTable->cellAt( m_rctDataArea.left() + x + 1, m_rctDataArea.top() + y + 1 )->valueDouble();

  matrix.columnDescription.length( m_rctDataArea.width() - 1 );
  int i = 0;
  for ( i = 0; i < m_rctDataArea.width() - 1; i++ )
    matrix.columnDescription[i] =
      CORBA::string_dup( m_pTable->cellAt( m_rctDataArea.left() + i + 1, m_rctDataArea.top() )->valueString() );

  matrix.rowDescription.length( m_rctDataArea.height() - 1 );
  for ( i = 0; i < m_rctDataArea.height() - 1; i++ )
    matrix.rowDescription[i] =
      CORBA::string_dup( m_pTable->cellAt( m_rctDataArea.left(), m_rctDataArea.top() + i + 1 )->valueString() );

  Chart::Range range;
  range.top = m_rctDataArea.top();
  range.left = m_rctDataArea.left();
  range.right = m_rctDataArea.right();
  range.bottom = m_rctDataArea.bottom();
  range.table = CORBA::string_dup( m_pTable->name() );

  m_vChart->fill( range, matrix );
}

/*****************************************************************************
 *
 * KSpreadTable
 *
 *****************************************************************************/

int KSpreadTable::s_id = 0L;
QIntDict<KSpreadTable>* KSpreadTable::s_mapTables;

KSpreadTable* KSpreadTable::find( int _id )
{
  if ( !s_mapTables )
    return 0L;

  return (*s_mapTables)[ _id ];
}

KSpreadTable::KSpreadTable( KSpreadDoc *_doc, const char *_name )
{
  if ( s_mapTables == 0L )
    s_mapTables = new QIntDict<KSpreadTable>;
  m_id = s_id++;
  s_mapTables->insert( m_id, this );

  m_pDoc = _doc;

  m_bShowPageBorders = FALSE;

  m_lstCellBindings.setAutoDelete( FALSE );

  m_strName = _name;

  m_lstChildren.setAutoDelete( true );

  m_dctCells.setAutoDelete( true );
  m_dctRows.setAutoDelete( true );
  m_dctColumns.setAutoDelete( true );

  m_pDefaultCell = new KSpreadCell( this, 0, 0 );
  m_pDefaultRowLayout = new RowLayout( this, 0 );
  m_pDefaultRowLayout->setDefault();
  m_pDefaultColumnLayout = new ColumnLayout( this, 0 );
  m_pDefaultColumnLayout->setDefault();

  // No selection is active
  m_rctSelection.setCoords( 0, 0, 0, 0 );

  m_pWidget = new QWidget();
  m_pPainter = new QPainter;
  m_pPainter->begin( m_pWidget );

  m_iMaxColumn = 256;
  m_iMaxRow = 256;
  m_bScrollbarUpdates = true;
  setSort(false);
  initInterpreter();
}

//-----------------------------------------------------------
// CORBA Interface for KSpread functionality
//-----------------------------------------------------------

KSpread::Book_ptr KSpreadTable::book()
{
  return KSpread::Book::_duplicate( m_pMap );
}

KSpread::Range* KSpreadTable::range( CORBA::ULong left, CORBA::ULong top,
				    CORBA::ULong right, CORBA::ULong bottom )
{
  KSpread::Range* r = new KSpread::Range;
  r->table = CORBA::string_dup( m_strName.data() );
  r->left = left;
  r->top = top;
  r->right = right;
  r->bottom = bottom;

  return r;
}

KSpread::Range* KSpreadTable::rangeFromString( const char* str )
{
  KSpread::Range* r = new KSpread::Range;
  *r = util_parseRange( str );
  r->table = CORBA::string_dup( m_strName.data() );

  return r;
}

KSpread::Range* KSpreadTable::rangeFromCells( const KSpread::Cell& topleft,
					     const KSpread::Cell& bottomright )
{
  if ( strcmp( topleft.table.in(), bottomright.table.in() ) != 0 )
  {
    KSpread::DifferentTables exc;
    exc.table1 = CORBA::string_dup( topleft.table.in() );
    exc.table2 = CORBA::string_dup( bottomright.table.in() );
    mico_throw( exc );
  }

  KSpread::Range* r = new KSpread::Range;
  r->left = topleft.x;
  r->top = topleft.y;
  r->right = bottomright.x;
  r->bottom = bottomright.y;
  r->table = CORBA::string_dup( topleft.table.in() );

  return r;
}

KSpread::Cell* KSpreadTable::cellFromString( const char* str )
{
  KSpread::Cell* c = new KSpread::Cell;
  *c = util_parseCell( str );
  c->table = CORBA::string_dup( m_strName.data() );

  return c;
}

void KSpreadTable::setValue( const KSpread::Cell& cell, CORBA::Double value )
{
  if ( m_strName != cell.table.in() )
  {
    KSpread::DifferentTables exc;
    exc.table1 = CORBA::string_dup( cell.table.in() );
    exc.table2 = CORBA::string_dup( m_strName.data() );
    mico_throw( exc );
  }

  KSpreadCell* c = nonDefaultCell( cell.x, cell.y );
  c->setValue( value );
}

void KSpreadTable::setStringValue( const KSpread::Cell& cell, const char* value )
{
  if ( m_strName != cell.table.in() )
  {
    KSpread::DifferentTables exc;
    exc.table1 = CORBA::string_dup( cell.table.in() );
    exc.table2 = CORBA::string_dup( m_strName.data() );
    mico_throw( exc );
  }

  KSpreadCell* c = nonDefaultCell( cell.x, cell.y );
  c->setText( value );
}

CORBA::Double KSpreadTable::value( const KSpread::Cell& cell )
{
  if ( m_strName != cell.table.in() )
  {
    KSpread::DifferentTables exc;
    exc.table1 = CORBA::string_dup( cell.table.in() );
    exc.table2 = CORBA::string_dup( m_strName.data() );
    mico_throw( exc );
  }

  KSpreadCell* c = cellAt( cell.x, cell.y );
  if ( !c->isValue() )
  {
    KSpread::NoNumericValue exc;
    exc.cell = cell;
    mico_throw( exc );
  }

  return c->valueDouble();
}

char* KSpreadTable::stringValue( const KSpread::Cell& cell )
{
  if ( m_strName != cell.table.in() )
  {
    KSpread::DifferentTables exc;
    exc.table1 = CORBA::string_dup( cell.table.in() );
    exc.table2 = CORBA::string_dup( m_strName.data() );
    mico_throw( exc );
  }

  KSpreadCell* c = cellAt( cell.x, cell.y );
  return CORBA::string_dup( c->valueString() );
}

void KSpreadTable::setSelection( const KSpread::Range& sel )
{
  if ( m_strName != sel.table.in() )
  {
    KSpread::DifferentTables exc;
    exc.table1 = CORBA::string_dup( sel.table.in() );
    exc.table2 = CORBA::string_dup( m_strName.data() );
    mico_throw( exc );
  }

  m_rctSelection.setCoords( sel.left, sel.top, sel.right, sel.bottom );
}

KSpread::Range* KSpreadTable::selection()
{
  KSpread::Range* r = new KSpread::Range;
  r->table = CORBA::string_dup( m_strName.data() );
  r->left = m_rctSelection.left();
  if ( r->left == 0 )
  {
    r->top = 0;
    r->bottom = 0;
    r->right = 0;
  }
  else
  {
    r->top = m_rctSelection.top();
    r->right = m_rctSelection.right();
    r->bottom = m_rctSelection.bottom();
  }

  return r;
}

void KSpreadTable::copySelection()
{
  if ( m_rctSelection.left() == 0 )
  {
    KSpread::NoActiveSelection exc;
    mico_throw( exc );
  }

  QPoint p( 0, 0 );
  copySelection( p );
}

void KSpreadTable::cutSelection()
{
  if ( m_rctSelection.left() == 0 )
  {
    KSpread::NoActiveSelection exc;
    mico_throw( exc );
  }

  QPoint p( 0, 0 );
  cutSelection( p );
}

void KSpreadTable::pasteSelection( const KSpread::Cell& cell )
{
  QPoint p( cell.x, cell.y );

  paste( p );
}

CORBA::Boolean KSpreadTable::isEmpty( CORBA::ULong x, CORBA::ULong y )
{
  KSpreadCell* c = cellAt( x, y );
  if ( !c || c->isEmpty() )
    return true;

  return false;
}

//-----------------------------------------------------------

ColumnLayout* KSpreadTable::columnLayout( int _column )
{
    ColumnLayout *p = m_dctColumns[ _column ];
    if ( p != 0L )
	return p;

    return m_pDefaultColumnLayout;
}

RowLayout* KSpreadTable::rowLayout( int _row )
{
    RowLayout *p = m_dctRows[ _row ];
    if ( p != 0L )
	return p;

    return m_pDefaultRowLayout;
}

int KSpreadTable::leftColumn( int _xpos, int &_left, KSpreadCanvas *_canvas )
{
    _xpos += _canvas->xOffset();
    _left = -_canvas->xOffset();

    int col = 1;
    int x = columnLayout( col )->width( _canvas );
    while ( x < _xpos )
    {
	// Should never happen
	if ( col == 0x10000 )
	    return 1;
	_left += columnLayout( col )->width( _canvas );
	col++;
	x += columnLayout( col )->width( _canvas );
    }

    return col;
}

int KSpreadTable::rightColumn( int _xpos, KSpreadCanvas *_canvas )
{
    _xpos += _canvas->xOffset();

    int col = 1;
    int x = 0;
    while ( x < _xpos )
    {
	// Should never happen
	if ( col == 0x10000 )
	    return 0x10000;
	x += columnLayout( col )->width( _canvas );
	col++;
    }

    return col;
}

int KSpreadTable::topRow( int _ypos, int & _top, KSpreadCanvas *_canvas )
{
    _ypos += _canvas->yOffset();
    _top = -_canvas->yOffset();

    int row = 1;
    int y = rowLayout( row )->height( _canvas );
    while ( y < _ypos )
    {
	// Should never happen
	if ( row == 0x10000 )
	    return 1;
	_top += rowLayout( row )->height( _canvas );
	row++;
	y += rowLayout( row )->height( _canvas);
    }

    return row;
}

int KSpreadTable::bottomRow( int _ypos, KSpreadCanvas *_canvas )
{
    _ypos += _canvas->yOffset();

    int row = 1;
    int y = 0;
    while ( y < _ypos )
    {
	// Should never happen
	if ( row == 0x10000 )
	    return 0x10000;
	y += rowLayout( row )->height( _canvas );
	row++;
    }

    return row;
}

int KSpreadTable::columnPos( int _col, KSpreadCanvas *_canvas )
{
    int col = 1;
    int x = 0;
    if ( _canvas )
      x -= _canvas->xOffset();
    while ( col < _col )
    {
	// Should never happen
	if ( col == 0x10000 )
	    return x;
	
	x += columnLayout( col )->width( _canvas );
	col++;
    }

    return x;
}

int KSpreadTable::rowPos( int _row, KSpreadCanvas *_canvas )
{
    int row = 1;
    int y = 0;
    if ( _canvas )
      y -= _canvas->yOffset();
    while ( row < _row )
    {
	// Should never happen
	if ( row == 0x10000 )
	    return y;
	
	y += rowLayout( row )->height( _canvas );
	row++;
    }

    return y;
}

KSpreadCell* KSpreadTable::visibleCellAt( int _column, int _row, bool _no_scrollbar_update )
{
  KSpreadCell* cell = cellAt( _column, _row, _no_scrollbar_update );
  if ( cell->isObscured() )
    return cellAt( cell->obscuringCellsColumn(), cell->obscuringCellsRow(), _no_scrollbar_update );

  return cell;
}

KSpreadCell* KSpreadTable::cellAt( int _column, int _row, bool _no_scrollbar_update )
{
  if ( !_no_scrollbar_update && m_bScrollbarUpdates )
  {
    if ( _column > m_iMaxColumn )
    {
      m_iMaxColumn = _column;
      emit sig_maxColumn( _column );
    }
    if ( _row > m_iMaxRow )
    {
      m_iMaxRow = _row;
      emit sig_maxRow( _row );
    }
  }

  int i = _row + ( _column * 0x10000 );

  KSpreadCell *p = m_dctCells[ i ];
  if ( p != 0L )
    return p;

  return m_pDefaultCell;
}

ColumnLayout* KSpreadTable::nonDefaultColumnLayout( int _column )
{
    ColumnLayout *p = m_dctColumns[ _column ];
    if ( p != 0L )
	return p;
	
    p = new ColumnLayout( this, _column );
    p->setWidth( m_pDefaultColumnLayout->width() );
    m_dctColumns.insert( _column, p );

    return p;
}

RowLayout* KSpreadTable::nonDefaultRowLayout( int _row )
{
    RowLayout *p = m_dctRows[ _row ];
    if ( p != 0L )
	return p;
	
    p = new RowLayout( this, _row );
    // TODO: copy the default RowLayout here!!
    p->setHeight( m_pDefaultRowLayout->height() );
    m_dctRows.insert( _row, p );

    return p;
}

KSpreadCell* KSpreadTable::nonDefaultCell( int _column, int _row,
					   bool _no_scrollbar_update )
{
  if ( !_no_scrollbar_update && m_bScrollbarUpdates )
  {
    if ( _column > m_iMaxColumn )
    {
      m_iMaxColumn = _column;
      emit sig_maxColumn( _column );
    }
    if ( _row > m_iMaxRow )
    {
      m_iMaxRow = _row;
      emit sig_maxRow( _row );
    }
  }

  int key = _row + ( _column * 0x10000 );

  KSpreadCell *p = m_dctCells[ key ];
  if ( p != 0L )
    return p;

  KSpreadCell *cell = new KSpreadCell( this, _column, _row );
  m_dctCells.insert( key, cell );

  return cell;
}

void KSpreadTable::setText( int _row, int _column, const QString& _text )
{
    m_pDoc->setModified( true );

    KSpreadCell *cell = nonDefaultCell( _column, _row );

    KSpreadUndoSetText *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
	undo = new KSpreadUndoSetText( m_pDoc, this, cell->text(), _column, _row );
	m_pDoc->undoBuffer()->appendUndo( undo );
    }

    // The cell will force a display refresh itself, so we dont have to care here.
    cell->setText( _text );
}

void KSpreadTable::setLayoutDirtyFlag()
{
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    for ( ; it.current(); ++it )
	it.current()->setLayoutDirtyFlag();
}

void KSpreadTable::setCalcDirtyFlag()
{
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    for ( ; it.current(); ++it )
      it.current()->setCalcDirtyFlag();
}

void KSpreadTable::recalc()
{
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    for ( ; it.current(); ++it )
      it.current()->calc();
}

void KSpreadTable::unselect()
{
    if ( m_rctSelection.left() == 0 )
	return;

    QRect r = m_rctSelection;
    // Discard the selection
    m_rctSelection.setCoords( 0, 0, 0, 0 );

    emit sig_unselect( this, r );
}

void KSpreadTable::setSelection( const QRect &_sel, KSpreadCanvas *_canvas )
{
  if ( _sel == m_rctSelection )
    return;

  // We want to see whether a single cell was clicked like a button.
  // This is only of interest if no cell was selected before
  if ( _sel.left() == 0 )
  {
    // So we test first whether only a single cell was selected
    KSpreadCell *cell = cellAt( m_rctSelection.left(), m_rctSelection.top() );
    // Did we mark only a single cell ?
    // Take care: One cell may obscure other cells ( extra size! ).
    if ( m_rctSelection.left() + cell->extraXCells() == m_rctSelection.right() &&
	 m_rctSelection.top() + cell->extraYCells() == m_rctSelection.bottom() )
      cell->clicked( _canvas );
  }

  QRect old( m_rctSelection );
  m_rctSelection = _sel;

  emit sig_changeSelection( this, old, m_rctSelection );
}

void KSpreadTable::setSelectionFont( const QPoint &_marker, const char *_font, int _size,
				     signed char _bold, signed char _italic )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  if ( _font )
	    it.current()->setTextFontFamily( _font );
	  if ( _size > 0 )
	    it.current()->setTextFontSize( _size );
	  if ( _italic >= 0 )
	    it.current()->setTextFontItalic( !it.current()->textFontItalic() );
	  if ( _bold >= 0 )
	    it.current()->setTextFontBold( !it.current()->textFontBold() );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  if ( _font )
	    it.current()->setTextFontFamily( _font );
	  if ( _size > 0 )
	    it.current()->setTextFontSize( _size );
	  if ( _italic >= 0 )
	    it.current()->setTextFontItalic( !it.current()->textFontItalic() );
	  if ( _bold >= 0 )
	    it.current()->setTextFontBold( !it.current()->textFontBold() );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
	
	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}

	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
	      KSpreadCell *cell = cellAt( x, y );
	
	      if ( cell == m_pDefaultCell )
	      {
		cell = new KSpreadCell( this, x, y );
		int key = y + ( x * 0x10000 );
		m_dctCells.insert( key, cell );
	      }

	      cell->setDisplayDirtyFlag();
		
	      if ( _font )
		cell->setTextFontFamily( _font );
	      if ( _size > 0 )
		cell->setTextFontSize( _size );
	      if ( _italic >= 0 )
		cell->setTextFontItalic( !cell->textFontItalic() );
	      if ( _bold >= 0 )
		cell->setTextFontBold( !cell->textFontBold() );

		cell->clearDisplayDirtyFlag();
	    }
	
	emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionTextColor( const QPoint &_marker, QColor tb_Color )
{
m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setTextColor(tb_Color);
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setTextColor(tb_Color);
	
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );
		
		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();
		cell-> setTextColor(tb_Color);
		cell->clearDisplayDirtyFlag();
	    }
	
	emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionbgColor( const QPoint &_marker, QColor bg_Color )
{
m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setBgColor(bg_Color);
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setBgColor(bg_Color);
	
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );
		
		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();
		cell-> setBgColor(bg_Color);
		cell->clearDisplayDirtyFlag();
	    }
	
	emit sig_updateView( this, r );
    }
}


void KSpreadTable::setSelectionPercent( const QPoint &_marker )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setFaktor( 100.0 );
	  it.current()->setPrecision( 0 );
	  it.current()->setPostfix( "%" );
	  it.current()->setPrefix( "" );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setFaktor( 100.0 );
	  it.current()->setPrecision( 0 );
	  it.current()->setPostfix( "%" );
	  it.current()->setPrefix( "" );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );
		
		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();
		cell->setFaktor( 100.0 );
		cell->setPrecision( 0 );
		cell->setPostfix( "%" );
		cell->setPrefix( "" );
		cell->clearDisplayDirtyFlag();
		//cout <<"value : "<<cell->text().ascii()<<endl;
	    }
	
	emit sig_updateView( this, r );
    }
}

bool KSpreadTable::replace( const QPoint &_marker,QString _find,QString _replace )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );
    bool b_replace=false;
    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  if(!it.current()->isValue() && !it.current()->isBool() &&!it.current()->isFormular() &&!it.current()->isDefault()&&!it.current()->text().isEmpty())
		{	
		if( it.current()->text().find(_find) >=0)
			{
			int index=0;
			int lenreplace=0;
			int lenfind=0;
			it.current()->setDisplayDirtyFlag();
			QString stmp=it.current()->text();
			b_replace=true;
			do
				{
				index=stmp.find(_find,index+lenreplace);
				int len=stmp.length();
				lenfind=_find.length();
				lenreplace=_replace.length();
				stmp=stmp.left(index)+_replace+stmp.right(len-index-lenfind);
				}
			while( stmp.find(_find,index+lenreplace) >=0);
			it.current()->setText(stmp);
			it.current()->clearDisplayDirtyFlag();
			}
		}
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return b_replace;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	   if(!it.current()->isValue() && !it.current()->isBool() &&!it.current()->isFormular() &&!it.current()->isDefault()&&!it.current()->text().isEmpty())
		{	
		if( it.current()->text().find(_find) >=0)
			{
			int index=0;
			int lenreplace=0;
			int lenfind=0;
			it.current()->setDisplayDirtyFlag();
			QString stmp=it.current()->text();
			b_replace=true;
			do
				{
				index=stmp.find(_find,index+lenreplace);
				int len=stmp.length();
				lenfind=_find.length();
				lenreplace=_replace.length();
				stmp=stmp.left(index)+_replace+stmp.right(len-index-lenfind);
				}
			while( stmp.find(_find,index+lenreplace) >=0);
			it.current()->setText(stmp);
			it.current()->clearDisplayDirtyFlag();
			}
		}
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return b_replace;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );

		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}
                if(!cell->isValue() && !cell->isBool() &&!cell->isFormular() &&!cell->isDefault()&&!cell->text().isEmpty())
		{	
		if( cell->text().find(_find) >=0)
			{
			cell->setDisplayDirtyFlag();
			QString stmp=cell->text();
			int index=0;
			int lenreplace=0;
			int lenfind=0;
			b_replace=true;
			do
				{
				index=stmp.find(_find,index+lenreplace);
				int len=stmp.length();
				lenfind=_find.length();
				lenreplace=_replace.length();
				
				stmp=stmp.left(index)+_replace+stmp.right(len-index-lenfind);
				
				}
			while( stmp.find(_find,index+lenreplace) >=0);
			cell->setText(stmp);
			cell->clearDisplayDirtyFlag();
			}
		}
	    }
	
	emit sig_updateView( this, r );
	return b_replace;
    }
}

void KSpreadTable::onlyRow(Mode_sort mode)
{
QRect r( selectionRect() );
setSort(true);
KSpreadCell *tmp = new KSpreadCell( this, -1, -1 );
for ( int d = r.left();  d<= r.right(); d++ )
	{
	KSpreadCell *cell1 = cellAt( d,r.top() );
	if(!cell1->isEmpty())
	{
	for ( int y = r.left(); y <= r.right(); y++ )
		{
		KSpreadCell *cell2 = cellAt( y,r.top());
		if(!cell2->isEmpty())
		{
		if(mode==Increase)
			{
			if(strcmp(cell2->text(), cell1->text())>0)
				{
				tmp->copyALL(cell1);
				cell1->copyALL(cell2);
				cell2->copyALL(tmp);
				}
			}
		else if(mode==Decrease)
			{
			 if(strcmp(cell2->text(), cell1->text())<0)
				{
				tmp->copyALL(cell1);
				cell1->copyALL(cell2);
				cell2->copyALL(tmp);
				}
			}
		else
			{
			cout <<"Err in Mode_sort\n";
			}
		}
		}
	}
	}
setSort(false);
emit sig_updateView( this, r );
}

void KSpreadTable::onlyColumn(Mode_sort mode)
{
setSort(true);
QRect r( selectionRect() );
KSpreadCell *tmp = new KSpreadCell( this, -1, -1 );
for ( int d = r.top();  d<= r.bottom(); d++ )
	{
	KSpreadCell *cell1 = cellAt( r.right(), d );
	if(!cell1->isEmpty())
	{
	for ( int y = r.top(); y <= r.bottom(); y++ )
		{
		KSpreadCell *cell2 = cellAt( r.right(), y );
		if(!cell2->isEmpty())
		{
		if(mode==Increase)
			{
			if(strcmp(cell2->text(), cell1->text())>0)
				{
				tmp->copyALL(cell1);
				cell1->copyALL(cell2);
				cell2->copyALL(tmp);
				}
			}
		else if(mode==Decrease)
			{
			 if(strcmp(cell2->text(), cell1->text())<0)
				{
				tmp->copyALL(cell1);
				cell1->copyALL(cell2);
				cell2->copyALL(tmp);
				}
			}
		else
			{
			cout <<"Err in Mode_sort\n";
			}
		}
		}
	}
	}
setSort(false);
emit sig_updateView( this, r );
}

void KSpreadTable::Row(int ref_row,Mode_sort mode)
{

QRect r( selectionRect() );
setSort(true);
KSpreadCell *tmp = new KSpreadCell( this, -1, -1 );
for ( int d = r.left();  d<= r.right(); d++ )
	{
	KSpreadCell *cell1 = cellAt( d,ref_row  );
	if(!cell1->isEmpty())
	{
	for ( int y = r.left(); y <= r.right(); y++ )
		{
		KSpreadCell *cell2 = cellAt( y,ref_row );
		if(!cell2->isEmpty())
		{
		if(mode==Increase)
			{
			if(strcmp(cell2->text(), cell1->text())>0)
				{
				for(int x=r.top();x<=r.bottom();x++)
					{
					KSpreadCell *ref1 = cellAt( d,x );
					KSpreadCell *ref2 = cellAt( y,x );
					if(!ref1->isEmpty()&&!ref2->isEmpty())
						{
						tmp->copyALL(ref1);
						ref1->copyALL(ref2);
						ref2->copyALL(tmp);
						}
					}
				}
			}
		else if(mode==Decrease)
			{
			if(strcmp(cell2->text(), cell1->text())<0)
				{
				for(int x=r.top();x<=r.bottom();x++)
					{
					KSpreadCell *ref1 = cellAt( d,x );
					KSpreadCell *ref2 = cellAt( y,x );
					if(!ref1->isEmpty()&&!ref2->isEmpty())
						{
						tmp->copyALL(ref1);
						ref1->copyALL(ref2);
						ref2->copyALL(tmp);
						}
					}
				}
			}
		else
			{
			cout <<"Err in mode_sort\n";
		        }
		}
		}
	}
	}
setSort(false);
emit sig_updateView( this, r );
}

void KSpreadTable::Column(int ref_column,Mode_sort mode)
{
QRect r( selectionRect() );
setSort(true);
KSpreadCell *tmp = new KSpreadCell( this, -1, -1 );
for ( int d = r.top();  d<= r.bottom(); d++ )
	{
	KSpreadCell *cell1 = cellAt( ref_column, d );
	if(!cell1->isEmpty())
	{
	for ( int y = r.top(); y <= r.bottom(); y++ )
		{
		KSpreadCell *cell2 = cellAt( ref_column, y );
		if(!cell2->isEmpty())
		{
		if(mode==Increase)
			{
			if(strcmp(cell2->text(), cell1->text())>0)
				{
				for(int x=r.left();x<=r.right();x++)
					{
					KSpreadCell *ref1 = cellAt( x, d );
					KSpreadCell *ref2 = cellAt( x, y );
					if(!ref1->isEmpty()&&!ref2->isEmpty())
						{
						tmp->copyALL(ref1);
						ref1->copyALL(ref2);
						ref2->copyALL(tmp);
						}
					}
				}
			}
		else if(mode ==Decrease)
			{
			if(strcmp(cell2->text(), cell1->text())<0)
				{
				for(int x=r.left();x<=r.right();x++)
					{
					KSpreadCell *ref1 = cellAt( x, d );
					KSpreadCell *ref2 = cellAt( x, y );
					if(!ref1->isEmpty()&&!ref2->isEmpty())
						{
						tmp->copyALL(ref1);
						ref1->copyALL(ref2);
						ref2->copyALL(tmp);
						}
					}
				}
			}
		else
			{
			cout <<"Err in Sort_mode\n";
			}
		}
		}
	}
	}
setSort(false);
emit sig_updateView( this, r );
}



void KSpreadTable::setSelectionMultiRow( const QPoint &_marker)
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setMultiRow( !it.current()->multiRow() );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setMultiRow( !it.current()->multiRow() );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );
		
		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();
		cell->setMultiRow( !cell->multiRow() );
		cell->clearDisplayDirtyFlag();
	    }

	emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionAlign( const QPoint &_marker, KSpreadLayout::Align _align )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setAlign( _align );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setAlign( _align );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );

		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();
		cell->setAlign( _align );
		cell->clearDisplayDirtyFlag();
	    }

	emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionPrecision( const QPoint &_marker, int _delta )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  if ( _delta == 1 )
	    it.current()->incPrecision();
	  else
	    it.current()->decPrecision();
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  if ( _delta == 1 )
	    it.current()->incPrecision();
	  else
	    it.current()->decPrecision();
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );

		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();

		if ( _delta == 1 )
		  cell->incPrecision();
		else
		  cell->decPrecision();

		cell->clearDisplayDirtyFlag();
	    }

	emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionMoneyFormat( const QPoint &_marker )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int row = l & 0xFFFF;
	if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setPostfix( " DM" );
	  it.current()->setPrefix( "" );
	  it.current()->setPrecision( 2 );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      QIntDictIterator<KSpreadCell> it( m_dctCells );
      for ( ; it.current(); ++it )
      {
	long l = it.currentKey();
	int col = l >> 16;
	if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
	{
	  it.current()->setDisplayDirtyFlag();
	  it.current()->setPostfix( " DM" );
	  it.current()->setPrefix( "" );
	  it.current()->setPrecision( 2 );
	  it.current()->clearDisplayDirtyFlag();
	}
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    else
    {
	QRect r( m_rctSelection );
	if ( !selected )
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

	KSpreadUndoCellLayout *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {		
		KSpreadCell *cell = cellAt( x, y );

		if ( cell == m_pDefaultCell )
		{
		    cell = new KSpreadCell( this, x, y );
		    int key = y + ( x * 0x10000 );
		    m_dctCells.insert( key, cell );
		}

		cell->setDisplayDirtyFlag();
		cell->setPostfix( " DM" );
		cell->setPrefix( "" );
		cell->setPrecision( 2 );
		cell->clearDisplayDirtyFlag();
	    }

	emit sig_updateView( this, r );
    }
}


void KSpreadTable::insertRow( CORBA::ULong _row )
{
    KSpreadUndoInsertRow *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
	undo = new KSpreadUndoInsertRow( m_pDoc, this, _row );
	m_pDoc->undoBuffer()->appendUndo( undo );
    }

    m_dctCells.setAutoDelete( FALSE );
    m_dctRows.setAutoDelete( FALSE );

    kauto_array<KSpreadCell*> list(m_dctCells.count());
    CORBA::ULong count = 0;
    // Find the last row
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    CORBA::ULong max_row = 1;
    for ( ; it.current(); ++it )
    {
      list[ count++ ] = it.current();
      if ( it.current()->row() > (int)max_row )
	max_row = it.current()->row();
    }

    for ( CORBA::ULong i = max_row; i >= _row; i-- )
    {
      for( CORBA::ULong k = 0; k < count; k++ )
      {
	if ( list[ k ]->row() == (int)i && !list[ k ]->isDefault() )
	{
	  int key = list[ k ]->row() + ( list[ k ]->column() * 0x10000 );
	  m_dctCells.remove( key );
	
	  list[ k ]->setRow( list[ k ]->row() + 1 );
		
	  key = list[ k ]->row() + ( list[ k ]->column() * 0x10000 );
	  m_dctCells.insert( key, list[ k ] );
	}
      }
    }

    kauto_array<RowLayout*> list2(m_dctRows.count());
    count = 0;
    QIntDictIterator<RowLayout> it2( m_dctRows );
    max_row = 1;
    for ( ; it2.current(); ++it2 )
    {
      list2[ count++ ] = it2.current();
      if ( it2.current()->row() > (int)max_row )
	max_row = it2.current()->row();
    }

    for ( CORBA::ULong i = max_row; i >= _row; i-- )
    {
      for( CORBA::ULong k = 0; k < count; k++ )
      {
	if ( list2[ k ]->row() == (int)i )
	{
	  int key = list2[ k ]->row();
	  m_dctRows.remove( key );
		
	  list2[ k ]->setRow( list2[ k ]->row() + 1 );
		
	  key = list2[ k ]->row();
	  m_dctRows.insert( key, list2[ k ] );
	}
      }
    }

    m_dctCells.setAutoDelete( TRUE );
    m_dctRows.setAutoDelete( TRUE );

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );
}

void KSpreadTable::deleteRow( CORBA::ULong _row )
{
    KSpreadUndoDeleteRow *undo = 0L;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
	undo = new KSpreadUndoDeleteRow( m_pDoc, this, _row );
	m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_dctCells.setAutoDelete( FALSE );
    m_dctRows.setAutoDelete( FALSE );

    // Remove row
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    for ( ; it.current(); ++it )
    {
	int key = it.current()->row() + ( it.current()->column() * 0x10000 );

	if ( it.current()->row() == (int)_row && !it.current()->isDefault() )
	{
	    KSpreadCell *cell = it.current();
	    m_dctCells.remove( key );
	    if ( undo )
	      undo->appendCell( cell );
	    else
	      delete cell;
	}
    }

    kauto_array<KSpreadCell*> list( m_dctCells.count());
    int count = 0;
    // Find last row
    it.toFirst();
    int max_row = 1;
    for ( ; it.current(); ++it )
    {
      list[ count++ ] = it.current();
      if ( it.current()->row() > max_row )
	max_row = it.current()->row();
    }

    // Move rows below the deleted one upwards
    for ( int i = _row + 1; i <= max_row; i++ )
    {
      for ( int k = 0; k < count; k++ )
      {
	if ( list[ k ]->row() == i && !list[ k ]->isDefault() )
	{
	  int key = list[ k ]->row() + ( list[ k ]->column() * 0x10000 );
	  m_dctCells.remove( key );
		
	  list[ k ]->setRow( list[ k ]->row() - 1 );
		
	  key = list[ k ]->row() + ( list[ k ]->column() * 0x10000 );
	  m_dctCells.insert( key, list[ k ] );
	}
      }
    }


    // Delete RowLayout
    QIntDictIterator<RowLayout> it2( m_dctRows );
    for ( ; it2.current(); ++it2 )
    {
	int key = it2.current()->row();
	if ( it2.current()->row() == (int)_row && !it2.current()->isDefault() )
	{
	    RowLayout *l = it2.current();
	    m_dctRows.remove( key );
	    if ( undo )
	      undo->setRowLayout( l );
	    else
	      delete l;
	}
    }

    kauto_array<RowLayout*> list2( m_dctRows.count());
    count = 0;
    // Find last RowLayout
    it2.toFirst();
    max_row = 1;
    for ( ; it2.current(); ++it2 )
    {
      list2[ count++ ] = it2.current();
      if ( it2.current()->row() > max_row )
	max_row = it2.current()->row();
    }

    for ( int i = _row + 1; i <= max_row; i++ )
    {
      for ( int k = 0; k < count; k++ )
      {
	if ( list2[ k ]->row() == i && !list2[ k ]->isDefault() )
	{
	  int key = list2[ k ]->row();
	  m_dctRows.remove( key );
		
	  list2[ k ]->setRow( list2[ k ]->row() - 1 );
		
	  key = list2[ k ]->row();
	  m_dctRows.insert( key, list2[ k ] );
	}
      }
    }

    m_dctCells.setAutoDelete( true );
    m_dctRows.setAutoDelete( true );

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );
}

void KSpreadTable::insertColumn( CORBA::ULong _column )
{
    KSpreadUndoInsertColumn *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
	undo = new KSpreadUndoInsertColumn( m_pDoc, this, _column );
	m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_dctCells.setAutoDelete( FALSE );
    m_dctColumns.setAutoDelete( FALSE );

    kauto_array<KSpreadCell*> list( m_dctCells.count());
    CORBA::ULong count = 0;
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    // Determine right most column
    CORBA::ULong max_column = 1;
    for ( ; it.current(); ++it )
    {
      list[ count++ ] = it.current();
      if ( it.current()->column() > (int)max_column )
	max_column = it.current()->column();
    }

    for ( CORBA::ULong i = max_column; i >= _column; i-- )
    {
      for( CORBA::ULong k = 0; k < count; k++ )
      {
	if ( list[ k ]->column() == (int)i && !list[ k ]->isDefault() )
	{
	  printf("Moving Cell %i %i\n",list[k]->column(),list[k]->row());
	  int key = list[ k ]->row() | ( list[ k ]->column() * 0x10000 );
	  m_dctCells.remove( key );
	
	  list[ k ]->setColumn( list[ k ]->column() + 1 );
		
	  key = list[ k ]->row() | ( list[ k ]->column() * 0x10000 );
	  m_dctCells.insert( key, list[ k ] );
	}
      }
    }

    kauto_array<ColumnLayout*> list2(m_dctColumns.count());
    count = 0;
    // Find right most ColumnLayout
    QIntDictIterator<ColumnLayout> it2( m_dctColumns );
    max_column = 1;
    for ( ; it2.current(); ++it2 )
    {
      list2[ count++ ] = it2.current();
      if ( it2.current()->column() > (int)max_column )
	max_column = it2.current()->column();
    }

    for ( CORBA::ULong i = max_column; i >= _column; i-- )
    {
      for( CORBA::ULong k = 0; k < count; k++ )
      {
	if ( list2[ k ]->column() == (int)i )
	{
	  int key = list2[ k ]->column();
	  m_dctColumns.remove( key );
		
	  list2[k]->setColumn( list2[ k ]->column() + 1 );
		
	  key = list2[k]->column();
	  m_dctColumns.insert( key, list2[k] );
	}
      }
    }

    m_dctCells.setAutoDelete( TRUE );
    m_dctColumns.setAutoDelete( TRUE );

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );
}

void KSpreadTable::deleteColumn( CORBA::ULong _column )
{
    KSpreadUndoDeleteColumn *undo = 0L;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
	undo = new KSpreadUndoDeleteColumn( m_pDoc, this, _column );
	m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_dctCells.setAutoDelete( FALSE );
    m_dctColumns.setAutoDelete( FALSE );

    // Delete column
    QIntDictIterator<KSpreadCell> it( m_dctCells );
    for ( ; it.current(); ++it )
    {
	int key = it.current()->row() + ( it.current()->column() * 0x10000 );

	if ( it.current()->column() == (int)_column && !it.current()->isDefault() )
	{
	    KSpreadCell *cell = it.current();
	    m_dctCells.remove( key );
	    if ( undo )
	      undo->appendCell( cell );
	    else
	      delete cell;
	}
    }

    kauto_array<KSpreadCell*> list( m_dctCells.count());
    int count = 0;
    // Find right most cell
    it.toFirst();
    int max_column = 1;
    for ( ; it.current(); ++it )
    {
      list[ count++ ] = it.current();
      if ( it.current()->column() > max_column )
	max_column = it.current()->column();
    }

    // Move cells
    for ( int i = _column + 1; i <= max_column; i++ )
    {
      for ( int k = 0; k < count; k++ )
      {
	if ( list[ k ]->column() == i && !list[ k ]->isDefault() )
	{
	  int key = list[ k ]->row() + ( list[ k ]->column() * 0x10000 );
	  m_dctCells.remove( key );
	
	  list[ k ]->setColumn( list[ k ]->column() - 1 );
		
	  key = list[ k ]->row() + ( list[ k ]->column() * 0x10000 );
	  m_dctCells.insert( key, list[ k ] );
	}
      }
    }

    // Delete ColumnLayout
    QIntDictIterator<ColumnLayout> it2( m_dctColumns );
    for ( ; it2.current(); ++it2 )
    {
	int key = it2.current()->column();
	if ( it2.current()->column() == (int)_column && !it2.current()->isDefault() )
	{
	    ColumnLayout *l = it2.current();
	    m_dctColumns.remove( key );
	    if ( undo )
	      undo->setColumnLayout( l );
	    else
	      delete l;
	}
    }

    kauto_array<ColumnLayout*> list2(m_dctColumns.count());
    count = 0;
    // Move ColumnLayouts
    it2.toFirst();
    max_column = 1;
    for ( ; it2.current(); ++it2 )
    {
      list2[ count++ ] = it2.current();	
      if ( it2.current()->column() > max_column )
	max_column = it2.current()->column();
    }

    for ( int i = _column + 1; i <= max_column; i++ )
    {
      for ( int k = 0; k < count; k++ )
      {
	if ( list2[ k ]->column() == i && !list2[ k ]->isDefault() )
	{
	  int key = list2[ k ]->column();
	  m_dctColumns.remove( key );
		
	  list2[ k ]->setColumn( list2[ k ]->column() - 1 );
		
	  key = list2[ k ]->column();
	  m_dctColumns.insert( key, list2[ k ] );
	}
      }
    }

    m_dctCells.setAutoDelete( TRUE );
    m_dctColumns.setAutoDelete( TRUE );

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );
}

void KSpreadTable::copySelection( const QPoint &_marker )
{
  QRect rct;

  // No selection ? => copy active cell
  if ( m_rctSelection.left() == 0 )
    rct.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
  else if ( m_rctSelection.right() == 0x7fff )
  {
    QMessageBox::critical( (QWidget*)0L, "KSpread Error", "Not supproted", i18n("OK"));
    return;
  }
  else if ( m_rctSelection.bottom() == 0x7fff )
  {
    QMessageBox::critical( (QWidget*)0L, "KSpread Error", "Not supproted", i18n("OK"));
    return;
  }
  else
    rct = selectionRect();

  string data;

  {
    tostrstream out( data );
    if ( !saveCellRect( out, rct ) )
    {
      cerr << "INTERNAL ERROR while copying" << endl;
      return;
    }
  }

  QClipboard *clip = QApplication::clipboard();
  clip->setText( data.c_str() );
  //cout <<" copy : " <<data.c_str()<<endl;
}

void KSpreadTable::cutSelection( const QPoint &_marker )
{
    m_pDoc->setModified( true );

    copySelection( _marker );
    deleteSelection( _marker );
}

void KSpreadTable::paste( const QPoint &_marker,Special_paste sp)
{

  string data = QApplication::clipboard()->text().ascii();
  if ( data.empty() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n( "KSpread Error" ), i18n( "Clipboard is empty" ), i18n( "Ok" ) );
    return;
  }

  istrstream in( data.c_str() );
  loadSelection( in, _marker.x() - 1, _marker.y() - 1,sp);

  m_pDoc->setModified( true );
  emit sig_updateView( this );
}

bool KSpreadTable::loadSelection( istream& _in, int _xshift, int _yshift, Special_paste sp )
{
  KOMLStreamFeed feed( _in );
  KOMLParser parser( &feed );

  string tag;
  vector<KOMLAttrib> lst;
  string name;
  KSpreadCell::Special_paste sp_cell;

  switch(sp)
  	{
  	case ALL:
  		sp_cell=KSpreadCell::ALL;
  		break;
  	case Formula:
  		sp_cell=KSpreadCell::FORMULA;
  		break;
  	case Format:
  		sp_cell= KSpreadCell::Format;
  		break;
  	case Wborder:
  		sp_cell= KSpreadCell::Wborder;
  		break;
  	case Link:
  		sp_cell= KSpreadCell::Link;
  		break;
  	case ALL_trans:
  		sp_cell=KSpreadCell::ALL_trans;
  		break;
  	case Formula_trans:
  		sp_cell=KSpreadCell::FORMULA_trans;
  		break;
  	case Format_trans:
  		sp_cell= KSpreadCell::Format_trans;
  		break;
  	case Wborder_trans:
  		sp_cell= KSpreadCell::Wborder_trans;
  		break;
  	case Link_trans:
  		sp_cell= KSpreadCell::Link_trans;
  		break;
  	default:
  		sp_cell=KSpreadCell::ALL;
  	}
  	
  		

  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    cerr << "Missing DOC" << endl;
    return false;
  }

  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for( ; it != lst.end(); it++ )
  {
    if ( (*it).m_strName == "mime" )
    {
      if ( (*it).m_strValue != "application/x-kspread-selection" )
      {
	cerr << "Unknown mime type " << (*it).m_strValue << endl;
	return false;
      }
    }
  }

  // OBJECT
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "CELL" )
    {
      KSpreadCell *cell = new KSpreadCell( this, 0, 0 );
      cell->load( parser, lst, _xshift, _yshift,sp_cell,m_strName );
      insertCell( cell );
    }
    else
      cerr << "Unknown tag '" << tag << "' in TABLE" << endl;

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing CELL" << endl;
      return false;
    }
  }

  if ( !parser.close( tag ) )
  {
    cerr << "ERR: Closing DOC" << endl;
    return false;
  }

  m_pDoc->setModified( true );

  return true;
}

void KSpreadTable::deleteCells( int _left, int _top, int _right, int _bottom )
{
    // A list of all cells we want to delete.
    QStack<KSpreadCell> cellStack;
    // cellStack.setAutoDelete( TRUE );

    QIntDictIterator<KSpreadCell> it( m_dctCells );
    for ( ; it.current(); ++it )
    {
	if ( !it.current()->isDefault() && it.current()->row() >= _top &&
	     it.current()->row() <= _bottom && it.current()->column() >= _left &&
	     it.current()->column() <= _right )
	  cellStack.push( it.current() );
    }

    m_dctCells.setAutoDelete( false );
    // Remove the cells from the table
    while ( !cellStack.isEmpty() )
    {
	KSpreadCell *cell = cellStack.pop();
	
	int key = cell->row() + ( cell->column() * 0x10000 );
	m_dctCells.remove( key );
	cell->updateDepending();
	
	delete cell;
    }
    m_dctCells.setAutoDelete( true );

    setLayoutDirtyFlag();

    QIntDictIterator<KSpreadCell> it2( m_dctCells );
    for ( ; it2.current(); ++it2 )
      if ( it2.current()->isForceExtraCells() && !it2.current()->isDefault() )
	it2.current()->forceExtraCells( it2.current()->column(), it2.current()->row(),
					it2.current()->extraXCells(), it2.current()->extraYCells() );
}

void KSpreadTable::deleteSelection( const QPoint &_marker )
{
    m_pDoc->setModified( true );

    if ( m_rctSelection.left() == 0 )
    {
	KSpreadUndoDelete *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    QRect r;
	    r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
	    undo = new KSpreadUndoDelete( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}

	deleteCells( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
    }
    else if ( m_rctSelection.right() == 0x7fff )
    {
      // TODO
    }
    else if ( m_rctSelection.bottom() == 0x7fff )
    {
      // TODO
    }
    else
    {
	KSpreadUndoDelete *undo;
	if ( !m_pDoc->undoBuffer()->isLocked() )
	{
	    QRect r;
	    r.setCoords( m_rctSelection.left(), m_rctSelection.top(),
			 m_rctSelection.right(), m_rctSelection.bottom() );
	    undo = new KSpreadUndoDelete( m_pDoc, this, r );
	    m_pDoc->undoBuffer()->appendUndo( undo );
	}
	
	deleteCells( m_rctSelection.left(), m_rctSelection.top(),
		     m_rctSelection.right(), m_rctSelection.bottom() );	
    }

    emit sig_updateView( this );
}

void KSpreadTable::draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
			 CORBA::Float _scale )
{
  QRect page_range;
  page_range.setLeft( 1 );
  page_range.setTop( 1 );

  QRect rect( 1, 1, _width, _height );

  int col = 1;
  int x = columnLayout( col )->width();
  bool bend = false;
  while ( !bend )
  {
    col++;
    int w = columnLayout( col )->width();
    if ( x + w > rect.width() )
    {
      bend = true;
      col--;
    }
    else
      x += w;
  }
  page_range.setRight( col );
	
  int row = 1;
  int y = rowLayout( row )->height();
  bend = false;
  while ( !bend )
  {
    row++;
    int h = rowLayout( row )->height();
    if ( y + h > rect.height() )
    {
      row--;
      bend = true;
    }
    else
      y += h;
  }
  page_range.setBottom( row );

  QPainter painter;
  painter.begin( _dev );

  if ( _scale != 1.0 )
    painter.scale( _scale, _scale );

  printPage( painter, &page_range, NoPen);//doc()->defaultGridPen() );

  painter.end();
}

void KSpreadTable::print( QPainter &painter, QPrinter *_printer )
{
  QPen gridPen;
  gridPen.setStyle( NoPen );

  unsigned int pages = 1;

  QRect cell_range;
  cell_range.setCoords( 1, 1, 1, 1 );

  // Find maximum right/bottom cell with content
  QIntDictIterator<KSpreadCell> it( m_dctCells );
  for ( ; it.current(); ++it )
  {
    if ( it.current()->column() > cell_range.right() )
      cell_range.setRight( it.current()->column() );
    if ( it.current()->row() > cell_range.bottom() )
      cell_range.setBottom( it.current()->row() );
  }

  QList<QRect> page_list;
  page_list.setAutoDelete( TRUE );

  QRect rect;
  rect.setCoords( 0, 0, MM_TO_POINT * (int)m_pDoc->printableWidth(),
		  MM_TO_POINT * (int)m_pDoc->printableHeight() );

  // Up to this row everything is already printed
  int bottom = 0;
  // Start of the next page
  int top = 1;
  // Calculate all pages, but if we are embedded, print only the first one
  while ( bottom < cell_range.bottom() && page_list.count() == 0 )
  {
    // Up to this column everything is already printed
    int right = 0;
    // Start of the next page
    int left = 1;
    while ( right < cell_range.right() )
    {
      QRect *page_range = new QRect;
      page_list.append( page_range );
      page_range->setLeft( left );
      page_range->setTop( top );

      int col = left;
      int x = columnLayout( col )->width();
      while ( x < rect.width() )
      {
	col++;
	x += columnLayout( col )->width();
      }
      // We want to print at least one column
      if ( col == left )
	col = left + 1;
      page_range->setRight( col - 1 );
	
      int row = top;
      int y = rowLayout( row )->height();
      while ( y < rect.height() )
      {
	row++;
	y += rowLayout( row )->height();
      }
      // We want to print at least one row
      if ( row == top )
	row = top + 1;
      page_range->setBottom( row - 1 );

      right = page_range->right();
      left = page_range->right() + 1;
      bottom = page_range->bottom();
    }

    top = bottom + 1;
  }

  int pagenr = 1;

  // Print all pages in the list
  QRect *p;
  for ( p = page_list.first(); p != 0L; p = page_list.next() )
  {
    // print head line
    QFont font( "Times", 10 );
    painter.setFont( font );
    QFontMetrics fm = painter.fontMetrics();
    int w = fm.width( m_pDoc->headLeft( pagenr, m_strName ) );
    if ( w > 0 )
      painter.drawText( (int)( MM_TO_POINT * m_pDoc->leftBorder() ),
			(int)( MM_TO_POINT * 10.0 ), m_pDoc->headLeft( pagenr, m_strName ) );
    w = fm.width( m_pDoc->headMid( pagenr, m_strName ) );
    if ( w > 0 )
      painter.drawText( (int)( MM_TO_POINT * m_pDoc->leftBorder() +
			       ( MM_TO_POINT * m_pDoc->printableWidth() - (float)w ) / 2.0 ),
			(int)( MM_TO_POINT * 10.0 ), m_pDoc->headMid( pagenr, m_strName ) );
    w = fm.width( m_pDoc->headRight( pagenr, m_strName ) );
    if ( w > 0 )
      painter.drawText( (int)( MM_TO_POINT * m_pDoc->leftBorder() +
			       MM_TO_POINT * m_pDoc->printableWidth() - (float)w ),
			(int)( MM_TO_POINT * 10.0 ), m_pDoc->headRight( pagenr, m_strName ) );

    // print foot line
    w = fm.width( m_pDoc->footLeft( pagenr, m_strName ) );
    if ( w > 0 )
      painter.drawText( (int)( MM_TO_POINT * m_pDoc->leftBorder() ),
			(int)( MM_TO_POINT * ( m_pDoc->paperHeight() - 10.0 ) ),
			m_pDoc->footLeft( pagenr, m_strName ) );
    w = fm.width( m_pDoc->footMid( pagenr, m_strName ) );
    if ( w > 0 )
      painter.drawText( (int)( MM_TO_POINT * m_pDoc->leftBorder() +
			       ( MM_TO_POINT * m_pDoc->printableWidth() - (float)w ) / 2.0 ),
			(int)( MM_TO_POINT * ( m_pDoc->paperHeight() - 10.0 ) ),
			m_pDoc->footMid( pagenr, m_strName ) );
    w = fm.width( m_pDoc->footRight( pagenr, m_strName ) );
    if ( w > 0 )
      painter.drawText( (int)( MM_TO_POINT * m_pDoc->leftBorder() +
			       MM_TO_POINT * m_pDoc->printableWidth() - (float)w ),
			(int)( MM_TO_POINT * ( m_pDoc->paperHeight() - 10.0 ) ),
			m_pDoc->footRight( pagenr, m_strName ) );
	
    painter.translate( MM_TO_POINT * m_pDoc->leftBorder(),
		       MM_TO_POINT * m_pDoc->rightBorder() );
    // Print the page
    printPage( painter, p, gridPen );
    painter.translate( - MM_TO_POINT * m_pDoc->leftBorder(),
		       - MM_TO_POINT * m_pDoc->rightBorder() );

    if ( pages < page_list.count() )
      _printer->newPage();
    pagenr++;
  }
}

void KSpreadTable::printPage( QPainter &_painter, QRect *page_range, const QPen& _grid_pen )
{
  int xpos;
  int ypos = 0;
  int x,y;

  for ( y = page_range->top(); y <= page_range->bottom() + 1; y++ )
  {
    RowLayout *row_lay = rowLayout( y );
    xpos = 0;

    for ( x = page_range->left(); x <= page_range->right() + 1; x++ )
    {
      ColumnLayout *col_lay = columnLayout( x );

      // painter.window();	
      KSpreadCell *cell = cellAt( x, y );
      if ( y > page_range->bottom() && x > page_range->right() )
	{ /* Do nothing */ }
      else if ( y > page_range->bottom() )
	cell->print( _painter, xpos, ypos, x, y, col_lay, row_lay, FALSE, TRUE, _grid_pen );
      else if ( x > page_range->right() )
	cell->print( _painter, xpos, ypos, x, y, col_lay, row_lay, TRUE, FALSE, _grid_pen );
      else
	cell->print( _painter, xpos, ypos, x, y, col_lay, row_lay,
		     FALSE, FALSE, _grid_pen );

      xpos += col_lay->width();
    }

    ypos += row_lay->height();
  }

  // Draw the children
  QListIterator<KSpreadChild> chl( m_lstChildren );
  for( ; chl.current(); ++chl )
  {
    cout << "Printing child ....." << endl;
    // HACK, dont display images that reside outside the paper
    _painter.translate( chl.current()->geometry().left(),
			chl.current()->geometry().top() );
    QPicture* pic;
    pic = chl.current()->draw();
    cout << "Fetched picture data" << endl;
    _painter.drawPicture( *pic );
    cout << "Played" << endl;
    _painter.translate( - chl.current()->geometry().left(),
			- chl.current()->geometry().top() );
  }
}

bool KSpreadTable::saveCellRect( ostream &out, const QRect &_rect )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" << "weis@kde.org" << "\" editor=\"" << "KSpread"
      << "\" mime=\"" << "application/x-kspread-selection" << "\" >" << endl;

  // Save all cells.
  QIntDictIterator<KSpreadCell> it( m_dctCells );
  for ( ; it.current(); ++it )
  {
    if ( !it.current()->isDefault() )
    {
      QPoint p( it.current()->column(), it.current()->row() );
      if ( _rect.contains( p ) )
	it.current()->save( out, _rect.left() - 1, _rect.top() - 1,name());
    }
  }

  out << "</DOC>" << endl;

  return true;
}

bool KSpreadTable::save( ostream &out )
{
  out << otag << "<TABLE name=\"" << m_strName.ascii() << "\">" << endl;

  // Save all cells.
  QIntDictIterator<KSpreadCell> it( m_dctCells );
  for ( ; it.current(); ++it )
  {
    if ( !it.current()->isDefault() )
      it.current()->save( out );
  }

  // Save all RowLayout objects.
  QIntDictIterator<RowLayout> rl( m_dctRows );
  for ( ; rl.current(); ++rl )
  {
    if ( !rl.current()->isDefault() )
      rl.current()->save( out );
  }

  // Save all ColumnLayout objects.
  QIntDictIterator<ColumnLayout> cl( m_dctColumns );
  for ( ; cl.current(); ++cl )
  {
    if ( !cl.current()->isDefault() )
      cl.current()->save( out );
  }

  QListIterator<KSpreadChild> chl( m_lstChildren );
  for( ; chl.current(); ++chl )
  {
    chl.current()->save( out );
  }

  out << etag << "</TABLE>" << endl;

  return true;
}

bool KSpreadTable::isLoading()
{
  return m_pDoc->isLoading();
}

bool KSpreadTable::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  // enableScrollBarUpdates( false );

  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "name" )
    {
      m_strName = (*it).m_strValue.c_str();
    }
    else
      cerr << "Unknown attrib 'TABLE:" << (*it).m_strName << "'" << endl;
  }

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  // CELL, ROW, COLUMN, OBJECT
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "CELL" )
    {
      KSpreadCell *cell = new KSpreadCell( this, 0, 0 );
      cell->load( parser, lst );
      insertCell( cell );
    }
    else if ( name == "ROW" )
    {
      RowLayout *rl = new RowLayout( this, 0 );
      rl->load( parser, lst );
      insertRowLayout( rl );
    }
    else if ( name == "COLUMN" )
    {
      ColumnLayout *cl = new ColumnLayout( this, 0 );
      cl->load( parser, lst );
      insertColumnLayout( cl );
    }
    else if ( name == "OBJECT" )
    {
      KSpreadChild *ch = new KSpreadChild( m_pDoc, this );
      ch->load( parser, lst );
      insertChild( ch );
    }
    else if ( name == "CHART" )
    {
      ChartChild *ch = new ChartChild( m_pDoc, this );
      ch->load( parser, lst );
      insertChild( ch );
    }
    else
      cerr << "Unknown tag '" << tag << "' in TABLE" << endl;

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing Child" << endl;
      return false;
    }
  }

  // enableScrollBarUpdates( false );

  return true;
}

void KSpreadTable::update()
{
  QIntDictIterator<KSpreadCell> it( m_dctCells );
  for ( ; it.current(); ++it )
    if ( it.current()->calcDirtyFlag() )
      it.current()->update();
}

bool KSpreadTable::loadChildren( KOStore::Store_ptr _store )
{
  QListIterator<KSpreadChild> it( m_lstChildren );
  for( ; it.current(); ++it )
    if ( !it.current()->loadDocument( _store, it.current()->mimeType() ) )
      return false;

  return true;
}

void KSpreadTable::setShowPageBorders( bool _b )
{
  if ( _b != m_bShowPageBorders )
  {
    m_bShowPageBorders = _b;
    emit sig_updateView( this );
  }
}

bool KSpreadTable::isOnNewPageX( int _column )
{
    int col = 1;
    float x = columnLayout( col )->mmWidth();
    while ( col <= _column )
    {
	// Should never happen
	if ( col == 0x10000 )
	    return FALSE;

	if ( x > m_pDoc->printableWidth() )
	{
	    if ( col == _column )
		return TRUE;
	    else
		x = columnLayout( col )->mmWidth();
	}
	
	col++;
	x += columnLayout( col )->mmWidth();
    }

    return FALSE;
}

bool KSpreadTable::isOnNewPageY( int _row )
{
    int row = 1;
    float y = rowLayout( row )->mmHeight();
    while ( row <= _row )
    {
	// Should never happen
	if ( row == 0x10000 )
	    return FALSE;

	if ( y > m_pDoc->printableHeight() )
	{
	    if ( row == _row )
		return TRUE;
	    else
		y = rowLayout( row )->mmHeight();
	}	
	row++;
	y += rowLayout( row )->mmHeight();
    }

    return FALSE;
}

void KSpreadTable::addCellBinding( CellBinding *_bind )
{
  m_lstCellBindings.append( _bind );

  m_pDoc->setModified( true );
}

void KSpreadTable::removeCellBinding( CellBinding *_bind )
{
  m_lstCellBindings.removeRef( _bind );

  m_pDoc->setModified( true );
}

const char *KSpreadTable::columnLabel(int column)
{
  if ( column <= 26 )
    sprintf( m_arrColumnLabel, "%c", 'A' + column - 1 );
  else if ( column <= 26 * 26 )
    sprintf( m_arrColumnLabel, "%c%c",'A'+((column-1)/26)-1,'A'+((column-1)%26));
  else
    strcpy( m_arrColumnLabel,"@@@");

  return m_arrColumnLabel;
}

KSpreadTable* KSpreadTable::findTable( const char *_name )
{
  if ( !m_pMap )
    return 0L;

  return m_pMap->findTable( _name );
}

void KSpreadTable::insertCell( KSpreadCell *_cell )
{
  int key = _cell->row() + ( _cell->column() * 0x10000 );
  m_dctCells.replace( key, _cell );

  if ( m_bScrollbarUpdates )
  {
    if ( _cell->column() > m_iMaxColumn )
    {
      m_iMaxColumn = _cell->column();
      emit sig_maxColumn( _cell->column() );
    }
    if ( _cell->row() > m_iMaxRow )
    {
      m_iMaxRow = _cell->row();
      emit sig_maxRow( _cell->row() );
    }
  }
}

void KSpreadTable::insertColumnLayout( ColumnLayout *_l )
{
  m_dctColumns.replace( _l->column(), _l );
}

void KSpreadTable::insertRowLayout( RowLayout *_l )
{
  m_dctRows.replace( _l->row(), _l );
}

void KSpreadTable::emit_updateCell( KSpreadCell *_cell, int _col, int _row )
{
  if ( doc()->isLoading() )
    return;

  emit sig_updateCell( this, _cell, _col, _row );
  _cell->clearDisplayDirtyFlag();
}

void KSpreadTable::emit_updateRow( RowLayout *_layout, int _row )
{
  QIntDictIterator<KSpreadCell> it( m_dctCells );
  for ( ; it.current(); ++it )
    if ( it.current()->row() == _row )
      it.current()->setLayoutDirtyFlag();

  emit sig_updateVBorder( this );
  emit sig_updateView( this );
  _layout->clearDisplayDirtyFlag();
}

void KSpreadTable::emit_updateColumn( ColumnLayout *_layout, int _column )
{
  QIntDictIterator<KSpreadCell> it( m_dctCells );
  for ( ; it.current(); ++it )
    if ( it.current()->column() == _column )
      it.current()->setLayoutDirtyFlag();

  emit sig_updateHBorder( this );
  emit sig_updateView( this );
  _layout->clearDisplayDirtyFlag();
}

void KSpreadTable::insertChart( const QRect& _rect, KoDocumentEntry& _e, const QRect& _data )
{
  cerr << "Createing document" << endl;
  KOffice::Document_var doc = _e.createDoc();
  cerr << "Created" << endl;
  if ( CORBA::is_nil( doc ) )
    // Error message is already displayed, so just return
    return;

  cerr << "NOW FETCHING INTERFACE" << endl;

  // doc->init();
  CORBA::Object_var obj = doc->getInterface( "IDL:Chart/SimpleChart:1.0" );
  if ( CORBA::is_nil( obj ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "The server %s does not support the required interface" ), _e.name.data() );
    QMessageBox::critical( (QWidget*)0L, i18n("KSpread Error" ), tmp, i18n("Ok" ) );
    return;
  }
  Chart::SimpleChart_var chart = Chart::SimpleChart::_narrow( obj );
  if ( CORBA::is_nil( chart ) )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KSpread Error" ),
			   i18n("The chart application seems to have an internal error" ), i18n("Ok" ) );
    return;
  }

  ChartChild* ch = new ChartChild( m_pDoc, this, _rect, doc );
  ch->setDataArea( _data );
  ch->setChart( chart );
  ch->update();

  // chart->showWizard();

  insertChild( ch );
}

void KSpreadTable::insertChild( const QRect& _rect, KoDocumentEntry& _e )
{
  cerr << "Createing document" << endl;
  KOffice::Document_var doc = _e.createDoc();
  cerr << "Created" << endl;
  if ( CORBA::is_nil( doc ) )
    // Error message is already displayed, so just return
    return;

  doc->initDoc();
  cerr << "Initialized" << endl;
  KSpreadChild* ch = new KSpreadChild( m_pDoc, this, _rect, doc );
  insertChild( ch );
}

void KSpreadTable::insertChild( KSpreadChild *_child )
{
  m_lstChildren.append( _child );

  emit sig_insertChild( _child );
}

void KSpreadTable::changeChildGeometry( KSpreadChild *_child, const QRect& _rect )
{
  _child->setGeometry( _rect );

  emit sig_updateChildGeometry( _child );
}

QListIterator<KSpreadChild> KSpreadTable::childIterator()
{
  return QListIterator<KSpreadChild> ( m_lstChildren );
}

void KSpreadTable::makeChildList( KOffice::Document_ptr _doc, const char *_path )
{
  int i = 0;

  QListIterator<KSpreadChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    QString tmp;
    tmp.sprintf("/%i", i++ );
    QString path( _path );
    path += tmp.data();

    KOffice::Document_var doc = it.current()->document();
    doc->makeChildList( _doc, path );
  }
}

bool KSpreadTable::hasToWriteMultipart()
{
  QListIterator<KSpreadChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    if ( !it.current()->isStoredExtern() )
      return true;
  }

  return false;
}

KSpreadTable::~KSpreadTable()
{
  s_mapTables->remove( m_id );

  m_pPainter->end();
  delete m_pPainter;
  delete m_pWidget;
}

void KSpreadTable::enableScrollBarUpdates( bool _enable )
{
  m_bScrollbarUpdates = _enable;
}

void KSpreadTable::initInterpreter()
{
  m_module = new KSModule( doc()->interpreter(), m_strName, 0 );
  m_context.setScope( new KSScope( doc()->interpreter()->globalNamespace(), m_module ) );
}

/**********************************************************
 *
 * KSpreadChild
 *
 **********************************************************/

KSpreadChild::KSpreadChild( KSpreadDoc *_spread, KSpreadTable *_table, const QRect& _rect, KOffice::Document_ptr _doc )
  : KoDocumentChild( _rect, _doc )
{
  m_pTable = _table;
  m_pDoc = _spread;
}

KSpreadChild::KSpreadChild( KSpreadDoc *_spread, KSpreadTable *_table ) : KoDocumentChild()
{
  m_pTable = _table;
  m_pDoc = _spread;
}


KSpreadChild::~KSpreadChild()
{
}

/**********************************************************
 *
 * ChartChild
 *
 **********************************************************/

ChartChild::ChartChild( KSpreadDoc *_spread, KSpreadTable *_table, const QRect& _rect, KOffice::Document_ptr _doc )
  : KSpreadChild( _spread, _table, _rect, _doc )
{
  m_pBinding = 0L;
}

ChartChild::ChartChild( KSpreadDoc *_spread, KSpreadTable *_table ) :
  KSpreadChild( _spread, _table )
{
  m_pBinding = 0L;
}

ChartChild::~ChartChild()
{
  if ( m_pBinding )
    delete m_pBinding;
}

void ChartChild::setDataArea( const QRect& _data )
{
  if ( m_pBinding == 0L )
    m_pBinding = new ChartBinding( m_pTable, _data, this );
  else
    m_pBinding->setDataArea( _data );
}

void ChartChild::setChart( Chart::SimpleChart_ptr _chart )
{
  if ( m_pBinding )
    m_pBinding->setChart( _chart );
}

void ChartChild::update()
{
  if ( m_pBinding )
    m_pBinding->cellChanged( 0L );
}

bool ChartChild::save( ostream& out )
{
  CORBA::String_var u = m_rDoc->url();
  CORBA::String_var mime = m_rDoc->mimeType();

  out << indent << "<CHART url=\"" << u << "\" mime=\"" << mime << "\">"
      << m_geometry;
  if ( m_pBinding )
    out << "<BINDING>" << m_pBinding->dataArea() << "</BINDING>";
  out << "</CHART>" << endl;

  return true;
}

bool ChartChild::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  cerr << "######################### CC:load ################" << endl;

  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "url" )
    {
      m_strURL = (*it).m_strValue.c_str();
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_strMimeType = (*it).m_strValue.c_str();
    }
    else
      cerr << "Unknown attrib 'CHART:" << (*it).m_strName << "'" << endl;
  }

  if ( m_strURL.isEmpty() )
  {
    cerr << "Empty 'id' attribute in CHART" << endl;
    return false;
  }
  else if ( m_strMimeType.isEmpty() )
  {
    cerr << "Empty mime attribute in CHART" << endl;
    return false;
  }

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  bool brect = false;
  bool bbind = false;
  QRect bind;

  // RECT, BINDING
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "RECT" )
    {
      brect = true;
      m_geometry = tagToRect( lst );
    }
    else if ( name == "BINDING" )
    {
      string tag2;
      vector<KOMLAttrib> lst2;
      string name2;
      // RECT
      while( parser.open( 0L, tag2 ) )
      {
	KOMLParser::parseTag( tag2.c_str(), name2, lst2 );

	if ( name2 == "RECT" )
	{
	  bbind = true;
	  bind = tagToRect( lst2 );
	}
	else
	  cerr << "Unknown tag '" << tag2 << "' in BINDING" << endl;

      }
      if ( !parser.close( tag ) )
      {
	cerr << "ERR: Closing BINDING" << endl;
	return false;
      }
    }
    else
      cerr << "Unknown tag '" << tag << "' in CHART" << endl;

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing Child in CHART" << endl;
      return false;
    }
  }

  if ( !brect )
  {
    cerr << "Missing RECT in CHART" << endl;
    return false;
  }
  if ( !bbind )
  {
    cerr << "Missing BINDING in CHART" << endl;
    return false;
  }

  setDataArea( bind );

  return true;
}

bool ChartChild::loadDocument( KOStore::Store_ptr _store, const char *_format )
{
  cerr << "######################### CC:loadDoc ################" << endl;

  bool res = KSpreadChild::loadDocument( _store, _format );
  if ( !res )
    return res;

  CORBA::Object_var obj = m_rDoc->getInterface( "IDL:Chart/SimpleChart:1.0" );
  Chart::SimpleChart_var chart = Chart::SimpleChart::_narrow( obj );
  if ( CORBA::is_nil( chart ) )
  {
    QMessageBox::critical( 0L, i18n("Internal Error"),
			   i18n("Chart does not support the required interface"),
			   i18n("OK") );
    return false;
  }

  setChart( chart );
  update();

  return true;
}


#include "kspread_table.moc"
