/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen kplato@kde.org>

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

#include "kptworkpackageview.h"
#include <kptworkpackagemodel.h>

#include "kptitemmodelbase.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"

#include <KoDocument.h>

#include <QAbstractItemModel>

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>


#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>

#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

namespace KPlato
{


WorkPackageTableView::WorkPackageTableView( QWidget *parent )
    : QTableView( parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
//    setStretchLastSection( false );
    
    setModel( new WorkPackageModel() );
    
//    setSelectionMode( QAbstractItemView::ExtendedSelection );

}

Resource *WorkPackageTableView::currentResource() const
{
    return 0;//itemModel()->object( selectionModel()->currentIndex() );
}

QList<Resource*> WorkPackageTableView::selectedResources() const
{
    QList<Resource*> lst;
/*    foreach (QModelIndex i, selectionModel()->selectedRows() ) {
        lst << static_cast<QObject*>( i.internalPointer() );
    }*/
    return lst;
}

WorkPackageModel *WorkPackageTableView::itemModel() const
{
    return static_cast<WorkPackageModel*>( model() );
}

Project *WorkPackageTableView::project() const
{
    return itemModel()->project();
}

void WorkPackageTableView::setProject( Project *project )
{
    itemModel()->setProject( project );
}

Task *WorkPackageTableView::task() const
{
    return itemModel()->task();
}

void WorkPackageTableView::setTask( Task *task )
{
    itemModel()->setTask( task );
}


} // namespace KPlato

#include "kptworkpackageview.moc"
