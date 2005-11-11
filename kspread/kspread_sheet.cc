/* This file is part of the KDE project
   Copyright (C) 1998,  1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 - 2005 The KSpread Team
                             www.koffice.org/kspread

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

#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <pwd.h>
#include <unistd.h>

#include <qapplication.h>
#include <qclipboard.h>
#include <qpicture.h>
#include <qregexp.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <kmessagebox.h>

#include <kfind.h>
#include <kfinddialog.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <kprinter.h>
#include <koDocumentInfo.h>
#include <koOasisStyles.h>
#include <koUnit.h>
#include <koStyleStack.h>
#include <koOasisSettings.h>
#include <koQueryTrader.h>
#include <koxmlns.h>
#include <kodom.h>

#include "dependencies.h"

#include "ksprsavinginfo.h"
#include "kspread_cluster.h"
#include "kspread_condition.h"
#include "kspread_sheet.h"
#include "kspread_sheetprint.h"
#include "kspread_locale.h"
#include "kspread_selection.h"
#include "kspread_global.h"
#include "kspread_undo.h"
#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_canvas.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "ksploadinginfo.h"
#include "KSpreadTableIface.h"

#include <kdebug.h>
#include <kmdcodec.h>
#include <assert.h>

#include <koChart.h>
#include "kspread_sheet.moc"

#define NO_MODIFICATION_POSSIBLE \
do { \
  KMessageBox::error( 0, i18n ( "You cannot change a protected sheet" ) ); return; \
} while(0)

using namespace KSpread;

/*****************************************************************************
 *
 * CellBinding
 *
 *****************************************************************************/

CellBinding::CellBinding( Sheet *_sheet, const QRect& _area )
{
  m_rctDataArea = _area;

  m_pSheet = _sheet;
  m_pSheet->addCellBinding( this );

  m_bIgnoreChanges = false;
}

CellBinding::~CellBinding()
{
  m_pSheet->removeCellBinding( this );
}

void CellBinding::cellChanged( Cell *_cell )
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

ChartBinding::ChartBinding( Sheet *_sheet, const QRect& _area, ChartChild *_child )
    : CellBinding( _sheet, _area )
{
  m_child = _child;
}

ChartBinding::~ChartBinding()
{
}

void ChartBinding::cellChanged( Cell* )
{
    kdDebug(36001) << "######### void ChartBinding::cellChanged( Cell* )" << endl;

    if ( m_bIgnoreChanges )
        return;

    kdDebug(36001) << "with=" << m_rctDataArea.width() << "  height=" << m_rctDataArea.height() << endl;

    KoChart::Data matrix( m_rctDataArea.height(), m_rctDataArea.width() );

    Cell* cell;
    for ( int y = 0; y < m_rctDataArea.height(); y++ )
        for ( int x = 0; x < m_rctDataArea.width(); x++ )
        {
            cell = m_pSheet->cellAt( m_rctDataArea.left() + x, m_rctDataArea.top() + y );
            if ( cell && cell->value().isNumber() )
                matrix.cell( y, x ) = KoChart::Value( cell->value().asFloat() );
            else if ( cell )
                matrix.cell( y, x ) = KoChart::Value( cell->value().asString() );
            else
                matrix.cell( y, x ) = KoChart::Value();
        }

    // ######### Kalle may be interested in that, too
    /* Chart::Range range;
       range.top = m_rctDataArea.top();
       range.left = m_rctDataArea.left();
       range.right = m_rctDataArea.right();
       range.bottom = m_rctDataArea.bottom();
       range.sheet = m_pSheet->name(); */

    m_child->chart()->setData( matrix );

    // Force a redraw of the chart on all views

    /** TODO - replace the call below with something that will repaint this chart */
//    sheet()->emit_polygonInvalidated( m_child->framePointArray() );
}

/******************************************************************/
/* Class: TextDrag                                               */
/******************************************************************/

TextDrag::TextDrag( QWidget * dragSource, const char * name )
    : QTextDrag( dragSource, name )
{
}

TextDrag::~TextDrag()
{
}


QByteArray TextDrag::encodedData( const char * mime ) const
{
  if ( strcmp( selectionMimeType(), mime ) == 0)
    return m_kspread;
  else
    return QTextDrag::encodedData( mime );
}

bool TextDrag::canDecode( QMimeSource* e )
{
  if ( e->provides( selectionMimeType() ) )
    return true;
  return QTextDrag::canDecode(e);
}

const char * TextDrag::format( int i ) const
{
  if ( i < 4 ) // HACK, but how to do otherwise ??
    return QTextDrag::format(i);
  else if ( i == 4 )
    return selectionMimeType();
  else return 0;
}

const char * TextDrag::selectionMimeType()
{
  return "application/x-kspread-snippet";
}

/*****************************************************************************
 *
 * Sheet
 *
 *****************************************************************************/

class KSpread::SheetPrivate
{
public:

  Map* workbook;

  DCOPObject* dcop;

  QString name;
  int id;

  Sheet::LayoutDirection layoutDirection;

  // true if sheet is hidden
  bool hide;

  // password of protected sheet
  QCString password;


  bool showGrid;
  bool showFormula;
  bool showFormulaIndicator;
  bool autoCalc;
  bool lcMode;
  bool showColumnNumber;
  bool hideZero;
  bool firstLetterUpper;

  // clusters to hold objects
  Cluster cells;
  RowCluster rows;
  ColumnCluster columns;

  // default objects
  Cell* defaultCell;
  Format* defaultFormat;
  RowFormat* defaultRowFormat;
  ColumnFormat* defaultColumnFormat;

  // hold the print object
  SheetPrint* print;

  // cells that need painting
  QValueList<QRect> paintDirtyList;

  // to get font metrics
  QPainter *painter;
  QWidget *widget;

  // List of all cell bindings. For example charts use bindings to get
  // informed about changing cell contents.
  QPtrList<CellBinding> cellBindings;

  // Indicates whether the sheet should paint the page breaks.
  // Doing so costs some time, so by default it should be turned off.
  bool showPageBorders;

  // List of all embedded objects. FIXME unused ??
  // QPtrList<Child> m_lstChildren;

  // The highest row and column ever accessed by the user.
  int maxRow;
  int maxColumn;

  // Max range of canvas in x and ye direction.
  //  Depends on KS_colMax/KS_rowMax and the width/height of all columns/rows
  double sizeMaxX;
  double sizeMaxY;


  bool scrollBarUpdates;

  QPen emptyPen;
  QBrush emptyBrush;
  QColor emptyColor;

  int scrollPosX;
  int scrollPosY;

  KSpread::DependencyManager *dependencies;
};

int Sheet::s_id = 0L;
QIntDict<Sheet>* Sheet::s_mapSheets;

Sheet* Sheet::find( int _id )
{
  if ( !s_mapSheets )
    return 0L;

  return (*s_mapSheets)[ _id ];
}

Sheet::Sheet (Map* map,
    const QString &sheetName, const char *_name )
  : QObject( map, _name )
{
  if ( s_mapSheets == 0L )
    s_mapSheets = new QIntDict<Sheet>;
  d = new SheetPrivate;

  d->workbook = map;

  d->id = s_id++;
  s_mapSheets->insert( d->id, this );

  d->layoutDirection = LeftToRight;

  d->defaultFormat = new Format (this, d->workbook->doc()->styleManager()->defaultStyle());
  d->emptyPen.setStyle( Qt::NoPen );
  d->dcop = 0;
  d->name = sheetName;

  dcopObject();
  d->cellBindings.setAutoDelete( false );

  // m_lstChildren.setAutoDelete( true );

  d->cells.setAutoDelete( true );
  d->rows.setAutoDelete( true );
  d->columns.setAutoDelete( true );

  d->defaultCell = new Cell( this, d->workbook->doc()->styleManager()->defaultStyle(), 0, 0);
  d->defaultRowFormat = new RowFormat( this, 0 );
  d->defaultRowFormat->setDefault();
  d->defaultColumnFormat = new ColumnFormat( this, 0 );
  d->defaultColumnFormat->setDefault();

  d->widget = new QWidget();
  d->painter = new QPainter;
  d->painter->begin( d->widget );

  d->maxColumn = 256;
  d->maxRow = 256;
  d->sizeMaxX = KS_colMax * d->defaultColumnFormat->dblWidth(); // default is max cols * default width
  d->sizeMaxY = KS_rowMax * d->defaultRowFormat->dblHeight(); // default is max rows * default height

  d->scrollBarUpdates = true;

  setHidden( false );
  d->showGrid=true;
  d->showFormula=false;
  d->showFormulaIndicator=true;
  d->showPageBorders = false;

  d->lcMode=false;
  d->showColumnNumber=false;
  d->hideZero=false;
  d->firstLetterUpper=false;
  d->autoCalc=true;
  // Get a unique name so that we can offer scripting
  if ( !_name )
  {
      QCString s;
      s.sprintf("Sheet%i", s_id );
      QObject::setName( s.data() );
  }
  d->print = new SheetPrint( this );

  // initialize dependencies
  d->dependencies = new KSpread::DependencyManager (this);
  
  // connect to named area slots
  QObject::connect( doc(), SIGNAL( sig_addAreaName( const QString & ) ),
    this, SLOT( slotAreaModified( const QString & ) ) );

  QObject::connect( doc(), SIGNAL( sig_removeAreaName( const QString & ) ), 
    this, SLOT( slotAreaModified( const QString & ) ) );


}

QString Sheet::sheetName() const
{
  return d->name;
}

Map* Sheet::workbook()
{
  return d->workbook;
}

Doc* Sheet::doc()
{
  return d->workbook->doc();
}

int Sheet::id() const
{
  return d->id;
}

Sheet::LayoutDirection Sheet::layoutDirection() const
{
  return d->layoutDirection;
}

void Sheet::setLayoutDirection( LayoutDirection dir )
{
  d->layoutDirection = dir;
}

bool Sheet::isRightToLeft() const
{
  return d->layoutDirection == RightToLeft;
}

bool Sheet::isHidden() const
{
  return d->hide;
}

void Sheet::setHidden( bool hidden )
{
  d->hide = hidden;
}

bool Sheet::getShowGrid() const
{
    return d->showGrid;
}

void Sheet::setShowGrid( bool _showGrid )
{
    d->showGrid=_showGrid;
}

bool Sheet::getShowFormula() const
{
    return d->showFormula;
}

void Sheet::setShowFormula( bool _showFormula )
{
    d->showFormula=_showFormula;
}

bool Sheet::getShowFormulaIndicator() const
{
    return d->showFormulaIndicator;
}

void Sheet::setShowFormulaIndicator( bool _showFormulaIndicator )
{
    d->showFormulaIndicator=_showFormulaIndicator;
}

bool Sheet::getLcMode() const
{
    return d->lcMode;
}

void Sheet::setLcMode( bool _lcMode )
{
    d->lcMode=_lcMode;
}

bool Sheet::getAutoCalc() const
{
    return d->autoCalc;
}

void Sheet::setAutoCalc( bool _AutoCalc )
{
    d->autoCalc=_AutoCalc;
}

bool Sheet::getShowColumnNumber() const
{
    return d->showColumnNumber;
}

void Sheet::setShowColumnNumber( bool _showColumnNumber )
{
    d->showColumnNumber=_showColumnNumber;
}

bool Sheet::getHideZero() const
{
    return d->hideZero;
}

void Sheet::setHideZero( bool _hideZero )
{
    d->hideZero=_hideZero;
}

bool Sheet::getFirstLetterUpper() const
{
    return d->firstLetterUpper;
}

void Sheet::setFirstLetterUpper( bool _firstUpper )
{
    d->firstLetterUpper=_firstUpper;
}

bool Sheet::isShowPageBorders() const
{
    return d->showPageBorders;
}

bool Sheet::isEmpty( unsigned long int x, unsigned long int y ) const
{
  const Cell* c = cellAt( x, y );
  if ( !c || c->isEmpty() )
    return true;

  return false;
}

Cell* Sheet::defaultCell() const
{
    return d->defaultCell;
}

KSpread::Format* Sheet::defaultFormat()
{
    return d->defaultFormat;
}

const KSpread::Format* Sheet::defaultFormat() const
{
    return d->defaultFormat;
}

const ColumnFormat* Sheet::columnFormat( int _column ) const
{
    const ColumnFormat *p = d->columns.lookup( _column );
    if ( p != 0L )
        return p;

    return d->defaultColumnFormat;
}

ColumnFormat* Sheet::columnFormat( int _column )
{
    ColumnFormat *p = d->columns.lookup( _column );
    if ( p != 0L )
        return p;

    return d->defaultColumnFormat;
}

const RowFormat* Sheet::rowFormat( int _row ) const
{
    const RowFormat *p = d->rows.lookup( _row );
    if ( p != 0L )
        return p;

    return d->defaultRowFormat;
}

RowFormat* Sheet::rowFormat( int _row )
{
    RowFormat *p = d->rows.lookup( _row );
    if ( p != 0L )
        return p;

    return d->defaultRowFormat;
}

Value Sheet::value (int col, int row) const
{
  Cell *cell = d->cells.lookup (col, row);
  if (cell)
    return cell->value ();
  Value empty;
  return empty;
}

Value Sheet::valueRange (int col1, int row1,
    int col2, int row2) const
{
  return d->cells.valueRange (col1, row1, col2, row2);
}

void Sheet::password( QCString & passwd ) const
{
    passwd = d->password;
}

bool Sheet::isProtected() const
{
    return !d->password.isNull();
}

void Sheet::setProtected( QCString const & passwd )
{
  d->password = passwd;
}

bool Sheet::checkPassword( QCString const & passwd ) const
{
    return ( passwd == d->password );
}

SheetPrint* Sheet::print() const
{
    return d->print;
}

QPainter& Sheet::painter()
{
    return *d->painter;
}

QWidget* Sheet::widget()const
{
    return d->widget;
}

CellBinding* Sheet::firstCellBinding()
{
    return d->cellBindings.first();
}

CellBinding* Sheet::nextCellBinding()
{
    return d->cellBindings.next();
}

void Sheet::setDefaultHeight( double height )
{
  if ( isProtected() )
    NO_MODIFICATION_POSSIBLE;

  d->defaultRowFormat->setDblHeight( height );
}

void Sheet::setDefaultWidth( double width )
{
  if ( isProtected() )
    NO_MODIFICATION_POSSIBLE;

  d->defaultColumnFormat->setDblWidth( width );
}

double Sheet::sizeMaxX() const
{
  return d->sizeMaxX;
}

double Sheet::sizeMaxY() const
{
  return d->sizeMaxY;
}

int Sheet::maxColumn() const
{
  return d->maxColumn;
}

int Sheet::maxRow() const
{
  return d->maxRow;
}

const QPen& Sheet::emptyPen() const
{
  return d->emptyPen;
}

const QBrush& Sheet::emptyBrush() const
{
  return d->emptyBrush;
}

const QColor& Sheet::emptyColor() const
{
  return d->emptyColor;
}

KSpread::DependencyManager *Sheet::dependencies ()
{
  return d->dependencies;
}

int Sheet::leftColumn( double _xpos, double &_left,
                              const Canvas *_canvas ) const
{
    if ( _canvas )
    {
        _xpos += _canvas->xOffset();
        _left = -_canvas->xOffset();
    }
    else
        _left = 0.0;

    int col = 1;
    double x = columnFormat( col )->dblWidth( _canvas );
    while ( x < _xpos )
    {
        // Should never happen
        if ( col >= KS_colMax )
  {
      kdDebug(36001) << "Sheet:leftColumn: invalid column (col: " << col + 1 << ")" << endl;
      return KS_colMax + 1; //Return out of range value, so other code can react on this
  }
        _left += columnFormat( col )->dblWidth( _canvas );
        col++;
        x += columnFormat( col )->dblWidth( _canvas );
    }

    return col;
}

int Sheet::rightColumn( double _xpos, const Canvas *_canvas ) const
{
    if ( _canvas )
        _xpos += _canvas->xOffset();

    int col = 1;
    double x = 0.0;
    while ( x < _xpos )
    {
        // Should never happen
        if ( col > KS_colMax )
  {
      kdDebug(36001) << "Sheet:rightColumn: invalid column (col: " << col << ")" << endl;
            return KS_colMax + 1; //Return out of range value, so other code can react on this
  }
        x += columnFormat( col )->dblWidth( _canvas );
        col++;
    }

    return col - 1;
}

QRect Sheet::visibleRect( Canvas const * const _canvas ) const
{
  int top    = 0;
  int left   = 0;

  double x      = 0;
  double y      = 0;
  double width  = 0;
  double height = 0;

  if ( _canvas )
  {
    y     += _canvas->yOffset() * _canvas->zoom();
    x     += _canvas->xOffset() * _canvas->zoom();
    width  = _canvas->width();
    height = _canvas->height();
  }

  double yn = rowFormat( top )->dblHeight( _canvas );
  while ( yn < y )
  {
    if ( top >= KS_rowMax ) // Should never happen
      break;

    ++top;
    yn += rowFormat( top )->dblHeight( _canvas );
  }

  int bottom = top + 1;

  y += height;
  while ( yn < y )
  {
    if ( bottom > KS_rowMax ) // Should never happen
      break;

    ++bottom;
    yn += rowFormat( bottom )->dblHeight( _canvas );
  }

  double xn = columnFormat( left )->dblWidth( _canvas );
  while ( xn < x )
  {
    if ( left >= KS_colMax )    // Should never happen
      break;

    ++left;
    xn += columnFormat( left )->dblWidth( _canvas );
  }
  x += width;

  int right = left + 1;

  while ( xn < x )
  {
    if ( right > KS_colMax )    // Should never happen
      break;

    ++right;
    xn += columnFormat( right )->dblWidth( _canvas );
  }
  x += width;

  return QRect( left, top, right - left + 1, bottom - top + 1 );
}

int Sheet::topRow( double _ypos, double & _top,
                          const Canvas *_canvas ) const
{
    if ( _canvas )
    {
        _ypos += _canvas->yOffset();
        _top = -_canvas->yOffset();
    }
    else
        _top = 0.0;

    int row = 1;
    double y = rowFormat( row )->dblHeight( _canvas );
    while ( y < _ypos )
    {
        // Should never happen
        if ( row >= KS_rowMax )
        {
            kdDebug(36001) << "Sheet:topRow: invalid row (row: " << row + 1 << ")" << endl;
            return KS_rowMax + 1; //Return out of range value, so other code can react on this
        }
        _top += rowFormat( row )->dblHeight( _canvas );
        row++;
        y += rowFormat( row )->dblHeight( _canvas );
    }

    return row;
}

int Sheet::bottomRow( double _ypos, const Canvas *_canvas ) const
{
    if ( _canvas )
        _ypos += _canvas->yOffset();

    int row = 1;
    double y = 0.0;
    while ( y < _ypos )
    {
        // Should never happen
        if ( row > KS_rowMax )
  {
      kdDebug(36001) << "Sheet:bottomRow: invalid row (row: " << row << ")" << endl;
            return KS_rowMax + 1; //Return out of range value, so other code can react on this
  }
        y += rowFormat( row )->dblHeight( _canvas );
        row++;
    }

    return row - 1;
}

double Sheet::dblColumnPos( int _col, const Canvas *_canvas ) const
{
    double x = 0.0;
    if ( _canvas )
      x -= _canvas->xOffset();
    for ( int col = 1; col < _col; col++ )
    {
        // Should never happen
        if ( col > KS_colMax )
  {
      kdDebug(36001) << "Sheet:columnPos: invalid column (col: " << col << ")" << endl;
            return x;
  }

        x += columnFormat( col )->dblWidth( _canvas );
    }

    return x;
}

int Sheet::columnPos( int _col, const Canvas *_canvas ) const
{
    return (int)dblColumnPos( _col, _canvas );
}


double Sheet::dblRowPos( int _row, const Canvas *_canvas ) const
{
    double y = 0.0;
    if ( _canvas )
      y -= _canvas->yOffset();

    for ( int row = 1 ; row < _row ; row++ )
    {
        // Should never happen
        if ( row > KS_rowMax )
  {
      kdDebug(36001) << "Sheet:rowPos: invalid row (row: " << row << ")" << endl;
            return y;
  }

        y += rowFormat( row )->dblHeight( _canvas );
    }

    return y;
}

int Sheet::rowPos( int _row, const Canvas *_canvas ) const
{
    return (int)dblRowPos( _row, _canvas );
}


void Sheet::adjustSizeMaxX ( double _x )
{
    d->sizeMaxX += _x;
}

void Sheet::adjustSizeMaxY ( double _y )
{
    d->sizeMaxY += _y;
}

Cell* Sheet::visibleCellAt( int _column, int _row, bool _scrollbar_update )
{
  Cell* cell = cellAt( _column, _row, _scrollbar_update );
  if ( cell->obscuringCells().isEmpty() )
      return cell;
  else
      return cell->obscuringCells().last();
}

Cell* Sheet::firstCell() const
{
    return d->cells.firstCell();
}

RowFormat* Sheet::firstRow() const
{
    return d->rows.first();
}

ColumnFormat* Sheet::firstCol() const
{
    return d->columns.first();
}

Cell* Sheet::cellAt( int _column, int _row ) const
{
    Cell *p = d->cells.lookup( _column, _row );
    if ( p != 0L )
        return p;

    return d->defaultCell;
}

Cell* Sheet::cellAt( int _column, int _row, bool _scrollbar_update )
{
  if ( _column > KS_colMax ) {
    _column = KS_colMax;
    kdDebug (36001) << "Sheet::cellAt: column range: (col: " << _column << ")" << endl;
  }
  if ( _row > KS_rowMax) {
    kdDebug (36001) << "Sheet::cellAt: row out of range: (row: " << _row << ")" << endl;
    _row = KS_rowMax;
  }

  if ( _scrollbar_update && d->scrollBarUpdates )
  {
    checkRangeHBorder( _column );
    checkRangeVBorder( _row );
  }

  Cell *p = d->cells.lookup( _column, _row );
  if ( p != 0L )
    return p;

  return d->defaultCell;
}

ColumnFormat* Sheet::nonDefaultColumnFormat( int _column, bool force_creation )
{
    ColumnFormat *p = d->columns.lookup( _column );
    if ( p != 0L || !force_creation )
        return p;

    p = new ColumnFormat( this, _column );
    // TODO: copy the default ColumnFormat here!!
    p->setDblWidth( d->defaultColumnFormat->dblWidth() );

    d->columns.insertElement( p, _column );

    return p;
}

RowFormat* Sheet::nonDefaultRowFormat( int _row, bool force_creation )
{
    RowFormat *p = d->rows.lookup( _row );
    if ( p != 0L || !force_creation )
        return p;

    p = new RowFormat( this, _row );
    // TODO: copy the default RowLFormat here!!
    p->setDblHeight( d->defaultRowFormat->dblHeight() );

    d->rows.insertElement( p, _row );

    return p;
}

Cell* Sheet::nonDefaultCell( int _column, int _row,
                                           bool _scrollbar_update, Style * _style )
{
  if ( _scrollbar_update && d->scrollBarUpdates )
  {
    checkRangeHBorder( _column );
    checkRangeVBorder( _row );
  }

  Cell * p = d->cells.lookup( _column, _row );
  if ( p != 0L )
    return p;

  Cell * cell = 0;

  if ( _style )
    cell = new Cell( this, _style, _column, _row );
  else
    cell = new Cell( this, _column, _row );

  insertCell( cell );

  return cell;
}

void Sheet::setText( int _row, int _column, const QString& _text, bool asString )
{
    Cell * cell = nonDefaultCell( _column, _row );

    if ( isProtected() )
    {
      if ( !cell->notProtected( _column, _row ) )
        NO_MODIFICATION_POSSIBLE;
    }

    if ( !doc()->undoLocked() )
    {
        UndoSetText *undo = new UndoSetText( doc(), this, cell->text(), _column, _row,cell->formatType() );
        doc()->addCommand( undo );
    }

    // The cell will force a display refresh itself, so we dont have to care here.
    cell->setCellText( _text, asString );
    //refresh anchor
    if(_text.at(0)=='!')
      emit sig_updateView( this, QRect(_column,_row,_column,_row) );
}

void Sheet::setArrayFormula (int _row, int _column, int rows, int cols,
    const QString &_text)
{
  // check protection
  bool nomodify = false;
  if ( isProtected() )
    for (int row = _row; row < _row+rows; ++row)
      for (int col = _column; col < _column+cols; ++col) {
        Cell * cell = nonDefaultCell (col, row);
        if ( !cell->notProtected( _column, _row ) )
        {
          nomodify = true;
          break;
        }
      }
  if (nomodify)
    NO_MODIFICATION_POSSIBLE;

  // add undo
  if ( !doc()->undoLocked() )
  {
    UndoChangeAreaTextCell *undo =
        new UndoChangeAreaTextCell (doc(), this,
        QRect (_column, _row, cols, rows));
    doc()->addCommand( undo );
  }

  // fill in the cells ... top-left one gets the formula, the rest gets =INDEX
  // TODO: also fill in information about cells being a part of a range
  Cell *cell = nonDefaultCell (_column, _row);
  cell->setCellText (_text, false);
  QString cellRef = cell->name();
  for (int row = 0; row < rows; ++row)
    for (int col = 0; col < cols; col++)
      if (col || row)
      {
        Cell *cell = nonDefaultCell (_column + col, _row + row);
        cell->setCellText ("=INDEX(" + cellRef + ";" + QString::number (row+1)
            + ";" + QString::number (col+1) + ")", false);
      }
}

void Sheet::setLayoutDirtyFlag()
{
    Cell * c = d->cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->setLayoutDirtyFlag();
}

void Sheet::setCalcDirtyFlag()
{
    Cell* c = d->cells.firstCell();
    for( ; c; c = c->nextCell() )
    {
        if ( !(c->isObscured() && c->isObscuringForced()) )
            c->setCalcDirtyFlag();
    }
}

void Sheet::recalc()
{
  //  emitBeginOperation(true);
  //  setRegionPaintDirty(QRect(QPoint(1,1), QPoint(KS_colMax, KS_rowMax)));
  setCalcDirtyFlag();
  //  emitEndOperation();
  emit sig_updateView( this );
}

void Sheet::valueChanged (Cell *cell)
{
  //TODO: call cell updating, when cell damaging implemented
  //TODO: do nothing is updates are disabled

  //prepare the Point structure
  Point c;
  c.setRow (cell->row());
  c.setColumn (cell->column());
  c.sheet = this;

  //update dependencies
  d->dependencies->cellChanged (c);

  //REMOVED - modification change - this was causing modified flag to be set inappropriately.
  //nobody else seems to be setting the modified flag, so we do it here
//  doc()->setModified (true);
}

/*
 Methods working on selections:

 TYPE A:
 { columns selected:
   for all rows with properties X,X':
     if default-cell create new cell
 }
 post undo object (always a UndoCellLayout; difference in title only)
 { rows selected:
   if condition Y clear properties X,X' of cells;
   set properties X,X' of rowformats
   emit complete update;
 }
 { columns selected:
   if condition Y clear properties X,X' of cells;
   set properties X,X' of columnformats;
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

class UndoAction* Sheet::CellWorkerTypeA::createUndoAction( Doc* doc, Sheet* sheet, QRect& r )
{
    QString title = getUndoTitle();
    return new UndoCellFormat( doc, sheet, r, title );
}

/*
Sheet::SelectionType Sheet::workOnCells( const QPoint& _marker, CellWorker& worker )
{
    // see what is selected; if nothing, take marker position
    bool selected = ( m_rctSelection.left() != 0 );
    QRect r( m_rctSelection );
    if ( !selected )
  r.setCoords( _marker.x(), _marker.y(), _marker.x(), _marker.y() );

    // create cells in rows if complete columns selected
    Cell *cell;
    if ( !worker.type_B && selected && isColumnSelected() )
    {
  for ( RowFormat* rw =d->rows.first(); rw; rw = rw->next() )
  {
      if ( !rw->isDefault() && worker.testCondition( rw ) )
      {
    for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ )
    {
        cell = cellAt( i, rw->row() );
        if ( cell == d->defaultCell )
      // '&& worker.create_if_default' unnecessary as never used in type A
        {
      cell = new Cell( this, i, rw->row() );
      insertCell( cell );
        }
    }
      }
  }
    }

    // create an undo action
    if ( !doc()->undoLocked() )
    {
  UndoAction *undo = worker.createUndoAction( doc(), this, r );
        // test if the worker has an undo action
        if ( undo != 0L )
      doc()->addCommand( undo );
    }

    // complete rows selected ?
    if ( selected && isRowSelected() )
    {
  int row;
  for ( Cell* cell = d->cells.firstCell(); cell; cell = cell->nextCell() )
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
            // for type A now work on row formats
      for ( int i=m_rctSelection.top(); i<=m_rctSelection.bottom(); i++ )
      {
    RowFormat *rw=nonDefaultRowFormat(i);
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
  for ( Cell* cell = d->cells.firstCell(); cell; cell = cell->nextCell() )
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
      // for type A now work on column formats
      for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ )
      {
    ColumnFormat *cl=nonDefaultColumnFormat(i);
    worker.doWork( cl );
      }
      Cell *cell;
      for ( RowFormat* rw =d->rows.first(); rw; rw = rw->next() )
      {
    if ( !rw->isDefault() && worker.testCondition( rw ) )
    {
        for ( int i=m_rctSelection.left(); i<=m_rctSelection.right(); i++ )
        {
      cell = cellAt( i, rw->row() );
      // ### this if should be not necessary; cells are created
      //     before the undo object is created, aren't they?
      if ( cell == d->defaultCell )
      {
          cell = new Cell( this, i, rw->row() );
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
  Cell *cell;
  for ( int x = r.left(); x <= r.right(); x++ )
      for ( int y = r.top(); y <= r.bottom(); y++ )
      {
    cell = cellAt( x, y );
                if ( worker.testCondition( cell ) )
    {
        if ( worker.create_if_default && cell == d->defaultCell )
        {
      cell = new Cell( this, x, y );
      insertCell( cell );
        }
                    if ( cell != d->defaultCell )
      worker.doWork( cell, true, x, y );
    }
      }
        if ( worker.emit_signal )
      emit sig_updateView( this, r );
        return CellRegion;
    }
}

*/

Sheet::SelectionType Sheet::workOnCells( Selection* selectionInfo, CellWorker & worker )
{
  // see what is selected; if nothing, take marker position
  QRect selection(selectionInfo->selection());
  bool selected = !(selectionInfo->singleCellSelection());

  int top = selection.top();
  int left = selection.left();
  int bottom = selection.bottom();
  int right  = selection.right();

  Sheet::SelectionType result;

  doc()->emitBeginOperation();

  // create cells in rows if complete columns selected
  Cell * cell;
  Style * s = doc()->styleManager()->defaultStyle();

  if ( !worker.type_B && selected && util_isColumnSelected(selection) )
  {
    for ( RowFormat * rw = d->rows.first(); rw; rw = rw->next() )
    {
      if ( worker.testCondition( rw ) )
      {
        for ( int col = left; col <= right; ++col )
        {
          cell = nonDefaultCell( col, rw->row(), false, s );
        }
      }
    }
  }

  // create an undo action
  if ( !doc()->undoLocked() )
  {
    UndoAction * undo = worker.createUndoAction(doc(), this, selection);
    // test if the worker has an undo action
    if ( undo != 0L )
      doc()->addCommand( undo );
  }

  // complete rows selected ?
  if ( selected && util_isRowSelected(selection) )
  {
    for ( int row = top; row <= bottom; ++row )
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
      // for type A now work on row formats
      for ( int i = top; i <= bottom; ++i )
      {
        RowFormat * rw = nonDefaultRowFormat(i);
        worker.doWork( rw );
      }

      for ( int row = top; row <= bottom; ++row )
      {
        cell = getFirstCellRow( row );
        while ( cell )
        {
          if ( worker.testCondition( cell ) )
          {
            worker.doWork( cell, false, cell->column(), row );
          }
        cell = getNextCellRight( cell->column(), row );
        }
      }

    }
    result = CompleteRows;
  }
  // complete columns selected ?
  else if ( selected && util_isColumnSelected(selection) )
  {
    for ( int col = selection.left(); col <= right; ++col )
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
      // for type A now work on column formats
      for ( int i = left; i <= right; ++i )
      {
        ColumnFormat * cl = nonDefaultColumnFormat( i );
        worker.doWork( cl );
      }

      for ( RowFormat * rw = d->rows.first(); rw; rw = rw->next() )
      {
        if ( worker.testCondition( rw ) )
        {
          for ( int i = left; i <= right; ++i )
          {
            cell = nonDefaultCell( i, rw->row(), false, s );
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
    for ( int x = left; x <= right; ++x )
    {
      for ( int y = top; y <= bottom; ++y )
      {
        cell = cellAt( x, y );
        if ( worker.testCondition( cell ) )
        {
          if ( cell == d->defaultCell && worker.create_if_default )
          {
            cell = new Cell( this, s, x, y );
            insertCell( cell );
          }
          if ( cell != d->defaultCell )
          {
            // kdDebug() << "not default" << endl;
            worker.doWork( cell, true, x, y );
          }
        }
      }
    }
    result = CellRegion;
  }

  // emitEndOperation();
  emit sig_updateView( this );

  if (worker.emit_signal)
  {
    emit sig_updateView( this, selection );
  }

  return result;
}

struct SetSelectionFontWorker : public Sheet::CellWorkerTypeA
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
    bool testCondition( RowFormat* rw ) {
        return ( rw->hasProperty( Cell::PFont ) );
    }
    void doWork( RowFormat* rw ) {
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
    void doWork( ColumnFormat* cl ) {
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
    void prepareCell( Cell* cell ) {
  cell->clearProperty( Cell::PFont );
  cell->clearNoFallBackProperties( Cell::PFont );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
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

void Sheet::setSelectionFont( Selection* selectionInfo,
                                     const char *_font, int _size,
                                     signed char _bold, signed char _italic,
                                     signed char _underline, signed char _strike)
{
    SetSelectionFontWorker w( _font, _size, _bold, _italic, _underline, _strike );
    workOnCells( selectionInfo, w );
}

struct SetSelectionSizeWorker : public Sheet::CellWorkerTypeA {
    int _size, size;
    SetSelectionSizeWorker( int __size, int size2 ) : _size( __size ), size( size2 ) { }

    QString getUndoTitle() { return i18n("Change Font"); }
    bool testCondition( RowFormat* rw ) {
        return ( rw->hasProperty( Cell::PFont ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setTextFontSize( size + _size) ;
    }
    void doWork( ColumnFormat* cl ) {
  cl->setTextFontSize( size + _size );
    }
    void prepareCell( Cell* cell ) {
  cell->clearProperty( Cell::PFont );
  cell->clearNoFallBackProperties( Cell::PFont );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->setTextFontSize( size + _size );
        if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionSize( Selection* selectionInfo, int _size )
{
    int size;
    Cell* c;
    QPoint marker(selectionInfo->marker());
    c = cellAt(marker);
    size = c->textFontSize(marker.x(), marker.y());

    SetSelectionSizeWorker w( _size, size );
    workOnCells( selectionInfo, w );
}


struct SetSelectionUpperLowerWorker : public Sheet::CellWorker {
    int _type;
    Sheet   * _s;
    SetSelectionUpperLowerWorker( int type, Sheet * s )
      : Sheet::CellWorker( false ), _type( type ),  _s( s ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
  return new UndoChangeAreaTextCell( doc, sheet, r );
    }
    bool testCondition( Cell* c ) {
  return ( !c->value().isNumber() && !c->value().isBoolean() &&!c->isFormula() && !c->isDefault()
     && !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
     && !c->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int )
    {
  cell->setDisplayDirtyFlag();
  if ( _type == -1 )
      cell->setCellText( (cell->text().lower()));
  else if ( _type == 1 )
      cell->setCellText( (cell->text().upper()));
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionUpperLower( Selection* selectionInfo,
                                           int _type )
{
  SetSelectionUpperLowerWorker w( _type, this );
  workOnCells( selectionInfo, w );
}


struct SetSelectionFirstLetterUpperWorker : public Sheet::CellWorker
{
    Changes * _c;
    Sheet   * _s;
    SetSelectionFirstLetterUpperWorker( Sheet * s )
      : Sheet::CellWorker( false ),  _s( s ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
  return   new UndoChangeAreaTextCell( doc, sheet, r );
    }
    bool testCondition( Cell* c ) {
  return ( !c->value().isNumber() && !c->value().isBoolean() &&!c->isFormula() && !c->isDefault()
     && !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
     && !c->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int )
    {

  cell->setDisplayDirtyFlag();
  QString tmp = cell->text();
  int len = tmp.length();
  cell->setCellText( (tmp.at(0).upper()+tmp.right(len-1)) );
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionfirstLetterUpper( Selection* selectionInfo)
{
  SetSelectionFirstLetterUpperWorker w(  this );
  workOnCells( selectionInfo, w );
}


struct SetSelectionVerticalTextWorker : public Sheet::CellWorker {
    bool _b;
    SetSelectionVerticalTextWorker( bool b ) : Sheet::CellWorker( ), _b( b ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
        QString title=i18n("Vertical Text");
  return new UndoCellFormat( doc, sheet, r, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  cell->setVerticalText( _b );
  cell->setMultiRow( false );
  cell->setAngle( 0 );
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionVerticalText( Selection* selectionInfo,
                                             bool _b )
{
    SetSelectionVerticalTextWorker w( _b );
    workOnCells( selectionInfo, w );
}


struct SetSelectionCommentWorker : public Sheet::CellWorker {
    QString _comment;
    SetSelectionCommentWorker( QString comment ) : Sheet::CellWorker( ), _comment( comment ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
        QString title=i18n("Add Comment");
  return new UndoCellFormat( doc, sheet, r, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  cell->setComment( _comment );
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionComment( Selection* selectionInfo,
                                        const QString &_comment)
{
    SetSelectionCommentWorker w( _comment );
    workOnCells( selectionInfo, w );
}


struct SetSelectionAngleWorker : public Sheet::CellWorkerTypeA {
    int _value;
    SetSelectionAngleWorker( int value ) : _value( value ) { }

    QString getUndoTitle() { return i18n("Change Angle"); }
    UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ){
        return new UndoChangeAngle( doc, sheet, r );
    }

    bool testCondition( RowFormat* rw ) {
        return ( rw->hasProperty( Cell::PAngle ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setAngle( _value );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setAngle( _value );
    }
    void prepareCell( Cell* cell ) {
  cell->clearProperty( Cell::PAngle );
  cell->clearNoFallBackProperties( Cell::PAngle );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
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

void Sheet::setSelectionAngle( Selection* selectionInfo,
                                      int _value )
{
    SetSelectionAngleWorker w( _value );
    workOnCells( selectionInfo, w );
}

struct SetSelectionRemoveCommentWorker : public Sheet::CellWorker {
    SetSelectionRemoveCommentWorker( ) : Sheet::CellWorker( false ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
        QString title=i18n("Remove Comment");
  return new UndoCellFormat( doc, sheet, r, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  cell->setComment( "" );
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionRemoveComment( Selection* selectionInfo )
{
    if(areaIsEmpty(selectionInfo->selection(), Comment))
        return;
    SetSelectionRemoveCommentWorker w;
    workOnCells( selectionInfo, w );
}


struct SetSelectionTextColorWorker : public Sheet::CellWorkerTypeA {
    const QColor& tb_Color;
    SetSelectionTextColorWorker( const QColor& _tb_Color ) : tb_Color( _tb_Color ) { }

    QString getUndoTitle() { return i18n("Change Text Color"); }
    bool testCondition( RowFormat* rw ) {
        return ( rw->hasProperty( Cell::PTextPen ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setTextColor( tb_Color );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setTextColor( tb_Color );
    }
    void prepareCell( Cell* cell ) {
  cell->clearProperty( Cell::PTextPen );
  cell->clearNoFallBackProperties( Cell::PTextPen );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->setTextColor( tb_Color );
  if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionTextColor( Selection* selectionInfo,
                                          const QColor &tb_Color )
{
    SetSelectionTextColorWorker w( tb_Color );
    workOnCells( selectionInfo, w );
}


struct SetSelectionBgColorWorker : public Sheet::CellWorkerTypeA {
    const QColor& bg_Color;
    SetSelectionBgColorWorker( const QColor& _bg_Color ) : bg_Color( _bg_Color ) { }

    QString getUndoTitle() { return i18n("Change Background Color"); }
    bool testCondition( RowFormat* rw ) {
        return ( rw->hasProperty( Cell::PBackgroundColor ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setBgColor( bg_Color );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setBgColor( bg_Color );
    }
    void prepareCell( Cell* cell ) {
  cell->clearProperty( Cell::PBackgroundColor );
  cell->clearNoFallBackProperties( Cell::PBackgroundColor );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->setBgColor( bg_Color );
  if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionbgColor( Selection* selectionInfo,
                                        const QColor &bg_Color )
{
    SetSelectionBgColorWorker w( bg_Color );
    workOnCells( selectionInfo, w );
}


struct SetSelectionBorderColorWorker : public Sheet::CellWorker {
    const QColor& bd_Color;
    SetSelectionBorderColorWorker( const QColor& _bd_Color ) : Sheet::CellWorker( false ), bd_Color( _bd_Color ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
        QString title=i18n("Change Border Color");
  return new UndoCellFormat( doc, sheet, r, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int ) {
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

void Sheet::setSelectionBorderColor( Selection* selectionInfo,
                                            const QColor &bd_Color )
{
    SetSelectionBorderColorWorker w( bd_Color );
    workOnCells( selectionInfo, w );
}


void Sheet::setSeries( const QPoint &_marker, double start, double end, double step, Series mode, Series type)
{
  doc()->emitBeginOperation();

  QString cellText;

  int x,y; /* just some loop counters */

  /* the actual number of columns or rows that the series will span.
     i.e. this will count 3 cells for a single cell that spans three rows
  */
  int numberOfCells;
  if (end > start)
    numberOfCells = (int) ((end - start) / step + 1); /*initialize for linear*/
  else if ( end <start )
    numberOfCells = (int) ((start - end) / step + 1); /*initialize for linear*/
  else //equal ! => one cell fix infini loop
      numberOfCells = 1;
  if (type == Geometric)
  {
    /* basically, A(n) = start ^ n
     * so when does end = start ^ n ??
     * when n = ln(end) / ln(start)
     */
    numberOfCells = (int)( (log((double)end) / log((double)start)) +
           DBL_EPSILON) + 1;
  }

  Cell * cell = NULL;

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
  if ( mode == Column )
  {
    for ( y = _marker.y(); y <= (_marker.y() + numberOfCells - 1); y++ )
    {
      cell = cellAt( _marker.x(), y );

      if ( cell->isObscuringForced() )
      {
        /* case 2. */
        cell = cell->obscuringCells().first();
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
        cell = cell->obscuringCells().first();
        undoRegion.setTop(QMIN(undoRegion.top(), cell->row()));
      }
      numberOfCells += cell->extraXCells();
      x += cell->extraXCells();
    }
    undoRegion.setBottom( _marker.y() );
    undoRegion.setRight( x - 1 );
  }

  kdDebug() << "Saving undo information" << endl;

  if ( !doc()->undoLocked() )
  {
    UndoChangeAreaTextCell *undo = new
      UndoChangeAreaTextCell( doc(), this, undoRegion );
    doc()->addCommand( undo );
  }

  kdDebug() << "Saving undo information done" << endl;

  x = _marker.x();
  y = _marker.y();

  /* now we're going to actually loop through and set the values */
  double incr;
  Style * s = doc()->styleManager()->defaultStyle();
  if (step >= 0 && start < end)
  {
    for ( incr = start; incr <= end; )
    {
      cell = nonDefaultCell( x, y, false, s );

      if ( cell->isObscuringForced() )
      {
        cell = cell->obscuringCells().first();
      }

      //      cell->setCellText(cellText.setNum( incr ));

      cell->setNumber( incr );
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
      cell = nonDefaultCell( x, y, false, s );

      if (cell->isObscuringForced())
      {
        cell = cell->obscuringCells().first();
      }

      //      cell->setCellText(cellText.setNum( incr ));
      cell->setNumber( incr );
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
    for ( incr = start; incr <= end; )
    {
      cell = nonDefaultCell( x, y, false, s );

      if (cell->isObscuringForced())
      {
        cell = cell->obscuringCells().first();
      }

      //cell->setCellText(cellText.setNum( incr ));
      cell->setNumber( incr );
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
        //a step = 1 into geometric serie is not good
        //we don't increase value => infini loop
        if (step == 1)
            return;
      }
      else
      {
        kdDebug(36001) << "Error in Series::type" << endl;
        return;
      }
    }
  }
  //  doc()->emitEndOperation();
  emit sig_updateView( this );
}


struct SetSelectionPercentWorker : public Sheet::CellWorkerTypeA
{
    bool b;
    SetSelectionPercentWorker( bool _b ) : b( _b ) { }

    QString getUndoTitle() { return i18n("Format Percent"); }
    bool testCondition( RowFormat* ) {
        //TODO: no idea what to put here, now that factor's gone :(
        return ( true );
    }
    void doWork( RowFormat* rw ) {
  //rw->setPrecision( 0 );
  rw->setFormatType( b ? Percentage_format : Generic_format);
    }
    void doWork( ColumnFormat* cl ) {
  cl->setFormatType( b ? Percentage_format : Generic_format);
    }
    void prepareCell( Cell* cell ) {
  cell->clearProperty(Cell::PFormatType);
  cell->clearNoFallBackProperties( Cell::PFormatType );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->setFormatType( b ? Percentage_format : Generic_format);
  if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionPercent( Selection* selectionInfo, bool b )
{
    SetSelectionPercentWorker w( b );
    workOnCells( selectionInfo, w );
}

void Sheet::slotAreaModified (const QString &name)
{
  d->dependencies->areaModified (name);
}


void Sheet::refreshRemoveAreaName(const QString & _areaName)
{
  Cell * c = d->cells.firstCell();
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

void Sheet::refreshChangeAreaName(const QString & _areaName)
{
  Cell * c = d->cells.firstCell();
  QString tmp = "'" + _areaName + "'";
  for( ;c ; c = c->nextCell() )
  {
    if ( c->isFormula() )
    {
      if (c->text().find(tmp) != -1)
      {
        if ( !c->makeFormula() )
          kdError(36001) << "ERROR: Syntax ERROR" << endl;
        else
        {
          /* setting a cell calc dirty also sets it paint dirty */
          c->setCalcDirtyFlag();
        }
      }
    }
  }
}

void Sheet::changeCellTabName( QString const & old_name, QString const & new_name )
{
    Cell* c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if( c->isFormula() )
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

bool Sheet::shiftRow( const QRect &rect,bool makeUndo )
{
    UndoInsertCellRow * undo = 0;
    if ( !doc()->undoLocked()  &&makeUndo)
    {
        undo = new UndoInsertCellRow( doc(), this, rect );
        doc()->addCommand( undo );
    }

    bool res=true;
    bool result;
    for( int i=rect.top(); i<=rect.bottom(); i++ )
    {
        for( int j=0; j<=(rect.right()-rect.left()); j++ )
        {
            result = d->cells.shiftRow( QPoint(rect.left(),i) );
            if( !result )
                res=false;
        }
    }
    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
    {
        for(int i = rect.top(); i <= rect.bottom(); i++ )
            it.current()->changeNameCellRef( QPoint( rect.left(), i ), false,
                                             Sheet::ColumnInsert, name(),
                                             ( rect.right() - rect.left() + 1),
                                             undo);
    }
    refreshChart(QPoint(rect.left(),rect.top()), false, Sheet::ColumnInsert);
    refreshMergedCell();
    recalc();
    emit sig_updateView( this );

    return res;
}

bool Sheet::shiftColumn( const QRect& rect,bool makeUndo )
{
    UndoInsertCellCol * undo = 0;
    if ( !doc()->undoLocked()  &&makeUndo)
    {
        undo = new UndoInsertCellCol( doc(), this,rect);
        doc()->addCommand( undo );
    }

    bool res=true;
    bool result;
    for( int i =rect.left(); i<=rect.right(); i++ )
    {
        for( int j=0; j<=(rect.bottom()-rect.top()); j++ )
        {
            result = d->cells.shiftColumn( QPoint(i,rect.top()) );
            if(!result)
                res=false;
        }
    }

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
    {
        for(int i=rect.left();i<=rect.right();i++)
            it.current()->changeNameCellRef( QPoint( i, rect.top() ), false,
                                             Sheet::RowInsert, name(),
                                             ( rect.bottom() - rect.top() + 1 ),
                                             undo );
    }
    refreshChart(/*marker*/QPoint(rect.left(),rect.top()), false, Sheet::RowInsert);
    refreshMergedCell();
    recalc();
    emit sig_updateView( this );

    return res;
}

void Sheet::unshiftColumn( const QRect & rect,bool makeUndo )
{
    UndoRemoveCellCol * undo = 0;
    if ( !doc()->undoLocked() && makeUndo )
    {
        undo = new UndoRemoveCellCol( doc(), this, rect );
        doc()->addCommand( undo );
    }

    for(int i =rect.top();i<=rect.bottom();i++)
        for(int j=rect.left();j<=rect.right();j++)
               d->cells.remove(j,i);

    for(int i =rect.left();i<=rect.right();i++)
        for(int j=0;j<=(rect.bottom()-rect.top());j++)
                d->cells.unshiftColumn( QPoint(i,rect.top()) );

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        for(int i=rect.left();i<=rect.right();i++)
                it.current()->changeNameCellRef( QPoint( i, rect.top() ), false,
                                                 Sheet::RowRemove, name(),
                                                 ( rect.bottom() - rect.top() + 1 ),
                                                 undo );

    refreshChart( QPoint(rect.left(),rect.top()), false, Sheet::RowRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateView( this );
}

void Sheet::unshiftRow( const QRect & rect,bool makeUndo )
{
    UndoRemoveCellRow * undo = 0;
    if ( !doc()->undoLocked() && makeUndo )
    {
        undo = new UndoRemoveCellRow( doc(), this, rect );
        doc()->addCommand( undo );
    }
    for(int i =rect.top();i<=rect.bottom();i++)
        for(int j=rect.left();j<=rect.right();j++)
                d->cells.remove(j,i);

    for(int i =rect.top();i<=rect.bottom();i++)
        for(int j=0;j<=(rect.right()-rect.left());j++)
                d->cells.unshiftRow( QPoint(rect.left(),i) );

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        for(int i=rect.top();i<=rect.bottom();i++)
                it.current()->changeNameCellRef( QPoint( rect.left(), i ), false,
                                                 Sheet::ColumnRemove, name(),
                                                 ( rect.right() - rect.left() + 1 ),
                                                 undo);

    refreshChart(QPoint(rect.left(),rect.top()), false, Sheet::ColumnRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateView( this );
}

bool Sheet::insertColumn( int col, int nbCol, bool makeUndo )
{
    UndoInsertColumn * undo = 0;
    if ( !doc()->undoLocked() && makeUndo)
    {
        undo = new UndoInsertColumn( doc(), this, col, nbCol );
        doc()->addCommand( undo );
    }

    bool res=true;
    bool result;
    for( int i=0; i<=nbCol; i++ )
    {
        // Recalculate range max (minus size of last column)
        d->sizeMaxX -= columnFormat( KS_colMax )->dblWidth();

        result = d->cells.insertColumn( col );
        d->columns.insertColumn( col );
        if(!result)
            res = false;

        //Recalculate range max (plus size of new column)
        d->sizeMaxX += columnFormat( col+i )->dblWidth();
    }

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true,
                                         Sheet::ColumnInsert, name(),
                                         nbCol + 1, undo );

    //update print settings
    d->print->insertColumn( col, nbCol );

    refreshChart( QPoint( col, 1 ), true, Sheet::ColumnInsert );
    refreshMergedCell();
    recalc();
    emit sig_updateHBorder( this );
    emit sig_updateView( this );

    return res;
}

bool Sheet::insertRow( int row, int nbRow, bool makeUndo )
{
    UndoInsertRow *undo = 0;
    if ( !doc()->undoLocked() && makeUndo)
    {
        undo = new UndoInsertRow( doc(), this, row, nbRow );
        doc()->addCommand( undo );
    }

    bool res=true;
    bool result;
    for( int i=0; i<=nbRow; i++ )
    {
        // Recalculate range max (minus size of last row)
        d->sizeMaxY -= rowFormat( KS_rowMax )->dblHeight();

        result = d->cells.insertRow( row );
        d->rows.insertRow( row );
        if( !result )
            res = false;

        //Recalculate range max (plus size of new row)
        d->sizeMaxY += rowFormat( row )->dblHeight();
    }

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true,
                                         Sheet::RowInsert, name(),
                                         nbRow + 1, undo );

    //update print settings
    d->print->insertRow( row, nbRow );

    refreshChart( QPoint( 1, row ), true, Sheet::RowInsert );
    refreshMergedCell();
    recalc();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );

    return res;
}

void Sheet::removeColumn( int col, int nbCol, bool makeUndo )
{
    UndoRemoveColumn *undo = 0;
    if ( !doc()->undoLocked() && makeUndo)
    {
        undo = new UndoRemoveColumn( doc(), this, col, nbCol );
        doc()->addCommand( undo );
    }

    for( int i = 0; i <= nbCol; ++i )
    {
        // Recalculate range max (minus size of removed column)
        d->sizeMaxX -= columnFormat( col )->dblWidth();

        d->cells.removeColumn( col );
        d->columns.removeColumn( col );

        //Recalculate range max (plus size of new column)
        d->sizeMaxX += columnFormat( KS_colMax )->dblWidth();
    }

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true,
                                         Sheet::ColumnRemove, name(),
                                         nbCol + 1, undo );

    //update print settings
    d->print->removeColumn( col, nbCol );

    refreshChart( QPoint( col, 1 ), true, Sheet::ColumnRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}

void Sheet::removeRow( int row, int nbRow, bool makeUndo )
{
    UndoRemoveRow *undo = 0;
    if ( !doc()->undoLocked() && makeUndo )
    {
        undo = new UndoRemoveRow( doc(), this, row, nbRow );
        doc()->addCommand( undo );
    }

    for( int i=0; i<=nbRow; i++ )
    {
        // Recalculate range max (minus size of removed row)
        d->sizeMaxY -= rowFormat( row )->dblHeight();

        d->cells.removeRow( row );
        d->rows.removeRow( row );

        //Recalculate range max (plus size of new row)
        d->sizeMaxY += rowFormat( KS_rowMax )->dblHeight();
    }

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true,
                                         Sheet::RowRemove, name(),
                                         nbRow + 1, undo );

    //update print settings
    d->print->removeRow( row, nbRow );

    refreshChart( QPoint( 1, row ), true, Sheet::RowRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void Sheet::hideRow( int _row, int nbRow, QValueList<int>_list )
{
    if ( !doc()->undoLocked() )
    {
      UndoHideRow *undo ;
      if( nbRow!=-1 )
  undo= new UndoHideRow( doc(), this, _row, nbRow );
      else
  undo= new UndoHideRow( doc(), this, _row, nbRow, _list );
      doc()->addCommand( undo  );
    }

    RowFormat *rl;
    if( nbRow!=-1 )
    {
  for( int i=0; i<=nbRow; i++ )
  {
      rl=nonDefaultRowFormat( _row+i );
      rl->setHide(true);
  }
    }
    else
    {
  QValueList<int>::Iterator it;
  for( it = _list.begin(); it != _list.end(); ++it )
  {
      rl=nonDefaultRowFormat( *it );
      rl->setHide(true);
  }
    }
    emitHideRow();
}

void Sheet::emitHideRow()
{
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void Sheet::showRow( int _row, int nbRow, QValueList<int>_list )
{
    if ( !doc()->undoLocked() )
    {
      UndoShowRow *undo;
      if(nbRow!=-1)
        undo = new UndoShowRow( doc(), this, _row,nbRow );
      else
  undo = new UndoShowRow( doc(), this, _row,nbRow, _list );
      doc()->addCommand( undo );
    }

    RowFormat *rl;
    if( nbRow!=-1 )
      {
  for( int i=0; i<=nbRow; i++ )
    {
      rl=nonDefaultRowFormat( _row + i );
      rl->setHide( false );
    }
      }
    else
      {
  QValueList<int>::Iterator it;
  for( it = _list.begin(); it != _list.end(); ++it )
    {
      rl=nonDefaultRowFormat( *it );
      rl->setHide( false );
    }
      }
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}


void Sheet::hideColumn( int _col, int nbCol, QValueList<int>_list )
{
    if ( !doc()->undoLocked() )
    {
        UndoHideColumn *undo;
  if( nbCol!=-1 )
    undo= new UndoHideColumn( doc(), this, _col, nbCol );
  else
    undo= new UndoHideColumn( doc(), this, _col, nbCol, _list );
        doc()->addCommand( undo );
    }

    ColumnFormat *cl;
    if( nbCol != -1 )
    {
  for( int i=0; i<=nbCol; i++ )
  {
      cl=nonDefaultColumnFormat( _col + i );
      cl->setHide( true );
  }
    }
    else
    {
  QValueList<int>::Iterator it;
  for( it = _list.begin(); it != _list.end(); ++it )
  {
      cl=nonDefaultColumnFormat( *it );
      cl->setHide( true );
  }
    }
    emitHideColumn();
}

void Sheet::emitHideColumn()
{
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}


void Sheet::showColumn( int _col, int nbCol, QValueList<int>_list )
{
    if ( !doc()->undoLocked() )
    {
      UndoShowColumn *undo;
      if( nbCol != -1 )
  undo = new UndoShowColumn( doc(), this, _col, nbCol );
      else
  undo = new UndoShowColumn( doc(), this, _col, nbCol, _list );
      doc()->addCommand( undo );
    }

    ColumnFormat *cl;
    if( nbCol != -1 )
    {
      for( int i=0; i<=nbCol; i++ )
      {
  cl=nonDefaultColumnFormat( _col + i );
  cl->setHide( false );
      }
    }
    else
    {
       QValueList<int>::Iterator it;
       for( it = _list.begin(); it != _list.end(); ++it )
       {
     cl=nonDefaultColumnFormat( *it );
     cl->setHide( false );
       }
    }
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}


void Sheet::refreshChart(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref)
{
  Cell * c = d->cells.firstCell();
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

void Sheet::refreshMergedCell()
{
  Cell* c = d->cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    if(c->isForceExtraCells())
      c->forceExtraCells( c->column(), c->row(), c->extraXCells(), c->extraYCells() );
  }
}


void Sheet::changeNameCellRef( const QPoint & pos, bool fullRowOrColumn,
                                      ChangeRef ref, QString tabname, int nbCol,
                                      UndoInsertRemoveAction * undo )
{
  bool correctDefaultSheetName = (tabname == name()); // for cells without sheet ref (eg "A1")
  Cell* c = d->cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    if( c->isFormula() )
    {
      QString origText = c->text();
      unsigned int i = 0;
      bool error = false;
      QString newText;

      bool correctSheetName = correctDefaultSheetName;
      //bool previousCorrectSheetName = false;
      QChar origCh;
      for ( ; i < origText.length(); ++i )
      {
        origCh = origText[i];
        if ( origCh != ':' && origCh != '$' && !origCh.isLetter() )
        {
          newText += origCh;
          // Reset the "correct table indicator"
          correctSheetName = correctDefaultSheetName;
        }
        else // Letter or dollar : maybe start of cell name/range
          // (or even ':', like in a range - note that correctSheet is kept in this case)
        {
          // Collect everything that forms a name (cell name or sheet name)
          QString str;
          bool sheetNameFound = false; //Sheet names need spaces
          for( ; ( i < origText.length() ) &&  // until the end
                 (  ( origText[i].isLetter() || origText[i].isDigit() || origText[i] == '$' ) ||  // all text and numbers are welcome
                    ( sheetNameFound && origText[i].isSpace() ) ) //in case of a sheet name, we include spaces too
               ; ++i )
          {
            str += origText[i];
            if ( origText[i] == '!' )
              sheetNameFound = true;
          }
          // Was it a sheet name ?
          if ( origText[i] == '!' )
          {
            newText += str + '!'; // Copy it (and the '!')
            // Look for the sheet name right before that '!'
            correctSheetName = ( newText.right( tabname.length()+1 ) == tabname+"!" );
          }
          else // It must be a cell identifier
          {
            // Parse it
            Point point( str );
            if ( point.isValid() )
            {
              int col = point.pos.x();
              int row = point.pos.y();
              QString newPoint;

              // Update column
              if ( point.columnFixed )
                newPoint = '$';

              if( ref == ColumnInsert
                  && correctSheetName
                  && col + nbCol <= KS_colMax
                  && col >= pos.x()     // Column after the new one : +1
                  && ( fullRowOrColumn || row == pos.y() ) ) // All rows or just one
              {
                newPoint += Cell::columnName( col + nbCol );
              }
              else if( ref == ColumnRemove
                       && correctSheetName
                       && col > pos.x() // Column after the deleted one : -1
                       && ( fullRowOrColumn || row == pos.y() ) ) // All rows or just one
              {
                newPoint += Cell::columnName( col - nbCol );
              }
              else
                newPoint += Cell::columnName( col );

              // Update row
              if ( point.rowFixed )
                newPoint += '$';

              if( ref == RowInsert
                  && correctSheetName
                  && row + nbCol <= KS_rowMax
                  && row >= pos.y() // Row after the new one : +1
                  && ( fullRowOrColumn || col == pos.x() ) ) // All columns or just one
              {
                newPoint += QString::number( row + nbCol );
              }
              else if( ref == RowRemove
                       && correctSheetName
                       && row > pos.y() // Row after the deleted one : -1
                       && ( fullRowOrColumn || col == pos.x() ) ) // All columns or just one
              {
                newPoint += QString::number( row - nbCol );
              }
              else
                newPoint += QString::number( row );

              if( correctSheetName &&
                  ( ( ref == ColumnRemove
                      && col == pos.x() // Column is the deleted one : error
                      && ( fullRowOrColumn || row == pos.y() ) ) ||
                    ( ref == RowRemove
                      && row == pos.y() // Row is the deleted one : error
                      && ( fullRowOrColumn || col == pos.x() ) ) ||
                    ( ref == ColumnInsert
                      && col + nbCol > KS_colMax
                      && col >= pos.x()     // Column after the new one : +1
                      && ( fullRowOrColumn || row == pos.y() ) ) ||
                    ( ref == RowInsert
                      && row + nbCol > KS_rowMax
                      && row >= pos.y() // Row after the new one : +1
                      && ( fullRowOrColumn || col == pos.x() ) ) ) )
              {
                newPoint = "#" + i18n("Dependency") + "!";
                error = true;
              }

              newText += newPoint;
            }
            else // Not a cell ref
            {
              kdDebug(36001) << "Copying (unchanged) : '" << str << "'" << endl;
              newText += str;
            }
            // Copy the char that got us to stop
            if ( i < origText.length() ) {
              newText += origText[i];
              if( origText[i] != ':' )
                correctSheetName = correctDefaultSheetName;
            }
          }
        }
      }

      if ( error && undo != 0 ) //Save the original formula, as we cannot calculate the undo of broken formulas
      {
          QString formulaText = c->text();
          int origCol = c->column();
          int origRow = c->row();

          if ( ref == ColumnInsert && origCol >= pos.x() )
              origCol -= nbCol;
          if ( ref == RowInsert && origRow >= pos.y() )
              origRow -= nbCol;

          if ( ref == ColumnRemove && origCol >= pos.x() )
              origCol += nbCol;
          if ( ref == RowRemove && origRow >= pos.y() )
              origRow += nbCol;

          undo->saveFormulaReference( this, origCol, origRow, formulaText );
      }

      c->setCellText( newText );
    }
  }
}

#if 0
void Sheet::replace( const QString &_find, const QString &_replace, long options,
                            Canvas *canvas )
{
  Selection* selectionInfo = canvas->view()->selectionInfo();

    // Identify the region of interest.
    QRect region( selectionInfo->selection() );
    QPoint marker( selectionInfo->marker() );

    if (options & KReplaceDialog::SelectedText)
    {

        // Complete rows selected ?
        if ( util_isRowSelected(region) )
        {
        }
        // Complete columns selected ?
        else if ( util_isColumnSelected(region) )
        {
        }
    }
    else
    {
        // All cells.
        region.setCoords( 1, 1, d->maxRow, d->maxColumn );
    }

    // Create the class that handles all the actual replace stuff, and connect it to its
    // local slots.
    KReplace dialog( _find, _replace, options );
    QObject::connect(
        &dialog, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
        canvas, SLOT( highlight( const QString &, int, int, const QRect & ) ) );
    QObject::connect(
        &dialog, SIGNAL( replace( const QString &, int, int,int, const QRect & ) ),
        canvas, SLOT( replace( const QString &, int, int,int, const QRect & ) ) );

    // Now do the replacing...
    if ( !doc()->undoLocked() )
    {
        UndoChangeAreaTextCell *undo = new UndoChangeAreaTextCell( doc(), this, region );
        doc()->addCommand( undo );
    }

    QRect cellRegion( 0, 0, 0, 0 );
    bool bck = options & KFindDialog::FindBackwards;

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( options & KFindDialog::FromCursor ) {
        colStart = marker.x();
        rowStart =  marker.y();
    }
    Cell *cell;
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
#endif

void Sheet::borderBottom( Selection* selectionInfo,
                                 const QColor &_color )
{
  QRect selection( selectionInfo->selection() );

  QPen pen( _color,1,SolidLine);

  // Complete rows selected ?
  if ( util_isRowSelected(selection) )
  {
    if ( !doc()->undoLocked() )
    {
      QString title = i18n("Change Border");
      UndoCellFormat * undo =
        new UndoCellFormat( doc(), this, selection, title );
      doc()->addCommand( undo );
    }

    int row = selection.bottom();
    Cell * c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( Cell::PBottomBorder );
      c->clearNoFallBackProperties( Cell::PBottomBorder );

      c = getNextCellRight( c->column(), row );
    }

    RowFormat * rw = nonDefaultRowFormat(selection.bottom());
    rw->setBottomBorderPen(pen);

    emit sig_updateView( this );
    return;
  }
  // Complete columns selected ?
  else if ( util_isColumnSelected(selection) )
  {
    //nothing
    return;
  }
  else
  {
    if ( !doc()->undoLocked() )
    {
      QString title=i18n("Change Border");
      UndoCellFormat *undo =
        new UndoCellFormat( doc(), this, selection,title );
      doc()->addCommand( undo );
    }

    Cell* cell;
    int y = selection.bottom();
    for ( int x = selection.left(); x <= selection.right(); ++x )
    {
      cell = nonDefaultCell( x, y );
      if ( cell->isObscuringForced() )
        cell = cell->obscuringCells().first();
      cell->setBottomBorderPen( pen );
    }
    emit sig_updateView( this, selection );
  }
}

void Sheet::borderRight( Selection* selectionInfo,
                                const QColor &_color )
{
  QRect selection( selectionInfo->selection() );

  QPen pen( _color,1,SolidLine);
  // Complete rows selected ?
  if ( util_isRowSelected(selection) )
  {
    //nothing
    return;
  }
  // Complete columns selected ?
  else if ( util_isColumnSelected(selection) )
  {

    if ( !doc()->undoLocked() )
    {
      QString title = i18n("Change Border");
      UndoCellFormat * undo =
        new UndoCellFormat( doc(), this, selection, title );
      doc()->addCommand( undo );
    }

    int col = selection.right();
    Cell * c = getFirstCellColumn( col );
    while ( c )
    {
      if ( !c->isObscuringForced() )
      {
        c->clearProperty( Cell::PRightBorder );
        c->clearNoFallBackProperties( Cell::PRightBorder );
      }
      c = getNextCellDown( col, c->row() );
    }

    RowFormat * rw = d->rows.first();

    ColumnFormat * cl = nonDefaultColumnFormat(selection.right());
    cl->setRightBorderPen(pen);

    Cell * cell;
    rw = d->rows.first();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(Cell::PRightBorder)))
      {
        for(int i = selection.left(); i <= selection.right(); i++)
        {
          cell = nonDefaultCell( i, rw->row() );
          if ( cell->isObscuringForced() )
            cell = cell->obscuringCells().first();
          cell->setRightBorderPen(pen);
        }
      }
    }

    emit sig_updateView( this );
    return;
  }
  else
  {
    if ( !doc()->undoLocked() )
    {
      QString title=i18n("Change Border");
      UndoCellFormat *undo =
        new UndoCellFormat( doc(), this, selection, title );
      doc()->addCommand( undo );
    }

    Cell* cell;
    int x = selection.right();
    for ( int y = selection.top(); y <= selection.bottom(); y++ )
    {
      cell = nonDefaultCell( x, y );
      if ( cell->isObscuringForced() )
        cell = cell->obscuringCells().first();
      cell->setRightBorderPen(pen);
    }
    emit sig_updateView( this, selection );
  }
}

void Sheet::borderLeft( Selection* selectionInfo,
                               const QColor &_color )
{
  QString title = i18n("Change Border");
  QRect selection( selectionInfo->selection() );

  QPen pen( _color,1,SolidLine);

  // Complete columns selected ?
  if ( util_isColumnSelected(selection) )
  {
    RowFormat* rw =d->rows.first();

    if ( !doc()->undoLocked() )
    {
      UndoCellFormat *undo =
        new UndoCellFormat( doc(), this, selection, title );
      doc()->addCommand( undo );
    }

    int col = selection.left();
    Cell * c = getFirstCellColumn( col );
    while ( c )
    {
      c->clearProperty( Cell::PLeftBorder );
      c->clearNoFallBackProperties( Cell::PLeftBorder );

      c = getNextCellDown( col, c->row() );
    }


    ColumnFormat * cl = nonDefaultColumnFormat( col );
    cl->setLeftBorderPen(pen);

    Cell * cell;
    rw = d->rows.first();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(Cell::PLeftBorder)))
      {
        for(int i = selection.left(); i <= selection.right(); ++i)
        {
          cell = nonDefaultCell( i,  rw->row() );
          if ( cell->isObscuringForced() )
            continue;
          cell->setLeftBorderPen(pen);
        }
      }
    }

    emit sig_updateView( this );
    return;
  }
  else
  {
    if ( !doc()->undoLocked() )
    {
      UndoCellFormat *undo = new UndoCellFormat( doc(), this,
                                                               selection,title );
      doc()->addCommand( undo );
    }

    Cell* cell;
    int x = selection.left();
    for ( int y = selection.top(); y <= selection.bottom(); y++ )
    {
      cell = nonDefaultCell( x, y );
      if ( cell->isObscuringForced() )
        continue;
      cell->setLeftBorderPen(pen);
    }
    emit sig_updateView( this, selection );
  }
}

void Sheet::borderTop( Selection* selectionInfo,
                              const QColor &_color )
{
  /* duplicate code in kspread_dlg_layout.cc  That needs fixed at some point
     We need to save the code here and have the dialog code removed.
   */
  QRect selection( selectionInfo->selection() );

  QString title = i18n("Change Border");
  QPen pen( _color, 1, SolidLine);
  // Complete rows selected ?
  if ( util_isRowSelected(selection) )
  {
    if ( !doc()->undoLocked() )
    {
      UndoCellFormat * undo =
        new UndoCellFormat( doc(), this, selection, title );
      doc()->addCommand( undo );
    }

    int row = selection.top();
    Cell * c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( Cell::PTopBorder );
      c->clearNoFallBackProperties( Cell::PTopBorder );

      c = getNextCellRight( c->column(), row );
    }

    RowFormat * rw = nonDefaultRowFormat( row );
    rw->setTopBorderPen( pen );

    emit sig_updateView( this );
    return;
  }
  // Complete columns selected ? -- the top will just be row 1, then
  // so it's the same as in no rows/columns selected
  else
  {
    if ( !doc()->undoLocked() )
    {
      UndoCellFormat *undo =
        new UndoCellFormat( doc(), this, selection, title );
      doc()->addCommand( undo );
    }

    Cell* cell;
    int y = selection.top();
    for ( int x = selection.left(); x <= selection.right(); x++ )
    {
      cell = nonDefaultCell( x, y );
      if ( cell->isObscuringForced() )
        continue;
      cell->setTopBorderPen(pen);
    }
    emit sig_updateView( this, selection );
  }
}

void Sheet::borderOutline( Selection* selectionInfo,
                                  const QColor &_color )
{
  QRect selection( selectionInfo->selection() );

  if ( !doc()->undoLocked() )
  {
    QString title = i18n("Change Border");
    UndoCellFormat *undo = new UndoCellFormat( doc(), this,
                                                             selection, title );
    doc()->addCommand( undo );
  }

  QPen pen( _color, 1, SolidLine );

  // Complete rows selected ?
  if ( util_isRowSelected(selection) )
  {
    int row = selection.top();
    Cell * c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( Cell::PTopBorder );
      c->clearNoFallBackProperties( Cell::PTopBorder );

      c = getNextCellRight( c->column(), row );
    }

    row = selection.bottom();
    c = getFirstCellRow( row );
    while ( c )
    {
      c->clearProperty( Cell::PBottomBorder );
      c->clearNoFallBackProperties( Cell::PBottomBorder );

      c = getNextCellRight( c->column(), row );
    }

    RowFormat * rw = nonDefaultRowFormat( selection.top() );
    rw->setTopBorderPen(pen);
    rw=nonDefaultRowFormat(selection.bottom());
    rw->setBottomBorderPen(pen);
    Cell* cell;
    int bottom = selection.bottom();
    int left   = selection.left();
    for ( int y = selection.top(); y <= bottom; ++y )
    {
      cell = nonDefaultCell( left, y );
      if ( cell->isObscuringForced() )
        continue;
      cell->setLeftBorderPen( pen );
    }
    emit sig_updateView( this );
    return;
  }
  // Complete columns selected ?
  else if ( util_isColumnSelected(selection) )
  {
    int col = selection.left();
    Cell * c = getFirstCellColumn( col );
    while ( c )
    {
      c->clearProperty( Cell::PLeftBorder );
      c->clearNoFallBackProperties( Cell::PLeftBorder );

      c = getNextCellDown( col, c->row() );
    }

    col = selection.right();
    c = getFirstCellColumn( col );
    while ( c )
    {
      c->clearProperty( Cell::PRightBorder );
      c->clearNoFallBackProperties( Cell::PRightBorder );

      c = getNextCellDown( col, c->row() );
    }

    ColumnFormat *cl=nonDefaultColumnFormat(selection.left());
    cl->setLeftBorderPen(pen);
    cl=nonDefaultColumnFormat(selection.right());
    cl->setRightBorderPen(pen);
    Cell* cell;
    for ( int x = selection.left(); x <= selection.right(); x++ )
    {
      cell = nonDefaultCell( x, selection.top() );
      if ( cell->isObscuringForced() )
        continue;
      cell->setTopBorderPen( pen );
    }
    emit sig_updateView( this );
    return;
  }
  else
  {
    Cell* cell;
    for ( int x = selection.left(); x <= selection.right(); x++ )
    {
      cell = nonDefaultCell( x, selection.top() );
      if ( !cell->isObscuringForced() )
        cell->setTopBorderPen( pen );

      cell = nonDefaultCell( x, selection.bottom() );
      if ( cell->isObscuringForced() )
        cell = cell->obscuringCells().first();
      cell->setBottomBorderPen( pen );
    }
    for ( int y = selection.top(); y <= selection.bottom(); y++ )
    {
      cell = nonDefaultCell( selection.left(), y );
      if ( !cell->isObscuringForced() )
        cell->setLeftBorderPen( pen );

      cell = nonDefaultCell( selection.right(), y );
      if ( cell->isObscuringForced() )
        cell = cell->obscuringCells().first();
      cell->setRightBorderPen( pen );
    }
    emit sig_updateView( this, selection );
  }
}

struct SetSelectionBorderAllWorker : public Sheet::CellWorkerTypeA {
    QPen pen;
    SetSelectionBorderAllWorker( const QColor& color ) : pen( color, 1, QPen::SolidLine ) { }

    QString getUndoTitle() { return i18n("Change Border"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PRightBorder )
     || rw->hasProperty( Cell::PLeftBorder )
     || rw->hasProperty( Cell::PTopBorder )
     || rw->hasProperty( Cell::PBottomBorder ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setTopBorderPen( pen );
        rw->setRightBorderPen( pen );
        rw->setLeftBorderPen( pen );
        rw->setBottomBorderPen( pen );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setTopBorderPen( pen );
        cl->setRightBorderPen( pen );
        cl->setLeftBorderPen( pen );
        cl->setBottomBorderPen( pen );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PTopBorder );
  c->clearNoFallBackProperties( Cell::PTopBorder );
  c->clearProperty( Cell::PBottomBorder );
  c->clearNoFallBackProperties( Cell::PBottomBorder );
  c->clearProperty( Cell::PLeftBorder );
  c->clearNoFallBackProperties( Cell::PLeftBorder );
  c->clearProperty( Cell::PRightBorder );
  c->clearNoFallBackProperties( Cell::PRightBorder );
    }

  bool testCondition( Cell */* cell*/ ) { return true; }

    void doWork( Cell* cell, bool, int, int ) {
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

void Sheet::borderAll( Selection * selectionInfo,
                              const QColor & _color )
{
  if ( selectionInfo->singleCellSelection() )
  {
    borderOutline( selectionInfo, _color );
  }
  else
  {
    SetSelectionBorderAllWorker w( _color );
    workOnCells( selectionInfo, w );
  }
}

struct SetSelectionBorderRemoveWorker : public Sheet::CellWorkerTypeA {
    QPen pen;
    SetSelectionBorderRemoveWorker() : pen( Qt::black, 1, Qt::NoPen  ) { }
    QString getUndoTitle() { return i18n("Change Border"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PRightBorder )
     || rw->hasProperty( Cell::PLeftBorder )
     || rw->hasProperty( Cell::PTopBorder )
     || rw->hasProperty( Cell::PBottomBorder )
     || rw->hasProperty( Cell::PFallDiagonal )
     || rw->hasProperty( Cell::PGoUpDiagonal ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setTopBorderPen( pen );
        rw->setRightBorderPen( pen );
        rw->setLeftBorderPen( pen );
        rw->setBottomBorderPen( pen);
        rw->setFallDiagonalPen( pen );
        rw->setGoUpDiagonalPen (pen );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setTopBorderPen( pen );
        cl->setRightBorderPen( pen );
        cl->setLeftBorderPen( pen );
        cl->setBottomBorderPen( pen);
        cl->setFallDiagonalPen( pen );
        cl->setGoUpDiagonalPen (pen );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PTopBorder );
  c->clearNoFallBackProperties( Cell::PTopBorder );
  c->clearProperty( Cell::PLeftBorder );
  c->clearNoFallBackProperties( Cell::PLeftBorder );
  c->clearProperty( Cell::PRightBorder );
  c->clearNoFallBackProperties( Cell::PRightBorder );
  c->clearProperty( Cell::PBottomBorder );
  c->clearNoFallBackProperties( Cell::PBottomBorder );
  c->clearProperty( Cell::PFallDiagonal );
  c->clearNoFallBackProperties( Cell::PFallDiagonal );
  c->clearProperty( Cell::PGoUpDiagonal );
  c->clearNoFallBackProperties( Cell::PGoUpDiagonal );
    }

    bool testCondition(Cell* /*cell*/ ){ return true; }

    void doWork( Cell* cell, bool, int, int ) {
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


void Sheet::borderRemove( Selection* selectionInfo )
{
    SetSelectionBorderRemoveWorker w;
    workOnCells( selectionInfo, w );
}


void Sheet::sortByRow( const QRect &area, int ref_row, SortingOrder mode )
{
  Point point;
  point.sheet = this;
  point.sheetName = d->name;
  point.pos = area.topLeft();
  point.columnFixed = false;
  point.rowFixed = false;

  sortByRow( area, ref_row, 0, 0, mode, mode, mode, 0, false, false, point,true );
}

void Sheet::sortByColumn( const QRect &area, int ref_column, SortingOrder mode )
{
  Point point;
  point.sheet = this;
  point.sheetName = d->name;
  point.pos = area.topLeft();
  point.columnFixed = false;
  point.rowFixed = false;

  sortByColumn( area, ref_column, 0, 0, mode, mode, mode, 0, false, false,
                point,true );
}

void Sheet::checkCellContent(Cell * cell1, Cell * cell2, int & ret)
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

void Sheet::sortByRow( const QRect &area, int key1, int key2, int key3,
                              SortingOrder order1, SortingOrder order2,
                              SortingOrder order3,
                              QStringList const * firstKey, bool copyFormat,
                              bool headerRow, Point const & outputPoint, bool respectCase )
{
  QRect r( area );
  Map::respectCase = respectCase;
  Q_ASSERT( order1 == Increase || order1 == Decrease );

  // It may not happen that entire columns are selected.
  Q_ASSERT( util_isColumnSelected(r) == false );

  // Are entire rows selected ?
  if ( util_isRowSelected(r) )
  {
    r.setLeft( KS_colMax );
    r.setRight( 0 );

    // Determine a correct left and right.
    // Iterate over all cells to find out which cells are
    // located in the selected rows.
    for ( int row = r.top(); row <= r.bottom(); ++row )
    {
      Cell * c = getFirstCellRow( row );
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

    // Any cells to sort here ?
    if ( r.right() < r.left() )
    {
        Map::respectCase = true;
        return;
    }
  }

  QRect target( outputPoint.pos.x(), outputPoint.pos.y(), r.width(), r.height() );

  doc()->emitBeginOperation();

  if ( !doc()->undoLocked() )
  {
    UndoSort *undo = new UndoSort( doc(), this, target );
    doc()->addCommand( undo );
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
                   targetLeft + x, targetTop + y, copyFormat );
      }
    }
  }

  // Sorting algorithm: David's :). Well, I guess it's called minmax or so.
  // For each column, we look for all cells right hand of it and we find the one to swap with it.
  // Much faster than the awful bubbleSort...
  Cell * cell;
  Cell * cell1;
  Cell * cell2;
  Cell * bestCell;
  int status = 0;

  for ( int d = target.left();  d <= target.right(); ++d )
  {
    cell1 = cellAt( d, key1 );
    if ( cell1->isObscured() && cell1->isObscuringForced() )
    {
      Cell* obscuring = cell1->obscuringCells().first();
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

            Cell * cell22 = cellAt( x, key2 );
            Cell * bestCell2 = cellAt( bestX, key2 );

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

            if ( (order2 == Increase && *cell22 < *bestCell2)
                 || (order2 == Decrease && *cell22 > *bestCell2) )
            {
              bestCell = cell2;
              bestX = x;
              continue;
            }
            else if ( (order2 == Increase && *cell22 > *bestCell2)
                      || (order2 == Decrease && *cell22 < *bestCell2) )
            {
              // already in right order
              continue;
            }
            else
            {
              // they are equal, check 3rd key
              if (key3 <= 0)
                continue;

              Cell * cell23 = cellAt( x, key3 );
              Cell * bestCell3 = cellAt( bestX, key3 );

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
              if ( (order3 == Increase && *cell23 < *bestCell3)
                   || (order3 == Decrease && *cell23 > *bestCell3) )
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
        Cell * cell22 = cellAt( d, key2 );
        Cell * bestCell2 = cellAt( x, key2 );

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
          Cell * cell23 = cellAt( d, key3 );
          Cell * bestCell3 = cellAt( x, key3 );

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
          swapCells( d, y, bestX, y, copyFormat );
      }
      if (key3 > 0)
        swapCells( d, key3, bestX, key3, copyFormat );
      if (key2 > 0)
        swapCells( d, key2, bestX, key2, copyFormat );
      swapCells( d, key1, bestX, key1, copyFormat );
    }
  } // for (d = ...; ...; ++d)
  Map::respectCase = true;
  //  doc()->emitEndOperation();
  emit sig_updateView( this );
}

void Sheet::sortByColumn( const QRect &area, int key1, int key2, int key3,
                                 SortingOrder order1, SortingOrder order2,
                                 SortingOrder order3,
                                 QStringList const * firstKey, bool copyFormat,
                                 bool headerRow,
                                 Point const & outputPoint, bool respectCase )
{
  QRect r( area );
  Map::respectCase = respectCase;

  Q_ASSERT( order1 == Increase || order1 == Decrease );

  // It may not happen that entire rows are selected.
  Q_ASSERT( util_isRowSelected(r) == false );

  // Are entire columns selected ?
  if ( util_isColumnSelected(r) )
  {
    r.setTop( KS_rowMax );
    r.setBottom( 0 );

    // Determine a correct top and bottom.
    // Iterate over all cells to find out which cells are
    // located in the selected columns.
    for ( int col = r.left(); col <= r.right(); ++col )
    {
      Cell * c = getFirstCellColumn( col );
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

    // Any cells to sort here ?
    if ( r.bottom() < r.top() )
    {
        Map::respectCase = true;
      return;
    }
  }
  QRect target( outputPoint.pos.x(), outputPoint.pos.y(), r.width(), r.height() );

  if ( !doc()->undoLocked() )
  {
    UndoSort *undo = new UndoSort( doc(), this, target );
    doc()->addCommand( undo );
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
                   targetLeft + x, targetTop + y, copyFormat );
      }
    }
  }

  // Sorting algorithm: David's :). Well, I guess it's called minmax or so.
  // For each row, we look for all rows under it and we find the one to swap with it.
  // Much faster than the awful bubbleSort...
  // Torben: Asymptotically it is alltogether O(n^2) :-)

  Cell * cell;
  Cell * cell1;
  Cell * cell2;
  Cell * bestCell;
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
      Cell* obscuring = cell1->obscuringCells().first();
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
            Cell * cell22 = cellAt( key2, d );
            Cell * bestCell2 = cellAt( key2, y );

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
              Cell * cell23 = cellAt( key3, d );
              Cell * bestCell3 = cellAt( key3, y );

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
        Cell * cell22 = cellAt( key2, y );
        Cell * bestCell2 = cellAt( key2, bestY );

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

        if ( (order2 == Increase && *cell22 < *bestCell2)
             || (order2 == Decrease && *cell22 > *bestCell2) )
        {
          bestCell = cell2;
          bestY = y;
          continue;
        }
        else if ( (order2 == Increase && *cell22 > *bestCell2)
                  || (order2 == Decrease && *cell22 < *bestCell2) )
        {
          continue;
        }
        else
        {
          // they are equal, check 3rd key
          if (key3 == 0)
            continue;
          Cell * cell23 = cellAt( key3, y );
          Cell * bestCell3 = cellAt( key3, bestY );

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

          if ( (order3 == Increase && *cell23 < *bestCell3)
               || (order3 == Decrease && *cell23 > *bestCell3) )
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
          swapCells( x, d, x, bestY, copyFormat );
      }
      if (key3 > 0)
        swapCells( key3, d, key3, bestY, copyFormat );
      if (key2 > 0)
        swapCells( key2, d, key2, bestY, copyFormat );
      swapCells( key1, d, key1, bestY, copyFormat );
    }
  } // for (d = ...; ...; ++d)
  // doc()->emitEndOperation();
  Map::respectCase = true;
  emit sig_updateView( this );
}

// from - to - copyFormat
void Sheet::copyCells( int x1, int y1, int x2, int y2, bool cpFormat )
{
  Cell * sourceCell = cellAt( x1, y1 );
  Cell * targetCell = cellAt( x2, y2 );

  if ( sourceCell->isDefault() && targetCell->isDefault())
  {
    // if the source and target is default there is nothing to copy
    return;
  }

  targetCell = nonDefaultCell(x2, y2);

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

  if (cpFormat)
  {
    targetCell->copyFormat( sourceCell );
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

void Sheet::swapCells( int x1, int y1, int x2, int y2, bool cpFormat )
{
  Cell * ref1 = cellAt( x1, y1 );
  Cell * ref2 = cellAt( x2, y2 );

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
  // information. Imagine sorting in a sheet. Swapping
  // the format while sorting is not what you would expect
  // as a user.
  if (!ref1->isFormula() && !ref2->isFormula())
  {
    Cell *tmp = new Cell( this, -1, -1 );

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
        ref1->setCellText(ref2->text());
        ref2->setCellText(ref2->decodeFormula(d));
        ref2->setCalcDirtyFlag();
        ref2->calc(false);
      }
      else
        if (!ref1->isFormula() && ref2->isFormula() )
        {
          QString d = ref2->encodeFormula();
          ref2->setCellText(ref1->text());
          ref1->setCellText(ref1->decodeFormula(d));
          ref1->setCalcDirtyFlag();
          ref1->calc(false);
        }

  if (cpFormat)
  {
    Format::Align a = ref1->align( ref1->column(), ref1->row() );
    ref1->setAlign( ref2->align( ref2->column(), ref2->row() ) );
    ref2->setAlign(a);

    Format::AlignY ay = ref1->alignY( ref1->column(), ref1->row() );
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

    Format::FloatFormat f = ref1->floatFormat( ref1->column(), ref1->row() );
    ref1->setFloatFormat( ref2->floatFormat( ref2->column(), ref2->row() ) );
    ref2->setFloatFormat(f);

    Format::FloatColor c = ref1->floatColor( ref1->column(), ref1->row() );
    ref1->setFloatColor( ref2->floatColor( ref2->column(), ref2->row() ) );
    ref2->setFloatColor(c);

    bool multi = ref1->multiRow( ref1->column(), ref1->row() );
    ref1->setMultiRow( ref2->multiRow( ref2->column(), ref2->row() ) );
    ref2->setMultiRow(multi);

    bool vert = ref1->verticalText( ref1->column(), ref1->row() );
    ref1->setVerticalText( ref2->verticalText( ref2->column(), ref2->row() ) );
    ref2->setVerticalText(vert);

    bool print = ref1->getDontprintText( ref1->column(), ref1->row() );
    ref1->setDontPrintText( ref2->getDontprintText( ref2->column(), ref2->row() ) );
    ref2->setDontPrintText(print);

    double ind = ref1->getIndent( ref1->column(), ref1->row() );
    ref1->setIndent( ref2->getIndent( ref2->column(), ref2->row() ) );
    ref2->setIndent( ind );

    QValueList<Conditional> conditionList = ref1->conditionList();
    ref1->setConditionList(ref2->conditionList());
    ref2->setConditionList(conditionList);

    QString com = ref1->comment( ref1->column(), ref1->row() );
    ref1->setComment( ref2->comment( ref2->column(), ref2->row() ) );
    ref2->setComment(com);

    int angle = ref1->getAngle( ref1->column(), ref1->row() );
    ref1->setAngle( ref2->getAngle( ref2->column(), ref2->row() ) );
    ref2->setAngle(angle);

    FormatType form = ref1->getFormatType( ref1->column(), ref1->row() );
    ref1->setFormatType( ref2->getFormatType( ref2->column(), ref2->row() ) );
    ref2->setFormatType(form);
  }
}

void Sheet::refreshPreference()
{
  if ( getAutoCalc() )
    recalc();

  emit sig_updateHBorder( this );
  emit sig_updateView( this );
}


bool Sheet::areaIsEmpty(const QRect &area, TestType _type)
{
    // Complete rows selected ?
    if ( util_isRowSelected(area) )
    {
        for ( int row = area.top(); row <= area.bottom(); ++row )
        {
            Cell * c = getFirstCellRow( row );
            while ( c )
            {
                if ( !c->isObscuringForced())
                {
                    switch( _type )
                    {
                    case Text :
                        if ( !c->text().isEmpty())
                            return false;
                        break;
                    case Validity:
                        if ( c->getValidity(0))
                            return false;
                        break;
                    case Comment:
                        if ( !c->comment(c->column(), row).isEmpty())
                            return false;
                        break;
                    case ConditionalCellAttribute:
                        if ( c->conditionList().count()> 0)
                            return false;
                        break;
                    }
                }

                c = getNextCellRight( c->column(), row );
            }
        }
    }
    // Complete columns selected ?
    else if ( util_isColumnSelected(area) )
    {
        for ( int col = area.left(); col <= area.right(); ++col )
        {
            Cell * c = getFirstCellColumn( col );
            while ( c )
            {
                if ( !c->isObscuringForced() )
                {
                    switch( _type )
                    {
                    case Text :
                        if ( !c->text().isEmpty())
                            return false;
                        break;
                    case Validity:
                        if ( c->getValidity(0))
                            return false;
                        break;
                    case Comment:
                        if ( !c->comment(col, c->row()).isEmpty())
                            return false;
                        break;
                    case ConditionalCellAttribute:
                        if ( c->conditionList().count()> 0)
                            return false;
                        break;
                    }
                }

                c = getNextCellDown( col, c->row() );
            }
        }
    }
    else
    {
        Cell * cell;

        int right  = area.right();
        int bottom = area.bottom();
        for ( int x = area.left(); x <= right; ++x )
            for ( int y = area.top(); y <= bottom; ++y )
            {
                cell = cellAt( x, y );
                if (!cell->isObscuringForced() )
                {
                    switch( _type )
                    {
                    case Text :
                        if ( !cell->text().isEmpty())
                            return false;
                        break;
                    case Validity:
                        if ( cell->getValidity(0))
                            return false;
                        break;
                    case Comment:
                        if ( !cell->comment(x, y).isEmpty())
                            return false;
                        break;
                    case ConditionalCellAttribute:
                        if ( cell->conditionList().count()> 0)
                            return false;
                        break;
                    }
                }
            }
    }
    return true;
}

struct SetSelectionMultiRowWorker : public Sheet::CellWorker
{
  bool enable;
  SetSelectionMultiRowWorker( bool _enable )
    : Sheet::CellWorker( ), enable( _enable ) { }

  class UndoAction* createUndoAction( Doc * doc, Sheet * sheet, QRect & r )
  {
    QString title = i18n("Multirow");
    return new UndoCellFormat( doc, sheet, r, title );
  }

  bool testCondition( Cell * cell )
  {
    return ( !cell->isObscuringForced() );
  }

  void doWork( Cell * cell, bool, int, int )
  {
    cell->setDisplayDirtyFlag();
    cell->setMultiRow( enable );
    cell->setVerticalText( false );
    cell->setAngle( 0 );
    cell->clearDisplayDirtyFlag();
  }
};

void Sheet::setSelectionMultiRow( Selection* selectionInfo,
                                         bool enable )
{
    SetSelectionMultiRowWorker w( enable );
    workOnCells( selectionInfo, w );
}


struct SetSelectionAlignWorker
  : public Sheet::CellWorkerTypeA
{
  KSpread::Format::Align _align;
  SetSelectionAlignWorker( KSpread::Format::Align align ) : _align( align ) {}
    QString getUndoTitle() { return i18n("Change Horizontal Alignment"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PAlign ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setAlign( _align );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setAlign( _align );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PAlign );
  c->clearNoFallBackProperties( Cell::PAlign );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->setAlign( _align );
  if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};

QString Sheet::guessColumnTitle(QRect& area, int col)
{
  //Verify range
  Range rg;
  rg.setRange(area);
  rg.sheet=this;

  if ( (!rg.isValid()) || (col < area.left()) || (col > area.right()))
    return QString();

  //The current guess logic is fairly simple - if the top row of the given area
  //appears to contain headers (ie. there is text in each column) the text in the column at
  //the top row of the area is returned.

/*  for (int i=area.left();i<=area.right();i++)
  {
    Value cellValue=value(i,area.top());

    if (!cellValue.isString())
      return QString();
  }*/

  Value cellValue=value(col,area.top());
  return cellValue.asString();
}

QString Sheet::guessRowTitle(QRect& area, int row)
{
  //Verify range
  Range rg;
  rg.setRange(area);
  rg.sheet=this;

  if ( (!rg.isValid()) || (row < area.top()) || (row > area.bottom()) )
    return QString();

  //The current guess logic is fairly simple - if the leftmost column of the given area
  //appears to contain headers (ie. there is text in each row) the text in the row at
  //the leftmost column of the area is returned.
  /*for (int i=area.top();i<=area.bottom();i++)
  {
    Value cellValue=value(area.left(),i);

    if (!cellValue.isString())
      return QString();
  }*/

  Value cellValue=value(area.left(),row);
  return cellValue.asString();
}

void Sheet::setSelectionAlign( Selection* selectionInfo,
                                      KSpread::Format::Align _align )
{
    SetSelectionAlignWorker w( _align );
    workOnCells( selectionInfo, w );
}


struct SetSelectionAlignYWorker : public Sheet::CellWorkerTypeA {
    KSpread::Format::AlignY _alignY;
    SetSelectionAlignYWorker( KSpread::Format::AlignY alignY )
      : _alignY( alignY )
    {
      kdDebug() << "AlignY: " << _alignY << endl;
    }
    QString getUndoTitle() { return i18n("Change Vertical Alignment"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PAlignY ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setAlignY( _alignY );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setAlignY( _alignY );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PAlignY );
  c->clearNoFallBackProperties( Cell::PAlignY );
    }
    bool testCondition( Cell* cell ) {
        kdDebug() << "testCondition" << endl;
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
        kdDebug() << "cell->setAlignY: " << _alignY << endl;
  cell->setAlignY( _alignY );
  if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};


void Sheet::setSelectionAlignY( Selection* selectionInfo,
                                KSpread::Format::AlignY _alignY )
{
  kdDebug() << "setSelectionAlignY: " << _alignY << endl;
    SetSelectionAlignYWorker w( _alignY );
    workOnCells( selectionInfo, w );
}


struct SetSelectionPrecisionWorker : public Sheet::CellWorker {
    int _delta;
    SetSelectionPrecisionWorker( int delta ) : Sheet::CellWorker( ), _delta( delta ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
        QString title=i18n("Change Precision");
  return new UndoCellFormat( doc, sheet, r, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  if ( _delta == 1 )
      cell->incPrecision();
  else
      cell->decPrecision();
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionPrecision( Selection* selectionInfo,
                                          int _delta )
{
    SetSelectionPrecisionWorker w( _delta );
    workOnCells( selectionInfo, w );
}

struct SetSelectionStyleWorker : public Sheet::CellWorkerTypeA
{
  Style * m_style;
  SetSelectionStyleWorker( Style * style )
    : m_style( style )
  {
  }

  QString getUndoTitle()
  {
    return i18n("Apply Style");
  }

  void doWork( RowFormat* rw )
  {
    rw->setStyle( m_style );
  }

  void doWork( ColumnFormat* cl )
  {
    cl->setStyle( m_style );
  }

  bool testCondition( Cell* cell )
  {
    return ( !cell->isObscuringForced() && cell->kspreadStyle() != m_style );
  }

  void doWork( Cell* cell, bool cellRegion, int, int )
  {
    if ( cellRegion )
      cell->setDisplayDirtyFlag();

    cell->setStyle( m_style );

    if ( cellRegion )
      cell->clearDisplayDirtyFlag();
  }
};


void Sheet::setSelectionStyle( Selection * selectionInfo, Style * style )
{
    SetSelectionStyleWorker w( style );
    workOnCells( selectionInfo, w );
}

struct SetSelectionMoneyFormatWorker : public Sheet::CellWorkerTypeA
{
    bool b;
    Doc *m_pDoc;
    SetSelectionMoneyFormatWorker( bool _b,Doc* _doc ) : b( _b ), m_pDoc(_doc) { }
    QString getUndoTitle() { return i18n("Format Money"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PFormatType )
     || rw->hasProperty( Cell::PPrecision ) );
    }
    void doWork( RowFormat* rw ) {
  rw->setFormatType( b ? Money_format : Generic_format );
  rw->setPrecision( b ? m_pDoc->locale()->fracDigits() : 0 );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setFormatType( b ? Money_format : Generic_format );
  cl->setPrecision( b ? m_pDoc->locale()->fracDigits() : 0 );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PPrecision );
  c->clearNoFallBackProperties( Cell::PPrecision );
  c->clearProperty( Cell::PFormatType );
  c->clearNoFallBackProperties( Cell::PFormatType );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->setFormatType( b ? Money_format : Generic_format );
  cell->setPrecision( b ?  m_pDoc->locale()->fracDigits() : 0 );
  if ( cellRegion )
      cell->clearDisplayDirtyFlag();
    }
};


void Sheet::setSelectionMoneyFormat( Selection* selectionInfo,
                                            bool b )
{
    SetSelectionMoneyFormatWorker w( b,doc() );
    workOnCells( selectionInfo, w );
}


struct IncreaseIndentWorker : public Sheet::CellWorkerTypeA {
    double   tmpIndent;
    double   valIndent;

    IncreaseIndentWorker( double _tmpIndent, double _valIndent )
  : tmpIndent( _tmpIndent ), valIndent( _valIndent ) { }

    QString  getUndoTitle() { return i18n("Increase Indent"); }
    bool     testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PIndent ) );
    }

    void doWork( RowFormat* rw ) {
  rw->setIndent( tmpIndent+valIndent );
  //rw->setAlign( Cell::Left );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setIndent( tmpIndent+valIndent );
  //cl->setAlign( Cell::Left );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PIndent );
  c->clearNoFallBackProperties( Cell::PIndent );
  //c->clearProperty( Cell::PAlign );
  //c->clearNoFallBackProperties( Cell::PAlign );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int x, int y ) {
  if ( cellRegion ) {
      if(cell->align(x,y)!=Cell::Left)
      {
    //cell->setAlign(Cell::Left);
    //cell->setIndent( 0.0 );
      }
      cell->setDisplayDirtyFlag();
      cell->setIndent( /* ### ??? --> */ cell->getIndent(x,y) /* <-- */ +valIndent );
      cell->clearDisplayDirtyFlag();
  } else {
      cell->setIndent( tmpIndent+valIndent);
      //cell->setAlign( Cell::Left);
  }
    }
};


void Sheet::increaseIndent(Selection* selectionInfo)
{
    QPoint       marker(selectionInfo->marker());
    double       valIndent = doc()->getIndentValue();
    Cell *c         = cellAt( marker );
    double       tmpIndent = c->getIndent( marker.x(), marker.y() );

    IncreaseIndentWorker  w( tmpIndent, valIndent );
    workOnCells( selectionInfo, w );
}


struct DecreaseIndentWorker : public Sheet::CellWorkerTypeA {
    double tmpIndent, valIndent;
    DecreaseIndentWorker( double _tmpIndent, double _valIndent ) : tmpIndent( _tmpIndent ), valIndent( _valIndent ) { }
    QString getUndoTitle() { return i18n("Decrease Indent"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Cell::PIndent ) );
    }
    void doWork( RowFormat* rw ) {
        rw->setIndent( QMAX( 0.0, tmpIndent - valIndent ) );
    }
    void doWork( ColumnFormat* cl ) {
        cl->setIndent( QMAX( 0.0, tmpIndent - valIndent ) );
    }
    void prepareCell( Cell* c ) {
  c->clearProperty( Cell::PIndent );
  c->clearNoFallBackProperties( Cell::PIndent );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscuringForced() );
    }
    void doWork( Cell* cell, bool cellRegion, int x, int y ) {
  if ( cellRegion ) {
      cell->setDisplayDirtyFlag();
      cell->setIndent( QMAX( 0.0, cell->getIndent( x, y ) - valIndent ) );
      cell->clearDisplayDirtyFlag();
  } else {
      cell->setIndent( QMAX( 0.0, tmpIndent - valIndent ) );
  }
    }
};


void Sheet::decreaseIndent( Selection* selectionInfo )
{
    double valIndent = doc()->getIndentValue();
    QPoint marker(selectionInfo->marker());
    Cell* c = cellAt( marker );
    double tmpIndent = c->getIndent( marker.x(), marker.y() );

    DecreaseIndentWorker w( tmpIndent, valIndent );
    workOnCells( selectionInfo, w );
}


int Sheet::adjustColumnHelper( Cell * c, int _col, int _row )
{
    double long_max = 0.0;
    c->calculateTextParameters( painter(), _col, _row );
    if ( c->textWidth() > long_max )
    {
        double indent = 0.0;
        int a = c->align( c->column(), c->row() );
        if ( a == Cell::Undefined )
        {
            if ( c->value().isNumber() || c->isDate() || c->isTime())
                a = Cell::Right;
            else
                a = Cell::Left;
        }

        if ( a == Cell::Left )
            indent = c->getIndent( c->column(), c->row() );
        long_max = indent + c->textWidth()
                   + c->leftBorderWidth( c->column(), c->row() )
                   + c->rightBorderWidth( c->column(), c->row() );
    }
    return (int)long_max;
}

int Sheet::adjustColumn( Selection* selectionInfo, int _col )
{
  QRect selection(selectionInfo->selection());
  double long_max = 0.0;
  if ( _col == -1 )
  {
    if ( util_isColumnSelected(selection) )
    {
      for ( int col = selection.left(); col <= selection.right(); ++col )
      {
        Cell * c = getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured() )
          {
              long_max = QMAX( adjustColumnHelper( c, col, c->row() ), long_max );
          } // if !isEmpty...
          c = getNextCellDown( col, c->row() );
        }
      }
    }
  }
  else
  {
    if ( util_isColumnSelected(selection) )
    {
      for ( int col = selection.left(); col <= selection.right(); ++col )
      {
        Cell * c = getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured())
          {
              long_max = QMAX( adjustColumnHelper( c, col, c->row() ), long_max );
          }
          c = getNextCellDown( col, c->row() );
        } // end while
      }
    }
    else
    {
      int x = _col;
      Cell * cell;
      for ( int y = selection.top(); y <= selection.bottom(); ++y )
      {
        cell = cellAt( x, y );
        if ( cell != d->defaultCell && !cell->isEmpty()
             && !cell->isObscured() )
        {
            long_max = QMAX( adjustColumnHelper( cell, x, y ), long_max );
        }
      } // for top...bottom
    } // not column selected
  }

  //add 4 because long_max is the long of the text
  //but column has borders
  if( long_max == 0 )
    return -1;
  else
    return ( (int)long_max + 4 );
}

int Sheet::adjustRow( Selection* selectionInfo, int _row )
{
  QRect selection(selectionInfo->selection());
  double long_max = 0.0;
  if( _row == -1 ) //No special row is defined, so use selected rows
  {
    if ( util_isRowSelected(selection) )
    {
      for ( int row = selection.top(); row <= selection.bottom(); ++row )
      {
        Cell * c = getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured() )
          {
            c->calculateTextParameters( painter(), c->column(), row );
            if( c->textHeight() > long_max )
              long_max = c->textHeight()
                + c->topBorderWidth( c->column(), c->row() )
                + c->bottomBorderWidth( c->column(), c->row() );
          }
          c = getNextCellRight( c->column(), row );
        }
      }
    }
  }
  else
  {
    if ( util_isRowSelected(selection) )
    {
      for ( int row = selection.top(); row <= selection.bottom(); ++row )
      {
        Cell * c = getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isEmpty() && !c->isObscured() )
          {
            c->calculateTextParameters( painter(), c->column(), row );
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
      Cell * cell;
      for ( int x = selection.left(); x <= selection.right(); ++x )
      {
        cell = cellAt( x, y );
        if ( cell != d->defaultCell && !cell->isEmpty()
            && !cell->isObscured() )
        {
          cell->calculateTextParameters( painter(), x, y );
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
  if( long_max == 0.0 )
    return -1;
  else
    return ( (int)long_max + 4 );
}

struct ClearTextSelectionWorker : public Sheet::CellWorker {
    Sheet   * _s;

    ClearTextSelectionWorker(  Sheet * s )
      : Sheet::CellWorker( ),  _s( s ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
  return new UndoChangeAreaTextCell( doc, sheet, r );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscured() );
    }
    void doWork( Cell* cell, bool, int, int )
    {
      cell->setCellText( "" );
    }
};

void Sheet::clearTextSelection( Selection* selectionInfo )
{
  if (areaIsEmpty(selectionInfo->selection()))
    return;

  ClearTextSelectionWorker w( this );
  workOnCells( selectionInfo, w );
}


struct ClearValiditySelectionWorker : public Sheet::CellWorker {
    ClearValiditySelectionWorker( ) : Sheet::CellWorker( ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
  return new UndoConditional( doc, sheet, r );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isObscured() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->removeValidity();
    }
};

void Sheet::clearValiditySelection( Selection* selectionInfo )
{
    if(areaIsEmpty(selectionInfo->selection(), Validity))
        return;

  ClearValiditySelectionWorker w;
  workOnCells( selectionInfo, w );
}


struct ClearConditionalSelectionWorker : public Sheet::CellWorker
{
  ClearConditionalSelectionWorker( ) : Sheet::CellWorker( ) { }

  class UndoAction* createUndoAction( Doc* doc,
               Sheet* sheet,
               QRect& r )
  {
    return new UndoConditional( doc, sheet, r );
  }
  bool testCondition( Cell* cell )
  {
    return ( !cell->isObscured() );
  }
  void doWork( Cell* cell, bool, int, int )
  {
    QValueList<KSpread::Conditional> emptyList;
    cell->setConditionList(emptyList);
  }
};

void Sheet::clearConditionalSelection( Selection* selectionInfo )
{
  ClearConditionalSelectionWorker w;
  workOnCells( selectionInfo, w );
}

void Sheet::fillSelection( Selection * selectionInfo, int direction )
{
  QRect rct( selectionInfo->selection() );
  int right  = rct.right();
  int bottom = rct.bottom();
  int left   = rct.left();
  int top    = rct.top();
  int width  = rct.width();
  int height = rct.height();

  QDomDocument undoDoc = saveCellRect( rct );
  loadSelectionUndo( undoDoc, rct, left - 1, top - 1, false, 0 );

  QDomDocument doc;

  switch( direction )
  {
   case Right:
    doc = saveCellRect( QRect( left, top, 1, height ) );
    break;

   case Up:
    doc = saveCellRect( QRect( left, bottom, width, 1 ) );
    break;

   case Left:
    doc = saveCellRect( QRect( right, top, 1, height ) );
    break;

   case Down:
    doc = saveCellRect( QRect( left, top, width, 1 ) );
    break;
  };

  // Save to buffer
  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  QTextStream str( &buffer );
  str.setEncoding( QTextStream::UnicodeUTF8 );
  str << doc;
  buffer.close();

  int i;
  switch( direction )
  {
   case Right:
    for ( i = left + 1; i <= right; ++i )
    {
      paste( buffer.buffer(), QRect( i, top, 1, 1 ), false );
    }
    break;

   case Up:
    for ( i = bottom + 1; i >= top; --i )
    {
      paste( buffer.buffer(), QRect( left, i, 1, 1 ), false );
    }
    break;

   case Left:
    for ( i = right - 1; i >= left; --i )
    {
      paste( buffer.buffer(), QRect( i, top, 1, 1 ), false );
    }
    break;

   case Down:
    for ( i = top + 1; i <= bottom; ++i )
    {
      paste( buffer.buffer(), QRect( left, i, 1, 1 ), false );
    }
    break;
  }

  this->doc()->setModified( true );
}


struct DefaultSelectionWorker : public Sheet::CellWorker {
    DefaultSelectionWorker( ) : Sheet::CellWorker( true, false, true ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
        QString title=i18n("Default Parameters");
  return new UndoCellFormat( doc, sheet, r, title );
    }
    bool testCondition( Cell* ) {
  return true;
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->defaultStyle();
    }
};

void Sheet::defaultSelection( Selection* selectionInfo )
{
  QRect selection(selectionInfo->selection());
  DefaultSelectionWorker w;
  SelectionType st = workOnCells( selectionInfo, w );
  switch ( st ) {
  case CompleteRows:
    RowFormat *rw;
    for ( int i = selection.top(); i <= selection.bottom(); i++ ) {
      rw = nonDefaultRowFormat( i );
      rw->defaultStyleFormat();
    }
    emit sig_updateView( this, selection );
    return;
  case CompleteColumns:
    ColumnFormat *cl;
    for ( int i = selection.left(); i <= selection.right(); i++ ) {
      cl=nonDefaultColumnFormat( i );
      cl->defaultStyleFormat();
    }
    emit sig_updateView( this, selection );
    return;
  case CellRegion:
      emit sig_updateView( this, selection );
      return;
  }
}


struct SetConditionalWorker : public Sheet::CellWorker
{
  QValueList<KSpread::Conditional> conditionList;
  SetConditionalWorker( QValueList<KSpread::Conditional> _tmp ) :
    Sheet::CellWorker( ), conditionList( _tmp ) { }

  class UndoAction* createUndoAction( Doc* doc,
               Sheet* sheet, QRect& r )
  {
    return new UndoConditional( doc, sheet, r );
  }

  bool testCondition( Cell* )
  {
    return true;
  }

  void doWork( Cell* cell, bool, int, int )
  {
    if ( !cell->isObscured() ) // TODO: isObscuringForced()???
    {
      cell->setConditionList(conditionList);
      cell->setDisplayDirtyFlag();
    }
  }
};

void Sheet::setConditional( Selection* selectionInfo,
                                   QValueList<Conditional> const & newConditions)
{
  QRect selection(selectionInfo->selection());
  if ( !doc()->undoLocked() )
  {
    UndoConditional * undo = new UndoConditional( doc(), this, selection );
    doc()->addCommand( undo );
  }

  int l = selection.left();
  int r = selection.right();
  int t = selection.top();
  int b = selection.bottom();

  Cell * cell;
  Style * s = doc()->styleManager()->defaultStyle();
  for (int x = l; x <= r; ++x)
  {
    for (int y = t; y <= b; ++y)
    {
      cell = nonDefaultCell( x, y, false, s );
      cell->setConditionList( newConditions );
      cell->setDisplayDirtyFlag();
    }
  }

  emit sig_updateView( this, selectionInfo->selection() );
}


struct SetValidityWorker : public Sheet::CellWorker {
    Validity tmp;
    SetValidityWorker( Validity _tmp ) : Sheet::CellWorker( ), tmp( _tmp ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
  return new UndoConditional( doc, sheet, r );
    }
    bool testCondition( Cell* ) {
        return true;
    }
    void doWork( Cell* cell, bool, int, int ) {
  if ( !cell->isObscured() ) {
      cell->setDisplayDirtyFlag();
      if ( tmp.m_allow==Allow_All )
    cell->removeValidity();
      else
      {
    Validity *tmpValidity = cell->getValidity();
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
                tmpValidity->displayMessage=tmp.displayMessage;
                tmpValidity->allowEmptyCell=tmp.allowEmptyCell;
                tmpValidity->displayValidationInformation=tmp.displayValidationInformation;
                tmpValidity->titleInfo=tmp.titleInfo;
                tmpValidity->messageInfo=tmp.messageInfo;
                tmpValidity->listValidity=tmp.listValidity;
      }
      cell->clearDisplayDirtyFlag();
  }
    }
};

void Sheet::setValidity(Selection* selectionInfo,
                        KSpread::Validity tmp )
{
    SetValidityWorker w( tmp );
    workOnCells( selectionInfo, w );
}


struct GetWordSpellingWorker : public Sheet::CellWorker {
    QString& listWord;
    GetWordSpellingWorker( QString& _listWord ) : Sheet::CellWorker( false, false, true ), listWord( _listWord ) { }

    class UndoAction* createUndoAction( Doc*, Sheet*, QRect& ) {
  return 0L;
    }
    bool testCondition( Cell* ) {
        return true;
    }
    void doWork( Cell* c, bool cellRegion, int, int ) {
  if ( !c->isObscured() || cellRegion /* ### ??? */ ) {
      if ( !c->isFormula() && !c->value().isNumber() && !c->value().asString().isEmpty() && !c->isTime()
     && !c->isDate()
     && !c->text().isEmpty())
      {
    listWord+=c->text()+'\n';
      }
  }
    }
};

QString Sheet::getWordSpelling(Selection* selectionInfo )
{
    QString listWord;
    GetWordSpellingWorker w( listWord );
    workOnCells( selectionInfo, w );
    return listWord;
}


struct SetWordSpellingWorker : public Sheet::CellWorker {
    QStringList& list;
    int pos;
    Sheet   * sheet;
    SetWordSpellingWorker( QStringList & _list,Sheet * s )
      : Sheet::CellWorker( false, false, true ), list( _list ), pos( 0 ),  sheet( s ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, QRect& r ) {
  return new UndoChangeAreaTextCell( doc, sheet, r );
    }
    bool testCondition( Cell* ) {
        return true;
    }
    void doWork( Cell* c, bool cellRegion, int, int )
    {
  if ( !c->isObscured() || cellRegion /* ### ??? */ ) {
      if ( !c->isFormula() && !c->value().isNumber() && !c->value().asString().isEmpty() && !c->isTime()
     && !c->isDate()
     && !c->text().isEmpty())
      {


    c->setCellText( list[pos] );
    pos++;
      }
  }
    }
};

void Sheet::setWordSpelling(Selection* selectionInfo,
                                   const QString _listWord )
{
    QStringList list = QStringList::split ( '\n', _listWord );
    SetWordSpellingWorker w( list,  this );
    workOnCells( selectionInfo, w );
}

static QString cellAsText( Cell* cell, unsigned int max )
{
  QString result;
  if( !cell->isDefault() )
  {
    int l = max - cell->strOutText().length();
    if (cell->defineAlignX() == KSpread::Format::Right )
    {
        for ( int i = 0; i < l; ++i )
          result += " ";
        result += cell->strOutText();
    }
    else if (cell->defineAlignX() == KSpread::Format::Left )
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

  return result;
}

QString Sheet::copyAsText( Selection* selectionInfo )
{
    // Only one cell selected? => copy active cell
    if ( selectionInfo->singleCellSelection() )
    {
        Cell * cell = cellAt( selectionInfo->marker() );
        if( !cell->isDefault() )
          return cell->strOutText();
        return "";
    }

    QRect selection(selectionInfo->selection());

    // Find area
    unsigned top = selection.bottom();
    unsigned bottom = selection.top();
    unsigned left = selection.right();
    unsigned right = selection.left();

    unsigned max = 1;
    for( Cell *c = d->cells.firstCell();c; c = c->nextCell() )
    {
      if ( !c->isDefault() )
      {
        QPoint p( c->column(), c->row() );
        if ( selection.contains( p ) )
        {
          top = QMIN( top, (unsigned) c->row() );
          left = QMIN( left, (unsigned) c->column() );
          bottom = QMAX( bottom, (unsigned) c->row() );
          right = QMAX( right, (unsigned) c->column() );

          if ( c->strOutText().length() > max )
                 max = c->strOutText().length();
        }
      }
    }

    ++max;

    QString result;
    for ( unsigned y = top; y <= bottom; ++y)
    {
      for ( unsigned x = left; x <= right; ++x)
      {
        Cell *cell = cellAt( x, y );
        result += cellAsText( cell, max );
      }
      result += "\n";
    }

    return result;
}

void Sheet::copySelection( Selection* selectionInfo )
{
    QRect rct;

    rct = selectionInfo->selection();

    QDomDocument doc = saveCellRect( rct, true );

    // Save to buffer
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    QTextStream str( &buffer );
    str.setEncoding( QTextStream::UnicodeUTF8 );
    str << doc;
    buffer.close();

    TextDrag * kd = new TextDrag( 0L );
    kd->setPlain( copyAsText(selectionInfo) );
    kd->setKSpread( buffer.buffer() );

    QApplication::clipboard()->setData( kd );
}

void Sheet::cutSelection( Selection* selectionInfo )
{
    QRect rct;

    rct = selectionInfo->selection();

    QDomDocument doc = saveCellRect( rct, true, true );

    // Save to buffer
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    QTextStream str( &buffer );
    str.setEncoding( QTextStream::UnicodeUTF8 );
    str << doc;
    buffer.close();

    TextDrag * kd = new TextDrag( 0L );
    kd->setPlain( copyAsText(selectionInfo) );
    kd->setKSpread( buffer.buffer() );

    QApplication::clipboard()->setData( kd );

    deleteSelection( selectionInfo, true );
}

void Sheet::paste( const QRect &pasteArea, bool makeUndo,
                          PasteMode sp, Operation op, bool insert, int insertTo, bool pasteFC )
{
    QMimeSource * mime = QApplication::clipboard()->data();
    if ( !mime )
        return;

    QByteArray b;

    if ( mime->provides( TextDrag::selectionMimeType() ) )
    {
        b = mime->encodedData( TextDrag::selectionMimeType() );
    }
    else if( mime->provides( "text/plain" ) )
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype). Copied from KPresenter ;-)
  QString _text = QApplication::clipboard()->text();
        doc()->emitBeginOperation();
  pasteTextPlain( _text, pasteArea);
        emit sig_updateView( this );
        // doc()->emitEndOperation();
  return;
    }
    else
        return;

    // Do the actual pasting.
    doc()->emitBeginOperation();
    paste( b, pasteArea, makeUndo, sp, op, insert, insertTo, pasteFC );
    emit sig_updateView( this );
    // doc()->emitEndOperation();
}


void Sheet::pasteTextPlain( QString &_text, QRect pasteArea)
{
//  QString tmp;
//  tmp= QString::fromLocal8Bit(_mime->encodedData( "text/plain" ));
  if( _text.isEmpty() )
    return;

  QString tmp = _text;
  int i;
  int mx   = pasteArea.left();
  int my   = pasteArea.top();
  int rows = 1;
  int len  = tmp.length();

  //count the numbers of lines in text
  for ( i = 0; i < len; ++i )
  {
    if ( tmp[i] == '\n' )
      ++rows;
  }

  Cell * cell = nonDefaultCell( mx, my );
  if ( rows == 1 )
  {
    if ( !doc()->undoLocked() )
    {
      UndoSetText * undo = new UndoSetText( doc(), this , cell->text(), mx, my, cell->formatType() );
      doc()->addCommand( undo );
    }
  }
  else
  {
      QRect rect(mx, my, mx, my + rows - 1);
      UndoChangeAreaTextCell * undo = new UndoChangeAreaTextCell( doc(), this , rect );
      doc()->addCommand( undo );
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

    if ( !isProtected() || cell->notProtected( mx, my + i ) )
    {
      cell->setCellText( rowtext );
      cell->updateChart();
    }

    // next cell
    ++i;
    cell = nonDefaultCell( mx, my + i );

    if (!cell || p == (int) tmp.length())
      break;

    // exclude the left part and '\n'
    tmp = tmp.right(tmp.length() - p - 1);
  }

  if (!isLoading())
    refreshMergedCell();

  emit sig_updateView( this );
  emit sig_updateHBorder( this );
  emit sig_updateVBorder( this );
}

void Sheet::paste( const QByteArray & b, const QRect & pasteArea, bool makeUndo,
                          PasteMode sp, Operation op, bool insert, int insertTo, bool pasteFC )
{
    kdDebug(36001) << "Parsing " << b.size() << " bytes" << endl;

    QBuffer buffer( b );
    buffer.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &buffer );
    buffer.close();

    // ##### TODO: Test for parsing errors

    int mx = pasteArea.left();
    int my = pasteArea.top();

    loadSelection( doc, pasteArea, mx - 1, my - 1, makeUndo, sp, op, insert,
                   insertTo, pasteFC );
}

bool Sheet::loadSelection( const QDomDocument& doc, const QRect &pasteArea,
                                  int _xshift, int _yshift, bool makeUndo,
                                  PasteMode sp, Operation op, bool insert,
                                  int insertTo, bool pasteFC )
{
    QDomElement e = doc.documentElement();

    //kdDebug(36001) << "loadSelection called. pasteArea=" << pasteArea << endl;

    if (!isLoading() && makeUndo)
        loadSelectionUndo( doc, pasteArea, _xshift, _yshift, insert, insertTo );

    int rowsInClpbrd    =  e.attribute( "rows" ).toInt();
    int columnsInClpbrd =  e.attribute( "columns" ).toInt();

    // find size of rectangle that we want to paste to (either clipboard size or current selection)
    const int pasteWidth = ( pasteArea.width() >= columnsInClpbrd
                             && util_isRowSelected(pasteArea) == false
                             && e.namedItem( "rows" ).toElement().isNull() )
      ? pasteArea.width() : columnsInClpbrd;
    const int pasteHeight = ( pasteArea.height() >= rowsInClpbrd
                              && util_isColumnSelected(pasteArea) == false
                              && e.namedItem( "columns" ).toElement().isNull())
      ? pasteArea.height() : rowsInClpbrd;

    /*    kdDebug() << "loadSelection: paste area has size " << pasteHeight << " rows * "
          << pasteWidth << " columns " << endl;
          kdDebug() << "loadSelection: " << rowsInClpbrd << " rows and "
          << columnsInClpbrd << " columns in clipboard." << endl;
          kdDebug() << "xshift: " << _xshift << " _yshift: " << _yshift << endl;
    */

    if ( !e.namedItem( "columns" ).toElement().isNull() && !isProtected() )
    {
        _yshift = 0;

        // Clear the existing columns
        for( int i = 1; i <= pasteWidth; ++i )
        {
            if(!insert)
            {
                d->cells.clearColumn( _xshift + i );
                d->columns.removeElement( _xshift + i );
            }
        }

        // Insert column formats
        QDomElement c = e.firstChild().toElement();
        for( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "column" )
            {
                ColumnFormat *cl = new ColumnFormat( this, 0 );
                if ( cl->load( c, _xshift, sp, pasteFC ) )
                    insertColumnFormat( cl );
                else
                    delete cl;
            }
        }

    }

    if ( !e.namedItem( "rows" ).toElement().isNull() && !isProtected() )
    {
        _xshift = 0;

        // Clear the existing rows
        for( int i = 1; i <= pasteHeight; ++i )
        {
            d->cells.clearRow( _yshift + i );
            d->rows.removeElement( _yshift + i );
        }

        // Insert row formats
        QDomElement c = e.firstChild().toElement();
        for( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "row" )
            {
                RowFormat *cl = new RowFormat( this, 0 );
                if ( cl->load( c, _yshift, sp, pasteFC ) )
                    insertRowFormat( cl );
                else
                    delete cl;
            }
        }
    }

    Cell* refreshCell = 0;
    Cell *cell;
    Cell *cellBackup = NULL;
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
            if ( isProtected() && !cell->notProtected( col + coff, row + roff ) )
              continue;

            cellBackup = new Cell(this, cell->column(), cell->row());
            cellBackup->copyAll(cell);

            if ( !cell->load( c, _xshift + coff, _yshift + roff, sp, op, pasteFC ) )
            {
              cell->copyAll(cellBackup);
            }
            else
            {
              if( cell->isFormula() )
              {
                  cell->setCalcDirtyFlag();
              }
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

    /* I don't think this is gonna work....doesn't this only update
       one chart -- the one which had a dependant cell update first? - John

       I don't have time to check on this now....
    */
    if ( refreshCell )
        refreshCell->updateChart();
    this->doc()->setModified( true );

    if(!isLoading())
        refreshMergedCell();

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );

    return true;
}

void Sheet::loadSelectionUndo( const QDomDocument & d, const QRect &loadArea,
                                      int _xshift, int _yshift, bool insert,
                                      int insertTo)
{
    QDomElement e = d.documentElement();
    QDomElement c = e.firstChild().toElement();
    int rowsInClpbrd =  e.attribute( "rows" ).toInt();
    int columnsInClpbrd =  e.attribute( "columns" ).toInt();
    // find rect that we paste to
    const int pasteWidth = ( loadArea.width() >= columnsInClpbrd &&
                             util_isRowSelected(loadArea) == false &&
                             e.namedItem( "rows" ).toElement().isNull() )
        ? loadArea.width() : columnsInClpbrd;
    const int pasteHeight = ( loadArea.height() >= rowsInClpbrd &&
                              util_isColumnSelected(loadArea) == false &&
                              e.namedItem( "columns" ).toElement().isNull() )
        ? loadArea.height() : rowsInClpbrd;
    QRect rect;
    if ( !e.namedItem( "columns" ).toElement().isNull() )
    {
        if ( !doc()->undoLocked() )
        {
                UndoCellPaste *undo = new UndoCellPaste( doc(), this, pasteWidth, 0, _xshift,_yshift,rect,insert );
                doc()->addCommand( undo );
        }
        if(insert)
                 insertColumn(  _xshift+1,pasteWidth-1,false);
  return;
    }

    if ( !e.namedItem( "rows" ).toElement().isNull() )
    {
        if ( !doc()->undoLocked() )
        {
                UndoCellPaste *undo = new UndoCellPaste( doc(), this, 0,pasteHeight, _xshift,_yshift,rect,insert );
                doc()->addCommand( undo );
        }
  if(insert)
      insertRow(  _yshift+1,pasteHeight-1,false);
  return;
    }

    rect.setRect( _xshift+1, _yshift+1, pasteWidth, pasteHeight );

    if(!c.isNull())
    {
        if ( !doc()->undoLocked() )
        {
                UndoCellPaste *undo = new UndoCellPaste( doc(), this, 0,0,_xshift,_yshift,rect,insert,insertTo );
                doc()->addCommand( undo );
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

bool Sheet::testAreaPasteInsert()const
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
    QDomDocument d;
    d.setContent( &buffer );
    buffer.close();

    QDomElement e = d.documentElement();
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

void Sheet::deleteCells( const QRect& rect )
{
    // A list of all cells we want to delete.
    QPtrStack<Cell> cellStack;

    QRect tmpRect;
    bool extraCell = false;
    if (rect.width() == 1 && rect.height() == 1 )
    {
      Cell * cell = nonDefaultCell( rect.x(), rect.y() );
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
      Cell * c = getFirstCellRow( row );
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

    d->cells.setAutoDelete( false );

    // Remove the cells from the sheet
    while ( !cellStack.isEmpty() )
    {
      Cell * cell = cellStack.pop();

      d->cells.remove( cell->column(), cell->row() );
      cell->setCalcDirtyFlag();
      setRegionPaintDirty(cell->cellRect());

      delete cell;
    }

    d->cells.setAutoDelete( true );

    setLayoutDirtyFlag();

    // TODO: don't go through all cells here!
    // Since obscured cells might have been deleted we
    // have to reenforce it.
    Cell * c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
      if ( c->isForceExtraCells() && !c->isDefault() )
        c->forceExtraCells( c->column(), c->row(),
                            c->extraXCells(), c->extraYCells() );
    }
    doc()->setModified( true );
}

void Sheet::deleteSelection( Selection* selectionInfo, bool undo )
{
    QRect r( selectionInfo->selection() );

    if ( undo && !doc()->undoLocked() )
    {
        UndoDelete *undo = new UndoDelete( doc(), this, r );
        doc()->addCommand( undo );
    }

    // Entire rows selected ?
    if ( util_isRowSelected(r) )
    {
        for( int i = r.top(); i <= r.bottom(); ++i )
        {
            d->cells.clearRow( i );
            d->rows.removeElement( i );
        }

        emit sig_updateVBorder( this );
    }
    // Entire columns selected ?
    else if ( util_isColumnSelected(r) )
    {
        for( int i = r.left(); i <= r.right(); ++i )
        {
            d->cells.clearColumn( i );
            d->columns.removeElement( i );
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

void Sheet::updateView()
{
  emit sig_updateView( this );
}

void Sheet::updateView( QRect const & rect )
{
  emit sig_updateView( this, rect );
}

void Sheet::refreshView( const QRect & rect )
{
  // TODO: don't go through all cells when refreshing!
    QRect tmp(rect);
    Cell * c = d->cells.firstCell();
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


void Sheet::changeMergedCell( int m_iCol, int m_iRow, int m_iExtraX, int m_iExtraY)
{
   if( m_iExtraX==0 && m_iExtraY==0)
   {
     dissociateCell( QPoint( m_iCol,m_iRow));
     return;
   }

   QRect rect;
   rect.setCoords(m_iCol,m_iRow,m_iCol+m_iExtraX,m_iRow+m_iExtraY);

   mergeCells(rect);
}

void Sheet::mergeCells( const QRect &area )
{
  // sanity check
  if( isProtected() )
    return;
  if( workbook()->isProtected() )
    return;

  // no span ?
  if( area.width() == 1 && area.height() == 1)
    return;

  QPoint topLeft = area.topLeft();

  Cell *cell = nonDefaultCell( topLeft );
  cell->forceExtraCells( topLeft.x(), topLeft.y(),
                         area.width() - 1, area.height() - 1);

  if ( getAutoCalc() )
    recalc();

  emit sig_updateView( this, area );
}

void Sheet::dissociateCell( const QPoint &cellRef )
{
  QPoint marker(cellRef);
  Cell *cell = nonDefaultCell( marker );
  if(!cell->isForceExtraCells())
    return;

  int x = cell->extraXCells() + 1;
  if( x == 0 )
    x = 1;
  int y = cell->extraYCells() + 1;
  if( y == 0 )
    y = 1;

  cell->forceExtraCells( marker.x() ,marker.y(), 0, 0 );
  QRect selection( marker.x() - 1, marker.y() - 1, x + 2, y + 2 );
  refreshMergedCell();
  emit sig_updateView( this, selection );
}

bool Sheet::testListChoose(Selection* selectionInfo)
{
   QRect selection( selectionInfo->selection() );
   QPoint marker( selectionInfo->marker() );

   Cell *cell = cellAt( marker.x(), marker.y() );
   QString tmp=cell->text();

   Cell* c = firstCell();
   bool different=false;
   int col;
   for( ;c; c = c->nextCell() )
     {
       col = c->column();
       if ( selection.left() <= col && selection.right() >= col &&
            !c->isObscuringForced() &&
            !(col==marker.x() && c->row()==marker.y()))
   {
     if(!c->isFormula() && !c->value().isNumber() && !c->value().asString().isEmpty()
        && !c->isTime() &&!c->isDate() )
       {
                 if(c->text()!=tmp)
                     different=true;
       }

   }
     }
   return different;
}




QDomDocument Sheet::saveCellRect( const QRect &_rect, bool copy, bool era )
{
    QDomDocument dd( "spreadsheet-snippet" );
    dd.appendChild( dd.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement spread = dd.createElement( "spreadsheet-snippet" );
    spread.setAttribute( "rows", _rect.bottom() - _rect.top() + 1 );
    spread.setAttribute( "columns", _rect.right() - _rect.left() + 1 );
    dd.appendChild( spread );

    //
    // Entire rows selected ?
    //
    if ( util_isRowSelected( _rect ) )
    {
        QDomElement rows = dd.createElement("rows");
        rows.setAttribute( "count", _rect.bottom() - _rect.top() + 1 );
        spread.appendChild( rows );

        // Save all cells.
        Cell* c = d->cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            if ( !c->isDefault()&&!c->isObscuringForced() )
            {
                QPoint p( c->column(), c->row() );
                if ( _rect.contains( p ) )
                    spread.appendChild( c->save( dd, 0, _rect.top() - 1, copy, copy, era ) );
            }
        }

        // ##### Inefficient
        // Save the row formats if there are any
        RowFormat* lay;
        for( int y = _rect.top(); y <= _rect.bottom(); ++y )
        {
            lay = rowFormat( y );
            if ( lay && !lay->isDefault() )
            {
                QDomElement e = lay->save( dd, _rect.top() - 1, copy );
                if ( !e.isNull() )
                    spread.appendChild( e );
            }
        }

        return dd;
    }

    //
    // Entire columns selected ?
    //
    if ( util_isColumnSelected( _rect ) )
    {
        QDomElement columns = dd.createElement("columns");
        columns.setAttribute( "count", _rect.right() - _rect.left() + 1 );
        spread.appendChild( columns );

        // Save all cells.
        Cell* c = d->cells.firstCell();
        for( ;c; c = c->nextCell() )
        {
            if ( !c->isDefault()&&!c->isObscuringForced())
            {
                QPoint p( c->column(), c->row() );
                if ( _rect.contains( p ) )
                    spread.appendChild( c->save( dd, _rect.left() - 1, 0, copy, copy, era ) );
            }
        }

        // ##### Inefficient
        // Save the column formats if there are any
        ColumnFormat* lay;
        for( int x = _rect.left(); x <= _rect.right(); ++x )
        {
            lay = columnFormat( x );
            if ( lay && !lay->isDefault() )
            {
                QDomElement e = lay->save( dd, _rect.left() - 1, copy );
                if ( !e.isNull() )
                    spread.appendChild( e );
            }
        }

        return dd;
    }

    // Save all cells.
    //store all cell
    //when they don't exist we created them
    //because it's necessary when there is a  format on a column/row
    //but I remove cell which is inserted.
    Cell *cell;
    bool insert;
    for (int i=_rect.left();i<=_rect.right();i++)
  for(int j=_rect.top();j<=_rect.bottom();j++)
  {
      insert = false;
      cell = cellAt( i, j );
      if ( cell == d->defaultCell )
      {
    cell = new Cell( this, i, j );
    insertCell( cell );
    insert=true;
      }
      spread.appendChild( cell->save( dd, _rect.left() - 1, _rect.top() - 1, true, copy, era ) );
      if( insert )
          d->cells.remove(i,j);
  }

    return dd;
}

QDomElement Sheet::saveXML( QDomDocument& dd )
{
    QDomElement sheet = dd.createElement( "table" );
    sheet.setAttribute( "name", d->name );


    //Laurent: for oasis format I think that we must use style:direction...
    sheet.setAttribute( "layoutDirection", (d->layoutDirection == RightToLeft) ? "rtl" : "ltr" );
    sheet.setAttribute( "columnnumber", (int)d->showColumnNumber);
    sheet.setAttribute( "borders", (int)d->showPageBorders);
    sheet.setAttribute( "hide", (int)d->hide);
    sheet.setAttribute( "hidezero", (int)d->hideZero);
    sheet.setAttribute( "firstletterupper", (int)d->firstLetterUpper);
    sheet.setAttribute( "grid", (int)d->showGrid );
    sheet.setAttribute( "printGrid", (int)d->print->printGrid() );
    sheet.setAttribute( "printCommentIndicator", (int)d->print->printCommentIndicator() );
    sheet.setAttribute( "printFormulaIndicator", (int)d->print->printFormulaIndicator() );
    if ( doc()->specialOutputFlag() == KoDocument::SaveAsKOffice1dot1 /* so it's KSpread < 1.2 */)
      sheet.setAttribute( "formular", (int)d->showFormula); //Was named different
    else
      sheet.setAttribute( "showFormula", (int)d->showFormula);
    sheet.setAttribute( "showFormulaIndicator", (int)d->showFormulaIndicator);
    sheet.setAttribute( "lcmode", (int)d->lcMode);
    sheet.setAttribute( "autoCalc", (int)d->autoCalc);
    sheet.setAttribute( "borders1.2", 1);
    if ( !d->password.isNull() )
    {
      if ( d->password.size() > 0 )
      {
        QCString str = KCodecs::base64Encode( d->password );
        sheet.setAttribute( "protected", QString( str.data() ) );
      }
      else
        sheet.setAttribute( "protected", "" );
    }

    // paper parameters
    QDomElement paper = dd.createElement( "paper" );
    paper.setAttribute( "format", d->print->paperFormatString() );
    paper.setAttribute( "orientation", d->print->orientationString() );
    sheet.appendChild( paper );

    QDomElement borders = dd.createElement( "borders" );
    borders.setAttribute( "left", d->print->leftBorder() );
    borders.setAttribute( "top", d->print->topBorder() );
    borders.setAttribute( "right", d->print->rightBorder() );
    borders.setAttribute( "bottom", d->print->bottomBorder() );
    paper.appendChild( borders );

    QDomElement head = dd.createElement( "head" );
    paper.appendChild( head );
    if ( !d->print->headLeft().isEmpty() )
    {
      QDomElement left = dd.createElement( "left" );
      head.appendChild( left );
      left.appendChild( dd.createTextNode( d->print->headLeft() ) );
    }
    if ( !d->print->headMid().isEmpty() )
    {
      QDomElement center = dd.createElement( "center" );
      head.appendChild( center );
      center.appendChild( dd.createTextNode( d->print->headMid() ) );
    }
    if ( !d->print->headRight().isEmpty() )
    {
      QDomElement right = dd.createElement( "right" );
      head.appendChild( right );
      right.appendChild( dd.createTextNode( d->print->headRight() ) );
    }
    QDomElement foot = dd.createElement( "foot" );
    paper.appendChild( foot );
    if ( !d->print->footLeft().isEmpty() )
    {
      QDomElement left = dd.createElement( "left" );
      foot.appendChild( left );
      left.appendChild( dd.createTextNode( d->print->footLeft() ) );
    }
    if ( !d->print->footMid().isEmpty() )
    {
      QDomElement center = dd.createElement( "center" );
      foot.appendChild( center );
      center.appendChild( dd.createTextNode( d->print->footMid() ) );
    }
    if ( !d->print->footRight().isEmpty() )
    {
      QDomElement right = dd.createElement( "right" );
      foot.appendChild( right );
      right.appendChild( dd.createTextNode( d->print->footRight() ) );
    }

    // print range
    QDomElement printrange = dd.createElement( "printrange-rect" );
    QRect _printRange = d->print->printRange();
    int left = _printRange.left();
    int right = _printRange.right();
    int top = _printRange.top();
    int bottom = _printRange.bottom();
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
    sheet.appendChild( printrange );

    // Print repeat columns
    QDomElement printRepeatColumns = dd.createElement( "printrepeatcolumns" );
    printRepeatColumns.setAttribute( "left", d->print->printRepeatColumns().first );
    printRepeatColumns.setAttribute( "right", d->print->printRepeatColumns().second );
    sheet.appendChild( printRepeatColumns );

    // Print repeat rows
    QDomElement printRepeatRows = dd.createElement( "printrepeatrows" );
    printRepeatRows.setAttribute( "top", d->print->printRepeatRows().first );
    printRepeatRows.setAttribute( "bottom", d->print->printRepeatRows().second );
    sheet.appendChild( printRepeatRows );

    //Save print zoom
    sheet.setAttribute( "printZoom", d->print->zoom() );

    //Save page limits
    sheet.setAttribute( "printPageLimitX", d->print->pageLimitX() );
    sheet.setAttribute( "printPageLimitY", d->print->pageLimitY() );

    // Save all cells.
    Cell* c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( !c->isDefault() )
        {
            QDomElement e = c->save( dd );
            if ( !e.isNull() )
                sheet.appendChild( e );
        }
    }

    // Save all RowFormat objects.
    RowFormat* rl = d->rows.first();
    for( ; rl; rl = rl->next() )
    {
        if ( !rl->isDefault() )
        {
            QDomElement e = rl->save( dd );
            if ( e.isNull() )
                return QDomElement();
            sheet.appendChild( e );
        }
    }

    // Save all ColumnFormat objects.
    ColumnFormat* cl = d->columns.first();
    for( ; cl; cl = cl->next() )
    {
        if ( !cl->isDefault() )
        {
            QDomElement e = cl->save( dd );
            if ( e.isNull() )
                return QDomElement();
            sheet.appendChild( e );
        }
    }

    QPtrListIterator<KoDocumentChild> chl( doc()->children() );
    for( ; chl.current(); ++chl )
    {
       if ( ((Child*)chl.current())->sheet() == this )
        {
            QDomElement e;
            Child * child = (Child *) chl.current();

            // stupid hack :-( has anybody a better solution?
            if ( child->inherits("KSpread::ChartChild") )
            {
                e = ((ChartChild *) child)->save( dd );
            }
            else
                e = chl.current()->save( dd );

            if ( e.isNull() )
                return QDomElement();
            sheet.appendChild( e );
        }
    }
    return sheet;
}

bool Sheet::isLoading()
{
    return doc()->isLoading();
}

void Sheet::checkContentDirection( QString const & name )
{
  /* set sheet's direction to RTL if sheet name is an RTL string */
  if ( (name.isRightToLeft()) )
    setLayoutDirection( RightToLeft );
  else
    setLayoutDirection( LeftToRight );

  emit sig_refreshView();
}

bool Sheet::loadSheetStyleFormat( QDomElement *style )
{
    QString hleft, hmiddle, hright;
    QString fleft, fmiddle, fright;
    QDomNode header = KoDom::namedItemNS( *style, KoXmlNS::style, "header" );

    if ( !header.isNull() )
    {
        kdDebug() << "Header exists" << endl;
        QDomNode part = KoDom::namedItemNS( header, KoXmlNS::style, "region-left" );
        if ( !part.isNull() )
        {
            hleft = getPart( part );
            kdDebug() << "Header left: " << hleft << endl;
        }
        else
            kdDebug() << "Style:region:left doesn't exist!" << endl;
        part = KoDom::namedItemNS( header, KoXmlNS::style, "region-center" );
        if ( !part.isNull() )
        {
            hmiddle = getPart( part );
            kdDebug() << "Header middle: " << hmiddle << endl;
        }
        part = KoDom::namedItemNS( header, KoXmlNS::style, "region-right" );
        if ( !part.isNull() )
        {
            hright = getPart( part );
            kdDebug() << "Header right: " << hright << endl;
        }
    }
    //TODO implement it under kspread
    QDomNode headerleft = KoDom::namedItemNS( *style, KoXmlNS::style, "header-left" );
    if ( !headerleft.isNull() )
    {
        QDomElement e = headerleft.toElement();
        if ( e.hasAttributeNS( KoXmlNS::style, "display" ) )
            kdDebug()<<"header.hasAttribute( style:display ) :"<<e.hasAttributeNS( KoXmlNS::style, "display" )<<endl;
        else
            kdDebug()<<"header left doesn't has attribute  style:display  \n";
    }
    //TODO implement it under kspread
    QDomNode footerleft = KoDom::namedItemNS( *style, KoXmlNS::style, "footer-left" );
    if ( !footerleft.isNull() )
    {
        QDomElement e = footerleft.toElement();
        if ( e.hasAttributeNS( KoXmlNS::style, "display" ) )
            kdDebug()<<"footer.hasAttribute( style:display ) :"<<e.hasAttributeNS( KoXmlNS::style, "display" )<<endl;
        else
            kdDebug()<<"footer left doesn't has attribute  style:display  \n";
    }

    QDomNode footer = KoDom::namedItemNS( *style, KoXmlNS::style, "footer" );

    if ( !footer.isNull() )
    {
        QDomNode part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-left" );
        if ( !part.isNull() )
        {
            fleft = getPart( part );
            kdDebug() << "Footer left: " << fleft << endl;
        }
        part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-center" );
        if ( !part.isNull() )
        {
            fmiddle = getPart( part );
            kdDebug() << "Footer middle: " << fmiddle << endl;
        }
        part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-right" );
        if ( !part.isNull() )
        {
            fright = getPart( part );
            kdDebug() << "Footer right: " << fright << endl;
        }
    }

    print()->setHeadFootLine( hleft, hmiddle, hright,
                              fleft, fmiddle, fright );
    return true;
}

void Sheet::replaceMacro( QString & text, const QString & old, const QString & newS )
{
  int n = text.find( old );
  if ( n != -1 )
    text = text.replace( n, old.length(), newS );
}


QString Sheet::getPart( const QDomNode & part )
{
  QString result;
  QDomElement e = KoDom::namedItemNS( part, KoXmlNS::text, "p" );
  while ( !e.isNull() )
  {
    QString text = e.text();
    kdDebug() << "PART: " << text << endl;

    QDomElement macro = KoDom::namedItemNS( e, KoXmlNS::text, "time" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<time>" );

    macro = KoDom::namedItemNS( e, KoXmlNS::text, "date" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<date>" );

    macro = KoDom::namedItemNS( e, KoXmlNS::text, "page-number" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<page>" );

    macro = KoDom::namedItemNS( e, KoXmlNS::text, "page-count" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<pages>" );

    macro = KoDom::namedItemNS( e, KoXmlNS::text, "sheet-name" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<sheet>" );

    macro = KoDom::namedItemNS( e, KoXmlNS::text, "title" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<name>" );

    macro = KoDom::namedItemNS( e, KoXmlNS::text, "file-name" );
    if ( !macro.isNull() )
      replaceMacro( text, macro.text(), "<file>" );

    //add support for multi line into kspread
    if ( !result.isEmpty() )
      result += '\n';
    result += text;
    e = e.nextSibling().toElement();
  }

  return result;
}


bool Sheet::loadOasis( const QDomElement& sheetElement, const KoOasisStyles& oasisStyles )
{
    d->layoutDirection = LeftToRight;
    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString stylename = sheetElement.attributeNS( KoXmlNS::table, "style-name", QString::null );
        kdDebug()<<" style of table :"<<stylename<<endl;
        QDomElement *style = oasisStyles.styles()[stylename];
        Q_ASSERT( style );
        kdDebug()<<" style :"<<style<<endl;
        if ( style )
        {
            QDomElement properties( KoDom::namedItemNS( *style, KoXmlNS::style, "table-properties" ) );
            if ( !properties.isNull() )
            {
                if ( properties.hasAttributeNS( KoXmlNS::table, "display" ) )
                {
                    bool visible = (properties.attributeNS( KoXmlNS::table, "display", QString::null ) == "true" ? true : false );
                    d->hide = !visible;
                }
            }
            if ( style->hasAttributeNS( KoXmlNS::style, "master-page-name" ) )
            {
                QString masterPageStyleName = style->attributeNS( KoXmlNS::style, "master-page-name", QString::null );
                kdDebug()<<"style->attribute( style:master-page-name ) :"<<masterPageStyleName <<endl;
                QDomElement *masterStyle = oasisStyles.masterPages()[masterPageStyleName];
                kdDebug()<<"oasisStyles.styles()[masterPageStyleName] :"<<masterStyle<<endl;
                if ( masterStyle )
                {
                    loadSheetStyleFormat( masterStyle );
                    if ( masterStyle->hasAttributeNS( KoXmlNS::style, "page-layout-name" ) )
                    {
                        QString masterPageLayoutStyleName=masterStyle->attributeNS( KoXmlNS::style, "page-layout-name", QString::null );
                        kdDebug()<<"masterPageLayoutStyleName :"<<masterPageLayoutStyleName<<endl;
                        QDomElement *masterLayoutStyle = oasisStyles.styles()[masterPageLayoutStyleName];
                        kdDebug()<<"masterLayoutStyle :"<<masterLayoutStyle<<endl;
                        KoStyleStack styleStack;
                        styleStack.setTypeProperties( "page-layout" );
                        styleStack.push( *masterLayoutStyle );
                        loadOasisMasterLayoutPage( styleStack );
                    }
                }
            }
        }
    }

    int rowIndex = 1;
    int indexCol = 1;
    QDomNode rowNode = sheetElement.firstChild();
    while( !rowNode.isNull() )
    {
        kdDebug()<<" rowIndex :"<<rowIndex<<" indexCol :"<<indexCol<<endl;
        QDomElement rowElement = rowNode.toElement();
        if( !rowElement.isNull() )
        {
            kdDebug()<<" Sheet::loadOasis rowElement.tagName() :"<<rowElement.localName()<<endl;
            if ( rowElement.namespaceURI() == KoXmlNS::table )
            {
                if ( rowElement.localName()=="table-column" )
                {
                    kdDebug ()<<" table-column found : index column before "<< indexCol<<endl;
                    loadColumnFormat( rowElement, oasisStyles, indexCol );
                    kdDebug ()<<" table-column found : index column after "<< indexCol<<endl;
                }
                else if( rowElement.localName() == "table-row" )
                {
                    kdDebug()<<" table-row found :index row before "<<rowIndex<<endl;
                    loadRowFormat( rowElement, rowIndex, oasisStyles, rowNode.isNull() );
                    kdDebug()<<" table-row found :index row after "<<rowIndex<<endl;
                }
            }
        }
        rowNode = rowNode.nextSibling();
    }

    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "print-ranges" ) )
    {
        // e.g.: Sheet4.A1:Sheet4.E28
        QString range = sheetElement.attributeNS( KoXmlNS::table, "print-ranges", QString::null );
        Range p( translateOpenCalcPoint( range ) );
        if ( sheetName() == p.sheetName )
            d->print->setPrintRange( p.range );
    }


    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "protected" ) )
    {
        QCString passwd( "" );
        if ( sheetElement.hasAttributeNS( KoXmlNS::table, "protection-key" ) )
        {
            QString p = sheetElement.attributeNS( KoXmlNS::table, "protection-key", QString::null );
            QCString str( p.latin1() );
            kdDebug(30518) << "Decoding password: " << str << endl;
            passwd = KCodecs::base64Decode( str );
        }
        kdDebug(30518) << "Password hash: '" << passwd << "'" << endl;
        d->password = passwd;
    }
    return true;
}


void Sheet::loadOasisMasterLayoutPage( KoStyleStack &styleStack )
{
    float left = 0.0;
    float right = 0.0;
    float top = 0.0;
    float bottom = 0.0;
    float width = 0.0;
    float height = 0.0;
    QString orientation = "Portrait";
    QString format;

    // Laurent : Why we stored layout information as Millimeter ?!!!!!
    // kspread used point for all other attribute
    // I don't understand :(
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "page-width" ) )
    {
        width = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "page-width" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "page-height" ) )
    {
        height = KoUnit::toMM( KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "page-height" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "margin-top" ) )
    {
        top = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-top" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "margin-bottom" ) )
    {
        bottom = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-bottom" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "margin-left" ) )
    {
        left = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-left" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "margin-right" ) )
    {
        right = KoUnit::toMM(KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-right" ) ) );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "writing-mode" ) )
    {
        kdDebug()<<"styleStack.hasAttribute( style:writing-mode ) :"<<styleStack.hasAttributeNS( KoXmlNS::style, "writing-mode" )<<endl;
        d->layoutDirection = ( styleStack.attributeNS( KoXmlNS::style, "writing-mode" )=="lr-tb" ) ? LeftToRight : RightToLeft;
        //TODO
        //<value>lr-tb</value>
        //<value>rl-tb</value>
        //<value>tb-rl</value>
        //<value>tb-lr</value>
        //<value>lr</value>
        //<value>rl</value>
        //<value>tb</value>
        //<value>page</value>

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "print-orientation" ) )
    {
        orientation = ( styleStack.attributeNS( KoXmlNS::style, "print-orientation" )=="landscape" ) ? "Landscape" : "Portrait" ;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "num-format" ) )
    {
        //not implemented into kspread
        //These attributes specify the numbering style to use.
        //If a numbering style is not specified, the numbering style is inherited from
        //the page style. See section 6.7.8 for information on these attributes
        kdDebug()<<" num-format :"<<styleStack.attributeNS( KoXmlNS::style, "num-format" )<<endl;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) )
    {
        //TODO
        kdDebug()<<" fo:background-color :"<<styleStack.attributeNS( KoXmlNS::fo, "background-color" )<<endl;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "print" ) )
    {
        //todo parsing
        QString str = styleStack.attributeNS( KoXmlNS::style, "print" );
        kdDebug()<<" style:print :"<<str<<endl;

        if (str.contains( "headers" ) )
        {
            //TODO implement it into kspread
        }
        if ( str.contains( "grid" ) )
        {
            d->print->setPrintGrid( true );
        }
        if ( str.contains( "annotations" ) )
        {
            //TODO it's not implemented
        }
        if ( str.contains( "objects" ) )
        {
            //TODO it's not implemented
        }
        if ( str.contains( "charts" ) )
        {
            //TODO it's not implemented
        }
        if ( str.contains( "drawings" ) )
        {
            //TODO it's not implemented
        }
        if ( str.contains( "formulas" ) )
        {
            d->showFormula = true;
        }
        if ( str.contains( "zero-values" ) )
        {
            //TODO it's not implemented
        }
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "table-centering" ) )
    {
        QString str = styleStack.attributeNS( KoXmlNS::style, "table-centering" );
        //TODO not implemented into kspread
        kdDebug()<<" styleStack.attribute( style:table-centering ) :"<<str<<endl;
#if 0
        if ( str == "horizontal" )
        {
        }
        else if ( str == "vertical" )
        {
        }
        else if ( str == "both" )
        {
        }
        else if ( str == "none" )
        {
        }
        else
            kdDebug()<<" table-centering unknown :"<<str<<endl;
#endif
    }
    format = QString( "%1x%2" ).arg( width ).arg( height );
    kdDebug()<<" format : "<<format<<endl;
    d->print->setPaperLayout( left, top, right, bottom, format, orientation );

    kdDebug()<<" left margin :"<<left<<" right :"<<right<<" top :"<<top<<" bottom :"<<bottom<<endl;
//<style:properties fo:page-width="21.8cm" fo:page-height="28.801cm" fo:margin-top="2cm" fo:margin-bottom="2.799cm" fo:margin-left="1.3cm" fo:margin-right="1.3cm" style:writing-mode="lr-tb"/>
//          QString format = paper.attribute( "format" );
//      QString orientation = paper.attribute( "orientation" );
//        d->print->setPaperLayout( left, top, right, bottom, format, orientation );
//      }
}


bool Sheet::loadColumnFormat(const QDomElement& column, const KoOasisStyles& oasisStyles, int & indexCol )
{
    kdDebug()<<"bool Sheet::loadColumnFormat(const QDomElement& column, const KoOasisStyles& oasisStyles, unsigned int & indexCol ) index Col :"<<indexCol<<endl;

    bool collapsed = ( column.attributeNS( KoXmlNS::table, "visibility", QString::null ) == "collapse" );
    Format layout( this , doc()->styleManager()->defaultStyle() );
    int number = 1;
    double width   = 10;//POINT_TO_MM( colWidth ); FIXME
    if ( column.hasAttributeNS( KoXmlNS::table, "number-columns-repeated" ) )
    {
        bool ok = true;
        number = column.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt( &ok );
        if ( !ok )
            number = 1;
        kdDebug() << "Repeated: " << number << endl;
    }

    KoStyleStack styleStack;
    styleStack.setTypeProperties("table-column"); //style for column
    if ( column.hasAttributeNS( KoXmlNS::table, "default-cell-style-name" ) )
    {
        QString str = column.attributeNS( KoXmlNS::table, "default-cell-style-name", QString::null );
        kdDebug()<<" default-cell-style-name :"<<str<<endl;
        QDomElement *style = oasisStyles.styles()[str];
        kdDebug()<<"default column style :"<<style<<endl;
        if ( style )
        {
            styleStack.push( *style );
            layout.loadOasisStyleProperties( styleStack, oasisStyles );
            styleStack.pop();
        }
    }

    styleStack.setTypeProperties("table-column");
    if ( column.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString str = column.attributeNS( KoXmlNS::table, "style-name", QString::null );
        QDomElement *style = oasisStyles.styles()[str];
        styleStack.push( *style );
        kdDebug()<<" style column:"<<style<<"style name : "<<str<<endl;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::style, "column-width" ) )
    {
        width = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::style, "column-width" ) , -1 );
        kdDebug()<<" style:column-width : width :"<<width<<endl;
    }

    bool insertPageBreak = false;
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "break-before" ) )
    {
        QString str = styleStack.attributeNS( KoXmlNS::fo, "break-before" );
        if ( str == "page" )
        {
            insertPageBreak = true;
        }
        else
            kdDebug()<<" str :"<<str<<endl;
    }


    if ( number>30 )
        number = 30; //TODO fixme !

    for ( int i = 0; i < number; ++i )
    {
        kdDebug()<<" insert new column: pos :"<<indexCol<<" width :"<<width<<" hidden ? "<<collapsed<<endl;
        ColumnFormat * col = new ColumnFormat( this, indexCol );
        col->copy( layout );
        if ( width != -1 ) //safe
            col->setWidth( (int) width );

        // if ( insertPageBreak )
        //   col->setPageBreak( true )

        if ( collapsed )
            col->setHide( true );

        insertColumnFormat( col );
        ++indexCol;
    }
    kdDebug()<<" after index column !!!!!!!!!!!!!!!!!! :"<<indexCol<<endl;
    return true;
}


bool Sheet::loadRowFormat( const QDomElement& row, int &rowIndex,const KoOasisStyles& oasisStyles, bool isLast )
{
    kdDebug()<<"Sheet::loadRowFormat( const QDomElement& row, int &rowIndex,const KoOasisStyles& oasisStyles, bool isLast )***********\n";
    double height = -1.0;
    Format layout( this , doc()->styleManager()->defaultStyle() );
    KoStyleStack styleStack;
    styleStack.setTypeProperties( "table-row" );
    int backupRow = rowIndex;
    if ( row.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString str = row.attributeNS( KoXmlNS::table, "style-name", QString::null );
        QDomElement *style = oasisStyles.styles()[str];
        styleStack.push( *style );
        kdDebug()<<" style column:"<<style<<"style name : "<<str<<endl;
    }
    layout.loadOasisStyleProperties( styleStack, oasisStyles );
    styleStack.setTypeProperties( "table-row" );
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "row-height" ) )
    {
        height = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::style, "row-height" ) , -1 );
        kdDebug()<<" properties style:row-height : height :"<<height<<endl;
    }

    int number = 1;
    if ( row.hasAttributeNS( KoXmlNS::table, "number-rows-repeated" ) )
    {
        bool ok = true;
        int n = row.attributeNS( KoXmlNS::table, "number-rows-repeated", QString::null ).toInt( &ok );
        if ( ok )
            number = n;
        kdDebug() << "Row repeated: " << number << endl;
    }
    bool collapse = false;
    if ( row.hasAttributeNS( KoXmlNS::table, "visibility" ) )
    {
        QString visible = row.attributeNS( KoXmlNS::table, "visibility", QString::null );
        kdDebug()<<" row.attribute( table:visibility ) "<<visible<<endl;
        if ( visible == "collapse" )
            collapse=true;
        else
            kdDebug()<<" visible row not implemented/supported : "<<visible<<endl;

    }
    //kdDebug()<<" height !!!!!!!!!!!!!!!!!!!!!!! :"<<height<<endl;
    //code from opencalc filter, I don't know why it's necessary.
    if ( isLast )
    {
        if ( number > 30 )
            number = 30;
    }
    else
    {
        if ( number > 256 )
            number = 256;
    }

    bool insertPageBreak = false;
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "break-before" ) )
    {
        QString str = styleStack.attributeNS( KoXmlNS::fo, "break-before" );
        if ( str == "page" )
        {
            insertPageBreak = true;
        }
        else
            kdDebug()<<" str :"<<str<<endl;
    }

    //number == number of row to be copy. But we must copy cell too.
    for ( int i = 0; i < number; ++i )
    {
        kdDebug()<<" create non defaultrow format :"<<rowIndex<<" repeate : "<<number<<" height :"<<height<<endl;
        RowFormat * rowL = nonDefaultRowFormat( rowIndex );
        rowL->copy( layout );
        if ( height != -1 )
        {
            kdDebug() << "Setting row height to " << height << endl;
            rowL->setHeight( (int) height );
        }
        if ( collapse )
            rowL->setHide( true );

        ++rowIndex;
    }

    int columnIndex = 0;
    QDomNode cellNode = row.firstChild();
    while( !cellNode.isNull() )
    {
        QDomElement cellElement = cellNode.toElement();
        if( !cellElement.isNull() )
        {
            ++columnIndex;
            //kdDebug()<<"bool Sheet::loadRowFormat( const QDomElement& row, int &rowIndex,const KoOasisStyles& oasisStyles, bool isLast ) cellElement.tagName() :"<<cellElement.tagName()<<endl;
            if( cellElement.localName() == "table-cell" && cellElement.namespaceURI() == KoXmlNS::table)
            {
                kdDebug()<<" create cell at row index :"<<backupRow<<endl;
                Cell* cell = nonDefaultCell( columnIndex, backupRow );
                cell->loadOasis( cellElement, oasisStyles );
                bool haveStyle = cellElement.hasAttributeNS( KoXmlNS::table, "style-name" );
                //kdDebug()<<" haveStyle ? :"<<haveStyle<<endl;
                int cols = 1;
                if( cellElement.hasAttributeNS( KoXmlNS::table, "number-columns-repeated" ) )
                {
                    bool ok = false;
                    cols = cellElement.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt( &ok );
                    //kdDebug()<<" cols :"<<cols<<endl;
                    //TODO: correct ?????
                    if ( !haveStyle && ( cell->isEmpty() && cell->comment( columnIndex, backupRow ).isEmpty() ) )
                    {
                        //just increment it
                        columnIndex +=cols - 1;
                    }
                    else
                    {
                        if( ok )
                        {
                            for( int i = 0; i < cols; i++ )
                            {

                                if ( i != 0 )
                                {
                                    ++columnIndex;
                                    Cell* target = nonDefaultCell( columnIndex, backupRow );
                                    target->copyAll( cell );
                                }
                                //copy contains of cell of each col
                                for ( int newRow = backupRow+1; newRow < backupRow + number;++newRow )
                                {
                                    Cell* target = nonDefaultCell( columnIndex, newRow );
                                    target->copyAll( cell );
                                }

                            }
                        }
                        else
                        {
                            //just one cell
                            for ( int newRow = backupRow+1; newRow < backupRow + number;++newRow )
                            {
                                Cell* target = nonDefaultCell( columnIndex, newRow );
                                target->copyAll( cell );
                            }
                        }
                    }
                }
                else
                {
                    if ( haveStyle )
                    {
                        //just one cell
                        for ( int newRow = backupRow+1; newRow < backupRow + number;++newRow )
                        {
                            Cell* target = nonDefaultCell( columnIndex, newRow );
                            target->copyAll( cell );
                        }
                    }
                }
            }
        }
        cellNode = cellNode.nextSibling();
    }

    return true;
}

QString Sheet::translateOpenCalcPoint( const QString & str )
{
    bool inQuote = false;

    int l = str.length();
    int colonPos = -1;
    QString range;
    bool isRange = false;
    // replace '.' with '!'
    for ( int i = 0; i < l; ++i )
    {
        if ( str[i] == '$' )
            continue;
        if ( str[i] == '\'' )
        {
            inQuote = !inQuote;
        }
        else if ( str[i] == '.' )
        {
            if ( !inQuote )
            {
                if ( i != 0 && i != (colonPos + 1) ) // no empty sheet names
                    range += '!';
            }
            else
                range += '.';
        }
        else if ( str[i] == ':' )
        {
            if ( !inQuote )
            {
                isRange  = true;
                colonPos = i;
            }
            range += ':';
        }
        else
            range += str[i];
    }
    return range;
}

void Sheet::maxRowCols( int & maxCols, int & maxRows )
{
  const Cell * cell = firstCell();
  while ( cell )
  {
    if ( cell->column() > maxCols )
      maxCols = cell->column();

    if ( cell->row() > maxRows )
      maxRows = cell->row();

    cell = cell->nextCell();
  }

  const RowFormat * row = firstRow();
  while ( row )
  {
    if ( row->row() > maxRows )
      maxRows = row->row();

    row = row->next();
  }
  const ColumnFormat* col = firstCol();
  while ( col )
  {
    if ( col->column() > maxCols )
      maxCols = col->column();

    col = col->next();
  }
}


void Sheet::saveOasisHeaderFooter( KoXmlWriter &xmlWriter ) const
{
    QString headerLeft = print()->headLeft();
    QString headerCenter= print()->headMid();
    QString headerRight = print()->headRight();

    QString footerLeft = print()->footLeft();
    QString footerCenter= print()->footMid();
    QString footerRight = print()->footRight();

    xmlWriter.startElement( "style:header");
    if ( ( !headerLeft.isEmpty() )
         || ( !headerCenter.isEmpty() )
         || ( !headerRight.isEmpty() ) )
    {
        xmlWriter.startElement( "style:region-left" );
        xmlWriter.startElement( "text:p" );
        convertPart( headerLeft, xmlWriter );
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement( "style:region-center" );
        xmlWriter.startElement( "text:p" );
        convertPart( headerCenter, xmlWriter );
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement( "style:region-right" );
        xmlWriter.startElement( "text:p" );
        convertPart( headerRight, xmlWriter );
        xmlWriter.endElement();
        xmlWriter.endElement();
    }
    else
    {
       xmlWriter.startElement( "text:p" );

       xmlWriter.startElement( "text:sheet-name" );
       xmlWriter.addTextNode( "???" );
       xmlWriter.endElement();

       xmlWriter.endElement();
    }
    xmlWriter.endElement();


    xmlWriter.startElement( "style:footer");
    if ( ( !footerLeft.isEmpty() )
         || ( !footerCenter.isEmpty() )
         || ( !footerRight.isEmpty() ) )
    {
        xmlWriter.startElement( "style:region-left" );
        xmlWriter.startElement( "text:p" );
        convertPart( footerLeft, xmlWriter );
        xmlWriter.endElement();
        xmlWriter.endElement(); //style:region-left

        xmlWriter.startElement( "style:region-center" );
        xmlWriter.startElement( "text:p" );
        convertPart( footerCenter, xmlWriter );
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement( "style:region-right" );
        xmlWriter.startElement( "text:p" );
        convertPart( footerRight, xmlWriter );
        xmlWriter.endElement();
        xmlWriter.endElement();
    }
    else
    {

       xmlWriter.startElement( "text:p" );

       xmlWriter.startElement( "text:sheet-name" );
       xmlWriter.addTextNode( "Page " ); // ???

       xmlWriter.startElement( "text:page-number" );
       xmlWriter.addTextNode( "1" ); // ???
       xmlWriter.endElement();


       xmlWriter.endElement();

       xmlWriter.endElement();
    }
    xmlWriter.endElement();


}

void Sheet::addText( const QString & text, KoXmlWriter & writer ) const
{
    if ( !text.isEmpty() )
        writer.addTextNode( text );
}

void Sheet::convertPart( const QString & part, KoXmlWriter & xmlWriter ) const
{
    QString text;
    QString var;

    bool inVar = false;
    uint i = 0;
    uint l = part.length();
    while ( i < l )
    {
        if ( inVar || part[i] == '<' )
        {
            inVar = true;
            var += part[i];
            if ( part[i] == '>' )
            {
                inVar = false;
                if ( var == "<page>" )
                {
                    addText( text, xmlWriter );
                    xmlWriter.startElement( "text:page-number" );
                    xmlWriter.addTextNode( "1" );
                    xmlWriter.endElement();
                }
                else if ( var == "<pages>" )
                {
                    addText( text, xmlWriter );
                    xmlWriter.startElement( "text:page-count" );
                    xmlWriter.addTextNode( "99" ); //TODO I think that it can be different from 99
                    xmlWriter.endElement();
                }
                else if ( var == "<date>" )
                {
                    addText( text, xmlWriter );
                    //text:p><text:date style:data-style-name="N2" text:date-value="2005-10-02">02/10/2005</text:date>, <text:time>10:20:12</text:time></text:p> "add style" => create new style
#if 0 //FIXME
                    QDomElement t = dd.createElement( "text:date" );
                    t.setAttribute( "text:date-value", "0-00-00" );
                    // todo: "style:data-style-name", "N2"
                    t.appendChild( dd.createTextNode( QDate::currentDate().toString() ) );
                    parent.appendChild( t );
#endif
                }
                else if ( var == "<time>" )
                {
                    addText( text, xmlWriter );

                    xmlWriter.startElement( "text:time" );
                    xmlWriter.addTextNode( QTime::currentTime().toString() );
                    xmlWriter.endElement();
                }
                else if ( var == "<file>" ) // filepath + name
                {
                    addText( text, xmlWriter );
                    xmlWriter.startElement( "text:file-name" );
                    xmlWriter.addAttribute( "text:display", "full" );
                    xmlWriter.addTextNode( "???" );
                    xmlWriter.endElement();
                }
                else if ( var == "<name>" ) // filename
                {
                    addText( text, xmlWriter );

                    xmlWriter.startElement( "text:title" );
                    xmlWriter.addTextNode( "???" );
                    xmlWriter.endElement();
                }
                else if ( var == "<author>" )
                {
                    Doc* sdoc = d->workbook->doc();
                    KoDocumentInfo       * docInfo    = sdoc->documentInfo();
                    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor*>( docInfo->page( "author" ) );

                    text += authorPage->fullName();

                    addText( text, xmlWriter );
                }
                else if ( var == "<email>" )
                {
                    Doc* sdoc = d->workbook->doc();
                    KoDocumentInfo       * docInfo    = sdoc->documentInfo();
                    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor*>( docInfo->page( "author" ) );

                    text += authorPage->email();
                    addText( text, xmlWriter );

                }
                else if ( var == "<org>" )
                {
                    Doc* sdoc = d->workbook->doc();
                    KoDocumentInfo       * docInfo    = sdoc->documentInfo();
                    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor*>( docInfo->page( "author" ) );

                    text += authorPage->company();
                    addText( text, xmlWriter );

                }
                else if ( var == "<sheet>" )
                {
                    addText( text, xmlWriter );

                    xmlWriter.startElement( "text:sheet-name" );
                    xmlWriter.addTextNode( "???" );
                    xmlWriter.endElement();
                }
                else
                {
                    // no known variable:
                    text += var;
                    addText( text, xmlWriter );
                }

                text = "";
                var  = "";
            }
        }
        else
        {
            text += part[i];
        }
        ++i;
    }
    if ( !text.isEmpty() || !var.isEmpty() )
    {
        //we don't have var at the end =>store it
        addText( text+var, xmlWriter );
    }
    kdDebug()<<" text end :"<<text<<" var :"<<var<<endl;
}


void Sheet::loadOasisSettings( const KoOasisSettings::NamedMap &settings )
{
    // Find the entry in the map that applies to this sheet (by name)
    KoOasisSettings::Items items = settings.entry( d->name );
    if ( items.isNull() )
        return;
    d->hideZero = items.parseConfigItemBool( "ShowZeroValues" );
    d->showGrid = items.parseConfigItemBool( "ShowGrid" );
    d->firstLetterUpper = items.parseConfigItemBool( "FirstLetterUpper" );

    int cursorX = items.parseConfigItemInt( "CursorPositionX" );
    int cursorY = items.parseConfigItemInt( "CursorPositionY" );

    doc()->loadingInfo()->addMarkerSelection( this, QPoint( cursorX, cursorY ) );
    kdDebug()<<"d->hideZero :"<<d->hideZero<<" d->showGrid :"<<d->showGrid<<" d->firstLetterUpper :"<<d->firstLetterUpper<<" cursorX :"<<cursorX<<" cursorY :"<<cursorY<< endl;

    d->showFormulaIndicator = items.parseConfigItemBool("ShowFormulaIndicator" );
    d->showPageBorders = items.parseConfigItemBool( "ShowPageBorders" );
    d->lcMode = items.parseConfigItemBool( "lcmode" );
    d->autoCalc = items.parseConfigItemBool( "autoCalc" );
    d->showColumnNumber = items.parseConfigItemBool( "ShowPageBorders" );
    d->firstLetterUpper = items.parseConfigItemBool( "FirstLetterUpper" );
}

void Sheet::saveOasisSettings( KoXmlWriter &settingsWriter, const QPoint& marker ) const
{
    //not into each page into oo spec
    settingsWriter.addConfigItem( "ShowZeroValues", d->hideZero );
    settingsWriter.addConfigItem( "ShowGrid", d->showGrid );
    //not define into oo spec

    settingsWriter.addConfigItem( "FirstLetterUpper", d->firstLetterUpper);

    //<config:config-item config:name="CursorPositionX" config:type="int">3</config:config-item>
    //<config:config-item config:name="CursorPositionY" config:type="int">34</config:config-item>
    settingsWriter.addConfigItem( "CursorPositionX", marker.x() );
    settingsWriter.addConfigItem( "CursorPositionY", marker.y() );

    settingsWriter.addConfigItem( "ShowFormulaIndicator", d->showFormulaIndicator );
    settingsWriter.addConfigItem( "ShowPageBorders",d->showPageBorders );
    settingsWriter.addConfigItem( "lcmode", d->lcMode );
    settingsWriter.addConfigItem( "autoCalc", d->autoCalc );
    settingsWriter.addConfigItem( "ShowPageNumber", d->showColumnNumber );
    settingsWriter.addConfigItem( "FirstLetterUpper", d->firstLetterUpper );
}


bool Sheet::saveOasis( KoXmlWriter & xmlWriter, KoGenStyles &mainStyles, GenValidationStyles &valStyle )
{
    int maxCols= 1;
    int maxRows= 1;
    xmlWriter.startElement( "table:table" );
    xmlWriter.addAttribute( "table:name", d->name );
    xmlWriter.addAttribute( "table:style-name", saveOasisSheetStyleName(mainStyles )  );
    if ( !d->password.isEmpty() )
    {
        xmlWriter.addAttribute("table:protected", "true" );
        QCString str = KCodecs::base64Encode( d->password );
        xmlWriter.addAttribute("table:protection-key", QString( str.data() ) );/* FIXME !!!!*/
    }
    QRect _printRange = d->print->printRange();
    if ( _printRange != ( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) ) )
    {
        QString range= convertRangeToRef( d->name, _printRange );
        kdDebug()<<" range : "<<range<<endl;
        xmlWriter.addAttribute( "table:print-ranges", range );
    }

    maxRowCols( maxCols, maxRows );
    saveOasisColRowCell( xmlWriter, mainStyles, maxCols, maxRows, valStyle );
    xmlWriter.endElement();
    return true;
}

void Sheet::saveOasisPrintStyleLayout( KoGenStyle &style ) const
{
    QString printParameter;
    if ( d->print->printGrid() )
        printParameter="grid ";
    if ( d->showFormula )
        printParameter="formulas ";
    if ( !printParameter.isEmpty() )
        style.addProperty( "style:print", printParameter );
}

QString Sheet::saveOasisSheetStyleName( KoGenStyles &mainStyles )
{
    KoGenStyle pageStyle( Doc::STYLE_PAGE, "table"/*FIXME I don't know if name is sheet*/ );

    KoGenStyle pageMaster( Doc::STYLE_PAGEMASTER );
    pageMaster.addAttribute( "style:page-layout-name", d->print->saveOasisSheetStyleLayout( mainStyles ) );

    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
    saveOasisHeaderFooter(elementWriter);

    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    pageMaster.addChildElement( "headerfooter", elementContents );
    pageStyle.addAttribute( "style:master-page-name", mainStyles.lookup( pageMaster, "Standard" ) );

    pageStyle.addProperty( "table:display", !d->hide );
    return mainStyles.lookup( pageStyle, "ta" );
}


void Sheet::saveOasisColRowCell( KoXmlWriter& xmlWriter, KoGenStyles &mainStyles, int maxCols, int maxRows, GenValidationStyles &valStyle )
{
    int i = 1;
    while ( i <= maxCols )
    {
        ColumnFormat * column = columnFormat( i );
        KoGenStyle styleCurrent( Doc::STYLE_COLUMN, "table-column" );
        styleCurrent.addPropertyPt( "style:column-width", column->dblWidth() );/*FIXME pt and not mm */
        styleCurrent.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

        //style default layout for column
        KoGenStyle styleColCurrent( Doc::STYLE_CELL, "table-cell" );
        column->saveOasisCellStyle(styleColCurrent,mainStyles );
        QString nameDefaultCellStyle = mainStyles.lookup( styleColCurrent, "ce" );


        bool hide = column->isHide();
        int j = i + 1;
        int repeated = 1;
        while ( j <= maxCols )
        {
            ColumnFormat *nextColumn = columnFormat( j );
            KoGenStyle nextStyle( Doc::STYLE_COLUMN, "table-column" );
            nextStyle.addPropertyPt( "style:column-width", nextColumn->dblWidth() );/*FIXME pt and not mm */
            nextStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

            KoGenStyle nextStyleCol( Doc::STYLE_CELL, "table-cell" );
            nextColumn->saveOasisCellStyle(nextStyleCol,mainStyles );
            QString nextNameDefaultCellStyle = mainStyles.lookup( nextStyleCol, "ce" );

            //FIXME all the time repeate == 2
            if ( ( nextStyle==styleCurrent ) && ( hide == nextColumn->isHide() ) && ( nextNameDefaultCellStyle == nameDefaultCellStyle ) )
                ++repeated;
            else
                break;
            ++j;
        }
        xmlWriter.startElement( "table:table-column" );
        xmlWriter.addAttribute( "table:style-name", mainStyles.lookup( styleCurrent, "co" ) );
        //FIXME doesn't create format if it's default format

        xmlWriter.addAttribute( "table:default-cell-style-name", nameDefaultCellStyle );//TODO fixme create style from cell
        if ( hide )
            xmlWriter.addAttribute( "table:visibility", "collapse" );

        if ( repeated > 1 )
            xmlWriter.addAttribute( "table:number-columns-repeated", repeated  );
        xmlWriter.endElement();
        i += repeated;
    }

    for ( i = 1; i <= maxRows; ++i )
    {
        const RowFormat * row = rowFormat( i );
        KoGenStyle rowStyle( Doc::STYLE_ROW, "table-row" );
        rowStyle.addPropertyPt( "style:row-height", row->dblHeight());/*FIXME pt and not mm */
        rowStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

        xmlWriter.startElement( "table:table-row" );
        xmlWriter.addAttribute( "table:style-name", mainStyles.lookup( rowStyle, "ro" ) );
        int repeated = 1;
        if ( !rowAsCell( i, maxRows ) )
        {
            bool hide = row->isHide();
            int j = i + 1;
            while ( j <= maxRows )
            {
                const RowFormat *nextRow = rowFormat( j );
                KoGenStyle nextStyle( Doc::STYLE_ROW, "table-row" );
                nextStyle.addPropertyPt( "style:row-height", nextRow->dblHeight() );/*FIXME pt and not mm */
                nextStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

                //FIXME all the time repeate == 2
                if ( ( nextStyle==rowStyle ) && ( hide == nextRow->isHide() ) &&!rowAsCell( j, maxRows ) )
                    ++repeated;
                else
                    break;
                ++j;
            }
            i += repeated-1; /*it's double incremented into loop for*/
            if ( row->isHide() )
                xmlWriter.addAttribute( "table:visibility", "collapse" );
            if (  repeated > 1 )
                xmlWriter.addAttribute( "table:number-rows-repeated", repeated  );
        }
        else
        {
            if ( row->isHide() )
                xmlWriter.addAttribute( "table:visibility", "collapse" );
            saveOasisCells(  xmlWriter, mainStyles, i, maxCols, valStyle );
        }
        xmlWriter.endElement();
    }
}

bool Sheet::rowAsCell( int row, int maxCols )
{
    int i = 1;
    while ( i <= maxCols )
    {
        Cell* cell = cellAt( i, row );
        if ( !cell->isDefault() )
            return true;
        i++;
    }
    return false;
}

void Sheet::saveOasisCells(  KoXmlWriter& xmlWriter, KoGenStyles &mainStyles, int row, int maxCols, GenValidationStyles &valStyle )
{
    int i = 1;
    while ( i <= maxCols )
    {
        int repeated = 1;
        Cell* cell = cellAt( i, row );
        cell->saveOasis( xmlWriter, mainStyles, row, i,  maxCols, repeated, valStyle );
        i += repeated;
    }
}

bool Sheet::loadXML( const QDomElement& sheet )
{
    bool ok = false;
    if ( !doc()->loadingInfo() ||  !doc()->loadingInfo()->loadTemplate() )
    {
        d->name = sheet.attribute( "name" );
        if ( d->name.isEmpty() )
        {
            doc()->setErrorMessage( i18n("Invalid document. Sheet name is empty.") );
            return false;
        }
    }

    bool detectDirection = true;
    d->layoutDirection = LeftToRight;
    QString layoutDir = sheet.attribute( "layoutDirection" );
    if( !layoutDir.isEmpty() )
    {
        if( layoutDir == "rtl" )
        {
           detectDirection = false;
           d->layoutDirection = RightToLeft;
        }
        else if( layoutDir == "ltr" )
        {
           detectDirection = false;
           d->layoutDirection = LeftToRight;
        }
        else
            kdDebug()<<" Direction not implemented : "<<layoutDir<<endl;
    }
    if( detectDirection )
       checkContentDirection( d->name );

    /* older versions of KSpread allowed all sorts of characters that
       the parser won't actually understand.  Replace these with '_'
       Also, the initial character cannot be a space.
    */
    if (d->name[0] == ' ')
    {
      d->name.remove(0,1);
    }
    for (unsigned int i=0; i < d->name.length(); i++)
    {
      if ( !(d->name[i].isLetterOrNumber() ||
             d->name[i] == ' ' || d->name[i] == '.' ||
             d->name[i] == '_'))
        {
        d->name[i] = '_';
      }
    }

    /* make sure there are no name collisions with the altered name */
    QString testName;
    QString baseName;
    int nameSuffix = 0;

    testName = d->name;
    baseName = d->name;

    /* so we don't panic over finding ourself in the follwing test*/
    d->name = "";
    while (workbook()->findSheet(testName) != NULL)
    {
      nameSuffix++;
      testName = baseName + '_' + QString::number(nameSuffix);
    }
    d->name = testName;

    kdDebug(36001)<<"Sheet::loadXML: table name="<<d->name<<endl;
    setName(d->name.utf8());
    (dynamic_cast<SheetIface*>(dcopObject()))->sheetNameHasChanged();

    if( sheet.hasAttribute( "grid" ) )
    {
        d->showGrid = (int)sheet.attribute("grid").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "printGrid" ) )
    {
        d->print->setPrintGrid( (bool)sheet.attribute("printGrid").toInt( &ok ) );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "printCommentIndicator" ) )
    {
        d->print->setPrintCommentIndicator( (bool)sheet.attribute("printCommentIndicator").toInt( &ok ) );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "printFormulaIndicator" ) )
    {
        d->print->setPrintFormulaIndicator( (bool)sheet.attribute("printFormulaIndicator").toInt( &ok ) );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "hide" ) )
    {
        d->hide = (bool)sheet.attribute("hide").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "showFormula" ) )
    {
        d->showFormula = (bool)sheet.attribute("showFormula").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    //Compatibility with KSpread 1.1.x
    if( sheet.hasAttribute( "formular" ) )
    {
        d->showFormula = (bool)sheet.attribute("formular").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "showFormulaIndicator" ) )
    {
        d->showFormulaIndicator = (bool)sheet.attribute("showFormulaIndicator").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "borders" ) )
    {
        d->showPageBorders = (bool)sheet.attribute("borders").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "lcmode" ) )
    {
        d->lcMode = (bool)sheet.attribute("lcmode").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if ( sheet.hasAttribute( "autoCalc" ) )
    {
        d->autoCalc = ( bool )sheet.attribute( "autoCalc" ).toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "columnnumber" ) )
    {
        d->showColumnNumber = (bool)sheet.attribute("columnnumber").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "hidezero" ) )
    {
        d->hideZero = (bool)sheet.attribute("hidezero").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "firstletterupper" ) )
    {
        d->firstLetterUpper = (bool)sheet.attribute("firstletterupper").toInt( &ok );
        // we just ignore 'ok' - if it didn't work, go on
    }

    // Load the paper layout
    QDomElement paper = sheet.namedItem( "paper" ).toElement();
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
        d->print->setPaperLayout( left, top, right, bottom, format, orientation );
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
      d->print->setHeadFootLine( hleft, hcenter, hright, fleft, fcenter, fright);
    }

      // load print range
      QDomElement printrange = sheet.namedItem( "printrange-rect" ).toElement();
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
        d->print->setPrintRange( QRect( QPoint( left, top ), QPoint( right, bottom ) ) );
      }

      // load print zoom
      if( sheet.hasAttribute( "printZoom" ) )
      {
        double zoom = sheet.attribute( "printZoom" ).toDouble( &ok );
        if ( ok )
        {
          d->print->setZoom( zoom );
        }
      }

      // load page limits
      if( sheet.hasAttribute( "printPageLimitX" ) )
      {
        int pageLimit = sheet.attribute( "printPageLimitX" ).toInt( &ok );
        if ( ok )
        {
          d->print->setPageLimitX( pageLimit );
        }
      }

      // load page limits
      if( sheet.hasAttribute( "printPageLimitY" ) )
      {
        int pageLimit = sheet.attribute( "printPageLimitY" ).toInt( &ok );
        if ( ok )
        {
          d->print->setPageLimitY( pageLimit );
        }
      }

    // Load the cells
    QDomNode n = sheet.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if ( !e.isNull() && e.tagName() == "cell" )
        {
            Cell *cell = new Cell( this, 0, 0 );
            if ( cell->load( e, 0, 0 ) )
                insertCell( cell );
            else
                delete cell; // Allow error handling: just skip invalid cells
        }
        else if ( !e.isNull() && e.tagName() == "row" )
        {
            RowFormat *rl = new RowFormat( this, 0 );
            if ( rl->load( e ) )
                insertRowFormat( rl );
            else
                delete rl;
        }
        else if ( !e.isNull() && e.tagName() == "column" )
        {
            ColumnFormat *cl = new ColumnFormat( this, 0 );
            if ( cl->load( e ) )
                insertColumnFormat( cl );
            else
                delete cl;
        }
        else if ( !e.isNull() && e.tagName() == "object" )
        {
            Child *ch = new Child( doc(), this );
            if ( ch->load( e ) )
                insertChild( ch );
            else
                delete ch;
        }
        else if ( !e.isNull() && e.tagName() == "chart" )
        {
            ChartChild *ch = new ChartChild( doc(), this );
            if ( ch->load( e ) )
                insertChild( ch );
            else
      {
    ch->setDeleted(true);
                delete ch;
      }
        }

        n = n.nextSibling();
    }

    // load print repeat columns
    QDomElement printrepeatcolumns = sheet.namedItem( "printrepeatcolumns" ).toElement();
    if ( !printrepeatcolumns.isNull() )
    {
        int left = printrepeatcolumns.attribute( "left" ).toInt();
        int right = printrepeatcolumns.attribute( "right" ).toInt();
        d->print->setPrintRepeatColumns( qMakePair( left, right ) );
    }

    // load print repeat rows
    QDomElement printrepeatrows = sheet.namedItem( "printrepeatrows" ).toElement();
    if ( !printrepeatrows.isNull() )
    {
        int top = printrepeatrows.attribute( "top" ).toInt();
        int bottom = printrepeatrows.attribute( "bottom" ).toInt();
        d->print->setPrintRepeatRows( qMakePair( top, bottom ) );
    }

    if( !sheet.hasAttribute( "borders1.2" ) )
    {
      convertObscuringBorders();
    }

    if ( sheet.hasAttribute( "protected" ) )
    {
      QString passwd = sheet.attribute( "protected" );

      if ( passwd.length() > 0 )
      {
        QCString str( passwd.latin1() );
        d->password = KCodecs::base64Decode( str );
      }
      else
        d->password = QCString( "" );
    }

    return true;
}


bool Sheet::loadChildren( KoStore* _store )
{
    QPtrListIterator<KoDocumentChild> it( doc()->children() );
    for( ; it.current(); ++it )
    {
        if ( ((Child*)it.current())->sheet() == this )
        {
            if ( !it.current()->loadDocument( _store ) )
                return false;
        }
    }

    return true;
}

void Sheet::setShowPageBorders( bool b )
{
    if ( b == d->showPageBorders )
        return;

    d->showPageBorders = b;
    emit sig_updateView( this );
}

void Sheet::addCellBinding( CellBinding *_bind )
{
  d->cellBindings.append( _bind );

  doc()->setModified( true );
}

void Sheet::removeCellBinding( CellBinding *_bind )
{
  d->cellBindings.removeRef( _bind );

  doc()->setModified( true );
}

Sheet* Sheet::findSheet( const QString & _name )
{
  if ( !workbook() )
    return 0L;

  return workbook()->findSheet( _name );
}

// ###### Torben: Use this one instead of d->cells.insert()
void Sheet::insertCell( Cell *_cell )
{

  d->cells.insert( _cell, _cell->column(), _cell->row() );

  if ( d->scrollBarUpdates )
  {
    checkRangeHBorder ( _cell->column() );
    checkRangeVBorder ( _cell->row() );
  }
}

void Sheet::insertColumnFormat( ColumnFormat *l )
{
  d->columns.insertElement( l, l->column() );
}

void Sheet::insertRowFormat( RowFormat *l )
{
  d->rows.insertElement( l, l->row() );
}

void Sheet::update()
{
  Cell* c = d->cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
    updateCell(c, c->column(), c->row());
  }
}

void Sheet::updateCellArea( const QRect &cellArea )
{
  if ( doc()->isLoading() || doc()->delayCalculation() || (!getAutoCalc()))
    return;

  setRegionPaintDirty( cellArea );
}

void Sheet::updateCell( Cell */*cell*/, int _column, int _row )
{
  QRect cellArea(QPoint(_column, _row), QPoint(_column, _row));

  updateCellArea(cellArea);
}

void Sheet::emit_updateRow( RowFormat *_format, int _row )
{
    if ( doc()->isLoading() )
        return;

    Cell* c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
      if ( c->row() == _row )
          c->setLayoutDirtyFlag( true );

    emit sig_updateVBorder( this );
    emit sig_updateView( this );
    emit sig_maxRow(maxRow());
    _format->clearDisplayDirtyFlag();
}

void Sheet::emit_updateColumn( ColumnFormat *_format, int _column )
{
    if ( doc()->isLoading() )
        return;

    Cell* c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
        if ( c->column() == _column )
            c->setLayoutDirtyFlag( true );

    emit sig_updateHBorder( this );
    emit sig_updateView( this );
    emit sig_maxColumn( maxColumn() );
    _format->clearDisplayDirtyFlag();
}

void Sheet::insertChart( const QRect& _rect, KoDocumentEntry& _e, const QRect& _data )
{
    kdDebug(36001) << "Creating document" << endl;
    KoDocument* dd = _e.createDoc();
    kdDebug(36001) << "Created" << endl;
    if ( !dd )
        // Error message is already displayed, so just return
        return;

    kdDebug(36001) << "NOW FETCHING INTERFACE" << endl;

    if ( !dd->initDoc(KoDocument::InitDocEmbedded) )
        return;

    ChartChild * ch = new ChartChild( doc(), this, dd, _rect );
    ch->setDataArea( _data );
    ch->update();
    ch->chart()->setCanChangeValue( false  );
    // doc()->insertChild( ch );
    //insertChild( ch );

    KoChart::WizardExtension * wiz = ch->chart()->wizardExtension();

    if ( wiz && wiz->show())
        insertChild( ch );
    else
    {
      ch->setDeleted(true);
        delete ch;
    }
}

void Sheet::insertChild( const QRect& _rect, KoDocumentEntry& _e )
{
    KoDocument* d = _e.createDoc( doc() );
    if ( !d )
    {
        kdDebug() << "Error inserting child!" << endl;
        return;
    }
    if ( !d->initDoc(KoDocument::InitDocEmbedded) )
        return;

    Child* ch = new Child( doc(), this, d, _rect );

    insertChild( ch );
}

void Sheet::insertChild( Child *_child )
{
    // m_lstChildren.append( _child );
    doc()->insertChild( _child );

    updateView( _child->boundingRect() );

    /* TODO - handle this */
//    emit sig_polygonInvalidated( _child->framePointArray() );
}

void Sheet::deleteChild( Child* child )
{
    QPointArray polygon = child->framePointArray();

    emit sig_removeChild( child );

    child->setDeleted(true);
    delete child;

    /** TODO - handle this */
//    emit sig_polygonInvalidated( polygon );
}

void Sheet::changeChildGeometry( Child *_child, const QRect& _rect )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );
}

/*
QPtrListIterator<Child> Sheet::childIterator()
{
  return QPtrListIterator<Child> ( m_lstChildren );
}
*/

bool Sheet::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;

    QPtrListIterator<KoDocumentChild> it( doc()->children() );
    for( ; it.current(); ++it )
    {
        if ( ((Child*)it.current())->sheet() == this )
        {
            QString path = QString( "%1/%2" ).arg( _path ).arg( i++ );
            if ( !it.current()->document()->saveToStore( _store, path ) )
                return false;
        }
    }
    return true;
}

Sheet::~Sheet()
{
    //kdDebug()<<" Sheet::~Sheet() :"<<this<<endl;
    s_mapSheets->remove( d->id );

    //when you remove all sheet (close file)
    //you must reinit s_id otherwise there is not
    //the good name between map and sheet
    if( s_mapSheets->count()==0)
      s_id=0L;

    Cell* c = d->cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->sheetDies();

    d->cells.clear(); // cells destructor needs sheet to still exist

    d->painter->end();
    delete d->painter;
    delete d->widget;

    delete d->defaultFormat;
    delete d->defaultCell;
    delete d->defaultRowFormat;
    delete d->defaultColumnFormat;
    delete d->print;
    delete d->dcop;

    delete d->dependencies;

    delete d;
}


void Sheet::checkRangeHBorder ( int _column )
{
    if ( d->scrollBarUpdates && _column > d->maxColumn )
    {
      d->maxColumn = _column;
      emit sig_maxColumn( _column );
    }
}

void Sheet::checkRangeVBorder ( int _row )
{
    if ( d->scrollBarUpdates && _row > d->maxRow )
    {
      d->maxRow = _row;
      emit sig_maxRow( _row );
    }
}


void Sheet::enableScrollBarUpdates( bool _enable )
{
  d->scrollBarUpdates = _enable;
}

DCOPObject* Sheet::dcopObject()
{
    if ( !d->dcop )
        d->dcop = new SheetIface( this );

    return d->dcop;
}

void Sheet::hideSheet(bool _hide)
{
    setHidden(_hide);
    if(_hide)
        emit sig_SheetHidden(this);
    else
        emit sig_SheetShown(this);
}

void Sheet::removeSheet()
{
    emit sig_SheetRemoved(this);
}

bool Sheet::setSheetName( const QString& name, bool init, bool /*makeUndo*/ )
{
    if ( workbook()->findSheet( name ) )
        return false;

    if ( isProtected() )
      return false;

    if ( d->name == name )
        return true;

    QString old_name = d->name;
    d->name = name;

    if ( init )
        return true;

    QPtrListIterator<Sheet> it( workbook()->sheetList() );
    for ( ; it.current(); ++it )
        it.current()->changeCellTabName( old_name, name );

    doc()->changeAreaSheetName( old_name, name );
    emit sig_nameChanged( this, old_name );

    setName(name.utf8());
    (dynamic_cast<SheetIface*>(dcopObject()))->sheetNameHasChanged();

    return true;
}


void Sheet::updateLocale()
{
  doc()->emitBeginOperation(true);
  setRegionPaintDirty(QRect(QPoint(1,1), QPoint(KS_colMax, KS_rowMax)));

  Cell* c = d->cells.firstCell();
  for( ;c; c = c->nextCell() )
  {
      QString _text = c->text();
      c->setDisplayText( _text );
  }
  emit sig_updateView( this );
  //  doc()->emitEndOperation();
}

Cell* Sheet::getFirstCellColumn(int col) const
{ return d->cells.getFirstCellColumn(col); }

Cell* Sheet::getLastCellColumn(int col) const
{ return d->cells.getLastCellColumn(col); }

Cell* Sheet::getFirstCellRow(int row) const
{ return d->cells.getFirstCellRow(row); }

Cell* Sheet::getLastCellRow(int row) const
{ return d->cells.getLastCellRow(row); }

Cell* Sheet::getNextCellUp(int col, int row) const
{ return d->cells.getNextCellUp(col, row); }

Cell* Sheet::getNextCellDown(int col, int row) const
{ return d->cells.getNextCellDown(col, row); }

Cell* Sheet::getNextCellLeft(int col, int row) const
{ return d->cells.getNextCellLeft(col, row); }

Cell* Sheet::getNextCellRight(int col, int row) const
{ return d->cells.getNextCellRight(col, row); }

void Sheet::convertObscuringBorders()
{
  /* a word of explanation here:
     beginning with KSpread 1.2 (actually, cvs of Mar 28, 2002), border information
     is stored differently.  Previously, for a cell obscuring a region, the entire
     region's border's data would be stored in the obscuring cell.  This caused
     some data loss in certain situations.  After that date, each cell stores
     its own border data, and prints it even if it is an obscured cell (as long
     as that border isn't across an obscuring border).
     Anyway, this function is used when loading a file that was stored with the
     old way of borders.  All new files have the sheet attribute "borders1.2" so
     if that isn't in the file, all the border data will be converted here.
     It's a bit of a hack but I can't think of a better way and it's not *that*
     bad of a hack.:-)
  */
  Cell* c = d->cells.firstCell();
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

void Sheet::setRegionPaintDirty( QRect const & region )
{
  QValueList<QRect>::iterator it  = d->paintDirtyList.begin();
  QValueList<QRect>::iterator end = d->paintDirtyList.end();

  while ( it != end )
  {
    if ( (*it).contains( region ) )
      return;

    ++it;
  }

  d->paintDirtyList.append( region );
}

void Sheet::clearPaintDirtyData()
{
  d->paintDirtyList.clear();
}

bool Sheet::cellIsPaintDirty( QPoint const & cell )
{
  QValueList<QRect>::iterator it;
  QValueList<QRect>::iterator end = d->paintDirtyList.end();
  bool found = false;

  /* Yes, this seems an inefficient method....I just want to get it working
     now then worry about optimization later (hash sheet?).
     And it might not matter -- this is going to be cleared every repaint
     of the screen, so it will never grow large
  */
  for ( it = d->paintDirtyList.begin(); it != end && !found; ++it )
  {
    found = (*it).contains( cell );
  }
  return found;
}

#ifndef NDEBUG
void Sheet::printDebug()
{
    int iMaxColumn = maxColumn();
    int iMaxRow = maxRow();

    kdDebug(36001) << "Cell | Content  | DataT | Text" << endl;
    Cell *cell;
    for ( int currentrow = 1 ; currentrow < iMaxRow ; ++currentrow )
    {
        for ( int currentcolumn = 1 ; currentcolumn < iMaxColumn ; currentcolumn++ )
        {
            cell = cellAt( currentcolumn, currentrow );
            if ( !cell->isDefault() && !cell->isEmpty() )
            {
                QString cellDescr = Cell::name( currentcolumn, currentrow );
                cellDescr = cellDescr.rightJustify( 4,' ' );
                //QString cellDescr = "Cell ";
                //cellDescr += QString::number(currentrow).rightJustify(3,'0') + ',';
                //cellDescr += QString::number(currentcolumn).rightJustify(3,'0') + ' ';
                cellDescr += " | ";
                cellDescr += cell->value().type();
                cellDescr += " | ";
                cellDescr += cell->text();
                if ( cell->isFormula() )
                    cellDescr += QString("  [result: %1]").arg( cell->value().asString() );
                kdDebug(36001) << cellDescr << endl;
            }
        }
    }
}
#endif

/**********************************************************
 *
 * Child
 *
 **********************************************************/

Child::Child( Doc *parent, Sheet *_sheet, KoDocument* doc, const QRect& geometry )
  : KoDocumentChild( parent, doc, geometry )
{
  m_pSheet = _sheet;
}

Child::Child( Doc *parent, Sheet *_sheet ) : KoDocumentChild( parent )
{
  m_pSheet = _sheet;
}


Child::~Child()
{
}

/**********************************************************
 *
 * ChartChild
 *
 **********************************************************/

ChartChild::ChartChild( Doc *_spread, Sheet *_sheet, KoDocument* doc, const QRect& geometry )
  : Child( _spread, _sheet, doc, geometry )
{
    m_pBinding = 0;
}

ChartChild::ChartChild( Doc *_spread, Sheet *_sheet )
  : Child( _spread, _sheet )
{
    m_pBinding = 0;
}

ChartChild::~ChartChild()
{
    if ( isDeleted() )
        delete m_pBinding;
}

void ChartChild::setDataArea( const QRect& _data )
{
    if ( m_pBinding == 0L )
        m_pBinding = new ChartBinding( m_pSheet, _data, this );
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
    if ( !Child::load( element ) )
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
    QDomElement element = Child::save( doc );
    element.setTagName( "chart" );

    element.setAttribute( "left-cell", m_pBinding->dataArea().left() );
    element.setAttribute( "right-cell", m_pBinding->dataArea().right() );
    element.setAttribute( "top-cell", m_pBinding->dataArea().top() );
    element.setAttribute( "bottom-cell", m_pBinding->dataArea().bottom() );

    return element;
}

bool ChartChild::loadDocument( KoStore* _store )
{
    bool res = Child::loadDocument( _store );
    if ( !res )
        return res;

    // Did we see a cell rectangle ?
    if ( !m_pBinding )
        return true;

    update();

    chart()->setCanChangeValue( false  );
    return true;
}

KoChart::Part* ChartChild::chart()
{

    assert( document()->inherits( "KoChart::Part" ) );
    return static_cast<KoChart::Part *>( document() );
}
