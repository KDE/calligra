/* This file is part of the KDE project
   Copyright 2005 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "inspector.h"

#include <qlayout.h>
#include <qlistview.h>
#include <qtextstream.h>

#include <kdialogbase.h>

#include "kspread_cell.h"
#include "kspread_style.h"
#include "kspread_sheet.h"
#include "dependencies.h"

namespace KSpread
{

class Inspector::Private
{
public:
  Cell* cell;
  Format* format;
  Sheet* sheet;
  
  QListView *cellView;
  QListView *formatView;
  QListView *sheetView;
  QListView* depView;
  
  void handleCell();
  void handleFormat();
  void handleSheet();
  void handleDep();
};

}

using namespace KSpread;

static QString boolAsString( bool b )
{
  if( b ) return QString( "True" );
  else return QString( "False" );
}

static QString longAsHexstring( long l )
{
    return QString("%1").arg(l, 8, 16);
}

static QString dirAsString( Sheet::LayoutDirection dir )
{
  QString str;
  switch( dir )
  {
    case Sheet::LeftToRight: str = "Left to Right"; break;
    case Sheet::RightToLeft: str = "Right to Left"; break;
    default: str = "Unknown"; break;
  }
  return str;
}

void Inspector::Private::handleCell()
{  
  QString str;
  
  cellView->clear();
  
  new QListViewItem( cellView, "Column", QString::number( cell->column() ) );
  new QListViewItem( cellView, "Row", QString::number( cell->row() ) );
  new QListViewItem( cellView, "Name", cell->name() );
  new QListViewItem( cellView, "Full Name", cell->fullName() );

  new QListViewItem( cellView, "Default", boolAsString( cell->isDefault() ) );
  new QListViewItem( cellView, "Empty", boolAsString( cell->isEmpty() ) );
  new QListViewItem( cellView, "Formula", boolAsString( cell->isFormula() ) );
  new QListViewItem( cellView, "Format Properties", longAsHexstring( static_cast<long>( cell->format()->propertiesMask() ) ) );
  new QListViewItem( cellView, "Style Properties", longAsHexstring( static_cast<long>( cell->format()->style()->features() ) ) );
  new QListViewItem( cellView, "Text", cell->text() );
  new QListViewItem( cellView, "Text (Displayed)", 
		     cell->strOutText().replace( QChar('\n'), "\\n" ) );

  QTextStream ts( &str, IO_WriteOnly );
  ts << cell->value();
  new QListViewItem( cellView, "Value", str );

  new QListViewItem( cellView, "Link", cell->link() );

  new QListViewItem( cellView, "Width", QString::number( cell->dblWidth() ) );
  new QListViewItem( cellView, "Height", QString::number( cell->dblHeight() ) );
}

void Inspector::Private::handleFormat()
{
  formatView->clear();
  int col = cell->column();
  int row = cell->row();

  new QListViewItem( formatView, "Angle", QString::number( format->getAngle(col, row) ) );
  new QListViewItem( formatView, "Multirow", boolAsString( format->multiRow(col, row) ) );
  new QListViewItem( formatView, "Protected", format->hasProperty( Format::PVerticalText ) 
    ? "Not specified" : boolAsString( format->isProtected(col, row) ) );
  new QListViewItem( formatView, "Vertical Text", boolAsString( format->verticalText(col, row ) ) );

  Format::Currency currrency;
  bool valid = format->currencyInfo(currrency);
  new QListViewItem( formatView, "Currency symbol", valid ? currrency.symbol : "Invalid" );
  bool ok = false;
  QString currencyType;
  if (valid)
    currencyType = Currency::getChooseString(currrency.type, ok);
  new QListViewItem( formatView, "Currency type", valid && ok ? currencyType : "Invalid" );
}

void Inspector::Private::handleSheet()
{  
  sheetView->clear();
  
  new QListViewItem( sheetView, "Name", sheet->sheetName() ) ;
  new QListViewItem( sheetView, "Layout Direction", dirAsString( sheet->layoutDirection() ) );
}

void Inspector::Private::handleDep()
{  
  Point cellPoint;
  cellPoint.setSheet(sheet);
  cellPoint.setRow( cell->row() );
  cellPoint.setColumn( cell->column() );
  
  DependencyManager* manager = sheet->dependencies();
  QValueList<Point> deps = manager->getDependants( cellPoint );
  
  depView->clear();
  for( unsigned i = 0; i < deps.count(); i++ )
  {
    QString k1, k2;
    
    Point point = deps[i];
    int row = point.row();
    int column = point.column();
    k1 = Cell::fullName( point.sheet(), column, row );
    
    new QListViewItem( depView, k1, k2 );
  }
  
}

Inspector::Inspector( Cell* cell ):
  KDialogBase( KDialogBase::Tabbed, "Inspector", KDialogBase::Close, 
  KDialogBase::Close )
{
  d = new Private;
  
  d->cell = cell;
  d->format = cell->format();
  d->sheet = cell->sheet();
  
  QFrame* cellPage = addPage( QString("Cell") );
  QVBoxLayout* cellLayout = new QVBoxLayout( cellPage, 0 );
  d->cellView = new QListView( cellPage );
  cellLayout->addWidget( d->cellView );
  d->cellView->addColumn( "Key", 150 );
  d->cellView->addColumn( "Value" );
  
  QFrame* formatPage = addPage( QString("Format") );
  QVBoxLayout* formatLayout = new QVBoxLayout( formatPage, 0 );
  d->formatView = new QListView( formatPage );
  formatLayout->addWidget( d->formatView );
  d->formatView->addColumn( "Key", 150 );
  d->formatView->addColumn( "Value" );
  
  QFrame* sheetPage = addPage( QString("Sheet") );
  QVBoxLayout* sheetLayout = new QVBoxLayout( sheetPage, 0 );
  d->sheetView = new QListView( sheetPage );
  sheetLayout->addWidget( d->sheetView );
  d->sheetView->addColumn( "Key", 150 );
  d->sheetView->addColumn( "Value" );
  
  QFrame* depPage = addPage( QString("Dependencies") );
  QVBoxLayout* depLayout = new QVBoxLayout( depPage, 0 );
  d->depView = new QListView( depPage );
  depLayout->addWidget( d->depView );
  d->depView->addColumn( "Cell", 150 );
  d->depView->addColumn( "Content" );
  
  d->handleCell();
  d->handleFormat();
  d->handleSheet();
  d->handleDep();
  
  resize( 350, 400 );
}  

Inspector::~Inspector()
{
  delete d;
}

#include "inspector.moc"


