/*
 * This file is part of KPlato
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ScriptingWidgets.h"
#include "Module.h"
#include "Project.h"

#include "kptproject.h"
#include "kptschedule.h"
#include "kptnodeitemmodel.h"

#include <QVBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>

#include <klocale.h>
#include <kdebug.h>


ScriptingScheduleListView::ScriptingScheduleListView(Scripting::Module* module, QWidget* parent)
    : QWidget(parent), m_module(module)
{
    kDebug()<<this<<parent;
    if ( parent->layout() ) {
        parent->layout()->addWidget( this );
    }
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    m_view = new QTreeView(this);
    m_view->setAlternatingRowColors(true);
    m_view->setRootIsDecorated(false);
    m_view->setSortingEnabled(false);
    m_view->setItemsExpandable(false);
//    m_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    QStandardItemModel *model = new QStandardItemModel(m_view);
    model->setHorizontalHeaderLabels( QStringList() << "Schedule Name" );
    KPlato::Project *p = static_cast<Scripting::Project*>( m_module->project() )->kplatoProject();
    kDebug()<<p;
    foreach ( KPlato::ScheduleManager *sm, p->allScheduleManagers() ) {
        if ( sm->isScheduled() ) {
            QStandardItem *i = new QStandardItem( sm->name() );
            i->setData( (qlonglong)sm->id() );
            model->appendRow( i );
            kDebug()<<i<<model->rowCount();
        }
    }
    layout->addWidget(m_view);
    m_view->setModel( model );
}

ScriptingScheduleListView::~ScriptingScheduleListView()
{
    kDebug()<<"gone!";
}

QVariant ScriptingScheduleListView::currentSchedule() const
{
    QModelIndex i = m_view->currentIndex();
    kDebug()<<i<<i.isValid();
    if ( ! i.isValid() ) {
        kDebug()<<"index not valid";
        return -1;
    }
    kDebug()<<m_view->model();
    QStandardItem *item = static_cast<QStandardItemModel*>(m_view->model())->itemFromIndex( i );
    kDebug()<<item;
    if ( item == 0 ) {
        return -1;
    }
    kDebug()<<item->data();
    return item->data();
}

//--------------------------------
ScriptingNodePropertyListView::ScriptingNodePropertyListView(Scripting::Module* module, QWidget* parent)
    : KActionSelector(parent), m_module(module)
{
    kDebug()<<this<<parent;
    if ( parent->layout() ) {
        parent->layout()->addWidget( this );
    }
    KPlato::NodeModel m;
    const QMetaEnum e = m.columnMap();
    if ( e.keyCount() > 0 ) {
        QListWidgetItem *item = new QListWidgetItem( m.headerData( 0 ).toString() );
        item->setToolTip( m.headerData( 0 ).toString() );
        item->setData( Qt::UserRole, e.key( 0 ) );  // should be name
        selectedListWidget()->addItem( item );
        for ( int i = 1; i < e.keyCount(); ++i ) {
            QListWidgetItem *item = new QListWidgetItem( m.headerData( i ).toString() );
            item->setToolTip( m.headerData( i ).toString() );
            item->setData( Qt::UserRole, e.key( i ) );
            availableListWidget()->addItem( item );
        }
    }
}

ScriptingNodePropertyListView::~ScriptingNodePropertyListView()
{
}

QVariant ScriptingNodePropertyListView::selectedProperties() const
{
    QStringList lst;
    QListWidget *s = selectedListWidget();
    for ( int i = 0; i < s->count(); ++i ) {
        lst << s->item( i )->data( Qt::UserRole ).toString();
    }
    return lst;
}

#include "ScriptingWidgets.moc"
