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
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <pwd.h>
#include <unistd.h>

#include <qapplication.h>
#include <qclipboard.h>
#include <qpicture.h>
#include <qdragobject.h>
#include <qregexp.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <kmessagebox.h>

#include <koReplace.h>
#include <kprinter.h>
#include <koDocumentInfo.h>

#include "kspread_global.h"
#include "kspread_undo.h"
#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_canvas.h"

#include "KSpreadTableIface.h"

#include <kdebug.h>
#include <assert.h>

#include <koChart.h>

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

    KoChart::Data matrix( m_rctDataArea.height(), m_rctDataArea.width() );

    KSpreadCell* cell;
    for ( int y = 0; y < m_rctDataArea.height(); y++ )
        for ( int x = 0; x < m_rctDataArea.width(); x++ )
        {
            cell = m_pTable->cellAt( m_rctDataArea.left() + x, m_rctDataArea.top() + y );
            if ( cell && cell->isNumeric() )
                matrix.cell( y, x ) = KoChart::Value( cell->valueDouble() );
            else if ( cell )
                matrix.cell( y, x ) = KoChart::Value( cell->valueString() );
            else
                matrix.cell( y, x ) = KoChart::Value();
        }

    // ######### Kalle may be interested in that, too
    /* Chart::Range range;
       range.top = m_rctDataArea.top();
       range.left = m_rctDataArea.left();
       range.right = m_rctDataArea.right();
       range.bottom = m_rctDataArea.bottom();
       range.table = m_pTable->name(); */

    m_child->chart()->setData( matrix );

    // Force a redraw of the chart on all views
    table()->emit_polygonInvalidated( m_child->framePointArray() );
}

/******************************************************************/
/* Class: KSpreadTextDrag                                               */
/******************************************************************/

KSpreadTextDrag::KSpreadTextDrag( QWidget * dragSource, const char * name )
    : QTextDrag( dragSource, name )
{
}

KSpreadTextDrag::~KSpreadTextDrag()
{
}


QByteArray KSpreadTextDrag::encodedData( const char * mime ) const
{
  if ( strcmp( selectionMimeType(), mime ) == 0)
    return m_kspread;
  else
    return QTextDrag::encodedData( mime );
}

bool KSpreadTextDrag::canDecode( QMimeSource* e )
{
  if ( e->provides( selectionMimeType() ) )
    return true;
  return QTextDrag::canDecode(e);
}

const char * KSpreadTextDrag::format( int i ) const
{
  if ( i < 4 ) // HACK, but how to do otherwise ??
    return QTextDrag::format(i);
  else if ( i == 4 )
    return selectionMimeType();
  else return 0;
}

const char * KSpreadTextDrag::selectionMimeType()
{
  return "application/x-kspread-snippet";
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

  m_marker = QPoint(1,1);
  m_cursorPosition = QPoint(1,1);

  m_pMap = _map;
  m_pDoc = _map->doc();
  m_dcop = 0;
  dcopObject();
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

  m_rctSelection.setCoords( 1, 1, 1, 1 );

  m_pWidget = new QWidget();
  m_pPainter = new QPainter;
  m_pPainter->begin( m_pWidget );

  m_iMaxColumn = 256;
  m_iMaxRow = 256;
  m_ulSizeMaxX = KS_colMax * m_pDefaultColumnLayout->width(); // default is max cols * default width
  m_ulSizeMaxY = KS_rowMax * m_pDefaultRowLayout->height(); // default is max rows * default height

  m_bScrollbarUpdates = true;

  setHidden( false );
  m_bShowGrid=true;
  m_bPrintGrid=false;
  m_bShowFormula=false;
  m_bShowFormulaIndicator=true;
  m_bLcMode=false;
  m_bShowColumnNumber=false;
  m_bHideZero=false;
  m_bFirstLetterUpper=false;
  m_bAutoCalc=true;
  // Get a unique name so that we can offer scripting
  if ( !_name )
  {
      QCString s;
      s.sprintf("Sheet%i", s_id );
      QObject::setName( s.data() );
  }
  m_iScrollPosX=0;
  m_iScrollPosY=0;

  //Init the printing options
  m_leftBorder = 20.0;
  m_rightBorder = 20.0;
  m_topBorder = 20.0;
  m_bottomBorder = 20.0;
  m_paperFormat = PG_DIN_A4;
  m_paperWidth = PG_A4_WIDTH;
  m_paperHeight = PG_A4_HEIGHT;
  m_orientation = PG_PORTRAIT;
  m_printRange = QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) );
  m_dPrintRepeatColumnsWidth = 0.0;
  m_dPrintRepeatRowsHeight = 0.0;
  m_printRepeatColumns = qMakePair( 0, 0 );
  m_printRepeatRows = qMakePair( 0, 0 );
  calcPaperSize();

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
        if ( col > KS_colMax - 1 )
	{
	    kdDebug(36001) << "KSpreadTable:leftColumn: invalid column (col: " << col + 1 << ")" << endl;
            return 1;
	}
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
        if ( col > KS_colMax )
	{
	    kdDebug(36001) << "KSpreadTable:rightColumn: invalid column (col: " << col << ")" << endl;
            return KS_colMax + 1;
	}
        x += columnLayout( col )->width( _canvas );
        col++;
    }

    return col - 1;
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
        if ( row > KS_rowMax - 1 )
        {
            kdDebug(36001) << "KSpreadTable:topRow: invalid row (row: " << row + 1 << ")" << endl;
            return 1;
        }
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
        if ( row > KS_rowMax )
	{
	    kdDebug(36001) << "KSpreadTable:bottomRow: invalid row (row: " << row << ")" << endl;
            return KS_rowMax + 1;
	}
        y += rowLayout( row )->height( _canvas );
        row++;
    }

    return row - 1;
}

int KSpreadTable::columnPos( int _col, KSpreadCanvas *_canvas )
{
    int x = 0;
    if ( _canvas )
      x -= _canvas->xOffset();
    for ( int col = 1; col < _col; col++ )
    {
        // Should never happen
        if ( col > KS_colMax )
	{
	    kdDebug(36001) << "KSpreadTable:columnPos: invalid column (col: " << col << ")" << endl;
            return x;
	}

        x += columnLayout( col )->width( _canvas );
    }

    return x;
}

int KSpreadTable::rowPos( int _row, KSpreadCanvas *_canvas )
{
    int y = 0;
    if ( _canvas )
      y -= _canvas->yOffset();

/*    _row--; //we need the height until the questioned row, so we need the heights of the previous ones
    int row = 0;
    RowLayout *p;

kdDebug(36001) << "row1: " << row << "  y: " << y << endl;

    while ( ( p = m_rows.getNextRowLayoutDown( row ) ) && ( p->row() < _row ) )
    {
      y += p->height( _canvas );
      y += ( p->row() - row - 1 ) * 20;
kdDebug(36001) << "row (n)): " << row << "   p->row()" << p->row() << "   y: " << y << endl;
      row = p->row();
    }
kdDebug(36001) << "row2: " << row << "  y: " << y << endl;

//    if ( p != NULL )
    {
kdDebug(36001) << "arow2: " << row << "  y: " << y << endl;
//      y += p->height( _canvas );
kdDebug(36001) << "_row2: " << _row << "  y: " << y << endl;
      y += ( _row - row ) * 20;
    }
kdDebug(36001) << "row3: " << row << "  y: " << y << endl;
kdDebug(36001) << endl;
*/

    for ( int row = 1 ; row < _row ; row++ )
    {
        // Should never happen
        if ( row > KS_rowMax )
	{
	    kdDebug(36001) << "KSpreadTable:rowPos: invalid row (row: " << row << ")" << endl;
            return y;
	}

        y += rowLayout( row )->height( _canvas );
    }

    return y;
}

void KSpreadTable::adjustSizeMaxX ( int _x )
{
    m_ulSizeMaxX += _x;
}

void KSpreadTable::adjustSizeMaxY ( int _y )
{
    m_ulSizeMaxY += _y;
}

KSpreadCell* KSpreadTable::visibleCellAt( int _column, int _row, bool _scrollbar_update )
{
  KSpreadCell* cell = cellAt( _column, _row, _scrollbar_update );
  KSpreadCell* obscuring = cell->obscuringCells().getLast();

  return obscuring == NULL ? cell : obscuring;
}

KSpreadCell* KSpreadTable::firstCell()
{
    return m_cells.firstCell();
}

RowLayout* KSpreadTable::firstRow()
{
    return m_rows.first();
}

ColumnLayout* KSpreadTable::firstCol()
{
    return m_columns.first();
}

const KSpreadCell* KSpreadTable::cellAt( int _column, int _row ) const
{
    const KSpreadCell *p = m_cells.lookup( _column, _row );
    if ( p != 0L )
        return p;

    return m_pDefaultCell;
}

KSpreadCell* KSpreadTable::cellAt( int _column, int _row, bool _scrollbar_update )
{
  if ( _column > KS_colMax ) {
    _column = KS_colMax;
    kdDebug (36001) << "KSpreadTable::cellAt: column range: (col: " << _column << ")" << endl;
  }
  if ( _row > KS_rowMax) {
    kdDebug (36001) << "KSpreadTable::cellAt: row out of range: (row: " << _row << ")" << endl;
    _row = KS_rowMax;
  }

  if ( _scrollbar_update && m_bScrollbarUpdates )
  {
    checkRangeHBorder( _column );
    checkRangeVBorder( _row );
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
    // TODO: copy the default ColumnLayout here!!
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
                                           bool _scrollbar_update )
{
  if ( _scrollbar_update && m_bScrollbarUpdates )
  {
    checkRangeHBorder( _column );
    checkRangeVBorder( _row );
  }

  KSpreadCell *p = m_cells.lookup( _column, _row );
  if ( p != 0L )
    return p;

  KSpreadCell *cell = new KSpreadCell( this, _column, _row );
  insertCell( cell );

  return cell;
}

void KSpreadTable::setText( int _row, int _column, const QString& _text, bool updateDepends )
{
    KSpreadCell *cell = nonDefaultCell( _column, _row );

    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        KSpreadUndoSetText *undo = new KSpreadUndoSetText( m_pDoc, this, cell->text(), _column, _row,cell->formatType() );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    // The cell will force a display refresh itself, so we dont have to care here.
    cell->setCellText( _text, updateDepends );
    //refresh anchor
    if(_text.at(0)=='!')
      emit sig_updateView( this, QRect(_column,_row,_column,_row) );
}

void KSpreadTable::setLayoutDirtyFlag()
{
    KSpreadCell * c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->setLayoutDirtyFlag();
}

void KSpreadTable::setCalcDirtyFlag()
{
    KSpreadCell* c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
    {
        if ( !(c->isObscured() && c->isObscuringForced()) )
            c->setCalcDirtyFlag();
    }
}

void KSpreadTable::recalc()
{
   // First set all cells as dirty
   setCalcDirtyFlag();

   this->calc();
}

void KSpreadTable::calc()
{
  KSpreadCell* c = m_cells.firstCell();
  for( ; c; c = c->nextCell() )
  {
     c->calc();
  }
}

void KSpreadTable::unselect()
{
    // No selection? Then do nothing.
    if ( singleCellSelection() )
        return;

    QRect r = m_rctSelection;
    // Discard the selection
    KSpreadCell *cell = cellAt(marker());
    m_rctSelection.setCoords( m_marker.x(), m_marker.y(),
                              m_marker.x() + cell->extraXCells(),
                              m_marker.y() + cell->extraYCells() );

    // Emit signal so that the views can update.
    emit sig_unselect( this, r );
}

void KSpreadTable::setMarker( QPoint _point, KSpreadCanvas *_canvas )
{
  QPoint topLeft(_point);
  KSpreadCell* cell = cellAt(topLeft);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().getFirst();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint botRight(topLeft.x() + cell->extraXCells(),
                  topLeft.y() + cell->extraYCells());
  setSelection( QRect(topLeft, botRight), topLeft, _canvas );
}


QPoint KSpreadTable::marker() const
{
  return m_marker;
}

QRect KSpreadTable::selection() const
{
  return m_rctSelection;
}

QPoint KSpreadTable::getCursorPosition()const
{
  return m_cursorPosition;
}

bool KSpreadTable::setCursorPosition(const QPoint &position)
{
  KSpreadCell* cell = cellAt(m_marker);

  QRect markerArea(m_marker, QPoint(m_marker.x() + cell->extraXCells(),
                                    m_marker.y() + cell->extraYCells()));

  if (markerArea.contains(position))
  {
    m_cursorPosition = position;
    return true;
  }
  return false;
}

QPoint KSpreadTable::selectionAnchor()
{
  /* the anchor is in the opposite corner of the selection rect from the marker */

  /* these show where the marker is */
  bool atTop;
  bool atLeft;

  atLeft = m_marker.x() == m_rctSelection.left();
  atTop = m_marker.y() == m_rctSelection.top();

  QPoint anchor(atLeft ? m_rctSelection.right() : m_rctSelection.left(),
                atTop ? m_rctSelection.bottom() : m_rctSelection.top());
  return anchor;
}


bool KSpreadTable::singleCellSelection() const
{
  const KSpreadCell* cell = cellAt(m_marker.x(), m_marker.y());

  return ((m_rctSelection.topLeft() == m_marker) &&
          (m_rctSelection.width() - 1 == cell->extraXCells()) &&
          (m_rctSelection.height() - 1 == cell->extraYCells()));
}

void KSpreadTable::setSelection( const QRect &_sel, KSpreadCanvas *canvas )
{
  Q_ASSERT(_sel.left() != 0);

  if (_sel.contains(m_marker))
  {
    setSelection( _sel, m_marker, canvas);
  }
  else
  {
    setSelection( _sel, _sel.topLeft(), canvas );
  }
}

void KSpreadTable::setSelection(QRect  newSelection, QPoint newMarker,
                                 KSpreadCanvas */*_canvas*/ )
{

  KSpreadCell* cell = cellAt(newMarker);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().getFirst();
    newMarker = QPoint(cell->column(), cell->row());
  }

  newSelection = selectionCellMerged(newSelection);

  Q_ASSERT(newSelection.contains(newMarker));

  /* see if we've actually changed anything */
  if ( newSelection == m_rctSelection && newMarker == m_marker )
    return;

  QPoint oldMarker = m_marker;
  QRect oldSelection( m_rctSelection );
  m_rctSelection = newSelection;
  m_marker = newMarker;

  /* see if the cursor position is still valid */
  if (!setCursorPosition(m_cursorPosition))
  {
    setCursorPosition(newMarker);
  }

  emit sig_changeSelection( this, oldSelection, oldMarker );
}

QRect KSpreadTable::getChooseRect()const
{
  QRect chooseRect;

  chooseRect.setLeft(QMIN(m_chooseMarker.x(), m_chooseAnchor.x()));
  chooseRect.setRight(QMAX(m_chooseMarker.x(), m_chooseAnchor.x()));
  chooseRect.setTop(QMIN(m_chooseMarker.y(), m_chooseAnchor.y()));
  chooseRect.setBottom(QMAX(m_chooseMarker.y(), m_chooseAnchor.y()));

  return chooseRect;
}

/*
 Methods working on selections:

 TYPE A:
 { columns selected:
   for all rows with properties X,X':
     if default-cell create new cell
 }
 post undo object (always a KSpreadUndoCellLayout; difference in title only)
 { rows selected:
   if condition Y clear properties X,X' of cells;
   set properties X,X' of rowlayouts
   emit complete update;
 }
 { columns selected:
   if condition Y clear properties X,X' of cells;
   set properties X,X' of columnlayouts;
   for all rows with properties X,X':
     create cells if necessary and set properties X,X'
   emit complete update;
 }
 { cells selected:
   for all cells with condition Y:
     create if necessary and set properties X,X' and do Z;
   emit update on selected region;
 }

 USED in:
 setSelectionFont
 setSelectionSize
 setSelectionAngle
 setSelectionTextColor
 setSelectionBgColor
 setSelectionPercent
 borderAll
 borderRemove (exceptions: ### creates cells (why?), ### changes default cell if cell-regions selected?)
 setSelectionAlign
 setSelectionAlignY
 setSelectionMoneyFormat
 increaseIndent
 decreaseIndent

 TYPE B:
 post undo object
 { rows selected:
   if condition Y do X with cells;
   emit update on selection;
 }
 { columns selected:
   if condition Y do X with cells;
   emit update on selection;
 }
 { cells selected:
   if condition Y do X with cells; create cell if non-default;
   emit update on selection;
 }

 USED in:
 setSelectionUpperLower (exceptions: no undo; no create-if-default; ### modifies default-cell?)
 setSelectionFirstLetterUpper (exceptions: no undo; no create-if-default; ### modifies default-cell?)
 setSelectionVerticalText
 setSelectionComment
 setSelectionRemoveComment (exeception: no create-if-default and work only on non-default-cells for cell regions)
 setSelectionBorderColor (exeception: no create-if-default and work only on non-default-cells for cell regions)
 setSelectionMultiRow
 setSelectionPrecision
 clearTextSelection (exception: all only if !areaIsEmpty())
 clearValiditySelection (exception: all only if !areaIsEmpty())
 clearConditionalSelection (exception: all only if !areaIsEmpty())
 setConditional (exception: conditional after create-if-default for cell regions)
 setValidity (exception: conditional after create-if-default for cell regions)

 OTHERS:
 borderBottom
 borderRight
 borderLeft
 borderTop
 borderOutline
 => these work only on some cells (at the border); undo only if cells affected; rest is similar to type A
 --> better not use CellWorker/workOnCells()

 defaultSelection
 => similar to TYPE B, but works on columns/rows if complete columns/rows selected
 --> use emit_signal=false and return value of workOnCells to finish

 getWordSpelling
 => returns text, no signal emitted, no cell-create, similar to TYPE B
 --> use emit_signal=false, create_if_default=false and type B

 setWordSpelling
 => no signal emitted, no cell-create, similar to type B
 --> use emit_signal=false, create_if_default=false and type B
 */

class KSpreadUndoAction* KSpreadTable::CellWorkerTypeA::createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r )
{
    QString title = getUndoTitle();
    return new KSpreadUndoCellLayout( doc, table, r, title );
}

/*
KSpreadTable::SelectionType KSpreadTable::workOnCells( const QPoint& _marker, CellWorker& worker )
{
    // see what is selected; if nothing, take marker position
    bool selected = ( m_rctSelection.left() != 0 );
    QRect r( m_rctSelection );
    if ( !selected )
	r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

    // create cells in rows if complete columns selected
    KSpreadCell *cell;
    if ( !worker.type_B && selected && isColumnSelected() )
    {
	for ( RowLayout* rw =m_rows.first(); rw; rw = rw->next() )
	{
	    if ( !rw->isDefault() && worker.testCondition( rw ) )
	    {
		for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ )
		{
		    cell = cellAt( i, rw->row() );
		    if ( cell == m_pDefaultCell )
			// '&& worker.create_if_default' unneccessary as never used in type A
		    {
			cell = new KSpreadCell( this, i, rw->row() );
			insertCell( cell );
		    }
		}
	    }
	}
    }

    // create an undo action
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
	KSpreadUndoAction *undo = worker.createUndoAction( m_pDoc, this, r );
        // test if the worker has an undo action
        if ( undo != 0L )
	    m_pDoc->undoBuffer()->appendUndo( undo );
    }

    // complete rows selected ?
    if ( selected && isRowSelected() )
    {
	int row;
	for ( KSpreadCell* cell = m_cells.firstCell(); cell; cell = cell->nextCell() )
	{
	    row = cell->row();
	    if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
		 && worker.testCondition( cell ) )
		if ( worker.type_B )
		    worker.doWork( cell, false, cell->column(), row );
		else
		    worker.prepareCell( cell );
	}

	if ( worker.type_B ) {
            // for type B there's nothing left to do
	    if ( worker.emit_signal )
		emit sig_updateView( this, r );
	} else {
            // for type A now work on row layouts
	    for ( int i=m_rctSelection.top(); i<=m_rctSelection.bottom(); i++ )
	    {
		RowLayout *rw=nonDefaultRowLayout(i);
		worker.doWork( rw );
	    }
	    if ( worker.emit_signal )
		emit sig_updateView( this );
	}
	return CompleteRows;
    }
    // complete columns selected ?
    else if ( selected && isColumnSelected() )
    {
	int col;
	for ( KSpreadCell* cell = m_cells.firstCell(); cell; cell = cell->nextCell() )
	{
	    col = cell->column();
	    if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
		 && worker.testCondition( cell ) )
		if ( worker.type_B )
		    worker.doWork( cell, false, col, cell->row() );
		else
		    worker.prepareCell( cell );
	}

	if ( worker.type_B ) {
	    if ( worker.emit_signal )
		emit sig_updateView( this, r );
	} else {
	    // for type A now work on column layouts
	    for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ )
	    {
		ColumnLayout *cl=nonDefaultColumnLayout(i);
		worker.doWork( cl );
	    }
	    KSpreadCell *cell;
	    for ( RowLayout* rw =m_rows.first(); rw; rw = rw->next() )
	    {
		if ( !rw->isDefault() && worker.testCondition( rw ) )
		{
		    for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ )
		    {
			cell = cellAt( i, rw->row() );
			// ### this if should be not necessary; cells are created
			//     before the undo object is created, aren't they?
			if ( cell == m_pDefaultCell )
			{
			    cell = new KSpreadCell( this, i, rw->row() );
			    insertCell( cell );
			}
			worker.doWork( cell, false, i, rw->row() );
		    }
		}
	    }
            if ( worker.emit_signal )
		emit sig_updateView( this );
	}
	return CompleteColumns;
    }
    // cell region selected
    else
    {
	KSpreadCell *cell;
	for ( int x = r.left(); x <= r.right(); x++ )
	    for ( int y = r.top(); y <= r.bottom(); y++ )
	    {
		cell = cellAt( x, y );
                if ( worker.testCondition( cell ) )
		{
		    if ( worker.create_if_default && cell == m_pDefaultCell )
		    {
			cell = new KSpreadCell( this, x, y );
			insertCell( cell );
		    }
                    if ( cell != m_pDefaultCell )
			worker.doWork( cell, true, x, y );
		}
	    }
        if ( worker.emit_signal )
	    emit sig_updateView( this, r );
        return CellRegion;
    }
}

*/

KSpreadTable::SelectionType KSpreadTable::workOnCells( const QPoint & /*_marker */, CellWorker & worker )
{
  // see what is selected; if nothing, take marker position
  QRect r( m_rctSelection );
  bool selected = !singleCellSelection();

  int bottom = r.bottom();
  int right  = r.right();

  KSpreadTable::SelectionType result;

  m_pDoc->emitBeginOperation();
//  m_pDoc->setCalculationDelay();

  // create cells in rows if complete columns selected
  KSpreadCell * cell;
  if ( !worker.type_B && selected && isColumnSelected() )
  {
    for ( RowLayout * rw = m_rows.first(); rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && worker.testCondition( rw ) )
      {
        for ( int col = m_rctSelection.left(); col <= right; ++col )
        {
          cell = nonDefaultCell( col, rw->row() );
        }
      }
    }
  }

  // create an undo action
  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoAction * undo = worker.createUndoAction( m_pDoc, this, r );
    // test if the worker has an undo action
    if ( undo != 0L )
      m_pDoc->undoBuffer()->appendUndo( undo );
  }

  // complete rows selected ?
  if ( selected && isRowSelected() )
  {
    for ( int row = m_rctSelection.top(); row <= bottom; ++row )
    {
      cell = getFirstCellRow( row );
      while ( cell )
      {
        if ( worker.testCondition( cell ) )
        {
          if ( worker.type_B )
            worker.doWork( cell, false, cell->column(), row );
          else
            worker.prepareCell( cell );
        }
        cell = getNextCellRight( cell->column(), row );
      }
    }

    if ( worker.type_B )
    {
      // for type B there's nothing left to do
      ;
    }
    else
    {
      // for type A now work on row layouts
      for ( int i = m_rctSelection.top(); i <= bottom; ++i )
      {
        RowLayout * rw = nonDefaultRowLayout(i);
        worker.doWork( rw );
      }
    }
    result = CompleteRows;
  }
  // complete columns selected ?
  else if ( selected && isColumnSelected() )
  {
    for ( int col = m_rctSelection.left(); col <= right; ++col )
    {
      cell = getFirstCellColumn( col );
      while ( cell )
      {
	if ( worker.testCondition( cell ) )
        {
          if ( worker.type_B )
            worker.doWork( cell, false, col, cell->row() );
          else
            worker.prepareCell( cell );
        }

        cell = getNextCellDown( col, cell->row() );
      }
    }

    if ( worker.type_B )
    {
      ;
    }
    else
    {
      // for type A now work on column layouts
      for ( int i = m_rctSelection.left(); i <= right; ++i )
      {
        ColumnLayout * cl = nonDefaultColumnLayout( i );
        worker.doWork( cl );
      }

      for ( RowLayout * rw = m_rows.first(); rw; rw = rw->next() )
      {
        if ( !rw->isDefault() && worker.testCondition( rw ) )
        {
          for ( int i = m_rctSelection.left(); i <= right; ++i )
          {
            cell = cellAt( i, rw->row() );
            // ### this if should be not necessary; cells are created
            //     before the undo object is created, aren't they?
            if ( cell == m_pDefaultCell )
            {
              cell = new KSpreadCell( this, i, rw->row() );
              insertCell( cell );
            }
            worker.doWork( cell, false, i, rw->row() );
          }
        }
      }
    }
    result = CompleteColumns;
  }
  // cell region selected
  else
  {
    for ( int x = r.left(); x <= right; ++x )
    {
      for ( int y = r.top(); y <= bottom; ++y )
      {
        cell = cellAt( x, y );
        if ( worker.testCondition( cell ) )
        {
          if ( cell == m_pDefaultCell && worker.create_if_default )
          {
            cell = new KSpreadCell( this, x, y );
            insertCell( cell );
          }
          if ( cell != m_pDefaultCell )
            worker.doWork( cell, true, x, y );
        }
      }
    }
    result = CellRegion;
  }

  m_pDoc->emitEndOperation();
//  m_pDoc->clearCalculationDelay();
//  updateCellArea(r);

  if (worker.emit_signal)
  {
    emit sig_updateView( this, r );
  }

  return result;
}

struct SetSelectionFontWorker : public KSpreadTable::CellWorkerTypeA
{
    const char *_font;
    int _size;
    signed char _bold;
    signed char _italic;
    signed char _underline;
    signed char _strike;
    SetSelectionFontWorker( const char *font, int size, signed char bold, signed char italic,signed char underline, signed char strike )
	: _font( font ), _size( size ), _bold( bold ), _italic( italic ), _underline( underline ), _strike( strike ) { }

    QString getUndoTitle() { return i18n("Change Font"); }
    bool testCondition( RowLayout* rw ) {
        return ( rw->hasProperty( KSpreadCell::PFont ) );
    }
    void doWork( RowLayout* rw ) {
	if ( _font )
	    rw->setTextFontFamily( _font );
	if ( _size > 0 )
	    rw->setTextFontSize( _size );
	if ( _italic >= 0 )
	    rw->setTextFontItalic( (bool)_italic );
	if ( _bold >= 0 )
	    rw->setTextFontBold( (bool)_bold );
	if ( _underline >= 0 )
	    rw->setTextFontUnderline( (bool)_underline );
	if ( _strike >= 0 )
	    rw->setTextFontStrike( (bool)_strike );
    }
    void doWork( ColumnLayout* cl ) {
	if ( _font )
	    cl->setTextFontFamily( _font );
	if ( _size > 0 )
	    cl->setTextFontSize( _size );
	if ( _italic >= 0 )
	    cl->setTextFontItalic( (bool)_italic );
	if ( _bold >= 0 )
	    cl->setTextFontBold( (bool)_bold );
	if ( _underline >= 0 )
	    cl->setTextFontUnderline( (bool)_underline );
	if ( _strike >= 0 )
	    cl->setTextFontStrike( (bool)_strike );
    }
    void prepareCell( KSpreadCell* cell ) {
	cell->clearProperty( KSpreadCell::PFont );
	cell->clearNoFallBackProperties( KSpreadCell::PFont );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
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
        if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionFont( const QPoint &_marker, const char *_font, int _size,
                                     signed char _bold, signed char _italic,signed char _underline,
                                     signed char _strike )
{
    SetSelectionFontWorker w( _font, _size, _bold, _italic, _underline, _strike );
    workOnCells( _marker, w );
}

struct SetSelectionSizeWorker : public KSpreadTable::CellWorkerTypeA {
    int _size, size;
    SetSelectionSizeWorker( int __size, int size2 ) : _size( __size ), size( size2 ) { }

    QString getUndoTitle() { return i18n("Change Font"); }
    bool testCondition( RowLayout* rw ) {
        return ( rw->hasProperty( KSpreadCell::PFont ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setTextFontSize( size + _size) ;
    }
    void doWork( ColumnLayout* cl ) {
	cl->setTextFontSize( size + _size );
    }
    void prepareCell( KSpreadCell* cell ) {
	cell->clearProperty( KSpreadCell::PFont );
	cell->clearNoFallBackProperties( KSpreadCell::PFont );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setTextFontSize( size + _size );
        if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionSize( const QPoint &_marker, int _size )
{
    int size;
    KSpreadCell* c;
    c=cellAt(_marker.x(), _marker.y());
    size=c->textFontSize(_marker.x(), _marker.y());

    SetSelectionSizeWorker w( _size, size );
    workOnCells( _marker, w );
}


struct SetSelectionUpperLowerWorker : public KSpreadTable::CellWorker {
    int _type;
    SetSelectionUpperLowerWorker( int type ) : KSpreadTable::CellWorker( false ), _type( type ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
	return new KSpreadUndoChangeAreaTextCell( doc, table, r );
    }
    bool testCondition( KSpreadCell* c ) {
	return ( !c->isNumeric() && !c->isBool() &&!c->isFormula() && !c->isDefault()
		 && !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
		 && !c->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	if ( _type == -1 )
	    cell->setCellText( (cell->text().lower()));
	else if ( _type == 1 )
	    cell->setCellText( (cell->text().upper()));
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionUpperLower( const QPoint &_marker, int _type )
{
    SetSelectionUpperLowerWorker w( _type );
    workOnCells( _marker, w );
}


struct SetSelectionFirstLetterUpperWorker : public KSpreadTable::CellWorker {
    SetSelectionFirstLetterUpperWorker( ) : KSpreadTable::CellWorker( false ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
	return   new KSpreadUndoChangeAreaTextCell( doc, table, r );
    }
    bool testCondition( KSpreadCell* c ) {
	return ( !c->isNumeric() && !c->isBool() &&!c->isFormula() && !c->isDefault()
		 && !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
		 && !c->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	QString tmp = cell->text();
	int len = tmp.length();
	cell->setCellText( (tmp.at(0).upper()+tmp.right(len-1)) );
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionfirstLetterUpper( const QPoint &_marker)
{
    SetSelectionFirstLetterUpperWorker w;
    workOnCells( _marker, w );
}


struct SetSelectionVerticalTextWorker : public KSpreadTable::CellWorker {
    bool _b;
    SetSelectionVerticalTextWorker( bool b ) : KSpreadTable::CellWorker( ), _b( b ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
        QString title=i18n("Vertical Text");
	return new KSpreadUndoCellLayout( doc, table, r, title );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	cell->setVerticalText( _b );
	cell->setMultiRow( false );
	cell->setAngle( 0 );
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionVerticalText( const QPoint &_marker, bool _b )
{
    SetSelectionVerticalTextWorker w( _b );
    workOnCells( _marker, w );
}


struct SetSelectionCommentWorker : public KSpreadTable::CellWorker {
    QString _comment;
    SetSelectionCommentWorker( QString comment ) : KSpreadTable::CellWorker( ), _comment( comment ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
        QString title=i18n("Add Comment");
	return new KSpreadUndoCellLayout( doc, table, r, title );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	cell->setComment( _comment );
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionComment( const QPoint &_marker, const QString &_comment)
{
    SetSelectionCommentWorker w( _comment );
    workOnCells( _marker, w );
}


struct SetSelectionAngleWorker : public KSpreadTable::CellWorkerTypeA {
    int _value;
    SetSelectionAngleWorker( int value ) : _value( value ) { }

    QString getUndoTitle() { return i18n("Change Angle"); }
    bool testCondition( RowLayout* rw ) {
        return ( rw->hasProperty( KSpreadCell::PAngle ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setAngle( _value );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setAngle( _value );
    }
    void prepareCell( KSpreadCell* cell ) {
	cell->clearProperty( KSpreadCell::PAngle );
	cell->clearNoFallBackProperties( KSpreadCell::PAngle );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setAngle( _value );
	if ( cellRegion ) {
	    cell->setVerticalText(false);
	    cell->setMultiRow( false );
	    cell->clearDisplayDirtyFlag();
	}
    }
};

void KSpreadTable::setSelectionAngle( const QPoint &_marker, int _value )
{
    SetSelectionAngleWorker w( _value );
    workOnCells( _marker, w );
}

struct SetSelectionRemoveCommentWorker : public KSpreadTable::CellWorker {
    SetSelectionRemoveCommentWorker( ) : KSpreadTable::CellWorker( false ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
        QString title=i18n("Remove Comment");
	return new KSpreadUndoCellLayout( doc, table, r, title );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	cell->setComment( "" );
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionRemoveComment( const QPoint &_marker)
{
    SetSelectionRemoveCommentWorker w;
    workOnCells( _marker, w );
}


struct SetSelectionTextColorWorker : public KSpreadTable::CellWorkerTypeA {
    const QColor& tb_Color;
    SetSelectionTextColorWorker( const QColor& _tb_Color ) : tb_Color( _tb_Color ) { }

    QString getUndoTitle() { return i18n("Change Text Color"); }
    bool testCondition( RowLayout* rw ) {
        return ( rw->hasProperty( KSpreadCell::PTextPen ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setTextColor( tb_Color );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setTextColor( tb_Color );
    }
    void prepareCell( KSpreadCell* cell ) {
	cell->clearProperty( KSpreadCell::PTextPen );
	cell->clearNoFallBackProperties( KSpreadCell::PTextPen );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setTextColor( tb_Color );
	if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionTextColor( const QPoint &_marker, const QColor &tb_Color )
{
    SetSelectionTextColorWorker w( tb_Color );
    workOnCells( _marker, w );
}


struct SetSelectionBgColorWorker : public KSpreadTable::CellWorkerTypeA {
    const QColor& bg_Color;
    SetSelectionBgColorWorker( const QColor& _bg_Color ) : bg_Color( _bg_Color ) { }

    QString getUndoTitle() { return i18n("Change Background Color"); }
    bool testCondition( RowLayout* rw ) {
        return ( rw->hasProperty( KSpreadCell::PBackgroundColor ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setBgColor( bg_Color );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setBgColor( bg_Color );
    }
    void prepareCell( KSpreadCell* cell ) {
	cell->clearProperty( KSpreadCell::PBackgroundColor );
	cell->clearNoFallBackProperties( KSpreadCell::PBackgroundColor );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setBgColor( bg_Color );
	if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionbgColor( const QPoint &_marker, const QColor &bg_Color )
{
    SetSelectionBgColorWorker w( bg_Color );
    workOnCells( _marker, w );
}


struct SetSelectionBorderColorWorker : public KSpreadTable::CellWorker {
    const QColor& bd_Color;
    SetSelectionBorderColorWorker( const QColor& _bd_Color ) : KSpreadTable::CellWorker( false ), bd_Color( _bd_Color ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
        QString title=i18n("Change Border Color");
	return new KSpreadUndoCellLayout( doc, table, r, title );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	int it_Row = cell->row();
	int it_Col = cell->column();
	if ( cell->topBorderStyle( it_Row, it_Col )!=Qt::NoPen )
	    cell->setTopBorderColor( bd_Color );
	if ( cell->leftBorderStyle( it_Row, it_Col )!=Qt::NoPen )
	    cell->setLeftBorderColor( bd_Color );
	if ( cell->fallDiagonalStyle( it_Row, it_Col )!=Qt::NoPen )
	    cell->setFallDiagonalColor( bd_Color );
	if ( cell->goUpDiagonalStyle( it_Row, it_Col )!=Qt::NoPen )
	    cell->setGoUpDiagonalColor( bd_Color );
	if ( cell->bottomBorderStyle( it_Row, it_Col )!=Qt::NoPen )
	    cell->setBottomBorderColor( bd_Color );
	if ( cell->rightBorderStyle( it_Row, it_Col )!=Qt::NoPen )
	    cell->setRightBorderColor( bd_Color );
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionBorderColor( const QPoint &_marker, const QColor &bd_Color )
{
    SetSelectionBorderColorWorker w( bd_Color );
    workOnCells( _marker, w );
}


void KSpreadTable::setSeries( const QPoint &_marker, double start, double end, double step, Series mode, Series type)
{
  doc()->emitBeginOperation();

  QString cellText;

  int x,y; /* just some loop counters */

  /* the actual number of columns or rows that the series will span.
     i.e. this will count 3 cells for a single cell that spans three rows
  */
  int numberOfCells;
  if (end > start)
    numberOfCells = (int) ((end - start) / step + 1); /* initialize for linear */
  else
    numberOfCells = (int) ((start - end) / step + 1); /* initialize for linear */

  if (type == Geometric)
  {
    /* basically, A(n) = start ^ n
     * so when does end = start ^ n ??
     * when n = ln(end) / ln(start)
     */
    numberOfCells = (int)( (log((double)end) / log((double)start)) +
			     DBL_EPSILON) + 1;
  }

  KSpreadCell * cell = NULL;

  /* markers for the top-left corner of the undo region.  It'll probably
   * be the top left corner of where the series is, but if something in front
   * is obscuring the cell, then it needs to be part of the undo region */
  QRect undoRegion;

  undoRegion.setLeft(_marker.x());
  undoRegion.setTop(_marker.y());

  /* this whole block is used to find the correct size for the undo region.
     We're checking for two different things (in these examples,
       mode==column):

       1.  cells are vertically merged.  This means that one value in the
       series will span multiple cells.

       2.  a cell in the column is merged to a cell to its left.  In this case
       the cell value will be stored in the left most cell so we need to
       extend the undo range to include that column.
  */
  if(mode == Column)
  {
    for ( y = _marker.y(); y <= (_marker.y() + numberOfCells - 1); y++ )
    {
      cell = cellAt( _marker.x(), y );

      if ( cell->isObscuringForced() )
      {
        /* case 2. */
        cell = cell->obscuringCells().getFirst();
        undoRegion.setLeft(QMIN(undoRegion.left(), cell->column()));
      }
      /* case 1.  Add the extra space to numberOfCells and then skip
	     over the region.  Note that because of the above if block 'cell'
	     points to the correct cell in the case where both case 1 and 2
	     are true
      */
      numberOfCells += cell->extraYCells();
      y += cell->extraYCells();
    }
    undoRegion.setRight( _marker.x() );
    undoRegion.setBottom( y - 1 );
  }
  else if(mode == Row)
  {
    for ( x = _marker.x(); x <=(_marker.x() + numberOfCells - 1); x++ )
    {
      /* see the code above for a column series for a description of
         what is going on here. */
      cell = cellAt( x,_marker.y() );

      if ( cell->isObscuringForced() )
      {
        cell = cell->obscuringCells().getFirst();
        undoRegion.setTop(QMIN(undoRegion.top(), cell->row()));
      }
      numberOfCells += cell->extraXCells();
      x += cell->extraXCells();
    }
    undoRegion.setBottom( _marker.y() );
    undoRegion.setRight( x - 1 );
  }

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoChangeAreaTextCell *undo = new
      KSpreadUndoChangeAreaTextCell( m_pDoc, this, undoRegion );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }


  x = _marker.x();
  y = _marker.y();

  /* now we're going to actually loop through and set the values */
  double incr;

  if (step >= 0 && start < end)
  {
    for ( incr = start; incr <= end; )
    {
      cell = nonDefaultCell( x, y );

      if (cell->isObscuringForced())
      {
        cell = cell->obscuringCells().getFirst();
      }

      //      cell->setCellText(cellText.setNum( incr ));
      cell->setCellText(m_pDoc->locale()->formatNumber(incr, 9));
      if (mode == Column)
      {
        ++y;
        if (cell->isForceExtraCells())
        {
          y += cell->extraYCells();
        }
      }
      else if (mode == Row)
      {
        ++x;
        if (cell->isForceExtraCells())
        {
          x += cell->extraXCells();
        }
      }
      else
      {
        kdDebug(36001) << "Error in Series::mode" << endl;
        return;
      }

      if (type == Linear)
        incr = incr + step;
      else if (type == Geometric)
        incr = incr * step;
      else
      {
        kdDebug(36001) << "Error in Series::type" << endl;
        return;
      }
    }
  }
  else
  if (step >= 0 && start > end)
  {
    for ( incr = start; incr >= end; )
    {
      cell = nonDefaultCell( x, y );

      if (cell->isObscuringForced())
      {
        cell = cell->obscuringCells().getFirst();
      }

      //      cell->setCellText(cellText.setNum( incr ));
      cell->setCellText(m_pDoc->locale()->formatNumber(incr, 9));
      if (mode == Column)
      {
        ++y;
        if (cell->isForceExtraCells())
        {
          y += cell->extraYCells();
        }
      }
      else if (mode == Row)
      {
        ++x;
        if (cell->isForceExtraCells())
        {
          x += cell->extraXCells();
        }
      }
      else
      {
        kdDebug(36001) << "Error in Series::mode" << endl;
        return;
      }

      if (type == Linear)
        incr = incr + step;
      else if (type == Geometric)
        incr = incr * step;
      else
      {
        kdDebug(36001) << "Error in Series::type" << endl;
        return;
      }
    }
  }
  else
  {
    for ( incr = start; incr >= end; )
    {
      cell = nonDefaultCell( x, y );

      if (cell->isObscuringForced())
      {
        cell = cell->obscuringCells().getFirst();
      }

      //cell->setCellText(cellText.setNum( incr ));
      cell->setCellText(m_pDoc->locale()->formatNumber(incr, 9));
      if (mode == Column)
      {
        ++y;
        if (cell->isForceExtraCells())
        {
          y += cell->extraYCells();
        }
      }
      else if (mode == Row)
      {
        ++x;
        if (cell->isForceExtraCells())
        {
          x += cell->extraXCells();
        }
      }
      else
      {
        kdDebug(36001) << "Error in Series::mode" << endl;
        return;
      }

      if (type == Linear)
        incr = incr + step;
      else if (type == Geometric)
      {
        incr = incr * step;
      }
      else
      {
        kdDebug(36001) << "Error in Series::type" << endl;
        return;
      }
    }
  }
  doc()->emitEndOperation();

}


struct SetSelectionPercentWorker : public KSpreadTable::CellWorkerTypeA {
    bool b;
    SetSelectionPercentWorker( bool _b ) : b( _b ) { }

    QString getUndoTitle() { return i18n("Format Percent"); }
    bool testCondition( RowLayout* rw ) {
        return ( rw->hasProperty( KSpreadCell::PFactor ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setFactor( b ? 100.0 : 1.0 );
	//rw->setPrecision( 0 );
	rw->setFormatType( b ? KSpreadCell::Percentage : KSpreadCell::Number);
    }
    void doWork( ColumnLayout* cl ) {
	cl->setFactor( b ? 100.0 : 1.0 );
	cl->setFormatType( b ? KSpreadCell::Percentage : KSpreadCell::Number);
    }
    void prepareCell( KSpreadCell* cell ) {
	cell->clearProperty(KSpreadCell::PFactor);
	cell->clearNoFallBackProperties( KSpreadCell::PFactor );
	cell->clearProperty(KSpreadCell::PFormatType);
	cell->clearNoFallBackProperties( KSpreadCell::PFormatType );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setFactor( b ? 100.0 : 1.0 );
	cell->setFormatType( b ? KSpreadCell::Percentage : KSpreadCell::Number);
	if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionPercent( const QPoint &_marker, bool b )
{
    SetSelectionPercentWorker w( b );
    workOnCells( _marker, w );
}


void KSpreadTable::refreshRemoveAreaName(const QString & _areaName)
{
  KSpreadCell * c = m_cells.firstCell();
  QString tmp = "'" + _areaName + "'";
  for( ;c ; c = c->nextCell() )
  {
    if ( c->isFormula() )
    {
      if (c->text().find(tmp) != -1)
      {
        if ( !c->makeFormula() )
          kdError(36001) << "ERROR: Syntax ERROR" << endl;
      }
    }
  }
}

void KSpreadTable::changeCellTabName(QString old_name,QString new_name)
{
    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if(c->isFormula() || c->content()==KSpreadCell::RichText)
        {
            if(c->text().find(old_name)!=-1)
            {
                int nb = c->text().contains(old_name+"!");
                QString tmp=old_name+"!";
                int len = tmp.length();
                tmp=c->text();

                for( int i=0; i<nb; i++ )
                {
                    int pos = tmp.find( old_name+"!" );
                    tmp.replace( pos, len, new_name+"!" );
                }
                c->setCellText(tmp);
            }
        }
    }
}

bool KSpreadTable::shiftRow( const QRect &rect,bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked()  &&makeUndo)
    {
        KSpreadUndoInsertCellRow *undo = new KSpreadUndoInsertCellRow( m_pDoc, this,rect );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    bool res=true;
    bool result;
    for( int i=rect.top(); i<=rect.bottom(); i++ )
    {
        for( int j=0; j<=(rect.right()-rect.left()); j++ )
        {
            result = m_cells.shiftRow( QPoint(rect.left(),i) );
            if( !result )
                res=false;
        }
    }
    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
    {
        for(int i=rect.top();i<=rect.bottom();i++)
            it.current()->changeNameCellRef( QPoint(rect.left(),i), false, KSpreadTable::ColumnInsert, name() ,(rect.right()-rect.left()+1));
    }
    refreshChart(QPoint(rect.left(),rect.top()), false, KSpreadTable::ColumnInsert);
    recalc();
    refreshMergedCell();
    emit sig_updateView( this );

    return res;
}

bool KSpreadTable::shiftColumn( const QRect& rect,bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked()  &&makeUndo)
    {
        KSpreadUndoInsertCellCol *undo = new KSpreadUndoInsertCellCol( m_pDoc, this,rect);
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    bool res=true;
    bool result;
    for( int i =rect.left(); i<=rect.right(); i++ )
    {
        for( int j=0; j<=(rect.bottom()-rect.top()); j++ )
        {
            result = m_cells.shiftColumn( QPoint(i,rect.top()) );
            if(!result)
                res=false;
        }
    }

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
    {
        for(int i=rect.left();i<=rect.right();i++)
            it.current()->changeNameCellRef( QPoint(i,rect.top()), false, KSpreadTable::RowInsert, name() ,(rect.bottom()-rect.top()+1));
    }
    refreshChart(/*marker*/QPoint(rect.left(),rect.top()), false, KSpreadTable::RowInsert);
    recalc();
    refreshMergedCell();
    emit sig_updateView( this );

    return res;
}

void KSpreadTable::unshiftColumn( const QRect & rect,bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked()  &&makeUndo)
    {
        KSpreadUndoRemoveCellCol *undo = new KSpreadUndoRemoveCellCol( m_pDoc, this,rect);
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    for(int i =rect.top();i<=rect.bottom();i++)
        for(int j=rect.left();j<=rect.right();j++)
               m_cells.remove(j,i);

    for(int i =rect.left();i<=rect.right();i++)
        for(int j=0;j<=(rect.bottom()-rect.top());j++)
                m_cells.unshiftColumn( QPoint(i,rect.top()) );

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        for(int i=rect.left();i<=rect.right();i++)
                it.current()->changeNameCellRef( QPoint(i,rect.top()), false, KSpreadTable::RowRemove, name(),(rect.bottom()-rect.top()+1) );

    refreshChart( QPoint(rect.left(),rect.top()), false, KSpreadTable::RowRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateView( this );
}

void KSpreadTable::unshiftRow( const QRect & rect,bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked() &&makeUndo)
    {
            KSpreadUndoRemoveCellRow *undo = new KSpreadUndoRemoveCellRow( m_pDoc, this,rect );
            m_pDoc->undoBuffer()->appendUndo( undo );
    }
    for(int i =rect.top();i<=rect.bottom();i++)
        for(int j=rect.left();j<=rect.right();j++)
                m_cells.remove(j,i);

    for(int i =rect.top();i<=rect.bottom();i++)
        for(int j=0;j<=(rect.right()-rect.left());j++)
                m_cells.unshiftRow( QPoint(rect.left(),i) );

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        for(int i=rect.top();i<=rect.bottom();i++)
                it.current()->changeNameCellRef( QPoint(rect.left(),i), false, KSpreadTable::ColumnRemove, name(),(rect.right()-rect.left()+1) );

    refreshChart(QPoint(rect.left(),rect.top()), false, KSpreadTable::ColumnRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateView( this );
}

bool KSpreadTable::insertColumn( int col, int nbCol, bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked() && makeUndo)
    {
        KSpreadUndoInsertColumn *undo = new KSpreadUndoInsertColumn( m_pDoc, this, col, nbCol );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    bool res=true;
    bool result;
    for( int i=0; i<=nbCol; i++ )
    {
	// Recalculate range max (minus size of last column)
	m_ulSizeMaxX -= columnLayout( KS_colMax )->width();

        result = m_cells.insertColumn( col );
        m_columns.insertColumn( col );
        if(!result)
	    res=false;

	//Recalculate range max (plus size of new column)
	m_ulSizeMaxX += columnLayout( col+i )->width();
    }

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true, KSpreadTable::ColumnInsert, name(), nbCol+1 );

    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int left = m_printRange.left();
        int right = m_printRange.right();

        for( int i=0; i<=nbCol; i++ )
        {
            if ( left >= col ) left++;
            if ( right >= col ) right++;
        }
        //Validity checks
        if ( left > KS_colMax ) left = KS_colMax;
        if ( right > KS_colMax ) right = KS_colMax;
        setPrintRange( QRect( QPoint( left, m_printRange.top() ), QPoint( right, m_printRange.bottom() ) ) );
    }

    refreshChart( QPoint( col, 1 ), true, KSpreadTable::ColumnInsert );
    refreshMergedCell();
    recalc();
    emit sig_updateHBorder( this );
    emit sig_updateView( this );

    return res;
}

bool KSpreadTable::insertRow( int row, int nbRow, bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked() && makeUndo)
    {
        KSpreadUndoInsertRow *undo = new KSpreadUndoInsertRow( m_pDoc, this, row, nbRow );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    bool res=true;
    bool result;
    for( int i=0; i<=nbRow; i++ )
    {
	// Recalculate range max (minus size of last row)
	m_ulSizeMaxY -= rowLayout( KS_rowMax )->height();

	result = m_cells.insertRow( row );
	m_rows.insertRow( row );
	if( !result )
	    res = false;

	//Recalculate range max (plus size of new row)
	m_ulSizeMaxY += rowLayout( row )->height();
    }

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true, KSpreadTable::RowInsert, name(), nbRow+1 );

    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int top = m_printRange.top();
        int bottom = m_printRange.bottom();

        for( int i=0; i<=nbRow; i++ )
        {
            if ( top >= row ) top++;
            if ( bottom >= row ) bottom++;
        }
        //Validity checks
        if ( top > KS_rowMax ) top = KS_rowMax;
        if ( bottom > KS_rowMax ) bottom = KS_rowMax;
        setPrintRange( QRect( QPoint( m_printRange.left(), top ), QPoint( m_printRange.right(), bottom ) ) );
    }

    refreshChart( QPoint( 1, row ), true, KSpreadTable::RowInsert );
    refreshMergedCell();
    recalc();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );

    return res;
}

void KSpreadTable::removeColumn( int col, int nbCol, bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked() && makeUndo)
    {
        KSpreadUndoRemoveColumn *undo = new KSpreadUndoRemoveColumn( m_pDoc, this, col, nbCol );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    for( int i=0; i<=nbCol; i++ )
    {
	// Recalculate range max (minus size of removed column)
	m_ulSizeMaxX -= columnLayout( col )->width();

	m_cells.removeColumn( col );
	m_columns.removeColumn( col );

	//Recalculate range max (plus size of new column)
	m_ulSizeMaxX += columnLayout( KS_colMax )->width();
    }

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true, KSpreadTable::ColumnRemove, name(), nbCol+1 );

    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int left = m_printRange.left();
        int right = m_printRange.right();

        for( int i=0; i<=nbCol; i++ )
        {
            if ( left > col ) left--;
            if ( right >= col ) right--;
        }
        //Validity checks
        if ( left < 1 ) left = 1;
        if ( right < 1 ) right = 1;
        setPrintRange( QRect( QPoint( left, m_printRange.top() ), QPoint( right, m_printRange.bottom() ) ) );
    }

    //update repeat columns, when it has been defined
    if ( m_printRepeatColumns.first != 0 )
    {
        int left = m_printRepeatColumns.first;
        int right = m_printRepeatColumns.second;

        for( int i=0; i<=nbCol; i++ )
        {
            if ( left > col ) left--;
            if ( right >= col ) right--;
        }
        //Validity checks
        if ( left < 1 ) left = 1;
        if ( right < 1 ) right = 1;
        setPrintRepeatColumns ( qMakePair( left, right ) );
    }

    refreshChart( QPoint( col, 1 ), true, KSpreadTable::ColumnRemove );
    recalc();
    refreshMergedCell();
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}

void KSpreadTable::removeRow( int row, int nbRow, bool makeUndo )
{
    if ( !m_pDoc->undoBuffer()->isLocked() && makeUndo )
    {
        KSpreadUndoRemoveRow *undo = new KSpreadUndoRemoveRow( m_pDoc, this, row, nbRow );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    for( int i=0; i<=nbRow; i++ )
    {
	// Recalculate range max (minus size of removed row)
	m_ulSizeMaxY -= rowLayout( row )->height();

	m_cells.removeRow( row );
	m_rows.removeRow( row );

	//Recalculate range max (plus size of new row)
	m_ulSizeMaxY += rowLayout( KS_rowMax )->height();
    }

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true, KSpreadTable::RowRemove, name(), nbRow+1 );

    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int top = m_printRange.top();
        int bottom = m_printRange.bottom();

        for( int i=0; i<=nbRow; i++ )
        {
            if ( top > row ) top--;
            if ( bottom >= row ) bottom--;
        }
        //Validity checks
        if ( top < 1 ) top = 1;
        if ( bottom < 1 ) bottom = 1;
        setPrintRange( QRect( QPoint( m_printRange.left(), top ), QPoint( m_printRange.right(), bottom ) ) );
    }

    //update repeat rows, when it has been defined
    if ( m_printRepeatRows.first != 0 )
    {
        int top = m_printRepeatRows.first;
        int bottom = m_printRepeatRows.second;

        for( int i=0; i<=nbRow; i++ )
        {
            if ( top > row ) top--;
            if ( bottom >= row ) bottom--;
        }
        //Validity checks
        if ( top < 1 ) top = 1;
        if ( bottom < 1 ) bottom = 1;
        setPrintRepeatRows( qMakePair( top, bottom ) );
    }

    refreshChart( QPoint( 1, row ), true, KSpreadTable::RowRemove );
    recalc();
    refreshMergedCell();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void KSpreadTable::hideRow( int _row, int nbRow, QValueList<int>_list )
{
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoHideRow *undo ;
      if( nbRow!=-1 )
	undo= new KSpreadUndoHideRow( m_pDoc, this, _row, nbRow );
      else
	undo= new KSpreadUndoHideRow( m_pDoc, this, _row, nbRow, _list );
      m_pDoc->undoBuffer()->appendUndo( undo  );
    }

    RowLayout *rl;
    if( nbRow!=-1 )
    {
	for( int i=0; i<=nbRow; i++ )
	{
	    rl=nonDefaultRowLayout( _row+i );
	    rl->setHide(true);
	}
    }
    else
    {
	QValueList<int>::Iterator it;
	for( it = _list.begin(); it != _list.end(); ++it )
	{
	    rl=nonDefaultRowLayout( *it );
	    rl->setHide(true);
	}
    }
    emitHideRow();
}

void KSpreadTable::emitHideRow()
{
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void KSpreadTable::showRow( int _row, int nbRow, QValueList<int>_list )
{
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoShowRow *undo;
      if(nbRow!=-1)
        undo = new KSpreadUndoShowRow( m_pDoc, this, _row,nbRow );
      else
	undo = new KSpreadUndoShowRow( m_pDoc, this, _row,nbRow, _list );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    RowLayout *rl;
    if( nbRow!=-1 )
      {
	for( int i=0; i<=nbRow; i++ )
	  {
	    rl=nonDefaultRowLayout( _row + i );
	    rl->setHide( false );
	  }
      }
    else
      {
	QValueList<int>::Iterator it;
	for( it = _list.begin(); it != _list.end(); ++it )
	  {
	    rl=nonDefaultRowLayout( *it );
	    rl->setHide( false );
	  }
      }
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}


void KSpreadTable::hideColumn( int _col, int nbCol, QValueList<int>_list )
{
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        KSpreadUndoHideColumn *undo;
	if( nbCol!=-1 )
	  undo= new KSpreadUndoHideColumn( m_pDoc, this, _col, nbCol );
	else
	  undo= new KSpreadUndoHideColumn( m_pDoc, this, _col, nbCol, _list );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    ColumnLayout *cl;
    if( nbCol != -1 )
    {
	for( int i=0; i<=nbCol; i++ )
	{
	    cl=nonDefaultColumnLayout( _col + i );
	    cl->setHide( true );
	}
    }
    else
    {
	QValueList<int>::Iterator it;
	for( it = _list.begin(); it != _list.end(); ++it )
	{
	    cl=nonDefaultColumnLayout( *it );
	    cl->setHide( true );
	}
    }
    emitHideColumn();
}

void KSpreadTable::emitHideColumn()
{
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}


void KSpreadTable::showColumn( int _col, int nbCol, QValueList<int>_list )
{
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoShowColumn *undo;
      if( nbCol != -1 )
	undo = new KSpreadUndoShowColumn( m_pDoc, this, _col, nbCol );
      else
	undo = new KSpreadUndoShowColumn( m_pDoc, this, _col, nbCol, _list );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    ColumnLayout *cl;
    if( nbCol != -1 )
    {
      for( int i=0; i<=nbCol; i++ )
      {
	cl=nonDefaultColumnLayout( _col + i );
	cl->setHide( false );
      }
    }
    else
    {
       QValueList<int>::Iterator it;
       for( it = _list.begin(); it != _list.end(); ++it )
       {
	   cl=nonDefaultColumnLayout( *it );
	   cl->setHide( false );
       }
    }
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}


void KSpreadTable::refreshChart(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref)
{
  KSpreadCell * c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    if ( (ref == ColumnInsert || ref == ColumnRemove) && fullRowOrColumn
        && c->column() >= (pos.x() - 1))
    {
      if (c->updateChart())
        return;
    }
    else if ( (ref == ColumnInsert || ref == ColumnRemove )&& !fullRowOrColumn
              && c->column() >= (pos.x() - 1) && c->row() == pos.y() )
    {
      if (c->updateChart())
        return;
    }
    else if ((ref == RowInsert || ref == RowRemove) && fullRowOrColumn
             && c->row() >= (pos.y() - 1))
    {
      if (c->updateChart())
        return;
    }
    else if ( (ref == RowInsert || ref == RowRemove) && !fullRowOrColumn
        && c->column() == pos.x() && c->row() >= (pos.y() - 1) )
    {
      if (c->updateChart())
        return;
    }
  }

  //refresh chart when there is a chart and you remove
  //all cells
  if (c == 0L)
  {
     CellBinding * bind;
     for ( bind = firstCellBinding(); bind != 0L; bind = nextCellBinding() )
     {
       bind->cellChanged( 0 );
     }
     //    CellBinding * bind = firstCellBinding();
     //    if ( bind != 0L )
     //      bind->cellChanged( 0 );
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

QRect KSpreadTable::selectionCellMerged(const QRect &_sel)
{
  KSpreadCell *cell;
  QRect selection(_sel);

  cell = cellAt(selection.left(), selection.top());
  if( isColumnSelected() || isRowSelected() )
    return selection;
  else if ( !(cell->isObscured() && cell->isObscuringForced()) &&
            (cell->extraXCells() + 1) == selection.width() &&
            (cell->extraYCells() + 1) == selection.height())
  {
    /* if just a single cell is selected, we need to merge even when
       the obscuring isn't forced.  But only if this is the cell that
       is doing the obscuring -- we still want to be able to click on a cell
       that is being obscured.
    */
    selection.setWidth(cell->extraXCells() + 1);
    selection.setHeight(cell->extraYCells() + 1);
  }
  else
  {
    int top=selection.top();
    int left=selection.left();
    int bottom=selection.bottom();
    int right=selection.right();
    for ( int x = selection.left(); x <= selection.right(); x++ )
      for ( int y = selection.top(); y <= selection.bottom(); y++ )
      {
        cell = cellAt( x, y );
        if( cell->isForceExtraCells())
        {
          right=QMAX(right,cell->extraXCells()+x);
          bottom=QMAX(bottom,cell->extraYCells()+y);
        }
        else if ( cell->isObscured() && cell->isObscuringForced() )
        {
          cell = cell->obscuringCells().getFirst();
          left=QMIN(left,cell->column());
          top=QMIN(top,cell->row());
          bottom=QMAX(bottom,cell->row() + cell->extraYCells());
          right=QMAX(right,cell->column() + cell->extraXCells());
        }
      }

    selection.setCoords(left,top,right,bottom);
  }
  return selection;
}

void KSpreadTable::changeNameCellRef(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref, QString tabname, int nbCol)
{
  bool correctDefaultTableName = (tabname == name()); // for cells without table ref (eg "A1")
  KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    if(c->isFormula())
    {
      QString origText = c->text();
      unsigned int i = 0;
      QString newText;

      bool correctTableName = correctDefaultTableName;
      //bool previousCorrectTableName = false;
      QChar origCh;
      for ( ; i < origText.length(); ++i )
      {
        origCh = origText[i];
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
          bool tableNameFound = false; //Table names need spaces
          for( ; ( i < origText.length() ) &&  // until the end
                 (  ( origText[i].isLetter() || origText[i].isDigit() || origText[i] == '$' ) ||  // all text and numbers are welcome
                    ( tableNameFound && origText[i].isSpace() ) ) //in case of a table name, we include spaces too
               ; ++i )
          {
            str += origText[i];
            if ( origText[i] == '!' )
              tableNameFound = true;
          }
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
                newText += '$';

              if(ref==ColumnInsert
                 && correctTableName
                 && col>=pos.x()     // Column after the new one : +1
                 && ( fullRowOrColumn || row == pos.y() ) ) // All rows or just one
              {
                newText += util_encodeColumnLabelText( col+nbCol );
              }
              else if(ref==ColumnRemove
                      && correctTableName
                      && col > pos.x() // Column after the deleted one : -1
                      && ( fullRowOrColumn || row == pos.y() ) ) // All rows or just one
              {
                newText += util_encodeColumnLabelText( col-nbCol );
              }
              else
                newText += util_encodeColumnLabelText( col );

              // Update row
              if ( point.rowFixed )
                newText += '$';

              if(ref==RowInsert
                 && correctTableName
                 && row >= pos.y() // Row after the new one : +1
                 && ( fullRowOrColumn || col == pos.x() ) ) // All columns or just one
              {
                newText += QString::number( row+nbCol );
              }
              else if(ref==RowRemove
                      && correctTableName
                      && row > pos.y() // Column after the deleted one : -1
                      && ( fullRowOrColumn || col == pos.x() ) ) // All columns or just one
              {
                newText += QString::number( row-nbCol );
              }
              else
                newText += QString::number( row );
            }
            else // Not a cell ref
            {
              kdDebug(36001) << "Copying (unchanged) : '" << str << "'" << endl;
              newText += str;
            }
            // Copy the char that got us to stop
            if ( i < origText.length() )
              newText += origText[i];
          }
        }
      }
      c->setCellText(newText, false /* no recalc deps for each, done independently */ );
    }
  }
}

void KSpreadTable::find( const QPoint &_marker, QString _find, long options, KSpreadCanvas *canvas )
{
    // Identify the region of interest.
    QRect region( m_rctSelection );
    if (options & KoFindDialog::SelectedText)
    {

        // Complete rows selected ?
        if ( isRowSelected() )
        {
        }
        // Complete columns selected ?
        else if ( isColumnSelected() )
        {
        }
    }
    else
    {
        // All cells.
        region.setCoords( 0, 0, m_iMaxColumn, m_iMaxRow );
    }

    // Create the class that handles all the actual Find stuff, and connect it to its
    // local slots.
    KoFind dialog( _find, options );
    QObject::connect(
        &dialog, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
        canvas, SLOT( highlight( const QString &, int, int, const QRect & ) ) );

    // Now do the finding...
    QRect cellRegion( 0, 0, 0, 0 );
    bool bck = options & KoFindDialog::FindBackwards;

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( options & KoFindDialog::FromCursor ) {
        colStart = _marker.x();
        rowStart =  _marker.y();
    }
    KSpreadCell *cell;
    for (int row = rowStart ; !bck ? row < rowEnd : row > rowEnd ; !bck ? ++row : --row )
    {
        for(int col = colStart ; !bck ? col < colEnd : col > colEnd ; !bck ? ++col : --col )
        {
            cell = cellAt( col, row );
            if ( !cell->isDefault() && !cell->isObscured() && !cell->isFormula() )
            {
                QString text = cell->text();
                cellRegion.setTop( row );
                cellRegion.setLeft( col );
                if ( !dialog.find( text, cellRegion ) )
                    return;
            }
        }
    }
}

void KSpreadTable::replace( const QPoint &_marker, QString _find, QString _replace, long options, KSpreadCanvas *canvas )
{
    // Identify the region of interest.
    QRect region( m_rctSelection );
    if (options & KoReplaceDialog::SelectedText)
    {

        // Complete rows selected ?
        if ( isRowSelected() )
        {
        }
        // Complete columns selected ?
        else if ( isColumnSelected() )
        {
        }
    }
    else
    {
        // All cells.
        region.setCoords( 0, 0, m_iMaxRow, m_iMaxColumn );
    }

    // Create the class that handles all the actual replace stuff, and connect it to its
    // local slots.
    KoReplace dialog( _find, _replace, options );
    QObject::connect(
        &dialog, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
        canvas, SLOT( highlight( const QString &, int, int, const QRect & ) ) );
    QObject::connect(
        &dialog, SIGNAL( replace( const QString &, int, int,int, const QRect & ) ),
        canvas, SLOT( replace( const QString &, int, int,int, const QRect & ) ) );

    // Now do the replacing...
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this, region );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    QRect cellRegion( 0, 0, 0, 0 );
    bool bck = options & KoFindDialog::FindBackwards;

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( options & KoFindDialog::FromCursor ) {
        colStart = _marker.x();
        rowStart =  _marker.y();
    }
    KSpreadCell *cell;
    for (int row = rowStart ; !bck ? row < rowEnd : row > rowEnd ; !bck ? ++row : --row )
    {
        for(int col = colStart ; !bck ? col < colEnd : col > colEnd ; !bck ? ++col : --col )
        {
            cell = cellAt( col, row );
            if ( !cell->isDefault() && !cell->isObscured() && !cell->isFormula() )
            {
                QString text = cell->text();
                cellRegion.setTop( row );
                cellRegion.setLeft( col );
                if (!dialog.replace( text, cellRegion ))
                    return;
            }
        }
    }
}

void KSpreadTable::borderBottom( const QPoint &/*_marker */,const QColor &_color )
{
  QRect r( m_rctSelection );

  QPen pen( _color,1,SolidLine);

  // Complete rows selected ?
  if ( isRowSelected() )
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      QString title = i18n("Change Border");
      KSpreadUndoCellLayout * undo = new KSpreadUndoCellLayout( m_pDoc, this, r, title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    int row = m_rctSelection.bottom();
    KSpreadCell * c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PBottomBorder );
      c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );

      c = getNextCellRight( c->column(), row );
    }

    /* TODO: remove that code if proven that code above works
      for( ;c; c = c->nextCell() )
      {
      row = c->row();
      if ( m_rctSelection.bottom() == row )
      {
        c->clearProperty( KSpreadCell::PBottomBorder );
        c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );
      }
      }
    */

    RowLayout * rw = nonDefaultRowLayout(m_rctSelection.bottom());
    rw->setBottomBorderPen(pen);

    emit sig_updateView( this );
    return;
  }
  // Complete columns selected ?
  else if ( isColumnSelected() )
  {
    //nothing
    return;
  }
  else
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      QString title=i18n("Change Border");
      KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_pDoc, this, r,title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    KSpreadCell* cell;
    int y;
    for ( int x = r.left(); x <= r.right(); ++x )
    {
      y = r.bottom();
      cell = nonDefaultCell( x, y );
      cell->setBottomBorderPen(pen);
    }
    emit sig_updateView( this, r );
  }
}

void KSpreadTable::borderRight( const QPoint &/* _marker */,const QColor &_color )
{
  QRect r( m_rctSelection );

  QPen pen( _color,1,SolidLine);
  // Complete rows selected ?
  if ( isRowSelected() )
  {
    //nothing
    return;
  }
  // Complete columns selected ?
  else if ( isColumnSelected() )
  {

    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      QString title = i18n("Change Border");
      KSpreadUndoCellLayout * undo = new KSpreadUndoCellLayout( m_pDoc, this,
                                                                r, title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    int col = m_rctSelection.right();
    KSpreadCell * c = getFirstCellColumn( col );
    while ( c )
    {
      if ( !c->isObscuringForced() )
      {
        c->clearProperty( KSpreadCell::PRightBorder );
        c->clearNoFallBackProperties( KSpreadCell::PRightBorder );
      }
      c = getNextCellDown( col, c->row() );
    }

    /* TODO: to be removed
      KSpreadCell* c = m_cells.firstCell();
      int col;
      for( ;c; c = c->nextCell() )
      {
      col = c->column();
      if ( m_rctSelection.right() == col &&!c->isObscuringForced())
      {
        c->clearProperty( KSpreadCell::PRightBorder );
        c->clearNoFallBackProperties( KSpreadCell::PRightBorder );
      }
      }
    */

    RowLayout * rw = m_rows.first();
/*    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PRightBorder)))
      {
        for(int i=m_rctSelection.left();i<=m_rctSelection.right();i++)
        {
          KSpreadCell *cell = nonDefaultCell( i,  rw->row() );
        }
      }
    }
*/
    ColumnLayout * cl = nonDefaultColumnLayout(m_rctSelection.right());
    cl->setRightBorderPen(pen);

    KSpreadCell * cell;
    rw = m_rows.first();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PRightBorder)))
      {
        for(int i=m_rctSelection.left();i<=m_rctSelection.right();i++)
        {
          cell = nonDefaultCell( i, rw->row() );
          cell->setRightBorderPen(pen);
        }
      }
    }

    emit sig_updateView( this );
    return;
  }
  else
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      QString title=i18n("Change Border");
      KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_pDoc, this, r,title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    KSpreadCell* cell;
    int x;
    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
      x = r.right();
      cell = nonDefaultCell( x, y );
      cell->setRightBorderPen(pen);
    }
    emit sig_updateView( this, r );
  }
}

void KSpreadTable::borderLeft( const QPoint &/*_marker*/, const QColor &_color )
{
  QString title = i18n("Change Border");
  QRect r( m_rctSelection );

  QPen pen( _color,1,SolidLine);

  // Complete columns selected ?
  if ( isColumnSelected() )
  {
    RowLayout* rw =m_rows.first();
/*    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PLeftBorder)))
      {
        for(int i=m_rctSelection.left();i<=m_rctSelection.right();i++)
        {
          KSpreadCell *cell = nonDefaultCell( i,  rw->row() );
        }
      }
    }
*/
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_pDoc, this,
                                                               r, title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    int col = m_rctSelection.left();
    KSpreadCell * c = getFirstCellColumn( col );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PLeftBorder );
      c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );

      c = getNextCellDown( col, c->row() );
    }


    /* TODO: to be removed
      KSpreadCell * c = m_cells.firstCell();
      int col;
      for( ;c; c = c->nextCell() )
      {
      col = c->column();
      if ( col==m_rctSelection.left() )
      {
        c->clearProperty( KSpreadCell::PLeftBorder );
        c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
      }
      }
    */
    ColumnLayout * cl = nonDefaultColumnLayout( col );
    cl->setLeftBorderPen(pen);

    KSpreadCell * cell;
    rw = m_rows.first();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PLeftBorder)))
      {
        for(int i = m_rctSelection.left(); i <= m_rctSelection.right(); ++i)
        {
          cell = nonDefaultCell( i,  rw->row() );
          cell->setLeftBorderPen(pen);
        }
      }
    }

    emit sig_updateView( this );
    return;
  }
  else
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_pDoc, this,
                                                               r,title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    KSpreadCell* cell;
    int x;
    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
      x = r.left();
      cell = nonDefaultCell( x, y );
      cell->setLeftBorderPen(pen);
    }
    emit sig_updateView( this, r );
  }
}

void KSpreadTable::borderTop( const QPoint &/*_marker*/,const QColor &_color )
{
  /* duplicate code in kspread_dlg_layout.cc  That needs fixed at some point
   */
  QRect r( m_rctSelection );

  QString title = i18n("Change Border");
  QPen pen( _color, 1, SolidLine);
  // Complete rows selected ?
  if ( isRowSelected() )
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoCellLayout * undo = new KSpreadUndoCellLayout( m_pDoc, this, r,title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    int row = m_rctSelection.top();
    KSpreadCell * c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PTopBorder );
      c->clearNoFallBackProperties( KSpreadCell::PTopBorder );

      c = getNextCellRight( c->column(), row );
    }

    /* TODO: to be removed:
      KSpreadCell* c = m_cells.firstCell();
      int row;
      for( ;c; c = c->nextCell() )
      {
      row = c->row();
      if ( m_rctSelection.top() == row )
      {
        c->clearProperty( KSpreadCell::PTopBorder );
        c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
      }
      }
    */

    RowLayout * rw = nonDefaultRowLayout( row );
    rw->setTopBorderPen( pen );

    emit sig_updateView( this );
    return;
  }
  // Complete columns selected ? -- the top will just be row 1, then
  // so it's the same as in no rows/columns selected
  else
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_pDoc, this, r,title );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

    KSpreadCell* cell;
    int y;
    for ( int x = r.left(); x <= r.right(); x++ )
    {
      y = r.top();
      cell = nonDefaultCell( x, y );
      cell->setTopBorderPen(pen);
    }
    emit sig_updateView( this, r );
  }
}

void KSpreadTable::borderOutline( const QPoint &/*_marker*/,const QColor &_color )
{
  QRect r( m_rctSelection );

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    QString title = i18n("Change Border");
    KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_pDoc, this,
                                                             r,title );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  QPen pen( _color, 1, SolidLine);

  // Complete rows selected ?
  if ( isRowSelected() )
  {
    int row = m_rctSelection.top();
    KSpreadCell * c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PTopBorder );
      c->clearNoFallBackProperties( KSpreadCell::PTopBorder );

      c = getNextCellRight( c->column(), row );
    }

    row = m_rctSelection.bottom();
    c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PBottomBorder );
      c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );

      c = getNextCellRight( c->column(), row );
    }

    /* TODO: to be removed
      KSpreadCell * c = m_cells.firstCell();
      int row;
      for( ;c; c = c->nextCell() )
      {
      row = c->row();
      if ( m_rctSelection.top() == row )
      {
        c->clearProperty( KSpreadCell::PTopBorder );
        c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
      }
      else if ( m_rctSelection.bottom() == row)
      {
        c->clearProperty( KSpreadCell::PBottomBorder );
        c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );
      }
      }
    */

    RowLayout * rw = nonDefaultRowLayout( m_rctSelection.top() );
    rw->setTopBorderPen(pen);
    rw=nonDefaultRowLayout(m_rctSelection.bottom());
    rw->setBottomBorderPen(pen);
    KSpreadCell* cell;
    int bottom = r.bottom();
    int left   = r.left();
    for ( int y = r.top(); y <= bottom; ++y )
    {
      cell = nonDefaultCell( left, y );
      cell->setLeftBorderPen(pen);
    }
    emit sig_updateView( this );
    return;
  }
  // Complete columns selected ?
  else if ( isColumnSelected() )
  {
    int col = m_rctSelection.left();
    KSpreadCell * c = getFirstCellColumn( col );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PLeftBorder );
      c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );

      c = getNextCellDown( col, c->row() );
    }

    col = m_rctSelection.right();
    c = getFirstCellColumn( col );
    while ( c )
    {
      c->clearProperty( KSpreadCell::PRightBorder );
      c->clearNoFallBackProperties( KSpreadCell::PRightBorder );

      c = getNextCellDown( col, c->row() );
    }

    /* TODO: to be removed
      for( ; c; c = c->nextCell() )
      {
      int row = c->row();
      if ( m_rctSelection.left() == row )
      {
        c->clearProperty( KSpreadCell::PLeftBorder );
        c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
      }
      else if ( m_rctSelection.right() == row)
      {
        c->clearProperty( KSpreadCell::PRightBorder );
        c->clearNoFallBackProperties( KSpreadCell::PRightBorder );
      }
      }
    */

    ColumnLayout *cl=nonDefaultColumnLayout(m_rctSelection.left());
    cl->setLeftBorderPen(pen);
    cl=nonDefaultColumnLayout(m_rctSelection.right());
    cl->setRightBorderPen(pen);
    KSpreadCell* cell;
    for ( int x = r.left(); x <= r.right(); x++ )
    {
      cell = nonDefaultCell( x, r.top() );
      cell->setTopBorderPen(pen);
    }
    emit sig_updateView( this );
    return;
  }
  else
  {
    for ( int x = r.left(); x <= r.right(); x++ )
    {
      nonDefaultCell( x, r.top() )->setTopBorderPen(pen);
      nonDefaultCell( x, r.bottom() )->setBottomBorderPen(pen);
    }
    for ( int y = r.top(); y <= r.bottom(); y++ )
    {
      nonDefaultCell( r.left(), y )->setLeftBorderPen(pen);
      nonDefaultCell( r.right(), y )->setRightBorderPen(pen);
    }
    emit sig_updateView( this, r );
  }
}

struct SetSelectionBorderAllWorker : public KSpreadTable::CellWorkerTypeA {
    QPen pen;
    SetSelectionBorderAllWorker( const QColor& color ) : pen( color, 1, QPen::SolidLine ) { }

    QString getUndoTitle() { return i18n("Change Border"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PRightBorder )
		 || rw->hasProperty( KSpreadCell::PLeftBorder )
		 || rw->hasProperty( KSpreadCell::PTopBorder )
		 || rw->hasProperty( KSpreadCell::PBottomBorder ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setTopBorderPen( pen );
        rw->setRightBorderPen( pen );
        rw->setLeftBorderPen( pen );
        rw->setBottomBorderPen( pen );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setTopBorderPen( pen );
        cl->setRightBorderPen( pen );
        cl->setLeftBorderPen( pen );
        cl->setBottomBorderPen( pen );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PTopBorder );
	c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
	c->clearProperty( KSpreadCell::PBottomBorder );
	c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );
	c->clearProperty( KSpreadCell::PLeftBorder );
	c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
	c->clearProperty( KSpreadCell::PRightBorder );
	c->clearNoFallBackProperties( KSpreadCell::PRightBorder );
    }

    bool testCondition(KSpreadCell */*cell*/) { return true; }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	//if ( cellRegion )
	//    cell->setDisplayDirtyFlag();
	cell->setTopBorderPen( pen );
        cell->setRightBorderPen( pen );
        cell->setLeftBorderPen( pen );
        cell->setBottomBorderPen( pen );
	//if ( cellRegion )
	//    cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::borderAll( const QPoint &_marker,const QColor &_color )
{
    SetSelectionBorderAllWorker w( _color );
    workOnCells( _marker, w );
}

struct SetSelectionBorderRemoveWorker : public KSpreadTable::CellWorkerTypeA {
    QPen pen;
    SetSelectionBorderRemoveWorker() : pen( Qt::black, 1, Qt::NoPen  ) { }
    QString getUndoTitle() { return i18n("Change Border"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PRightBorder )
		 || rw->hasProperty( KSpreadCell::PLeftBorder )
		 || rw->hasProperty( KSpreadCell::PTopBorder )
		 || rw->hasProperty( KSpreadCell::PBottomBorder )
		 || rw->hasProperty( KSpreadCell::PFallDiagonal )
		 || rw->hasProperty( KSpreadCell::PGoUpDiagonal ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setTopBorderPen( pen );
        rw->setRightBorderPen( pen );
        rw->setLeftBorderPen( pen );
        rw->setBottomBorderPen( pen);
        rw->setFallDiagonalPen( pen );
        rw->setGoUpDiagonalPen (pen );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setTopBorderPen( pen );
        cl->setRightBorderPen( pen );
        cl->setLeftBorderPen( pen );
        cl->setBottomBorderPen( pen);
        cl->setFallDiagonalPen( pen );
        cl->setGoUpDiagonalPen (pen );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PTopBorder );
	c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
	c->clearProperty( KSpreadCell::PLeftBorder );
	c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
	c->clearProperty( KSpreadCell::PRightBorder );
	c->clearNoFallBackProperties( KSpreadCell::PRightBorder );
	c->clearProperty( KSpreadCell::PBottomBorder );
	c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );
	c->clearProperty( KSpreadCell::PFallDiagonal );
	c->clearNoFallBackProperties( KSpreadCell::PFallDiagonal );
	c->clearProperty( KSpreadCell::PGoUpDiagonal );
	c->clearNoFallBackProperties( KSpreadCell::PGoUpDiagonal );
    }

    bool testCondition(KSpreadCell* /*cell*/ ){ return true; }

    void doWork( KSpreadCell* cell, bool, int, int ) {
	//if ( cellRegion )
	//    cell->setDisplayDirtyFlag();
	cell->setTopBorderPen( pen );
        cell->setRightBorderPen( pen );
        cell->setLeftBorderPen( pen );
        cell->setBottomBorderPen( pen);
        cell->setFallDiagonalPen( pen );
        cell->setGoUpDiagonalPen (pen );
	//if ( cellRegion )
	//    cell->clearDisplayDirtyFlag();
    }
};


void KSpreadTable::borderRemove( const QPoint &_marker )
{
    SetSelectionBorderRemoveWorker w;
    workOnCells( _marker, w );
}


void KSpreadTable::sortByRow( int ref_row, SortingOrder mode )
{
  KSpreadPoint point;
  point.table = this;
  point.tableName = m_strName;
  point.pos = QPoint( selection().left(), selection().top() );
  point.columnFixed = false;
  point.rowFixed = false;

  sortByRow( ref_row, 0, 0, mode, mode, mode, 0, false, false, point );
}

void KSpreadTable::sortByColumn( int ref_column, SortingOrder mode )
{
  KSpreadPoint point;
  point.table = this;
  point.tableName = m_strName;
  point.pos = QPoint( selection().left(), selection().top() );
  point.columnFixed = false;
  point.rowFixed = false;

  sortByColumn( ref_column, 0, 0, mode, mode, mode, 0, false, false, point );
}

void KSpreadTable::checkCellContent(KSpreadCell * cell1, KSpreadCell * cell2, int & ret)
{
  if ( cell1->isEmpty() )
  {
    ret = 1;
    return;
  }
  else if ( cell1->isObscured() && cell1->isObscuringForced() )
  {
    ret = 1;
    return;
  }
  else if ( cell2->isEmpty() )
  {
    ret = 2;
    return;
  }
  ret = 0;
}

void KSpreadTable::sortByRow( int key1, int key2, int key3,
                              SortingOrder order1, SortingOrder order2, SortingOrder order3,
                              QStringList const * firstKey, bool copyLayout, bool headerRow,
                              KSpreadPoint const & outputPoint )
{
  QRect r( selection() );

  Q_ASSERT( order1 == Increase || order1 == Decrease );

  // It may not happen that entire columns are selected.
  Q_ASSERT( isColumnSelected() == FALSE );

  // Are entire rows selected ?
  if ( isRowSelected() )
  {
    r.setLeft( KS_colMax );
    r.setRight( 0 );

    // Determine a correct left and right.
    // Iterate over all cells to find out which cells are
    // located in the selected rows.
    for ( int row = r.top(); row <= r.bottom(); ++row )
    {
      KSpreadCell * c = getFirstCellRow( row );
      int col;
      while ( c )
      {
        col = c->column();
        if ( !c->isEmpty() )
        {
          if ( col > r.right() )
            r.rRight() = col;
          if ( col < r.left() )
            r.rLeft() = col;
        }
        c = getNextCellRight( col, row );
      }
    }

    /* TODO: to be removed
      KSpreadCell * c = m_cells.firstCell();
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
    */

    // Any cells to sort here ?
    if ( r.right() < r.left() )
      return;
  }

  QRect target( outputPoint.pos.x(), outputPoint.pos.y(), r.width(), r.height() );

  doc()->emitBeginOperation();

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoSort *undo = new KSpreadUndoSort( m_pDoc, this, target );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  if (target.topLeft() != r.topLeft())
  {
    int targetLeft = target.left();
    int targetTop  = target.top();
    int sourceTop  = r.top();
    int sourceLeft = r.left();

    key1 = key1 - sourceTop + targetTop;
    key2 = key2 - sourceTop + targetTop;
    key3 = key3 - sourceTop + targetTop;

    for ( int x = 0; x < r.width(); ++x)
    {
      for ( int y = 0; y < r.height(); ++y )
      {
        // from - to
        copyCells( sourceLeft + x, sourceTop + y,
                   targetLeft + x, targetTop + y, copyLayout );
      }
    }
  }

  // Sorting algorithm: David's :). Well, I guess it's called minmax or so.
  // For each column, we look for all cells right hand of it and we find the one to swap with it.
  // Much faster than the awful bubbleSort...
  KSpreadCell * cell;
  KSpreadCell * cell1;
  KSpreadCell * cell2;
  KSpreadCell * bestCell;
  int status = 0;

  for ( int d = target.left();  d <= target.right(); ++d )
  {
    cell1 = cellAt( d, key1 );
    if ( cell1->isObscured() && cell1->isObscuringForced() )
    {
      KSpreadCell* obscuring = cell1->obscuringCells().getFirst();
      cell = cellAt( obscuring->column(), key1 );
      cell1 = cellAt( obscuring->column() + cell->extraXCells() + 1,
                      obscuring->column());
      d = obscuring->column() + cell->extraXCells() + 1;
    }

    // Look for which column we want to swap with the one number d
    bestCell = cell1;
    int bestX = d;
    for ( int x = d + 1 ; x <= target.right(); x++ )
    {
      cell2 = cellAt( x, key1 );

      checkCellContent(cell2, bestCell, status);
      if (status == 1)
        continue;
      else if (status == 2)
      {
        // empty cells are always shifted to the end
        bestCell = cell2;
        bestX = x;
        continue;
      }

      if ( firstKey )
      {
        int i1 = firstKey->findIndex( cell2->text() );
        int i2 = firstKey->findIndex( bestCell->text() );

        if ( i1 != -1 && i2 != -1 )
        {
          if ( (order1 == Increase && i1 < i2 )
               || (order1 == Decrease && i1 > i2) )
          {
            bestCell = cell2;
            bestX = x;
            continue;
          }

          if ( i1 == i2 )
          {
            // check 2nd key
            if (key2 <= 0)
              continue;

            KSpreadCell * cell22 = cellAt( d, key2 );
            KSpreadCell * bestCell2 = cellAt( x, key2 );

            if ( cell22->isEmpty() )
            {
              /* No need to swap */
              continue;
            }
            else if ( cell22->isObscured() && cell22->isObscuringForced() )
            {
              /* No need to swap */
              continue;
            }
            else if ( bestCell2->isEmpty() )
            {
              // empty cells are always shifted to the end
              bestCell = cell2;
              bestX = x;
              continue;
            }

            if ( (order2 == Increase && *cell22 > *bestCell2)
                 || (order2 == Decrease && *cell22 < *bestCell2) )
            {
              bestCell = cell2;
              bestX = x;
              continue;
            }
            else if ( (order2 == Increase && *cell22 < *bestCell2)
                      || (order2 == Decrease && *cell22 > *bestCell2) )
            {
              // already in right order
              continue;
            }
            else
            {
              // they are equal, check 3rd key
              if (key3 <= 0)
                continue;

              KSpreadCell * cell23 = cellAt( d, key3 );
              KSpreadCell * bestCell3 = cellAt( x, key3 );

              if ( cell23->isEmpty() )
              {
                /* No need to swap */
                continue;
              }
              else if ( cell23->isObscured() && cell23->isObscuringForced() )
              {
                /* No need to swap */
                continue;
              }
              else if ( bestCell3->isEmpty() )
              {
                // empty cells are always shifted to the end
                bestCell = cell2;
                bestX = x;
                continue;
              }
              if ( (order3 == Increase && *cell23 > *bestCell3)
                   || (order3 == Decrease && *cell23 < *bestCell3) )
              {
                // they are really equal or in the right order
                // no swap necessary
                continue;
              }
              else
              {
                bestCell = cell2;
                bestX = x;
                continue;
              }
            }
          }
          continue;
        }
        else if ( i1 != -1 && i2 == -1 )
        {
          // if not in the key list, the cell is shifted to the end - always
          bestCell = cell2;
          bestX = x;
          continue;
       }
        else if ( i2 != -1 && i1 == -1 )
        {
          // only text of cell2 is in the list so it is smaller than bestCell
          /* No need to swap */
          continue;
        }

        // if i1 and i2 are equals -1 go on:
      } // end if (firstKey)

      // Here we use the operators < and > for cells, which do it all.
      if ( (order1 == Increase && *cell2 < *bestCell)
           || (order1 == Decrease && *cell2 > *bestCell) )
      {
        bestCell = cell2;
        bestX = x;
        continue;
      }
      else if ( (order1 == Increase && *cell2 > *bestCell)
                || (order1 == Decrease && *cell2 < *bestCell) )
      {
        // no change necessary
        continue;
      }
      else
      {
        // *cell2 equals *bestCell
        // check 2nd key
        if (key2 <= 0)
          continue;
        KSpreadCell * cell22 = cellAt( d, key2 );
        KSpreadCell * bestCell2 = cellAt( x, key2 );

        checkCellContent(cell2, bestCell, status);
        if (status == 1)
          continue;
        else if (status == 2)
        {
          // empty cells are always shifted to the end
          bestCell = cell2;
          bestX = x;
          continue;
        }

        if ( (order2 == Increase && *cell22 > *bestCell2)
             || (order2 == Decrease && *cell22 < *bestCell2) )
        {
          bestCell = cell2;
          bestX = x;
          continue;
        }
        else
        if ( (order2 == Increase && *cell22 > *bestCell2)
             || (order2 == Decrease && *cell22 < *bestCell2) )
        {
          // already in right order
          continue;
        }
        else
        {
          // they are equal, check 3rd key
          if (key3 == 0)
            continue;
          KSpreadCell * cell23 = cellAt( d, key3 );
          KSpreadCell * bestCell3 = cellAt( x, key3 );

          checkCellContent(cell2, bestCell, status);
          if (status == 1)
            continue;
          else if (status == 2)
          {
            // empty cells are always shifted to the end
            bestCell = cell2;
            bestX = x;
            continue;
          }
          if ( (order3 == Increase && *cell23 > *bestCell3)
               || (order3 == Decrease && *cell23 < *bestCell3) )
          {
            bestCell = cell2;
            bestX = x;
            continue;
          }
          else
          {
            // they are really equal
            // no swap necessary
            continue;
          }
        }
      }
    }

    // Swap columns cell1 and bestCell (i.e. d and bestX)
    if ( d != bestX )
    {
      int top = target.top();
      if (headerRow)
        ++top;

      for( int y = target.bottom(); y >= top; --y )
      {
        if ( y != key1 && y != key2 && y != key3 )
          swapCells( d, y, bestX, y, copyLayout );
      }
      if (key3 > 0)
        swapCells( d, key3, bestX, key3, copyLayout );
      if (key2 > 0)
        swapCells( d, key2, bestX, key2, copyLayout );
      swapCells( d, key1, bestX, key1, copyLayout );
    }
  } // for (d = ...; ...; ++d)

  doc()->emitEndOperation();
}

void KSpreadTable::sortByColumn( int key1, int key2, int key3,
                                 SortingOrder order1, SortingOrder order2, SortingOrder order3,
                                 QStringList const * firstKey, bool copyLayout, bool headerRow,
                                 KSpreadPoint const & outputPoint )
{
  QRect r( m_rctSelection );

  Q_ASSERT( order1 == Increase || order1 == Decrease );

  // It may not happen that entire rows are selected.
  Q_ASSERT( isRowSelected() == FALSE );

  // Are entire columns selected ?
  if ( isColumnSelected() )
  {
    r.setTop( KS_rowMax );
    r.setBottom( 0 );

    // Determine a correct top and bottom.
    // Iterate over all cells to find out which cells are
    // located in the selected columns.
    for ( int col = r.left(); col <= r.right(); ++col )
    {
      KSpreadCell * c = getFirstCellColumn( col );
      int row;
      while ( c )
      {
        row = c->row();
        if ( !c->isEmpty() )
        {
          if ( row > r.bottom() )
            r.rBottom() = row;
          if ( row < r.top() )
            r.rTop() = row;
        }
        c = getNextCellDown( col, row );
      }
    }

    /* TODO: to be removed
      KSpreadCell * c = m_cells.firstCell();
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
    */

    // Any cells to sort here ?
    if ( r.bottom() < r.top() )
      return;
  }
  QRect target( outputPoint.pos.x(), outputPoint.pos.y(), r.width(), r.height() );

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoSort *undo = new KSpreadUndoSort( m_pDoc, this, target );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  doc()->emitBeginOperation();

  if (target.topLeft() != r.topLeft())
  {
    int targetLeft = target.left();
    int targetTop  = target.top();
    int sourceTop  = r.top();
    int sourceLeft = r.left();

    key1 = key1 - sourceLeft + targetLeft;
    key2 = key2 - sourceLeft + targetLeft;
    key3 = key3 - sourceLeft + targetLeft;

    for ( int x = 0; x < r.width(); ++x)
    {
      for ( int y = 0; y < r.height(); ++y )
      {
        // from - to
        copyCells( sourceLeft + x, sourceTop + y,
                   targetLeft + x, targetTop + y, copyLayout );
      }
    }
  }

  // Sorting algorithm: David's :). Well, I guess it's called minmax or so.
  // For each row, we look for all rows under it and we find the one to swap with it.
  // Much faster than the awful bubbleSort...
  // Torben: Asymptotically it is alltogether O(n^2) :-)

  KSpreadCell * cell;
  KSpreadCell * cell1;
  KSpreadCell * cell2;
  KSpreadCell * bestCell;
  int status = 0;

  int d = target.top();

  if (headerRow)
    ++d;

  for ( ; d <= target.bottom(); ++d )
  {
    // Look for which row we want to swap with the one number d
    cell1 = cellAt( key1, d );
    if ( cell1->isObscured() && cell1->isObscuringForced() )
    {
      KSpreadCell* obscuring = cell1->obscuringCells().getFirst();
      cell  = cellAt( key1, obscuring->row() );
      cell1 = cellAt( key1, obscuring->row() + cell->extraYCells() + 1 );
      d     = obscuring->row() + cell->extraYCells() + 1;
    }

    bestCell  = cell1;
    int bestY = d;

    for ( int y = d + 1 ; y <= target.bottom(); ++y )
    {
      cell2 = cellAt( key1, y );

      if ( cell2->isEmpty() )
      {
        /* No need to swap */
        continue;
      }
      else if ( cell2->isObscured() && cell2->isObscuringForced() )
      {
        /* No need to swap */
        continue;
      }
      else if ( bestCell->isEmpty() )
      {
        // empty cells are always shifted to the end
        bestCell = cell2;
        bestY = y;
        continue;
      }

      if ( firstKey )
      {
        int i1 = firstKey->findIndex( cell2->text() );
        int i2 = firstKey->findIndex( bestCell->text() );

        if ( i1 != -1 && i2 != -1 )
        {
          if ( (order1 == Increase && i1 < i2 )
               || (order1 == Decrease && i1 > i2) )
          {
            bestCell = cell2;
            bestY = y;
            continue;
          }

          if ( i1 == i2 )
          {
            // check 2nd key
            if (key2 <= 0)
              continue;
            KSpreadCell * cell22 = cellAt( key2, d );
            KSpreadCell * bestCell2 = cellAt( key2, y );

            if ( cell22->isEmpty() )
            {
              /* No need to swap */
              continue;
            }
            else if ( cell22->isObscured() && cell22->isObscuringForced() )
            {
              /* No need to swap */
              continue;
            }
            else if ( bestCell2->isEmpty() )
            {
              // empty cells are always shifted to the end
              bestCell = cell2;
              bestY = y;
              continue;
            }

            if ( (order2 == Increase && *cell22 > *bestCell2)
                 || (order2 == Decrease && *cell22 < *bestCell2) )
            {
              bestCell = cell2;
              bestY = y;
              continue;
            }
            else if ( (order2 == Increase && *cell22 < *bestCell2)
                      || (order2 == Decrease && *cell22 > *bestCell2) )
            {
              // already in right order
              continue;
            }
            else
            {
              // they are equal, check 3rd key
              if (key3 <= 0)
                continue;
              KSpreadCell * cell23 = cellAt( key3, d );
              KSpreadCell * bestCell3 = cellAt( key3, y );

              checkCellContent(cell2, bestCell, status);
              if (status == 1)
                continue;
              else if (status == 2)
              {
                // empty cells are always shifted to the end
                bestCell = cell2;
                bestY = y;
                continue;
              }

              if ( (order3 == Increase && *cell23 < *bestCell3)
                   || (order3 == Decrease && *cell23 > *bestCell3) )
              {
                bestCell = cell2;
                bestY = y;
                continue;
              }
              else
              {
                // they are really equal or in the correct order
                // no swap necessary
                continue;
              }
            }
          }
          continue;
        }
        else if ( i1 != -1 && i2 == -1 )
        {
          // if not in the key list, the cell is shifted to the end - always
          bestCell = cell2;
          bestY = y;
          continue;
        }
        else if ( i2 != -1 && i1 == -1 )
        {
          // only text of cell2 is in the list so it is smaller than bestCell
          /* No need to swap */
          continue;
        }

        // if i1 and i2 are equals -1 go on:
      } // if (firstKey)


      // Here we use the operators < and > for cells, which do it all.
      if ( (order1 == Increase && *cell2 < *bestCell)
           || (order1 == Decrease && *cell2 > *bestCell) )
      {
        bestCell = cell2;
        bestY = y;
      }
      else if ( (order1 == Increase && *cell2 > *bestCell)
                || (order1 == Decrease && *cell2 < *bestCell) )
      {
        // no change necessary
        continue;
      }
      else
      {
        // *cell2 equals *bestCell
        // check 2nd key
        if (key2 == 0)
          continue;
        KSpreadCell * cell22 = cellAt( key2, d );
        KSpreadCell * bestCell2 = cellAt( key2, y );

        if ( cell22->isEmpty() )
        {
          /* No need to swap */
          continue;
        }
        else if ( cell22->isObscured() && cell22->isObscuringForced() )
        {
          /* No need to swap */
          continue;
        }
        else if ( bestCell2->isEmpty() )
        {
          // empty cells are always shifted to the end
          bestCell = cell2;
          bestY = y;
          continue;
        }

        if ( (order2 == Increase && *cell22 > *bestCell2)
             || (order2 == Decrease && *cell22 < *bestCell2) )
        {
          bestCell = cell2;
          bestY = y;
          continue;
        }
        else if ( (order2 == Increase && *cell22 < *bestCell2)
                  || (order2 == Decrease && *cell22 > *bestCell2) )
        {
          continue;
        }
        else
        {
          // they are equal, check 3rd key
          if (key3 == 0)
            continue;
          KSpreadCell * cell23 = cellAt( key3, d );
          KSpreadCell * bestCell3 = cellAt( key3, y );

          if ( cell23->isEmpty() )
          {
            /* No need to swap */
            continue;
          }
          else if ( cell23->isObscured() && cell23->isObscuringForced() )
          {
            /* No need to swap */
            continue;
          }
          else if ( bestCell3->isEmpty() )
          {
            // empty cells are always shifted to the end
            bestCell = cell2;
            bestY = y;
            continue;
          }

          if ( (order3 == Increase && *cell23 > *bestCell3)
               || (order3 == Decrease && *cell23 < *bestCell3) )
          {
            bestCell = cell2;
            bestY = y;
            continue;
          }
          else
          {
            // they are really equal or already in the correct order
            // no swap necessary
            continue;
          }
        }
      }
    }

    // Swap rows cell1 and bestCell (i.e. d and bestY)
    if ( d != bestY )
    {
      for (int x = target.left(); x <= target.right(); ++x)
      {
        if ( x != key1 && x != key2 && x != key3)
          swapCells( x, d, x, bestY, copyLayout );
      }
      if (key3 > 0)
        swapCells( key3, d, key3, bestY, copyLayout );
      if (key2 > 0)
        swapCells( key2, d, key2, bestY, copyLayout );
      swapCells( key1, d, key1, bestY, copyLayout );
    }
  } // for (d = ...; ...; ++d)
  doc()->emitEndOperation();
}

// from - to - copyLayout
void KSpreadTable::copyCells( int x1, int y1, int x2, int y2, bool cpLayout )
{
  KSpreadCell * sourceCell = cellAt( x1, y1 );
  KSpreadCell * targetCell = cellAt( x2, y2 );

  if ( sourceCell->isDefault() )
  {
    // if the source and target is default there is nothing to copy
    if ( targetCell->isDefault() )
    {
      return;
    }
    else // overwrite target with defaultcell
    {
      targetCell = new KSpreadCell( this, x2, y2 );
      insertCell( targetCell );
      return;
    }
  }

  if ( targetCell->isDefault() )
  {
    targetCell = new KSpreadCell( this, x2, y2 );
    insertCell( targetCell );
  }


  // TODO: check if this enough
  targetCell->copyContent( sourceCell );

  /*
    if ( !sourceCell->isFormula() )
    {
    targetCell->copyContent( sourceCell );
    }
    else
    {
    targetCell->setCellText( targetCell->decodeFormula( sourceCell->encodeFormula() ) );
    targetCell->setCalcDirtyFlag();
    targetCell->calc(false);
  }
  */

  if (cpLayout)
  {
    targetCell->copyLayout( sourceCell );
    /*
    targetCell->setAlign( sourceCell->align( x1, y1 ) );
    targetCell->setAlignY( sourceCell->alignY( x1, y1 ) );
    targetCell->setTextFont( sourceCell->textFont( x1, y1 ) );
    targetCell->setTextColor( sourceCell->textColor( x1, y1 ) );
    targetCell->setBgColor( sourceCell->bgColor( x1, y1 ) );
    targetCell->setLeftBorderPen( sourceCell->leftBorderPen( x1, y1 ) );
    targetCell->setTopBorderPen( sourceCell->topBorderPen( x1, y1 ) );
    targetCell->setBottomBorderPen( sourceCell->bottomBorderPen( x1, y1 ) );
    targetCell->setRightBorderPen( sourceCell->rightBorderPen( x1, y1 ) );
    targetCell->setFallDiagonalPen( sourceCell->fallDiagonalPen( x1, y1 ) );
    targetCell->setGoUpDiagonalPen( sourceCell->goUpDiagonalPen( x1, y1 ) );
    targetCell->setBackGroundBrush( sourceCell->backGroundBrush( x1, y1 ) );
    targetCell->setPrecision( sourceCell->precision( x1, y1 ) );
    targetCell->setPrefix( sourceCell->prefix( x1, y1 ) );
    targetCell->setPostfix( sourceCell->postfix( x1, y1 ) );
    targetCell->setFloatFormat( sourceCell->floatFormat( x1, y1 ) );
    targetCell->setFloatColor( sourceCell->floatColor( x1, y1 ) );
    targetCell->setFactor( sourceCell->factor( x1, y1 ) );
    targetCell->setMultiRow( sourceCell->multiRow( x1, y1 ) );
    targetCell->setVerticalText( sourceCell->verticalText( x1, y1 ) );
    targetCell->setStyle( sourceCell->style() );
    targetCell->setDontPrintText( sourceCell->getDontprintText( x1, y1 ) );
    targetCell->setIndent( sourceCell->getIndent( x1, y1 ) );
    targetCell->SetConditionList(sourceCell->GetConditionList());
    targetCell->setComment( sourceCell->comment( x1, y1 ) );
    targetCell->setAngle( sourceCell->getAngle( x1, y1 ) );
    targetCell->setFormatType( sourceCell->getFormatType( x1, y1 ) );
    */
  }
}

void KSpreadTable::swapCells( int x1, int y1, int x2, int y2, bool cpLayout )
{
  KSpreadCell * ref1 = cellAt( x1, y1 );
  KSpreadCell * ref2 = cellAt( x2, y2 );

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
  // information. Imagine sorting in a table. Swapping
  // the layout while sorting is not what you would expect
  // as a user.
  if (!ref1->isFormula() && !ref2->isFormula())
  {
    KSpreadCell *tmp = new KSpreadCell( this, -1, -1 );

    tmp->copyContent( ref1 );
    ref1->copyContent( ref2 );
    ref2->copyContent( tmp );

    delete tmp;
  }
  else
    if ( ref1->isFormula() && ref2->isFormula() )
    {
      QString d = ref1->encodeFormula();
      ref1->setCellText( ref1->decodeFormula( ref2->encodeFormula( ) ) );
      ref1->setCalcDirtyFlag();
      ref1->calc(false);
      ref2->setCellText( ref2->decodeFormula( d ) );
      ref2->setCalcDirtyFlag();
      ref2->calc(false);
    }
    else
      if (ref1->isFormula() && !ref2->isFormula() )
      {
        QString d = ref1->encodeFormula();
        ref1->setCellText(ref2->text(), true);
        ref1->setAction(ref2->action());
        ref2->setCellText(ref2->decodeFormula(d), true);
        ref2->setCalcDirtyFlag();
        ref2->calc(false);
      }
      else
        if (!ref1->isFormula() && ref2->isFormula() )
        {
          QString d = ref2->encodeFormula();
          ref2->setCellText(ref1->text(), true);
          ref2->setAction(ref1->action());
          ref1->setCellText(ref1->decodeFormula(d), true);
          ref1->setCalcDirtyFlag();
          ref1->calc(false);
        }

  if (cpLayout)
  {
    KSpreadLayout::Align a = ref1->align( ref1->column(), ref1->row() );
    ref1->setAlign( ref2->align( ref2->column(), ref2->row() ) );
    ref2->setAlign(a);

    KSpreadLayout::AlignY ay = ref1->alignY( ref1->column(), ref1->row() );
    ref1->setAlignY( ref2->alignY( ref2->column(), ref2->row() ) );
    ref2->setAlignY(ay);

    QFont textFont = ref1->textFont( ref1->column(), ref1->row() );
    ref1->setTextFont( ref2->textFont( ref2->column(), ref2->row() ) );
    ref2->setTextFont(textFont);

    QColor textColor = ref1->textColor( ref1->column(), ref1->row() );
    ref1->setTextColor( ref2->textColor( ref2->column(), ref2->row() ) );
    ref2->setTextColor(textColor);

    QColor bgColor = ref1->bgColor( ref1->column(), ref1->row() );
    ref1->setBgColor( ref2->bgColor( ref2->column(), ref2->row() ) );
    ref2->setBgColor(bgColor);

    QPen lbp = ref1->leftBorderPen( ref1->column(), ref1->row() );
    ref1->setLeftBorderPen( ref2->leftBorderPen( ref2->column(), ref2->row() ) );
    ref2->setLeftBorderPen(lbp);

    QPen tbp = ref1->topBorderPen( ref1->column(), ref1->row() );
    ref1->setTopBorderPen( ref2->topBorderPen( ref2->column(), ref2->row() ) );
    ref2->setTopBorderPen(tbp);

    QPen bbp = ref1->bottomBorderPen( ref1->column(), ref1->row() );
    ref1->setBottomBorderPen( ref2->bottomBorderPen( ref2->column(), ref2->row() ) );
    ref2->setBottomBorderPen(bbp);

    QPen rbp = ref1->rightBorderPen( ref1->column(), ref1->row() );
    ref1->setRightBorderPen( ref2->rightBorderPen( ref2->column(), ref2->row() ) );
    ref2->setRightBorderPen(rbp);

    QPen fdp = ref1->fallDiagonalPen( ref1->column(), ref1->row() );
    ref1->setFallDiagonalPen( ref2->fallDiagonalPen( ref2->column(), ref2->row() ) );
    ref2->setFallDiagonalPen(fdp);

    QPen udp = ref1->goUpDiagonalPen( ref1->column(), ref1->row() );
    ref1->setGoUpDiagonalPen( ref2->goUpDiagonalPen( ref2->column(), ref2->row() ) );
    ref2->setGoUpDiagonalPen(udp);

    QBrush bgBrush = ref1->backGroundBrush( ref1->column(), ref1->row() );
    ref1->setBackGroundBrush( ref2->backGroundBrush( ref2->column(), ref2->row() ) );
    ref2->setBackGroundBrush(bgBrush);

    int pre = ref1->precision( ref1->column(), ref1->row() );
    ref1->setPrecision( ref2->precision( ref2->column(), ref2->row() ) );
    ref2->setPrecision(pre);

    QString prefix = ref1->prefix( ref1->column(), ref1->row() );
    ref1->setPrefix( ref2->prefix( ref2->column(), ref2->row() ) );
    ref2->setPrefix(prefix);

    QString postfix = ref1->postfix( ref1->column(), ref1->row() );
    ref1->setPostfix( ref2->postfix( ref2->column(), ref2->row() ) );
    ref2->setPostfix(postfix);

    KSpreadLayout::FloatFormat f = ref1->floatFormat( ref1->column(), ref1->row() );
    ref1->setFloatFormat( ref2->floatFormat( ref2->column(), ref2->row() ) );
    ref2->setFloatFormat(f);

    KSpreadLayout::FloatColor c = ref1->floatColor( ref1->column(), ref1->row() );
    ref1->setFloatColor( ref2->floatColor( ref2->column(), ref2->row() ) );
    ref2->setFloatColor(c);

    double fact = ref1->factor( ref1->column(), ref1->row() );
    ref1->setFactor( ref2->factor( ref2->column(), ref2->row() ) );
    ref2->setFactor(fact);

    bool multi = ref1->multiRow( ref1->column(), ref1->row() );
    ref1->setMultiRow( ref2->multiRow( ref2->column(), ref2->row() ) );
    ref2->setMultiRow(multi);

    bool vert = ref1->verticalText( ref1->column(), ref1->row() );
    ref1->setVerticalText( ref2->verticalText( ref2->column(), ref2->row() ) );
    ref2->setVerticalText(vert);

    KSpreadCell::Style style = ref1->style();
    ref1->setStyle( ref2->style() );
    ref2->setStyle(style);

    bool print = ref1->getDontprintText( ref1->column(), ref1->row() );
    ref1->setDontPrintText( ref2->getDontprintText( ref2->column(), ref2->row() ) );
    ref2->setDontPrintText(print);

    int ind = ref1->getIndent( ref1->column(), ref1->row() );
    ref1->setIndent( ref2->getIndent( ref2->column(), ref2->row() ) );
    ref2->setIndent(ind);

    QValueList<KSpreadConditional> conditionList = ref1->GetConditionList();
    ref1->SetConditionList(ref2->GetConditionList());
    ref2->SetConditionList(conditionList);

    QString com = ref1->comment( ref1->column(), ref1->row() );
    ref1->setComment( ref2->comment( ref2->column(), ref2->row() ) );
    ref2->setComment(com);

    int angle = ref1->getAngle( ref1->column(), ref1->row() );
    ref1->setAngle( ref2->getAngle( ref2->column(), ref2->row() ) );
    ref2->setAngle(angle);

    KSpreadLayout::FormatType form = ref1->getFormatType( ref1->column(), ref1->row() );
    ref1->setFormatType( ref2->getFormatType( ref2->column(), ref2->row() ) );
    ref2->setFormatType(form);
  }
}

void KSpreadTable::refreshPreference()
{
  if(getAutoCalc())
        recalc();

  emit sig_updateHBorder( this );
  emit sig_updateView( this );
}


bool KSpreadTable::areaIsEmpty()
{
    // Complete rows selected ?
    if ( isRowSelected() )
    {
      for ( int row = m_rctSelection.top(); row <= m_rctSelection.bottom(); ++row )
      {
        KSpreadCell * c = getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isObscuringForced() && !c->text().isEmpty())
          {
            return false;
          }

          c = getNextCellRight( c->column(), row );
        }
      }

      /* TODO: to be removed
        KSpreadCell* c = m_cells.firstCell();
        int row;
        for( ;c; c = c->nextCell() )
        {
        row = c->row();
        if ( m_rctSelection.top() <= row && m_rctSelection.bottom() >= row
             &&!c->isObscuringForced() && !c->text().isEmpty())
        {
          return false;
        }
        }
      */
    }
    // Complete columns selected ?
    else if ( isColumnSelected() )
    {
      for ( int col = m_rctSelection.left(); col <= m_rctSelection.right(); ++col )
      {
        KSpreadCell * c = getFirstCellColumn( col );
        while ( c )
        {
         if ( !c->isObscuringForced() && !c->text().isEmpty() )
         {
           return false;
         }

          c = getNextCellDown( col, c->row() );
        }
      }

      /* TODO: to be removed
         KSpreadCell* c = m_cells.firstCell();
         int col;
         for( ;c; c = c->nextCell() )
         {
         col = c->column();
         if ( m_rctSelection.left() <= col && m_rctSelection.right() >= col
         &&!c->isObscuringForced() && !c->text().isEmpty())
         {
         return false;
         }
         }
      */
    }
    else
    {
        KSpreadCell * cell;
        QRect r( m_rctSelection );

        int right  = r.right();
        int bottom = r.bottom();
        for ( int x = r.left(); x <= right; ++x )
          for ( int y = r.top(); y <= bottom; ++y )
          {
            cell = cellAt( x, y );
            if (!cell->isObscuringForced() && !cell->text().isEmpty())
            {
              return false;
            }
          }
    }
    return true;
}

struct SetSelectionMultiRowWorker : public KSpreadTable::CellWorker
{
  bool enable;
  SetSelectionMultiRowWorker( bool _enable )
    : KSpreadTable::CellWorker( ), enable( _enable ) { }

  class KSpreadUndoAction* createUndoAction( KSpreadDoc * doc, KSpreadTable * table, QRect & r )
  {
    QString title = i18n("Multirow");
    return new KSpreadUndoCellLayout( doc, table, r, title );
  }

  bool testCondition( KSpreadCell * cell )
  {
    return ( !cell->isObscuringForced() );
  }

  void doWork( KSpreadCell * cell, bool, int, int )
  {
    cell->setDisplayDirtyFlag();
    cell->setMultiRow( enable );
    cell->setVerticalText( false );
    cell->setAngle( 0 );
    cell->clearDisplayDirtyFlag();
  }
};

void KSpreadTable::setSelectionMultiRow( const QPoint &_marker, bool enable )
{
    SetSelectionMultiRowWorker w( enable );
    workOnCells( _marker, w );
}


struct SetSelectionAlignWorker : public KSpreadTable::CellWorkerTypeA {
    KSpreadLayout::Align _align;
    SetSelectionAlignWorker( KSpreadLayout::Align align ) : _align( align ) { }
    QString getUndoTitle() { return i18n("Change Horizontal Alignment"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PAlign ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setAlign( _align );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setAlign( _align );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PAlign );
	c->clearNoFallBackProperties( KSpreadCell::PAlign );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setAlign( _align );
	if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};


void KSpreadTable::setSelectionAlign( const QPoint &_marker, KSpreadLayout::Align _align )
{
    SetSelectionAlignWorker w( _align );
    workOnCells( _marker, w );
}


struct SetSelectionAlignYWorker : public KSpreadTable::CellWorkerTypeA {
    KSpreadLayout::AlignY _alignY;
    SetSelectionAlignYWorker( KSpreadLayout::AlignY alignY ) : _alignY( alignY ) { }
    QString getUndoTitle() { return i18n("Change Vertical Alignment"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PAlignY ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setAlignY( _alignY );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setAlignY( _alignY );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PAlignY );
	c->clearNoFallBackProperties( KSpreadCell::PAlignY );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setAlignY( _alignY );
	if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};


void KSpreadTable::setSelectionAlignY( const QPoint &_marker, KSpreadLayout::AlignY _alignY )
{
    SetSelectionAlignYWorker w( _alignY );
    workOnCells( _marker, w );
}


struct SetSelectionPrecisionWorker : public KSpreadTable::CellWorker {
    int _delta;
    SetSelectionPrecisionWorker( int delta ) : KSpreadTable::CellWorker( ), _delta( delta ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
        QString title=i18n("Change Precision");
	return new KSpreadUndoCellLayout( doc, table, r, title );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setDisplayDirtyFlag();
	if ( _delta == 1 )
	    cell->incPrecision();
	else
	    cell->decPrecision();
	cell->clearDisplayDirtyFlag();
    }
};

void KSpreadTable::setSelectionPrecision( const QPoint &_marker, int _delta )
{
    SetSelectionPrecisionWorker w( _delta );
    workOnCells( _marker, w );
}


struct SetSelectionMoneyFormatWorker : public KSpreadTable::CellWorkerTypeA {
    bool b;
    KSpreadDoc *m_pDoc;
    SetSelectionMoneyFormatWorker( bool _b,KSpreadDoc* _doc ) : b( _b ), m_pDoc(_doc) { }
    QString getUndoTitle() { return i18n("Format Money"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PFormatType )
		 || rw->hasProperty( KSpreadCell::PPrecision )
		 || rw->hasProperty( KSpreadCell::PFactor ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setFormatType( b ? KSpreadCell::Money : KSpreadCell::Number );
	rw->setFactor( 1.0 );
	rw->setPrecision( b ? m_pDoc->locale()->fracDigits() : 0 );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setFormatType( b ? KSpreadCell::Money : KSpreadCell::Number );
	cl->setFactor( 1.0 );
	cl->setPrecision( b ? m_pDoc->locale()->fracDigits() : 0 );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PFactor );
	c->clearNoFallBackProperties( KSpreadCell::PFactor );
	c->clearProperty( KSpreadCell::PPrecision );
	c->clearNoFallBackProperties( KSpreadCell::PPrecision );
	c->clearProperty( KSpreadCell::PFormatType );
	c->clearNoFallBackProperties( KSpreadCell::PFormatType );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int, int ) {
	if ( cellRegion )
	    cell->setDisplayDirtyFlag();
	cell->setFormatType( b ? KSpreadCell::Money : KSpreadCell::Number );
	cell->setFactor( 1.0 );
	cell->setPrecision( b ?  m_pDoc->locale()->fracDigits() : 0 );
	if ( cellRegion )
	    cell->clearDisplayDirtyFlag();
    }
};


void KSpreadTable::setSelectionMoneyFormat( const QPoint &_marker, bool b )
{
    SetSelectionMoneyFormatWorker w( b,doc() );
    workOnCells( _marker, w );
}


struct IncreaseIndentWorker : public KSpreadTable::CellWorkerTypeA {
    int tmpIndent, valIndent;
    IncreaseIndentWorker( int _tmpIndent, int _valIndent ) : tmpIndent( _tmpIndent ), valIndent( _valIndent ) { }
    QString getUndoTitle() { return i18n("Increase Indent"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PIndent ) );
    }
    void doWork( RowLayout* rw ) {
	rw->setIndent( tmpIndent+valIndent );
	rw->setAlign( KSpreadCell::Left );
    }
    void doWork( ColumnLayout* cl ) {
	cl->setIndent( tmpIndent+valIndent );
	cl->setAlign( KSpreadCell::Left );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PIndent );
	c->clearNoFallBackProperties( KSpreadCell::PIndent );
	c->clearProperty( KSpreadCell::PAlign );
	c->clearNoFallBackProperties( KSpreadCell::PAlign );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int x, int y ) {
	if ( cellRegion ) {
	    if(cell->align(x,y)!=KSpreadCell::Left)
	    {
		cell->setAlign(KSpreadCell::Left);
		cell->setIndent(0);
	    }
	    cell->setDisplayDirtyFlag();
	    cell->setIndent( /* ### ??? --> */ cell->getIndent(x,y) /* <-- */ +valIndent );
	    cell->clearDisplayDirtyFlag();
	} else {
	    cell->setIndent( tmpIndent+valIndent);
	    cell->setAlign( KSpreadCell::Left);
	}
    }
};


void KSpreadTable::increaseIndent( const QPoint &_marker )
{
    int valIndent = doc()->getIndentValue();
    KSpreadCell* c = cellAt( _marker.x(), _marker.y() );
    int tmpIndent = c->getIndent( _marker.x(), _marker.y() );

    IncreaseIndentWorker w( tmpIndent, valIndent );
    workOnCells( _marker, w );
}


struct DecreaseIndentWorker : public KSpreadTable::CellWorkerTypeA {
    int tmpIndent, valIndent;
    DecreaseIndentWorker( int _tmpIndent, int _valIndent ) : tmpIndent( _tmpIndent ), valIndent( _valIndent ) { }
    QString getUndoTitle() { return i18n("Decrease Indent"); }
    bool testCondition( RowLayout* rw ) {
	return ( rw->hasProperty( KSpreadCell::PIndent ) );
    }
    void doWork( RowLayout* rw ) {
        rw->setIndent( QMAX(0, tmpIndent - valIndent) );
    }
    void doWork( ColumnLayout* cl ) {
        cl->setIndent( QMAX(0, tmpIndent - valIndent) );
    }
    void prepareCell( KSpreadCell* c ) {
	c->clearProperty( KSpreadCell::PIndent );
	c->clearNoFallBackProperties( KSpreadCell::PIndent );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscuringForced() );
    }
    void doWork( KSpreadCell* cell, bool cellRegion, int x, int y ) {
	if ( cellRegion ) {
	    cell->setDisplayDirtyFlag();
	    cell->setIndent( QMAX(0, cell->getIndent(x,y) - valIndent) );
	    cell->clearDisplayDirtyFlag();
	} else {
	    cell->setIndent( QMAX(0, tmpIndent - valIndent) );
	}
    }
};


void KSpreadTable::decreaseIndent( const QPoint &_marker )
{
    int valIndent = doc()->getIndentValue();
    KSpreadCell* c = cellAt( _marker.x(), _marker.y() );
    int tmpIndent = c->getIndent( _marker.x(), _marker.y() );

    DecreaseIndentWorker w( tmpIndent, valIndent );
    workOnCells( _marker, w );
}


int KSpreadTable::adjustColumn( const QPoint& /*_marker*/, int _col )
{
  int long_max = 0;
  if ( _col == -1 )
  {
    if ( isColumnSelected() )
    {
      for ( int col = m_rctSelection.left(); col <= m_rctSelection.right(); ++col )
      {
        KSpreadCell * c = getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured() )
          {
            c->conditionAlign( painter(), col, c->row() );
            if ( c->textWidth() > long_max )
            {
              int indent = 0;
              int a = c->align( c->column(), c->row() );
              if ( a == KSpreadCell::Undefined )
              {
                if ( c->isNumeric() || c->isDate() || c->isTime())
                  a = KSpreadCell::Right;
                else
                  a = KSpreadCell::Left;
              }

              if ( a == KSpreadCell::Left )
                indent = c->getIndent( c->column(), c->row() );
              long_max = indent + c->textWidth()
                + c->leftBorderWidth( c->column(), c->row() )
                + c->rightBorderWidth( c->column(), c->row() );
            }
          } // if !isEmpty...
          c = getNextCellDown( col, c->row() );
        }
      }
    }
  }
  else
  {
    QRect r( m_rctSelection );

    if ( isColumnSelected() )
    {
      for ( int col = m_rctSelection.left(); col <= m_rctSelection.right(); ++col )
      {
        KSpreadCell * c = getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured())
          {
            c->conditionAlign( painter(), col, c->row() );
            if ( c->textWidth() > long_max )
            {
              int indent = 0;
              int a = c->align( c->column(), c->row() );
              if ( a == KSpreadCell::Undefined )
              {
                if ( c->isNumeric() || c->isDate() || c->isTime())
                  a = KSpreadCell::Right;
                else
                  a = KSpreadCell::Left;
              }

              if ( a == KSpreadCell::Left )
                indent = c->getIndent( c->column(), c->row() );
              long_max = indent + c->textWidth()
                + c->leftBorderWidth( c->column(), c->row() )
                + c->rightBorderWidth( c->column(), c->row() );
            }
          }
          c = getNextCellDown( col, c->row() );
        } // end while
      }
    }
    else
    {
      int x = _col;
      KSpreadCell * cell;
      for ( int y = r.top(); y <= r.bottom(); ++y )
      {
        cell = cellAt( x, y );
        if ( cell != m_pDefaultCell && !cell->isEmpty()
             && !cell->isObscured() )
        {
          cell->conditionAlign( painter(), x, y );
          if ( cell->textWidth() > long_max )
          {
            int indent = 0;

            int a = cell->align(x, y);
            if ( a == KSpreadCell::Undefined )
            {
              if ( cell->isNumeric() || cell->isDate() || cell->isTime())
                a = KSpreadCell::Right;
              else
                a = KSpreadCell::Left;
            }

            if ( a == KSpreadCell::Left )
              indent=cell->getIndent( x, y );

            long_max = indent + cell->textWidth()
              + cell->leftBorderWidth( cell->column(), cell->row() )
              + cell->rightBorderWidth( cell->column(), cell->row() );
          }
        }
      } // for top...bottom
    } // not column selected
  }

  //add 4 because long_max is the long of the text
  //but column has borders
  if( long_max == 0 )
    return -1;
  else
    return ( long_max + 4 );
}

int KSpreadTable::adjustRow( const QPoint &/*_marker*/, int _row )
{
  int long_max = 0;
  if( _row == -1 ) //No special row is defined, so use selected rows
  {
    if ( isRowSelected() )
    {
      for ( int row = m_rctSelection.top(); row <= m_rctSelection.bottom(); ++row )
      {
        KSpreadCell * c = getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured() )
          {
            c->conditionAlign( painter(), c->column(), row );
            if( c->textHeight() > long_max )
              long_max = c->textHeight()
                + c->topBorderWidth( c->column(), c->row() )
                + c->bottomBorderWidth( c->column(), c->row() );
          }
          c = getNextCellDown( c->column(), row );
        }
      }
    }
  }
  else
  {
    QRect r( m_rctSelection );

    if ( isRowSelected() )
    {
      for ( int row = m_rctSelection.top(); row <= m_rctSelection.bottom(); ++row )
      {
        KSpreadCell * c = getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured() )
          {
            c->conditionAlign( painter(), c->column(), row );
            if ( c->textHeight() > long_max )
              long_max = c->textHeight()
                + c->topBorderWidth( c->column(), c->row() )
                + c->bottomBorderWidth( c->column(), c->row() );
          }
          c = getNextCellRight( c->column(), row );
        }
      }
    }
    else // no row selected
    {
      int y = _row;
      KSpreadCell * cell;
      for ( int x = r.left(); x <= r.right(); ++x )
      {
        cell = cellAt( x, y );
        if ( cell != m_pDefaultCell && !cell->isEmpty()
            && !cell->isObscured() )
        {
          cell->conditionAlign( painter(), x, y );
          if ( cell->textHeight() > long_max )
            long_max = cell->textHeight()
              + cell->topBorderWidth( cell->column(), cell->row() )
              + cell->bottomBorderWidth( cell->column(), cell->row() );
        }
      }
    }
  }
  //add 4 because long_max is the long of the text
  //but row has borders
  if( long_max == 0 )
    return -1;
  else
    return ( long_max + 4 );
}

struct ClearTextSelectionWorker : public KSpreadTable::CellWorker {
    ClearTextSelectionWorker( ) : KSpreadTable::CellWorker( ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
	return new KSpreadUndoChangeAreaTextCell( doc, table, r );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscured() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->setCellText( "" );
    }
};


bool KSpreadTable::isRowSelected ()const{
//selection() contains a QRect with the current selection
    if ( isRowSelected( selection() ) ){
	return TRUE;
    }
    return FALSE;
}


bool KSpreadTable::isRowSelected (const QRect &_rect)const{
//If a row is selected, then it must have a selection area from left 1 to right KS_colMax
    if ( (_rect.left() == 1) && (_rect.right() == KS_colMax) ){
	return TRUE;
    }
    return FALSE;
}


bool KSpreadTable::isColumnSelected ()const{
//selection() contains a QRect with the current selection
    if ( isColumnSelected( selection() ) ){
	return TRUE;
    }
    return FALSE;
}


bool KSpreadTable::isColumnSelected (const QRect &_rect)const{
//If a column is selected, then it must have a selection area from top 1 to bottom KS_rowMax
    if ( (_rect.top() == 1) && (_rect.bottom() == KS_rowMax) ){
	return TRUE;
    }
    return FALSE;
}


void KSpreadTable::clearTextSelection( const QPoint &_marker )
{
    if(areaIsEmpty())
        return;
    ClearTextSelectionWorker w;
    workOnCells( _marker, w );
}


struct ClearValiditySelectionWorker : public KSpreadTable::CellWorker {
    ClearValiditySelectionWorker( ) : KSpreadTable::CellWorker( ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
	return new KSpreadUndoConditional( doc, table, r );
    }
    bool testCondition( KSpreadCell* cell ) {
	return ( !cell->isObscured() );
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->removeValidity();
    }
};

void KSpreadTable::clearValiditySelection( const QPoint &_marker )
{
    if(areaIsEmpty())
        return;
    ClearValiditySelectionWorker w;
    workOnCells( _marker, w );
}


struct ClearConditionalSelectionWorker : public KSpreadTable::CellWorker
{
  ClearConditionalSelectionWorker( ) : KSpreadTable::CellWorker( ) { }

  class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc,
					     KSpreadTable* table,
					     QRect& r )
  {
    return new KSpreadUndoConditional( doc, table, r );
  }
  bool testCondition( KSpreadCell* cell )
  {
    return ( !cell->isObscured() );
  }
  void doWork( KSpreadCell* cell, bool, int, int )
  {
    QValueList<KSpreadConditional> emptyList;
    cell->SetConditionList(emptyList);
  }
};

void KSpreadTable::clearConditionalSelection( const QPoint &_marker )
{
    if(areaIsEmpty())
        return;
    ClearConditionalSelectionWorker w;
    workOnCells( _marker, w );
}


struct DefaultSelectionWorker : public KSpreadTable::CellWorker {
    DefaultSelectionWorker( ) : KSpreadTable::CellWorker( true, false, true ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
        QString title=i18n("Default Parameters");
	return new KSpreadUndoCellLayout( doc, table, r, title );
    }
    bool testCondition( KSpreadCell* ) {
	return true;
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	cell->defaultStyle();
    }
};

void KSpreadTable::defaultSelection( const QPoint &_marker )
{
    DefaultSelectionWorker w;
    SelectionType st = workOnCells( _marker, w );
    switch ( st ) {
    case CompleteRows:
	RowLayout *rw;
	for ( int i=m_rctSelection.top(); i<=m_rctSelection.bottom(); i++ ) {
	    rw = nonDefaultRowLayout( i );
	    rw->defaultStyleLayout();
	}
	emit sig_updateView( this, m_rctSelection );
	return;
    case CompleteColumns:
	ColumnLayout *cl;
	for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ ) {
	    cl=nonDefaultColumnLayout( i );
	    cl->defaultStyleLayout();
	}
	emit sig_updateView( this, m_rctSelection );
	return;
    case CellRegion:
	QRect r( m_rctSelection );
	emit sig_updateView( this, r );
	return;
    }
}


struct SetConditionalWorker : public KSpreadTable::CellWorker
{
  QValueList<KSpreadConditional> conditionList;
  SetConditionalWorker( QValueList<KSpreadConditional> _tmp ) :
    KSpreadTable::CellWorker( ), conditionList( _tmp ) { }

  class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc,
					     KSpreadTable* table, QRect& r )
  {
    return new KSpreadUndoConditional( doc, table, r );
  }

  bool testCondition( KSpreadCell* )
  {
    return true;
  }

  void doWork( KSpreadCell* cell, bool, int, int )
  {
    if ( !cell->isObscured() ) // TODO: isObscuringForced()???
    {
      cell->SetConditionList(conditionList);
      cell->setDisplayDirtyFlag();
    }
  }
};

void KSpreadTable::setConditional( const QRect & _marker,
                                   QValueList<KSpreadConditional> const & newConditions)
{
  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoConditional * undo = new KSpreadUndoConditional( m_pDoc, this, _marker );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  int l = _marker.left();
  int r = _marker.right();
  int t = _marker.top();
  int b = _marker.bottom();

  KSpreadCell * cell;

  for (int x = l; x <= r; ++x)
  {
    for (int y = t; y <= b; ++y)
    {
      cell = cellAt( x, y );
      if ( cell->isDefault() )
      {
        cell = new KSpreadCell( this, x, y );
        insertCell( cell );
      }

      if ( cell->isObscuringForced() )
        continue;

      cell->SetConditionList(newConditions);
      cell->setDisplayDirtyFlag();
    }
  }

  emit sig_updateView( this, _marker );
}


struct SetValidityWorker : public KSpreadTable::CellWorker {
    KSpreadValidity tmp;
    SetValidityWorker( KSpreadValidity _tmp ) : KSpreadTable::CellWorker( ), tmp( _tmp ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
	return new KSpreadUndoConditional( doc, table, r );
    }
    bool testCondition( KSpreadCell* ) {
        return true;
    }
    void doWork( KSpreadCell* cell, bool, int, int ) {
	if ( !cell->isObscured() ) {
	    cell->setDisplayDirtyFlag();
	    if ( tmp.m_allow==Allow_All )
		cell->removeValidity();
	    else
	    {
		KSpreadValidity *tmpValidity = cell->getValidity();
		tmpValidity->message=tmp.message;
		tmpValidity->title=tmp.title;
		tmpValidity->valMin=tmp.valMin;
		tmpValidity->valMax=tmp.valMax;
		tmpValidity->m_cond=tmp.m_cond;
		tmpValidity->m_action=tmp.m_action;
		tmpValidity->m_allow=tmp.m_allow;
		tmpValidity->timeMin=tmp.timeMin;
		tmpValidity->timeMax=tmp.timeMax;
		tmpValidity->dateMin=tmp.dateMin;
		tmpValidity->dateMax=tmp.dateMax;
	    }
	    cell->clearDisplayDirtyFlag();
	}
    }
};

void KSpreadTable::setValidity(const QPoint &_marker,KSpreadValidity tmp )
{
    SetValidityWorker w( tmp );
    workOnCells( _marker, w );
}


struct GetWordSpellingWorker : public KSpreadTable::CellWorker {
    QString& listWord;
    GetWordSpellingWorker( QString& _listWord ) : KSpreadTable::CellWorker( false, false, true ), listWord( _listWord ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc*, KSpreadTable*, QRect& ) {
	return 0L;
    }
    bool testCondition( KSpreadCell* ) {
        return true;
    }
    void doWork( KSpreadCell* c, bool cellRegion, int, int ) {
	if ( !c->isObscured() || cellRegion /* ### ??? */ ) {
	    if ( !c->isFormula() && !c->isNumeric() && !c->valueString().isEmpty() && !c->isTime()
		 && !c->isDate() && c->content() != KSpreadCell::VisualFormula
		 && !c->text().isEmpty())
	    {
		listWord+=c->text()+'\n';
	    }
	}
    }
};

QString KSpreadTable::getWordSpelling(const QPoint &_marker )
{
    QString listWord;
    GetWordSpellingWorker w( listWord );
    workOnCells( _marker, w );
    return listWord;
}


struct SetWordSpellingWorker : public KSpreadTable::CellWorker {
    QStringList& list;
    int pos;
    SetWordSpellingWorker( QStringList& _list ) : KSpreadTable::CellWorker( false, false, true ), list( _list ), pos( 0 ) { }

    class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) {
	return new KSpreadUndoChangeAreaTextCell( doc, table, r );
    }
    bool testCondition( KSpreadCell* ) {
        return true;
    }
    void doWork( KSpreadCell* c, bool cellRegion, int, int ) {
	if ( !c->isObscured() || cellRegion /* ### ??? */ ) {
	    if ( !c->isFormula() && !c->isNumeric() && !c->valueString().isEmpty() && !c->isTime()
		 && !c->isDate() && c->content() != KSpreadCell::VisualFormula
		 && !c->text().isEmpty())
	    {
		c->setCellText( list[pos] );
		pos++;
	    }
	}
    }
};

void KSpreadTable::setWordSpelling(const QPoint &_marker, const QString _listWord )
{
    QStringList list = QStringList::split ( '\n', _listWord );
    SetWordSpellingWorker w( list );
    workOnCells( _marker, w );
}


QString KSpreadTable::copyAsText( const QPoint &_marker )
{
    // Only one cell selected? => copy active cell
    if ( singleCellSelection() )
    {
        KSpreadCell * cell = cellAt( _marker.x(), _marker.y() );
        if( !cell->isDefault() )
          return cell->strOutText();
        return "";
    }

    int x;
    int y;
    unsigned int max = 1;
    QString result;
    KSpreadCell *cell;
    for (y = m_rctSelection.top(); y <= m_rctSelection.bottom(); ++y)
    {
      for (x = m_rctSelection.left(); x <= m_rctSelection.right(); ++x)
      {
        cell = cellAt( x, y );
        if( !cell->isDefault() )
        {
          if ( cell->strOutText().length() > max )
            max = cell->strOutText().length();
        }
      }
    }

    ++max;

    for (y = m_rctSelection.top(); y <= m_rctSelection.bottom(); ++y)
    {
      for (x = m_rctSelection.left(); x <= m_rctSelection.right(); ++x)
      {
        cell = cellAt( x, y );
        if( !cell->isDefault() )
        {
            int l = max - cell->strOutText().length();
            if (cell->align(x, y) == KSpreadLayout::Right
                || cell->defineAlignX() == KSpreadLayout::Right )
            {
              for ( int i = 0; i < l; ++i )
                result += " ";
              result += cell->strOutText();
            }
            else if (cell->align(x, y) == KSpreadLayout::Left
                     || cell->defineAlignX() == KSpreadLayout::Left )
            {
              result += " ";
              result += cell->strOutText();
              // start with "1" because we already set one space
              for ( int i = 1; i < l; ++i )
                result += " ";
            }
            else // centered
            {
              int i;
              int s = (int) l / 2;
              for ( i = 0; i < s; ++i )
                result += " ";
              result += cell->strOutText();
              for ( i = s; i < l; ++i )
                result += " ";
            }
        }
        else
        {
            for ( unsigned int i = 0; i < max; ++i )
              result += " ";
        }
      }
      result += "\n";
    }

    return result;
}

void KSpreadTable::copySelection( const QPoint &_marker )
{
    QRect rct;

    rct = selection();

    QDomDocument doc = saveCellRect( rct );

    // Save to buffer
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    QTextStream str( &buffer );
    str.setEncoding( QTextStream::UnicodeUTF8 );
    str << doc;
    buffer.close();

    KSpreadTextDrag * kd = new KSpreadTextDrag( 0L );
    kd->setPlain( copyAsText(_marker) );
    kd->setKSpread( buffer.buffer() );

    QApplication::clipboard()->setData( kd );
}

void KSpreadTable::cutSelection( const QPoint &_marker )
{
    copySelection( _marker );
    deleteSelection( _marker );
}

void KSpreadTable::paste( const QPoint &_marker,bool makeUndo, PasteMode sp, Operation op,bool insert,int insertTo )
{
    QMimeSource* mime = QApplication::clipboard()->data();
    if ( !mime )
        return;

    QByteArray b;

    if ( mime->provides( KSpreadTextDrag::selectionMimeType() ) )
    {
        b = mime->encodedData( KSpreadTextDrag::selectionMimeType() );
    }
    else if( mime->provides( "text/plain" ) )
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype). Copied from KPresenter ;-)
	QString _text = QApplication::clipboard()->text();
	pasteTextPlain( _text, _marker);
	return;
    }
    else
        return;

    paste( b, _marker, makeUndo, sp, op, insert, insertTo );
}

void KSpreadTable::pasteTextPlain( QString &_text, const QPoint &_marker)
{
//  QString tmp;
//  tmp= QString::fromLocal8Bit(_mime->encodedData( "text/plain" ));
  if( _text.isEmpty() )
    return;

  QString tmp = _text;
  int i;
  int mx   = _marker.x();
  int my   = _marker.y();
  int rows = 1;
  int len  = tmp.length();

  //count the numbers of lines in text
  for ( i = 0; i < len; ++i )
  {
    if ( tmp[i] == '\n' )
      ++rows;
  }

  KSpreadCell * cell = nonDefaultCell( mx, my );
  if ( rows == 1 )
  {
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoSetText * undo = new KSpreadUndoSetText( m_pDoc, this , cell->text(), mx, my, cell->formatType() );
      m_pDoc->undoBuffer()->appendUndo( undo );
    }
  }
  else
  {
      QRect rect(mx, my, mx, my + rows - 1);
      KSpreadUndoChangeAreaTextCell * undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, this , rect );
      m_pDoc->undoBuffer()->appendUndo( undo );
  }

  i = 0;
  QString rowtext;

  while ( i < rows )
  {
    int p = 0;

    p = tmp.find('\n');

    if (p < 0)
      p = tmp.length();

    rowtext = tmp.left(p);

    cell->setCellText( rowtext );
    cell->updateChart();

    // next cell
    ++i;
    cell = nonDefaultCell( mx, my + i );

    if (!cell || p == (int) tmp.length())
      break;

    // exclude the left part and '\n'
    tmp = tmp.right(tmp.length() - p - 1);
  }

  if(!isLoading())
    refreshMergedCell();

  emit sig_updateView( this );
  emit sig_updateHBorder( this );
  emit sig_updateVBorder( this );
}

void KSpreadTable::paste( const QByteArray& b, const QPoint &_marker, bool makeUndo, PasteMode sp, Operation op, bool insert, int insertTo )
{
    kdDebug(36001) << "Parsing " << b.size() << " bytes" << endl;

    QBuffer buffer( b );
    buffer.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &buffer );
    buffer.close();

    // ##### TODO: Test for parsing errors

    int mx = _marker.x();
    int my = _marker.y();

    mx = QMIN(selection().x(), mx);
    my = QMIN(selection().y(), my);

    loadSelection( doc, mx - 1, my - 1, makeUndo, sp, op, insert, insertTo );
}

bool KSpreadTable::loadSelection( const QDomDocument& doc, int _xshift, int _yshift, bool makeUndo, PasteMode sp, Operation op, bool insert, int insertTo )
{
    QDomElement e = doc.documentElement();

    if (!isLoading() && makeUndo)
        loadSelectionUndo( doc, _xshift, _yshift, insert, insertTo );

    int rowsInClpbrd    =  e.attribute( "rows" ).toInt();
    int columnsInClpbrd =  e.attribute( "columns" ).toInt();

    // find size of rectangle that we want to paste to (either clipboard size or current selection)
    const int pasteWidth = ( selection().width() >= columnsInClpbrd
                             && isRowSelected() == FALSE
                             && e.namedItem( "rows" ).toElement().isNull() )
      ? selection().width() : columnsInClpbrd;
    const int pasteHeight = ( selection().height() >= rowsInClpbrd
                              && isColumnSelected() == FALSE
                              && e.namedItem( "columns" ).toElement().isNull())
      ? selection().height() : rowsInClpbrd;

    /*    kdDebug() << "loadSelection: paste area has size " << pasteHeight << " rows * "
          << pasteWidth << " columns " << endl;
          kdDebug() << "loadSelection: " << rowsInClpbrd << " rows and "
          << columnsInClpbrd << " columns in clipboard." << endl;
          kdDebug() << "xshift: " << _xshift << " _yshift: " << _yshift << endl;
    */

    if ( !e.namedItem( "columns" ).toElement().isNull() )
    {
        _yshift = 0;

        // Clear the existing columns
        for( int i = 1; i <= pasteWidth; ++i )
        {
            if(!insert)
            {
                m_cells.clearColumn( _xshift + i );
                m_columns.removeElement( _xshift + i );
            }
        }

        // Insert column layouts
        QDomElement c = e.firstChild().toElement();
        for( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "column" )
            {
                ColumnLayout *cl = new ColumnLayout( this, 0 );
                if ( cl->load( c, _xshift,sp ) )
                    insertColumnLayout( cl );
                else
                    delete cl;
            }
        }

    }

    if ( !e.namedItem( "rows" ).toElement().isNull() )
    {
        _xshift = 0;

        // Clear the existing rows
        for( int i = 1; i <= pasteHeight; ++i )
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
                if ( cl->load( c, _yshift,sp ) )
                    insertRowLayout( cl );
                else
                    delete cl;
            }
        }
    }

    KSpreadCell* refreshCell = 0;
    KSpreadCell *cell;
    KSpreadCell *cellBackup = NULL;
    QDomElement c = e.firstChild().toElement();
    for( ; !c.isNull(); c = c.nextSibling().toElement() )
    {
      if ( c.tagName() == "cell" )
      {
        int row = c.attribute( "row" ).toInt() + _yshift;
        int col = c.attribute( "column" ).toInt() + _xshift;
        // tile the selection with the clipboard contents

        for (int roff = 0; row + roff - _yshift <= pasteHeight; roff += rowsInClpbrd)
        {
          for (int coff = 0; col + coff - _xshift <= pasteWidth; coff += columnsInClpbrd)
          {
            //kdDebug() << "loadSelection: cell at " << (col+coff) << "," << (row+roff) << " with roff,coff= "
            //          << roff << "," << coff << ", _xshift: " << _xshift << ", _yshift: " << _yshift << endl;

            cell = nonDefaultCell( col + coff, row + roff );

            cellBackup = new KSpreadCell(this, cell->column(), cell->row());
            cellBackup->copyAll(cell);

            if ( !cell->load( c, _xshift + coff, _yshift + roff, sp, op ) )
            {
              cell->copyAll(cellBackup);
            }

            delete cellBackup;



            cell = cellAt( col + coff, row + roff );
            if( !refreshCell && cell->updateChart( false ) )
            {
              refreshCell = cell;
            }
          }
        }
      }
    }
    //refresh chart after that you paste all cells

    /* John: I don't think this is gonna work....doesn't this only update
       one chart -- the one which had a dependant cell update first?

       I don't have time to check on this now....
    */
    if ( refreshCell )
        refreshCell->updateChart();
    m_pDoc->setModified( true );

    if(!isLoading())
        refreshMergedCell();

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );

    return true;
}

void KSpreadTable::loadSelectionUndo( const QDomDocument & doc,int _xshift, int _yshift,bool insert,int insertTo)
{
    QDomElement e = doc.documentElement();
    QDomElement c = e.firstChild().toElement();
    int rowsInClpbrd =  e.attribute( "rows" ).toInt();
    int columnsInClpbrd =  e.attribute( "columns" ).toInt();
    // find rect that we paste to
    const int pasteWidth = ( selection().width() >= columnsInClpbrd &&
                             isRowSelected() == FALSE &&
                             e.namedItem( "rows" ).toElement().isNull() )
        ? selection().width() : columnsInClpbrd;
    const int pasteHeight = ( selection().height() >= rowsInClpbrd &&
                              isColumnSelected() == FALSE &&
                              e.namedItem( "columns" ).toElement().isNull() )
        ? selection().height() : rowsInClpbrd;
    QRect rect;
    if ( !e.namedItem( "columns" ).toElement().isNull() )
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoCellPaste *undo = new KSpreadUndoCellPaste( m_pDoc, this, pasteWidth, 0, _xshift,_yshift,rect,insert );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
        if(insert)
                 insertColumn(  _xshift+1,pasteWidth-1,false);
	return;
    }

    if ( !e.namedItem( "rows" ).toElement().isNull() )
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoCellPaste *undo = new KSpreadUndoCellPaste( m_pDoc, this, 0,pasteHeight, _xshift,_yshift,rect,insert );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
	if(insert)
	    insertRow(  _yshift+1,pasteHeight-1,false);
	return;
    }

    rect.setRect( _xshift+1, _yshift+1, pasteWidth, pasteHeight );

    if(!c.isNull())
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
                KSpreadUndoCellPaste *undo = new KSpreadUndoCellPaste( m_pDoc, this, 0,0,_xshift,_yshift,rect,insert,insertTo );
                m_pDoc->undoBuffer()->appendUndo( undo );
        }
    if(insert)
        {
        if(insertTo==-1)
                shiftRow(rect,false);
        else if(insertTo==1)
                shiftColumn(rect,false);
        }
    }
}

bool KSpreadTable::testAreaPasteInsert()const
{
   QMimeSource* mime = QApplication::clipboard()->data();
    if ( !mime )
        return false;

    QByteArray b;

    if ( mime->provides( "application/x-kspread-snippet" ) )
        b = mime->encodedData( "application/x-kspread-snippet" );
    else
        return false;

    QBuffer buffer( b );
    buffer.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &buffer );
    buffer.close();

    QDomElement e = doc.documentElement();
    if ( !e.namedItem( "columns" ).toElement().isNull() )
        return false;

    if ( !e.namedItem( "rows" ).toElement().isNull() )
        return false;

    QDomElement c = e.firstChild().toElement();
    for( ; !c.isNull(); c = c.nextSibling().toElement() )
    {
        if ( c.tagName() == "cell" )
                return true;
    }
    return false;
}

void KSpreadTable::deleteCells( const QRect& rect )
{
    // A list of all cells we want to delete.
    QPtrStack<KSpreadCell> cellStack;

    QRect tmpRect;
    bool extraCell = false;
    if (rect.width() == 1 && rect.height() == 1 )
    {
      KSpreadCell * cell = nonDefaultCell( rect.x(), rect.y() );
      if (cell->isForceExtraCells())
      {
        extraCell = true;
        tmpRect = rect;
      }
    }

    int right  = rect.right();
    int left   = rect.left();
    int bottom = rect.bottom();
    int col;
    for ( int row = rect.top(); row <= bottom; ++row )
    {
      KSpreadCell * c = getFirstCellRow( row );
      while ( c )
      {
        col = c->column();
        if ( col < left )
        {
          c = getNextCellRight( left - 1, row );
          continue;
        }
        if ( col > right )
          break;

        if ( !c->isDefault() )
          cellStack.push( c );

        c = getNextCellRight( col, row );
      }
    }

    m_cells.setAutoDelete( false );

    // Remove the cells from the table
    while ( !cellStack.isEmpty() )
    {
      KSpreadCell * cell = cellStack.pop();

      m_cells.remove( cell->column(), cell->row() );
      cell->updateDepending();

      delete cell;
    }

    m_cells.setAutoDelete( true );

    setLayoutDirtyFlag();

    // TODO: don't go through all cells here!
    // Since obscured cells might have been deleted we
    // have to reenforce it.
    KSpreadCell * c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
      if ( c->isForceExtraCells() && !c->isDefault() )
        c->forceExtraCells( c->column(), c->row(),
                            c->extraXCells(), c->extraYCells() );
    }

    if (extraCell)
    {
      setSelection(tmpRect);
      unselect();
    }

    m_pDoc->setModified( true );
}

void KSpreadTable::deleteSelection( const QPoint& /*_marker*/ )
{
    QRect r( m_rctSelection );

    if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            KSpreadUndoDelete *undo = new KSpreadUndoDelete( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }

    if ( !m_pDoc->undoBuffer()->isLocked() )
        {
            KSpreadUndoDelete *undo = new KSpreadUndoDelete( m_pDoc, this, r );
            m_pDoc->undoBuffer()->appendUndo( undo );
        }

    // Entire rows selected ?
    if ( isRowSelected() )
    {
        for( int i = r.top(); i <= r.bottom(); ++i )
        {
            m_cells.clearRow( i );
            m_rows.removeElement( i );
        }

        emit sig_updateVBorder( this );
    }
    // Entire columns selected ?
    else if ( isColumnSelected() )
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

        deleteCells( r );
    }
    refreshMergedCell();
    emit sig_updateView( this );
}

void KSpreadTable::refreshView(const QRect& rect)
{
  // TODO: don't go through all cells when refreshing!
    QRect tmp(rect);
    KSpreadCell * c = m_cells.firstCell();
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
    KSpreadCell *cell = nonDefaultCell( m_iCol, m_iRow );
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

void KSpreadTable::mergeCell( const QPoint &_marker, bool makeUndo)
{
    if( singleCellSelection() )
        return;
    int x=_marker.x();
    int y=_marker.y();
    if( _marker.x() > m_rctSelection.left() )
        x = m_rctSelection.left();
    if( _marker.y() > m_rctSelection.top() )
        y = m_rctSelection.top();
    KSpreadCell *cell = nonDefaultCell( x , y );

    if ( !m_pDoc->undoBuffer()->isLocked() && makeUndo)
    {
        KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( m_pDoc, this, x ,y,cell->extraXCells() ,cell->extraYCells());
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    cell->forceExtraCells( x ,y,
                           abs(m_rctSelection.right() -m_rctSelection.left()),
                           abs(m_rctSelection.bottom() - m_rctSelection.top()));

    setMarker(QPoint(x,y));
    if(getAutoCalc())
        recalc();
    emit sig_updateView( this, m_rctSelection );
}

void KSpreadTable::dissociateCell( const QPoint &_marker,bool makeUndo)
{
    KSpreadCell *cell = nonDefaultCell( _marker.x(), _marker.y() );
    if(!cell->isForceExtraCells())
        return;

    if ( !m_pDoc->undoBuffer()->isLocked() && makeUndo)
    {
        KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( m_pDoc, this, _marker.x() ,_marker.y(),cell->extraXCells() ,cell->extraYCells());
        m_pDoc->undoBuffer()->appendUndo( undo );
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

bool KSpreadTable::testListChoose(const QPoint &_marker)
{
   QRect selection( selection() );

   KSpreadCell *cell = cellAt( _marker.x(), _marker.y() );
   QString tmp=cell->text();

   KSpreadCell* c = firstCell();
   bool different=false;
   int col;
   for( ;c; c = c->nextCell() )
     {
       col = c->column();
       if ( selection.left() <= col && selection.right() >= col
	    &&!c->isObscuringForced()&& !(col==_marker.x()&& c->row()==_marker.y()))
	 {
	   if(!c->isFormula() && !c->isNumeric() && !c->valueString().isEmpty()
	      && !c->isTime() &&!c->isDate()
	      && c->content() != KSpreadCell::VisualFormula)
	     {
                 if(c->text()!=tmp)
                     different=true;
	     }

	 }
     }
   return different;
}




void KSpreadTable::print( QPainter &painter, KPrinter *_printer )
{
    kdDebug(36001)<<"PRINTING ...."<<endl;

    // Override the current grid pen setting, when set to disable
    QPen gridPen;
    bool oldShowGrid = m_bShowGrid;
    m_bShowGrid = m_bPrintGrid;
    if ( !m_bPrintGrid )
    {
	gridPen = m_pDoc->defaultGridPen();
	QPen nopen;
	nopen.setStyle( NoPen );
	m_pDoc->setDefaultGridPen( nopen );
    }

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
    QPtrListIterator<KoDocumentChild> cit( m_pDoc->children() );
    int dummy;
    int i;
    for( ; cit.current(); ++cit )
    {
        QRect bound = cit.current()->boundingRect();
        i = leftColumn( bound.right(), dummy );
        if ( i > cell_range.right() )
            cell_range.setRight( i );
        i = topRow( bound.bottom(), dummy );
        if ( i > cell_range.bottom() )
            cell_range.setBottom( i );
    }

    // Adjust to the print range
    if ( cell_range.top() < m_printRange.top() ) cell_range.setTop( m_printRange.top() );
    if ( cell_range.left() < m_printRange.left() ) cell_range.setLeft( m_printRange.left() );
    if ( cell_range.bottom() > m_printRange.bottom()-1 ) cell_range.setBottom( m_printRange.bottom()-1 );
    if ( cell_range.right() > m_printRange.right()-1 ) cell_range.setRight( m_printRange.right()-1 );

    //If we have repeated columns/rows, children get an offset on pages
    int offsetX = 0;
    int offsetY = 0;
    int currentOffsetX = 0;
    int currentOffsetY = 0;
    //Calculate offsetX for repeated columns
    if ( m_printRepeatColumns.first != 0 )
    {
        //When we need repeated columns, reservate space for them
        for ( int i = m_printRepeatColumns.first; i <= m_printRepeatColumns.second; i++)
            offsetX += columnLayout( i )->width();
    }
    //Calculate offsetY for repeated rows
    if ( m_printRepeatRows.first != 0 )
    {
        //When we need repeated rows, reservate space for them
        for ( int i = m_printRepeatRows.first; i <= m_printRepeatRows.second; i++)
            offsetY += rowLayout( i )->height();
    }

    //
    // Find out how many pages need printing
    // and which cells to print on which page.
    //
    QValueList<QRect> page_list;
    QValueList<QRect> page_frame_list;
    QValueList<QPoint> page_frame_list_offset;

    // How much space is on every page for table content ?
    QRect rect;
    rect.setCoords( 0, 0, (int)( MM_TO_POINT ( printableWidth()  )),
                          (int)( MM_TO_POINT ( printableHeight() )) );

    // Up to this row everything is already handled, starting with the print range
    int bottom = m_printRange.top()-1;
    // Start of the next page
    int top = bottom + 1;
    // Calculate all pages, but if we are embedded, print only the first one
    while ( bottom < cell_range.bottom() /* && page_list.count() == 0 */ )
    {
        kdDebug(36001) << "KSpreadTable::print: bottom=" << bottom << " bottom_range=" << cell_range.bottom() << endl;

        // Up to this column everything is already printed, starting with the print range
        int right = m_printRange.left()-1;
        // Start of the next page
        int left = right + 1;
        while ( right < cell_range.right() )
        {
            kdDebug(36001) << "KSpreadTable::print: right=" << right << " right_range=" << cell_range.right() << endl;

            QRect page_range;
            page_range.setLeft( left );
            page_range.setTop( top );

            int col = left;
            int x = columnLayout( col )->width();

            //Check if we have to repeat some columns
            if ( ( m_printRepeatColumns.first != 0 ) && ( col > m_printRepeatColumns.first ) )
            {
                //When we need repeated columns, reservate space for them
                x += offsetX;
                currentOffsetX = offsetX;
            }

            //Count the columns which still fit on the page
            while ( ( x < rect.width() ) && ( col <= m_printRange.right() ) )
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

            //Check if we have to repeat some rows
            if ( ( m_printRepeatRows.first != 0 ) && ( row > m_printRepeatRows.first ) )
            {
                //When we need repeated rows, reservate space for them
                y += offsetY;
                currentOffsetY = offsetY;
            }

            //Count the rows, which still fit on the page
            while ( ( y < rect.height() ) && ( row <= m_printRange.bottom() ) )
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
            for( int r = page_range.top(); empty && ( r <= page_range.bottom() ); ++r )
                for( int c = page_range.left(); empty && ( c <= page_range.right() ); ++c )
                    if ( cellAt( c, r )->needsPrinting() )
                        empty = FALSE;

            // Look for children
            QRect view = QRect( QPoint( columnPos( page_range.left() ), 
                                        rowPos( page_range.top() ) ),
                                QPoint( columnPos( col-1 ) + columnLayout( col-1 )->width(), 
                                        rowPos( row-1 ) + rowLayout( row-1 )->height() ) );
            QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
            for( ; empty && it.current(); ++it )
            {
                QRect bound = it.current()->boundingRect();
                if ( bound.intersects( view ) )
                    empty = FALSE;
            }

            if ( !empty )
            {
                page_list.append( page_range );
                page_frame_list.append( view );
                page_frame_list_offset.append( QPoint( currentOffsetX, currentOffsetY ) );
            }
        }

        top = bottom + 1;
    }

    kdDebug(36001) << "PRINTING " << page_list.count() << " pages" << endl;

    int pagenr = 1;

    //
    // Print all pages in the list
    //

    QValueList<QRect>::Iterator it = page_list.begin();
    QValueList<QRect>::Iterator fit = page_frame_list.begin();
    QValueList<QPoint>::Iterator fito = page_frame_list_offset.begin();
    int w;
    for( ; it != page_list.end(); ++it, ++fit, ++fito, ++pagenr )
    {
        // print head line
        QFont font( "Times", 10 );
        painter.setFont( font );
        QFontMetrics fm = painter.fontMetrics();
        w = fm.width( headLeft( pagenr, m_strName ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( leftBorder() )),
                              (int)( MM_TO_POINT ( 10.0 )), headLeft( pagenr, m_strName ) );
        w = fm.width( headMid( pagenr, m_strName.latin1() ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( leftBorder()) +
                                     ( MM_TO_POINT ( printableWidth()) - (float)w ) / 2.0 ),
                              (int)( MM_TO_POINT ( 10.0 )), headMid( pagenr, m_strName ) );
        w = fm.width( headRight( pagenr, m_strName ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( leftBorder()) +
                                     MM_TO_POINT ( printableWidth()) - (float)w ),
                              (int)( MM_TO_POINT ( 10.0 )), headRight( pagenr, m_strName) );

        // print foot line
        w = fm.width( footLeft( pagenr, m_strName ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( leftBorder() )),
                              (int)( MM_TO_POINT ( paperHeight() - 10.0 )),
                              footLeft( pagenr, m_strName ) );
        w = fm.width( footMid( pagenr, m_strName ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( leftBorder() )+
                                     ( MM_TO_POINT ( printableWidth()) - (float)w ) / 2.0 ),
                              (int)( MM_TO_POINT  ( paperHeight() - 10.0 ) ),
                              footMid( pagenr, m_strName ) );
        w = fm.width( footRight( pagenr, m_strName ) );
        if ( w > 0 )
            painter.drawText( (int)( MM_TO_POINT ( leftBorder()) +
                                     MM_TO_POINT ( printableWidth()) - (float)w ),
                              (int)( MM_TO_POINT ( paperHeight() - 10.0 ) ),
                              footRight( pagenr, m_strName ) );

        painter.translate( MM_TO_POINT ( leftBorder()),
                           MM_TO_POINT ( topBorder() ));
        // Print the page
        printPage( painter, *it, *fit, *fito );

        painter.translate( - MM_TO_POINT ( leftBorder()),
                           - MM_TO_POINT ( topBorder() ));

        if ( pagenr < (int)page_list.count() )
            _printer->newPage();
    }

    if ( !m_bPrintGrid )
    {
	// Restore the grid pen
	m_pDoc->setDefaultGridPen( gridPen );
    }
    m_bShowGrid = oldShowGrid;
}

void KSpreadTable::printPage( QPainter &_painter, const QRect& page_range, const QRect& view, const QPoint _childOffset )
{
    // kdDebug(36001) << "Rect x=" << page_range->left() << " y=" << page_range->top() << ", w="
    // << page_range->width() << " h="  << page_range->height() << endl;

    //
    // Draw the cells.
    //
    int xpos;
    int xpos_Start;
    int ypos;
    int ypos_Start;
    KSpreadCell *cell;
    RowLayout *row_lay;
    ColumnLayout *col_lay;

    //Check if we have to repeat some rows and columns
    xpos = 0;
    ypos = 0;
    xpos_Start = 0;
    ypos_Start = 0;
    if ( m_printRepeatColumns.first != 0 && page_range.left() > m_printRepeatColumns.first &&
         m_printRepeatRows.first != 0    && page_range.top() > m_printRepeatRows.first )
    {
        for ( int y = m_printRepeatRows.first; y <= m_printRepeatRows.second; y++ )
        {
            row_lay = rowLayout( y );
            xpos = 0;

            for ( int x = m_printRepeatColumns.first; x <= m_printRepeatColumns.second; x++ )
            {
                col_lay = columnLayout( x );

                cell = cellAt( x, y );
                QRect r( 0, 0, view.width(), view.height() );
                cell->paintCell( r, _painter, QPoint(xpos, ypos), QPoint(x,y));

                xpos += col_lay->width();
            }

            ypos += row_lay->height();
        }
        ypos_Start = ypos;
        xpos_Start = xpos;
    }

    //Check if we have to repeat some rows
    xpos = xpos_Start;
    ypos = 0;
    if ( m_printRepeatRows.first != 0 && page_range.top() > m_printRepeatRows.first )
    {
        for ( int y = m_printRepeatRows.first; y <= m_printRepeatRows.second; y++ )
        {
            row_lay = rowLayout( y );
            xpos = xpos_Start;

            for ( int x = page_range.left(); x <= page_range.right(); x++ )
            {
                col_lay = columnLayout( x );

                cell = cellAt( x, y );
                QRect r( 0, 0, view.width() + xpos, view.height() );
                cell->paintCell( r, _painter, QPoint(xpos, ypos), QPoint(x,y));

                xpos += col_lay->width();
            }

            ypos += row_lay->height();
        }
        ypos_Start = ypos;
    }

    //Check if we have to repeat some columns
    xpos = 0;
    ypos = ypos_Start;
    if ( m_printRepeatColumns.first != 0 && page_range.left() > m_printRepeatColumns.first )
    {
        for ( int y = page_range.top(); y <= page_range.bottom(); y++ )
        {
            row_lay = rowLayout( y );
            xpos = 0;

            for ( int x = m_printRepeatColumns.first; x <= m_printRepeatColumns.second; x++ )
            {
                col_lay = columnLayout( x );

                cell = cellAt( x, y );
                QRect r( 0, 0, view.width() + xpos, view.height() + ypos );
                cell->paintCell( r, _painter, QPoint(xpos, ypos), QPoint(x,y));

                xpos += col_lay->width();
            }

            ypos += row_lay->height();
        }
        xpos_Start = xpos;
    }

    //Print the cells
    xpos = xpos_Start;
    ypos = ypos_Start;
    for ( int y = page_range.top(); y <= page_range.bottom(); y++ )
    {
        row_lay = rowLayout( y );
        xpos = xpos_Start;

        for ( int x = page_range.left(); x <= page_range.right(); x++ )
        {
            col_lay = columnLayout( x );

            cell = cellAt( x, y );
            QRect r( 0, 0, view.width() + xpos, view.height() + ypos );
            cell->paintCell( r, _painter, QPoint(xpos, ypos), QPoint(x,y));

            xpos += col_lay->width();
        }

        ypos += row_lay->height();
    }

    //
    // Draw the children
    //
    QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
    QRect contentRectPart;
    for( ; it.current(); ++it )
    {
        QString tmp=QString("Testing child %1/%2 %3/%4 against view %5/%6 %7/%8")
        .arg(it.current()->contentRect().left())
        .arg(it.current()->contentRect().top())
        .arg(it.current()->contentRect().right())
        .arg(it.current()->contentRect().bottom())
        .arg(view.left()).arg(view.top()).arg(view.right()).arg(view.bottom());
        kdDebug(36001)<<tmp<<" offset "<<_childOffset.x()<<"/"<<_childOffset.y()<<endl;

        QRect bound = it.current()->boundingRect();
        if ( ((KSpreadChild*)it.current())->table() == this && bound.intersects( view ) )
        {
            _painter.save();

            _painter.translate( -view.left()+_childOffset.x(), -view.top()+_childOffset.y() );

            //print only the visible part of the children
            contentRectPart = bound.intersect( view );
            contentRectPart.moveBy( -bound.x(), -bound.y() );
            
            it.current()->transform( _painter );
            it.current()->document()->paintEverything( _painter,
                                                       contentRectPart,
                                                       it.current()->isTransparent() );
            _painter.restore();
        }
    }
}

QDomDocument KSpreadTable::saveCellRect( const QRect &_rect )
{
    QDomDocument doc( "spreadsheet-snippet" );
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement spread = doc.createElement( "spreadsheet-snippet" );
    spread.setAttribute( "rows", _rect.bottom() - _rect.top() + 1 );
    spread.setAttribute( "columns", _rect.right() - _rect.left() + 1 );
    doc.appendChild( spread );

    //
    // Entire rows selected ?
    //
    if ( isRowSelected( _rect ) )
    {
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
        RowLayout* lay;
        for( int y = _rect.top(); y <= _rect.bottom(); ++y )
        {
            lay = rowLayout( y );
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
    if ( isColumnSelected( _rect ) )
    {
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
        ColumnLayout* lay;
        for( int x = _rect.left(); x <= _rect.right(); ++x )
        {
            lay = columnLayout( x );
            if ( lay && !lay->isDefault() )
            {
                QDomElement e = lay->save( doc, _rect.left() - 1 );
                if ( !e.isNull() )
                    spread.appendChild( e );
            }
        }

        return doc;
    }

    // Save all cells.
    /*KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() && !c->isObscuringForced())
        {
            QPoint p( c->column(), c->row() );
            if ( _rect.contains( p ) )
                spread.appendChild( c->save( doc, _rect.left() - 1, _rect.top() - 1  ,true));
        }
    } */
    //store all cell
    //when they don't exist we created them
    //because it's necessary when there is a  layout on a column/row
    //but I remove cell which is inserted.
    KSpreadCell *cell;
    bool insert;
    for (int i=_rect.left();i<=_rect.right();i++)
	for(int j=_rect.top();j<=_rect.bottom();j++)
	{
	    insert = false;
	    cell = cellAt( i, j );
	    if ( cell == m_pDefaultCell )
	    {
		cell = new KSpreadCell( this, i, j );
		insertCell( cell );
		insert=true;
	    }
	    spread.appendChild( cell->save( doc, _rect.left() - 1, _rect.top() - 1  ,true));
	    if( insert )
	        m_cells.remove(i,j);
	}

    return doc;
}

QDomElement KSpreadTable::saveXML( QDomDocument& doc )
{
    QDomElement table = doc.createElement( "table" );
    table.setAttribute( "name", m_strName );
    table.setAttribute( "grid", (int)m_bShowGrid);
    table.setAttribute( "printGrid", (int)m_bPrintGrid);
    table.setAttribute( "hide", (int)m_bTableHide);
    table.setAttribute( "formular", (int)m_bShowFormula);
    table.setAttribute( "borders", (int)m_bShowPageBorders);
    table.setAttribute( "lcmode", (int)m_bLcMode);
    table.setAttribute( "columnnumber", (int)m_bShowColumnNumber);
    table.setAttribute( "hidezero", (int)m_bHideZero);
    table.setAttribute( "firstletterupper", (int)m_bFirstLetterUpper);
    table.setAttribute( "borders1.2", 1);

    // paper parameters
    QDomElement paper = doc.createElement( "paper" );
    paper.setAttribute( "format", paperFormatString() );
    paper.setAttribute( "orientation", orientationString() );
    table.appendChild( paper );

    QDomElement borders = doc.createElement( "borders" );
    borders.setAttribute( "left", leftBorder() );
    borders.setAttribute( "top", topBorder() );
    borders.setAttribute( "right", rightBorder() );
    borders.setAttribute( "bottom", bottomBorder() );
    paper.appendChild( borders );

    QDomElement head = doc.createElement( "head" );
    paper.appendChild( head );
    if ( !headLeft().isEmpty() )
    {
      QDomElement left = doc.createElement( "left" );
      head.appendChild( left );
      left.appendChild( doc.createTextNode( headLeft() ) );
    }
    if ( !headMid().isEmpty() )
    {
      QDomElement center = doc.createElement( "center" );
      head.appendChild( center );
      center.appendChild( doc.createTextNode( headMid() ) );
    }
    if ( !headRight().isEmpty() )
    {
      QDomElement right = doc.createElement( "right" );
      head.appendChild( right );
      right.appendChild( doc.createTextNode( headRight() ) );
    }
    QDomElement foot = doc.createElement( "foot" );
    paper.appendChild( foot );
    if ( !footLeft().isEmpty() )
    {
      QDomElement left = doc.createElement( "left" );
      foot.appendChild( left );
      left.appendChild( doc.createTextNode( footLeft() ) );
    }
    if ( !footMid().isEmpty() )
    {
      QDomElement center = doc.createElement( "center" );
      foot.appendChild( center );
      center.appendChild( doc.createTextNode( footMid() ) );
    }
    if ( !footRight().isEmpty() )
    {
      QDomElement right = doc.createElement( "right" );
      foot.appendChild( right );
      right.appendChild( doc.createTextNode( footRight() ) );
    }

    // print range
    QDomElement printrange = doc.createElement( "printrange-rect" );
    int left = m_printRange.left();
    int right = m_printRange.right();
    int top = m_printRange.top();
    int bottom = m_printRange.bottom();
    //If whole rows are selected, then we store zeros, as KS_colMax may change in future
    if ( left == 1 && right == KS_colMax )
    {
      left = 0;
      right = 0;
    }
    //If whole columns are selected, then we store zeros, as KS_rowMax may change in future
    if ( top == 1 && bottom == KS_rowMax )
    {
      top = 0;
      bottom = 0;
    }
    printrange.setAttribute( "left-rect", left );
    printrange.setAttribute( "right-rect", right );
    printrange.setAttribute( "bottom-rect", bottom );
    printrange.setAttribute( "top-rect", top );
    table.appendChild( printrange );

    // Print repeat columns
    QDomElement printRepeatColumns = doc.createElement( "printrepeatcolumns" );
    printRepeatColumns.setAttribute( "left", m_printRepeatColumns.first );
    printRepeatColumns.setAttribute( "right", m_printRepeatColumns.second );
    table.appendChild( printRepeatColumns );

    // Print repeat rows
    QDomElement printRepeatRows = doc.createElement( "printrepeatrows" );
    printRepeatRows.setAttribute( "top", m_printRepeatRows.first );
    printRepeatRows.setAttribute( "bottom", m_printRepeatRows.second );
    table.appendChild( printRepeatRows );

    // Save all cells.
    KSpreadCell* c = m_cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() )
        {
            QDomElement e = c->save( doc );
            if ( !e.isNull() )
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

    QPtrListIterator<KoDocumentChild> chl( m_pDoc->children() );
    for( ; chl.current(); ++chl )
    {
       if ( ((KSpreadChild*)chl.current())->table() == this )
        {
            QDomElement e;
            KSpreadChild * child = (KSpreadChild *) chl.current();

            // stupid hack :-( has anybody a better solution?
            if ( child->inherits("ChartChild") )
            {
                e = ((ChartChild *) child)->save( doc );
            }
            else
                e = chl.current()->save( doc );

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
    {
        m_pDoc->setErrorMessage( i18n("Invalid document. Table name is empty") );
        return false;
    }
    if( table.hasAttribute( "grid" ) )
    {
        m_bShowGrid = (int)table.attribute("grid").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "printGrid" ) )
    {
        m_bPrintGrid = (int)table.attribute("printGrid").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "hide" ) )
    {
        m_bTableHide = (int)table.attribute("hide").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "formular" ) )
    {
        m_bShowFormula = (int)table.attribute("formular").toInt( &ok );
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
    if( table.hasAttribute( "hidezero" ) )
    {
        m_bHideZero = (int)table.attribute("hidezero").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( table.hasAttribute( "firstletterupper" ) )
    {
        m_bFirstLetterUpper = (int)table.attribute("firstletterupper").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }

    // Load the paper layout
    QDomElement paper = table.namedItem( "paper" ).toElement();
    if ( !paper.isNull() )
    {
      QString format = paper.attribute( "format" );
      QString orientation = paper.attribute( "orientation" );

      // <borders>
      QDomElement borders = paper.namedItem( "borders" ).toElement();
      if ( !borders.isNull() )
      {
        float left = borders.attribute( "left" ).toFloat();
        float right = borders.attribute( "right" ).toFloat();
        float top = borders.attribute( "top" ).toFloat();
        float bottom = borders.attribute( "bottom" ).toFloat();
        setPaperLayout( left, top, right, bottom, format, orientation );
      }
      QString hleft, hright, hcenter;
      QString fleft, fright, fcenter;
      // <head>
      QDomElement head = paper.namedItem( "head" ).toElement();
      if ( !head.isNull() )
      {
        QDomElement left = head.namedItem( "left" ).toElement();
        if ( !left.isNull() )
          hleft = left.text();
        QDomElement center = head.namedItem( "center" ).toElement();
        if ( !center.isNull() )
        hcenter = center.text();
        QDomElement right = head.namedItem( "right" ).toElement();
        if ( !right.isNull() )
          hright = right.text();
      }
      // <foot>
      QDomElement foot = paper.namedItem( "foot" ).toElement();
      if ( !foot.isNull() )
      {
        QDomElement left = foot.namedItem( "left" ).toElement();
        if ( !left.isNull() )
          fleft = left.text();
        QDomElement center = foot.namedItem( "center" ).toElement();
        if ( !center.isNull() )
          fcenter = center.text();
        QDomElement right = foot.namedItem( "right" ).toElement();
        if ( !right.isNull() )
          fright = right.text();
      }
      setHeadFootLine( hleft, hcenter, hright, fleft, fcenter, fright);
    }

      // load print range
      QDomElement printrange = table.namedItem( "printrange-rect" ).toElement();
      if ( !printrange.isNull() )
      {
        int left = printrange.attribute( "left-rect" ).toInt();
        int right = printrange.attribute( "right-rect" ).toInt();
        int bottom = printrange.attribute( "bottom-rect" ).toInt();
        int top = printrange.attribute( "top-rect" ).toInt();
        if ( left == 0 ) //whole row(s) selected
        {
          left = 1;
          right = KS_colMax;
        }
        if ( top == 0 ) //whole column(s) selected
        {
          top = 1;
          bottom = KS_rowMax;
        }
        m_printRange = QRect( QPoint( left, top ), QPoint( right, bottom ) );
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

    // load print repeat columns
    QDomElement printrepeatcolumns = table.namedItem( "printrepeatcolumns" ).toElement();
    if ( !printrepeatcolumns.isNull() )
    {
        int left = printrepeatcolumns.attribute( "left" ).toInt();
        int right = printrepeatcolumns.attribute( "right" ).toInt();
        setPrintRepeatColumns( qMakePair( left, right ) );
    }

    // load print repeat rows
    QDomElement printrepeatrows = table.namedItem( "printrepeatrows" ).toElement();
    if ( !printrepeatrows.isNull() )
    {
        int top = printrepeatrows.attribute( "top" ).toInt();
        int bottom = printrepeatrows.attribute( "bottom" ).toInt();
        setPrintRepeatRows( qMakePair( top, bottom ) );
    }

    if( !table.hasAttribute( "borders1.2" ) )
    {
      convertObscuringBorders();
    }

    return true;
}


bool KSpreadTable::loadChildren( KoStore* _store )
{
    QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
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
    //Are these the edges of the print range?
    if ( _column == m_printRange.left() || _column == m_printRange.right() + 1 )
        return true;

    int col = m_printRange.left();
    float x = columnLayout( col )->mmWidth();
    while ( ( col <= _column ) && ( col < m_printRange.right() ) )
    {
        if ( x > printableWidth() )
        {
            if ( col == _column )
                return TRUE;
            else
            {
                x = columnLayout( col )->mmWidth();
                if ( col >= m_printRepeatColumns.first )
                    x += m_dPrintRepeatColumnsWidth;
            }
        }

        col++;
        x += columnLayout( col )->mmWidth();
    }

    return FALSE;
}

bool KSpreadTable::isOnNewPageY( int _row )
{
    //Are these the edges of the print range?
    if ( _row == m_printRange.top() || _row == m_printRange.bottom() + 1 )
        return true;

    int row = m_printRange.top();
    float y = rowLayout( row )->mmHeight();
    while ( ( row <= _row ) && ( row < m_printRange.bottom() ) )
    {
        if ( y > printableHeight() )
        {
            if ( row == _row )
                return TRUE;
            else
            {
                y = rowLayout( row )->mmHeight();
                if ( row >= m_printRepeatRows.first )
                    y += m_dPrintRepeatRowsHeight;
            }
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
    checkRangeHBorder ( _cell->column() );
    checkRangeVBorder ( _cell->row() );
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

void KSpreadTable::update()
{
  KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    updateCell(c, c->column(), c->row());
  }
}

void KSpreadTable::updateCellArea( const QRect &cellArea )
{
  if ( doc()->isLoading() || doc()->delayCalculation() || (!getAutoCalc()))
    return;

  KSpreadCell* cell = cellAt(cellArea.bottomRight());
  // Get the size
  int left = columnPos( cellArea.left() );
  int top = rowPos( cellArea.top() );
  int right = columnPos(cellArea.right()) + cell->extraWidth();
  int bottom = rowPos(cellArea.bottom()) + cell->extraHeight();

  // Need to calculate ?
  for (int x = cellArea.left(); x <= cellArea.right(); x++)
  {
    for (int y = cellArea.top(); y <= cellArea.bottom(); y++)
    {
      cell = cellAt(x,y);

      cell->calc();

      // Need to make layout ?
      cell->makeLayout( painter(), x, y );

      // Perhaps the size changed now ?
      right = QMAX( right, left + cell->extraWidth() );
      bottom = QMAX( bottom, top + cell->extraHeight() );
    }
  }

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

void KSpreadTable::updateCell( KSpreadCell */*cell*/, int _column, int _row )
{
  QRect cellArea(QPoint(_column, _row), QPoint(_column, _row));

  updateCellArea(cellArea);
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
    emit sig_maxRow(maxRow());
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
    emit sig_maxColumn( maxColumn() );
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

    ChartChild * ch = new ChartChild( m_pDoc, this, doc, _rect );
    ch->setDataArea( _data );
    ch->update();
    ch->chart()->setCanChangeValue( false  );
    // m_pDoc->insertChild( ch );
    insertChild( ch );

    KoChart::WizardExtension * wiz = ch->chart()->wizardExtension();

    if ( wiz )
        wiz->show();
}

void KSpreadTable::insertChild( const QRect& _rect, KoDocumentEntry& _e )
{
    KoDocument* doc = _e.createDoc( m_pDoc );
    if ( !doc )
    {
        kdDebug() << "Error inserting child!" << endl;
        return;
    }

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

void KSpreadTable::deleteChild( KSpreadChild* child )
{
    QPointArray polygon = child->framePointArray();

    emit sig_removeChild( child );

    delete child;

    emit sig_polygonInvalidated( polygon );
}

void KSpreadTable::changeChildGeometry( KSpreadChild *_child, const QRect& _rect )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );
}

/*
QPtrListIterator<KSpreadChild> KSpreadTable::childIterator()
{
  return QPtrListIterator<KSpreadChild> ( m_lstChildren );
}
*/

bool KSpreadTable::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;

    QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
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
    kdDebug()<<" KSpreadTable::~KSpreadTable() :"<<this<<endl;
    s_mapTables->remove( m_id );

    //when you remove all table (close file)
    //you must reinit s_id otherwise there is not
    //the good name between map and table
    if( s_mapTables->count()==0)
      s_id=0L;

    KSpreadCell* c = m_cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->tableDies();

    m_cells.clear(); // cells destructor needs table to still exist

    m_pPainter->end();
    delete m_pPainter;
    delete m_pWidget;

    delete m_defaultLayout;
    delete m_pDefaultCell;
    delete m_pDefaultRowLayout;
    delete m_pDefaultColumnLayout;
    delete m_dcop;
}


void KSpreadTable::checkRangeHBorder ( int _column )
{
    if ( m_bScrollbarUpdates && _column > m_iMaxColumn )
    {
      m_iMaxColumn = _column;
      emit sig_maxColumn( _column );
    }
}

void KSpreadTable::checkRangeVBorder ( int _row )
{
    if ( m_bScrollbarUpdates && _row > m_iMaxRow )
    {
      m_iMaxRow = _row;
      emit sig_maxRow( _row );
    }
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

void KSpreadTable::hideTable(bool _hide)
{
    setHidden(_hide);
    if(_hide)
        emit sig_TableHidden(this);
    else
        emit sig_TableShown(this);
}

void KSpreadTable::removeTable()
{
    emit sig_TableRemoved(this);
}

void KSpreadTable::setActiveTable()
{
    emit sig_maxColumn( maxColumn() );
    emit sig_maxRow( maxRow() );
    emit sig_TableActivated( this );
    emit sig_updateVBorder( this );
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
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

    QPtrListIterator<KSpreadTable> it( map()->tableList() );
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


void KSpreadTable::updateLocale()
{
  KSpreadCell* c = m_cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
      QString _text = c->text();
      c->setDisplayText( _text, false/* no recalc deps for each, done independently */ );
  }
  recalc();
}

KSpreadCell* KSpreadTable::getFirstCellColumn(int col)
{ return m_cells.getFirstCellColumn(col); }

KSpreadCell* KSpreadTable::getLastCellColumn(int col)
{ return m_cells.getLastCellColumn(col); }

KSpreadCell* KSpreadTable::getFirstCellRow(int row)
{ return m_cells.getFirstCellRow(row); }

KSpreadCell* KSpreadTable::getLastCellRow(int row)
{ return m_cells.getLastCellRow(row); }

KSpreadCell* KSpreadTable::getNextCellUp(int col, int row)
{ return m_cells.getNextCellUp(col, row); }

KSpreadCell* KSpreadTable::getNextCellDown(int col, int row)
{ return m_cells.getNextCellDown(col, row); }

KSpreadCell* KSpreadTable::getNextCellLeft(int col, int row)
{ return m_cells.getNextCellLeft(col, row); }

KSpreadCell* KSpreadTable::getNextCellRight(int col, int row)
{ return m_cells.getNextCellRight(col, row); }

void KSpreadTable::convertObscuringBorders()
{
  /* a word of explanation here:
     beginning with KSpread 1.2 (actually, cvs of Mar 28, 2002), border information
     is stored differently.  Previously, for a cell obscuring a region, the entire
     region's border's data would be stored in the obscuring cell.  This caused
     some data loss in certain situations.  After that date, each cell stores
     its own border data, and prints it even if it is an obscured cell (as long
     as that border isn't across an obscuring border).
     Anyway, this function is used when loading a file that was stored with the
     old way of borders.  All new files have the table attribute "borders1.2" so
     if that isn't in the file, all the border data will be converted here.
     It's a bit of a hack but I can't think of a better way and it's not *that*
     bad of a hack.:-)
  */
  KSpreadCell* c = m_cells.firstCell();
  QPen topPen, bottomPen, leftPen, rightPen;
  for( ;c; c = c->nextCell() )
  {
    if (c->extraXCells() > 0 || c->extraYCells() > 0)
    {
      topPen = c->topBorderPen(c->column(), c->row());
      leftPen = c->leftBorderPen(c->column(), c->row());
      rightPen = c->rightBorderPen(c->column(), c->row());
      bottomPen = c->bottomBorderPen(c->column(), c->row());

      c->setTopBorderStyle(Qt::NoPen);
      c->setLeftBorderStyle(Qt::NoPen);
      c->setRightBorderStyle(Qt::NoPen);
      c->setBottomBorderStyle(Qt::NoPen);

      for (int x = c->column(); x < c->column() + c->extraXCells(); x++)
      {
        nonDefaultCell( x, c->row() )->setTopBorderPen(topPen);
        nonDefaultCell( x, c->row() + c->extraYCells() )->
          setBottomBorderPen(bottomPen);
      }
      for (int y = c->row(); y < c->row() + c->extraYCells(); y++)
      {
        nonDefaultCell( c->column(), y )->setLeftBorderPen(leftPen);
        nonDefaultCell( c->column() + c->extraXCells(), y )->
          setRightBorderPen(rightPen);
      }
    }
  }
}

/**********************
 * Printout Functions *
 **********************/

void KSpreadTable::paperLayoutDlg()
{
    KoPageLayout pl;
    pl.format = paperFormat();
    pl.orientation = orientation();
    pl.ptWidth = MM_TO_POINT( m_paperWidth );
    pl.ptHeight = MM_TO_POINT( m_paperHeight );
    pl.ptLeft = MM_TO_POINT( leftBorder() );
    pl.ptRight = MM_TO_POINT(  rightBorder() );
    pl.ptTop = MM_TO_POINT(  topBorder() );
    pl.ptBottom = MM_TO_POINT(  bottomBorder() );

    KoHeadFoot hf;
    hf.headLeft  = localizeHeadFootLine( headLeft()  );
    hf.headRight = localizeHeadFootLine( headRight() );
    hf.headMid   = localizeHeadFootLine( headMid()   );
    hf.footLeft  = localizeHeadFootLine( footLeft()  );
    hf.footRight = localizeHeadFootLine( footRight() );
    hf.footMid   = localizeHeadFootLine( footMid()   );

    KoUnit::Unit unit = m_pDoc->getUnit();

    KoPageLayoutDia dlg( 0, "PageLayout", pl, hf, FORMAT_AND_BORDERS | HEADER_AND_FOOTER, unit );

    // ------------- options ---------------
    QWidget *tab = dlg.addPage(i18n( "Options" ));
    QGridLayout *grid = new QGridLayout( tab, 5, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QCheckBox *pPrintGrid = new QCheckBox ( i18n("Print &grid"), tab );
    pPrintGrid->setChecked( getPrintGrid() );
    grid->addWidget( pPrintGrid, 0, 0 );

    QLabel *pPrintRange = new QLabel ( i18n("Print range:"), tab );
    grid->addWidget( pPrintRange, 1, 0 );

    QLineEdit *ePrintRange = new QLineEdit( tab );
    grid->addWidget( ePrintRange, 1, 1 );
    ePrintRange->setText( util_rangeName( m_printRange ) );

    QLabel *pRepeatCols = new QLabel ( i18n("Repeat columns on each page:"), tab );
    grid->addWidget( pRepeatCols, 3, 0 );

    QLineEdit *eRepeatCols = new QLineEdit( tab );
    grid->addWidget( eRepeatCols, 3, 1 );
    if ( m_printRepeatColumns.first != 0 )
        eRepeatCols->setText( util_encodeColumnLabelText( m_printRepeatColumns.first ) +
                              ":" +
                              util_encodeColumnLabelText( m_printRepeatColumns.second ) );

    QLabel *pRepeatRows = new QLabel ( i18n("Repeat rows on each page:"), tab );
    grid->addWidget( pRepeatRows, 2, 0 );

    QLineEdit *eRepeatRows = new QLineEdit( tab );
    grid->addWidget( eRepeatRows, 2, 1 );
    if ( m_printRepeatRows.first != 0 )
        eRepeatRows->setText( QString().setNum( m_printRepeatRows.first ) +
                              ":" +
                              QString().setNum( m_printRepeatRows.second ) );

    // --------------- main grid ------------------
    grid->addColSpacing( 0, pPrintGrid->width() );
    grid->addColSpacing( 0, pPrintRange->width() );
    grid->addColSpacing( 0, pRepeatRows->width() );
    grid->addColSpacing( 0, pRepeatCols->width() );
    grid->addColSpacing( 1, ePrintRange->width() );
    grid->addColSpacing( 1, eRepeatRows->width() );
    grid->addColSpacing( 1, eRepeatCols->width() );

    grid->addRowSpacing( 0, pPrintGrid->height() );
    grid->addRowSpacing( 1, pPrintRange->height() );
    grid->addRowSpacing( 1, ePrintRange->height() );
    grid->addRowSpacing( 2, pRepeatRows->height() );
    grid->addRowSpacing( 2, eRepeatRows->height() );
    grid->addRowSpacing( 3, pRepeatCols->height() );
    grid->addRowSpacing( 3, eRepeatCols->height() );
    grid->setRowStretch( 4, 1 );

    int result = dlg.exec();
    if ( result == QDialog::Accepted )
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
             KSpreadUndoAction* undo = new KSpreadUndoPaperLayout( doc(), this );
             m_pDoc->undoBuffer()->appendUndo( undo );
        }

        pl = dlg.getLayout();
        hf = dlg.getHeadFoot();
        unit = dlg.unit();
        setPrintGrid( pPrintGrid->isChecked() );
        QString tmpPrintRange = ePrintRange->text();
        QString tmpRepeatCols = eRepeatCols->text();
        QString tmpRepeatRows = eRepeatRows->text();
        if ( tmpPrintRange.isEmpty() )
        {
            setPrintRange( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) );
        }
        else
        {
            bool error = true;
            int first = tmpPrintRange.find(":");
            if ( ( first != -1 ) && ( (int)tmpPrintRange.length() > first ) )
            {
                KSpreadPoint point1 ( tmpPrintRange.left( first ) );
                if ( point1.isValid() )
                {
                    KSpreadPoint point2 ( tmpPrintRange.mid( first+1 ) );
                    if ( point2.isValid() )
                    {
                        error = false;
                        setPrintRange ( QRect( QPoint( QMIN( point1.pos.x(), point2.pos.x() ),
                                                       QMIN( point1.pos.y(), point2.pos.y() ) ),
                                               QPoint( QMAX( point1.pos.x(), point2.pos.x() ),
                                                       QMAX( point1.pos.y(), point2.pos.y() ) ) ) );
                    }
                }
            }

            if ( error ) KMessageBox::information( 0, i18n( "Print range wrong, changes are ignored." ) );
        }

        if ( tmpRepeatCols.isEmpty() )
        {
            setPrintRepeatColumns( qMakePair( 0, 0 ) );
        }
        else
        {
            bool error = true;
            int first = tmpRepeatCols.find(":");
            if ( ( first != -1 ) && ( (int)tmpRepeatCols.length() > first ) )
            {
                int col1 = util_decodeColumnLabelText( tmpRepeatCols.left( first ) );
                if ( col1 > 0 && col1 <= KS_colMax )
                {
                    int col2 = util_decodeColumnLabelText( tmpRepeatCols.mid( first+1 ) );
                    if ( col2 > 0 && col2 <= KS_colMax )
                    {
                        error = false;
                        setPrintRepeatColumns ( qMakePair( col1, col2 ) );
                    }
                }
            }

            if ( error ) KMessageBox::information( 0, i18n( "Repeated columss range wrong, changes are ignored.\nMust be in format column:column (eg. B:C)" ) );
        }

        if ( tmpRepeatRows.isEmpty() )
        {
            setPrintRepeatRows ( qMakePair( 0, 0 ) );
        }
        else
        {
            bool error = true;
            int first = tmpRepeatRows.find(":");
            if ( ( first != -1 ) && ( (int)tmpRepeatRows.length() > first ) )
            {
                int row1 = tmpRepeatRows.left( first ).toInt();
                if ( row1 > 0 && row1 <= KS_rowMax )
                {
                    int row2 = tmpRepeatRows.mid( first+1 ).toInt();
                    if ( row2 > 0 && row2 <= KS_rowMax )
                    {
                        error = false;
                        setPrintRepeatRows ( qMakePair( row1, row2 ) );
                    }
                }
            }

            if ( error ) KMessageBox::information( 0, i18n( "Repeated rows range wrong, changes are ignored.\nMust be in format row:row (eg. 2:3)" ) );
        }
        m_pDoc->setModified( true );
    }
    else
    {
        return;
    }

    if ( pl.format == PG_CUSTOM )
    {
        m_paperWidth = POINT_TO_MM(pl.ptWidth);
        m_paperHeight = POINT_TO_MM(pl.ptHeight);
    }

    setPaperLayout( POINT_TO_MM(pl.ptLeft), POINT_TO_MM(pl.ptTop), POINT_TO_MM(pl.ptRight), POINT_TO_MM(pl.ptBottom), pl.format, pl.orientation );

    setHeadFootLine( localizeHeadFootLine( hf.headLeft  ),
                     localizeHeadFootLine( hf.headMid   ),
                     localizeHeadFootLine( hf.headRight ),
                     localizeHeadFootLine( hf.footLeft  ),
                     localizeHeadFootLine( hf.footMid   ),
                     localizeHeadFootLine( hf.footRight ) );

    m_pDoc->setUnit( unit );
}

void KSpreadTable::definePrintRange ()
{
    if ( !singleCellSelection() )
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
             KSpreadUndoAction* undo = new KSpreadUndoDefinePrintRange( doc(), this );
             m_pDoc->undoBuffer()->appendUndo( undo );
        }

        setPrintRange( selection() );
    }
}

void KSpreadTable::resetPrintRange ()
{
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
         KSpreadUndoAction* undo = new KSpreadUndoDefinePrintRange( doc(), this );
         m_pDoc->undoBuffer()->appendUndo( undo );
    }

    setPrintRange( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) );
}

void KSpreadTable::replaceHeadFootLineMacro ( QString &_text, const QString &_search, const QString &_replace )
{
    if ( _search != _replace )
        _text.replace ( QRegExp( "<" + _search + ">" ), "<" + _replace + ">" );
}

QString KSpreadTable::localizeHeadFootLine ( const QString &_text )
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro ( tmp, "page",   i18n("page") );
    replaceHeadFootLineMacro ( tmp, "file",   i18n("file") );
    replaceHeadFootLineMacro ( tmp, "name",   i18n("name") );
    replaceHeadFootLineMacro ( tmp, "time",   i18n("time") );
    replaceHeadFootLineMacro ( tmp, "date",   i18n("date") );
    replaceHeadFootLineMacro ( tmp, "author", i18n("author") );
    replaceHeadFootLineMacro ( tmp, "email",  i18n("email") );
    replaceHeadFootLineMacro ( tmp, "org",    i18n("org") );
    replaceHeadFootLineMacro ( tmp, "table",  i18n("table") );

    return tmp;
}


QString KSpreadTable::delocalizeHeadFootLine ( const QString &_text )
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro ( tmp, i18n("page"),   "page" );
    replaceHeadFootLineMacro ( tmp, i18n("file"),   "file" );
    replaceHeadFootLineMacro ( tmp, i18n("name"),   "name" );
    replaceHeadFootLineMacro ( tmp, i18n("time"),   "time" );
    replaceHeadFootLineMacro ( tmp, i18n("date"),   "date" );
    replaceHeadFootLineMacro ( tmp, i18n("author"), "author" );
    replaceHeadFootLineMacro ( tmp, i18n("email"),  "email" );
    replaceHeadFootLineMacro ( tmp, i18n("org"),    "org" );
    replaceHeadFootLineMacro ( tmp, i18n("table"),  "table" );

    return tmp;
}


KoHeadFoot KSpreadTable::getHeadFootLine() const
{
  KoHeadFoot hf;
  hf.headLeft  = m_headLeft;
  hf.headRight = m_headRight;
  hf.headMid   = m_headMid;
  hf.footLeft  = m_footLeft;
  hf.footRight = m_footRight;
  hf.footMid   = m_footMid;

  return hf;
}


void KSpreadTable::setHeadFootLine( const QString &_headl, const QString &_headm, const QString &_headr,
                                    const QString &_footl, const QString &_footm, const QString &_footr )
{
  m_headLeft  = _headl;
  m_headRight = _headr;
  m_headMid   = _headm;
  m_footLeft  = _footl;
  m_footRight = _footr;
  m_footMid   = _footm;

  m_pDoc->setModified( TRUE );
}

void KSpreadTable::setPaperOrientation( KoOrientation _orient )
{
  m_orientation = _orient;
  calcPaperSize();
}


KoPageLayout KSpreadTable::getPaperLayout() const
{
  KoPageLayout pl;
  pl.format = m_paperFormat;
  pl.orientation = m_orientation;
  pl.ptWidth  =  m_paperWidth;
  pl.ptHeight =  m_paperHeight;
  pl.ptLeft   =  m_leftBorder;
  pl.ptRight  =  m_rightBorder;
  pl.ptTop    =  m_topBorder;
  pl.ptBottom =  m_bottomBorder;
  return pl;
}


void KSpreadTable::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
                                   KoFormat _paper, KoOrientation _orientation )
{
  m_leftBorder   = _leftBorder;
  m_rightBorder  = _rightBorder;
  m_topBorder    = _topBorder;
  m_bottomBorder = _bottomBorder;
  m_orientation  = _orientation;
  m_paperFormat  = _paper;

  calcPaperSize();

//  QPtrListIterator<KoView> it( views() );
//  for( ;it.current(); ++it )
//  {
//        KSpreadView *v = static_cast<KSpreadView *>( it.current() );
        // We need to trigger the appropriate repaintings in the cells near the
        // border of the page. The easiest way for this is to turn the borders
        // off and on (or on and off if they were off).
//        bool bBorderWasShown = v->activeTable()->isShowPageBorders();
//        v->activeTable()->setShowPageBorders( !bBorderWasShown );
//        v->activeTable()->setShowPageBorders( bBorderWasShown );
//  }

  m_pDoc->setModified( TRUE );
}

void KSpreadTable::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
                                   const QString& _paper, const QString& _orientation )
{
    KoFormat f = paperFormat();
    KoOrientation o = orientation();

    QString paper( _paper );
    if ( paper[0].isDigit() ) // Custom format
    {
        const int i = paper.find( 'x' );
        if ( i < 0 )
        {
            // We have nothing useful, so assume ISO A4
            f = PG_DIN_A4;
        }
        else
        {
            f = PG_CUSTOM;
            m_paperWidth  = paper.left(i).toFloat();
            m_paperHeight = paper.mid(i+1).toFloat();
            if ( m_paperWidth < 10.0 )
                m_paperWidth = PG_A4_WIDTH;
            if ( m_paperHeight < 10.0 )
                m_paperWidth = PG_A4_HEIGHT;
        }
    }
    else
    {
        f = KoPageFormat::formatFromString( paper );
        if ( f == PG_CUSTOM )
            // We have no idea about height or width, therefore assume ISO A4
            f = PG_DIN_A4;
    }

    if ( _orientation == "Portrait" )
        o = PG_PORTRAIT;
    else if ( _orientation == "Landscape" )
        o = PG_LANDSCAPE;

    setPaperLayout( _leftBorder, _topBorder, _rightBorder, _bottomBorder, f, o );
}

void KSpreadTable::calcPaperSize()
{
    if ( m_paperFormat != PG_CUSTOM )
    {
        m_paperWidth = KoPageFormat::width( m_paperFormat, m_orientation );
        m_paperHeight = KoPageFormat::height( m_paperFormat, m_orientation );
    }
}

QString KSpreadTable::paperFormatString()const
{
    if ( m_paperFormat == PG_CUSTOM )
    {
      QString tmp;
      tmp.sprintf( "%fx%f", m_paperWidth, m_paperHeight );
      return tmp;
    }

    return KoPageFormat::formatString( m_paperFormat );
}

const char* KSpreadTable::orientationString()
{
    switch( m_orientation )
    {
    case KPrinter::Portrait:
        return "Portrait";
    case KPrinter::Landscape:
        return "Landscape";
    }

    assert( 0 );
    return 0;
}

QString KSpreadTable::completeHeading( const QString &_data, int _page, const QString &_table ) const
{
    QString page(QString::number(_page));

    QString pathFileName(m_pDoc->url().path());
    if ( pathFileName.isNull() )
        pathFileName="";

    QString fileName(m_pDoc->url().fileName());
    if( fileName.isNull())
        fileName="";

    QString t(QTime::currentTime().toString());
    QString d(QDate::currentDate().toString());
    QString ta;
    if ( !_table.isEmpty() )
        ta = _table;

    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    QString full_name;
    QString email_addr;
    QString organization;
    QString tmp;
    if ( !authorPage )
        kdWarning() << "Author information not found in document Info !" << endl;
    else
    {
        full_name = authorPage->fullName();
        email_addr = authorPage->email();
        organization = authorPage->company();
    }

    char hostname[80];
    struct passwd *p;

    p = getpwuid(getuid());
    gethostname(hostname, sizeof(hostname));

    if(full_name.isEmpty())
 	full_name=p->pw_gecos;

    if( email_addr.isEmpty())
	email_addr = QString("%1@%2").arg(p->pw_name).arg(hostname);

    tmp = _data;
    int pos = 0;
    while ( ( pos = tmp.find( "<page>", pos ) ) != -1 )
        tmp.replace( pos, 6, page );
    pos = 0;
    while ( ( pos = tmp.find( "<file>", pos ) ) != -1 )
        tmp.replace( pos, 6, pathFileName );
    pos = 0;
    while ( ( pos = tmp.find( "<name>", pos ) ) != -1 )
        tmp.replace( pos, 6, fileName );
    pos = 0;
    while ( ( pos = tmp.find( "<time>", pos ) ) != -1 )
        tmp.replace( pos, 6, t );
    pos = 0;
    while ( ( pos = tmp.find( "<date>", pos ) ) != -1 )
        tmp.replace( pos, 6, d );
    pos = 0;
    while ( ( pos = tmp.find( "<author>", pos ) ) != -1 )
        tmp.replace( pos, 8, full_name );
    pos = 0;
    while ( ( pos = tmp.find( "<email>", pos ) ) != -1 )
        tmp.replace( pos, 7, email_addr );
    pos = 0;
    while ( ( pos = tmp.find( "<org>", pos ) ) != -1 )
        tmp.replace( pos, 5, organization );
    pos = 0;
    while ( ( pos = tmp.find( "<table>", pos ) ) != -1 )
        tmp.replace( pos, 7, ta );

    return tmp;
}


void KSpreadTable::setPrintRange( QRect _printRange )
{
  if ( m_printRange == _printRange )
    return;

  m_printRange = _printRange;
  m_pDoc->setModified( true );

  emit sig_updateView( this );
}

void KSpreadTable::setPrintGrid( bool _printGrid )
{
  if ( m_bPrintGrid == _printGrid )
    return;

  m_bPrintGrid = _printGrid;
  m_pDoc->setModified( true );
}

void KSpreadTable::setPrintRepeatColumns( QPair<int, int> _printRepeatColumns )
{
    //Bring arguments in order
    if ( _printRepeatColumns.first > _printRepeatColumns.second )
    {
        int tmp = _printRepeatColumns.first;
        _printRepeatColumns.first = _printRepeatColumns.second;
        _printRepeatColumns.second = tmp;
    }

    //If old are equal to the new setting, nothing is to be done at all
    if ( m_printRepeatColumns == _printRepeatColumns )
        return;

    m_printRepeatColumns = _printRepeatColumns;

    //Recalcualte the space needed for the repeated columns
    m_dPrintRepeatColumnsWidth = 0.0;
    if ( m_printRepeatColumns.first != 0 )
    {
        for ( int i = m_printRepeatColumns.first; i <= m_printRepeatColumns.second; i++)
        {
            m_dPrintRepeatColumnsWidth += columnLayout( i )->mmWidth();
        }
    }

    //Refresh view, if page borders are shown
    if ( m_bShowPageBorders ) emit sig_updateView( this );
    m_pDoc->setModified( true );
}

void KSpreadTable::setPrintRepeatRows( QPair<int, int> _printRepeatRows )
{
    //Bring arguments in order
    if ( _printRepeatRows.first > _printRepeatRows.second )
    {
        int tmp = _printRepeatRows.first;
        _printRepeatRows.first = _printRepeatRows.second;
        _printRepeatRows.second = tmp;
    }

    //If old are equal to the new setting, nothing is to be done at all
    if ( m_printRepeatRows == _printRepeatRows )
        return;

    m_printRepeatRows = _printRepeatRows;
  
    //Recalcualte the space needed for the repeated rows
    m_dPrintRepeatRowsHeight += 0.0;
    if ( m_printRepeatRows.first != 0 )
    {
        for ( int i = m_printRepeatRows.first; i <= m_printRepeatRows.second; i++)
        {
            m_dPrintRepeatRowsHeight += rowLayout( i )->mmHeight();
        }
    }

    //Refresh view, if page borders are shown
    if ( m_bShowPageBorders ) emit sig_updateView( this );
    m_pDoc->setModified( true );
}

QRect KSpreadTable::getSelectionHandleArea(KSpreadCanvas* canvas)
{
  /* maybe this should be moved to the table? */
  KSpreadCell* cell = NULL;
  int column, row, xpos, ypos, width, height;

  // complete rows/columns are selected, use the marker.
  if ( isRowSelected() || isColumnSelected() )
  {
    column = marker().x();
    row = marker().y();
  }
  else
  {
    column = selection().right();
    row = selection().bottom();
  }
  cell = cellAt(column, row);

  xpos = columnPos( column, canvas );
  ypos = rowPos( row, canvas );
  width = cell->width( column );
  height = cell->height( row );

  QRect handle( (xpos + width - 2), (ypos + height - 1), 5, 5);
  return handle;
}

#ifndef NDEBUG
void KSpreadTable::printDebug()
{
    int iMaxColumn = maxColumn();
    int iMaxRow = maxRow();

    kdDebug(36001) << "Cell | Content  | DataT | Text" << endl;
    KSpreadCell *cell;
    for ( int currentrow = 1 ; currentrow < iMaxRow ; ++currentrow )
    {
        for ( int currentcolumn = 1 ; currentcolumn < iMaxColumn ; currentcolumn++ )
        {
            cell = cellAt( currentcolumn, currentrow );
            if ( !cell->isDefault() && !cell->isEmpty() )
            {
                QString cellDescr = util_cellName( currentcolumn, currentrow );
                cellDescr = cellDescr.rightJustify( 4,' ' );
                //QString cellDescr = "Cell ";
                //cellDescr += QString::number(currentrow).rightJustify(3,'0') + ',';
                //cellDescr += QString::number(currentcolumn).rightJustify(3,'0') + ' ';
                cellDescr += " | ";
                static const char* s_contentString[] = {
                        "Text    ", "RichTxt ", "Formula ", "VisForm ", "ERROR   " };
                cellDescr += s_contentString[ cell->content() ];
                cellDescr += " | ";
                cellDescr += cell->dataTypeToString( cell->dataType() ).rightJustify(5,' ');
                cellDescr += " | ";
                cellDescr += cell->text();
                if ( cell->content() == KSpreadCell::Formula )
                    cellDescr += QString("  [result: %1]").arg( cell->valueString() );
                kdDebug(36001) << cellDescr << endl;
            }
        }
    }
}
#endif

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

KoChart::Part* ChartChild::chart()
{
    assert( document()->inherits( "KoChart::Part" ) );
    return static_cast<KoChart::Part *>( document() );
}

#include "kspread_table.moc"
