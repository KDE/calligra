/* This file is part of the KDE project
  Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskappointmentsview.h"

#include "kptappointment.h"
#include "kpttask.h"

#include <QApplication>
#include <QList>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>

namespace KPlato
{

TaskAppointmentsView::ResourceItem::ResourceItem( Resource *r, QTreeWidget *parent, bool highlight )
        : DoubleListViewBase::MasterListItem( parent, r->name(), highlight ),
        resource( r )
{

    setFormat( 0, 'f', 1 );
    //kDebug()<<k_funcinfo<<endl;
}
TaskAppointmentsView::ResourceItem::ResourceItem( Resource *r, QTreeWidgetItem *p, bool highlight )
        : DoubleListViewBase::MasterListItem( p, r->name(), highlight ),
        resource( r )
{

    setFormat( 0, 'f', 1 );
    //kDebug()<<k_funcinfo<<endl;
}

TaskAppointmentsView::ResourceItem::ResourceItem( const QString& text, QTreeWidgetItem *parent, bool highlight )
        : DoubleListViewBase::MasterListItem( parent, text, highlight ),
        resource( 0 )
{

    setFormat( 0, 'f', 1 );
    //kDebug()<<k_funcinfo<<endl;
}

//-------------------------------------------
TaskAppointmentsView::TaskAppointmentsView( QWidget *parent )
        : DoubleListViewBase( parent ),
        m_task( 0 )
{

    setNameHeader( i18n( "Resource" ) );


    QList<int> list = sizes();
    int tot = list[ 0 ] + list[ 1 ];
    list[ 0 ] = qMin( 35, tot );
    list[ 1 ] = tot - list[ 0 ];
    setSizes( list );
}

void TaskAppointmentsView::setZoom( double zoom )
{
    Q_UNUSED( zoom );
}


void TaskAppointmentsView::draw( Task *task )
{
    m_task = task;
    draw();
}

void TaskAppointmentsView::draw()
{
    //kDebug()<<k_funcinfo<<endl;
    clearLists();
    if ( !m_task )
        return ;

    QList<Appointment*> lst = m_task->appointments();
    QListIterator<Appointment*> it( lst );
    while ( it.hasNext() ) {
        Appointment * a = it.next();
        Resource *r = a->resource() ->resource();
        TaskAppointmentsView::ResourceItem *item = new TaskAppointmentsView::ResourceItem( r, masterListView() );

        item->effortMap = a->plannedPrDay( m_task->startTime().date(), m_task->endTime().date() );
    }
    slotUpdate();
}

void TaskAppointmentsView::slotUpdate()
{
    //kDebug()<<k_funcinfo<<endl;
    if ( !m_task )
        return ;
    QApplication::setOverrideCursor( Qt::WaitCursor );
    createSlaveItems();
    KLocale *locale = KGlobal::locale();
    const KCalendarSystem *cal = locale->calendar();

    // Add columns for selected period/periods
    QDate start = m_task->startTime().date();
    QDate end = m_task->endTime().date();
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    QStringList df;
    for ( QDate dt = start; dt <= end; dt = cal->addDays( dt, 1 ) ) {
        df << locale->formatDate( dt, KLocale::ShortDate );
    }
    setSlaveLabels( df );
    foreach ( QTreeWidgetItem * i, masterItems() ) {
        TaskAppointmentsView::ResourceItem * item = static_cast<TaskAppointmentsView::ResourceItem*>( i );
        if ( !item ) {
            continue;
        }
        double eff;
        int col = 0;
        for ( QDate d = start; d <= end; d = cal->addDays( d, 1 ), ++col ) {
            eff = ( double ) ( item->effortMap.effortOnDate( d ).minutes() ) / 60.0;
            item->setSlaveItem( col, eff );
            item->addToTotal( eff );
        }
    }
    calculate();
    QApplication::restoreOverrideCursor();
}


void TaskAppointmentsView::print( KPrinter & /*printer*/ )
{
    kDebug() << k_funcinfo << endl;
}

// bool TaskAppointmentsView::setContext(const Context::TaskAppointmentsView &c) {
//     //kDebug()<<k_funcinfo<<endl;
//     const Context::TaskAppointmentsView &context = c.taskAppointmentsView;
//     QValueList<int> list;
//     list << context.accountsviewsize << context.periodviewsize;
//     m_dlv->setSizes(list);
//     m_date = context.date;
//     if (!m_date.isValid())
//         m_date = QDate::currentDate();
//     m_period = context.period;
//     m_cumulative = context.cumulative;
//
//     return true;
// }
//
// void TaskAppointmentsView::getContext(Context::TaskAppointmentsView &c) const {
//     //kDebug()<<k_funcinfo<<endl;
//     Context::TaskAppointmentsView &context = c.taskAppointmentsView;
//     context.accountsviewsize = m_dlv->sizes()[0];
//     context.periodviewsize = m_dlv->sizes()[1];
//     context.date = m_date;
//     context.period = m_period;
//     context.cumulative = m_cumulative;
//     //kDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
// }

void TaskAppointmentsView::clear()
{
    clearLists();
}

void TaskAppointmentsView::createSlaveItems()
{
    DoubleListViewBase::createSlaveItems();
    setSlaveFormat( 0, 'f', 1 );
}

}  //KPlato namespace

#include "kpttaskappointmentsview.moc"
