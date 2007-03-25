/* This file is part of the KDE project
   Copyright 2005 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Style.h"
#include "Value.h"

namespace KSpread
{

class Inspector::Private
{
public:
  Cell cell;
  Style style;
  Sheet* sheet;

  Q3ListView *cellView;
  Q3ListView *sheetView;
  Q3ListView *styleView;
  Q3ListView* depView;

  void handleCell();
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

#if 0
static QString longAsHexstring( long l )
{
    return QString("%1").arg(l, 8, 16);
}
#endif

static QString dirAsString( Qt::LayoutDirection dir )
{
  QString str;
  switch( dir )
  {
    case Qt::LeftToRight: str = "Left to Right"; break;
    case Qt::RightToLeft: str = "Right to Left"; break;
    default: str = "Unknown"; break;
  }
  return str;
}

void Inspector::Private::handleCell()
{
  QString str;

  cellView->clear();

  new Q3ListViewItem( cellView, "Column", QString::number( cell.column() ) );
  new Q3ListViewItem( cellView, "Row", QString::number( cell.row() ) );
  new Q3ListViewItem( cellView, "Name", cell.name() );
  new Q3ListViewItem( cellView, "Full Name", cell.fullName() );

  new Q3ListViewItem( cellView, "Default", boolAsString( cell.isDefault() ) );
  new Q3ListViewItem( cellView, "Empty", boolAsString( cell.isEmpty() ) );
  new Q3ListViewItem( cellView, "Formula", boolAsString( cell.isFormula() ) );
//   new Q3ListViewItem( cellView, "Format Properties", longAsHexstring( static_cast<long>( cell.style()->propertiesMask() ) ) );
//   new Q3ListViewItem( cellView, "Style Properties", longAsHexstring( static_cast<long>( cell.style()->style()->features() ) ) );
  new Q3ListViewItem( cellView, "Text", cell.inputText() );
  new Q3ListViewItem( cellView, "Text (Displayed)",
		     cell.displayText().replace( QChar('\n'), "\\n" ) );

  QTextStream ts( &str, QIODevice::WriteOnly );
  ts << cell.value();
  new Q3ListViewItem( cellView, "Value", str );

  new Q3ListViewItem( cellView, "Link", cell.link() );

  new Q3ListViewItem( cellView, "Width", QString::number( cell.width() ) );
  new Q3ListViewItem( cellView, "Height", QString::number( cell.height() ) );
}

void Inspector::Private::handleStyle() // direct style access
{
  styleView->clear();
  const Style style = cell.style();

  new Q3ListViewItem( styleView, "Angle", QString::number( style.angle() ) );
  new Q3ListViewItem( styleView, "Multirow", boolAsString( style.wrapText() ) );
  new Q3ListViewItem( styleView, "Protected", boolAsString( !style.notProtected() ) );
  new Q3ListViewItem( styleView, "Vertical Text", boolAsString( style.verticalText() ) );

  new Q3ListViewItem( styleView, "Currency symbol", style.currency().symbol() );
  new Q3ListViewItem( styleView, "Currency code", style.currency().code() );

  Q3ListViewItem* flags = new Q3ListViewItem( styleView, "Flags" );
  new Q3ListViewItem( flags, "Border (left)",
                     boolAsString( style.hasAttribute(Style::LeftPen) ) );
  new Q3ListViewItem( flags, "Border (right)",
                     boolAsString( style.hasAttribute(Style::RightPen) ) );
  new Q3ListViewItem( flags, "Border (top)",
                     boolAsString( style.hasAttribute(Style::TopPen) ) );
  new Q3ListViewItem( flags, "Border (bottom)",
                     boolAsString( style.hasAttribute(Style::BottomPen) ) );

  new Q3ListViewItem( styleView, "Border pen width (bottom)",
                     QString::number( style.bottomBorderPen().width() ) );
}

void Inspector::Private::handleSheet()
{
  sheetView->clear();

  new Q3ListViewItem( sheetView, "Name", sheet->sheetName() ) ;
  new Q3ListViewItem( sheetView, "Layout Direction", dirAsString( sheet->layoutDirection() ) );
}

void Inspector::Private::handleDep()
{
  DependencyManager* manager = sheet->map()->dependencyManager();
  Region deps = manager->consumingRegion( cell );

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

Inspector::Inspector( const Cell& cell )
    : KPageDialog()
    , d( new Private )
{
  setFaceType( Tabbed );
  setCaption( "Inspector" );
  setButtons( Close );
  setDefaultButton( Close );

  d->cell = cell;
  d->style = cell.style();
  d->sheet = cell.sheet();

  QFrame* cellPage = new QFrame();
  addPage( cellPage, QString("Cell") );
  QVBoxLayout* cellLayout = new QVBoxLayout( cellPage );
  cellLayout->setMargin(KDialog::marginHint());
  cellLayout->setSpacing(KDialog::spacingHint());
  d->cellView = new Q3ListView( cellPage );
  cellLayout->addWidget( d->cellView );
  d->cellView->addColumn( "Key", 150 );
  d->cellView->addColumn( "Value" );

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


