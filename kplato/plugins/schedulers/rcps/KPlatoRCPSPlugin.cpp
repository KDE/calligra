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

#include "KPlatoRCPSPlugin.h"

#include "kptschedulerplugin_macros.h"

#include "KPlatoRCPSScheduler.h"

#include "kptproject.h"
#include "kptschedule.h"

#include <librcps.h>

#include <KDebug>

#include <QTimer>

KPLATO_SCHEDULERPLUGIN_EXPORT(KPlatoRCPSPlugin)

KPlatoRCPSPlugin::KPlatoRCPSPlugin( QObject * parent, const QVariantList & )
    : KPlato::SchedulerPlugin(parent)
{
    kDebug()<<rcps_version();
}

KPlatoRCPSPlugin::~KPlatoRCPSPlugin()
{
}

void KPlatoRCPSPlugin::calculate( KPlato::Project &project, KPlato::ScheduleManager *sm, bool nothread )
{
    qDebug()<<"KPlatoRCPSPlugin::calculate:";
    foreach ( KPlatoRCPSScheduler *j, m_jobs ) {
        if ( j->manager() == sm ) {
            return;
        }
    }
    sm->setScheduling( true );
    KPlatoRCPSScheduler *job = new KPlatoRCPSScheduler( &project, sm, this );
    project.changed( sm );
    int result = job->kplatoToRCPS();
    if ( result != 0 ) {
        sm->expected()->logError( i18n( "Failed to build a valid RCPS project: %1", result, 0 ) );
        delete job;
        sm->setScheduling( false );
        return;
    }
    m_jobs << job;
    connect( job, SIGNAL( jobStarted( KPlatoRCPSScheduler* ) ), SLOT( slotStarted( KPlatoRCPSScheduler* ) ) );
    connect( job, SIGNAL( jobFinished( KPlatoRCPSScheduler* ) ), SLOT( slotFinished( KPlatoRCPSScheduler* ) ) );
    if ( nothread ) {
        job->doRun();
    } else {
        job->start();
    }
    qDebug()<<"KPlatoRCPSPlugin::calculate: started";
}

void KPlatoRCPSPlugin::slotStarted( KPlatoRCPSScheduler *job )
{
    qDebug()<<"KPlatoRCPSPlugin::slotStarted:";
}

void KPlatoRCPSPlugin::slotFinished( KPlatoRCPSScheduler *job )
{
    qDebug()<<"KPlatoRCPSPlugin::slotFinished:";
    //KPlatoRCPSScheduler *s = qobject_cast<KPlatoRCPSScheduler*>( job );
    job->manager()->setScheduling( false );
    job->deleteLater();
}


#include "KPlatoRCPSPlugin.moc"
