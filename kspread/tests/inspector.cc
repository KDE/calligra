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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "inspector.h"

#include <qlayout.h>
#include <qlistview.h>
#include <qtextstream.h>

#include <kdialogbase.h>

#include "kspread_cell.h"
#include "kspread_sheet.h"

namespace KSpread
{

class Inspector::Private
{
public:
  KSpreadCell* cell;
  KSpreadSheet* sheet;
  
  QListView *cellView;
  QListView *sheetView;
  
  
  void handleCell();
  void handleSheet();
  
};

}

using namespace KSpread;

static QString boolAsString( bool b )
{
  if( b ) return QString( "True" );
  else return QString( "False" );
}

static QString dirAsString( KSpreadSheet::LayoutDirection dir )
{
  QString str;
  switch( dir )
  {
    case KSpreadSheet::LeftToRight: str = "Left to Right"; break;
    case KSpreadSheet::RightToLeft: str = "Right to Left"; break;
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
  new QListViewItem( cellView, "Text", cell->text() );
  new QListViewItem( cellView, "Text (Displayed)", cell->strOutText() );

  QTextStream ts( &str, IO_WriteOnly );
  ts << cell->value();
  new QListViewItem( cellView, "Value", str );
  
  new QListViewItem( cellView, "Width", QString::number( cell->dblWidth() ) );
  new QListViewItem( cellView, "Height", QString::number( cell->dblHeight() ) );
}


void Inspector::Private::handleSheet()
{  
  sheetView->clear();
  
  new QListViewItem( sheetView, "Name", sheet->sheetName() ) ;
  new QListViewItem( sheetView, "Layout Direction", dirAsString( sheet->layoutDirection() ) );
}

Inspector::Inspector( KSpreadCell* cell ):
  KDialogBase( KDialogBase::Tabbed, "Inspector", KDialogBase::Close, 
  KDialogBase::Close )
{
  d = new Private;
  
  d->cell = cell;
  d->sheet = cell->sheet();
  
  QFrame* cellPage = addPage( QString("Cell") );
  QVBoxLayout* cellLayout = new QVBoxLayout( cellPage, 0 );
  d->cellView = new QListView( cellPage );
  cellLayout->addWidget( d->cellView );
  d->cellView->addColumn( "Key", 150 );
  d->cellView->addColumn( "Value" );
  
  QFrame* sheetPage = addPage( QString("Sheet") );
  QVBoxLayout* sheetLayout = new QVBoxLayout( sheetPage, 0 );
  d->sheetView = new QListView( sheetPage );
  sheetLayout->addWidget( d->sheetView );
  d->sheetView->addColumn( "Key", 150 );
  d->sheetView->addColumn( "Value" );
  
  d->handleCell();
  d->handleSheet();
  
  resize( 350, 400 );
}  

Inspector::~Inspector()
{
  delete d;
}

#include "inspector.moc"


