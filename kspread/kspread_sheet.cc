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

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include <qapplication.h>
#include <qbuffer.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <q3picture.h>
#include <qregexp.h>
#include <q3vbox.h>
#include <qmap.h>
//Added by qt3to4:
#include <QTextStream>
#include <Q3ValueList>
#include <Q3PtrList>
#include <QPixmap>

#include <kdebug.h>
#include <kcodecs.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <kmessagebox.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <kprinter.h>
#include <kurl.h>

#include <koChart.h>
#include <KoDom.h>
#include <KoDocumentInfo.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisSettings.h>
#include <KoOasisStyles.h>
#include <KoQueryTrader.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "commands.h"
#include "dependencies.h"
#include "selection.h"
#include "ksploadinginfo.h"
#include "ksprsavinginfo.h"
#include "kspread_canvas.h"
#include "kspread_cluster.h"
#include "kspread_condition.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_object.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "kspread_view.h"
#include "manipulator.h"
#include "manipulator_data.h"
#include "KSpreadTableIface.h"

#include "kspread_sheet.h"
#include "kspread_sheet.moc"

#define NO_MODIFICATION_POSSIBLE \
do { \
  KMessageBox::error( 0, i18n ( "You cannot change a protected sheet" ) ); return; \
} while(0)

namespace KSpread {

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

ChartBinding::ChartBinding( Sheet *_sheet, const QRect& _area, EmbeddedChart *_child )
    : CellBinding( _sheet, _area )
{
  m_child = _child;
}

ChartBinding::~ChartBinding()
{
}

void ChartBinding::cellChanged( Cell* /*changedCell*/ )
{
    if ( m_bIgnoreChanges )
        return;

	//Ensure display gets updated by marking all cells underneath the chart as
	//dirty

	const QRect chartGeometry = m_child->geometry().toQRect();

	double tmp;
  	int left = sheet()->leftColumn( chartGeometry.left() , tmp );
  	int top = sheet()->topRow( chartGeometry.top() , tmp );
	int right = sheet()->rightColumn( chartGeometry.right() );
	int bottom = sheet()->bottomRow( chartGeometry.bottom() );

	sheet()->setRegionPaintDirty( QRect(left,top,right-left,bottom-top) );

    //kDebug(36001) << m_rctDataArea << endl;

    // Get the chart and resize its data if necessary.
    //
    // FIXME: Only do this if he data actually changed size.
    KoChart::Part  *chart = m_child->chart();
    chart->resizeData( m_rctDataArea.height(), m_rctDataArea.width() );

    // Reset all the data, i.e. retransfer them to the chart.
    // This is definitely not the most efficient way to do this.
    //
    // FIXME: Find a way to do it with just the data that changed.
    Cell* cell;
    for ( int row = 0; row < m_rctDataArea.height(); row++ ) {
        for ( int col = 0; col < m_rctDataArea.width(); col++ ) {
            cell = m_pSheet->cellAt( m_rctDataArea.left() + col,
				     m_rctDataArea.top() + row );
            if ( cell && cell->value().isNumber() )
		chart->setCellData( row, col, cell->value().asFloat() );
            else if ( cell )
	        chart->setCellData( row, col, cell->value().asString() );
            else
	        chart->setCellData( row, col, KoChart::Value() );
        }
    }
    chart->analyzeHeaders( );

    // ######### Kalle may be interested in that, too
#if 0
    Chart::Range range;
    range.top = m_rctDataArea.top();
    range.left = m_rctDataArea.left();
    range.right = m_rctDataArea.right();
    range.bottom = m_rctDataArea.bottom();
    range.sheet = m_pSheet->name(); */

    //m_child->chart()->setData( matrix );

    // Force a redraw of the chart on all views

    /** TODO - replace the call below with something that will repaint this chart */
#endif
    //    sheet()->emit_polygonInvalidated( m_child->framePointArray() );
}


/******************************************************************/
/* Class: TextDrag                                               */
/******************************************************************/


TextDrag::TextDrag( QWidget * dragSource, const char * name )
    : Q3TextDrag( dragSource, name )
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
    return Q3TextDrag::encodedData( mime );
}

bool TextDrag::canDecode( QMimeSource* e )
{
  if ( e->provides( selectionMimeType() ) )
    return true;
  return Q3TextDrag::canDecode(e);
}

const char * TextDrag::format( int i ) const
{
  if ( i < 4 ) // HACK, but how to do otherwise ??
    return Q3TextDrag::format(i);
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

class Sheet::Private
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
  QByteArray password;


  bool showGrid;
  bool showFormula;
  bool showFormulaIndicator;
  bool showCommentIndicator;
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
  Region paintDirtyList;

  // to get font metrics
  QPainter *painter;
  QWidget *widget;

  // List of all cell bindings. For example charts use bindings to get
  // informed about changing cell contents.
  Q3PtrList<CellBinding> cellBindings;

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
Q3IntDict<Sheet>* Sheet::s_mapSheets;

Sheet* Sheet::find( int _id )
{
  if ( !s_mapSheets )
    return 0L;

  return (*s_mapSheets)[ _id ];
}

Sheet::Sheet (Map* map,
    const QString &sheetName, const char *_name )
  : QObject( map )
{
  setObjectName( _name );
  if ( s_mapSheets == 0L )
    s_mapSheets = new Q3IntDict<Sheet>;
  d = new Private;

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
  d->showCommentIndicator=true;
  d->showPageBorders = false;

  d->lcMode=false;
  d->showColumnNumber=false;
  d->hideZero=false;
  d->firstLetterUpper=false;
  d->autoCalc=true;
  // Get a unique name so that we can offer scripting
  if ( !_name )
  {
      QByteArray s;
      s.sprintf("Sheet%i", s_id );
      QObject::setObjectName( s.data() );
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

Map* Sheet::workbook() const
{
  return d->workbook;
}

Doc* Sheet::doc() const
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

bool Sheet::getShowCommentIndicator() const
{
    return d->showCommentIndicator;
}

void Sheet::setShowCommentIndicator(bool _indic)
{
    d->showCommentIndicator=_indic;
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
    //Avoid possible recalculation of dependancies if the auto calc setting hasn't changed
    if (d->autoCalc == _AutoCalc)
        return;

    //If enabling automatic calculation, make sure that the dependencies are up-to-date
    if (_AutoCalc == true)
    {
        updateAllDependencies();
        recalc();
    }

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

Format* Sheet::defaultFormat()
{
    return d->defaultFormat;
}

const Format* Sheet::defaultFormat() const
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

void Sheet::password( QByteArray & passwd ) const
{
    passwd = d->password;
}

bool Sheet::isProtected() const
{
    return !d->password.isNull();
}

void Sheet::setProtected( QByteArray const & passwd )
{
  d->password = passwd;
}

bool Sheet::checkPassword( QByteArray const & passwd ) const
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

int Sheet::numSelected() const
{
    int num = 0;

    Q3PtrListIterator<EmbeddedObject> it(  d->workbook->doc()->embeddedObjects() );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->sheet() == this && it.current()->isSelected() )
            num++;
    }

    return num;
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
      kDebug(36001) << "Sheet:leftColumn: invalid column (col: " << col + 1 << ")" << endl;
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
      kDebug(36001) << "Sheet:rightColumn: invalid column (col: " << col << ")" << endl;
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
            kDebug(36001) << "Sheet:topRow: invalid row (row: " << row + 1 << ")" << endl;
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
      kDebug(36001) << "Sheet:bottomRow: invalid row (row: " << row << ")" << endl;
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
      kDebug(36001) << "Sheet:columnPos: invalid column (col: " << col << ")" << endl;
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
      kDebug(36001) << "Sheet:rowPos: invalid row (row: " << row << ")" << endl;
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
    kDebug (36001) << "Sheet::cellAt: column range: (col: " << _column << ")" << endl;
  }
  if ( _row > KS_rowMax) {
    kDebug (36001) << "Sheet::cellAt: row out of range: (row: " << _row << ")" << endl;
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
  ProtectedCheck prot;
  prot.setSheet (this);
  prot.add (QPoint (_column, _row));
  if (prot.check())
    NO_MODIFICATION_POSSIBLE;

  DataManipulator *dm = new DataManipulator ();
  dm->setSheet (this);
  dm->setValue (_text);
  dm->setParsing (!asString);
  dm->add (QPoint (_column, _row));
  dm->execute ();

  /* PRE-MANIPULATOR CODE looked like this:
  TODO remove this after the new code works
  if ( !doc()->undoLocked() )
  {
      UndoSetText *undo = new UndoSetText( doc(), this, cell->text(), _column, _row,cell->formatType() );
      doc()->addCommand( undo );
  }

  // The cell will force a display refresh itself, so we dont have to care here.
  cell->setCellText( _text, asString );
  */

  //refresh anchor
  if(_text.at(0)=='!')
    emit sig_updateView( this, Region(_column,_row,_column,_row) );
}

void Sheet::setArrayFormula (Selection *selectionInfo, const QString &_text)
{
  // check protection
  ProtectedCheck prot;
  prot.setSheet (this);
  prot.add (*selectionInfo);
  if (prot.check())
    NO_MODIFICATION_POSSIBLE;

  // create and call the manipulator
  ArrayFormulaManipulator *afm = new ArrayFormulaManipulator;
  afm->setSheet (this);
  afm->setText (_text);
  afm->add (*selectionInfo);
  afm->execute ();

  /* PRE-MANIPULATOR CODE LOOKED LIKE THIS
  TODO remove this when the above code works
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
  */
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
        if ( !(c->isObscured() && c->isPartOfMerged()) )
            c->setCalcDirtyFlag();
    }
}

void Sheet::updateAllDependencies()
{
        for (Cell* cell = d->cells.firstCell() ; cell ; cell = cell->nextCell())
        {
            Point cellLocation;
            cellLocation.setSheet(cell->sheet());
            cellLocation.setRow(cell->row());
            cellLocation.setColumn(cell->column());
            d->dependencies->cellChanged(cellLocation);
        }
}

void Sheet::recalc()
{
    recalc(false);
}

void Sheet::recalc( bool force )
{
  ElapsedTime et( "Recalculating " + d->name, ElapsedTime::PrintOnlyTime );
  //  emitBeginOperation(true);
  //  setRegionPaintDirty(QRect(QPoint(1,1), QPoint(KS_colMax, KS_rowMax)));
  setCalcDirtyFlag();

  //If automatic calculation is disabled, don't recalculate unless the force flag has been
  //set.
  if ( !getAutoCalc() && !force )
        return;

  //If automatic calculation is disabled, the dependencies won't be up to date, so they need
  //to be recalculated.
  //FIXME:  Tomas, is there a more efficient way to do this?
  if ( !getAutoCalc() )
    updateAllDependencies();


  // (Tomas): actually recalc each cell
  // this is FAR from being perfect, dependencies will cause some to be
  // recalculated a LOT, but it's still better than otherwise, where
  // we get no recalc if the result stored in a file differs from the
  // current one - then we only obtain the correct result AFTER we scroll
  // to the cell ... recalc should actually ... recalc :)
  Cell* c;

  int count = 0;
  c = d->cells.firstCell();
  for( ; c; c = c->nextCell() )
    ++count;

  int cur = 0;
  int percent = -1;
  c = d->cells.firstCell();
  for( ; c; c = c->nextCell() )
  {
    c->calc (false);
    cur++;
    // some debug output to get some idea how damn slow this is ...
    if (cur*100/count != percent) {
      percent = cur*100/count;
//       kDebug() << "Recalc: " << percent << "%" << endl;
    }
  }

  //  emitEndOperation();
  emit sig_updateView( this );
}

void Sheet::valueChanged (Cell *cell)
{

  //TODO: call cell updating, when cell damaging implemented

  //prepare the Point structure
  Point c;
  c.setRow (cell->row());
  c.setColumn (cell->column());
  c.setSheet( this );

  //update dependencies
  if ( getAutoCalc() )
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

class UndoAction* Sheet::CellWorkerTypeA::createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region )
{
    QString title = getUndoTitle();
    return new UndoCellFormat( doc, sheet, region, title );
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
  Sheet::SelectionType result;

  doc()->emitBeginOperation();

  // see what is selected; if nothing, take marker position
  bool selected = !(selectionInfo->isSingular());

  // create an undo action
  if ( !doc()->undoLocked() )
  {
    UndoAction* undo = worker.createUndoAction(doc(), this, *selectionInfo);
    // test if the worker has an undo action
    if ( undo != 0 )
    {
      doc()->addCommand( undo );
    }
  }

  Region::ConstIterator endOfList(selectionInfo->constEnd());
  for (Region::ConstIterator it = selectionInfo->constBegin(); it != endOfList; ++it)
  {
    // see what is selected; if nothing, take marker position
    QRect range = (*it)->rect().normalized();

  int top = range.top();
  int left = range.left();
  int bottom = range.bottom();
  int right  = range.right();

  // create cells in rows if complete columns selected
  Cell * cell;
  Style * s = doc()->styleManager()->defaultStyle();

  if ( !worker.type_B && selected && util_isColumnSelected(range) )
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

  // complete rows selected ?
  if ( selected && util_isRowSelected(range) )
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
  else if ( selected && util_isColumnSelected(range) )
  {
    for ( int col = range.left(); col <= right; ++col )
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
            // kDebug() << "not default" << endl;
            worker.doWork( cell, true, x, y );
          }
        }
      }
    }
    result = CellRegion;
  }

  } // for Region::Elements

  // emitEndOperation();
  emit sig_updateView( this );

  if (worker.emit_signal)
  {
    emit sig_updateView( this, *selectionInfo );
  }

  return result;
}

void Sheet::setSelectionFont( Selection* selectionInfo,
                              const char *_font, int _size,
                              signed char _bold, signed char _italic,
                              signed char _underline, signed char _strike)
{
  FontManipulator* manipulator = new FontManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::SFont);
  manipulator->setFontFamily(_font);
  manipulator->setFontSize(_size);
  manipulator->setFontBold(_bold);
  manipulator->setFontItalic(_italic);
  manipulator->setFontStrike(_strike);
  manipulator->setFontUnderline(_underline);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::setSelectionSize(Selection* selectionInfo,
                              int _size)
{
  // TODO Stefan: Increase/Decrease font size still used?
  int size;
  Cell* c;
  QPoint marker(selectionInfo->marker());
  c = cellAt(marker);
  size = c->format()->textFontSize(marker.x(), marker.y());

  FontManipulator* manipulator = new FontManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::SFont);
  manipulator->setFontSize(_size+size);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}


struct SetSelectionUpperLowerWorker : public Sheet::CellWorker {
    int _type;
    Sheet   * _s;
    SetSelectionUpperLowerWorker( int type, Sheet * s )
      : Sheet::CellWorker( false ), _type( type ),  _s( s ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region )
    {
      return new UndoChangeAreaTextCell( doc, sheet, region );
    }
    bool testCondition( Cell* c ) {
  return ( !c->value().isNumber() && !c->value().isBoolean() &&!c->isFormula() && !c->isDefault()
     && !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
     && !c->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool, int, int )
    {
  cell->setDisplayDirtyFlag();
  if ( _type == -1 )
      cell->setCellText( (cell->text().toLower()));
  else if ( _type == 1 )
      cell->setCellText( (cell->text().toUpper()));
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

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
  return   new UndoChangeAreaTextCell( doc, sheet, region );
    }
    bool testCondition( Cell* c ) {
  return ( !c->value().isNumber() && !c->value().isBoolean() &&!c->isFormula() && !c->isDefault()
     && !c->text().isEmpty() && c->text()[0] != '*' && c->text()[0] != '!'
     && !c->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool, int, int )
    {

  cell->setDisplayDirtyFlag();
  QString tmp = cell->text();
  int len = tmp.length();
  cell->setCellText( (tmp.at(0).toUpper()+tmp.right(len-1)) );
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

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
        QString title=i18n("Vertical Text");
        return new UndoCellFormat( doc, sheet, region, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  cell->format()->setVerticalText( _b );
  cell->format()->setMultiRow( false );
  cell->format()->setAngle( 0 );
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

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
        QString title=i18n("Add Comment");
  return new UndoCellFormat( doc, sheet, region, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  cell->format()->setComment( _comment );
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionComment( Selection* selectionInfo,
                                        const QString &_comment)
{
    SetSelectionCommentWorker w( _comment );
    workOnCells( selectionInfo, w );
}


void Sheet::setSelectionAngle( Selection* selectionInfo,
                               int _value )
{
  AngleManipulator* manipulator = new AngleManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::SAngle);
  manipulator->setAngle(_value);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

struct SetSelectionRemoveCommentWorker : public Sheet::CellWorker {
    SetSelectionRemoveCommentWorker( ) : Sheet::CellWorker( false ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
        QString title=i18n("Remove Comment");
  return new UndoCellFormat( doc, sheet, region, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  cell->format()->setComment( "" );
  cell->clearDisplayDirtyFlag();
    }
};

void Sheet::setSelectionRemoveComment( Selection* selectionInfo )
{
  if (areaIsEmpty(*selectionInfo, Comment))
    return;

  SetSelectionRemoveCommentWorker w;
  workOnCells( selectionInfo, w );
}


void Sheet::setSelectionTextColor( Selection* selectionInfo,
                                   const QColor &tb_Color )
{
  FontColorManipulator* manipulator = new FontColorManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::STextPen);
  manipulator->setTextColor(tb_Color);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::setSelectionbgColor( Selection* selectionInfo,
                                 const QColor &bg_Color )
{
  BackgroundColorManipulator* manipulator = new BackgroundColorManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::SBackgroundColor);
  manipulator->setBackgroundColor(bg_Color);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}


struct SetSelectionBorderColorWorker : public Sheet::CellWorker {
    const QColor& bd_Color;
    SetSelectionBorderColorWorker( const QColor& _bd_Color ) : Sheet::CellWorker( false ), bd_Color( _bd_Color ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
        QString title=i18n("Change Border Color");
  return new UndoCellFormat( doc, sheet, region, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool, int, int ) {
  cell->setDisplayDirtyFlag();
  int it_Row = cell->row();
  int it_Col = cell->column();
  if ( cell->format()->topBorderStyle( it_Row, it_Col )!=Qt::NoPen )
    cell->format()->setTopBorderColor( bd_Color );
  if ( cell->format()->leftBorderStyle( it_Row, it_Col )!=Qt::NoPen )
    cell->format()->setLeftBorderColor( bd_Color );
  if ( cell->format()->fallDiagonalStyle( it_Row, it_Col )!=Qt::NoPen )
    cell->format()->setFallDiagonalColor( bd_Color );
  if ( cell->format()->goUpDiagonalStyle( it_Row, it_Col )!=Qt::NoPen )
    cell->format()->setGoUpDiagonalColor( bd_Color );
  if ( cell->format()->bottomBorderStyle( it_Row, it_Col )!=Qt::NoPen )
    cell->format()->setBottomBorderColor( bd_Color );
  if ( cell->format()->rightBorderStyle( it_Row, it_Col )!=Qt::NoPen )
    cell->format()->setRightBorderColor( bd_Color );
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

      if ( cell->isPartOfMerged() )
      {
        /* case 2. */
        cell = cell->obscuringCells().first();
        undoRegion.setLeft(qMin(undoRegion.left(), cell->column()));
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

      if ( cell->isPartOfMerged() )
      {
        cell = cell->obscuringCells().first();
        undoRegion.setTop(qMin(undoRegion.top(), cell->row()));
      }
      numberOfCells += cell->extraXCells();
      x += cell->extraXCells();
    }
    undoRegion.setBottom( _marker.y() );
    undoRegion.setRight( x - 1 );
  }

  kDebug() << "Saving undo information" << endl;

  if ( !doc()->undoLocked() )
  {
    UndoChangeAreaTextCell *undo = new
      UndoChangeAreaTextCell( doc(), this, undoRegion );
    doc()->addCommand( undo );
  }

  kDebug() << "Saving undo information done" << endl;

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

      if ( cell->isPartOfMerged() )
      {
        cell = cell->obscuringCells().first();
      }

      //      cell->setCellText(cellText.setNum( incr ));

      cell->setNumber( incr );
      if (mode == Column)
      {
        ++y;
        if (cell->doesMergeCells())
        {
          y += cell->extraYCells();
        }
      }
      else if (mode == Row)
      {
        ++x;
        if (cell->doesMergeCells())
        {
          x += cell->extraXCells();
        }
      }
      else
      {
        kDebug(36001) << "Error in Series::mode" << endl;
        return;
      }

      if (type == Linear)
        incr = incr + step;
      else if (type == Geometric)
        incr = incr * step;
      else
      {
        kDebug(36001) << "Error in Series::type" << endl;
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

      if (cell->isPartOfMerged())
      {
        cell = cell->obscuringCells().first();
      }

      //      cell->setCellText(cellText.setNum( incr ));
      cell->setNumber( incr );
      if (mode == Column)
      {
        ++y;
        if (cell->doesMergeCells())
        {
          y += cell->extraYCells();
        }
      }
      else if (mode == Row)
      {
        ++x;
        if (cell->doesMergeCells())
        {
          x += cell->extraXCells();
        }
      }
      else
      {
        kDebug(36001) << "Error in Series::mode" << endl;
        return;
      }

      if (type == Linear)
        incr = incr + step;
      else if (type == Geometric)
        incr = incr * step;
      else
      {
        kDebug(36001) << "Error in Series::type" << endl;
        return;
      }
    }
  }
  else
  {
    for ( incr = start; incr <= end; )
    {
      cell = nonDefaultCell( x, y, false, s );

      if (cell->isPartOfMerged())
      {
        cell = cell->obscuringCells().first();
      }

      //cell->setCellText(cellText.setNum( incr ));
      cell->setNumber( incr );
      if (mode == Column)
      {
        ++y;
        if (cell->doesMergeCells())
        {
          y += cell->extraYCells();
        }
      }
      else if (mode == Row)
      {
        ++x;
        if (cell->doesMergeCells())
        {
          x += cell->extraXCells();
        }
      }
      else
      {
        kDebug(36001) << "Error in Series::mode" << endl;
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
        kDebug(36001) << "Error in Series::type" << endl;
        return;
      }
    }
  }

  setRegionPaintDirty( undoRegion );

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
  cell->format()->clearProperty(Style::SFormatType);
  cell->format()->clearNoFallBackProperties( Style::SFormatType );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->format()->setFormatType( b ? Percentage_format : Generic_format);
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
      if (c->text().indexOf(tmp) != -1)
      {
        if ( !c->makeFormula() )
          kError(36001) << "ERROR: Syntax ERROR" << endl;
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
      if (c->text().indexOf(tmp) != -1)
      {
        if ( !c->makeFormula() )
          kError(36001) << "ERROR: Syntax ERROR" << endl;
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
            if(c->text().indexOf(old_name)!=-1)
            {
                int nb = c->text().count( old_name + "!" );
                QString tmp = old_name + "!";
                int len = tmp.length();
                tmp=c->text();

                for( int i=0; i<nb; i++ )
                {
                    int pos = tmp.indexOf( old_name + "!" );
                    tmp.replace( pos, len, new_name + "!" );
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
    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
    {
        for(int i = rect.top(); i <= rect.bottom(); i++ )
            it.current()->changeNameCellRef( QPoint( rect.left(), i ), false,
                                             Sheet::ColumnInsert, objectName(),
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
    {
        for(int i=rect.left();i<=rect.right();i++)
            it.current()->changeNameCellRef( QPoint( i, rect.top() ), false,
                                             Sheet::RowInsert, objectName(),
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        for(int i=rect.left();i<=rect.right();i++)
                it.current()->changeNameCellRef( QPoint( i, rect.top() ), false,
                                                 Sheet::RowRemove, objectName(),
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        for(int i=rect.top();i<=rect.bottom();i++)
                it.current()->changeNameCellRef( QPoint( rect.left(), i ), false,
                                                 Sheet::ColumnRemove, objectName(),
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true,
                                         Sheet::ColumnInsert, objectName(),
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true,
                                         Sheet::RowInsert, objectName(),
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

    for ( int i = 0; i <= nbCol; ++i )
    {
        // Recalculate range max (minus size of removed column)
        d->sizeMaxX -= columnFormat( col )->dblWidth();

        d->cells.removeColumn( col );
        d->columns.removeColumn( col );

        //Recalculate range max (plus size of new column)
        d->sizeMaxX += columnFormat( KS_colMax )->dblWidth();
    }

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( col, 1 ), true,
                                         Sheet::ColumnRemove, objectName(),
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for( ; it.current(); ++it )
        it.current()->changeNameCellRef( QPoint( 1, row ), true,
                                         Sheet::RowRemove, objectName(),
                                         nbRow + 1, undo );

    //update print settings
    d->print->removeRow( row, nbRow );

    refreshChart( QPoint( 1, row ), true, Sheet::RowRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void Sheet::hideRow(const Region& region)
{
  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet(this);
  manipulator->setManipulateRows(true);
  manipulator->add(region);
  manipulator->execute();
}

void Sheet::emitHideRow()
{
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void Sheet::showRow(const Region& region)
{
  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet(this);
  manipulator->setManipulateRows(true);
  manipulator->setReverse(true);
  manipulator->add(region);
  manipulator->execute();
}


void Sheet::hideColumn(const Region& region)
{
  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet(this);
  manipulator->setManipulateColumns(true);
  manipulator->add(region);
  manipulator->execute();
}

void Sheet::emitHideColumn()
{
    emit sig_updateHBorder( this );
    emit sig_updateView( this );
}

void Sheet::showColumn(const Region& region)
{
  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet(this);
  manipulator->setManipulateColumns(true);
  manipulator->setReverse(true);
  manipulator->add(region);
  manipulator->execute();
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
    if(c->doesMergeCells())
      c->mergeCells( c->column(), c->row(), c->extraXCells(), c->extraYCells() );
  }
}


void Sheet::changeNameCellRef( const QPoint & pos, bool fullRowOrColumn,
                                      ChangeRef ref, QString tabname, int nbCol,
                                      UndoInsertRemoveAction * undo )
{
  bool correctDefaultSheetName = (tabname == objectName()); // for cells without sheet ref (eg "A1")
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
              int col = point.pos().x();
              int row = point.pos().y();
              QString newPoint;

              // Update column
              if ( point.columnFixed() )
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
              if ( point.rowFixed() )
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
              kDebug(36001) << "Copying (unchanged) : '" << str << "'" << endl;
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
    bool bck = options & KFind::FindBackwards;

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( options & KFind::FromCursor ) {
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

void Sheet::borderBottom( Selection* selectionInfo, const QColor &_color )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setBottomBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::borderRight( Selection* selectionInfo, const QColor &_color )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setRightBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::borderLeft( Selection* selectionInfo, const QColor &_color )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setLeftBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::borderTop( Selection* selectionInfo, const QColor &_color )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setTopBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::borderOutline( Selection* selectionInfo, const QColor &_color )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setTopBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setBottomBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setLeftBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setRightBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::borderAll( Selection * selectionInfo,
                       const QColor & _color )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setTopBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setBottomBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setLeftBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setRightBorderPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setHorizontalPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->setVerticalPen(QPen(_color, 1, Qt::SolidLine));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::borderRemove( Selection* selectionInfo )
{
  BorderManipulator* manipulator = new BorderManipulator();
  manipulator->setSheet(this);
  manipulator->setTopBorderPen(QPen(Qt::NoPen));
  manipulator->setBottomBorderPen(QPen(Qt::NoPen));
  manipulator->setLeftBorderPen(QPen(Qt::NoPen));
  manipulator->setRightBorderPen(QPen(Qt::NoPen));
  manipulator->setHorizontalPen(QPen(Qt::NoPen));
  manipulator->setVerticalPen(QPen(Qt::NoPen));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}


void Sheet::sortByRow( const QRect &area, int ref_row, SortingOrder mode )
{
  Point point;
  point.setSheet(this);
  point.setSheetName (d->name);
  point.setPos(area.topLeft());
  point.setColumnFixed(false);
  point.setRowFixed(false);

  sortByRow( area, ref_row, 0, 0, mode, mode, mode, 0, false, false, point,true );
}

void Sheet::sortByColumn( const QRect &area, int ref_column, SortingOrder mode )
{
  Point point;
  point.setSheet(this);
  point.setSheetName(d->name);
  point.setPos(area.topLeft());
  point.setColumnFixed(false);
  point.setRowFixed(false);

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
  else if ( cell1->isObscured() && cell1->isPartOfMerged() )
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
            r.setRight(col);
          if ( col < r.left() )
            r.setLeft(col);
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

  QRect target( outputPoint.pos().x(), outputPoint.pos().y(), r.width(), r.height() );

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
    if ( cell1->isObscured() && cell1->isPartOfMerged() )
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
        int i1 = firstKey->indexOf( cell2->text() );
        int i2 = firstKey->indexOf( bestCell->text() );

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
            else if ( cell22->isObscured() && cell22->isPartOfMerged() )
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
              else if ( cell23->isObscured() && cell23->isPartOfMerged() )
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
            r.setBottom(row);
          if ( row < r.top() )
            r.setTop(row);
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
  QRect target( outputPoint.pos().x(), outputPoint.pos().y(), r.width(), r.height() );

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
    if ( cell1->isObscured() && cell1->isPartOfMerged() )
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
      else if ( cell2->isObscured() && cell2->isPartOfMerged() )
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
        int i1 = firstKey->indexOf( cell2->text() );
        int i2 = firstKey->indexOf( bestCell->text() );

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
            else if ( cell22->isObscured() && cell22->isPartOfMerged() )
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
        else if ( cell22->isObscured() && cell22->isPartOfMerged() )
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
          else if ( cell23->isObscured() && cell23->isPartOfMerged() )
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
    targetCell->setAlign( sourceCell->format()->align( x1, y1 ) );
    targetCell->setAlignY( sourceCell->format()->alignY( x1, y1 ) );
    targetCell->setTextFont( sourceCell->format()->textFont( x1, y1 ) );
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
    targetCell->format()->setPrefix( sourceCell->prefix( x1, y1 ) );
    targetCell->format()->setPostfix( sourceCell->postfix( x1, y1 ) );
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
    Style::HAlign a = ref1->format()->align( ref1->column(), ref1->row() );
    ref1->format()->setAlign( ref2->format()->align( ref2->column(), ref2->row() ) );
    ref2->format()->setAlign(a);

    Style::VAlign ay = ref1->format()->alignY( ref1->column(), ref1->row() );
    ref1->format()->setAlignY( ref2->format()->alignY( ref2->column(), ref2->row() ) );
    ref2->format()->setAlignY(ay);

    QFont textFont = ref1->format()->textFont( ref1->column(), ref1->row() );
    ref1->format()->setTextFont( ref2->format()->textFont( ref2->column(), ref2->row() ) );
    ref2->format()->setTextFont(textFont);

    QColor textColor = ref1->format()->textColor( ref1->column(), ref1->row() );
    ref1->format()->setTextColor( ref2->format()->textColor( ref2->column(), ref2->row() ) );
    ref2->format()->setTextColor(textColor);

    QColor bgColor = ref1->bgColor( ref1->column(), ref1->row() );
    ref1->format()->setBgColor( ref2->bgColor( ref2->column(), ref2->row() ) );
    ref2->format()->setBgColor(bgColor);

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

    QPen fdp = ref1->format()->fallDiagonalPen( ref1->column(), ref1->row() );
    ref1->format()->setFallDiagonalPen( ref2->format()->fallDiagonalPen( ref2->column(), ref2->row() ) );
    ref2->format()->setFallDiagonalPen(fdp);

    QPen udp = ref1->format()->goUpDiagonalPen( ref1->column(), ref1->row() );
    ref1->format()->setGoUpDiagonalPen( ref2->format()->goUpDiagonalPen( ref2->column(), ref2->row() ) );
    ref2->format()->setGoUpDiagonalPen(udp);

    QBrush bgBrush = ref1->backGroundBrush( ref1->column(), ref1->row() );
    ref1->format()->setBackGroundBrush( ref2->backGroundBrush( ref2->column(), ref2->row() ) );
    ref2->format()->setBackGroundBrush(bgBrush);

    int pre = ref1->format()->precision( ref1->column(), ref1->row() );
    ref1->format()->setPrecision( ref2->format()->precision( ref2->column(), ref2->row() ) );
    ref2->format()->setPrecision(pre);

    QString prefix = ref1->format()->prefix( ref1->column(), ref1->row() );
    ref1->format()->setPrefix( ref2->format()->prefix( ref2->column(), ref2->row() ) );
    ref2->format()->setPrefix(prefix);

    QString postfix = ref1->format()->postfix( ref1->column(), ref1->row() );
    ref1->format()->setPostfix( ref2->format()->postfix( ref2->column(), ref2->row() ) );
    ref2->format()->setPostfix(postfix);

    Style::FloatFormat f = ref1->format()->floatFormat( ref1->column(), ref1->row() );
    ref1->format()->setFloatFormat( ref2->format()->floatFormat( ref2->column(), ref2->row() ) );
    ref2->format()->setFloatFormat(f);

    Style::FloatColor c = ref1->format()->floatColor( ref1->column(), ref1->row() );
    ref1->format()->setFloatColor( ref2->format()->floatColor( ref2->column(), ref2->row() ) );
    ref2->format()->setFloatColor(c);

    bool multi = ref1->format()->multiRow( ref1->column(), ref1->row() );
    ref1->format()->setMultiRow( ref2->format()->multiRow( ref2->column(), ref2->row() ) );
    ref2->format()->setMultiRow(multi);

    bool vert = ref1->format()->verticalText( ref1->column(), ref1->row() );
    ref1->format()->setVerticalText( ref2->format()->verticalText( ref2->column(), ref2->row() ) );
    ref2->format()->setVerticalText(vert);

    bool print = ref1->format()->getDontprintText( ref1->column(), ref1->row() );
    ref1->format()->setDontPrintText( ref2->format()->getDontprintText( ref2->column(), ref2->row() ) );
    ref2->format()->setDontPrintText(print);

    double ind = ref1->format()->getIndent( ref1->column(), ref1->row() );
    ref1->format()->setIndent( ref2->format()->getIndent( ref2->column(), ref2->row() ) );
    ref2->format()->setIndent( ind );

    QLinkedList<Conditional> conditionList = ref1->conditionList();
    ref1->setConditionList(ref2->conditionList());
    ref2->setConditionList(conditionList);

    QString com = ref1->format()->comment( ref1->column(), ref1->row() );
    ref1->format()->setComment( ref2->format()->comment( ref2->column(), ref2->row() ) );
    ref2->format()->setComment(com);

    int angle = ref1->format()->getAngle( ref1->column(), ref1->row() );
    ref1->format()->setAngle( ref2->format()->getAngle( ref2->column(), ref2->row() ) );
    ref2->format()->setAngle(angle);

    FormatType form = ref1->format()->getFormatType( ref1->column(), ref1->row() );
    ref1->format()->setFormatType( ref2->format()->getFormatType( ref2->column(), ref2->row() ) );
    ref2->format()->setFormatType(form);
  }
}

void Sheet::refreshPreference()
{
  if ( getAutoCalc() )
    recalc();

  emit sig_updateHBorder( this );
  emit sig_updateView( this );
}


bool Sheet::areaIsEmpty(const Region& region, TestType _type)
{
  Region::ConstIterator endOfList = region.constEnd();
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalized();
    // Complete rows selected ?
    if ((*it)->isRow())
    {
        for ( int row = range.top(); row <= range.bottom(); ++row )
        {
            Cell * c = getFirstCellRow( row );
            while ( c )
            {
                if ( !c->isPartOfMerged())
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
                        if ( !c->format()->comment(c->column(), row).isEmpty())
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
    else if ((*it)->isColumn())
    {
        for ( int col = range.left(); col <= range.right(); ++col )
        {
            Cell * c = getFirstCellColumn( col );
            while ( c )
            {
                if ( !c->isPartOfMerged() )
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
                        if ( !c->format()->comment(col, c->row()).isEmpty())
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

        int right  = range.right();
        int bottom = range.bottom();
        for ( int x = range.left(); x <= right; ++x )
            for ( int y = range.top(); y <= bottom; ++y )
            {
                cell = cellAt( x, y );
                if (!cell->isPartOfMerged() )
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
                        if ( !cell->format()->comment(x, y).isEmpty())
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
  }
  return true;
}

struct SetSelectionMultiRowWorker : public Sheet::CellWorker
{
  bool enable;
  SetSelectionMultiRowWorker( bool _enable )
    : Sheet::CellWorker( ), enable( _enable ) { }

  class UndoAction* createUndoAction( Doc * doc, Sheet * sheet, const KSpread::Region& region )
  {
    QString title = i18n("Multirow");
    return new UndoCellFormat( doc, sheet, region, title );
  }

  bool testCondition( Cell * cell )
  {
    return ( !cell->isPartOfMerged() );
  }

  void doWork( Cell * cell, bool, int, int )
  {
    cell->setDisplayDirtyFlag();
    cell->format()->setMultiRow( enable );
    cell->format()->setVerticalText( false );
    cell->format()->setAngle( 0 );
    cell->clearDisplayDirtyFlag();
  }
};

void Sheet::setSelectionMultiRow( Selection* selectionInfo,
                                         bool enable )
{
    SetSelectionMultiRowWorker w( enable );
    workOnCells( selectionInfo, w );
}

QString Sheet::guessColumnTitle(QRect& area, int col)
{
  //Verify range
  Range rg;
  rg.setRange(area);
  rg.setSheet(this);

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
  rg.setSheet(this);

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
                               Style::HAlign _align )
{
  HorAlignManipulator* manipulator = new HorAlignManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::SHAlign);
  manipulator->setHorizontalAlignment(_align);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}

void Sheet::setSelectionAlignY( Selection* selectionInfo,
                                Style::VAlign _alignY )
{
  VerAlignManipulator* manipulator = new VerAlignManipulator();
  manipulator->setSheet(this);
  manipulator->setProperty(Style::SVAlign);
  manipulator->setVerticalAlignment(_alignY);
  manipulator->add(*selectionInfo);
  manipulator->execute();
}


struct SetSelectionPrecisionWorker : public Sheet::CellWorker {
    int _delta;
    SetSelectionPrecisionWorker( int delta ) : Sheet::CellWorker( ), _delta( delta ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
        QString title=i18n("Change Precision");
  return new UndoCellFormat( doc, sheet, region, title );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
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
    return ( !cell->isPartOfMerged() && cell->format()->style() != m_style );
  }

  void doWork( Cell* cell, bool cellRegion, int, int )
  {
    if ( cellRegion )
      cell->setDisplayDirtyFlag();

    cell->format()->setStyle( m_style );

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
  return ( rw->hasProperty( Style::SFormatType )
     || rw->hasProperty( Style::SPrecision ) );
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
  c->format()->clearProperty( Style::SPrecision );
  c->format()->clearNoFallBackProperties( Style::SPrecision );
  c->format()->clearProperty( Style::SFormatType );
  c->format()->clearNoFallBackProperties( Style::SFormatType );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool cellRegion, int, int ) {
  if ( cellRegion )
      cell->setDisplayDirtyFlag();
  cell->format()->setFormatType( b ? Money_format : Generic_format );
  cell->format()->setPrecision( b ?  m_pDoc->locale()->fracDigits() : 0 );
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
  return ( rw->hasProperty( Style::SIndent ) );
    }

    void doWork( RowFormat* rw ) {
  rw->setIndent( tmpIndent+valIndent );
  //rw->setAlign( Style::Left );
    }
    void doWork( ColumnFormat* cl ) {
  cl->setIndent( tmpIndent+valIndent );
  //cl->setAlign( Style::Left );
    }
    void prepareCell( Cell* c ) {
  c->format()->clearProperty( Style::SIndent );
  c->format()->clearNoFallBackProperties( Style::SIndent );
  //c->format()->clearProperty( Style::SAlign );
  //c->format()->clearNoFallBackProperties( Style::SAlign );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool cellRegion, int x, int y ) {
  if ( cellRegion ) {
      if(cell->format()->align(x,y)!=Style::Left)
      {
    //cell->setAlign(Style::Left);
    //cell->format()->setIndent( 0.0 );
      }
      cell->setDisplayDirtyFlag();
      cell->format()->setIndent( /* ### ??? --> */ cell->format()->getIndent(x,y) /* <-- */ +valIndent );
      cell->clearDisplayDirtyFlag();
  } else {
      cell->format()->setIndent( tmpIndent+valIndent);
      //cell->setAlign( Style::Left);
  }
    }
};


void Sheet::increaseIndent(Selection* selectionInfo)
{
    QPoint       marker(selectionInfo->marker());
    double       valIndent = doc()->getIndentValue();
    Cell *c         = cellAt( marker );
    double       tmpIndent = c->format()->getIndent( marker.x(), marker.y() );

    IncreaseIndentWorker  w( tmpIndent, valIndent );
    workOnCells( selectionInfo, w );
}


struct DecreaseIndentWorker : public Sheet::CellWorkerTypeA {
    double tmpIndent, valIndent;
    DecreaseIndentWorker( double _tmpIndent, double _valIndent ) : tmpIndent( _tmpIndent ), valIndent( _valIndent ) { }
    QString getUndoTitle() { return i18n("Decrease Indent"); }
    bool testCondition( RowFormat* rw ) {
  return ( rw->hasProperty( Style::SIndent ) );
    }
    void doWork( RowFormat* rw ) {
        rw->setIndent( qMax( 0.0, tmpIndent - valIndent ) );
    }
    void doWork( ColumnFormat* cl ) {
        cl->setIndent( qMax( 0.0, tmpIndent - valIndent ) );
    }
    void prepareCell( Cell* c ) {
  c->format()->clearProperty( Style::SIndent );
  c->format()->clearNoFallBackProperties( Style::SIndent );
    }
    bool testCondition( Cell* cell ) {
  return ( !cell->isPartOfMerged() );
    }
    void doWork( Cell* cell, bool cellRegion, int x, int y ) {
  if ( cellRegion ) {
      cell->setDisplayDirtyFlag();
      cell->format()->setIndent( qMax( 0.0, cell->format()->getIndent( x, y ) - valIndent ) );
      cell->clearDisplayDirtyFlag();
  } else {
      cell->format()->setIndent( qMax( 0.0, tmpIndent - valIndent ) );
  }
    }
};


void Sheet::decreaseIndent( Selection* selectionInfo )
{
    double valIndent = doc()->getIndentValue();
    QPoint marker(selectionInfo->marker());
    Cell* c = cellAt( marker );
    double tmpIndent = c->format()->getIndent( marker.x(), marker.y() );

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
        int a = c->format()->align( c->column(), c->row() );
        if ( a == Style::HAlignUndefined )
        {
            if ( c->value().isNumber() || c->isDate() || c->isTime())
                a = Style::Right;
            else
                a = Style::Left;
        }

        if ( a == Style::Left )
            indent = c->format()->getIndent( c->column(), c->row() );
        long_max = indent + c->textWidth()
            + c->format()->leftBorderWidth( c->column(), c->row() )
            + c->format()->rightBorderWidth( c->column(), c->row() );
    }
    return (int)long_max;
}

void Sheet::adjustArea(const Region& region)
{
  AdjustColumnRowManipulator* manipulator = new AdjustColumnRowManipulator();
  manipulator->setSheet(this);
  manipulator->setAdjustColumn(true);
  manipulator->setAdjustRow(true);
  manipulator->add(region);
  manipulator->execute();
}

void Sheet::adjustColumn(const Region& region)
{
  AdjustColumnRowManipulator* manipulator = new AdjustColumnRowManipulator();
  manipulator->setSheet(this);
  manipulator->setAdjustColumn(true);
  manipulator->add(region);
  manipulator->execute();
}

void Sheet::adjustRow(const Region& region)
{
  AdjustColumnRowManipulator* manipulator = new AdjustColumnRowManipulator();
  manipulator->setSheet(this);
  manipulator->setAdjustRow(true);
  manipulator->add(region);
  manipulator->execute();
}

struct ClearTextSelectionWorker : public Sheet::CellWorker {
    Sheet   * _s;

    ClearTextSelectionWorker(  Sheet * s )
      : Sheet::CellWorker( ),  _s( s ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
  return new UndoChangeAreaTextCell( doc, sheet, region );
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
  if (areaIsEmpty(*selectionInfo))
    return;

  ClearTextSelectionWorker w( this );
  workOnCells( selectionInfo, w );
}


struct ClearValiditySelectionWorker : public Sheet::CellWorker {
    ClearValiditySelectionWorker( ) : Sheet::CellWorker( ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
  return new UndoConditional( doc, sheet, region );
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
  if (areaIsEmpty(*selectionInfo, Validity))
    return;

  ClearValiditySelectionWorker w;
  workOnCells( selectionInfo, w );
}


struct ClearConditionalSelectionWorker : public Sheet::CellWorker
{
  ClearConditionalSelectionWorker( ) : Sheet::CellWorker( ) { }

  class UndoAction* createUndoAction( Doc* doc,
               Sheet* sheet,
               const KSpread::Region& region )
  {
    return new UndoConditional( doc, sheet, region );
  }
  bool testCondition( Cell* cell )
  {
    return ( !cell->isObscured() );
  }
  void doWork( Cell* cell, bool, int, int )
  {
    QLinkedList<Conditional> emptyList;
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

  QDomDocument undoDoc = saveCellRegion( rct );
  loadSelectionUndo( undoDoc, rct, left - 1, top - 1, false, 0 );

  QDomDocument doc;

  switch( direction )
  {
   case Right:
    doc = saveCellRegion( QRect( left, top, 1, height ) );
    break;

   case Up:
    doc = saveCellRegion( QRect( left, bottom, width, 1 ) );
    break;

   case Left:
    doc = saveCellRegion( QRect( right, top, 1, height ) );
    break;

   case Down:
    doc = saveCellRegion( QRect( left, top, width, 1 ) );
    break;
  };

  // Save to buffer
  QBuffer buffer;
  buffer.open( QIODevice::WriteOnly );
  QTextStream str( &buffer );
  str.setCode( "UTF-8" );
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

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
        QString title=i18n("Default Parameters");
  return new UndoCellFormat( doc, sheet, region, title );
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
    emit sig_updateView( this, *selectionInfo );
    return;
  case CompleteColumns:
    ColumnFormat *cl;
    for ( int i = selection.left(); i <= selection.right(); i++ ) {
      cl=nonDefaultColumnFormat( i );
      cl->defaultStyleFormat();
    }
    emit sig_updateView( this, *selectionInfo );
    return;
  case CellRegion:
    emit sig_updateView( this, *selectionInfo );
      return;
  }
}


struct SetConditionalWorker : public Sheet::CellWorker
{
  QLinkedList<Conditional> conditionList;
  SetConditionalWorker( QLinkedList<Conditional> _tmp ) :
    Sheet::CellWorker( ), conditionList( _tmp ) { }

  class UndoAction* createUndoAction( Doc* doc,
                                      Sheet* sheet, const KSpread::Region& region )
  {
    return new UndoConditional( doc, sheet, region );
  }

  bool testCondition( Cell* )
  {
    return true;
  }

  void doWork( Cell* cell, bool, int, int )
  {
    if ( !cell->isObscured() ) // TODO: isPartOfMerged()???
    {
      cell->setConditionList(conditionList);
      cell->setDisplayDirtyFlag();
    }
  }
};

void Sheet::setConditional( Selection* selectionInfo,
                                   QLinkedList<Conditional> const & newConditions)
{
  if ( !doc()->undoLocked() )
  {
    UndoConditional * undo = new UndoConditional(doc(), this, *selectionInfo);
    doc()->addCommand( undo );
  }

  Region::ConstIterator endOfList = selectionInfo->constEnd();
  for (Region::ConstIterator it = selectionInfo->constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalized();

    int l = range.left();
    int r = range.right();
    int t = range.top();
    int b = range.bottom();

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
  }

  emit sig_updateView( this, *selectionInfo );
}


struct SetValidityWorker : public Sheet::CellWorker {
    Validity tmp;
    SetValidityWorker( Validity _tmp ) : Sheet::CellWorker( ), tmp( _tmp ) { }

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
  return new UndoConditional( doc, sheet, region );
    }
    bool testCondition( Cell* ) {
        return true;
    }
    void doWork( Cell* cell, bool, int, int ) {
  if ( !cell->isObscured() ) {
      cell->setDisplayDirtyFlag();
      if ( tmp.m_restriction==Restriction::None )
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
    tmpValidity->m_restriction=tmp.m_restriction;
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

    class UndoAction* createUndoAction( Doc*, Sheet*, const KSpread::Region& ) {
  return 0;
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

    class UndoAction* createUndoAction( Doc* doc, Sheet* sheet, const KSpread::Region& region ) {
  return new UndoChangeAreaTextCell( doc, sheet, region );
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
    QStringList list = _listWord.split( '\n' );
    SetWordSpellingWorker w( list,  this );
    workOnCells( selectionInfo, w );
}

static QString cellAsText( Cell* cell, unsigned int max )
{
  QString result;
  if( !cell->isDefault() )
  {
    int l = max - cell->strOutText().length();
    if (cell->defineAlignX() == Style::Right )
    {
        for ( int i = 0; i < l; ++i )
          result += " ";
        result += cell->strOutText();
    }
    else if (cell->defineAlignX() == Style::Left )
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
    if ( selectionInfo->isSingular() )
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
          top = qMin( top, (unsigned) c->row() );
          left = qMin( left, (unsigned) c->column() );
          bottom = qMax( bottom, (unsigned) c->row() );
          right = qMax( right, (unsigned) c->column() );

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
    QDomDocument doc = saveCellRegion( *selectionInfo, true );

    // Save to buffer
    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    QTextStream str( &buffer );
    str.setCodec( "UTF-8" );
    str << doc;
    buffer.close();

    TextDrag * kd = new TextDrag( 0L );
    kd->setPlain( copyAsText(selectionInfo) );
    kd->setKSpread( buffer.buffer() );

    QApplication::clipboard()->setData( kd );
}

void Sheet::cutSelection( Selection* selectionInfo )
{
    QDomDocument doc = saveCellRegion(*selectionInfo, true, true);

    // Save to buffer
    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    QTextStream str( &buffer );
    str.setCodec( "UTF-8" );
    str << doc;
    buffer.close();

    TextDrag * kd = new TextDrag( 0L );
    kd->setPlain( copyAsText(selectionInfo) );
    kd->setKSpread( buffer.buffer() );

    QApplication::clipboard()->setData( kd );

    deleteSelection( selectionInfo, true );
}

void Sheet::paste( const QRect& pasteArea, bool makeUndo,
                   Paste::Mode mode, Paste::Operation operation,
                   bool insert, int insertTo, bool pasteFC,
                   QClipboard::Mode clipboardMode )
{
    QMimeSource * mime = QApplication::clipboard()->data( clipboardMode );
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
        QString _text = QApplication::clipboard()->text( clipboardMode );
        doc()->emitBeginOperation();
        pasteTextPlain( _text, pasteArea );
        emit sig_updateView( this );
        // doc()->emitEndOperation();
        return;
    }
    else
        return;

    // Do the actual pasting.
    doc()->emitBeginOperation();
    paste( b, pasteArea, makeUndo, mode, operation, insert, insertTo, pasteFC );
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

    p = tmp.indexOf('\n');

    if (p < 0)
      p = tmp.length();

    rowtext = tmp.left(p);

    if ( !isProtected() || cell->format()->notProtected( mx, my + i ) )
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

void Sheet::paste( const QByteArray& b, const QRect& pasteArea, bool makeUndo,
                   Paste::Mode mode, Paste::Operation operation,
                   bool insert, int insertTo, bool pasteFC )
{
    kDebug(36001) << "Parsing " << b.size() << " bytes" << endl;

    QByteArray byteArray( b );
    QBuffer buffer( &byteArray );
    buffer.open( QIODevice::ReadOnly );
    QDomDocument doc;
    doc.setContent( &buffer );
    buffer.close();

    // ##### TODO: Test for parsing errors

    int mx = pasteArea.left();
    int my = pasteArea.top();

    loadSelection( doc, pasteArea, mx - 1, my - 1, makeUndo,
                   mode, operation, insert, insertTo, pasteFC );
}

bool Sheet::loadSelection(const QDomDocument& doc, const QRect& pasteArea,
                          int _xshift, int _yshift, bool makeUndo,
                          Paste::Mode mode, Paste::Operation operation, bool insert,
                          int insertTo, bool pasteFC)
{
  //kDebug(36001) << "loadSelection called. pasteArea=" << pasteArea << endl;

  if (!isLoading() && makeUndo)
  {
    loadSelectionUndo( doc, pasteArea, _xshift, _yshift, insert, insertTo );
  }

  QDomElement root = doc.documentElement(); // "spreadsheet-snippet"

  int rowsInClpbrd    =  root.attribute( "rows" ).toInt();
  int columnsInClpbrd =  root.attribute( "columns" ).toInt();

  // find size of rectangle that we want to paste to (either clipboard size or current selection)
  const int pasteWidth = ( pasteArea.width() >= columnsInClpbrd
                            && util_isRowSelected(pasteArea) == false
                            && root.namedItem( "rows" ).toElement().isNull() )
    ? pasteArea.width() : columnsInClpbrd;
  const int pasteHeight = ( pasteArea.height() >= rowsInClpbrd
                            && util_isColumnSelected(pasteArea) == false
                            && root.namedItem( "columns" ).toElement().isNull())
    ? pasteArea.height() : rowsInClpbrd;

//   kDebug() << "loadSelection: paste area has size "
//             << pasteHeight << " rows * "
//             << pasteWidth << " columns " << endl;
//   kDebug() << "loadSelection: " << rowsInClpbrd << " rows and "
//             << columnsInClpbrd << " columns in clipboard." << endl;
//   kDebug() << "xshift: " << _xshift << " _yshift: " << _yshift << endl;

  QDomElement e = root.firstChild().toElement(); // "columns", "rows" or "cell"
  for (; !e.isNull(); e = e.nextSibling().toElement())
  {
    // entire columns given
    if (e.tagName() == "columns" && !isProtected())
    {
        _yshift = 0;

        // Clear the existing columns
        int col = e.attribute("column").toInt();
        int width = e.attribute("count").toInt();
        if (!insert)
        {
            for ( int i = col; i < col + width; ++i )
            {
                d->cells.clearColumn( _xshift + i );
                d->columns.removeElement( _xshift + i );
            }
        }

        // Insert column formats
        QDomElement c = e.firstChild().toElement();
        for ( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "column" )
            {
                ColumnFormat *cl = new ColumnFormat( this, 0 );
                if ( cl->load( c, _xshift, mode, pasteFC ) )
                    insertColumnFormat( cl );
                else
                    delete cl;
            }
        }
    }

    // entire rows given
    if (e.tagName() == "rows" && !isProtected())
    {
        _xshift = 0;

        // Clear the existing rows
        int row = e.attribute("row").toInt();
        int height = e.attribute("count").toInt();
        if ( !insert )
        {
          for( int i = row; i < row + height; ++i )
          {
            d->cells.clearRow( _yshift + i );
            d->rows.removeElement( _yshift + i );
          }
        }

        // Insert row formats
        QDomElement c = e.firstChild().toElement();
        for( ; !c.isNull(); c = c.nextSibling().toElement() )
        {
            if ( c.tagName() == "row" )
            {
                RowFormat *cl = new RowFormat( this, 0 );
                if ( cl->load( c, _yshift, mode, pasteFC ) )
                    insertRowFormat( cl );
                else
                    delete cl;
            }
        }
    }

    Cell* refreshCell = 0;
    Cell *cell;
    Cell *cellBackup = NULL;
    if (e.tagName() == "cell")
    {
      int row = e.attribute( "row" ).toInt() + _yshift;
      int col = e.attribute( "column" ).toInt() + _xshift;

      // tile the selection with the clipboard contents
      for (int roff = 0; row + roff - _yshift <= pasteHeight; roff += rowsInClpbrd)
      {
        for (int coff = 0; col + coff - _xshift <= pasteWidth; coff += columnsInClpbrd)
        {
//           kDebug() << "loadSelection: cell at " << (col+coff) << "," << (row+roff)
//                     << " with roff,coff= " << roff << "," << coff
//                     << ", _xshift: " << _xshift << ", _yshift: " << _yshift << endl;

          cell = nonDefaultCell( col + coff, row + roff );
          if (isProtected() && !cell->format()->notProtected(col + coff, row + roff))
          {
            continue;
          }

          cellBackup = new Cell(this, cell->column(), cell->row());
          cellBackup->copyAll(cell);

          if (!cell->load(e, _xshift + coff, _yshift + roff, mode, operation, pasteFC))
          {
            cell->copyAll(cellBackup);
          }
          else
          {
            if (cell->isFormula())
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

    //refresh chart after that you paste all cells

    /* I don't think this is gonna work....doesn't this only update
       one chart -- the one which had a dependant cell update first? - John

       I don't have time to check on this now....
    */
    if ( refreshCell )
        refreshCell->updateChart();
  }
    this->doc()->setModified( true );

    if (!isLoading())
        refreshMergedCell();

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );

    return true;
}

void Sheet::loadSelectionUndo(const QDomDocument& d, const QRect& loadArea,
                              int _xshift, int _yshift,
                              bool insert, int insertTo)
{
  QDomElement root = d.documentElement(); // "spreadsheet-snippet"

  int rowsInClpbrd    = root.attribute( "rows" ).toInt();
  int columnsInClpbrd = root.attribute( "columns" ).toInt();

  // find rect that we paste to
  const int pasteWidth = (loadArea.width() >= columnsInClpbrd &&
                          util_isRowSelected(loadArea) == false &&
                          root.namedItem( "rows" ).toElement().isNull())
      ? loadArea.width() : columnsInClpbrd;
  const int pasteHeight = (loadArea.height() >= rowsInClpbrd &&
                           util_isColumnSelected(loadArea) == false &&
                           root.namedItem( "columns" ).toElement().isNull())
      ? loadArea.height() : rowsInClpbrd;

  uint numCols = 0;
  uint numRows = 0;

  Region region;
  for (QDomNode n = root.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QDomElement e = n.toElement(); // "columns", "rows" or "cell"
    if (e.tagName() == "columns")
    {
      _yshift = 0;
      int col = e.attribute("column").toInt();
      int width = e.attribute("count").toInt();
      for (int coff = 0; col + coff <= pasteWidth; coff += columnsInClpbrd)
      {
        uint overlap = qMax(0, (col - 1 + coff + width) - pasteWidth);
        uint effWidth = width - overlap;
        region.add(QRect(_xshift + col + coff, 1, effWidth, KS_rowMax));
        numCols += effWidth;
      }
    }
    else if (e.tagName() == "rows")
    {
      _xshift = 0;
      int row = e.attribute("row").toInt();
      int height = e.attribute("count").toInt();
      for (int roff = 0; row + roff <= pasteHeight; roff += rowsInClpbrd)
      {
        uint overlap = qMax(0, (row - 1 + roff + height) - pasteHeight);
        uint effHeight = height - overlap;
        region.add(QRect(1, _yshift + row + roff, KS_colMax, effHeight));
        numRows += effHeight;
      }
    }
    else if (!e.isNull())
    {
      // store the cols/rows for the insertion
      int col = e.attribute("column").toInt();
      int row = e.attribute("row").toInt();
      for (int coff = 0; col + coff <= pasteWidth; coff += columnsInClpbrd)
      {
        for (int roff = 0; row + roff <= pasteHeight; roff += rowsInClpbrd)
        {
          region.add(QPoint(_xshift + col + coff, _yshift + row + roff));
        }
      }
    }
  }

  if (!doc()->undoLocked())
  {
    UndoCellPaste *undo = new UndoCellPaste( doc(), this, _xshift, _yshift, region, insert, insertTo );
    doc()->addCommand( undo );
  }

  if (insert)
  {
    QRect rect = region.boundingRect();
    // shift cells to the right
    if (insertTo == -1 && numCols == 0 && numRows == 0)
    {
      rect.setWidth(rect.width());
      shiftRow(rect, false);
    }
    // shift cells to the bottom
    else if (insertTo == 1 && numCols == 0 && numRows == 0)
    {
      rect.setHeight(rect.height());
      shiftColumn( rect, false );
    }
    // insert columns
    else if (insertTo == 0 && numCols == 0 && numRows > 0)
    {
      insertRow(rect.top(), rect.height() - 1, false);
    }
    // insert rows
    else if (insertTo == 0 && numCols > 0 && numRows == 0)
    {
      insertColumn(rect.left(), rect.width() - 1, false);
    }
  }
}

bool Sheet::testAreaPasteInsert()const
{
    QMimeSource* mime = QApplication::clipboard()->data( QClipboard::Clipboard );
    if ( !mime )
        return false;

    QByteArray b;

    if ( mime->provides( "application/x-kspread-snippet" ) )
        b = mime->encodedData( "application/x-kspread-snippet" );
    else
        return false;

    QBuffer buffer( &b );
    buffer.open( QIODevice::ReadOnly );
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

void Sheet::deleteCells(const Region& region)
{
    // A list of all cells we want to delete.
    Q3PtrStack<Cell> cellStack;

  Region::ConstIterator endOfList = region.constEnd();
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalized();

    int right  = range.right();
    int left   = range.left();
    int bottom = range.bottom();
    int col;
    for ( int row = range.top(); row <= bottom; ++row )
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
      if ( c->doesMergeCells() && !c->isDefault() )
        c->mergeCells( c->column(), c->row(),
           c->extraXCells(), c->extraYCells() );
    }
    doc()->setModified( true );
}

void Sheet::deleteSelection( Selection* selectionInfo, bool undo )
{
    if ( undo && !doc()->undoLocked() )
    {
        UndoDelete *undo = new UndoDelete( doc(), this, *selectionInfo );
        doc()->addCommand( undo );
    }

  Region::ConstIterator endOfList = selectionInfo->constEnd();
  for (Region::ConstIterator it = selectionInfo->constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalized();

    // Entire rows selected ?
    if ( util_isRowSelected(range) )
    {
        for( int i = range.top(); i <= range.bottom(); ++i )
        {
            d->cells.clearRow( i );
            d->rows.removeElement( i );
        }

        emit sig_updateVBorder( this );
    }
    // Entire columns selected ?
    else if ( util_isColumnSelected(range) )
    {
        for( int i = range.left(); i <= range.right(); ++i )
        {
            d->cells.clearColumn( i );
            d->columns.removeElement( i );
        }

        emit sig_updateHBorder( this );
    }
    else
    {
	setRegionPaintDirty( range );
        deleteCells( range );
    }
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

void Sheet::updateView(Region* region)
{
  emit sig_updateView( this, *region );
}

void Sheet::refreshView( const Region& region )
{
  Region tmpRegion;
  Region::ConstIterator endOfList = region.constEnd();
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalized();
    // TODO: don't go through all cells when refreshing!
    QRect tmp(range);
    Cell * c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
    {
      if ( !c->isDefault() &&
            c->row() >= range.top() && c->row() <= range.bottom() &&
            c->column() >= range.left() && c->column() <= range.right() )
      {
        if (c->doesMergeCells())
        {
              int right=qMax(tmp.right(),c->column()+c->extraXCells());
              int bottom=qMax(tmp.bottom(),c->row()+c->extraYCells());

              tmp.setRight(right);
              tmp.setBottom(bottom);
        }
      }
    }
    deleteCells( range );
    tmpRegion.add(tmp);
  }
  emit sig_updateView( this, tmpRegion );
}


void Sheet::mergeCells(const Region& region, bool hor, bool ver)
{
  // sanity check
  if( isProtected() )
    return;
  if( workbook()->isProtected() )
    return;

  MergeManipulator* manipulator = new MergeManipulator();
  manipulator->setSheet(this);
  manipulator->setHorizontalMerge(hor);
  manipulator->setVerticalMerge(ver);
  manipulator->add(region);
  manipulator->execute();
}

void Sheet::dissociateCells(const Region& region)
{
  // sanity check
  if( isProtected() )
    return;
  if( workbook()->isProtected() )
    return;

  Manipulator* manipulator = new MergeManipulator();
  manipulator->setSheet(this);
  manipulator->setReverse(true);
  manipulator->add(region);
  manipulator->execute();
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
            !c->isPartOfMerged() &&
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



QDomDocument Sheet::saveCellRegion(const Region& region, bool copy, bool era)
{
  QDomDocument dd( "spreadsheet-snippet" );
  dd.appendChild( dd.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  QDomElement root = dd.createElement( "spreadsheet-snippet" );
  dd.appendChild(root);

  // find the upper left corner of the selection
  QRect boundingRect = region.boundingRect();
  int left = boundingRect.left();
  int top = boundingRect.top();

  // for tiling the clipboard content in the selection
  root.setAttribute( "rows", boundingRect.height() );
  root.setAttribute( "columns", boundingRect.width() );

  Region::ConstIterator endOfList = region.constEnd();
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalized();

    //
    // Entire rows selected?
    //
    if ((*it)->isRow())
    {
      QDomElement rows = dd.createElement("rows");
      rows.setAttribute( "count", range.height() );
      rows.setAttribute( "row", range.top() - top + 1 );
      root.appendChild( rows );

      // Save all cells.
      for (Cell* cell = d->cells.firstCell(); cell; cell = cell->nextCell())
      {
        if (!cell->isDefault() && !cell->isPartOfMerged())
        {
          QPoint point(cell->column(), cell->row());
          if (range.contains(point))
          {
            root.appendChild(cell->save( dd, 0, top - 1, copy, copy, era));
          }
        }
      }

      // ##### Inefficient
      // Save the row formats if there are any
      RowFormat* format;
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        format = rowFormat( row );
        if (format && !format->isDefault())
        {
          QDomElement e = format->save(dd, top - 1, copy);
          if (!e.isNull())
          {
            rows.appendChild( e );
          }
        }
      }
      continue;
    }

    //
    // Entire columns selected?
    //
    if ((*it)->isColumn())
    {
      QDomElement columns = dd.createElement("columns");
      columns.setAttribute( "count", range.width() );
      columns.setAttribute( "column", range.left() - left + 1 );
      root.appendChild( columns );

      // Save all cells.
      for (Cell* cell = d->cells.firstCell();cell; cell = cell->nextCell())
      {
        if (!cell->isDefault() && !cell->isPartOfMerged())
        {
          QPoint point(cell->column(), cell->row());
          if (range.contains(point))
          {
            root.appendChild(cell->save( dd, left - 1, 0, copy, copy, era));
          }
        }
      }

      // ##### Inefficient
      // Save the column formats if there are any
      ColumnFormat* format;
      for (int col = range.left(); col <= range.right(); ++col)
      {
        format = columnFormat(col);
        if (format && !format->isDefault())
        {
          QDomElement e = format->save(dd, left - 1, copy);
          if (!e.isNull())
          {
            columns.appendChild(e);
          }
        }
      }
      continue;
    }

    // Save all cells.
    //store all cell
    //when they don't exist we created them
    //because it's necessary when there is a  format on a column/row
    //but I remove cell which is inserted.
    Cell* cell;
    bool insert;
    for (int col = range.left(); col <= range.right(); ++col)
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        insert = false;
        cell = cellAt(col, row);
        if (cell == d->defaultCell)
        {
          cell = new Cell(this, col, row);
          insertCell(cell);
          insert = true;
        }
        root.appendChild(cell->save(dd, left - 1, top - 1, true, copy, era));
        if (insert)
        {
          d->cells.remove(col, row);
        }
      }
    }
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
    sheet.setAttribute( "showFormula", (int)d->showFormula);
    sheet.setAttribute( "showFormulaIndicator", (int)d->showFormulaIndicator);
    sheet.setAttribute( "showCommentIndicator", (int)d->showCommentIndicator);
    sheet.setAttribute( "lcmode", (int)d->lcMode);
    sheet.setAttribute( "autoCalc", (int)d->autoCalc);
    sheet.setAttribute( "borders1.2", 1);
    if ( !d->password.isNull() )
    {
      if ( d->password.size() > 0 )
      {
        QByteArray str = KCodecs::base64Encode( d->password );
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

    Q3PtrListIterator<EmbeddedObject>  chl = doc()->embeddedObjects();
    for( ; chl.current(); ++chl )
    {
       if ( chl.current()->sheet() == this )
       {
         QDomElement e = chl.current()->save( dd );

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


Q3PtrList<EmbeddedObject> Sheet::getSelectedObjects()
{
    Q3PtrList<EmbeddedObject> objects;
    Q3PtrListIterator<EmbeddedObject> it = doc()->embeddedObjects();
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected()
            && it.current()->sheet() == this )
        {
            objects.append( it.current() );
        }
    }
     return objects;
}

KoRect Sheet::getRealRect( bool all )
{
    KoRect rect;

    Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects() );
    for ( ; it.current() ; ++it )
    {

        if ( all || ( it.current()->isSelected() && ! it.current()->isProtect() ) )
            rect |= it.current()->geometry();
    }

    return rect;
}

// move object for releasemouseevent
KCommand *Sheet::moveObject(View *_view, double diffx, double diffy)
{
    bool createCommand=false;
    MoveObjectByCmd *moveByCmd=0L;
    Canvas * canvas = _view->canvasWidget();
    Q3PtrList<EmbeddedObject> _objects;
    _objects.setAutoDelete( false );
    Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects()/*m_objectList*/ );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && !it.current()->isProtect())
        {
            _objects.append( it.current() );
            KoRect geometry = it.current()->geometry();
            geometry.moveBy( -canvas->xOffset(), -canvas->yOffset() );
            QRect br = doc()->zoomRect( geometry/*it.current()->geometry()*/ );
            br.translate( doc()->zoomItX( diffx ), doc()->zoomItY( diffy ) );
            br.translate( doc()->zoomItX( -canvas->xOffset() ), doc()->zoomItY( -canvas->yOffset() ) );
            canvas->repaint( br ); // Previous position
            canvas->repaintObject( it.current() ); // New position
            createCommand=true;
        }
    }
    if(createCommand) {
        moveByCmd = new MoveObjectByCmd( i18n( "Move Objects" ), KoPoint( diffx, diffy ),
                                   _objects, doc(), this );

//         m_doc->updateSideBarItem( this );
    }
    return moveByCmd;
}

KCommand *Sheet::moveObject(View *_view,const KoPoint &_move,bool key)
{
    Q3PtrList<EmbeddedObject> _objects;
    _objects.setAutoDelete( false );
    MoveObjectByCmd *moveByCmd=0L;
    Canvas * canvas = _view->canvasWidget();
    Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects()/*m_objectList*/ );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && !it.current()->isProtect()) {

            KoRect geometry = it.current()->geometry();
            geometry.moveBy( -canvas->xOffset(), -canvas->yOffset() );
            QRect oldBoundingRect = doc()->zoomRect( geometry );


            KoRect r = it.current()->geometry();
            r.moveBy( _move.x(), _move.y() );

            it.current()->setGeometry( r );
            _objects.append( it.current() );

            canvas->repaint( oldBoundingRect );
            canvas->repaintObject( it.current() );
        }
    }

    if ( key && !_objects.isEmpty())
        moveByCmd = new MoveObjectByCmd( i18n( "Move Objects" ),
                                   KoPoint( _move ),
                                   _objects, doc() ,this );

    return moveByCmd;
}

/*
 * Check if object name already exists.
 */
bool Sheet::objectNameExists( EmbeddedObject *object, Q3PtrList<EmbeddedObject> &list ) {
    Q3PtrListIterator<EmbeddedObject> it( list );

    for ( it.toFirst(); it.current(); ++it ) {
        // object name can exist in current object.
        if ( it.current()->getObjectName() == object->getObjectName() &&
             it.current() != object ) {
            return true;
        }
    }
    return false;
}

void Sheet::unifyObjectName( EmbeddedObject *object ) {
    if ( object->getObjectName().isEmpty() ) {
        object->setObjectName( object->getTypeString() );
    }
    QString objectName( object->getObjectName() );

    Q3PtrList<EmbeddedObject> list( doc()->embeddedObjects() );

    int count = 1;

    while ( objectNameExists( object, list ) ) {
        count++;
        QRegExp rx( " \\(\\d{1,3}\\)$" );
        if ( rx.indexIn( objectName ) != -1 ) {
            objectName.remove( rx );
        }
        objectName += QString(" (%1)").arg( count );
        object->setObjectName( objectName );
    }
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
        kDebug() << "Header exists" << endl;
        QDomNode part = KoDom::namedItemNS( header, KoXmlNS::style, "region-left" );
        if ( !part.isNull() )
        {
            hleft = getPart( part );
            kDebug() << "Header left: " << hleft << endl;
        }
        else
            kDebug() << "Style:region:left doesn't exist!" << endl;
        part = KoDom::namedItemNS( header, KoXmlNS::style, "region-center" );
        if ( !part.isNull() )
        {
            hmiddle = getPart( part );
            kDebug() << "Header middle: " << hmiddle << endl;
        }
        part = KoDom::namedItemNS( header, KoXmlNS::style, "region-right" );
        if ( !part.isNull() )
        {
            hright = getPart( part );
            kDebug() << "Header right: " << hright << endl;
        }
    }
    //TODO implement it under kspread
    QDomNode headerleft = KoDom::namedItemNS( *style, KoXmlNS::style, "header-left" );
    if ( !headerleft.isNull() )
    {
        QDomElement e = headerleft.toElement();
        if ( e.hasAttributeNS( KoXmlNS::style, "display" ) )
            kDebug()<<"header.hasAttribute( style:display ) :"<<e.hasAttributeNS( KoXmlNS::style, "display" )<<endl;
        else
            kDebug()<<"header left doesn't has attribute  style:display  \n";
    }
    //TODO implement it under kspread
    QDomNode footerleft = KoDom::namedItemNS( *style, KoXmlNS::style, "footer-left" );
    if ( !footerleft.isNull() )
    {
        QDomElement e = footerleft.toElement();
        if ( e.hasAttributeNS( KoXmlNS::style, "display" ) )
            kDebug()<<"footer.hasAttribute( style:display ) :"<<e.hasAttributeNS( KoXmlNS::style, "display" )<<endl;
        else
            kDebug()<<"footer left doesn't has attribute  style:display  \n";
    }

    QDomNode footer = KoDom::namedItemNS( *style, KoXmlNS::style, "footer" );

    if ( !footer.isNull() )
    {
        QDomNode part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-left" );
        if ( !part.isNull() )
        {
            fleft = getPart( part );
            kDebug() << "Footer left: " << fleft << endl;
        }
        part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-center" );
        if ( !part.isNull() )
        {
            fmiddle = getPart( part );
            kDebug() << "Footer middle: " << fmiddle << endl;
        }
        part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-right" );
        if ( !part.isNull() )
        {
            fright = getPart( part );
            kDebug() << "Footer right: " << fright << endl;
        }
    }

    print()->setHeadFootLine( hleft, hmiddle, hright,
                              fleft, fmiddle, fright );
    return true;
}

void Sheet::replaceMacro( QString & text, const QString & old, const QString & newS )
{
  int n = text.indexOf( old );
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
    kDebug() << "PART: " << text << endl;

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


bool Sheet::loadOasis( const QDomElement& sheetElement, KoOasisLoadingContext& oasisContext, Q3Dict<Style>& styleMap )
{
    d->layoutDirection = LeftToRight;
    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString stylename = sheetElement.attributeNS( KoXmlNS::table, "style-name", QString::null );
        kDebug()<<" style of table :"<<stylename<<endl;
        const QDomElement *style = oasisContext.oasisStyles().findStyle( stylename, "table" );
        Q_ASSERT( style );
        kDebug()<<" style :"<<style<<endl;
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
                kDebug()<<"style->attribute( style:master-page-name ) :"<<masterPageStyleName <<endl;
                QDomElement *masterStyle = oasisContext.oasisStyles().masterPages()[masterPageStyleName];
                kDebug()<<"oasisStyles.styles()[masterPageStyleName] :"<<masterStyle<<endl;
                if ( masterStyle )
                {
                    loadSheetStyleFormat( masterStyle );
                    if ( masterStyle->hasAttributeNS( KoXmlNS::style, "page-layout-name" ) )
                    {
                        QString masterPageLayoutStyleName = masterStyle->attributeNS( KoXmlNS::style, "page-layout-name", QString::null );
                        kDebug()<<"masterPageLayoutStyleName :"<<masterPageLayoutStyleName<<endl;
                        const QDomElement *masterLayoutStyle = oasisContext.oasisStyles().findStyle( masterPageLayoutStyleName );
                      if ( masterLayoutStyle )
                      {
                        kDebug()<<"masterLayoutStyle :"<<masterLayoutStyle<<endl;
                        KoStyleStack styleStack;
                        styleStack.setTypeProperties( "page-layout" );
                        styleStack.push( *masterLayoutStyle );
                        loadOasisMasterLayoutPage( styleStack );
                      }
                    }
                }
            }
        }
    }

    //Maps from a column index to the name of the default cell style for that column
    QMap<int,QString> defaultColumnCellStyles;

    int rowIndex = 1;
    int indexCol = 1;
    QDomNode rowNode = sheetElement.firstChild();
    // Some spreadsheet programs may support more rows than
    // KSpread so limit the number of repeated rows.
    // FIXME POSSIBLE DATA LOSS!
    while( !rowNode.isNull() && indexCol <= KS_colMax && rowIndex <= KS_rowMax )
    {
        kDebug()<<" rowIndex :"<<rowIndex<<" indexCol :"<<indexCol<<endl;
        QDomElement rowElement = rowNode.toElement();
        if( !rowElement.isNull() )
        {
            kDebug()<<" Sheet::loadOasis rowElement.tagName() :"<<rowElement.localName()<<endl;
            if ( rowElement.namespaceURI() == KoXmlNS::table )
            {
                if ( rowElement.localName()=="table-column" )
                {
                    kDebug ()<<" table-column found : index column before "<< indexCol<<endl;
                    loadColumnFormat( rowElement, oasisContext.oasisStyles(), indexCol , styleMap);
                    kDebug ()<<" table-column found : index column after "<< indexCol<<endl;
                }
                else if( rowElement.localName() == "table-row" )
                {
                    kDebug()<<" table-row found :index row before "<<rowIndex<<endl;
                    loadRowFormat( rowElement, rowIndex, oasisContext, /*rowNode.isNull() ,*/ styleMap );
                    kDebug()<<" table-row found :index row after "<<rowIndex<<endl;
                }
                else if ( rowElement.localName() == "shapes" )
                    loadOasisObjects( rowElement, oasisContext );
            }
        }
        rowNode = rowNode.nextSibling();
    }

    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "print-ranges" ) )
    {
        // e.g.: Sheet4.A1:Sheet4.E28
        QString range = sheetElement.attributeNS( KoXmlNS::table, "print-ranges", QString::null );
        range = Oasis::decodeFormula( range );
        Range p( range );
        if ( sheetName() == p.sheetName() )
            d->print->setPrintRange( p.range() );
    }


    if ( sheetElement.attributeNS( KoXmlNS::table, "protected", QString::null ) == "true" )
    {
        QByteArray passwd( "" );
        if ( sheetElement.hasAttributeNS( KoXmlNS::table, "protection-key" ) )
        {
            QString p = sheetElement.attributeNS( KoXmlNS::table, "protection-key", QString::null );
            QByteArray str( p.toLatin1() );
            kDebug(30518) << "Decoding password: " << str << endl;
            passwd = KCodecs::base64Decode( str );
        }
        kDebug(30518) << "Password hash: '" << passwd << "'" << endl;
        d->password = passwd;
    }
    return true;
}


void Sheet::loadOasisObjects( const QDomElement &parent, KoOasisLoadingContext& oasisContext )
{
    QDomElement e;
    QDomNode n = parent.firstChild();
    while( !n.isNull() )
    {
        e = n.toElement();
        if ( e.localName() == "frame" && e.namespaceURI() == KoXmlNS::draw )
        {
          EmbeddedObject *obj = 0;
          QDomNode object = KoDom::namedItemNS( e, KoXmlNS::draw, "object" );
          if ( !object.isNull() )
          {
            if ( !object.toElement().attributeNS( KoXmlNS::draw, "notify-on-update-of-ranges", QString::null).isNull() )
                obj = new EmbeddedChart( doc(), this );
            else
                obj = new EmbeddedKOfficeObject( doc(), this );
          }
          else
          {
            QDomNode image = KoDom::namedItemNS( e, KoXmlNS::draw, "image" );
            if ( !image.isNull() )
              obj = new EmbeddedPictureObject( this, doc()->pictureCollection() );
            else
              kDebug() << "Object type wasn't loaded!" << endl;
          }

          if ( obj )
          {
            obj->loadOasis( e, oasisContext );
            insertObject( obj );
          }
        }
        n = n.nextSibling();
    }
}


void Sheet::loadOasisMasterLayoutPage( KoStyleStack &styleStack )
{
    // use A4 as default page size
    float left = 20.0;
    float right = 20.0;
    float top = 20.0;
    float bottom = 20.0;
    float width = 210.0;
    float height = 297.0;
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
        kDebug()<<"styleStack.hasAttribute( style:writing-mode ) :"<<styleStack.hasAttributeNS( KoXmlNS::style, "writing-mode" )<<endl;
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
        kDebug()<<" num-format :"<<styleStack.attributeNS( KoXmlNS::style, "num-format" )<<endl;

    }
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) )
    {
        //TODO
        kDebug()<<" fo:background-color :"<<styleStack.attributeNS( KoXmlNS::fo, "background-color" )<<endl;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "print" ) )
    {
        //todo parsing
        QString str = styleStack.attributeNS( KoXmlNS::style, "print" );
        kDebug()<<" style:print :"<<str<<endl;

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
        kDebug()<<" styleStack.attribute( style:table-centering ) :"<<str<<endl;
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
            kDebug()<<" table-centering unknown :"<<str<<endl;
#endif
    }
    format = QString( "%1x%2" ).arg( width ).arg( height );
    kDebug()<<" format : "<<format<<endl;
    d->print->setPaperLayout( left, top, right, bottom, format, orientation );

    kDebug()<<" left margin :"<<left<<" right :"<<right<<" top :"<<top<<" bottom :"<<bottom<<endl;
//<style:properties fo:page-width="21.8cm" fo:page-height="28.801cm" fo:margin-top="2cm" fo:margin-bottom="2.799cm" fo:margin-left="1.3cm" fo:margin-right="1.3cm" style:writing-mode="lr-tb"/>
//          QString format = paper.attribute( "format" );
//      QString orientation = paper.attribute( "orientation" );
//        d->print->setPaperLayout( left, top, right, bottom, format, orientation );
//      }
}


bool Sheet::loadColumnFormat(const QDomElement& column, const KoOasisStyles& oasisStyles, int & indexCol, const Q3Dict<Style>& styleMap)
{
    kDebug()<<"bool Sheet::loadColumnFormat(const QDomElement& column, const KoOasisStyles& oasisStyles, unsigned int & indexCol ) index Col :"<<indexCol<<endl;

    bool collapsed = ( column.attributeNS( KoXmlNS::table, "visibility", QString::null ) == "collapse" );
    Format layout( this , doc()->styleManager()->defaultStyle() );
    int number = 1;
    double width   = 10;//POINT_TO_MM( colWidth ); FIXME
    if ( column.hasAttributeNS( KoXmlNS::table, "number-columns-repeated" ) )
    {
        bool ok = true;
        int n = column.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt( &ok );
        if ( ok )
          // Some spreadsheet programs may support more rows than KSpread so
          // limit the number of repeated rows.
          // FIXME POSSIBLE DATA LOSS!
          number = qMin( n, KS_colMax - indexCol + 1 );
        kDebug() << "Repeated: " << number << endl;
    }

    KoStyleStack styleStack;
    styleStack.setTypeProperties("table-column"); //style for column
    if ( column.hasAttributeNS( KoXmlNS::table, "default-cell-style-name" ) )
    {
        QString str = column.attributeNS( KoXmlNS::table, "default-cell-style-name", QString::null );
        kDebug()<<" default-cell-style-name:"<<str<<" for column " << indexCol <<endl;

	//TODO - Code to look up the style in styleMap and store a reference to it in some map
	// between column indicies and Style instances.  This can then be used when rendering cells

    }

    styleStack.setTypeProperties("table-column");
    if ( column.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString str = column.attributeNS( KoXmlNS::table, "style-name", QString::null );
        const QDomElement *style = oasisStyles.findStyle( str, "table-column" );
        if (style)
	{
		styleStack.push( *style );
	/*	FIX_BEFORE_COMMIT
	 	layout.loadOasisStyleProperties( styleStack , oasisStyles );
		styleStack.pop();*/
	}

        kDebug()<<" style column:"<<style<<"style name : "<<str<<endl;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::style, "column-width" ) )
    {
        width = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::style, "column-width" ) , -1 );
        kDebug()<<" style:column-width : width :"<<width<<endl;
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
            kDebug()<<" str :"<<str<<endl;
    }


  //  if ( number>30 )
  //      number = 30; //TODO fixme !

    for ( int i = 0; i < number; ++i )
    {
        kDebug()<<" insert new column: pos :"<<indexCol<<" width :"<<width<<" hidden ? "<<collapsed<<endl;
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
    kDebug()<<" after index column !!!!!!!!!!!!!!!!!! :"<<indexCol<<endl;
    return true;
}


bool Sheet::loadRowFormat( const QDomElement& row, int &rowIndex, KoOasisLoadingContext& oasisContext,  Q3Dict<Style>& styleMap )
{
//    kDebug()<<"Sheet::loadRowFormat( const QDomElement& row, int &rowIndex,const KoOasisStyles& oasisStyles, bool isLast )***********\n";
    double height = -1.0;
    Format layout( this , doc()->styleManager()->defaultStyle() );
    KoStyleStack styleStack;
    styleStack.setTypeProperties( "table-row" );
    int backupRow = rowIndex;

    if ( row.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString str = row.attributeNS( KoXmlNS::table, "style-name", QString::null );
        const QDomElement *style = oasisContext.oasisStyles().findStyle( str, "table-row" );
      if ( style )
        styleStack.push( *style );
  //      kDebug()<<" style column:"<<style<<"style name : "<<str<<endl;
    }

    if ( row.hasAttributeNS( KoXmlNS::table,"default-cell-style-name" ) )
    {
	    QString str = row.attributeNS( KoXmlNS::table, "default-cell-style-name", QString::null );

	    //TODO - Code to look up this style name in the style map and store it in a map somewhere between
	    //row indicies and Style instances for use when rendering cells later on.
	    //	    defaultRowCellStyle = styleMap[str];
    }

    layout.loadOasisStyleProperties( styleStack, oasisContext.oasisStyles() );
    styleStack.setTypeProperties( "table-row" );
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "row-height" ) )
    {
        height = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::style, "row-height" ) , -1 );
    //    kDebug()<<" properties style:row-height : height :"<<height<<endl;
    }

    int number = 1;
    if ( row.hasAttributeNS( KoXmlNS::table, "number-rows-repeated" ) )
    {
        bool ok = true;
        int n = row.attributeNS( KoXmlNS::table, "number-rows-repeated", QString::null ).toInt( &ok );
        if ( ok )
            // Some spreadsheet programs may support more rows than KSpread so
            // limit the number of repeated rows.
            // FIXME POSSIBLE DATA LOSS!
            number = qMin( n, KS_rowMax - rowIndex + 1 );
    }
    bool collapse = false;
    if ( row.hasAttributeNS( KoXmlNS::table, "visibility" ) )
    {
        QString visible = row.attributeNS( KoXmlNS::table, "visibility", QString::null );
    //    kDebug()<<" row.attribute( table:visibility ) "<<visible<<endl;
        if ( visible == "collapse" )
            collapse=true;
        else
            kDebug()<<" visible row not implemented/supported : "<<visible<<endl;

    }

    bool insertPageBreak = false;
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "break-before" ) )
    {
        QString str = styleStack.attributeNS( KoXmlNS::fo, "break-before" );
        if ( str == "page" )
        {
            insertPageBreak = true;
        }
      //  else
      //      kDebug()<<" str :"<<str<<endl;
    }

    //number == number of row to be copy. But we must copy cell too.
    for ( int i = 0; i < number; ++i )
    {
       // kDebug()<<" create non defaultrow format :"<<rowIndex<<" repeate : "<<number<<" height :"<<height<<endl;
        RowFormat * rowL = nonDefaultRowFormat( rowIndex );
        rowL->copy( layout );
        if ( height != -1 )
        {
         //   kDebug() << "Setting row height to " << height << endl;
            rowL->setHeight( (int) height );
        }
        if ( collapse )
            rowL->setHide( true );

        ++rowIndex;
    }

    int columnIndex = 0;
    QDomNode cellNode = row.firstChild();
    int endRow = qMin(backupRow+number,KS_rowMax);


    while( !cellNode.isNull() )
    {
        QDomElement cellElement = cellNode.toElement();
        if( !cellElement.isNull() )
        {
            columnIndex++;
            QString localName = cellElement.localName();

	    if( ((localName == "table-cell") || (localName == "covered-table-cell")) && cellElement.namespaceURI() == KoXmlNS::table)
            {
	//	kDebug() << "Loading cell #" << cellCount << endl;

                Cell* cell = nonDefaultCell( columnIndex, backupRow );
		bool cellHasStyle = cellElement.hasAttributeNS( KoXmlNS::table, "style-name" );

		Style* style = 0;

		if ( cellHasStyle )
		{
			style = styleMap[ cellElement.attributeNS( KoXmlNS::table , "style-name" , QString::null ) ];
		}

                cell->loadOasis( cellElement, oasisContext, style );


		bool haveStyle = cellHasStyle;
                int cols = 1;

		//Copy this cell across & down if it has repeated rows or columns, but only
		//if the cell has some content or a style associated with it.
                if( (number > 1) || cellElement.hasAttributeNS( KoXmlNS::table, "number-columns-repeated" ) )
                {
                    bool ok = false;
                    int n = cellElement.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt( &ok );

                    if (ok)
                      // Some spreadsheet programs may support more rows than
                      // KSpread so limit the number of repeated rows.
                      // FIXME POSSIBLE DATA LOSS!
                      cols = qMin( n, KS_colMax - columnIndex + 1 );

		    if ( !haveStyle && ( cell->isEmpty() && cell->format()->comment( columnIndex, backupRow ).isEmpty() ) )
                    {
                        //just increment it
                        columnIndex +=cols - 1;
                    }
                    else
                    {
                            for(int k = cols ; k ; --k )
                            {
				if (k != cols)
					columnIndex++;

				Style* targetStyle = style;

				for ( int newRow = backupRow; newRow < endRow;++newRow )
				{
				    if ( targetStyle && (targetStyle->features() != 0 ) )
				    {
                                    	Cell* target = nonDefaultCell( columnIndex, newRow );

					if (cell != target)
						target->copyAll( cell );
				    }
                                }
			    }
                    }
                }
            }
        }
        cellNode = cellNode.nextSibling();
    }

    return true;
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
    kDebug()<<" text end :"<<text<<" var :"<<var<<endl;
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
    kDebug()<<"d->hideZero :"<<d->hideZero<<" d->showGrid :"<<d->showGrid<<" d->firstLetterUpper :"<<d->firstLetterUpper<<" cursorX :"<<cursorX<<" cursorY :"<<cursorY<< endl;

    d->showFormulaIndicator = items.parseConfigItemBool( "ShowFormulaIndicator" );
    d->showCommentIndicator = items.parseConfigItemBool( "ShowCommentIndicator" );
    d->showPageBorders = items.parseConfigItemBool( "ShowPageBorders" );
    d->lcMode = items.parseConfigItemBool( "lcmode" );
    d->autoCalc = items.parseConfigItemBool( "autoCalc" );
    d->showColumnNumber = items.parseConfigItemBool( "ShowColumnNumber" );
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
    settingsWriter.addConfigItem( "ShowCommentIndicator", d->showCommentIndicator );
    settingsWriter.addConfigItem( "ShowPageBorders",d->showPageBorders );
    settingsWriter.addConfigItem( "lcmode", d->lcMode );
    settingsWriter.addConfigItem( "autoCalc", d->autoCalc );
    settingsWriter.addConfigItem( "ShowColumnNumber", d->showColumnNumber );
    settingsWriter.addConfigItem( "FirstLetterUpper", d->firstLetterUpper );
}


bool Sheet::saveOasis( KoXmlWriter & xmlWriter, KoGenStyles &mainStyles, GenValidationStyles &valStyle, KoStore *store, KoXmlWriter* /*manifestWriter*/, int &indexObj, int &partIndexObj )
{
    int maxCols= 1;
    int maxRows= 1;
    xmlWriter.startElement( "table:table" );
    xmlWriter.addAttribute( "table:name", d->name );
    xmlWriter.addAttribute( "table:style-name", saveOasisSheetStyleName(mainStyles )  );
    if ( !d->password.isEmpty() )
    {
        xmlWriter.addAttribute("table:protected", "true" );
        QByteArray str = KCodecs::base64Encode( d->password );
        xmlWriter.addAttribute("table:protection-key", QString( str.data() ) );/* FIXME !!!!*/
    }
    QRect _printRange = d->print->printRange();
    if ( _printRange != ( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) ) )
    {
        QString range= convertRangeToRef( d->name, _printRange );
        kDebug()<<" range : "<<range<<endl;
        xmlWriter.addAttribute( "table:print-ranges", range );
    }

    saveOasisObjects( store, xmlWriter, mainStyles, indexObj, partIndexObj );
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
    if ( d->print->printObjects() )
      printParameter+="objects ";
    if ( d->print->printCharts() )
      printParameter+="charts ";
    if ( d->showFormula )
        printParameter+="formulas ";
    if ( !printParameter.isEmpty() )
    {
        printParameter+="drawings zero-values"; //default print style attributes in OO
        style.addProperty( "style:print", printParameter );
    }
}

QString Sheet::saveOasisSheetStyleName( KoGenStyles &mainStyles )
{
    KoGenStyle pageStyle( Doc::STYLE_PAGE, "table"/*FIXME I don't know if name is sheet*/ );

    KoGenStyle pageMaster( Doc::STYLE_PAGEMASTER );
    pageMaster.addAttribute( "style:page-layout-name", d->print->saveOasisSheetStyleLayout( mainStyles ) );

    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
    saveOasisHeaderFooter(elementWriter);

    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    pageMaster.addChildElement( "headerfooter", elementContents );
    pageStyle.addAttribute( "style:master-page-name", mainStyles.lookup( pageMaster, "Standard" ) );

    pageStyle.addProperty( "table:display", !d->hide );
    return mainStyles.lookup( pageStyle, "ta" );
}


void Sheet::saveOasisColRowCell( KoXmlWriter& xmlWriter, KoGenStyles& mainStyles,
                                 int maxCols, int maxRows, GenValidationStyles& valStyle )
{
    Q_UNUSED( maxCols );
    kDebug() << "Sheet::saveOasisColRowCell: " << d->name << endl;
    int i = 1;
    ColumnFormat* column = columnFormat( i );
    ColumnFormat* nextColumn = d->columns.next( i );
    while ( !column->isDefault() || nextColumn )
    {
//         kDebug() << "Sheet::saveOasisColRowCell: first col loop: "
//                   << "i: " << i << " "
//                   << "column: " << column->column() << endl;
        KoGenStyle currentColumnStyle( Doc::STYLE_COLUMN, "table-column" );
        currentColumnStyle.addPropertyPt( "style:column-width", column->dblWidth() );/*FIXME pt and not mm */
        currentColumnStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

        //style default layout for column
        KoGenStyle currentDefaultCellStyle; // the type is determined in saveOasisCellStyle
        QString currentDefaultCellStyleName = column->saveOasisCellStyle( currentDefaultCellStyle, mainStyles );

        bool hide = column->isHide();
        int j = i + 1;
        int repeated = 1;

        while ( nextColumn )
        {
//           kDebug() << "Sheet::saveOasisColRowCell: second col loop:"
//               << "j: " << j << " "
//               << "column: " << nextColumn->column() << endl;
          // not the adjacent column?
          if ( nextColumn->column() != j )
          {
            if ( column->isDefault() )
            {
              // if the origin column was a default column,
              // we count the default columns
              repeated = nextColumn->column() - j + 1;
            }
            // otherwise we just stop here to process the adjacent
            // default column in the next iteration of the outer loop
            break;
          }

          KoGenStyle nextColumnStyle( Doc::STYLE_COLUMN, "table-column" );
          nextColumnStyle.addPropertyPt( "style:column-width", nextColumn->dblWidth() );/*FIXME pt and not mm */
          nextColumnStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

          KoGenStyle nextDefaultCellStyle; // the type is determined in saveOasisCellStyle
          QString nextDefaultCellStyleName = nextColumn->saveOasisCellStyle( nextDefaultCellStyle, mainStyles );

          if ( hide != nextColumn->isHide() ||
               nextDefaultCellStyleName != currentDefaultCellStyleName ||
               !( nextColumnStyle == currentColumnStyle ) )
          {
            break;
          }

          ++repeated;
          nextColumn = d->columns.next( j++ );
        }

        xmlWriter.startElement( "table:table-column" );
        xmlWriter.addAttribute( "table:style-name", mainStyles.lookup( currentColumnStyle, "co" ) );

        //FIXME don't create format if it's default format

        // skip 'table:default-cell-style-name' attribute for the default style
        if ( !currentDefaultCellStyle.isDefaultStyle() )
            xmlWriter.addAttribute( "table:default-cell-style-name", currentDefaultCellStyleName );

        if ( hide )
            xmlWriter.addAttribute( "table:visibility", "collapse" );

        if ( repeated > 1 )
            xmlWriter.addAttribute( "table:number-columns-repeated", repeated  );

        xmlWriter.endElement();

        kDebug() << "Sheet::saveOasisColRowCell: column " << i << " "
                  << "repeated " << repeated << " time(s)" << endl;
        i += repeated;
        column = columnFormat( i );
        nextColumn = d->columns.next( i );
    }

    // we have to loop through all rows of the used area
    for ( i = 1; i <= maxRows; ++i )
    {
//         kDebug() << "Sheet::saveOasisColRowCell: row: " << i << endl;
        const RowFormat* row = rowFormat( i );

        KoGenStyle currentRowStyle( Doc::STYLE_ROW, "table-row" );
        currentRowStyle.addPropertyPt( "style:row-height", row->dblHeight());/*FIXME pt and not mm */
        currentRowStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

        xmlWriter.startElement( "table:table-row" );

        // TODO skip attribute saving for default row

        xmlWriter.addAttribute( "table:style-name", mainStyles.lookup( currentRowStyle, "ro" ) );
        int repeated = 1;
        // empty row?
        if ( !getFirstCellRow( i ) )
        {
            bool hide = row->isHide();
            int j = i + 1;
            RowFormat *nextRow = d->rows.next( i );
            while ( nextRow )
            {
//               kDebug() << "Sheet::saveOasisColRowCell: row loop:"
//                         << "j: " << j << " "
//                         << "row: " << nextRow->row() << endl;
              // not the adjacent column?
              if ( nextRow->row() != j )
              {
                if ( row->isDefault() )
                {
                  // if the origin row was a default row,
                  // we count the default rows
                  repeated = nextRow->row() - j + 1;
                }
                // otherwise we just stop here to process the adjacent
                // default row in the next iteration of the outer loop
                break;
              }

              KoGenStyle nextRowStyle( Doc::STYLE_ROW, "table-row" );
              nextRowStyle.addPropertyPt( "style:row-height", nextRow->dblHeight() );/*FIXME pt and not mm */
              nextRowStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

              // TODO default cell style name

              if ( hide != nextRow->isHide() ||
                   !(nextRowStyle == currentRowStyle) )
              {
                break;
              }

              ++repeated;
              nextRow = d->rows.next( j++ );
            }

            kDebug() << "Sheet::saveOasisColRowCell: empty row " << i << " "
                      << "repeated " << repeated << " time(s)" << endl;
            i += repeated - 1; /*it's already incremented in the for loop*/

            if ( repeated > 1 )
                xmlWriter.addAttribute( "table:number-rows-repeated", repeated  );
        }
        else
        {
            saveOasisCells( xmlWriter, mainStyles, i, valStyle );
        }

        if ( row->isHide() )
            xmlWriter.addAttribute( "table:visibility", "collapse" );

        xmlWriter.endElement();
    }
}

void Sheet::saveOasisCells( KoXmlWriter& xmlWriter, KoGenStyles& mainStyles,
                            int row, GenValidationStyles& valStyle )
{
    int i = 1;
    Cell* cell = cellAt( i, row );
    Cell* nextCell = getNextCellRight( i, row );
    while ( !cell->isDefault() || nextCell )
    {
//         kDebug() << "Sheet::saveOasisCells: "
//                   << "i: " << i << " "
//                   << "column: " << (cell->isDefault() ? 0 : cell->column()) << endl;
        int repeated = 1;
        cell->saveOasis( xmlWriter, mainStyles, row, i, repeated, valStyle );
        i += repeated;
        cell = cellAt( i, row );
        nextCell = getNextCellRight( i, row );
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
            kDebug()<<" Direction not implemented : "<<layoutDir<<endl;
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

    kDebug(36001)<<"Sheet::loadXML: table name="<<d->name<<endl;
    setObjectName(d->name.toUtf8());
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
    if( sheet.hasAttribute( "showCommentIndicator" ) )
    {
        d->showCommentIndicator = (bool)sheet.attribute("showCommentIndicator").toInt( &ok );
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
        if ( !e.isNull() )
        {
            QString tagName=e.tagName();
            if ( tagName == "cell" )
        {
            Cell *cell = new Cell( this, 0, 0 );
            if ( cell->load( e, 0, 0 ) )
                insertCell( cell );
            else
                delete cell; // Allow error handling: just skip invalid cells
        }
            else if ( tagName == "row" )
        {
            RowFormat *rl = new RowFormat( this, 0 );
            if ( rl->load( e ) )
                insertRowFormat( rl );
            else
                delete rl;
        }
            else if ( tagName == "column" )
        {
            ColumnFormat *cl = new ColumnFormat( this, 0 );
            if ( cl->load( e ) )
                insertColumnFormat( cl );
            else
                delete cl;
        }
            else if ( tagName == "object" )
        {
            EmbeddedKOfficeObject *ch = new EmbeddedKOfficeObject( doc(), this );
            if ( ch->load( e ) )
                insertObject( ch );
            else
            {
                ch->embeddedObject()->setDeleted(true);
                delete ch;
            }
        }
            else if ( tagName == "chart" )
        {
          EmbeddedChart *ch = new EmbeddedChart( doc(), this );
          if ( ch->load( e ) )
                insertObject( ch );
          else
          {
            ch->embeddedObject()->setDeleted(true);
            delete ch;
          }
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
        QByteArray str( passwd.toLatin1() );
        d->password = KCodecs::base64Decode( str );
      }
      else
        d->password = QByteArray( "" );
    }

    return true;
}


bool Sheet::loadChildren( KoStore* _store )
{
    Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects() );
    for( ; it.current(); ++it )
    {
        if ( it.current()->sheet() == this && ( it.current()->getType() == OBJECT_KOFFICE_PART || it.current()->getType() == OBJECT_CHART ) )
        {
            kDebug() << "KSpreadSheet::loadChildren" << endl;
            if ( !dynamic_cast<EmbeddedKOfficeObject*>( it.current() )->embeddedObject()->loadDocument( _store ) )
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

void Sheet::updateCellArea(const Region& cellArea)
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

void Sheet::emit_updateRow( RowFormat *_format, int _row, bool repaint )
{
    if ( doc()->isLoading() )
        return;

    Cell* c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
      if ( c->row() == _row )
          c->setLayoutDirtyFlag( true );

    if ( repaint )
    {
        //All the cells in this row, or below this row will need to be repainted
        //So add that region of the sheet to the paint dirty list.
        setRegionPaintDirty( QRect( 0 , _row , KS_colMax , KS_rowMax) );

      emit sig_updateVBorder( this );
      emit sig_updateView( this );
    }
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

    //All the cells in this column or to the right of it will need to be repainted if the column
    //has been resized or hidden, so add that region of the sheet to the paint dirty list.
    setRegionPaintDirty( QRect( _column , 0 , KS_colMax , KS_rowMax) );

    emit sig_updateHBorder( this );
    emit sig_updateView( this );
    emit sig_maxColumn( maxColumn() );



    _format->clearDisplayDirtyFlag();
}

bool Sheet::insertChart( const KoRect& _rect, KoDocumentEntry& _e, const QRect& _data )
{
    kDebug(36001) << "Creating document" << endl;
    KoDocument* dd = _e.createDoc();
    kDebug(36001) << "Created" << endl;
    if ( !dd )
        // Error message is already displayed, so just return
        return false;

    kDebug(36001) << "NOW FETCHING INTERFACE" << endl;

    if ( !dd->initDoc(KoDocument::InitDocEmbedded) )
        return false;

    EmbeddedChart * ch = new EmbeddedChart( doc(), this, dd, _rect );
    ch->setDataArea( _data );
    ch->update();
    ch->chart()->setCanChangeValue( false  );

    KoChart::WizardExtension * wiz = ch->chart()->wizardExtension();

    Range dataRange;
    dataRange.setRange( _data );
    dataRange.setSheet( this );

    QString rangeString=dataRange.toString();

    if ( wiz )
        wiz->show( rangeString );

    insertObject( ch );

    return true;
}

bool Sheet::insertChild( const KoRect& _rect, KoDocumentEntry& _e )
{
    KoDocument* d = _e.createDoc( doc() );
    if ( !d )
    {
        kDebug() << "Error inserting child!" << endl;
        return false;
    }
    if ( !d->initDoc(KoDocument::InitDocEmbedded) )
        return false;

    EmbeddedKOfficeObject* ch = new EmbeddedKOfficeObject( doc(), this, d, _rect );
    insertObject( ch );
    return true;
}

bool Sheet::insertPicture( const KoPoint& point , const KUrl& url )
{
    KoPicture picture = doc()->pictureCollection()->downloadPicture( url , 0 );

    return insertPicture(point,picture);
}

bool Sheet::insertPicture( const KoPoint& point ,  KoPicture& picture )
{

    if (picture.isNull())
	    return false;

    KoPictureKey key = picture.getKey();

    KoRect destinationRect;
    destinationRect.setLeft( point.x()  );
    destinationRect.setTop( point.y()  );

    //Generate correct pixel size - this is a bit tricky.
    //This ensures that when we load the image it appears
    //the same size on screen on a 100%-zoom KSpread spreadsheet as it would in an
    //image viewer or another spreadsheet program such as OpenOffice.
    //
    //KoUnit assumes 72DPI, whereas the user's display resolution will probably be
    //different (eg. 96*96).  So, we convert the actual size in pixels into inches
    //using the screen display resolution and then use KoUnit to convert back into
    //the appropriate pixel size KSpread.

    KoSize destinationSize;

    double inchWidth = (double)picture.getOriginalSize().width() / KoGlobal::dpiX();
    double inchHeight = (double)picture.getOriginalSize().height() / KoGlobal::dpiY();

    destinationSize.setWidth( KoUnit::fromUserValue(inchWidth,KoUnit::U_INCH) );
    destinationSize.setHeight( KoUnit::fromUserValue(inchHeight,KoUnit::U_INCH) );

    destinationRect.setSize( destinationSize);

    EmbeddedPictureObject* object = new EmbeddedPictureObject( this, destinationRect, doc()->pictureCollection(),key);
   // ch->setPicture(key);

    insertObject( object );
    return true;
}

bool Sheet::insertPicture( const KoPoint& point, const QPixmap& pixmap  )
{
	QByteArray data;
	QBuffer buffer( &data );

	buffer.open( QIODevice::ReadWrite );
	pixmap.save( &buffer , "PNG" );

	//Reset the buffer so that KoPicture reads the whole file from the beginning
	//(at the moment the read/write position is at the end)
	buffer.reset();

	KoPicture picture;
	picture.load( &buffer , "PNG" );

	doc()->pictureCollection()->insertPicture(picture);

	return insertPicture( point , picture );
}

void Sheet::insertObject( EmbeddedObject *_obj )
{
    doc()->insertObject( _obj );
    emit sig_updateView( _obj );
}

void Sheet::changeChildGeometry( EmbeddedKOfficeObject *_child, const KoRect& _rect )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );
}

bool Sheet::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;

    Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects() );
    for( ; it.current(); ++it )
    {
        if ( it.current()->sheet() == this && ( it.current()->getType() == OBJECT_KOFFICE_PART || it.current()->getType() == OBJECT_CHART ) )
        {
            QString path = QString( "%1/%2" ).arg( _path ).arg( i++ );
            if ( !dynamic_cast<EmbeddedKOfficeObject*>( it.current() )->embeddedObject()->document()->saveToStore( _store, path ) )
                return false;
        }
    }
    return true;
}

bool Sheet::saveOasisObjects( KoStore */*store*/, KoXmlWriter &xmlWriter, KoGenStyles& mainStyles, int & indexObj, int &partIndexObj )
{
  //int i = 0;
  if ( doc()->embeddedObjects().isEmpty() )
    return true;

  bool objectFound = false; // object on this sheet?
  EmbeddedObject::KSpreadOasisSaveContext sc( xmlWriter, mainStyles, indexObj, partIndexObj );
  Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects() );
  for( ; it.current(); ++it )
  {
    if ( it.current()->sheet() == this && ( doc()->savingWholeDocument() || it.current()->isSelected() ) )
    {
      if ( !objectFound )
      {
        xmlWriter.startElement( "table:shapes" );
        objectFound = true;
      }
      if ( !it.current()->saveOasisObject(sc)  )
      {
        xmlWriter.endElement();
        return false;
      }
      ++indexObj;
    }
  }
  if ( objectFound )
  {
    xmlWriter.endElement();
  }
  return true;
}

Sheet::~Sheet()
{
    //Disable automatic recalculation of dependancies on this sheet to prevent crashes
    //in certain situations:
    //
    //For example, suppose a cell in SheetB depends upon a cell in SheetA.  If the cell in SheetB is emptied
    //after SheetA has already been deleted, the program would try to remove dependancies from the cell in SheetA
    //causing a crash.
    setAutoCalc(false);

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

    //this is for debugging a crash
    d=0;
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

    Q3PtrListIterator<Sheet> it( workbook()->sheetList() );
    for ( ; it.current(); ++it )
        it.current()->changeCellTabName( old_name, name );

    doc()->changeAreaSheetName( old_name, name );
    emit sig_nameChanged( this, old_name );

    setObjectName(name.toUtf8());
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
    c->setCellText( _text );
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

      c->format()->setTopBorderStyle(Qt::NoPen);
      c->format()->setLeftBorderStyle(Qt::NoPen);
      c->format()->setRightBorderStyle(Qt::NoPen);
      c->format()->setBottomBorderStyle(Qt::NoPen);

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

// TODO Stefan: these belong to View, even better Canvas
void Sheet::setRegionPaintDirty( Region const & region )
{
  Manipulator* manipulator = new DilationManipulator();
  manipulator->setSheet(this);
  manipulator->add(region);
  manipulator->execute();
  // don't put it in the undo list! ;-)
  d->paintDirtyList.add(*manipulator);
  kDebug() << "setRegionPaintDirty "<< static_cast<Region*>(manipulator)->name(this) << endl;
  delete manipulator;
}

void Sheet::setRegionPaintDirty( QRect const & range )
{

    d->paintDirtyList.add(range);

}

void Sheet::clearPaintDirtyData()
{
  d->paintDirtyList.clear();
}

bool Sheet::cellIsPaintDirty( QPoint const & cell ) const
{
  return d->paintDirtyList.contains(cell);
}

#ifndef NDEBUG
void Sheet::printDebug()
{
    int iMaxColumn = maxColumn();
    int iMaxRow = maxRow();

    kDebug(36001) << "Cell | Content  | DataT | Text" << endl;
    Cell *cell;
    for ( int currentrow = 1 ; currentrow < iMaxRow ; ++currentrow )
    {
        for ( int currentcolumn = 1 ; currentcolumn < iMaxColumn ; currentcolumn++ )
        {
            cell = cellAt( currentcolumn, currentrow );
            if ( !cell->isDefault() && !cell->isEmpty() )
            {
                QString cellDescr = Cell::name( currentcolumn, currentrow );
                cellDescr = cellDescr.rightJustified( 4,' ' );
                //QString cellDescr = "Cell ";
                //cellDescr += QString::number(currentrow).rightJustified(3,'0') + ',';
                //cellDescr += QString::number(currentcolumn).rightJustified(3,'0') + ' ';
                cellDescr += " | ";
                cellDescr += cell->value().type();
                cellDescr += " | ";
                cellDescr += cell->text();
                if ( cell->isFormula() )
                    cellDescr += QString("  [result: %1]").arg( cell->value().asString() );
                kDebug(36001) << cellDescr << endl;
            }
        }
    }
}
#endif

} // namespace KSpread

