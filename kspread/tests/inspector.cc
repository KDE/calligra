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

// Qt
#include <QFrame>
#include <QLayout>
#include <q3listview.h>
#include <QTextStream>
//Added by qt3to4:
#include <QVBoxLayout>

#include <kpagedialog.h>

// KSpread
#include "Cell.h"
#include "DependencyManager.h"
#include "Format.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Style.h"

namespace KSpread
{

class Inspector::Private
{
public:
  Cell* cell;
  Format* format;
  Sheet* sheet;

  Q3ListView *cellView;
  Q3ListView *formatView;
  Q3ListView *sheetView;
  Q3ListView *styleView;
  Q3ListView* depView;

  void handleCell();
  void handleFormat();
  void handleSheet();
  void handleStyle();
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

  new Q3ListViewItem( cellView, "Column", QString::number( cell->column() ) );
  new Q3ListViewItem( cellView, "Row", QString::number( cell->row() ) );
  new Q3ListViewItem( cellView, "Name", cell->name() );
  new Q3ListViewItem( cellView, "Full Name", cell->fullName() );

  new Q3ListViewItem( cellView, "Default", boolAsString( cell->isDefault() ) );
  new Q3ListViewItem( cellView, "Empty", boolAsString( cell->isEmpty() ) );
  new Q3ListViewItem( cellView, "Formula", boolAsString( cell->isFormula() ) );
  new Q3ListViewItem( cellView, "Format Properties", longAsHexstring( static_cast<long>( cell->format()->propertiesMask() ) ) );
  new Q3ListViewItem( cellView, "Style Properties", longAsHexstring( static_cast<long>( cell->format()->style()->features() ) ) );
  new Q3ListViewItem( cellView, "Text", cell->text() );
  new Q3ListViewItem( cellView, "Text (Displayed)",
		     cell->strOutText().replace( QChar('\n'), "\\n" ) );

  QTextStream ts( &str, QIODevice::WriteOnly );
  ts << cell->value();
  new Q3ListViewItem( cellView, "Value", str );

  new Q3ListViewItem( cellView, "Link", cell->link() );

  new Q3ListViewItem( cellView, "Width", QString::number( cell->dblWidth() ) );
  new Q3ListViewItem( cellView, "Height", QString::number( cell->dblHeight() ) );
}

void Inspector::Private::handleFormat()
{
  formatView->clear();
  int col = cell->column();
  int row = cell->row();

  new Q3ListViewItem( formatView, "Angle", QString::number( format->getAngle(col, row) ) );
  new Q3ListViewItem( formatView, "Multirow", boolAsString( format->multiRow(col, row) ) );
  new Q3ListViewItem( formatView, "Protected", format->hasProperty( Style::SVerticalText )
    ? "Not specified" : boolAsString( format->isProtected(col, row) ) );
  new Q3ListViewItem( formatView, "Vertical Text", boolAsString( format->verticalText(col, row ) ) );

  Style::Currency currrency;
  bool valid = format->currencyInfo(currrency);
  new Q3ListViewItem( formatView, "Currency symbol", valid ? currrency.symbol : "Invalid" );
  bool ok = false;
  QString currencyType;
  if (valid)
    currencyType = Currency::getChooseString(currrency.type, ok);
  new Q3ListViewItem( formatView, "Currency type", valid && ok ? currencyType : "Invalid" );

  Q3ListViewItem* flags = new Q3ListViewItem( formatView, "Flags" );
  new Q3ListViewItem( flags, "Border (left)",
                     boolAsString( format->hasProperty(Style::SLeftBorder, true) ) );
  new Q3ListViewItem( flags, "Border (right)",
                     boolAsString( format->hasProperty(Style::SRightBorder, true) ) );
  new Q3ListViewItem( flags, "Border (top)",
                     boolAsString( format->hasProperty(Style::STopBorder, true) ) );
  new Q3ListViewItem( flags, "Border (bottom)",
                     boolAsString( format->hasProperty(Style::SBottomBorder, true) ) );

  new Q3ListViewItem( formatView, "Border pen width (bottom)",
                     QString::number( format->bottomBorderPen(col,row).width() ) );
}

void Inspector::Private::handleStyle() // direct style access
{
  styleView->clear();
  const Style* style = cell->format()->style();

  Q3ListViewItem* flags = new Q3ListViewItem( styleView, "Flags" );
  new Q3ListViewItem( flags, "Border (left)",
                     boolAsString( style->hasFeature(Style::SLeftBorder, true) ) );
  new Q3ListViewItem( flags, "Border (right)",
                     boolAsString( style->hasFeature(Style::SRightBorder, true) ) );
  new Q3ListViewItem( flags, "Border (top)",
                     boolAsString( style->hasFeature(Style::STopBorder, true) ) );
  new Q3ListViewItem( flags, "Border (bottom)",
                     boolAsString( style->hasFeature(Style::SBottomBorder, true) ) );

  new Q3ListViewItem( styleView, "Border pen width (bottom)",
                     QString::number( style->bottomBorderPen().width() ) );
}

void Inspector::Private::handleSheet()
{
  sheetView->clear();

  new Q3ListViewItem( sheetView, "Name", sheet->sheetName() ) ;
  new Q3ListViewItem( sheetView, "Layout Direction", dirAsString( sheet->layoutDirection() ) );
}

void Inspector::Private::handleDep()
{
  DependencyManager* manager = sheet->workbook()->dependencyManager();
  Region deps = manager->getDependants( cell );

  depView->clear();
  Region::ConstIterator end(deps.constEnd());
  for( Region::ConstIterator it(deps.constBegin()); it != end; ++it )
  {
    const QRect range = (*it)->rect();
    for (int col = range.left(); col <= range.right(); ++col)
      for (int row = range.top(); row <= range.bottom(); ++row)
    {
    QString k1, k2;

    k1 = Cell::fullName( (*it)->sheet(), col, row );

    new Q3ListViewItem( depView, k1, k2 );
    }
  }

}

Inspector::Inspector( Cell* cell ):
  KPageDialog()
{
  setFaceType( Tabbed );
  setCaption( "Inspector" );
  setButtons( Close );
  setDefaultButton( Close );
  d = new Private;

  d->cell = cell;
  d->format = cell->format();
  d->sheet = cell->sheet();

  QFrame* cellPage = new QFrame();
  addPage( cellPage, QString("Cell") );
  QVBoxLayout* cellLayout = new QVBoxLayout( cellPage );
  cellLayout->setMargin(KDialog::marginHint());
  cellLayout->setSpacing(KDialog::spacingHint());
  d->cellView = new Q3ListView( cellPage );
  cellLayout->addWidget( d->cellView );
  d->cellView->addColumn( "Key", 150 );
  d->cellView->addColumn( "Value" );

  QFrame* formatPage = new QFrame();
  addPage(formatPage,  QString("Format") );
  QVBoxLayout* formatLayout = new QVBoxLayout( formatPage );
  formatLayout->setMargin(KDialog::marginHint());
  formatLayout->setSpacing(KDialog::spacingHint());
  d->formatView = new Q3ListView( formatPage );
  formatLayout->addWidget( d->formatView );
  d->formatView->addColumn( "Key", 150 );
  d->formatView->addColumn( "Value" );

  QFrame* stylePage = new QFrame();
  addPage(stylePage, QString("Style") );
  QVBoxLayout* styleLayout = new QVBoxLayout( stylePage );
  d->styleView = new Q3ListView( stylePage );
  styleLayout->addWidget( d->styleView );
  d->styleView->addColumn( "Key", 150 );
  d->styleView->addColumn( "Value" );

  QFrame* sheetPage = new QFrame();
  addPage(sheetPage,  QString("Sheet") );
  QVBoxLayout* sheetLayout = new QVBoxLayout( sheetPage );
  sheetLayout->setMargin(KDialog::marginHint());
  sheetLayout->setSpacing(KDialog::spacingHint());
  d->sheetView = new Q3ListView( sheetPage );
  sheetLayout->addWidget( d->sheetView );
  d->sheetView->addColumn( "Key", 150 );
  d->sheetView->addColumn( "Value" );

  QFrame* depPage = new QFrame();
  addPage(depPage,  QString("Dependencies") );
  QVBoxLayout* depLayout = new QVBoxLayout( depPage );
  depLayout->setMargin(KDialog::marginHint());
  depLayout->setSpacing(KDialog::spacingHint());
  d->depView = new Q3ListView( depPage );
  depLayout->addWidget( d->depView );
  d->depView->addColumn( "Cell", 150 );
  d->depView->addColumn( "Content" );

  d->handleCell();
  d->handleFormat();
  d->handleSheet();
  d->handleStyle();
  d->handleDep();

  resize( 350, 400 );
}

Inspector::~Inspector()
{
  delete d;
}

#include "inspector.moc"


