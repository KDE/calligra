/* This file is part of the KDE project
   Copyright 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 1999- 2006 The KSpread Team <koffice-devel@kde.org>

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

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QLinkedList>
#include <QList>
#include <QMap>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QRegExp>
#include <QStack>
#include <QTextStream>

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

#include "Commands.h"
#include "DependencyManager.h"
#include "Selection.h"
#include "LoadingInfo.h"
#include "Canvas.h"
#include "Cluster.h"
#include "Condition.h"
#include "Doc.h"
#include "Global.h"
#include "Locale.h"
#include "Map.h"
#include "Object.h"
#include "RecalcManager.h"
#include "SheetPrint.h"
#include "Style.h"
#include "StyleManager.h"
#include "Undo.h"
#include "Util.h"
#include "Validity.h"
#include "View.h"
#include "DataManipulators.h"


#include "SheetAdaptor.h"
#include <QtDBus/QtDBus>


#include "Sheet.h"
#include "Sheet.moc"

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

  SheetAdaptor* dbus;

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

  // List of all cell bindings. For example charts use bindings to get
  // informed about changing cell contents.
  QList<CellBinding*> cellBindings;

  // Indicates whether the sheet should paint the page breaks.
  // Doing so costs some time, so by default it should be turned off.
  bool showPageBorders;

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
};

int Sheet::s_id = 0;
QHash<int,Sheet*>* Sheet::s_mapSheets;

Sheet* Sheet::find( int _id )
{
  if ( !s_mapSheets )
    return 0;

  return (*s_mapSheets)[ _id ];
}

Sheet::Sheet( Map* map, const QString &sheetName, const char *objectName )
  : QObject( map )
{
  Q_ASSERT(objectName);
  // Get a unique name so that we can offer scripting
  if ( !objectName )
  {
    objectName = "Sheet" + QByteArray::number(s_id);
  }
  setObjectName( objectName );

  if ( s_mapSheets == 0 )
    s_mapSheets = new QHash<int,Sheet*>;
  d = new Private;

  d->workbook = map;

  d->id = s_id++;
  s_mapSheets->insert( d->id, this );

  d->layoutDirection = LeftToRight;

  d->defaultFormat = new Format (this, d->workbook->doc()->styleManager()->defaultStyle());
  d->emptyPen.setStyle( Qt::NoPen );
  d->name = sheetName;

  new SheetAdaptor(this);
  QDBus::sessionBus().registerObject( '/'+map->doc()->objectName() + '/' + map->objectName()+ '/' + objectName, this);

  d->cells.setAutoDelete( true );
  d->rows.setAutoDelete( true );
  d->columns.setAutoDelete( true );

  d->defaultCell = new Cell( this, d->workbook->doc()->styleManager()->defaultStyle(), 0, 0);
  d->defaultRowFormat = new RowFormat( this, 0 );
  d->defaultRowFormat->setDefault();
  d->defaultColumnFormat = new ColumnFormat( this, 0 );
  d->defaultColumnFormat->setDefault();

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
  d->print = new SheetPrint( this );

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

Sheet::LayoutDirection Sheet::layoutDirection() const
{
  return d->layoutDirection;
}

void Sheet::setLayoutDirection( LayoutDirection dir )
{
  d->layoutDirection = dir;
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
    if ( p != 0 )
        return p;

    return d->defaultColumnFormat;
}

ColumnFormat* Sheet::columnFormat( int _column )
{
    ColumnFormat *p = d->columns.lookup( _column );
    if ( p != 0 )
        return p;

    return d->defaultColumnFormat;
}

const RowFormat* Sheet::rowFormat( int _row ) const
{
    const RowFormat *p = d->rows.lookup( _row );
    if ( p != 0 )
        return p;

    return d->defaultRowFormat;
}

RowFormat* Sheet::rowFormat( int _row )
{
    RowFormat *p = d->rows.lookup( _row );
    if ( p != 0 )
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

int Sheet::numberSelectedObjects() const
{
    int num = 0;

    foreach ( EmbeddedObject* object, d->workbook->doc()->embeddedObjects() )
    {
        if( object->sheet() == this && object->isSelected() )
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
      kDebug(36001) << "Sheet:leftColumn: invalid column (col: " << col + 1 << ')' << endl;
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
    double x = columnFormat( col )->dblWidth( _canvas );
    while ( x <= _xpos )
    {
        // Should never happen
        if ( col > KS_colMax )
        {
            kDebug(36001) << "Sheet:rightColumn: invalid column (col: " << col << ')' << endl;
                  return KS_colMax + 1; //Return out of range value, so other code can react on this
        }
        x += columnFormat( col )->dblWidth( _canvas );
        col++;
    }

    return col;
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
            kDebug(36001) << "Sheet:topRow: invalid row (row: " << row + 1 << ')' << endl;
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
    double y = rowFormat( row )->dblHeight( _canvas );
    while ( y <= _ypos )
    {
        // Should never happen
        if ( row > KS_rowMax )
        {
            kDebug(36001) << "Sheet:bottomRow: invalid row (row: " << row << ')' << endl;
                  return KS_rowMax + 1; //Return out of range value, so other code can react on this
        }
        y += rowFormat( row )->dblHeight( _canvas );
        row++;
    }

    return row;
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
      kDebug(36001) << "Sheet:columnPos: invalid column (col: " << col << ')' << endl;
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
      kDebug(36001) << "Sheet:rowPos: invalid row (row: " << row << ')' << endl;
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
    if ( p != 0 )
        return p;

    return d->defaultCell;
}

Cell* Sheet::cellAt( int _column, int _row, bool _scrollbar_update )
{
  if ( _scrollbar_update && d->scrollBarUpdates )
  {
    checkRangeHBorder( _column );
    checkRangeVBorder( _row );
  }

  Cell *p = d->cells.lookup( _column, _row );
  if ( p != 0 )
    return p;

  return d->defaultCell;
}

ColumnFormat* Sheet::nonDefaultColumnFormat( int _column, bool force_creation )
{
    ColumnFormat *p = d->columns.lookup( _column );
    if ( p != 0 || !force_creation )
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
    if ( p != 0 || !force_creation )
        return p;

    p = new RowFormat( this, _row );
    // TODO: copy the default RowLFormat here!!
    p->setDblHeight( d->defaultRowFormat->dblHeight() );

    d->rows.insertElement( p, _row );

    return p;
}

Cell* Sheet::nonDefaultCell( int _column, int _row, Style* _style )
{
  Cell * p = d->cells.lookup( _column, _row );
  if ( p != 0 )
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
  DataManipulator *dm = new DataManipulator ();
  dm->setSheet (this);
  dm->setValue (_text);
  dm->setParsing (!asString);
  dm->add (QPoint (_column, _row));
  dm->execute ();

  //refresh anchor
  if ((!_text.isEmpty()) && (_text.at(0)=='!'))
    emit sig_updateView( this, Region( _column, _row, this ) );
}

void Sheet::setArrayFormula (Selection *selectionInfo, const QString &_text)
{
  // create and call the manipulator
  ArrayFormulaManipulator *afm = new ArrayFormulaManipulator;
  afm->setSheet (this);
  afm->setText (_text);
  afm->add (*selectionInfo);
  afm->execute ();
}

void Sheet::setLayoutDirtyFlag()
{
    Cell * c = d->cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->setLayoutDirtyFlag();
}

void Sheet::recalc( bool force )
{
  ElapsedTime et( "Recalculating " + d->name, ElapsedTime::PrintOnlyTime );
  //  emitBeginOperation(true);
  //  setRegionPaintDirty(QRect(QPoint(1,1), QPoint(KS_colMax, KS_rowMax)));

  //If automatic calculation is disabled, don't recalculate unless the force flag has been
  //set.
  if ( !getAutoCalc() && !force )
        return;

  // Recalculate cells
  d->workbook->recalcManager()->recalcSheet(this);

  //  emitEndOperation();
  emit sig_updateView( this );
}

void Sheet::valueChanged (Cell *cell)
{
  //TODO: call cell updating, when cell damaging implemented

  // Recaculate cells depending on this cell.
  if ( !doc()->isLoading() && getAutoCalc() )
  {
    // Prepare the Region structure.
    Region region;
    region.add(QPoint(cell->column(), cell->row()), this);

    d->workbook->recalcManager()->regionChanged(region);
  }

  //REMOVED - modification change - this was causing modified flag to be set inappropriately.
  //nobody else seems to be setting the modified flag, so we do it here
//  doc()->setModified (true);
}

void Sheet::formulaChanged(Cell *cell)
{
  // NOTE Stefan: Always update the dependencies, not just for recalculations,
  //              because we want formulas to be updated on cut'n'paste.

  // Prepare the Region structure.
  Region region;
  region.add(QPoint(cell->column(), cell->row()), this);

  // Update dependencies.
  if (!doc()->isLoading())
    d->workbook->dependencyManager()->regionChanged (region);
}

void Sheet::setSelectionUpperLower( Selection* selectionInfo, int _type )
{
  CaseManipulator::CaseMode m;
  QString name;
  if (_type == -1) {
    m = CaseManipulator::Lower;
    name = i18n ("Switch to lowercase");
  }
  else if (_type == 1) {
    m = CaseManipulator::Upper;
    name = i18n ("Switch to uppercase");
  }
  else
    return;  // wrong type
  CaseManipulator *manipulator = new CaseManipulator;
  manipulator->setSheet (this);
  manipulator->setName (name);
  manipulator->changeMode (m);
  manipulator->add (*selectionInfo);
  manipulator->execute ();
}

void Sheet::setSelectionfirstLetterUpper( Selection* selectionInfo)
{
  CaseManipulator *manipulator = new CaseManipulator;
  manipulator->setSheet (this);
  manipulator->setName (i18n ("First letter uppercase"));
  manipulator->changeMode (CaseManipulator::FirstUpper);
  manipulator->add (*selectionInfo);
  manipulator->execute ();
}

void Sheet::slotAreaModified (const QString &name)
{
  d->workbook->dependencyManager()->areaModified (name);
}


void Sheet::refreshRemoveAreaName(const QString & _areaName)
{
  Cell * c = d->cells.firstCell();
  QString tmp = '\'' + _areaName + '\'';
  for( ;c ; c = c->nextCell() )
  {
    if ( c->isFormula() )
    {
      if (c->text().indexOf(tmp) != -1)
      {
        c->makeFormula();
      }
    }
  }
}

void Sheet::refreshChangeAreaName(const QString & _areaName)
{
  Region region; // for recalculation
  Cell * c = d->cells.firstCell();
  QString tmp = '\'' + _areaName + '\'';
  for( ;c ; c = c->nextCell() )
  {
    if ( c->isFormula() )
    {
      if (c->text().indexOf(tmp) != -1)
      {
        if ( c->makeFormula() )
          region.add(QPoint(c->column(), c->row()), c->sheet());
      }
    }
  }
  // recalculate cells
  d->workbook->recalcManager()->regionChanged(region);
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
                int nb = c->text().count( old_name + '!' );
                QString tmp = old_name + '!';
                int len = tmp.length();
                tmp=c->text();

                for( int i=0; i<nb; i++ )
                {
                    int pos = tmp.indexOf( old_name + '!' );
                    tmp.replace( pos, len, new_name + '!' );
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
    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      for ( int i = rect.top(); i <= rect.bottom(); ++i )
      {
        sheet->changeNameCellRef( QPoint( rect.left(), i ), false,
                                  Sheet::ColumnInsert, objectName(),
                                  rect.right() - rect.left() + 1,
                                  undo );
      }
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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      for ( int i = rect.left(); i <= rect.right(); ++i )
      {
        sheet->changeNameCellRef( QPoint( i, rect.top() ), false,
                                  Sheet::RowInsert, objectName(),
                                  rect.bottom() - rect.top() + 1,
                                  undo );
      }
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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      for ( int i = rect.left(); i <= rect.right(); ++i )
      {
        sheet->changeNameCellRef( QPoint( i, rect.top() ), false,
                                  Sheet::RowRemove, objectName(),
                                  rect.bottom() - rect.top() + 1,
                                  undo );
      }
    }

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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      for ( int i = rect.top(); i <= rect.bottom(); ++i )
      {
        sheet->changeNameCellRef( QPoint( rect.left(), i ), false,
                                  Sheet::ColumnRemove, objectName(),
                                  rect.right() - rect.left() + 1,
                                  undo);
      }
    }

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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
        sheet->changeNameCellRef( QPoint( col, 1 ), true,
                                  Sheet::ColumnInsert, objectName(),
                                  nbCol + 1, undo );
    }

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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      sheet->changeNameCellRef( QPoint( 1, row ), true,
                                Sheet::RowInsert, objectName(),
                                nbRow + 1, undo );
    }

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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
        sheet->changeNameCellRef( QPoint( col, 1 ), true,
                                  Sheet::ColumnRemove, objectName(),
                                  nbCol + 1, undo );
    }

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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      sheet->changeNameCellRef( QPoint( 1, row ), true,
                                Sheet::RowRemove, objectName(),
                                nbRow + 1, undo );
    }

    //update print settings
    d->print->removeRow( row, nbRow );

    refreshChart( QPoint( 1, row ), true, Sheet::RowRemove );
    refreshMergedCell();
    recalc();
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void Sheet::emitHideRow()
{
    emit sig_updateVBorder( this );
    emit sig_updateView( this );
}

void Sheet::emitHideColumn()
{
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
  if (c == 0)
  {
     foreach ( CellBinding * binding, d->cellBindings )
     {
       binding->cellChanged( 0 );
     }
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
      int i = 0;
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
            correctSheetName = ( newText.right( tabname.length()+1 ) == tabname+'!' );
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
                newPoint = '#' + i18n("Dependency") + '!';
                error = true;
              }

              newText += newPoint;
            }
            else // Not a cell ref
            {
              kDebug(36001) << "Copying (unchanged) : '" << str << '\'' << endl;
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
    QRect range = (*it)->rect();
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
                        if ( c->validity())
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
                        if ( c->validity())
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
                        if ( cell->validity())
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


void Sheet::clearText( Selection* selectionInfo )
{
  if (areaIsEmpty(*selectionInfo))
    return;

  DataManipulator* manipulator = new DataManipulator();
  manipulator->setSheet(this);
  manipulator->setName (i18n ("Clear Text"));
  // parsing gets set only so that setCellText is called as it should be,
  // no actual parsing shall be done
  manipulator->setParsing (true);
  manipulator->setValue (Value (""));
  manipulator->add(*selectionInfo);
  manipulator->execute();
}


/**
 * Here we define two manipulators - GetWordSpellingManipulator and
 * SetWordSpellingManipulator. This is not ideal, but these two are so specific
 * that there isn't much use in declaring them in *Manipulators.h
 * */

// returns all the strings in the range. Name a bit confusing.
class GetWordSpellingManipulator : public Manipulator {
 public:
  QString getSpelling () {
    QString res;
    
    Region::Iterator endOfList(cells().end());
    for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
    {
      Region::Element *element = *it;
      QRect range = element->rect();

      for (int col = range.left(); col <= range.right(); ++col) {
        for (int row = range.top(); row <= range.bottom(); ++row) {
          Cell *cell = m_sheet->cellAt (col, row);
          if (cell->value().isString() && (!cell->isFormula())) {
            QString txt = cell->value().asString();
            if (!txt.isEmpty())
              res += txt + '\n';
          }
        }
      }
    }
    return res;
  };  
};

QString Sheet::getWordSpelling(Selection* selectionInfo )
{
  GetWordSpellingManipulator manipulator;
  manipulator.setSheet (this);
  manipulator.add (*selectionInfo);
  return manipulator.getSpelling();
}

// applies new strings to the range
class SetWordSpellingManipulator : public AbstractDataManipulator {
 public:
  SetWordSpellingManipulator () : idx(0) {
    setName (i18n ("Set Word Spelling"));  // TODO: is the name correct ?
  }
  void setString (QString str) {
    list = str.split ('\n');
    idx = 0;
  };
  
 protected:
  int idx;
  QStringList list;
  Value newValue (Element *, int col, int row, bool *parsing, FormatType *)
  {
    *parsing = false;
    // find out whether this cell was supplying data for the original list
    Cell *cell = m_sheet->cellAt (col, row);
    if (cell->value().isString() && (!cell->isFormula())) {
      QString txt = cell->value().asString();
      if (!txt.isEmpty())
        // yes it was - return new data
        return list[idx++];
      // no it wasn't - keep old data
    }
    return cell->value();
  }
};

void Sheet::setWordSpelling(Selection* selectionInfo, const QString _listWord )
{
  SetWordSpellingManipulator *manipulator = new SetWordSpellingManipulator;
  manipulator->setSheet (this);
  manipulator->setString (_listWord);
  manipulator->add (*selectionInfo);
  manipulator->execute ();
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
          result += ' ';
        result += cell->strOutText();
    }
    else if (cell->defineAlignX() == Style::Left )
      {
          result += ' ';
          result += cell->strOutText();
          // start with '1' because we already set one space
          for ( int i = 1; i < l; ++i )
            result += ' ';
       }
         else // centered
         {
           int i;
           int s = (int) l / 2;
           for ( i = 0; i < s; ++i )
             result += ' ';
           result += cell->strOutText();
           for ( i = s; i < l; ++i )
             result += ' ';
          }
  }
  else
  {
    for ( unsigned int i = 0; i < max; ++i )
      result += ' ';
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

    int max = 1;
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
      result += '\n';
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

    QMimeData* mimeData = new QMimeData();
    mimeData->setText( copyAsText(selectionInfo) );
    mimeData->setData( "application/x-kspread-snippet", buffer.buffer() );

    QApplication::clipboard()->setMimeData( mimeData );
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

    QMimeData* mimeData = new QMimeData();
    mimeData->setText( copyAsText(selectionInfo) );
    mimeData->setData( "application/x-kspread-snippet", buffer.buffer() );

    QApplication::clipboard()->setMimeData( mimeData );

    deleteSelection( selectionInfo, true );
}

void Sheet::paste( const QRect& pasteArea, bool makeUndo,
                   Paste::Mode mode, Paste::Operation operation,
                   bool insert, int insertTo, bool pasteFC,
                   QClipboard::Mode clipboardMode )
{
    const QMimeData* mimeData = QApplication::clipboard()->mimeData( clipboardMode );
    if ( !mimeData )
        return;

    QByteArray b;

    if ( mimeData->hasFormat( "application/x-kspread-snippet" ) )
    {
      b = mimeData->data( "application/x-kspread-snippet" );
    }
    else if( mimeData->hasText() )
    {
      // ### Stefan: Is this still true for Qt4?
      // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
      // In particular it handles charsets (in the mimetype). Copied from KPresenter ;-)
      QString _text = QApplication::clipboard()->text( clipboardMode );
      pasteTextPlain( _text, pasteArea );
      return;
    }
    else
      // TODO: complain about unrecognized type ?
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

  if (_text.isEmpty())
    return;

  int mx = pasteArea.left();
  int my = pasteArea.top();
  
  // split the text into lines and put them into an array value
  QStringList list = _text.split( '\n' );
  Value value (1, list.size());
  int which = 0;
  QStringList::iterator it;
  for (it = list.begin(); it != list.end(); ++it)
    value.setElement (0, which++, Value (*it));

  Region range (mx, my, 1, list.size());
  
  // create a manipulator, configure it and execute it
  DataManipulator *manipulator = new DataManipulator;
  manipulator->setSheet (this);
  manipulator->setParsing (false);
  manipulator->setValue (value);
  manipulator->add (range);
  manipulator->execute ();
}

void Sheet::paste( const QByteArray& b, const QRect& pasteArea, bool makeUndo,
                   Paste::Mode mode, Paste::Operation operation,
                   bool insert, int insertTo, bool pasteFC )
{
    kDebug(36001) << "Parsing " << b.size() << " bytes" << endl;

    QString errorMsg;
    int errorLine;
    int errorColumn;
    KoXmlDocument doc;
    if ( !doc.setContent( b, false, &errorMsg, &errorLine, &errorColumn ) )
    {
      // an error occurred
      kDebug() << "Sheet::paste(const QByteArray&): an error occurred" << endl
               << "line: " << errorLine << " col: " << errorColumn
               << ' ' << errorMsg << endl;
      return;
    }

    int mx = pasteArea.left();
    int my = pasteArea.top();

    loadSelection( doc, pasteArea, mx - 1, my - 1, makeUndo,
                   mode, operation, insert, insertTo, pasteFC );
}

bool Sheet::loadSelection(const KoXmlDocument& doc, const QRect& pasteArea,
                          int _xshift, int _yshift, bool makeUndo,
                          Paste::Mode mode, Paste::Operation operation, bool insert,
                          int insertTo, bool pasteFC)
{
  //kDebug(36001) << "loadSelection called. pasteArea=" << pasteArea << endl;

  if (!isLoading() && makeUndo)
  {
    loadSelectionUndo( doc, pasteArea, _xshift, _yshift, insert, insertTo );
  }

  KoXmlElement root = doc.documentElement(); // "spreadsheet-snippet"

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

  Region recalcRegion;
  KoXmlElement e = root.firstChild().toElement(); // "columns", "rows" or "cell"
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
        KoXmlElement c = e.firstChild().toElement();
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
        KoXmlElement c = e.firstChild().toElement();
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
    Cell *cellBackup = 0;
    if (e.tagName() == "cell")
    {
      int row = e.attribute( "row" ).toInt() + _yshift;
      int col = e.attribute( "column" ).toInt() + _xshift;

      // tile the selection with the clipboard contents
      for (int roff = 0; row + roff - _yshift <= pasteHeight; roff += rowsInClpbrd)
      {
        for (int coff = 0; col + coff - _xshift <= pasteWidth; coff += columnsInClpbrd)
        {
//           kDebug() << "loadSelection: cell at " << (col+coff) << ',' << (row+roff)
//                     << " with roff,coff= " << roff << ',' << coff
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
              recalcRegion.add(QPoint(cell->column(), cell->row()), cell->sheet());
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

  // recalculate cells
  d->workbook->recalcManager()->regionChanged(recalcRegion);

    this->doc()->setModified( true );

    if (!isLoading())
        refreshMergedCell();

    emit sig_updateView( this );
    emit sig_updateHBorder( this );
    emit sig_updateVBorder( this );

    return true;
}

void Sheet::loadSelectionUndo(const KoXmlDocument& d, const QRect& loadArea,
                              int _xshift, int _yshift,
                              bool insert, int insertTo)
{
  KoXmlElement root = d.documentElement(); // "spreadsheet-snippet"

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
  for (KoXmlNode n = root.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    KoXmlElement e = n.toElement(); // "columns", "rows" or "cell"
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

bool Sheet::testAreaPasteInsert() const
{
    const QMimeData* mimeData = QApplication::clipboard()->mimeData( QClipboard::Clipboard );
    if ( !mimeData )
        return false;

    QByteArray byteArray;

    if ( mimeData->hasFormat( "application/x-kspread-snippet" ) )
        byteArray = mimeData->data( "application/x-kspread-snippet" );
    else
        return false;

    QString errorMsg;
    int errorLine;
    int errorColumn;
    KoXmlDocument d;
    if ( !d.setContent( byteArray, false, &errorMsg, &errorLine, &errorColumn ) )
    {
      // an error occurred
      kDebug() << "Sheet::testAreaPasteInsert(): an error occurred" << endl
               << "line: " << errorLine << " col: " << errorColumn
               << ' ' << errorMsg << endl;
      return false;
    }

    KoXmlElement e = d.documentElement();
    if ( !e.namedItem( "columns" ).toElement().isNull() )
        return false;

    if ( !e.namedItem( "rows" ).toElement().isNull() )
        return false;

    KoXmlElement c = e.firstChild().toElement();
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
    QStack<Cell*> cellStack;

  Region::ConstIterator endOfList = region.constEnd();
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect();

    // The RecalcManager needs a valid sheet.
    if ( !(*it)->sheet() )
      (*it)->setSheet( this );

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

    // Remove the cells from the sheet
    d->cells.setAutoDelete( false );
    while ( !cellStack.isEmpty() )
    {
      Cell * cell = cellStack.pop();

      d->cells.remove( cell->column(), cell->row() );

      delete cell;
    }
    d->cells.setAutoDelete( true );

    // recalculate dependant cells
    d->workbook->recalcManager()->regionChanged(region);
    // relayout region
    setLayoutDirtyFlag();
    // repaint region
    setRegionPaintDirty(region);

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
    QRect range = (*it)->rect();

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

void Sheet::updateView(const Region& region)
{
  emit sig_updateView( this, region );
}

void Sheet::refreshView( const Region& region )
{
  Region tmpRegion;
  Region::ConstIterator endOfList = region.constEnd();
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect();
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


// era: absolute references
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
    QRect range = (*it)->rect();

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

      // TODO Stefan: Inefficient, use cluster functionality
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

      // TODO Stefan: Inefficient, use the cluster functionality
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
    enableScrollBarUpdates(false);
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
    enableScrollBarUpdates(true);
  }
  return dd;
}

QDomElement Sheet::saveXML( QDomDocument& dd )
{
    QDomElement sheet = dd.createElement( "table" );
    sheet.setAttribute( "name", sheetName() );

    //Laurent: for oasis format I think that we must use style:direction...
    sheet.setAttribute( "layoutDirection", (layoutDirection() == RightToLeft) ? "rtl" : "ltr" );
    sheet.setAttribute( "columnnumber", (int)getShowColumnNumber());
    sheet.setAttribute( "borders", (int)isShowPageBorders());
    sheet.setAttribute( "hide", (int)isHidden());
    sheet.setAttribute( "hidezero", (int)getHideZero());
    sheet.setAttribute( "firstletterupper", (int)getFirstLetterUpper());
    sheet.setAttribute( "grid", (int)getShowGrid() );
    sheet.setAttribute( "printGrid", (int)print()->printGrid() );
    sheet.setAttribute( "printCommentIndicator", (int)print()->printCommentIndicator() );
    sheet.setAttribute( "printFormulaIndicator", (int)print()->printFormulaIndicator() );
    sheet.setAttribute( "showFormula", (int)getShowFormula());
    sheet.setAttribute( "showFormulaIndicator", (int)getShowFormulaIndicator());
    sheet.setAttribute( "showCommentIndicator", (int)getShowCommentIndicator());
    sheet.setAttribute( "lcmode", (int)getLcMode());
    sheet.setAttribute( "autoCalc", (int)getAutoCalc());
    sheet.setAttribute( "borders1.2", 1);
    QByteArray pwd;
    password (pwd);
    if ( !pwd.isNull() )
    {
      if ( pwd.size() > 0 )
      {
        QByteArray str = KCodecs::base64Encode(pwd);
        sheet.setAttribute( "protected", QString( str.data() ) );
      }
      else
        sheet.setAttribute( "protected", "" );
    }

    // paper parameters
    QDomElement paper = dd.createElement( "paper" );
    paper.setAttribute( "format", print()->paperFormatString() );
    paper.setAttribute( "orientation", print()->orientationString() );
    sheet.appendChild( paper );

    QDomElement borders = dd.createElement( "borders" );
    borders.setAttribute( "left", print()->leftBorder() );
    borders.setAttribute( "top", print()->topBorder() );
    borders.setAttribute( "right", print()->rightBorder() );
    borders.setAttribute( "bottom", print()->bottomBorder() );
    paper.appendChild( borders );

    QDomElement head = dd.createElement( "head" );
    paper.appendChild( head );
    if ( !print()->headLeft().isEmpty() )
    {
      QDomElement left = dd.createElement( "left" );
      head.appendChild( left );
      left.appendChild( dd.createTextNode( print()->headLeft() ) );
    }
    if ( !print()->headMid().isEmpty() )
    {
      QDomElement center = dd.createElement( "center" );
      head.appendChild( center );
      center.appendChild( dd.createTextNode( print()->headMid() ) );
    }
    if ( !print()->headRight().isEmpty() )
    {
      QDomElement right = dd.createElement( "right" );
      head.appendChild( right );
      right.appendChild( dd.createTextNode( print()->headRight() ) );
    }
    QDomElement foot = dd.createElement( "foot" );
    paper.appendChild( foot );
    if ( !print()->footLeft().isEmpty() )
    {
      QDomElement left = dd.createElement( "left" );
      foot.appendChild( left );
      left.appendChild( dd.createTextNode( print()->footLeft() ) );
    }
    if ( !print()->footMid().isEmpty() )
    {
      QDomElement center = dd.createElement( "center" );
      foot.appendChild( center );
      center.appendChild( dd.createTextNode( print()->footMid() ) );
    }
    if ( !print()->footRight().isEmpty() )
    {
      QDomElement right = dd.createElement( "right" );
      foot.appendChild( right );
      right.appendChild( dd.createTextNode( print()->footRight() ) );
    }

    // print range
    QDomElement printrange = dd.createElement( "printrange-rect" );
    QRect _printRange = print()->printRange();
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
    printRepeatColumns.setAttribute( "left", print()->printRepeatColumns().first );
    printRepeatColumns.setAttribute( "right", print()->printRepeatColumns().second );
    sheet.appendChild( printRepeatColumns );

    // Print repeat rows
    QDomElement printRepeatRows = dd.createElement( "printrepeatrows" );
    printRepeatRows.setAttribute( "top", print()->printRepeatRows().first );
    printRepeatRows.setAttribute( "bottom", print()->printRepeatRows().second );
    sheet.appendChild( printRepeatRows );

    //Save print zoom
    sheet.setAttribute( "printZoom", print()->zoom() );

    //Save page limits
    sheet.setAttribute( "printPageLimitX", print()->pageLimitX() );
    sheet.setAttribute( "printPageLimitY", print()->pageLimitY() );

    // Save all cells.
    Cell* c = firstCell();
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
    RowFormat* rl = firstRow();
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
    ColumnFormat* cl = firstCol();
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

    foreach ( EmbeddedObject* object,doc()->embeddedObjects() )
    {
       if ( object->sheet() == this )
       {
         QDomElement e = object->save( dd );

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


QList<EmbeddedObject*> Sheet::getSelectedObjects()
{
    QList<EmbeddedObject*> objects;
    foreach ( EmbeddedObject* object, d->workbook->doc()->embeddedObjects() )
    {
        if( object->isSelected()
            && object->sheet() == this )
        {
            objects.append( object );
        }
    }
     return objects;
}

KoRect Sheet::getRealRect( bool all )
{
    KoRect rect;

    foreach ( EmbeddedObject* object, d->workbook->doc()->embeddedObjects() )
    {

        if ( all || ( object->isSelected() && ! object->isProtect() ) )
            rect |= object->geometry();
    }

    return rect;
}

// move object for releasemouseevent
KCommand *Sheet::moveObject(View *_view, double diffx, double diffy)
{
    bool createCommand=false;
    MoveObjectByCmd *moveByCmd=0;
    Canvas * canvas = _view->canvasWidget();
    QList<EmbeddedObject*> _objects;
    foreach ( EmbeddedObject* object, d->workbook->doc()->embeddedObjects() )
    {
        if ( object->isSelected() && !object->isProtect())
        {
            _objects.append( object );
            KoRect geometry = object->geometry();
            geometry.moveBy( -canvas->xOffset(), -canvas->yOffset() );
            QRect br = doc()->zoomRectOld( geometry/*object->geometry()*/ );
            br.translate( doc()->zoomItXOld( diffx ), doc()->zoomItYOld( diffy ) );
            br.translate( doc()->zoomItXOld( -canvas->xOffset() ), doc()->zoomItYOld( -canvas->yOffset() ) );
            canvas->repaint( br ); // Previous position
            canvas->repaintObject( object ); // New position
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
    QList<EmbeddedObject*> _objects;
    MoveObjectByCmd *moveByCmd=0;
    Canvas * canvas = _view->canvasWidget();
    foreach ( EmbeddedObject* object, d->workbook->doc()->embeddedObjects() )
    {
        if ( object->isSelected() && !object->isProtect()) {

            KoRect geometry = object->geometry();
            geometry.moveBy( -canvas->xOffset(), -canvas->yOffset() );
            QRect oldBoundingRect = doc()->zoomRectOld( geometry );


            KoRect r = object->geometry();
            r.moveBy( _move.x(), _move.y() );

            object->setGeometry( r );
            _objects.append( object );

            canvas->repaint( oldBoundingRect );
            canvas->repaintObject( object );
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
bool Sheet::objectNameExists( EmbeddedObject *object, QList<EmbeddedObject*> &list )
{
    foreach ( EmbeddedObject* obj, list ) {
        // object name can exist in current object.
        if ( obj->getObjectName() == object->getObjectName() &&
             obj != object ) {
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

    QList<EmbeddedObject*> list( doc()->embeddedObjects() );

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

bool Sheet::loadSheetStyleFormat( KoXmlElement *style )
{
    QString hleft, hmiddle, hright;
    QString fleft, fmiddle, fright;
    KoXmlNode header = KoDom::namedItemNS( *style, KoXmlNS::style, "header" );

    if ( !header.isNull() )
    {
        kDebug() << "Header exists" << endl;
        KoXmlNode part = KoDom::namedItemNS( header, KoXmlNS::style, "region-left" );
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
    KoXmlNode headerleft = KoDom::namedItemNS( *style, KoXmlNS::style, "header-left" );
    if ( !headerleft.isNull() )
    {
        KoXmlElement e = headerleft.toElement();
        if ( e.hasAttributeNS( KoXmlNS::style, "display" ) )
            kDebug()<<"header.hasAttribute( style:display ) :"<<e.hasAttributeNS( KoXmlNS::style, "display" )<<endl;
        else
            kDebug()<<"header left doesn't has attribute  style:display  \n";
    }
    //TODO implement it under kspread
    KoXmlNode footerleft = KoDom::namedItemNS( *style, KoXmlNS::style, "footer-left" );
    if ( !footerleft.isNull() )
    {
        KoXmlElement e = footerleft.toElement();
        if ( e.hasAttributeNS( KoXmlNS::style, "display" ) )
            kDebug()<<"footer.hasAttribute( style:display ) :"<<e.hasAttributeNS( KoXmlNS::style, "display" )<<endl;
        else
            kDebug()<<"footer left doesn't has attribute  style:display  \n";
    }

    KoXmlNode footer = KoDom::namedItemNS( *style, KoXmlNS::style, "footer" );

    if ( !footer.isNull() )
    {
        KoXmlNode part = KoDom::namedItemNS( footer, KoXmlNS::style, "region-left" );
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


QString Sheet::getPart( const KoXmlNode & part )
{
  QString result;
  KoXmlElement e = KoDom::namedItemNS( part, KoXmlNS::text, "p" );
  while ( !e.isNull() )
  {
    QString text = e.text();
    kDebug() << "PART: " << text << endl;

    KoXmlElement macro = KoDom::namedItemNS( e, KoXmlNS::text, "time" );
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


bool Sheet::loadOasis( const KoXmlElement& sheetElement,
                       KoOasisLoadingContext& oasisContext,
                       const Styles& styleMap )
{
    setLayoutDirection (LeftToRight);
    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
        QString stylename = sheetElement.attributeNS( KoXmlNS::table, "style-name", QString::null );
        //kDebug()<<" style of table :"<<stylename<<endl;
        const KoXmlElement *style = oasisContext.oasisStyles().findStyle( stylename, "table" );
        Q_ASSERT( style );
        //kDebug()<<" style :"<<style<<endl;
        if ( style )
        {
            KoXmlElement properties( KoDom::namedItemNS( *style, KoXmlNS::style, "table-properties" ) );
            if ( !properties.isNull() )
            {
                if ( properties.hasAttributeNS( KoXmlNS::table, "display" ) )
                {
                    bool visible = (properties.attributeNS( KoXmlNS::table, "display", QString::null ) == "true" ? true : false );
                    setHidden (!visible);
                }
            }
            if ( style->hasAttributeNS( KoXmlNS::style, "master-page-name" ) )
            {
                QString masterPageStyleName = style->attributeNS( KoXmlNS::style, "master-page-name", QString::null );
                //kDebug()<<"style->attribute( style:master-page-name ) :"<<masterPageStyleName <<endl;
                KoXmlElement *masterStyle = oasisContext.oasisStyles().masterPages()[masterPageStyleName];
                //kDebug()<<"oasisStyles.styles()[masterPageStyleName] :"<<masterStyle<<endl;
                if ( masterStyle )
                {
                    loadSheetStyleFormat( masterStyle );
                    if ( masterStyle->hasAttributeNS( KoXmlNS::style, "page-layout-name" ) )
                    {
                        QString masterPageLayoutStyleName = masterStyle->attributeNS( KoXmlNS::style, "page-layout-name", QString::null );
                        //kDebug()<<"masterPageLayoutStyleName :"<<masterPageLayoutStyleName<<endl;
                        const KoXmlElement *masterLayoutStyle = oasisContext.oasisStyles().findStyle( masterPageLayoutStyleName );
                      if ( masterLayoutStyle )
                      {
                        //kDebug()<<"masterLayoutStyle :"<<masterLayoutStyle<<endl;
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

    const int overallRowCount = workbook()->overallRowCount();
    int rowIndex = 1;
    int indexCol = 1;
    KoXmlNode rowNode = sheetElement.firstChild();
    // Some spreadsheet programs may support more rows than
    // KSpread so limit the number of repeated rows.
    // FIXME POSSIBLE DATA LOSS!
    while( !rowNode.isNull() && rowIndex <= KS_rowMax )
    {
        kDebug()<<" rowIndex :"<<rowIndex<<" indexCol :"<<indexCol<<endl;
        KoXmlElement rowElement = rowNode.toElement();
        if( !rowElement.isNull() )
        {
            kDebug()<<" Sheet::loadOasis rowElement.tagName() :"<<rowElement.localName()<<endl;
            if ( rowElement.namespaceURI() == KoXmlNS::table )
            {
                if ( rowElement.localName()=="table-column" && indexCol <= KS_colMax )
                {
                    kDebug ()<<" table-column found : index column before "<< indexCol<<endl;
                    loadColumnFormat( rowElement, oasisContext.oasisStyles(), indexCol , styleMap);
                    kDebug ()<<" table-column found : index column after "<< indexCol<<endl;
                }
                else if ( rowElement.localName() == "table-header-rows" )
                {
                  KoXmlNode headerRowNode = rowElement.firstChild();
                  while ( !headerRowNode.isNull() )
                  {
                    // NOTE Handle header rows as ordinary ones
                    //      as long as they're not supported.
                    loadRowFormat( headerRowNode.toElement(), rowIndex,
                                   oasisContext, /*rowNode.isNull() ,*/ styleMap );
                    headerRowNode = headerRowNode.nextSibling();
                  }
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
#ifdef KOXML_USE_QDOM
        doc()->emitProgress( 100 * rowIndex / overallRowCount );
#else
#warning Problem with KoXmlReader conversion!
        kWarning() << "Problem with KoXmlReader conversion!" << endl;
#endif
    }

    if ( sheetElement.hasAttributeNS( KoXmlNS::table, "print-ranges" ) )
    {
        // e.g.: Sheet4.A1:Sheet4.E28
        QString range = sheetElement.attributeNS( KoXmlNS::table, "print-ranges", QString::null );
        range = Oasis::decodeFormula( range );
        Range p( range );
        if ( sheetName() == p.sheetName() )
          print()->setPrintRange( p.range() );
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
        kDebug(30518) << "Password hash: '" << passwd << '\'' << endl;
        setProtected (passwd);
    }
    return true;
}


void Sheet::loadOasisObjects( const KoXmlElement &parent, KoOasisLoadingContext& oasisContext )
{
    KoXmlElement e;
    KoXmlNode n = parent.firstChild();
    while( !n.isNull() )
    {
        e = n.toElement();
        if ( e.localName() == "frame" && e.namespaceURI() == KoXmlNS::draw )
        {
          EmbeddedObject *obj = 0;
          KoXmlNode object = KoDom::namedItemNS( e, KoXmlNS::draw, "object" );
          if ( !object.isNull() )
          {
            if ( !object.toElement().attributeNS( KoXmlNS::draw, "notify-on-update-of-ranges", QString::null).isNull() )
                obj = new EmbeddedChart( doc(), this );
            else
                obj = new EmbeddedKOfficeObject( doc(), this );
          }
          else
          {
            KoXmlNode image = KoDom::namedItemNS( e, KoXmlNS::draw, "image" );
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
        setLayoutDirection (( styleStack.attributeNS( KoXmlNS::style, "writing-mode" )=="lr-tb" ) ? LeftToRight : RightToLeft);
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
          print()->setPrintGrid( true );
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
    print()->setPaperLayout( left, top, right, bottom, format, orientation );

    kDebug()<<" left margin :"<<left<<" right :"<<right<<" top :"<<top<<" bottom :"<<bottom<<endl;
//<style:properties fo:page-width="21.8cm" fo:page-height="28.801cm" fo:margin-top="2cm" fo:margin-bottom="2.799cm" fo:margin-left="1.3cm" fo:margin-right="1.3cm" style:writing-mode="lr-tb"/>
//          QString format = paper.attribute( "format" );
//      QString orientation = paper.attribute( "orientation" );
//        d->print->setPaperLayout( left, top, right, bottom, format, orientation );
//      }
}


bool Sheet::loadColumnFormat(const KoXmlElement& column,
                             const KoOasisStyles& oasisStyles, int & indexCol,
                             const Styles& styleMap)
{
  Q_UNUSED(styleMap)
//   kDebug()<<"bool Sheet::loadColumnFormat(const KoXmlElement& column, const KoOasisStyles& oasisStyles, unsigned int & indexCol ) index Col :"<<indexCol<<endl;

    bool isNonDefaultColumn = false;

    int number = 1;
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

    Format layout( this , doc()->styleManager()->defaultStyle() );
    if ( column.hasAttributeNS( KoXmlNS::table, "default-cell-style-name" ) )
    {
      const QString styleName = column.attributeNS( KoXmlNS::table, "default-cell-style-name", QString::null );
      if ( !styleName.isEmpty() )
      {
        Style* const style = styleMap[ styleName ];
        if ( style )
        {
          layout.setStyle( style );
          isNonDefaultColumn = true;
        }
      }
    }

    bool collapsed = false;
    if ( column.hasAttributeNS( KoXmlNS::table, "visibility" ) )
    {
      const QString visibility = column.attributeNS( KoXmlNS::table, "visibility", QString::null );
      if ( visibility == "visible" )
        collapsed = false;
      else if ( visibility == "collapse" )
        collapsed = true;
      else if ( visibility == "filter" )
        collapsed = false; // FIXME Stefan: Set to true, if filters are supported.
      isNonDefaultColumn = true;
    }

    KoStyleStack styleStack;
    if ( column.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
      QString str = column.attributeNS( KoXmlNS::table, "style-name", QString::null );
      const KoXmlElement *style = oasisStyles.findStyle( str, "table-column" );
      if ( style )
      {
        styleStack.push( *style );
        isNonDefaultColumn = true;
      }
    }
    styleStack.setTypeProperties("table-column"); //style for column

    double width = -1.0;
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "column-width" ) )
    {
        width = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::style, "column-width" ) , -1.0 );
        kDebug()<<" style:column-width : width :"<<width<<endl;
        isNonDefaultColumn = true;
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
        isNonDefaultColumn = true;
    }

    for ( int i = 0; i < number; ++i )
    {
        kDebug()<<" insert new column: pos :"<<indexCol<<" width :"<<width<<" hidden ? "<<collapsed<<endl;

        ColumnFormat* columnFormat;
        if ( isNonDefaultColumn )
        {
          columnFormat = nonDefaultColumnFormat( indexCol );

          if ( width != -1.0 ) //safe
            columnFormat->setWidth( (int) width );
        // if ( insertPageBreak )
        //   columnFormat->setPageBreak( true )
          if ( collapsed )
            columnFormat->setHide( true );
        }
        else
        {
          columnFormat = this->columnFormat( indexCol );
        }
        columnFormat->copy( layout );

        ++indexCol;
    }
//     kDebug()<<" after index column !!!!!!!!!!!!!!!!!! :"<<indexCol<<endl;
    return true;
}


bool Sheet::loadRowFormat( const KoXmlElement& row, int &rowIndex,
                           KoOasisLoadingContext& oasisContext,
                           const Styles& styleMap )
{
//    kDebug()<<"Sheet::loadRowFormat( const KoXmlElement& row, int &rowIndex,const KoOasisStyles& oasisStyles, bool isLast )***********\n";

    int backupRow = rowIndex;
    bool isNonDefaultRow = false;

    KoStyleStack styleStack;
    if ( row.hasAttributeNS( KoXmlNS::table, "style-name" ) )
    {
      QString str = row.attributeNS( KoXmlNS::table, "style-name", QString::null );
      const KoXmlElement *style = oasisContext.oasisStyles().findStyle( str, "table-row" );
      if ( style )
      {
        styleStack.push( *style );
        isNonDefaultRow = true;
      }
    }
    styleStack.setTypeProperties( "table-row" );

    Format layout( this , doc()->styleManager()->defaultStyle() );
    if ( row.hasAttributeNS( KoXmlNS::table,"default-cell-style-name" ) )
    {
      const QString styleName = row.attributeNS( KoXmlNS::table, "default-cell-style-name", QString::null );
      if ( !styleName.isEmpty() )
      {
        Style* const style = styleMap[ styleName ];
        if ( style )
        {
          layout.setStyle( style );
          isNonDefaultRow = true;
        }
      }
    }

    double height = -1.0;
    if ( styleStack.hasAttributeNS( KoXmlNS::style, "row-height" ) )
    {
        height = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::style, "row-height" ) , -1.0 );
    //    kDebug()<<" properties style:row-height : height :"<<height<<endl;
        isNonDefaultRow = true;
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
      const QString visibility = row.attributeNS( KoXmlNS::table, "visibility", QString::null );
      if ( visibility == "visible" )
        collapse = false;
      else if ( visibility == "collapse" )
        collapse = true;
      else if ( visibility == "filter" )
        collapse = false; // FIXME Stefan: Set to true, if filters are supported.
      isNonDefaultRow = true;
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
        isNonDefaultRow = true;
    }

    //number == number of row to be copy. But we must copy cell too.
    for ( int i = 0; i < number; ++i )
    {
       // kDebug()<<" create non defaultrow format :"<<rowIndex<<" repeate : "<<number<<" height :"<<height<<endl;

      RowFormat* rowFormat;
      if ( isNonDefaultRow )
      {
        rowFormat = nonDefaultRowFormat( rowIndex );

        if ( height != -1.0 )
          rowFormat->setHeight( (int) height );
        if ( collapse )
          rowFormat->setHide( true );
      }
      else
      {
        rowFormat = this->rowFormat( rowIndex );
      }
      rowFormat->copy( layout );

      ++rowIndex;
    }

    int columnIndex = 0;
    KoXmlNode cellNode = row.firstChild();
    int endRow = qMin(backupRow+number,KS_rowMax);


    while ( !cellNode.isNull() )
    {
        KoXmlElement cellElement = cellNode.toElement();
        if ( !cellElement.isNull() )
        {
            columnIndex++;
            QString localName = cellElement.localName();

            if ( ((localName == "table-cell") || (localName == "covered-table-cell")) && cellElement.namespaceURI() == KoXmlNS::table)
            {
                //kDebug() << "Loading cell #" << cellCount << endl;

                Style* style = 0;
                const bool cellHasStyle = cellElement.hasAttributeNS( KoXmlNS::table, "style-name" );
                if ( cellHasStyle )
                {
                    style = styleMap[ cellElement.attributeNS( KoXmlNS::table , "style-name" , QString::null ) ];
                }

                Cell* const cell = nonDefaultCell( columnIndex, backupRow ); // FIXME Stefan: if empty, delete afterwards
                cell->loadOasis( cellElement, oasisContext, style );

                int cols = 1;

                // Copy this cell across & down, if it has repeated rows or columns, but only
                // if the cell has some content or a style associated with it.
                if ( (number > 1) || cellElement.hasAttributeNS( KoXmlNS::table, "number-columns-repeated" ) )
                {
                    bool ok = false;
                    int n = cellElement.attributeNS( KoXmlNS::table, "number-columns-repeated", QString::null ).toInt( &ok );

                    if (ok)
                        // Some spreadsheet programs may support more columns than
                        // KSpread so limit the number of repeated columns.
                        // FIXME POSSIBLE DATA LOSS!
                        cols = qMin( n, KS_colMax - columnIndex + 1 );

                    if ( !cellHasStyle && ( cell->isEmpty() && cell->format()->comment( columnIndex, backupRow ).isEmpty() ) )
                    {
                        // just increment it
                        columnIndex += cols - 1;
                    }
                    else
                    {
                        for ( int k = cols ; k ; --k )
                        {
                            if ( k != cols )
                                columnIndex++;

                            for ( int newRow = backupRow; newRow < endRow; ++newRow )
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


bool Sheet::compareRows( int row1, int row2, int& maxCols ) const
{
  if ( *rowFormat( row1 ) != *rowFormat( row2 ) )
  {
//     kDebug() << "\t Formats of " << row1 << " and " << row2 << " are different" << endl;
    return false;
  }
  // FIXME Stefan: Make use of the cluster functionality.
  for ( int col = 1; col <= maxCols; ++col )
  {
    if ( *cellAt( col, row1 ) != *cellAt( col, row2 ) )
    {
//       kDebug() << "\t Cell at column " << col << " in row " << row2 << " differs from the one in row " << row1 << endl;
      return false;
    }
  }
  return true;
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
       xmlWriter.endElement();

       xmlWriter.startElement( "text:page-number" );
       xmlWriter.addTextNode( "1" ); // ???
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
                    KoXmlElement t = dd.createElement( "text:date" );
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
                    Doc* sdoc = workbook()->doc();
                    KoDocumentInfo* docInfo = sdoc->documentInfo();

                    text += docInfo->authorInfo( "creator" );
                    addText( text, xmlWriter );
                }
                else if ( var == "<email>" )
                {
                    Doc* sdoc = workbook()->doc();
                    KoDocumentInfo* docInfo = sdoc->documentInfo();

                    text += docInfo->authorInfo( "email" );
                    addText( text, xmlWriter );

                }
                else if ( var == "<org>" )
                {
                    Doc* sdoc = workbook()->doc();
                    KoDocumentInfo* docInfo    = sdoc->documentInfo();

                    text += docInfo->authorInfo( "company" );
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
    KoOasisSettings::Items items = settings.entry( sheetName() );
    if ( items.isNull() )
        return;
    setHideZero (items.parseConfigItemBool( "ShowZeroValues" ));
    setShowGrid (items.parseConfigItemBool( "ShowGrid" ));
    setFirstLetterUpper (items.parseConfigItemBool( "FirstLetterUpper" ));

    int cursorX = items.parseConfigItemInt( "CursorPositionX" );
    int cursorY = items.parseConfigItemInt( "CursorPositionY" );
    doc()->loadingInfo()->setCursorPosition( this, QPoint( cursorX, cursorY ) );

    double offsetX = items.parseConfigItemDouble( "xOffset" );
    double offsetY = items.parseConfigItemDouble( "yOffset" );
    doc()->loadingInfo()->setScrollingOffset( this, KoPoint( offsetX, offsetY ) );

    setShowFormulaIndicator (items.parseConfigItemBool( "ShowFormulaIndicator" ));
    setShowCommentIndicator (items.parseConfigItemBool( "ShowCommentIndicator" ));
    setShowPageBorders (items.parseConfigItemBool( "ShowPageBorders" ));
    setLcMode (items.parseConfigItemBool( "lcmode" ));
    setAutoCalc (items.parseConfigItemBool( "autoCalc" ));
    setShowColumnNumber (items.parseConfigItemBool( "ShowColumnNumber" ));
}

void Sheet::saveOasisSettings( KoXmlWriter &settingsWriter ) const
{
  //not into each page into oo spec
  settingsWriter.addConfigItem( "ShowZeroValues", getHideZero() );
  settingsWriter.addConfigItem( "ShowGrid", getShowGrid() );
  //not define into oo spec
  settingsWriter.addConfigItem( "FirstLetterUpper", getFirstLetterUpper());
  settingsWriter.addConfigItem( "ShowFormulaIndicator", getShowFormulaIndicator() );
  settingsWriter.addConfigItem( "ShowCommentIndicator", getShowCommentIndicator() );
  settingsWriter.addConfigItem( "ShowPageBorders", isShowPageBorders() );
  settingsWriter.addConfigItem( "lcmode", getLcMode() );
  settingsWriter.addConfigItem( "autoCalc", getAutoCalc() );
  settingsWriter.addConfigItem( "ShowColumnNumber", getShowColumnNumber() );
}

bool Sheet::saveOasis( KoXmlWriter & xmlWriter, KoGenStyles &mainStyles, GenValidationStyles &valStyle, KoStore *store, KoXmlWriter* /*manifestWriter*/, int &indexObj, int &partIndexObj )
{
    int maxCols= 1;
    int maxRows= 1;
    xmlWriter.startElement( "table:table" );
    xmlWriter.addAttribute( "table:name", sheetName() );
    xmlWriter.addAttribute( "table:style-name", saveOasisSheetStyleName(mainStyles )  );
    QByteArray pwd;
    password (pwd);
    if ( !pwd.isEmpty() )
    {
        xmlWriter.addAttribute("table:protected", "true" );
        QByteArray str = KCodecs::base64Encode( pwd );
        // FIXME Stefan: see OpenDocument spec, ch. 17.3 Encryption
        xmlWriter.addAttribute("table:protection-key", QString( str.data() ) );
    }
    QRect _printRange = print()->printRange();
    if ( _printRange != ( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) ) )
    {
        QString range= convertRangeToRef( sheetName(), _printRange );
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
    if ( print()->printGrid() )
        printParameter="grid ";
    if ( print()->printObjects() )
      printParameter+="objects ";
    if ( print()->printCharts() )
      printParameter+="charts ";
    if ( getShowFormula() )
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
    pageMaster.addAttribute( "style:page-layout-name", print()->saveOasisSheetStyleLayout( mainStyles ) );

    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
    saveOasisHeaderFooter(elementWriter);

    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    pageMaster.addChildElement( "headerfooter", elementContents );
    pageStyle.addAttribute( "style:master-page-name", mainStyles.lookup( pageMaster, "Standard" ) );

    pageStyle.addProperty( "table:display", !isHidden() );
    return mainStyles.lookup( pageStyle, "ta" );
}


void Sheet::saveOasisColRowCell( KoXmlWriter& xmlWriter, KoGenStyles &mainStyles,
                                 int maxCols, int maxRows, GenValidationStyles &valStyle )
{
    kDebug() << "Sheet::saveOasisColRowCell: " << d->name << endl;
    kDebug() << "\t Sheet dimension: " << maxCols << " x " << maxRows << endl;

    // saving the columns
    //
    int i = 1;
    while ( i <= maxCols )
    {
//         kDebug() << "Sheet::saveOasisColRowCell: first col loop:"
//                   << " i: " << i
//                   << " column: " << column->column() << endl;
        ColumnFormat* column = columnFormat( i );

        KoGenStyle currentColumnStyle( Doc::STYLE_COLUMN_AUTO, "table-column" );
        currentColumnStyle.addPropertyPt( "style:column-width", column->dblWidth() );
        currentColumnStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

        //style default layout for column
        KoGenStyle currentDefaultCellStyle; // the type is determined in saveOasisCellStyle
        QString currentDefaultCellStyleName = column->saveOasisCellStyle( currentDefaultCellStyle, mainStyles );

        bool hide = column->isHide();
        bool refColumnIsDefault = column->isDefault();
        int j = i;
        int repeated = 1;

        while ( j <= maxCols )
        {
//           kDebug() << "Sheet::saveOasisColRowCell: second col loop:"
//                     << " j: " << j
//                     << " column: " << nextColumn->column() << endl;
          ColumnFormat* nextColumn = d->columns.next( j++ );

          // no next or not the adjacent column?
          if ( !nextColumn || nextColumn->column() != j )
          {
            if ( refColumnIsDefault )
            {
              // if the origin column was a default column,
              // we count the default columns
              if ( !nextColumn )
              {
                repeated = maxCols - i + 1;
              }
              else
              {
                repeated = nextColumn->column() - j + 1;
              }
            }
            // otherwise we just stop here to process the adjacent
            // column in the next iteration of the outer loop
            break;
          }
#if 0
          KoGenStyle nextColumnStyle( Doc::STYLE_COLUMN_AUTO, "table-column" );
          nextColumnStyle.addPropertyPt( "style:column-width", nextColumn->dblWidth() );
          nextColumnStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

          KoGenStyle nextDefaultCellStyle; // the type is determined in saveOasisCellStyle
          QString nextDefaultCellStyleName = nextColumn->saveOasisCellStyle( nextDefaultCellStyle, mainStyles );

          if ( hide != nextColumn->isHide() ||
               nextDefaultCellStyleName != currentDefaultCellStyleName ||
               !( nextColumnStyle == currentColumnStyle ) )
          {
            break;
          }
#endif
          if ( *column != *nextColumn )
          {
            break;
          }
          ++repeated;
        }

        xmlWriter.startElement( "table:table-column" );
        if ( !column->isDefault() )
        {
          xmlWriter.addAttribute( "table:style-name", mainStyles.lookup( currentColumnStyle, "co" ) );

          if ( !currentDefaultCellStyle.isDefaultStyle() )
              xmlWriter.addAttribute( "table:default-cell-style-name", currentDefaultCellStyleName );

          if ( hide )
              xmlWriter.addAttribute( "table:visibility", "collapse" );
        }
        if ( repeated > 1 )
            xmlWriter.addAttribute( "table:number-columns-repeated", repeated  );

        xmlWriter.endElement();

        kDebug() << "Sheet::saveOasisColRowCell: column " << i << ' '
                  << "repeated " << repeated << " time(s)" << endl;
        i += repeated;
    }

    // saving the rows and the cells
    // we have to loop through all rows of the used area
    for ( i = 1; i <= maxRows; ++i )
    {
        RowFormat* const row = rowFormat( i );

        KoGenStyle currentRowStyle( Doc::STYLE_ROW_AUTO, "table-row" );
        currentRowStyle.addPropertyPt( "style:row-height", row->dblHeight() );
        currentRowStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

        // default cell style for row
        KoGenStyle currentDefaultCellStyle; // the type is determined in saveOasisCellStyle
        QString currentDefaultCellStyleName = row->saveOasisCellStyle( currentDefaultCellStyle, mainStyles );

        xmlWriter.startElement( "table:table-row" );

        if ( !row->isDefault() )
        {
          xmlWriter.addAttribute( "table:style-name", mainStyles.lookup( currentRowStyle, "ro" ) );
        }
        int repeated = 1;
        // empty row?
        if ( !getFirstCellRow( i ) )
        {
//               kDebug() << "Sheet::saveOasisColRowCell: first row loop:"
//                         << " i: " << i
//                         << " row: " << row->row() << endl;
            //bool isHidden = row->isHide();
            bool isDefault = row->isDefault();
            int j = i + 1;

            // search for
            //   next non-empty row
            // or
            //   next row with different Format
            while ( j <= maxRows && !getFirstCellRow( j ) )
            {
              RowFormat* const nextRow = rowFormat( j );
//               kDebug() << "Sheet::saveOasisColRowCell: second row loop:"
//                         << " j: " << j
//                         << " row: " << nextRow->row() << endl;

              // if the reference row has the default row format
              if ( isDefault )
              {
                // if the next is not default, stop here
                if ( !nextRow->isDefault() )
                  break;
                // otherwise, jump to the next
                ++j;
                continue;
              }
#if 0
              // create the Oasis representation of the format for the comparison
              KoGenStyle nextRowStyle( Doc::STYLE_ROW_AUTO, "table-row" );
              nextRowStyle.addPropertyPt( "style:row-height", nextRow->dblHeight() );
              nextRowStyle.addProperty( "fo:break-before", "auto" );/*FIXME auto or not ?*/

              // default cell style name for next row
              KoGenStyle nextDefaultCellStyle; // the type is determined in saveOasisCellStyle
              QString nextDefaultCellStyleName = nextRow->saveOasisCellStyle( nextDefaultCellStyle, mainStyles );

              // if the formats differ, stop here
              if ( isHidden != nextRow->isHide() ||
                   nextDefaultCellStyleName != currentDefaultCellStyleName ||
                   !(nextRowStyle == currentRowStyle) )
              {
                break;
              }
#endif
              if ( *row != *nextRow )
              {
                break;
              }
              // otherwise, process the next
              ++j;
            }
            repeated = j - i;

            if ( repeated > 1 )
                xmlWriter.addAttribute( "table:number-rows-repeated", repeated  );
            if ( !currentDefaultCellStyle.isDefaultStyle() )
              xmlWriter.addAttribute( "table:default-cell-style-name", currentDefaultCellStyleName );
            if ( row->isHide() ) // never true for the default row
              xmlWriter.addAttribute( "table:visibility", "collapse" );

            // NOTE Stefan: Even if paragraph 8.1 states, that rows may be empty, the
            //              RelaxNG schema does not allow that.
            xmlWriter.startElement( "table:table-cell" );
            xmlWriter.endElement();

            kDebug() << "Sheet::saveOasisColRowCell: empty row " << i << ' '
                      << "repeated " << repeated << " time(s)" << endl;

            // copy the index for the next row to process
            i = j - 1; /*it's already incremented in the for loop*/
        }
        else // row is not empty
        {
            if ( !currentDefaultCellStyle.isDefaultStyle() )
              xmlWriter.addAttribute( "table:default-cell-style-name", currentDefaultCellStyleName );
            if ( row->isHide() ) // never true for the default row
              xmlWriter.addAttribute( "table:visibility", "collapse" );

            int j = i + 1;
            while ( compareRows( i, j, maxCols ) && j <= maxRows )
            {
              j++;
              repeated++;
            }
            if ( repeated > 1 )
            {
              kDebug() << "Sheet::saveOasisColRowCell: NON-empty row " << i << ' '
                        << "repeated " << repeated << " times" << endl;

              xmlWriter.addAttribute( "table:number-rows-repeated", repeated  );
            }

            saveOasisCells( xmlWriter, mainStyles, i, maxCols, valStyle );

            // copy the index for the next row to process
            i = j - 1; /*it's already incremented in the for loop*/
        }
        xmlWriter.endElement();
    }
}

void Sheet::saveOasisCells( KoXmlWriter& xmlWriter, KoGenStyles &mainStyles, int row, int maxCols, GenValidationStyles &valStyle )
{
    int i = 1;
    Cell* cell = cellAt( i, row );
    Cell* nextCell = getNextCellRight( i, row );
    // while
    //   the current cell is not a default one
    // or
    //   we have a further cell in this row
    while ( !cell->isDefault() || nextCell )
    {
//         kDebug() << "Sheet::saveOasisCells:"
//                   << " i: " << i
//                   << " column: " << (cell->isDefault() ? 0 : cell->column()) << endl;
        int repeated = 1;
        cell->saveOasis( xmlWriter, mainStyles, row, i, repeated, valStyle );
        i += repeated;
        // stop if we reached the end column
        if ( i > maxCols )
          break;
        cell = cellAt( i, row );
        nextCell = getNextCellRight( i, row );
    }
}

bool Sheet::loadXML( const KoXmlElement& sheet )
{
    bool ok = false;
    QString sname = sheetName();
    if ( !doc()->loadingInfo() || !doc()->loadingInfo()->loadTemplate() )
    {
        sname = sheet.attribute( "name" );
        if ( sname.isEmpty() )
        {
            doc()->setErrorMessage( i18n("Invalid document. Sheet name is empty.") );
            return false;
        }
    }

    bool detectDirection = true;
    setLayoutDirection (LeftToRight);
    QString layoutDir = sheet.attribute( "layoutDirection" );
    if( !layoutDir.isEmpty() )
    {
        if( layoutDir == "rtl" )
        {
           detectDirection = false;
           setLayoutDirection (RightToLeft);
        }
        else if( layoutDir == "ltr" )
        {
           detectDirection = false;
           setLayoutDirection (LeftToRight);
        }
        else
            kDebug()<<" Direction not implemented : "<<layoutDir<<endl;
    }
    if( detectDirection )
      checkContentDirection( sname );

    /* older versions of KSpread allowed all sorts of characters that
       the parser won't actually understand.  Replace these with '_'
       Also, the initial character cannot be a space.
    */
    while (sname[0] == ' ')
    {
      sname.remove(0,1);
    }
    for (int i=0; i < sname.length(); i++)
    {
      if ( !(sname[i].isLetterOrNumber() ||
             sname[i] == ' ' || sname[i] == '.' || sname[i] == '_') )
      {
        sname[i] = '_';
      }
    }

    // validate sheet name, if it differs from the current one
    if ( sname != sheetName() )
    {
        /* make sure there are no name collisions with the altered name */
        QString testName = sname;
        QString baseName = sname;
        int nameSuffix = 0;

        /* so we don't panic over finding ourself in the follwing test*/
        sname.clear();
        while (workbook()->findSheet(testName) != 0)
        {
            nameSuffix++;
            testName = baseName + '_' + QString::number(nameSuffix);
        }
        sname = testName;

        kDebug(36001) << "Sheet::loadXML: table name = " << sname << endl;
        setObjectName(sname.toUtf8());
        setSheetName (sname, true);
    }

//     (dynamic_cast<SheetIface*>(dcopObject()))->sheetNameHasChanged();

    if( sheet.hasAttribute( "grid" ) )
    {
        setShowGrid ((int)sheet.attribute("grid").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "printGrid" ) )
    {
        print()->setPrintGrid( (bool)sheet.attribute("printGrid").toInt( &ok ) );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "printCommentIndicator" ) )
    {
        print()->setPrintCommentIndicator( (bool)sheet.attribute("printCommentIndicator").toInt( &ok ) );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "printFormulaIndicator" ) )
    {
        print()->setPrintFormulaIndicator( (bool)sheet.attribute("printFormulaIndicator").toInt( &ok ) );
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "hide" ) )
    {
        setHidden ( (bool)sheet.attribute("hide").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "showFormula" ) )
    {
        setShowFormula ((bool)sheet.attribute("showFormula").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    //Compatibility with KSpread 1.1.x
    if( sheet.hasAttribute( "formular" ) )
    {
        setShowFormula ((bool)sheet.attribute("formular").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "showFormulaIndicator" ) )
    {
        setShowFormulaIndicator ( (bool)sheet.attribute("showFormulaIndicator").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "showCommentIndicator" ) )
    {
        setShowCommentIndicator ( (bool)sheet.attribute("showCommentIndicator").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "borders" ) )
    {
        setShowPageBorders ((bool)sheet.attribute("borders").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "lcmode" ) )
    {
        setLcMode ((bool)sheet.attribute("lcmode").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if ( sheet.hasAttribute( "autoCalc" ) )
    {
        setAutoCalc (( bool )sheet.attribute( "autoCalc" ).toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "columnnumber" ) )
    {
        setShowColumnNumber ((bool)sheet.attribute("columnnumber").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "hidezero" ) )
    {
        setHideZero ((bool)sheet.attribute("hidezero").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if( sheet.hasAttribute( "firstletterupper" ) )
    {
        setFirstLetterUpper ((bool)sheet.attribute("firstletterupper").toInt( &ok ));
        // we just ignore 'ok' - if it didn't work, go on
    }

    // Load the paper layout
    KoXmlElement paper = sheet.namedItem( "paper" ).toElement();
    if ( !paper.isNull() )
    {
      QString format = paper.attribute( "format" );
      QString orientation = paper.attribute( "orientation" );

      // <borders>
      KoXmlElement borders = paper.namedItem( "borders" ).toElement();
      if ( !borders.isNull() )
      {
        float left = borders.attribute( "left" ).toFloat();
        float right = borders.attribute( "right" ).toFloat();
        float top = borders.attribute( "top" ).toFloat();
        float bottom = borders.attribute( "bottom" ).toFloat();
        print()->setPaperLayout( left, top, right, bottom, format, orientation );
      }
      QString hleft, hright, hcenter;
      QString fleft, fright, fcenter;
      // <head>
      KoXmlElement head = paper.namedItem( "head" ).toElement();
      if ( !head.isNull() )
      {
        KoXmlElement left = head.namedItem( "left" ).toElement();
        if ( !left.isNull() )
          hleft = left.text();
        KoXmlElement center = head.namedItem( "center" ).toElement();
        if ( !center.isNull() )
        hcenter = center.text();
        KoXmlElement right = head.namedItem( "right" ).toElement();
        if ( !right.isNull() )
          hright = right.text();
      }
      // <foot>
      KoXmlElement foot = paper.namedItem( "foot" ).toElement();
      if ( !foot.isNull() )
      {
        KoXmlElement left = foot.namedItem( "left" ).toElement();
        if ( !left.isNull() )
          fleft = left.text();
        KoXmlElement center = foot.namedItem( "center" ).toElement();
        if ( !center.isNull() )
          fcenter = center.text();
        KoXmlElement right = foot.namedItem( "right" ).toElement();
        if ( !right.isNull() )
          fright = right.text();
      }
      print()->setHeadFootLine( hleft, hcenter, hright, fleft, fcenter, fright);
    }

      // load print range
      KoXmlElement printrange = sheet.namedItem( "printrange-rect" ).toElement();
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
        print()->setPrintRange( QRect( QPoint( left, top ), QPoint( right, bottom ) ) );
      }

      // load print zoom
      if( sheet.hasAttribute( "printZoom" ) )
      {
        double zoom = sheet.attribute( "printZoom" ).toDouble( &ok );
        if ( ok )
        {
          print()->setZoom( zoom );
        }
      }

      // load page limits
      if( sheet.hasAttribute( "printPageLimitX" ) )
      {
        int pageLimit = sheet.attribute( "printPageLimitX" ).toInt( &ok );
        if ( ok )
        {
          print()->setPageLimitX( pageLimit );
        }
      }

      // load page limits
      if( sheet.hasAttribute( "printPageLimitY" ) )
      {
        int pageLimit = sheet.attribute( "printPageLimitY" ).toInt( &ok );
        if ( ok )
        {
          print()->setPageLimitY( pageLimit );
        }
      }

    // Load the cells
    KoXmlNode n = sheet.firstChild();
    while( !n.isNull() )
    {
        KoXmlElement e = n.toElement();
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
    KoXmlElement printrepeatcolumns = sheet.namedItem( "printrepeatcolumns" ).toElement();
    if ( !printrepeatcolumns.isNull() )
    {
        int left = printrepeatcolumns.attribute( "left" ).toInt();
        int right = printrepeatcolumns.attribute( "right" ).toInt();
        print()->setPrintRepeatColumns( qMakePair( left, right ) );
    }

    // load print repeat rows
    KoXmlElement printrepeatrows = sheet.namedItem( "printrepeatrows" ).toElement();
    if ( !printrepeatrows.isNull() )
    {
        int top = printrepeatrows.attribute( "top" ).toInt();
        int bottom = printrepeatrows.attribute( "bottom" ).toInt();
        print()->setPrintRepeatRows( qMakePair( top, bottom ) );
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
        setProtected (KCodecs::base64Decode( str ));
      }
      else
        setProtected (QByteArray( "" ));
    }

    return true;
}


bool Sheet::loadChildren( KoStore* _store )
{
    foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
    {
        if ( object->sheet() == this && ( object->getType() == OBJECT_KOFFICE_PART || object->getType() == OBJECT_CHART ) )
        {
            kDebug() << "KSpreadSheet::loadChildren" << endl;
            if ( !dynamic_cast<EmbeddedKOfficeObject*>( object )->embeddedObject()->loadDocument( _store ) )
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

void Sheet::addCellBinding( CellBinding* bind )
{
  d->cellBindings.append( bind );
  doc()->setModified( true );
}

void Sheet::removeCellBinding( CellBinding* bind )
{
  d->cellBindings.removeAll( bind );
  doc()->setModified( true );
}

const QList<CellBinding*>& Sheet::cellBindings() const
{
  return d->cellBindings;
}

Sheet* Sheet::findSheet( const QString & _name )
{
  if ( !workbook() )
    return 0;

  return workbook()->findSheet( _name );
}

void Sheet::insertCell( Cell *_cell )
{
  d->cells.insert( _cell, _cell->column(), _cell->row() );

  // TODO Stefan: use a SheetDamage
  //              124806: creating series takes extremely long time
  // Adjust the scrollbar range, if the max. dimension has changed.
  if ( d->scrollBarUpdates )
  {
    checkRangeHBorder( _cell->column() );
    checkRangeVBorder( _cell->row() );
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
    Q_UNUSED(_format);
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
}

void Sheet::emit_updateColumn( ColumnFormat *_format, int _column )
{
    Q_UNUSED(_format);
    if ( doc()->isLoading() )
        return;

    Cell* c = d->cells.firstCell();
    for( ;c; c = c->nextCell() )
        if ( c->column() == _column )
            c->setLayoutDirtyFlag( true );

    //All the cells in this column or to the right of it will need to be repainted if the column
    //has been resized or hidden, so add that region of the sheet to the paint dirty list.
    setRegionPaintDirty( QRect( _column , 1 , KS_colMax , KS_rowMax) );

    emit sig_updateHBorder( this );
    emit sig_updateView( this );
    emit sig_maxColumn( maxColumn() );
}

bool Sheet::insertChart( const KoRect& rect, KoDocumentEntry& documentEntry,
                         const QRect& dataArea, QWidget* parentWidget )
{
    QString errorMsg; // TODO MESSAGE
    KoDocument* document = documentEntry.createDoc( &errorMsg, doc() );
    if ( !document )
    {
        kDebug() << "Error inserting chart!" << endl;
        return false;
    }

    if ( !document->showEmbedInitDialog( parentWidget ) )
        return false;

    EmbeddedChart * ch = new EmbeddedChart( doc(), this, document, rect );
    ch->setDataArea( dataArea );
    ch->update();
    ch->chart()->setCanChangeValue( false  );

    KoChart::WizardExtension * wiz = ch->chart()->wizardExtension();

    Range dataRange;
    dataRange.setRange( dataArea );
    dataRange.setSheet( this );

    QString rangeString = dataRange.toString();

    if ( wiz )
        wiz->show( rangeString );

    insertObject( ch );

    return true;
}

bool Sheet::insertChild( const KoRect& rect, KoDocumentEntry& documentEntry,
                         QWidget* parentWidget )
{
    QString errorMsg; // TODO MESSAGE
    KoDocument* document = documentEntry.createDoc( &errorMsg, doc() );
    if ( !document )
    {
        kDebug() << "Error inserting child!" << endl;
        return false;
    }
    if ( !document->showEmbedInitDialog( parentWidget ) )
        return false;

    EmbeddedKOfficeObject* ch = new EmbeddedKOfficeObject( doc(), this, document, rect );
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

    foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
    {
        if ( object->sheet() == this && ( object->getType() == OBJECT_KOFFICE_PART || object->getType() == OBJECT_CHART ) )
        {
            QString path = QString( "%1/%2" ).arg( _path ).arg( i++ );
            if ( !dynamic_cast<EmbeddedKOfficeObject*>( object )->embeddedObject()->document()->saveToStore( _store, path ) )
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
  foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
  {
    if ( object->sheet() == this && ( doc()->savingWholeDocument() || object->isSelected() ) )
    {
      if ( !objectFound )
      {
        xmlWriter.startElement( "table:shapes" );
        objectFound = true;
      }
      if ( !object->saveOasisObject(sc)  )
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
      s_id=0;

    Cell* c = d->cells.firstCell();
    for( ; c; c = c->nextCell() )
        c->sheetDies();

    d->cells.clear(); // cells destructor needs sheet to still exist

    delete d->defaultFormat;
    delete d->defaultCell;
    delete d->defaultRowFormat;
    delete d->defaultColumnFormat;
    delete d->print;

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

    foreach ( Sheet* sheet, workbook()->sheetList() )
    {
      sheet->changeCellTabName( old_name, name );
    }

    doc()->changeAreaSheetName( old_name, name );
    emit sig_nameChanged( this, old_name );

    setObjectName(name.toUtf8());
//     (dynamic_cast<SheetIface*>(dcopObject()))->sheetNameHasChanged();

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

void Sheet::setRegionPaintDirty( const Region & region )
{
  if ( isLoading() )
    return;

  d->paintDirtyList.add(region);

  kDebug() << "setRegionPaintDirty "<< static_cast<const Region*>(&region)->name(this) << endl;
}

void Sheet::clearPaintDirtyData()
{
  d->paintDirtyList.clear();
}

const Region& Sheet::paintDirtyData() const
{
  return d->paintDirtyList;
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

