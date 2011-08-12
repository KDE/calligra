/* This file is part of the KDE project
  Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#include "reportscripts.h"

#include "reportdata.h"

namespace KPlato
{


ProjectAccess::ProjectAccess( ReportData *rd )
    : m_reportdata( rd )
{
}

QString ProjectAccess::Name() const
{
    if ( m_reportdata && m_reportdata->project() ) {
        return m_reportdata->project()->name();
    }
    return QString();
}

QString ProjectAccess::Manager() const
{
    if ( m_reportdata && m_reportdata->project() ) {
        return m_reportdata->project()->leader();
    }
    return QString();
}

QString ProjectAccess::Plan() const
{
    if ( m_reportdata && m_reportdata->scheduleManager() ) {
        return m_reportdata->scheduleManager()->name();
    }
    return QString();
}

QVariant ProjectAccess::BCWS() const
{
    if ( m_reportdata && m_reportdata->project() ) {
        long id = m_reportdata->scheduleManager() ? m_reportdata->scheduleManager()->scheduleId() : BASELINESCHEDULE;
        return m_reportdata->project()->bcws( QDate::currentDate(), id );
    }
    return QString();
}

QVariant ProjectAccess::BCWP() const
{
    if ( m_reportdata && m_reportdata->project() ) {
        long id = m_reportdata->scheduleManager() ? m_reportdata->scheduleManager()->scheduleId() : BASELINESCHEDULE;
        return m_reportdata->project()->bcwp( QDate::currentDate(), id );
    }
    return QString();
}

QVariant ProjectAccess::ACWP() const
{
    if ( m_reportdata && m_reportdata->project() ) {
        long id = m_reportdata->scheduleManager() ? m_reportdata->scheduleManager()->scheduleId() : BASELINESCHEDULE;
        return m_reportdata->project()->acwp( QDate::currentDate(), id ).cost();
    }
    return QString();
}

QVariant ProjectAccess::CPI() const
{
    if ( m_reportdata && m_reportdata->project() ) {
        double r = 0.0;
        long id = m_reportdata->scheduleManager() ? m_reportdata->scheduleManager()->scheduleId() : BASELINESCHEDULE;
        double b = m_reportdata->project()->bcwp( QDate::currentDate(), id );
        double a = m_reportdata->project()->acwp( QDate::currentDate(), id ).cost();
        if ( a > 0 ) {
            r = b / a;
        }
        return r;
    }
    return QVariant();
}

QVariant ProjectAccess::SPI() const
{
    qDebug()<<"ProjectAccess::SPI:";
    if ( m_reportdata && m_reportdata->project() ) {
        int id = m_reportdata->scheduleManager() ? m_reportdata->scheduleManager()->scheduleId() : BASELINESCHEDULE;
        return m_reportdata->project()->schedulePerformanceIndex( QDate::currentDate(), id );
    }
    return QVariant();
}


} // namespace KPlato

#include "reportscripts.moc"
