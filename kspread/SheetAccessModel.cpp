/* This file is part of the KDE project
   Copyright 2009 Johannes Simon <johannes.simon@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

// Ours
#include "SheetAccessModel.h"
#include "Limits.h"
#include "Map.h"
#include "Binding.h"
#include "BindingManager.h"
#include "Region.h"

// Qt
#include <QList>
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QVariant>

// KOffice
//#include <KoStore.h>
//#include <KoXmlWriter.h>
//#include <KoShapeSavingContext.h>

Q_DECLARE_METATYPE( QPointer<QAbstractItemModel> )

namespace KSpread {

class SheetAccessModel::Private
{
public:
    Map *map;
};

SheetAccessModel::SheetAccessModel( Map *map )
    : d( new Private )
{
    d->map = map;

    connect( map, SIGNAL( sheetAdded(Sheet*) ),
             this, SLOT( slotSheetAdded(Sheet*) ) );
    // FIXME: Check if we can simply connect sheetRevived() to slotSheetAdded()
    connect( map, SIGNAL( sheetRevived(Sheet*) ),
             this, SLOT( slotSheetAdded(Sheet*) ) );
    connect( map, SIGNAL( sheetRemoved(Sheet*) ),
             this, SLOT( slotSheetRemoved(Sheet*) ) );

    setRowCount( 1 );
    setColumnCount( 0 );
}

void SheetAccessModel::slotSheetAdded( Sheet *sheet )
{
    QStandardItem *item = new QStandardItem;
    QList<QStandardItem*> col;
    col.append( item );

    connect( sheet, SIGNAL( sig_nameChanged( Sheet*, QString ) ),
             this,  SLOT( slotSheetNameChanged( Sheet*, QString ) ) );

    // This region contains the entire sheet
    const Region region( 1, 1, KS_colMax, KS_rowMax, sheet );
    const QPointer<QAbstractItemModel> model = const_cast<QAbstractItemModel*>( d->map->bindingManager()->createModel( region.name() ) );

    item->setData( qVariantFromValue( model ), Qt::DisplayRole );

    const int sheetIndex = d->map->indexOf( sheet );

    insertColumn( sheetIndex, col );
    setHeaderData( sheetIndex, Qt::Horizontal, sheet->sheetName() );
}

void SheetAccessModel::slotSheetRemoved( Sheet *sheet )
{
    disconnect( sheet, SIGNAL( sig_nameChanged( Sheet*, QString ) ),
                this,  SLOT( slotSheetNameChanged( Sheet*, QString ) ) );

    removeColumn( d->map->indexOf( sheet ), QModelIndex() );
}

void SheetAccessModel::slotSheetNameChanged( Sheet *sheet, const QString &oldName )
{
    Q_UNUSED( oldName );

    const int sheetIndex = d->map->indexOf( sheet );
    // We should never receive signals from sheets that are not in our model
    Q_ASSERT( sheetIndex >= 0 );
    setHeaderData( sheetIndex, Qt::Horizontal, sheet->sheetName() );
}

} // namespace KSpread
