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

#include <qpainter.h>
#include <qapplication.h>
#include <qdrawutil.h>
#include <qkeycode.h>
#include <qregexp.h>
#include <qpoint.h>
#include <qprinter.h>
#include <qcursor.h>
#include <qstack.h>
#include <qbuffer.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qpicture.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qdragobject.h>
#include <qmime.h>

#include <klocale.h>
#include <kglobal.h>

#include "kspread_table.h"
#include "kspread_undo.h"
#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_canvas.h"

#include "KSpreadTableIface.h"

#include <koStream.h>

#include <koscript_context.h>

#include <strstream.h>
#include <kdebug.h>

#include "../kchart/kchart_part.h"

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
  m_child = _child;
}

ChartBinding::~ChartBinding()
{
}

void ChartBinding::cellChanged( KSpreadCell* )
{
    kdDebug(36001) << "######### void ChartBinding::cellChanged( KSpreadCell* )" << endl;

    if ( m_bIgnoreChanges )
        return;

    kdDebug(36001) << "with=" << m_rctDataArea.width() << "  height=" << m_rctDataArea.height() << endl;

    KChartData matrix( m_rctDataArea.height(), m_rctDataArea.width() );

    for ( int y = 0; y < m_rctDataArea.height(); y++ )
        for ( int x = 0; x < m_rctDataArea.width(); x++ )
        {
            KSpreadCell* cell = m_pTable->cellAt( m_rctDataArea.left() + x, m_rctDataArea.top() + y );
            matrix.cell( y, x ).exists = TRUE;
            if ( cell && cell->isValue() )
                matrix.cell( y, x ).value = cell->valueDouble();
            else if ( cell )
                matrix.cell( y, x ).value = cell->valueString();
            else
                matrix.cell( y, x ).exists = FALSE;
        }

    // ######### Kalle may be interested in that, too
    /* Chart::Range range;
       range.top = m_rctDataArea.top();
       range.left = m_rctDataArea.left();
       range.right = m_rctDataArea.right();
       range.bottom = m_rctDataArea.bottom();
       range.table = m_pTable->name(); */

    m_child->chart()->setPart( matrix );

    // Force a redraw of the chart on all views
    table()->emit_polygonInvalidated( m_child->framePointArray() );
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

KSpreadTable::KSpreadTable( KSpreadMap *_map, const QString &tableName, const char *_name )
    : QObject( _map, _name )
{
  if ( s_mapTables == 0L )
    s_mapTables = new QIntDict<KSpreadTable>;
  m_id = s_id++;
  s_mapTables->insert( m_id, this );

  m_defaultLayout = new KSpreadLayout( this );

  m_emptyPen.setStyle( Qt::NoPen );

  m_marker.setCoords( 1, 1, 1, 1 );

  m_pMap = _map;
  m_pDoc = _map->doc();
  m_dcop = 0;
  m_bShowPageBorders = FALSE;

  m_lstCellBindings.setAutoDelete( FALSE );

  m_strName = tableName;

  // m_lstChildren.setAutoDelete( true );

  m_cells.setAutoDelete( true );
  m_rows.setAutoDelete( true );
  m_columns.setAutoDelete( true );

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

  setHidden( false );
  m_bShowGrid=true;
  m_bShowFormular=false;
  m_bLcMode=false;
  m_bShowColumnNumber=false;
  // Get a unique name so that we can offer scripting
  if ( !_name )
  {
      QCString s;
      s.sprintf("Table%i", s_id );
      QObject::setName( s.data() );
  }
}

bool KSpreadTable::isEmpty( unsigned long int x, unsigned long int y )
{
  KSpreadCell* c = cellAt( x, y );
  if ( !c || c->isEmpty() )
    return true;

  return false;
}

const ColumnLayout* KSpreadTable::columnLayout( int _column ) const
{
    const ColumnLayout *p = m_columns.lookup( _column );
    if ( p != 0L )
        return p;

    return m_pDefaultColumnLayout;
}

ColumnLayout* KSpreadTable::columnLayout( int _column )
{
    ColumnLayout *p = m_columns.lookup( _column );
    if ( p != 0L )
        return p;

    return m_pDefaultColumnLayout;
}

const RowLayout* KSpreadTable::rowLayout( int _row ) const
{
    const RowLayout *p = m_rows.lookup( _row );
    if ( p != 0L )
        return p;

    return m_pDefaultRowLayout;
}

RowLayout* KSpreadTable::rowLayout( int _row )
{
    RowLayout *p = m_rows.lookup( _row );
    if ( p != 0L )
        return p;

    return m_pDefaultRowLayout;
}

int KSpreadTable::leftColumn( int _xpos, int &_left, KSpreadCanvas *_canvas )
{
    if ( _canvas )
    {
        _xpos += _canvas->xOffset();
        _left = -_canvas->xOffset();
    }
    else
        _left = 0;

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
    if ( _canvas )
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
    if ( _canvas )
    {
        _ypos += _canvas->yOffset();
        _top = -_canvas->yOffset();
    }
    else
        _top = 0;

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
    if ( _canvas )
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
    int x = 0;
    if ( _canvas )
      x -= _canvas->xOffset();
    for ( int col = 1; col < _col; col++ )
    {
        // Should never happen
        if ( col == 0x10000 )
            return x;

        x += columnLayout( col )->width( _canvas );
    }

    return x;
}

int KSpreadTable::rowPos( int _row, KSpreadCanvas *_canvas )
{
    int y = 0;
    if ( _canvas )
      y -= _canvas->yOffset();
    for ( int row = 1 ; row < _row ; row++ )
    {
        // Should never happen
        if ( row == 0x10000 )
            return y;

        y += rowLayout( row )->height( _canvas );
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

KSpreadCell* KSpreadTable::firstCell()
{
    return m_cells.firstCell();
}

const KSpreadCell* KSpreadTable::cellAt( int _column, int _row ) const
{
    const KSpreadCell *p = m_cells.lookup( _column, _row );
    if ( p != 0L )
        return p;

    return m_pDefaultCell;
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

  KSpreadCell *p = m_cells.lookup( _column, _row );
  if ( p != 0L )
    return p;

  return m_pDefaultCell;
}

ColumnLayout* KSpreadTable::nonDefaultColumnLayout( int _column, bool force_creation )
{
    ColumnLayout *p = m_columns.lookup( _column );
    if ( p != 0L || !force_creation )
        return p;

    p = new ColumnLayout( this, _column );
    p->setWidth( m_pDefaultColumnLayout->width() );
    m_columns.insertElement( p, _column );

    return p;
}

RowLayout* KSpreadTable::nonDefaultRowLayout( int _row, bool force_creation )
{
    RowLayout *p = m_rows.lookup( _row );
    if ( p != 0L || !force_creation )
        return p;

    p = new RowLayout( this, _row );
    // TODO: copy the default RowLayout here!!
    p->setHeight( m_pDefaultRowLayout->height() );
    m_rows.insertElement( p, _row );

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

  KSpreadCell *p = m_cells.lookup( _column, _row );
  if ( p != 0L )
    return p;

  KSpreadCell *cell = new KSpreadCell( this, _column, _row );
  m_cells.insert( cell, _column, _row );

  return cell;
}

void KSpreadTable::setText( int _row, int _column, const QString& _text, bool updateDepends )
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
    cell->setCellText( _text, updateDepends );
}

void KSpreadTable::setLayoutDirtyFlag()
{
    KSpreadCell* c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->setLayoutDirtyFlag();
}

void KSpreadTable::setCalcDirtyFlag()
{
    KSpreadCell* c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->setCalcDirtyFlag();
}

void KSpreadTable::recalc(bool m_depend)
{
    kdDebug(36001) << "KSpreadTable::recalc(" << m_depend << ") STARTING" << endl;
    // First set all cells as dirty
    setCalcDirtyFlag();

    // Now recalc cells - it is important to do it AFTER, so that when
    // calculating one cell calculates many others, those are not done again.
    KSpreadCell* c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->calc( m_depend );

    kdDebug(36001) << "KSpreadTable::recalc(" << m_depend << ") DONE" << endl;
}

void KSpreadTable::setChooseRect( const QRect &_sel )
{
    if ( _sel == m_chooseRect )
        return;

    QRect old( m_chooseRect );
    m_chooseRect = _sel;

    emit sig_changeChooseSelection( this, old, m_chooseRect );
}

void KSpreadTable::unselect()
{
    // No selection? Then do nothing.
    if ( m_rctSelection.left() == 0 )
        return;

    QRect r = m_rctSelection;
    // Discard the selection
    m_rctSelection.setCoords( 0, 0, 0, 0 );

    // Emit signal so that the views can update.
    emit sig_unselect( this, r );
}

void KSpreadTable::setMarker( const QPoint& _point, KSpreadCanvas *_canvas )
{
    setSelection( QRect(), _point, _canvas );
}

QRect KSpreadTable::markerRect() const
{
    QRect r;
    if ( m_rctSelection.left() == 0 )
        r = m_marker;
    else
        r = m_rctSelection;

    if ( r.topLeft() == r.bottomRight() )
    {
        const KSpreadCell* cell = cellAt( r.left(), r.top() );
        if ( cell->extraXCells() || cell->extraYCells() )
            r.setCoords( r.left(), r.top(),
                         r.left() + cell->extraXCells(), r.top() + cell->extraYCells() );
    }

    return r;
}

QRect KSpreadTable::marker() const
{
    return m_marker;
}

void KSpreadTable::setSelection( const QRect &_sel, KSpreadCanvas *_canvas )
{
    if ( _sel.left() == 0 )
        setSelection( _sel, m_marker.topLeft(), _canvas );
    else
    {
        if ( m_marker.topLeft() != _sel.topLeft() && m_marker.topRight() != _sel.topRight() &&
             m_marker.bottomLeft() != _sel.bottomLeft() && m_marker.bottomRight() != _sel.bottomRight() )
            setSelection( _sel, _sel.topLeft(), _canvas );
        else
            setSelection( _sel, m_marker.topLeft(), _canvas );
    }
}

void KSpreadTable::setSelection( const QRect &_sel, const QPoint& m, KSpreadCanvas *_canvas )
{
  if ( _sel == m_rctSelection && m == m_marker.topLeft() )
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

  QRect old_marker = m_marker;
  QRect old( m_rctSelection );
  m_rctSelection = _sel;

  KSpreadCell* cell = cellAt( m.x(), m.y() );
  if ( cell->extraXCells() || cell->extraYCells())
      m_marker.setCoords( m.x(), m.y(), m.x() + cell->extraXCells(), m.y() + cell->extraYCells() );
  else if(cell->isObscuringForced())
        {
        KSpreadCell* cell2 = cellAt( cell->obscuringCellsColumn(),
        cell->obscuringCellsRow() );
        if( m.x()==cell->obscuringCellsColumn()+ cell2->extraXCells() &&
                m.y()==cell->obscuringCellsRow()+ cell2->extraYCells())
                {
                m_marker.setCoords( cell->obscuringCellsColumn(),
                        cell->obscuringCellsRow(), m.x(), m.y()  );
                }
        else
                m_marker = QRect( m, m );
        }
  else
      m_marker = QRect( m, m );

  emit sig_changeSelection( this, old, old_marker );
}

void KSpreadTable::setSelectionFont( const QPoint &_marker, const char *_font, int _size,
                                     signed char _bold, signed char _italic,signed char _underline,
                                     signed char _strike )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
            &&!c->isObscuringForced())
            {
                c->setDisplayDirtyFlag();
                if ( _font )
                    c->setTextFontFamily( _font );
                if ( _size > 0 )
                    c->setTextFontSize( _size );
                if ( _italic >= 0 )
                    c->setTextFontItalic( (bool)_italic );
                if ( _bold >= 0 )
                    c->setTextFontBold( (bool)_bold );
                if ( _underline >= 0 )
                    c->setTextFontUnderline( (bool)_underline );
                if ( _strike >= 0 )
                    c->setTextFontStrike( (bool)_strike );
                c->clearDisplayDirtyFlag();
            }
        }

        emit sig_updateView( this, m_rctSelection );
        return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
            &&!c->isObscuringForced())
            {
                c->setDisplayDirtyFlag();
                if ( _font )
                    c->setTextFontFamily( _font );
                if ( _size > 0 )
                    c->setTextFontSize( _size );
                if ( _italic >= 0 )
                    c->setTextFontItalic( (bool)_italic );
                if ( _bold >= 0 )
                    c->setTextFontBold( (bool)_bold );
                if ( _underline >= 0 )
                    c->setTextFontUnderline( (bool)_underline );
                if ( _strike >= 0 )
                    c->setTextFontStrike( (bool)_strike );
                c->clearDisplayDirtyFlag();
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
              if(!cell->isObscuringForced())
              {
               if ( cell == m_pDefaultCell )
               {
                  cell = new KSpreadCell( this, x, y );
                  m_cells.insert( cell, x, y );
               }

              cell->setDisplayDirtyFlag();

              if ( _font )
                cell->setTextFontFamily( _font );
              if ( _size > 0 )
                cell->setTextFontSize( _size );
              if ( _italic >= 0 )
                  cell->setTextFontItalic( (bool)_italic );
              if ( _bold >= 0 )
                  cell->setTextFontBold( (bool)_bold );
              if ( _underline >= 0 )
                  cell->setTextFontUnderline( (bool)_underline );
              if ( _strike >= 0 )
                  cell->setTextFontStrike( (bool)_strike );
              cell->clearDisplayDirtyFlag();
              }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionSize( const QPoint &_marker, int _size )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
            &&!c->isObscuringForced())
            {
                c->setDisplayDirtyFlag();
                c->setTextFontSize( ( c->textFontSize( _marker.x(), _marker.y() ) + _size ) );
                c->clearDisplayDirtyFlag();
            }
        }

        emit sig_updateView( this, m_rctSelection );
        return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
            &&!c->isObscuringForced())
            {
                c->setDisplayDirtyFlag();
                c->setTextFontSize( ( c->textFontSize( _marker.x(), _marker.y() ) + _size ) );
                c->clearDisplayDirtyFlag();
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
              if(!cell->isObscuringForced())
              {
               if ( cell == m_pDefaultCell )
               {
                cell = new KSpreadCell( this, x, y );
                m_cells.insert( cell, x, y );
               }

              cell->setDisplayDirtyFlag();
              cell->setTextFontSize( (cell->textFontSize( _marker.x(), _marker.y() ) + _size) );
              cell->clearDisplayDirtyFlag();
              }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionUpperLower( const QPoint &_marker,int _type )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
            {
                if( !c->isValue() && !c->isBool() &&!c->isFormular() && !c->isDefault()&&
                    !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
                    &&!c->isObscuringForced())
                {
                    c->setDisplayDirtyFlag();
                    if( _type == -1 )
                        c->setCellText( (c->text().lower()));
                    else if(_type==1)
                        c->setCellText( (c->text().upper()));
                    c->clearDisplayDirtyFlag();
                }
            }
        }

        emit sig_updateView( this, m_rctSelection );
        return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
            {
                if( !c->isValue() && !c->isBool() &&!c->isFormular() && !c->isDefault() && !c->text().isEmpty() &&
                    c->text()[0] != '*' && c->text()[0] != '!' &&!c->isObscuringForced())
                {
                    c->setDisplayDirtyFlag();
                    if(_type==-1)
                        c->setCellText( (c->text().lower()));
                    else if(_type==1)
                        c->setCellText( (c->text().upper()));
                    c->clearDisplayDirtyFlag();
                }
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

        for ( int x = r.left(); x <= r.right(); x++ )
            for ( int y = r.top(); y <= r.bottom(); y++ )
            {
              KSpreadCell *cell = cellAt( x, y );
              if(!cell->isValue() && !cell->isBool() &&!cell->isFormular() &&!cell->isDefault()&&!cell->text().isEmpty()&&(cell->text().find('*')!=0)&&(cell->text().find('!')!=0))
                {
                if(!cell->isObscuringForced())
                {
                cell->setDisplayDirtyFlag();
                if(_type==-1)
                        cell->setCellText( (cell->text().lower()));
                else if(_type==1)
                        cell->setCellText( (cell->text().upper()));
                cell->clearDisplayDirtyFlag();
                }
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionfirstLetterUpper( const QPoint &_marker)
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
            {
                if( !c->isValue() && !c->isBool() && !c->isFormular() && !c->isDefault() &&
                    !c->text().isEmpty() && c->text()[0] != '*'  && c->text()[0] != '!'
                    &&!c->isObscuringForced())
                {
                    c->setDisplayDirtyFlag();
                    QString tmp=c->text();
                    int len=tmp.length();
                    c->setCellText( (tmp.at(0).upper()+tmp.right(len-1)));
                    c->clearDisplayDirtyFlag();
                }
            }
        }

        emit sig_updateView( this, m_rctSelection );
        return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
            {
                if( !c->isValue() && !c->isBool() && !c->isFormular() && !c->isDefault() &&
                    !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
                    &&!c->isObscuringForced())
                {
                    c->setDisplayDirtyFlag();
                    QString tmp=c->text();
                    int len=tmp.length();
                    c->setCellText( (tmp.at(0).upper()+tmp.right(len-1)));
                    c->clearDisplayDirtyFlag();
                }
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

        for ( int x = r.left(); x <= r.right(); x++ )
            for ( int y = r.top(); y <= r.bottom(); y++ )
            {
              KSpreadCell *cell = cellAt( x, y );
              if(!cell->isValue() && !cell->isBool() &&!cell->isFormular() &&!cell->isDefault()&&!cell->text().isEmpty()&&(cell->text().find('*')!=0)&&(cell->text().find('!')!=0)
              &&!cell->isObscuringForced())
                {

                cell->setDisplayDirtyFlag();
                QString tmp=cell->text();
                int len=tmp.length();
                cell->setCellText( (tmp.at(0).upper()+tmp.right(len-1)));
                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionVerticalText( const QPoint &_marker,bool _b)
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setVerticalText(_b);
                c->setMultiRow( false );
                c->setAngle(0);
                c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setVerticalText(_b);
                c->setMultiRow( false );
                c->setAngle(0);
                c->clearDisplayDirtyFlag();

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
                if(!cell->isObscuringForced())
                {
                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }

                cell->setDisplayDirtyFlag();
                cell->setVerticalText(_b);
                cell->setMultiRow( false );
                cell->setAngle(0);
                cell->clearDisplayDirtyFlag();
                }

            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionComment( const QPoint &_marker,QString _comment)
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setComment(_comment);
                c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setComment(_comment);
                c->clearDisplayDirtyFlag();

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
              if(!cell->isObscuringForced())
               {
               if ( cell == m_pDefaultCell )
                {
                cell = new KSpreadCell( this, x, y );
                m_cells.insert( cell, x, y );
                }

               cell->setDisplayDirtyFlag();
               cell->setComment(_comment);
               cell->clearDisplayDirtyFlag();
               }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionAngle( const QPoint &_marker,int _value)
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setAngle(_value);
                c->setVerticalText(false);
                c->setMultiRow( false );
                c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setAngle(_value);
                c->setVerticalText(false);
                c->setMultiRow( false );
                c->clearDisplayDirtyFlag();

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
              if(!cell->isObscuringForced())
               {
               if ( cell == m_pDefaultCell )
                {
                cell = new KSpreadCell( this, x, y );
                m_cells.insert( cell, x, y );
                }

               cell->setDisplayDirtyFlag();
               cell->setAngle(_value);
               cell->setVerticalText(false);
               cell->setMultiRow( false );
               cell->clearDisplayDirtyFlag();
               }
            }
        emit sig_updateView( this, r );
    }
}


void KSpreadTable::setSelectionRemoveComment( const QPoint &_marker)
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setComment("");
                c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
                c->setDisplayDirtyFlag();
                c->setComment("");
                c->clearDisplayDirtyFlag();

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
              if(!cell->isObscuringForced())
              {
                if ( cell != m_pDefaultCell )
                {

                        cell->setDisplayDirtyFlag();
                        cell->setComment("");
                        cell->clearDisplayDirtyFlag();
                }
              }


            }

        emit sig_updateView( this, r );
    }
}


void KSpreadTable::setSelectionTextColor( const QPoint &_marker, const QColor &tb_Color )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setTextColor(tb_Color);
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setTextColor(tb_Color);

          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                if ( cell == m_pDefaultCell )
                {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                }

                cell->setDisplayDirtyFlag();
                cell-> setTextColor(tb_Color);
                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionbgColor( const QPoint &_marker, const QColor &bg_Color )
{
m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setBgColor(bg_Color);
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setBgColor(bg_Color);

          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                if ( cell == m_pDefaultCell )
                {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                }

                cell->setDisplayDirtyFlag();
                cell-> setBgColor(bg_Color);
                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionBorderColor( const QPoint &_marker, const QColor &bd_Color )
{
    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          int it_Row=c->row();
          int it_Col=c->column();
          if(c->topBorderStyle(it_Row,it_Col)!=Qt::NoPen )
                c->setTopBorderColor( bd_Color );
          if(c->leftBorderStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setLeftBorderColor(bd_Color);
          if(c->fallDiagonalStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setFallDiagonalColor(bd_Color);
          if(c->goUpDiagonalStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setGoUpDiagonalColor(bd_Color);
          if(c->bottomBorderStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setBottomBorderColor(bd_Color);
          if(c->rightBorderStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setRightBorderColor(bd_Color);

          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          int it_Row=c->row();
          int it_Col=c->column();
          if(c->topBorderStyle(it_Row,it_Col)!=Qt::NoPen )
                c->setTopBorderColor( bd_Color );
          if(c->leftBorderStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setLeftBorderColor(bd_Color);
          if(c->fallDiagonalStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setFallDiagonalColor(bd_Color);
          if(c->goUpDiagonalStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setGoUpDiagonalColor(bd_Color);
          if(c->bottomBorderStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setBottomBorderColor(bd_Color);
          if(c->rightBorderStyle(it_Row,it_Col)!=Qt::NoPen)
                c->setRightBorderColor(bd_Color);

          c->clearDisplayDirtyFlag();
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

                if ( cell != m_pDefaultCell &&!cell->isObscuringForced())
                {
                        cell->setDisplayDirtyFlag();
                        if(cell->topBorderStyle(x,y)!=Qt::NoPen )
                                cell->setTopBorderColor( bd_Color );
                        if(cell->leftBorderStyle(x,y)!=Qt::NoPen)
                                cell->setLeftBorderColor(bd_Color);
                        if(cell->fallDiagonalStyle(x,y)!=Qt::NoPen)
                                cell->setFallDiagonalColor(bd_Color);
                        if(cell->goUpDiagonalStyle(x,y)!=Qt::NoPen)
                                cell->setGoUpDiagonalColor(bd_Color);
                        if(cell->bottomBorderStyle(x,y)!=Qt::NoPen)
                                cell->setBottomBorderColor(bd_Color);
                        if(cell->rightBorderStyle(x,y)!=Qt::NoPen)
                                cell->setRightBorderColor(bd_Color);
                        cell->clearDisplayDirtyFlag();
                }


            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSeries( const QPoint &_marker,int start,int end,int step,Series mode,Series type)
{
m_pDoc->setModified( true );
QRect r(_marker.x(), _marker.y(), _marker.x(), _marker.y() );

int y = r.top();
int x = r.left();
int posx=0;
int posy=0;
int numberOfCell=0;
for ( int incr=start;incr<=end; )
        {
        if(type==Linear)
                incr=incr+step;
        else if(type==Geometric)
                incr=incr*step;
        numberOfCell++;
        }

int extraX=_marker.x();
int extraY=_marker.y();
if(mode==Column)
{
for ( int y = _marker.y(); y <=(_marker.y()+numberOfCell); y++ )
        {
        KSpreadCell *cell = cellAt( _marker.x(), y );
        if( cell->isObscuringForced())
                {
                numberOfCell+=cell->extraYCells()+1;
                extraX=QMIN(extraX,cell->obscuringCellsColumn());
                }
        }
}
else if(mode==Row)
{
for ( int x = _marker.x(); x <=(_marker.x()+numberOfCell); x++ )
        {
        KSpreadCell *cell = cellAt( x,_marker.y() );
        if( cell->isObscuringForced())
                {
                numberOfCell+=cell->extraXCells()+1;
                extraY=QMIN(extraY,cell->obscuringCellsRow());
                }
        }
}
QRect rect;
if(mode==Column)
{
        rect.setCoords( extraX,_marker.y(),_marker.x(),_marker.y()+numberOfCell);
}
else if(mode==Row)
{
        rect.setCoords(_marker.x(),extraY,_marker.x()+numberOfCell,_marker.y());
}

if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, rect );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }

for ( int incr=start;incr<=end; )
        {
        KSpreadCell *cell = cellAt( x+posx, y+posy );
        if(cell->isObscuringForced())
                {
                cell = cellAt( cell->obscuringCellsColumn(), cell->obscuringCellsRow());
                }
        if ( cell == m_pDefaultCell )
                {
                cell = new KSpreadCell( this, x+posx, y+posy );
                m_cells.insert( cell, x + posx, y + posy );
                }

        QString tmp;
        cell->setCellText(tmp.setNum(incr));
        if(mode==Column)
                if(cell->isForceExtraCells())
                        posy+=cell->extraYCells()+1;
                else
                        posy++;
        else if(mode==Row)
                if(cell->isForceExtraCells())
                        posx+=cell->extraXCells()+1;
                else
                        posx++;
        else
                kdDebug(36001) << "Error in Series::mode" << endl;
        if(type==Linear)
            incr=incr+step;
        else if(type==Geometric)
            incr=incr*step;
        else
            kdDebug(36001) << "Error in Series::type" << endl;
        }
}



void KSpreadTable::setSelectionPercent( const QPoint &_marker ,bool b )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          if(!b)
                {
                c->setFaktor( 1.0 );
                c->setPrecision( 0 );
                c->setFormatNumber(KSpreadCell::Number);
                }
          else
                {
                c->setFaktor( 100.0 );
                c->setPrecision( 0 );
                c->setFormatNumber(KSpreadCell::Percentage);
                }

          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          if(!b)
                {
                c->setFaktor( 1.0 );
                c->setPrecision( 0 );
                c->setFormatNumber(KSpreadCell::Number);
                }
          else
                {
                c->setFaktor( 100.0 );
                c->setPrecision( 0 );
                c->setFormatNumber(KSpreadCell::Percentage);
                }
          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }

                cell->setDisplayDirtyFlag();

                if(!b )
                        {
                        cell->setFaktor( 1.0 );
                        cell->setPrecision( 0 );
                        cell->setFormatNumber(KSpreadCell::Number);
                        }
                else
                        {
                        cell->setFaktor( 100.0 );
                        cell->setPrecision( 0 );
                        cell->setFormatNumber(KSpreadCell::Percentage);
                        }
                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::changeCellTabName(QString old_name,QString new_name)
{
    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if(c->isFormular() || c->content()==KSpreadCell::RichText)
        {
            if(c->text().find(old_name)!=-1)
            {
                int nb = c->text().contains(old_name+"!");
                QString tmp=old_name+"!";
                int len=tmp.length();
                tmp=c->text();

                for(int i=0;i<nb;i++)
                {
                    int pos= tmp.find(old_name+"!");
                    tmp.replace(pos,len,new_name+"!");
                }
                c->setCellText(tmp);
            }
        }
    }
}

bool KSpreadTable::shiftRow( const QPoint &_marker )
{
    m_pDoc->setModified( true );

    bool res = m_cells.shiftRow( _marker );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( _marker, false, KSpreadTable::ColumnInsert, name() );
    refreshChart(_marker, false, KSpreadTable::ColumnInsert);
    refreshMergedCell();
    emit sig_updateView( this );

    return res;
}

bool KSpreadTable::shiftColumn( const QPoint& marker )
{
    m_pDoc->setModified( true );

    bool res = m_cells.shiftColumn( marker );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( marker, false, KSpreadTable::RowInsert, name() );
    refreshChart(marker, false, KSpreadTable::RowInsert);
    refreshMergedCell();
    emit sig_updateView( this );

    return res;
}

void KSpreadTable::unshiftColumn( const QPoint& marker )
{
    m_pDoc->setModified( true );
    m_cells.remove(marker.x(),marker.y());
    m_cells.unshiftColumn( marker );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( marker, false, KSpreadTable::RowRemove, name() );
    refreshChart( marker, false, KSpreadTable::RowRemove );
    refreshMergedCell();
    emit sig_updateView( this );
}

void KSpreadTable::unshiftRow( const QPoint& marker )
{
    m_pDoc->setModified( true );
    m_cells.remove(marker.x(),marker.y());
    m_cells.unshiftRow( marker );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( marker, false, KSpreadTable::ColumnRemove, name() );
    refreshChart( marker, false, KSpreadTable::ColumnRemove );
    refreshMergedCell();
    emit sig_updateView( this );
}

bool KSpreadTable::insertColumn( int col )
{
    KSpreadUndoInsertColumn *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoInsertColumn( m_pDoc, this, col );
        m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_pDoc->setModified( true );

    bool res = m_cells.insertColumn( col );
    m_columns.insertColumn( col );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true, KSpreadTable::ColumnInsert, name() );
    refreshChart( QPoint( col, 1 ), true, KSpreadTable::ColumnInsert );
    refreshMergedCell();
    emit sig_updateHBorder( this );
    emit sig_updateView( this );

    return res;
}

bool KSpreadTable::insertRow( int row )
{
    KSpreadUndoInsertRow *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoInsertRow( m_pDoc, this, row );
        m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_pDoc->setModified( true );

    bool res = m_cells.insertRow( row );
    m_rows.insertRow( row );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true, KSpreadTable::RowInsert, name() );
    refreshChart( QPoint( 1, row ), true, KSpreadTable::RowInsert );
    refreshMergedCell();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );

    return res;
}

void KSpreadTable::removeColumn( int col )
{
    KSpreadUndoRemoveColumn *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoRemoveColumn( m_pDoc, this, col );
        m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_pDoc->setModified( true );

    m_cells.removeColumn( col );
    m_columns.removeColumn( col );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true, KSpreadTable::ColumnRemove, name() );
    refreshChart( QPoint( col, 1 ), true, KSpreadTable::ColumnRemove );
    refreshMergedCell();
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}

void KSpreadTable::removeRow( int row )
{
    KSpreadUndoRemoveRow *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoRemoveRow( m_pDoc, this, row );
        m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    m_pDoc->setModified( true );

    m_cells.removeRow( row );
    m_rows.removeRow( row );

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true, KSpreadTable::RowRemove, name() );
    refreshChart(QPoint( 1, row ), true, KSpreadTable::RowRemove);
    refreshMergedCell();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void KSpreadTable::refreshChart(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref)
{
KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
  if((ref==ColumnInsert || ref==ColumnRemove) && fullRowOrColumn
        && c->column()>=(pos.x()-1))
        {
        if(c->updateChart())
                return;
        }
  else if((ref==ColumnInsert || ref==ColumnRemove)&& !fullRowOrColumn
        && c->column()>=(pos.x()-1) && c->row()==pos.y())
        {
        if(c->updateChart())
                return;
        }
  else if((ref==RowInsert|| ref==RowRemove) && fullRowOrColumn
        && c->row()>=(pos.y()-1))
        {
        if(c->updateChart())
                return;
        }
  else if((ref==RowInsert || ref==RowRemove)&& !fullRowOrColumn
        && c->column()==pos.x() && c->row()>=(pos.y()-1))
        {
        if(c->updateChart())
                return;
        }
  }
  //refresh chart when there is a chart and you remove
  //all cells
  if(c==0L)
        {
        CellBinding *bind=firstCellBinding();
        if(bind!=0L)
                bind->cellChanged( 0 );
        }
}

void KSpreadTable::refreshMergedCell()
{
KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
  if(c->isForceExtraCells())
        c->forceExtraCells( c->column(), c->row(), c->extraXCells(), c->extraYCells() );
  }

}


void KSpreadTable::changeNameCellRef(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref, QString tabname)
{
  bool correctDefaultTableName = (tabname == name()); // for cells without table ref (eg "A1")
  KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    if(c->isFormular())
    {
      QString origText = c->text();
      unsigned int i = 0;
      QString newText;

      bool correctTableName = correctDefaultTableName;
      //bool previousCorrectTableName = false;
      for ( ; i < origText.length(); ++i )
      {
        QChar origCh = origText[i];
        if ( origCh != ':' && origCh != '$' && !origCh.isLetter() )
        {
          newText += origCh;
          // Reset the "correct table indicator"
          correctTableName = correctDefaultTableName;
        }
        else // Letter or dollar : maybe start of cell name/range
          // (or even ':', like in a range - note that correctTable is kept in this case)
        {
          // Collect everything that forms a name (cell name or table name)
          QString str;
          for( ; i < origText.length() &&
                  (origText[i].isLetter() || origText[i].isDigit()
                   || origText[i].isSpace() || origText[i] == '$')
                   ; ++i )
            str += origText[i];

          // Was it a table name ?
          if ( origText[i] == '!' )
          {
            newText += str + '!'; // Copy it (and the '!')
            // Look for the table name right before that '!'
            correctTableName = ( newText.right( tabname.length()+1 ) == tabname+"!" );
          }
          else // It must be a cell identifier
          {
            // Parse it
            KSpreadPoint point( str );
            if (point.isValid())
            {
              int col = point.pos.x();
              int row = point.pos.y();
              // Update column
              if ( point.columnFixed )
                newText += '$' + util_columnLabel( col );
              else
              {
                if(ref==ColumnInsert
                   && correctTableName
                   && col>=pos.x()     // Column after the new one : +1
                   && ( fullRowOrColumn || row == pos.y() ) ) // All rows or just one
                {
                  newText += util_columnLabel(col+1);
                }
                else if(ref==ColumnRemove
                        && correctTableName
                        && col > pos.x() // Column after the deleted one : -1
                        && ( fullRowOrColumn || row == pos.y() ) ) // All rows or just one
                {
                  newText += util_columnLabel(col-1);
                }
                else
                  newText += util_columnLabel(col);
              }
              // Update row
              if ( point.rowFixed )
                newText += '$' + QString::number( row );
              else
              {
                if(ref==RowInsert
                   && correctTableName
                   && row >= pos.y() // Row after the new one : +1
                   && ( fullRowOrColumn || col == pos.x() ) ) // All columns or just one
                {
                  newText += QString::number( row+1 );
                }
                else if(ref==RowRemove
                        && correctTableName
                        && row > pos.y() // Column after the deleted one : -1
                        && ( fullRowOrColumn || col == pos.x() ) ) // All columns or just one
                {
                  newText += QString::number( row-1 );
                }
                else
                  newText += QString::number( row );
              }
            }
            else // Not a cell ref
            {
              //kdDebug(36001) << "Copying (unchanged) : " << str << endl;
              newText += str;
            }
            // Copy the char that got us to stop
            newText += origText[i];
          }
        }
      }
      c->setCellText(newText, false /* no recalc deps for each, done independently */ );
    }
  }
}

bool KSpreadTable::replace( const QPoint &_marker, QString _find, QString _replace,
                            bool b_sensitive, bool b_whole )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );
    bool b_replace=false;
    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, m_rctSelection );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
        KSpreadCell* c = m_cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
            && !c->isObscured())
            {
                if(!c->isValue() && !c->isBool() &&!c->isFormular() &&!c->isDefault()&&!c->text().isEmpty())
                {
                    QString text;
                    if((text=replaceText(c->text(), _find, _replace,b_sensitive,b_whole))!=c->text())
                    {
                        c->setDisplayDirtyFlag();
                        c->setCellText(text);
                        c->clearDisplayDirtyFlag();
                        b_replace=true;
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
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, m_rctSelection );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
        KSpreadCell* c = m_cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
            && !c->isObscured())
            {
                if(!c->isValue() && !c->isBool() &&!c->isFormular() &&!c->isDefault()&&!c->text().isEmpty())
                {
                    QString text;
                    if((text=replaceText(c->text(), _find, _replace,b_sensitive,b_whole))!=c->text())
                    {
                        c->setDisplayDirtyFlag();
                        c->setCellText(text);
                        c->clearDisplayDirtyFlag();
                        b_replace=true;
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

        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, r );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
        for ( int x = r.left(); x <= r.right(); x++ )
            for ( int y = r.top(); y <= r.bottom(); y++ )
            {
                KSpreadCell *cell = cellAt( x, y );
                if( !cell->isObscured())
                {
                  if ( cell == m_pDefaultCell )
                  {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                  }
                  if(!cell->isValue() && !cell->isBool() &&!cell->isFormular() &&!cell->isDefault()&&!cell->text().isEmpty())
                  {
                    QString text;
                    if((text=replaceText(cell->text(), _find, _replace,b_sensitive,b_whole))!=cell->text())
                    {
                        cell->setDisplayDirtyFlag();
                        cell->setCellText(text);
                        cell->clearDisplayDirtyFlag();
                        b_replace=true;
                    }
                  }
                }
            }

        emit sig_updateView( this, r );
        return b_replace;
    }
}

QString KSpreadTable::replaceText(QString _cellText,QString _find,QString _replace, bool b_sensitive,bool b_whole)
{
    QString findText;
    QString replaceText;
    QString realCellText;
    int index=0;
    int lenreplace=0;
    int lenfind=0;
    if(b_sensitive)
    {
        realCellText=_cellText;
        findText=_find;
        replaceText=_replace;
        if( realCellText.find(findText) >=0)
        {
            do
            {
                index=realCellText.find(findText,index+lenreplace);
                int len=realCellText.length();
                lenfind=findText.length();
                lenreplace=replaceText.length();
                if(!b_whole)
                    realCellText=realCellText.left(index)+replaceText+realCellText.right(len-index-lenfind);
                else if(b_whole)
                {
                    if (((index==0 || realCellText.mid(index-1,1)==" ")
                         && (realCellText.mid(index+lenfind,1)==" "||(index+lenfind)==len)))
                        realCellText=realCellText.left(index)+replaceText+realCellText.right(len-index-lenfind);
                }
            }
            while( realCellText.find(findText,index+lenreplace) >=0);
        }
        return realCellText;
    }
    else
    {
        realCellText=_cellText;
        findText=_find.lower();
        replaceText=_replace;
        if( realCellText.lower().find(findText) >=0)
        {
            do
            {
                index=realCellText.lower().find(findText,index+lenreplace);
                int len=realCellText.length();
                lenfind=findText.length();
                lenreplace=replaceText.length();
                if(!b_whole)
                    realCellText=realCellText.left(index)+replaceText+realCellText.right(len-index-lenfind);
                else if(b_whole)
                {
                    if (((index==0 || realCellText.mid(index-1,1)==" ")
                         && (realCellText.mid(index+lenfind,1)==" "||(index+lenfind)==len)))
                        realCellText=realCellText.left(index)+replaceText+realCellText.right(len-index-lenfind);
                }
            }
            while( realCellText.lower().find(findText,index+lenreplace) >=0);
        }
        return realCellText;
    }
}

void KSpreadTable::borderBottom( const QPoint &_marker,const QColor &_color )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left()==0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }
    for ( int x = r.left(); x <= r.right(); x++ )
    {
        int y = r.bottom();
        KSpreadCell *cell = cellAt( x, y );
        if(!cell->isObscuringForced())
        {
        if ( cell == m_pDefaultCell )
        {
            cell = new KSpreadCell( this, x, y );
            m_cells.insert( cell, x, y );
        }
        cell->setBottomBorderStyle( SolidLine );
        cell->setBottomBorderColor( _color );
        cell->setBottomBorderWidth( 2 );
        }
    }
    emit sig_updateView( this, r );
}

void KSpreadTable::borderRight( const QPoint &_marker,const QColor &_color )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left() == 0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
        int x = r.right();
        KSpreadCell *cell = nonDefaultCell( x, y );
        if(!cell->isObscuringForced())
        {
        cell->setRightBorderStyle( SolidLine );
        cell->setRightBorderColor( _color );
        cell->setRightBorderWidth( 2 );
        }
    }

    emit sig_updateView( this, r );
}

void KSpreadTable::borderLeft( const QPoint &_marker, const QColor &_color )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left()==0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }
    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
        int x = r.left();
        KSpreadCell *cell = nonDefaultCell( x, y );
        if(!cell->isObscuringForced())
        {
        cell->setLeftBorderStyle( SolidLine );
        cell->setLeftBorderColor( _color );
        cell->setLeftBorderWidth( 2 );
        }
    }
    emit sig_updateView( this, r );
}

void KSpreadTable::borderTop( const QPoint &_marker,const QColor &_color )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left()==0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }
    for ( int x = r.left(); x <= r.right(); x++ )
    {
        int y = r.top();
        KSpreadCell *cell = cellAt( x, y );
        if(!cell->isObscuringForced())
        {
        if ( cell == m_pDefaultCell )
        {
            cell = new KSpreadCell( this, x, y );
            m_cells.insert( cell, x, y );
        }
        cell->setTopBorderStyle( SolidLine );
        cell->setTopBorderColor( _color );
        cell->setTopBorderWidth( 2 );
        }
    }
    emit sig_updateView( this, r );
}

void KSpreadTable::borderOutline( const QPoint &_marker,const QColor &_color )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left()==0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }
    for ( int x = r.left(); x <= r.right(); x++ )
    {
        int y = r.top();
        KSpreadCell *cell = cellAt( x, y );
        if(!cell->isObscuringForced())
        {
         if ( cell == m_pDefaultCell )
         {
            cell = new KSpreadCell( this, x, y );
            m_cells.insert( cell, x, y );
         }
         cell->setTopBorderStyle( SolidLine );
         cell->setTopBorderColor( _color );
         cell->setTopBorderWidth( 2 );
        }
    }
    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
        int x = r.left();
        KSpreadCell *cell = nonDefaultCell( x, y );
        if(!cell->isObscuringForced())
        {
        cell->setLeftBorderStyle( SolidLine );
        cell->setLeftBorderColor( _color );
        cell->setLeftBorderWidth( 2 );
        }
    }
    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
        int x = r.right();
        KSpreadCell *cell = nonDefaultCell( x, y );
        if(!cell->isObscuringForced())
        {
        cell->setRightBorderStyle( SolidLine );
        cell->setRightBorderColor( _color );
        cell->setRightBorderWidth( 2 );
        }
    }
    for ( int x = r.left(); x <= r.right(); x++ )
    {
        int y = r.bottom();
        KSpreadCell *cell = cellAt( x, y );
        if(!cell->isObscuringForced())
        {
        if ( cell == m_pDefaultCell )
        {
            cell = new KSpreadCell( this, x, y );
            m_cells.insert( cell, x, y );
        }
        cell->setBottomBorderStyle( SolidLine );
        cell->setBottomBorderColor( _color );
        cell->setBottomBorderWidth( 2 );
        }
    }

    emit sig_updateView( this, r );

}

void KSpreadTable::borderAll( const QPoint &_marker,const QColor &_color )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left()==0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }

    for ( int x = r.left(); x <= r.right(); x++ )
    {
        for(int y=r.top();y<=r.bottom();y++)
        {
            KSpreadCell *cell = cellAt( x, y );
            if(!cell->isObscuringForced())
            {
             if ( cell == m_pDefaultCell )
             {
                cell = new KSpreadCell( this, x, y );
                m_cells.insert( cell, x, y );
             }
             cell->setBottomBorderStyle( SolidLine );
             cell->setBottomBorderColor( _color );
             cell->setBottomBorderWidth( 2 );
             cell->setRightBorderStyle( SolidLine );
             cell->setRightBorderColor( _color );
             cell->setRightBorderWidth( 2 );
             cell->setLeftBorderStyle( SolidLine );
             cell->setLeftBorderColor( _color );
             cell->setLeftBorderWidth( 2 );
             cell->setTopBorderStyle( SolidLine );
             cell->setTopBorderColor( _color );
             cell->setTopBorderWidth( 2 );
             }
        }
    }
    emit sig_updateView( this, r );
}

void KSpreadTable::borderRemove( const QPoint &_marker )
{
    QRect r( m_rctSelection );
    if ( m_rctSelection.left()==0 )
        r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

    KSpreadUndoCellLayout *undo;
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        undo = new KSpreadUndoCellLayout( m_pDoc, this, r );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    for ( int x = r.left(); x <= r.right(); x++ )
    {
        for(int y = r.top();y <= r.bottom(); y++ )
        {
            KSpreadCell *cell = cellAt( x, y );
            if(!cell->isObscuringForced())
            {
             cell->setBottomBorderStyle( NoPen );
             cell->setBottomBorderColor( black );
             cell->setBottomBorderWidth( 1 );
             cell->setRightBorderStyle( NoPen );
             cell->setRightBorderColor( black );
             cell->setRightBorderWidth( 1 );
             cell->setLeftBorderStyle( NoPen );
             cell->setLeftBorderColor( black );
             cell->setLeftBorderWidth( 1 );
             cell->setTopBorderStyle( NoPen );
             cell->setTopBorderColor( black );
             cell->setTopBorderWidth( 1 );
             cell->setFallDiagonalStyle( NoPen );
             cell->setFallDiagonalColor( black );
             cell->setFallDiagonalWidth( 1 );
             cell->setGoUpDiagonalStyle( NoPen );
             cell->setGoUpDiagonalColor( black );
             cell->setGoUpDiagonalWidth( 1 );
            }
        }

    }
    emit sig_updateView( this, r );
}


void KSpreadTable::sortByRow( int ref_row, SortingOrder mode )
{
    QRect r( selectionRect() );
    ASSERT( mode == Increase || mode == Decrease );

    // It may not happen that entire columns are selected.
    ASSERT( r.right() != 0x7fff );

    // Are entire rows selected ?
    if ( r.right() == 0x7FFF )
    {
        r.setLeft( 0x7fff );
        r.setRight( 0 );

        // Determine a correct left and right.
        // Iterate over all cells to find out which cells are
        // located in the selected rows.
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            int col = c->column();

            // Is the cell in the selected columns ?
            if ( !c->isEmpty() && row >= r.top() && row <= r.bottom())
            {
                if ( col > r.right() )
                    r.rRight() = col;
                if ( col < r.left() )
                    r.rLeft() = col;
            }
        }

        // Any cells to sort here ?
        if ( r.right() < r.left() )
            return;
    }

    // Sorting algorithm: David's :). Well, I guess it's called minmax or so.
    // For each column, we look for all cells right hand of it and we find the one to swap with it.
    // Much faster than the awful bubbleSort...
    for ( int d = r.left();  d <= r.right(); d++ )
    {
        KSpreadCell *cell1 = cellAt( d, ref_row  );
        // Look for which column we want to swap with the one number d
        KSpreadCell * bestCell = cell1;
        int bestX = d;

        for ( int x = d + 1 ; x <= r.right(); x++ )
        {
            KSpreadCell *cell2 = cellAt( x, ref_row );

            if ( cell2->isEmpty() )
            { /* No need to swap */ }
            else if ( bestCell->isEmpty() )
            {
                // empty cells are always shifted to the end
                bestCell = cell2;
                bestX = x;
            }
            // Here we use the operators < and > for cells, which do it all.
            else if ( (mode == Increase && *cell2 < *bestCell) ||
                      (mode == Decrease && *cell2 > *bestCell) )
            {
                bestCell = cell2;
                bestX = x;
            }
        }

        // Swap columns cell1 and bestCell (i.e. d and bestX)
        if ( d != bestX )
        {
            for( int y = r.top(); y <= r.bottom(); y++ )
                swapCells( d, y, bestX, y );
        }

    }

    emit sig_updateView( this, r );
}

void KSpreadTable::sortByColumn(int ref_column,SortingOrder mode)
{
    ASSERT( mode == Increase || mode == Decrease );

    QRect r( selectionRect() );

    // It may not happen that entire rows are selected.
    ASSERT( r.right() != 0x7fff );

    // Are entire columns selected ?
    if ( r.bottom() == 0x7FFF )
    {
        r.setTop( 0x7fff );
        r.setBottom( 0 );

        // Determine a correct top and bottom.
        // Iterate over all cells to find out which cells are
        // located in the selected columns.
        KSpreadCell* c = m_cells.firstCell();
        for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            int col = c->column();

            // Is the cell in the selected columns ?
            if ( !c->isEmpty() && col >= r.left() && col <= r.right())
            {
                if ( row > r.bottom() )
                    r.rBottom() = row;
                if ( row < r.top() )
                    r.rTop() = row;
            }
        }

        // Any cells to sort here ?
        if ( r.bottom() < r.top() )
            return;
    }

    // Sorting algorithm: David's :). Well, I guess it's called minmax or so.
    // For each row, we look for all rows under it and we find the one to swap with it.
    // Much faster than the awful bubbleSort...
    // Torben: Asymptotically it is alltogether O(n^2) :-)
    for ( int d = r.top(); d <= r.bottom(); d++ )
    {
        // Look for which row we want to swap with the one number d
        KSpreadCell *cell1 = cellAt( ref_column, d );
        //kdDebug() << "New ref row " << d << endl;

        KSpreadCell * bestCell = cell1;
        int bestY = d;

        for ( int y = d + 1 ; y <= r.bottom(); y++ )
        {
            KSpreadCell *cell2 = cellAt( ref_column, y );

            if ( cell2->isEmpty() )
            { /* No need to swap */ }
            else if ( bestCell->isEmpty() )
            {
                // empty cells are always shifted to the end
                bestCell = cell2;
                bestY = y;
            }
            // Here we use the operators < and > for cells, which do it all.
            else if ( (mode==Increase && *cell2 < *bestCell) ||
                 (mode==Decrease && *cell2 > *bestCell) )
            {
                bestCell = cell2;
                bestY = y;
            }
        }

        // Swap rows cell1 and bestCell (i.e. d and bestY)
        if ( d != bestY )
        {
            for(int x=r.left();x<=r.right();x++)
                swapCells( x, d, x, bestY );
        }
    }

    emit sig_updateView( this, r );
}

void KSpreadTable::swapCells( int x1, int y1, int x2, int y2 )
{
  KSpreadCell *ref1 = cellAt( x1, y1 );
  KSpreadCell *ref2 = cellAt( x2, y2 );
  if ( ref1->isDefault() )
  {
    if ( !ref2->isDefault() )
    {
      ref1 = nonDefaultCell( x1, y1 );
      // TODO : make ref2 default instead of copying a default cell into it
    }
    else
      return; // nothing to do
  }
  else
    if ( ref2->isDefault() )
    {
      ref2 = nonDefaultCell( x2, y2 );
      // TODO : make ref1 default instead of copying a default cell into it
    }

  // Dummy cell used for swapping cells.
  // In fact we copy only content and no layout
  // information. Imagine sortting in a table. Swapping
  // the layout while sorting is not what you would expect
  // as a user.
  KSpreadCell *tmp = new KSpreadCell( this, -1, -1 );
  tmp->copyContent( ref1 );
  ref1->copyContent( ref2 );
  ref2->copyContent( tmp );
  delete tmp;
}

void KSpreadTable::refreshPreference()
{
        recalc();
        emit sig_updateHBorder( this );
        emit sig_updateView( this );
}

bool KSpreadTable::areaIsEmpty()
{
    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
      int row = c->row();
      if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
      &&!c->isObscuringForced() && !c->text().isEmpty())
        {
        return false;
        }
      }
    }
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced() && !c->text().isEmpty())
        {
        return false;
        }
      }
    }
    else
    {
        QRect r( m_rctSelection );
        if ( !selected )
            r.setCoords( marker().x(), marker().y(), marker().x(), marker().y() );

        for ( int x = r.left(); x <= r.right(); x++ )
            for ( int y = r.top(); y <= r.bottom(); y++ )
            {
                KSpreadCell *cell = cellAt( x, y );
                if(!cell->isObscuringForced() && !cell->text().isEmpty())
                {
                return false;
                }
            }
    }
    return true;
}

void KSpreadTable::setSelectionMultiRow( const QPoint &_marker, bool enable )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setMultiRow( enable );
          c->setVerticalText( false );
          c->setAngle(0);
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setMultiRow( enable );
          c->setVerticalText( false );
          c->setAngle(0);
          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }

                cell->setDisplayDirtyFlag();
                cell->setMultiRow( enable );
                cell->setVerticalText( false );
                cell->setAngle(0);
                cell->clearDisplayDirtyFlag();
                }
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
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setAlign( _align );
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setAlign( _align );
          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }

                cell->setDisplayDirtyFlag();
                cell->setAlign( _align );
                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionAlignY( const QPoint &_marker, KSpreadLayout::AlignY _alignY )
{
    m_pDoc->setModified( true );

    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setAlignY( _alignY );
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          c->setAlignY( _alignY );
          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }

                cell->setDisplayDirtyFlag();
                cell->setAlignY( _alignY );
                cell->clearDisplayDirtyFlag();
                }
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
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          if ( _delta == 1 )
            c->incPrecision();
          else
            c->decPrecision();
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          if ( _delta == 1 )
            c->incPrecision();
          else
            c->decPrecision();
          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {
                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }

                cell->setDisplayDirtyFlag();

                if ( _delta == 1 )
                  cell->incPrecision();
                else
                  cell->decPrecision();

                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setSelectionMoneyFormat( const QPoint &_marker,bool b )
{
    m_pDoc->setModified( true );
    bool selected = ( m_rctSelection.left() != 0 );
    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          if(b)
                {
                c->setFormatNumber(KSpreadCell::Money);
                c->setFaktor( 1.0 );
                c->setPrecision( KGlobal::locale()->fracDigits() );
                }
          else
                {
                c->setFormatNumber(KSpreadCell::Number);
                c->setFaktor( 1.0 );
                c->setPrecision( 0 );
                }
          c->clearDisplayDirtyFlag();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        &&!c->isObscuringForced())
        {
          c->setDisplayDirtyFlag();
          if(b)
                {
                c->setFormatNumber(KSpreadCell::Money);
                c->setFaktor( 1.0 );
                c->setPrecision( KGlobal::locale()->fracDigits() );
                }
          else
                {
                c->setFormatNumber(KSpreadCell::Number);
                c->setFaktor( 1.0 );
                c->setPrecision( 0 );
                }
          c->clearDisplayDirtyFlag();
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
                if(!cell->isObscuringForced())
                {

                 if ( cell == m_pDefaultCell )
                 {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                 }


                cell->setDisplayDirtyFlag();
                if(b)
                        {
                        cell->setFormatNumber(KSpreadCell::Money);
                        cell->setFaktor( 1.0 );
                        cell->setPrecision( KGlobal::locale()->fracDigits() );
                        }
                else
                        {
                        cell->setFormatNumber(KSpreadCell::Number);
                        cell->setFaktor( 1.0 );
                        cell->setPrecision( 0 );
                        }
                cell->clearDisplayDirtyFlag();
                }
                }

        emit sig_updateView( this, r );
    }
}

int KSpreadTable::adjustColumn( const QPoint& _marker, int _col )
{
    int long_max=0;
    if( _col == -1 )
    {
        if ( m_rctSelection.left() != 0 && m_rctSelection.bottom() == 0x7FFF )
        {
            KSpreadCell* c = m_cells.firstCell();
            for( ;c; c = c->nextCell() )
            {
                int col = c->column();
                if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
                {
                    if( !c->isEmpty() && !c->isObscured())
                    {
                        c->conditionAlign(painter(),col,c->row());
                        if( c->textWidth() > long_max )
                                {
                                long_max = c->textWidth() +
                                       c->leftBorderWidth(c->column(),c->row() ) +
                                       c->rightBorderWidth(c->column(),c->row() );
                                }

                    }
                }
            }
        }

    }
    else
    {
        QRect r( m_rctSelection );
        if( r.left() == 0 || r.right() == 0 || r.top() == 0 || r.bottom() == 0 )
        {
            r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
        }
        int x = _col;
        for ( int y = r.top(); y <= r.bottom(); y++ )
        {
            KSpreadCell *cell = cellAt( x, y );
            if( cell != m_pDefaultCell && !cell->isEmpty()
            && !cell->isObscured())
            {
                   cell->conditionAlign(painter(),x,y);
                   if(cell->textWidth() > long_max )
                                {
                                long_max = cell->textWidth() +
                                cell->leftBorderWidth(cell->column(),cell->row() ) +
                                cell->rightBorderWidth(cell->column(),cell->row() );
                                }

            }
        }


    }
    //add 4 because long_max is the long of the text
    //but column has borders
    if( long_max == 0 )
        return -1;
    else
        return ( long_max + 4 );
}

int KSpreadTable::adjustRow(const QPoint &_marker,int _row)
{
    int long_max=0;
    if(_row==-1)
    {
        if ( m_rctSelection.left() != 0 && m_rctSelection.right() == 0x7FFF )
        {
            KSpreadCell* c = m_cells.firstCell();
            for( ;c; c = c->nextCell() )
            {
                int row = c->row();
                if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
                {
                    if(!c->isEmpty() && !c->isObscured())
                    {
                        c->conditionAlign(painter(),c->column(),row);
                        if(c->textHeight()>long_max)
                            long_max = c->textHeight() +
                                       c->topBorderWidth(c->column(),c->row() ) +
                                       c->bottomBorderWidth(c->column(),c->row() );

                    }
                }
            }
        }
    }
    else
    {
        QRect r( m_rctSelection );
        if( r.left() == 0 || r.right() == 0 || r.top() == 0 || r.bottom() == 0 )
        {
            r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
        }
        int y=_row;
        for ( int x = r.left(); x <= r.right(); x++ )
        {
            KSpreadCell *cell = cellAt( x, y );
            if(cell != m_pDefaultCell && !cell->isEmpty()
            && !cell->isObscured())
            {
                cell->conditionAlign(painter(),x,y);
                if(cell->textHeight()>long_max )
                    long_max = cell->textHeight() +
                               cell->topBorderWidth(cell->column(),cell->row() ) +
                               cell->bottomBorderWidth(cell->column(),cell->row() );


            }
        }
    }

    //add 4 because long_max is the long of the text
    //but column has borders
    if( long_max == 0 )
        return -1;
    else
        return ( long_max + 4 );
}

void KSpreadTable::clearSelection( const QPoint &_marker )
{
    m_pDoc->setModified( true );
    bool selected = ( m_rctSelection.left() != 0 );

    if(areaIsEmpty())
        return;

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      if ( !m_pDoc->undoBuffer()->isLocked() )
      {
        KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, m_rctSelection );
        m_pDoc->undoBuffer()->appendUndo( undo );
      }

      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
        && !c->isObscured())
        {
          c->setCellText("");
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      if ( !m_pDoc->undoBuffer()->isLocked() )
      {
        KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, m_rctSelection );
        m_pDoc->undoBuffer()->appendUndo( undo );
      }
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
        && !c->isObscured())
        {
          c->setCellText("");
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

        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, r );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }

        for ( int x = r.left(); x <= r.right(); x++ )
            for ( int y = r.top(); y <= r.bottom(); y++ )
            {
                KSpreadCell *cell = cellAt( x, y );

                if(!cell->isObscured())
                {
                   if ( cell == m_pDefaultCell )
                   {
                        cell = new KSpreadCell( this, x, y );
                        m_cells.insert( cell, x, y );
                   }

                   cell->setCellText("");
                }
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::defaultSelection( const QPoint &_marker )
{
    m_pDoc->setModified( true );
    bool selected = ( m_rctSelection.left() != 0 );

    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row )
        {
          c->defaultStyle();
        }
      }

      emit sig_updateView( this, m_rctSelection );
      return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
      KSpreadCell* c = m_cells.firstCell();
      for( ;c; c = c->nextCell() )
      {
        int col = c->column();
        if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col )
        {
          c->defaultStyle();
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
                    m_cells.insert( cell, x, y );
                }

                cell->defaultStyle();
            }

        emit sig_updateView( this, r );
    }
}

void KSpreadTable::setConditional( const QPoint &_marker,KSpreadConditional tmp[3] )
{
    m_pDoc->setModified( true );
    KSpreadConditional *tmpCondition=0;

    bool selected = ( m_rctSelection.left() != 0 );
    // Complete rows selected ?
    if ( selected && m_rctSelection.right() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
            && !c->isObscured())
            {
                c->setDisplayDirtyFlag();
                for(int i=0;i<3;i++)
                {
                    switch(i)
                    {
                    case 0:

                        if(tmp[i].m_cond==None)
                            c->removeFirstCondition();
                        else
                        {
                            tmpCondition=c->getFirstCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }
                        break;
                    case 1:

                        if(tmp[i].m_cond==None)
                            c->removeSecondCondition();
                        else
                        {
                            tmpCondition=c->getSecondCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }

                        break;
                    case 2:

                        if(tmp[i].m_cond==None)
                            c->removeThirdCondition();
                        else
                        {
                            tmpCondition=c->getThirdCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }

                        break;
                    }
                 }
                c->clearDisplayDirtyFlag();
            }
        }

        emit sig_updateView( this, m_rctSelection );
        return;
    }
    // Complete columns selected ?
    else if ( selected && m_rctSelection.bottom() == 0x7FFF )
    {
        KSpreadCell* c = m_cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
            && !c->isObscured())
            {
                c->setDisplayDirtyFlag();

                for(int i=0;i<3;i++)
                {
                    switch(i)
                    {
                    case 0:

                        if(tmp[i].m_cond==None)
                            c->removeFirstCondition();
                        else
                    {
                        tmpCondition=c->getFirstCondition();
                        tmpCondition->val1=tmp[i].val1;
                        tmpCondition->val2=tmp[i].val2;
                        tmpCondition->colorcond=tmp[i].colorcond;
                        tmpCondition->fontcond=tmp[i].fontcond;
                        tmpCondition->m_cond=tmp[i].m_cond;
                    }
                        break;
                    case 1:

                        if(tmp[i].m_cond==None)
                            c->removeSecondCondition();
                        else
                        {
                            tmpCondition=c->getSecondCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }
                        break;
                    case 2:

                        if(tmp[i].m_cond==None)
                            c->removeThirdCondition();
                        else
                        {
                            tmpCondition=c->getThirdCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }

                        break;
                    }
                }
                c->clearDisplayDirtyFlag();
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

        for ( int x = r.left(); x <= r.right(); x++ )
            for ( int y = r.top(); y <= r.bottom(); y++ )
            {
                KSpreadCell *cell = cellAt( x, y );

                if ( cell == m_pDefaultCell )
                {
                    cell = new KSpreadCell( this, x, y );
                    m_cells.insert( cell, x, y );
                }

                if (!cell->isObscured())
                {

                cell->setDisplayDirtyFlag();
                for(int i=0;i<3;i++)
                {
                    switch(i)
                    {
                    case 0:

                        if(tmp[i].m_cond==None)
                            cell->removeFirstCondition();
                        else
                        {
                            tmpCondition=cell->getFirstCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }
                        break;
                    case 1:

                        if(tmp[i].m_cond==None)
                            cell->removeSecondCondition();
                        else
                        {
                            tmpCondition=cell->getSecondCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }
                        break;
                    case 2:

                        if(tmp[i].m_cond==None)
                            cell->removeThirdCondition();
                        else
                        {
                            tmpCondition=cell->getThirdCondition();
                            tmpCondition->val1=tmp[i].val1;
                            tmpCondition->val2=tmp[i].val2;
                            tmpCondition->colorcond=tmp[i].colorcond;
                            tmpCondition->fontcond=tmp[i].fontcond;
                            tmpCondition->m_cond=tmp[i].m_cond;
                        }

                        break;
                    }
                }
                cell->clearDisplayDirtyFlag();
                }
            }

        emit sig_updateView( this, r );
    }
}


void KSpreadTable::copySelection( const QPoint &_marker )
{
    QRect rct;

    // No selection ? => copy active cell
    if ( m_rctSelection.left() == 0 )
        rct.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );
    else
        rct = selectionRect();

    QDomDocument doc = saveCellRect( rct );

    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    QCString s = buffer.utf8();

    printf("COPY %s\n", s.data() );

    int len = s.length();
    char tmp = s[ len - 1 ];
    s.resize( len );
    *( s.data() + len - 1 ) = tmp;

    buffer = QString::null;

    QStoredDrag* data = new QStoredDrag( "application/x-kspread-snippet" );
    data->setEncodedData( s );

    QApplication::clipboard()->setData( data );
}

void KSpreadTable::cutSelection( const QPoint &_marker )
{
    m_pDoc->setModified( true );

    copySelection( _marker );
    deleteSelection( _marker );
}

void KSpreadTable::paste( const QPoint &_marker, PasteMode sp, Operation op )
{
    QMimeSource* mime = QApplication::clipboard()->data();
    if ( !mime )
        return;

    QByteArray b;

    if ( mime->provides( "application/x-kspread-snippet" ) )
        b = mime->encodedData( "application/x-kspread-snippet" );
    else
        return;

    paste( b, _marker, sp, op );
}

void KSpreadTable::paste( const QByteArray& b, const QPoint &_marker, PasteMode sp, Operation op )
{
    kdDebug(36001) << "Parsing " << b.size() << " bytes" << endl;

    QBuffer buffer( b );
    buffer.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &buffer );
    buffer.close();

    // ##### TODO: Test for parsing errors

    loadSelection( doc, _marker.x() - 1, _marker.y() - 1, sp, op );
    m_pDoc->setModified( true );
}

bool KSpreadTable::loadSelection( const QDomDocument& doc, int _xshift, int _yshift, PasteMode sp, Operation op )
{
    QDomElement e = doc.documentElement();

    if ( !e.namedItem( "columns" ).toElement().isNull() )
    {
        _yshift = 0;

        QDomElement columns = e.namedItem( "columns" ).toElement();

        // Clear the existing columns
        int count = columns.attribute("count").toInt();
        for( int i = 1; i <= count; ++i )
        {
            m_cells.clearColumn( _xshift + i );
            m_columns.removeElement( _xshift + i );
        }

        // Insert column layouts
        QDomElement c = e.firstChild().toElement();
        for( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "column" )
            {
                ColumnLayout *cl = new ColumnLayout( this, 0 );
                if ( cl->load( c, _xshift ) )
                    insertColumnLayout( cl );
                else
                    delete cl;
            }
        }

    }

    if ( !e.namedItem( "rows" ).toElement().isNull() )
    {
        _xshift = 0;

        QDomElement rows = e.namedItem( "rows" ).toElement();

        // Clear the existing columns
        int count = rows.attribute("count").toInt();
        for( int i = 1; i <= count; ++i )
        {
            m_cells.clearRow( _yshift + i );
            m_rows.removeElement( _yshift + i );
        }

        // Insert row layouts
        QDomElement c = e.firstChild().toElement();
        for( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "row" )
            {
                RowLayout *cl = new RowLayout( this, 0 );
                if ( cl->load( c, _yshift ) )
                    insertRowLayout( cl );
                else
                    delete cl;
            }
        }
    }
    bool refreshChart=false;
    int refresCol=0;
    int refreshRow=0;
    QDomElement c = e.firstChild().toElement();
    for( ; !c.isNull(); c = c.nextSibling().toElement() )
    {
        if ( c.tagName() == "cell" )
        {
            int row = c.attribute( "row" ).toInt() + _yshift;
            int col = c.attribute( "column" ).toInt() + _xshift;

            bool needInsert = FALSE;
            KSpreadCell* cell = cellAt( col, row );
            if ( ( op == OverWrite && sp == Normal ) || cell->isDefault() )
            {
                cell = new KSpreadCell( this, 0, 0 );
                needInsert = TRUE;
            }
            if ( !cell->load( c, _xshift, _yshift, sp, op ) )
            {
                if ( needInsert )
                  delete cell;
            }
            else
              if ( needInsert )
                insertCell( cell );
            if(!refreshChart)
                {
                refreshChart=cell->updateChart(false);
                refresCol=col;
                refreshRow=row;
                }
        }
    }
    //refresh chart after that you paste all cells
    if(refreshChart)
        {
        KSpreadCell* cell = cellAt( refresCol, refreshRow );
        cell->updateChart();
        }
    m_pDoc->setModified( true );
    refreshMergedCell();
    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );

    return true;
}

void KSpreadTable::deleteCells( const QRect& rect )
{
    // A list of all cells we want to delete.
    QStack<KSpreadCell> cellStack;

    QRect tmpRect;
    bool extraCell=false;
    if(rect.width()==1 && rect.height()==1)
        {
        KSpreadCell *cell = nonDefaultCell( rect.x(), rect.y() );
        if(cell->isForceExtraCells())
                {
                extraCell=true;
                tmpRect=rect;
                }
        }
    else if(rect.contains(m_marker.x(),m_marker.y())
    &&m_rctSelection.left()==0)
        {
        KSpreadCell *cell = nonDefaultCell( m_marker.x(),m_marker.y() );
        if(cell->isForceExtraCells())
                {
                extraCell=true;
                tmpRect=QRect(m_marker.x(),m_marker.y(),1,1);
                }
        }

    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() && c->row() >= rect.top() &&
             c->row() <= rect.bottom() && c->column() >= rect.left() &&
             c->column() <= rect.right() )
          cellStack.push( c );
    }

    m_cells.setAutoDelete( false );

    // Remove the cells from the table
    while ( !cellStack.isEmpty() )
    {
        KSpreadCell *cell = cellStack.pop();

        m_cells.remove( cell->column(), cell->row() );
        cell->updateDepending();

        delete cell;
    }

    m_cells.setAutoDelete( true );

    setLayoutDirtyFlag();

    // Since obscured cells might have been deleted we
    // have to reenforce it.
    c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
        if ( c->isForceExtraCells() && !c->isDefault() )
            c->forceExtraCells( c->column(), c->row(), c->extraXCells(), c->extraYCells() );

    if(extraCell)
        {
        setSelection(tmpRect);
        unselect();
        }

    m_pDoc->setModified( true );

}

void KSpreadTable::deleteSelection( const QPoint& _marker )
{
    m_pDoc->setModified( true );

    QRect r( m_rctSelection );

    if ( r.left() == 0 )
        r = QRect( _marker.x(), _marker.y(), 1, 1 );

    // Entire rows selected ?
    if ( r.right() == 0x7fff )
    {
        for( int i = r.top(); i <= r.bottom(); ++i )
        {
            m_cells.clearRow( i );
            m_rows.removeElement( i );
        }

        emit sig_updateVBorder( this );
    }
    // Entire columns selected ?
    else if ( r.bottom() == 0x7fff )
    {
        for( int i = r.left(); i <= r.right(); ++i )
        {
            m_cells.clearColumn( i );
            m_columns.removeElement( i );
        }

        emit sig_updateHBorder( this );
    }
    else
    {
        KSpreadUndoDelete *undo;
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            undo = new KSpreadUndoDelete( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }

        deleteCells( r );
    }

    emit sig_updateView( this );
}

void KSpreadTable::refreshView(const QRect& rect)
{
    QRect tmp(rect);
    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() && c->row() >= rect.top() &&
             c->row() <= rect.bottom() && c->column() >= rect.left() &&
             c->column() <= rect.right() )
                if(c->isForceExtraCells())
                {
                int right=QMAX(tmp.right(),c->column()+c->extraXCells());
                int bottom=QMAX(tmp.bottom(),c->row()+c->extraYCells());

                tmp.setRight(right);
                tmp.setBottom(bottom);
                }
    }
    deleteCells( rect );
    emit sig_updateView( this, tmp );
}

void KSpreadTable::updateView(const QRect& rect)
{
    emit sig_updateView( this, rect );
}

void KSpreadTable::changeMergedCell( int m_iCol, int m_iRow, int m_iExtraX, int m_iExtraY)
{
   if( m_iExtraX==0 && m_iExtraY==0)
        {
        dissociateCell( QPoint( m_iCol,m_iRow),false);
        return;
        }
    KSpreadCell *cell = nonDefaultCell( m_iCol,m_iRow  );
    if(cell->isForceExtraCells())
        dissociateCell( QPoint( m_iCol,m_iRow),false);

    cell->forceExtraCells( m_iCol,m_iRow,
                           m_iExtraX,m_iExtraY);

    setMarker(QPoint(m_iCol,m_iRow));
    refreshMergedCell();
    QRect rect;
    rect.setCoords(m_iCol,m_iRow,m_iCol+m_iExtraX,m_iRow+m_iExtraY);
    emit sig_updateView( this, rect );

}

void KSpreadTable::mergeCell( const QPoint &_marker)
{
    if(m_rctSelection.left() == 0)
        return;
    int x=_marker.x();
    int y=_marker.y();
    if( _marker.x() > m_rctSelection.left() )
        x = m_rctSelection.left();
    if( _marker.y() > m_rctSelection.top() )
        y = m_rctSelection.top();
    KSpreadCell *cell = nonDefaultCell( x ,y  );

    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( m_pDoc, this, x ,y,cell->extraXCells() ,cell->extraYCells());
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    cell->forceExtraCells( x ,y,
                           abs(m_rctSelection.right() -m_rctSelection.left()),
                           abs(m_rctSelection.bottom() - m_rctSelection.top()));

    setMarker(QPoint(x,y));
    emit sig_updateView( this, m_rctSelection );
}

void KSpreadTable::dissociateCell( const QPoint &_marker,bool makeUndo)
{
    KSpreadCell *cell = nonDefaultCell(_marker.x() ,_marker.y()  );
    if(!cell->isForceExtraCells())
        return;

    if(makeUndo)
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( m_pDoc, this, _marker.x() ,_marker.y(),cell->extraXCells() ,cell->extraYCells());
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
    }
    int x=cell->extraXCells();
    if( x == 0 )
        x=1;
    int y = cell->extraYCells();
    if( y == 0 )
        y=1;

    cell->forceExtraCells( _marker.x() ,_marker.y(), 0, 0 );
    QRect selection( _marker.x(), _marker.y(), x, y );
    setSelection(selection);
    unselect();
    refreshMergedCell();
    emit sig_updateView( this, selection );
}

void KSpreadTable::print( QPainter &painter, QPrinter *_printer )
{
    qDebug("PRINTING ....");

    // Override the current grid pen setting
    QPen gridPen = m_pDoc->defaultGridPen();
    QPen nopen;
    nopen.setStyle( NoPen );
    m_pDoc->setDefaultGridPen( nopen );

    //
    // Find maximum right/bottom cell with content
    //
    QRect cell_range;
    cell_range.setCoords( 1, 1, 1, 1 );

    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( c->needsPrinting() )
        {
            if ( c->column() > cell_range.right() )
                cell_range.setRight( c->column() );
            if ( c->row() > cell_range.bottom() )
                cell_range.setBottom( c->row() );
        }
    }

    // Now look at the children
    QListIterator<KoDocumentChild> cit( m_pDoc->children() );
    for( ; cit.current(); ++cit )
    {
        QRect bound = cit.current()->boundingRect();
        int dummy;
        int i = leftColumn( bound.right(), dummy );
        if ( i > cell_range.right() )
            cell_range.setRight( i );
        i = topRow( bound.bottom(), dummy );
        if ( i > cell_range.bottom() )
            cell_range.setBottom( i );
    }


    //
    // Find out how many pages need printing
    // and which cells to print on which page.
    //
    QValueList<QRect> page_list;
    QValueList<QRect> page_frame_list;

    // How much space is on every page for table content ?
    QRect rect;
    rect.setCoords( 0, 0, (int)( MM_TO_POINT ( m_pDoc->printableWidth() )),
                    (int)( MM_TO_POINT ( m_pDoc->printableHeight() )) );

    // Up to this row everything is already handled
    int bottom = 0;
    // Start of the next page
    int top = 1;
    // Calculate all pages, but if we are embedded, print only the first one
    while ( bottom < cell_range.bottom() /* && page_list.count() == 0 */ )
    {
        kdDebug(36001) << "bottom=" << bottom << " bottom_range=" << cell_range.bottom() << endl;

        // Up to this column everything is already printed
        int right = 0;
        // Start of the next page
        int left = 1;
        while ( right < cell_range.right() )
        {
            kdDebug(36001) << "right=" << right << " right_range=" << cell_range.right() << endl;

            QRect page_range;
            page_range.setLeft( left );
            page_range.setTop( top );

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
            page_range.setRight( col - 1 );

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
            page_range.setBottom( row - 1 );

            right = page_range.right();
            left = page_range.right() + 1;
            bottom = page_range.bottom();

            //
            // Test wether there is anything on the page at all.
            //

            // Look at the cells
            bool empty = TRUE;
            for( int r = page_range.top(); r <= page_range.bottom(); ++r )
                for( int c = page_range.left(); c <= page_range.right(); ++c )
                    if ( cellAt( c, r )->needsPrinting() )
                        empty = FALSE;

            // Look for children
            QRect view( columnPos( page_range.left() ), rowPos( page_range.top() ),
                        rect.width(), rect.height() );

            QListIterator<KoDocumentChild> it( m_pDoc->children() );
            for( ; it.current(); ++it )
            {
                QRect bound = it.current()->boundingRect();
                if ( bound.intersects( view ) )
                    empty = FALSE;
            }

            if ( !empty )
            {
                page_list.append( page_range );
                page_frame_list.append( view );
            }
        }

        top = bottom + 1;
    }

    qDebug("PRINTING %i pages", page_list.count() );

    int pagenr = 1;

    //
    // Print all pages in the list
    //

    QValueList<QRect>::Iterator it = page_list.begin();
    QValueList<QRect>::Iterator fit = page_frame_list.begin();
    for( ; it != page_list.end(); ++it, ++fit, ++pagenr )
    {
        // print head line
        QFont font( "Times", 10 );
        painter.setFont( font );
        QFontMetrics fm = painter.fontMetrics();
        int w = fm.width( m_pDoc->headLeft( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( m_pDoc->leftBorder() )),
                              (int)( MM_TO_POINT ( 10.0 )), m_pDoc->headLeft( pagenr, m_strName.latin1() ) );
        w = fm.width( m_pDoc->headMid( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( m_pDoc->leftBorder()) +
                                     ( MM_TO_POINT ( m_pDoc->printableWidth()) - (float)w ) / 2.0 ),
                              (int)( MM_TO_POINT ( 10.0 )), m_pDoc->headMid( pagenr, m_strName.latin1() ) );
        w = fm.width( m_pDoc->headRight( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( m_pDoc->leftBorder()) +
                                     MM_TO_POINT ( m_pDoc->printableWidth()) - (float)w ),
                              (int)( MM_TO_POINT ( 10.0 )), m_pDoc->headRight( pagenr, m_strName.latin1() ) );

        // print foot line
        w = fm.width( m_pDoc->footLeft( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( m_pDoc->leftBorder() )),
                              (int)( MM_TO_POINT ( m_pDoc->paperHeight() - 10.0 )),
                              m_pDoc->footLeft( pagenr, m_strName.latin1() ) );
        w = fm.width( m_pDoc->footMid( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( m_pDoc->leftBorder() )+
                                     ( MM_TO_POINT ( m_pDoc->printableWidth()) - (float)w ) / 2.0 ),
                              (int)( MM_TO_POINT  ( m_pDoc->paperHeight() - 10.0 ) ),
                              m_pDoc->footMid( pagenr, m_strName.latin1() ) );
        w = fm.width( m_pDoc->footRight( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( m_pDoc->leftBorder()) +
                                     MM_TO_POINT ( m_pDoc->printableWidth()) - (float)w ),
                              (int)( MM_TO_POINT ( m_pDoc->paperHeight() - 10.0 ) ),
                              m_pDoc->footRight( pagenr, m_strName.latin1() ) );

        painter.translate( MM_TO_POINT ( m_pDoc->leftBorder()),
                           MM_TO_POINT ( m_pDoc->topBorder() ));
        // Print the page
        printPage( painter, *it, *fit );

        painter.translate( - MM_TO_POINT ( m_pDoc->leftBorder()),
                           - MM_TO_POINT ( m_pDoc->topBorder() ));

        if ( pagenr < page_list.count() )
            _printer->newPage();
    }

    // Restore the grid pen
    m_pDoc->setDefaultGridPen( gridPen );
}

void KSpreadTable::printPage( QPainter &_painter, const QRect& page_range, const QRect& view )
{
    // kdDebug(36001) << "Rect x=" << page_range->left() << " y=" << page_range->top() << ", w="
    // << page_range->width() << " h="  << page_range->height() << endl;

    //
    // Draw the cells.
    //
    int ypos = 0;
    for ( int y = page_range.top(); y <= page_range.bottom(); y++ )
    {
        RowLayout *row_lay = rowLayout( y );
        int xpos = 0;

        for ( int x = page_range.left(); x <= page_range.right(); x++ )
        {
            ColumnLayout *col_lay = columnLayout( x );

            KSpreadCell *cell = cellAt( x, y );
            QRect r( 0, 0, view.width(), view.height() );
            cell->paintCell( r, _painter, xpos, ypos, x, y, col_lay, row_lay );

            xpos += col_lay->width();
        }

        ypos += row_lay->height();
    }

    //
    // Draw the children
    //
    QListIterator<KoDocumentChild> it( m_pDoc->children() );
    for( ; it.current(); ++it )
    {
        qDebug("Testing child %i/%i %i/%i against view %i/%i %i/%i",
               it.current()->contentRect().left(),
               it.current()->contentRect().top(),
               it.current()->contentRect().right(),
               it.current()->contentRect().bottom(),
               view.left(), view.top(), view.right(), view.bottom() );

        QRect bound = it.current()->boundingRect();
        if ( ((KSpreadChild*)it.current())->table() == this && bound.intersects( view ) )
        {
            _painter.save();
            _painter.translate( -view.left(), -view.top() );

            it.current()->transform( _painter );
            it.current()->document()->paintEverything( _painter,
                                                       it.current()->contentRect(),
                                                       it.current()->isTransparent() );
            _painter.restore();
        }
    }
}

QDomDocument KSpreadTable::saveCellRect( const QRect &_rect )
{
    //
    // Entire rows selected ?
    //
    if ( _rect.right() == 0x7fff )
    {
        QDomDocument doc( "spreadsheet-snippet" );
        doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
        QDomElement spread = doc.createElement( "spreadsheet-snippet" );
        doc.appendChild( spread );

        QDomElement rows = doc.createElement("rows");
        rows.setAttribute( "count", _rect.bottom() - _rect.top() + 1 );
        spread.appendChild( rows );

        // Save all cells.
        KSpreadCell* c = m_cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            if ( !c->isDefault()&&!c->isObscuringForced() )
            {
                QPoint p( c->column(), c->row() );
                if ( _rect.contains( p ) )
                    spread.appendChild( c->save( doc, 0, _rect.top() - 1 ) );
            }
        }

        // ##### Inefficient
        // Save the row layouts if there are any
        for( int y = _rect.top(); y <= _rect.bottom(); ++y )
        {
            RowLayout* lay = rowLayout( y );
            if ( lay && !lay->isDefault() )
            {
                QDomElement e = lay->save( doc, _rect.top() - 1 );
                if ( !e.isNull() )
                    spread.appendChild( e );
            }
        }

        return doc;
    }

    //
    // Entire columns selected ?
    //
    if ( _rect.bottom() == 0x7fff )
    {
        QDomDocument doc( "spreadsheet-snippet" );
        doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
        QDomElement spread = doc.createElement( "spreadsheet-snippet" );
        doc.appendChild( spread );

        QDomElement columns = doc.createElement("columns");
        columns.setAttribute( "count", _rect.right() - _rect.left() + 1 );
        spread.appendChild( columns );

        // Save all cells.
        KSpreadCell* c = m_cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            if ( !c->isDefault()&&!c->isObscuringForced())
            {
                QPoint p( c->column(), c->row() );
                if ( _rect.contains( p ) )
                    spread.appendChild( c->save( doc, _rect.left() - 1, 0 ) );
            }
        }

        // ##### Inefficient
        // Save the column layouts if there are any
        for( int x = _rect.left(); x <= _rect.right(); ++x )
        {
            ColumnLayout* lay = columnLayout( x );
            if ( lay && !lay->isDefault() )
            {
                QDomElement e = lay->save( doc, _rect.left() - 1 );
                if ( !e.isNull() )
                    spread.appendChild( e );
            }
        }

        return doc;
    }

    QDomDocument doc( "spreadsheet-snippet" );
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement spread = doc.createElement( "spreadsheet-snippet" );
    doc.appendChild( spread );

    // Save all cells.
    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() && !c->isObscuringForced())
        {
            QPoint p( c->column(), c->row() );
            if ( _rect.contains( p ) )
                spread.appendChild( c->save( doc, _rect.left() - 1, _rect.top() - 1 ) );
        }
    }

    return doc;
}

QDomElement KSpreadTable::save( QDomDocument& doc )
{
    QDomElement table = doc.createElement( "table" );
    table.setAttribute( "name", m_strName );
    table.setAttribute( "grid", (int)m_bShowGrid);
    table.setAttribute( "hide", (int)m_bTableHide);
    table.setAttribute( "formular", (int)m_bShowFormular);
    table.setAttribute( "borders", (int)m_bShowPageBorders);
    table.setAttribute( "lcmode", (int)m_bLcMode);
    table.setAttribute( "columnnumber", (int)m_bShowColumnNumber);
    // Save all cells.
    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() )
        {
            QDomElement e = c->save( doc );
            if ( e.isNull() )
                return QDomElement();
            table.appendChild( e );
        }
    }

    // Save all RowLayout objects.
    RowLayout* rl = m_rows.first();
    for( ; rl; rl = rl->next() )
    {
        if ( !rl->isDefault() )
        {
            QDomElement e = rl->save( doc );
            if ( e.isNull() )
                return QDomElement();
            table.appendChild( e );
        }
    }

    // Save all ColumnLayout objects.
    ColumnLayout* cl = m_columns.first();
    for( ; cl; cl = cl->next() )
    {
        if ( !cl->isDefault() )
        {
            QDomElement e = cl->save( doc );
            if ( e.isNull() )
                return QDomElement();
            table.appendChild( e );
        }
    }

    QListIterator<KoDocumentChild> chl( m_pDoc->children() );
    for( ; chl.current(); ++chl )
    {
        if ( ((KSpreadChild*)chl.current())->table() == this )
        {
            QDomElement e = chl.current()->save( doc );
            if ( e.isNull() )
                return QDomElement();
            table.appendChild( e );
        }
    }

    return table;
}

bool KSpreadTable::isLoading()
{
    return m_pDoc->isLoading();
}

bool KSpreadTable::loadXML( const QDomElement& table )
{
    bool ok = false;
    m_strName = table.attribute( "name" );
    if ( m_strName.isEmpty() )
        return false;

    if( table.hasAttribute( "grid" ) )
    {
        m_bShowGrid = (int)table.attribute("grid").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "hide" ) )
    {
        m_bTableHide = (int)table.attribute("hide").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "formular" ) )
    {
        m_bShowFormular = (int)table.attribute("formular").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "borders" ) )
    {
        m_bShowPageBorders = (int)table.attribute("borders").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "lcmode" ) )
    {
        m_bLcMode = (int)table.attribute("lcmode").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "columnnumber" ) )
    {
        m_bShowColumnNumber = (int)table.attribute("columnnumber").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }

    // Load the cells
    QDomNode n = table.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if ( !e.isNull() && e.tagName() == "cell" )
        {
            KSpreadCell *cell = new KSpreadCell( this, 0, 0 );
            if ( cell->load( e, 0, 0 ) )
                insertCell( cell );
            else
                delete cell; // Allow error handling: just skip invalid cells
        }
        else if ( !e.isNull() && e.tagName() == "row" )
        {
            RowLayout *rl = new RowLayout( this, 0 );
            if ( rl->load( e ) )
                insertRowLayout( rl );
            else
                delete rl;
        }
        else if ( !e.isNull() && e.tagName() == "column" )
        {
            ColumnLayout *cl = new ColumnLayout( this, 0 );
            if ( cl->load( e ) )
                insertColumnLayout( cl );
            else
                delete cl;
        }
        else if ( !e.isNull() && e.tagName() == "object" )
        {
            KSpreadChild *ch = new KSpreadChild( m_pDoc, this );
            if ( ch->load( e ) )
                insertChild( ch );
            else
                delete ch;
        }
        else if ( !e.isNull() && e.tagName() == "chart" )
        {
            ChartChild *ch = new ChartChild( m_pDoc, this );
            if ( ch->load( e ) )
                insertChild( ch );
            else
                delete ch;
        }

    n = n.nextSibling();
  }

  return true;
}

void KSpreadTable::update()
{
  kdDebug(36001) << "KSpreadTable::update()" << endl;
  KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
      if ( c->isFormular() )
          c->makeFormular();
      if ( c->calcDirtyFlag() )
          c->update();
  }
}

bool KSpreadTable::loadChildren( KoStore* _store )
{
    QListIterator<KoDocumentChild> it( m_pDoc->children() );
    for( ; it.current(); ++it )
    {
        if ( ((KSpreadChild*)it.current())->table() == this )
        {
            if ( !it.current()->loadDocument( _store ) )
                return false;
        }
    }

    return true;
}

void KSpreadTable::setShowPageBorders( bool b )
{
    if ( b == m_bShowPageBorders )
        return;

    m_bShowPageBorders = b;
    emit sig_updateView( this );
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

KSpreadTable* KSpreadTable::findTable( const QString & _name )
{
  if ( !m_pMap )
    return 0L;

  return m_pMap->findTable( _name );
}

// ###### Torben: Use this one instead of m_cells.insert()
void KSpreadTable::insertCell( KSpreadCell *_cell )
{
    m_cells.insert( _cell, _cell->column(), _cell->row() );

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

void KSpreadTable::insertColumnLayout( ColumnLayout *l )
{
  m_columns.insertElement( l, l->column() );
}

void KSpreadTable::insertRowLayout( RowLayout *l )
{
  m_rows.insertElement( l, l->row() );
}

void KSpreadTable::updateCell( KSpreadCell *cell, int _column, int _row )
{
    if ( doc()->isLoading() )
        return;

    // Get the size
    int left = columnPos( _column );
    int top = rowPos( _row );
    int right = left + cell->extraWidth();
    int bottom = top + cell->extraHeight();

    // Need to calculate ?
    if ( cell->calcDirtyFlag() )
        cell->calc();

    // Need to make layout ?
    if ( cell->layoutDirtyFlag() )
        cell->makeLayout( painter(), _column, _row );

    // Perhaps the size changed now ?
    right = QMAX( right, left + cell->extraWidth() );
    bottom = QMAX( bottom, top + cell->extraHeight() );

    // Force redraw
    QPointArray arr( 4 );
    arr.setPoint( 0, left, top );
    arr.setPoint( 1, right, top );
    arr.setPoint( 2, right, bottom );
    arr.setPoint( 3, left, bottom );

    // ##### Hmmmm, why not draw the cell directly ?
    // That will be faster.
    emit sig_polygonInvalidated( arr );

    cell->clearDisplayDirtyFlag();
}

void KSpreadTable::emit_polygonInvalidated( const QPointArray& arr )
{
    emit sig_polygonInvalidated( arr );
}

void KSpreadTable::emit_updateRow( RowLayout *_layout, int _row )
{
    if ( doc()->isLoading() )
        return;

    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
      if ( c->row() == _row )
          c->setLayoutDirtyFlag();

    emit sig_updateVBorder( this );
    emit sig_updateView( this );
    _layout->clearDisplayDirtyFlag();
}

void KSpreadTable::emit_updateColumn( ColumnLayout *_layout, int _column )
{
    if ( doc()->isLoading() )
        return;

    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
        if ( c->column() == _column )
            c->setLayoutDirtyFlag();

    emit sig_updateHBorder( this );
    emit sig_updateView( this );
    _layout->clearDisplayDirtyFlag();
}

void KSpreadTable::insertChart( const QRect& _rect, KoDocumentEntry& _e, const QRect& _data )
{
    kdDebug(36001) << "Creating document" << endl;
    KoDocument* doc = _e.createDoc();
    kdDebug(36001) << "Created" << endl;
    if ( !doc )
        // Error message is already displayed, so just return
        return;

    kdDebug(36001) << "NOW FETCHING INTERFACE" << endl;

    if ( !doc->initDoc() )
        return;

    ChartChild* ch = new ChartChild( m_pDoc, this, doc, _rect );
    ch->setDataArea( _data );
    ch->update();

    // m_pDoc->insertChild( ch );
    insertChild( ch );

    ch->chart()->showWizard();
}

void KSpreadTable::insertChild( const QRect& _rect, KoDocumentEntry& _e )
{
    KoDocument* doc = _e.createDoc( m_pDoc );
    doc->initDoc();

    KSpreadChild* ch = new KSpreadChild( m_pDoc, this, doc, _rect );

    insertChild( ch );
}

void KSpreadTable::insertChild( KSpreadChild *_child )
{
    // m_lstChildren.append( _child );
    m_pDoc->insertChild( _child );

    emit sig_polygonInvalidated( _child->framePointArray() );
}

void KSpreadTable::changeChildGeometry( KSpreadChild *_child, const QRect& _rect )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );
}

/*
QListIterator<KSpreadChild> KSpreadTable::childIterator()
{
  return QListIterator<KSpreadChild> ( m_lstChildren );
}
*/

bool KSpreadTable::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;

    QListIterator<KoDocumentChild> it( m_pDoc->children() );
    for( ; it.current(); ++it )
    {
        if ( ((KSpreadChild*)it.current())->table() == this )
        {
            QString path = QString( "%1/%2" ).arg( _path ).arg( i++ );
            if ( !it.current()->document()->saveToStore( _store, path ) )
                return false;
        }
    }
    return true;
}

KSpreadTable::~KSpreadTable()
{
    s_mapTables->remove( m_id );

    KSpreadCell* c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->tableDies();

    m_cells.clear(); // cells destructor needs table to still exist

    m_pPainter->end();
    delete m_pPainter;
    delete m_pWidget;

    delete m_defaultLayout;
}

void KSpreadTable::enableScrollBarUpdates( bool _enable )
{
  m_bScrollbarUpdates = _enable;
}

DCOPObject* KSpreadTable::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadTableIface( this );

    return m_dcop;
}

bool KSpreadTable::setTableName( const QString& name, bool init, bool makeUndo )
{
    if ( map()->findTable( name ) )
        return FALSE;

    if ( m_strName == name )
        return TRUE;

    QString old_name = m_strName;
    m_strName = name;

    if ( init )
        return TRUE;

    QListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeCellTabName( old_name, name );
    if(makeUndo)
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoAction* undo = new KSpreadUndoSetTableName( doc(), this, old_name );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
    }

    m_pDoc->changeAreaTableName(old_name,name);
    emit sig_nameChanged( this, old_name );

    return TRUE;
}

/**********************************************************
 *
 * KSpreadChild
 *
 **********************************************************/

KSpreadChild::KSpreadChild( KSpreadDoc *parent, KSpreadTable *_table, KoDocument* doc, const QRect& geometry )
  : KoDocumentChild( parent, doc, geometry )
{
  m_pTable = _table;
}

KSpreadChild::KSpreadChild( KSpreadDoc *parent, KSpreadTable *_table ) : KoDocumentChild( parent )
{
  m_pTable = _table;
}


KSpreadChild::~KSpreadChild()
{
}

/**********************************************************
 *
 * ChartChild
 *
 **********************************************************/

ChartChild::ChartChild( KSpreadDoc *_spread, KSpreadTable *_table, KoDocument* doc, const QRect& geometry )
  : KSpreadChild( _spread, _table, doc, geometry )
{
    m_pBinding = 0;
}

ChartChild::ChartChild( KSpreadDoc *_spread, KSpreadTable *_table )
  : KSpreadChild( _spread, _table )
{
    m_pBinding = 0;
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

void ChartChild::update()
{
    if ( m_pBinding )
        m_pBinding->cellChanged( 0 );
}

bool ChartChild::load( const QDomElement& element )
{
    if ( !KSpreadChild::load( element ) )
        return false;

    if ( element.hasAttribute( "left-cell" ) &&
         element.hasAttribute( "top-cell" ) &&
         element.hasAttribute( "right-cell" ) &&
         element.hasAttribute( "bottom-cell" ) )
    {
        QRect r;
        r.setCoords( element.attribute( "left-cell" ).toInt(),
                     element.attribute( "top-cell" ).toInt(),
                     element.attribute( "right-cell" ).toInt(),
                     element.attribute( "bottom-cell" ).toInt() );

        setDataArea( r );
    }

    return true;
}

QDomElement ChartChild::save( QDomDocument& doc )
{
    QDomElement element = KSpreadChild::save( doc );
    element.setTagName( "chart" );

    element.setAttribute( "left-cell", m_pBinding->dataArea().left() );
    element.setAttribute( "right-cell", m_pBinding->dataArea().right() );
    element.setAttribute( "top-cell", m_pBinding->dataArea().top() );
    element.setAttribute( "bottom-cell", m_pBinding->dataArea().bottom() );

    return element;
}

bool ChartChild::loadDocument( KoStore* _store )
{
    bool res = KSpreadChild::loadDocument( _store );
    if ( !res )
        return res;

    // Did we see a cell rectangle ?
    if ( !m_pBinding )
        return true;

    update();

    return true;
}

KChartPart* ChartChild::chart()
{
    return (KChartPart*)document();
}

#include "kspread_table.moc"
