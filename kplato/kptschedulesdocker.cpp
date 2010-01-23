/* This file is part of the KDE project
 * Copyright (C) 2009 Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kptschedulesdocker.h"

#include "kptschedule.h"

#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoResourceManager.h>

#include <klocale.h>
#include <kdebug.h>

#include <QAbstractItemView>
#include <QModelIndex>
#include <QModelIndexList>
#include <QTreeView>
#include <QVBoxLayout>
#include <QTimer>

namespace KPlato
{

SchedulesDocker::SchedulesDocker()
{
    setWindowTitle(i18n("Schedule Selector"));
    m_view = new QTreeView( this );
    m_sfModel.setSourceModel( &m_model );
    m_view->setModel( &m_sfModel );
    m_sfModel.setFilterKeyColumn ( ScheduleModel::ScheduleScheduled );
    m_sfModel.setFilterRole( Qt::EditRole );
    m_sfModel.setFilterFixedString( "true" );
    m_sfModel.setDynamicSortFilter ( true );

    for( int c = 1; c <  m_model.columnCount(); ++c ) { 
        m_view->setColumnHidden( c, true );
    }
    m_view->setHeaderHidden( true );
    m_view->setSelectionMode( QAbstractItemView::SingleSelection );
    m_view->setSelectionBehavior( QAbstractItemView::SelectRows );

    setWidget(m_view);

    connect( m_view->selectionModel(), SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection& ) ), SLOT( slotSelectionChanged() ) );
}

SchedulesDocker::~SchedulesDocker()
{
}

void SchedulesDocker::slotSelectionChanged()
{
    emit selectionChanged( selectedSchedule() );
}

void SchedulesDocker::setProject( Project *project )
{
    kDebug()<<project;
    m_model.setProject( project );
}

ScheduleManager *SchedulesDocker::selectedSchedule() const
{
    QModelIndexList lst = m_view->selectionModel()->selectedRows();
    Q_ASSERT( lst.count() <= 1 );
    ScheduleManager *sm = 0;
    if ( ! lst.isEmpty() ) {
        sm = m_model.manager( m_sfModel.mapToSource( lst.first() ) );
    }
    return sm;
}

void SchedulesDocker::setSelectedSchedule( ScheduleManager *sm )
{
    qDebug()<<"setSelectedSchedule:"<<sm<<m_model.index( sm );
    QModelIndex idx = m_sfModel.mapFromSource( m_model.index( sm ) );
    if ( sm ) {
        Q_ASSERT( idx.isValid() );
    }
    m_view->selectionModel()->select( idx, QItemSelectionModel::ClearAndSelect );
    qDebug()<<"setSelectedSchedule:"<<sm<<idx;
}

//--------------------
SchedulesDockerFactory::SchedulesDockerFactory()
{
}

QString SchedulesDockerFactory::id() const
{
    return QString("KPlatoSchedulesDocker");
}

QDockWidget* SchedulesDockerFactory::createDockWidget()
{
    return new SchedulesDocker();
}

} //namespace KPlato

#include "kptschedulesdocker.moc"

