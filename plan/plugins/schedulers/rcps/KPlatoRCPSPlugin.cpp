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

#include <QApplication>
#include <KMessageBox>
#include <kptschedulerplugin.h>

KPLATO_SCHEDULERPLUGIN_EXPORT(KPlatoRCPSPlugin)

using namespace KPlato;

KPlatoRCPSPlugin::KPlatoRCPSPlugin( QObject * parent, const QVariantList & )
    : KPlato::SchedulerPlugin(parent)
{
    kDebug()<<rcps_version();
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "planrcpsplugin" );
    }
}

KPlatoRCPSPlugin::~KPlatoRCPSPlugin()
{
}

QString KPlatoRCPSPlugin::description() const
{
    return i18nc( "@info:whatsthis", "<title>RCPS Scheduler</title>"
                    "<para>The Resource Constrained Project Scheduler (RCPS) focuses on scheduling"
                    " the project to avoid overbooking resources."
                    " It still respects task dependencies and also tries to fullfill time constraints."
                    " However, time constraints can make it very difficult to find a good solution,"
                    " so it may be preferable to use a different scheduler in these cases.</para>"
                );
}

int KPlatoRCPSPlugin::capabilities() const
{
    return SchedulerPlugin::AvoidOverbooking | SchedulerPlugin::ScheduleForward | SchedulerPlugin::ScheduleBackward;
}

void KPlatoRCPSPlugin::calculate( KPlato::Project &project, KPlato::ScheduleManager *sm, bool nothread )
{
    foreach ( SchedulerThread *j, m_jobs ) {
        if ( j->manager() == sm ) {
            return;
        }
    }
    sm->setScheduling( true );

    KPlatoRCPSScheduler *job = new KPlatoRCPSScheduler( &project, sm );
    m_jobs << job;
    connect(job, SIGNAL(jobFinished(SchedulerThread*)), SLOT(slotFinished(SchedulerThread*)));

    project.changed( sm );

//     connect(this, SIGNAL(sigCalculationStarted(Project*, ScheduleManager*)), &project, SIGNAL(sigCalculationStarted(Project*, ScheduleManager*)));
//     connect(this, SIGNAL( sigCalculationFinished(Project*, ScheduleManager*)), &project, SIGNAL(sigCalculationFinished(Project*, ScheduleManager* )));

    connect(job, SIGNAL(maxProgressChanged(int)), sm, SLOT(setMaxProgress(int)));
    connect(job, SIGNAL(progressChanged(int)), sm, SLOT(setProgress(int)));

    if ( nothread ) {
        job->doRun();
    } else {
        job->start();
    }
}

void KPlatoRCPSPlugin::stopAllCalculations()
{
    foreach ( SchedulerThread *s, m_jobs ) {
        stopCalculation( s );
    }
}

void KPlatoRCPSPlugin::stopCalculation( SchedulerThread *sch )
{
    if ( sch ) {
         //FIXME: this should just call stopScheduling() and let the job finish "normally"
        disconnect( sch, SIGNAL( jobFinished( KPlatoRCPSScheduler* ) ), this, SLOT( slotFinished( KPlatoRCPSScheduler* ) ) );
        sch->stopScheduling();
        // wait max 20 seconds.
        sch->mainManager()->setCalculationResult( ScheduleManager::CalculationStopped );
        if ( ! sch->wait( 20000 ) ) {
            sch->deleteLater();
            m_jobs.removeAt( m_jobs.indexOf( sch ) );
        }   else {
            slotFinished( sch );
        }
    }
}

void KPlatoRCPSPlugin::slotStarted( SchedulerThread */*job*/ )
{
//    qDebug()<<"KPlatoRCPSPlugin::slotStarted:";
}

void KPlatoRCPSPlugin::slotFinished( SchedulerThread *j )
{
    KPlatoRCPSScheduler *job = static_cast<KPlatoRCPSScheduler*>( j );
    Project *mp = job->mainProject();
    ScheduleManager *sm = job->mainManager();
    //qDebug()<<"KPlatoRCPSPlugin::slotFinished:"<<mp<<sm<<job->isStopped();
    if ( job->isStopped() ) {
        sm->setCalculationResult( ScheduleManager::CalculationCanceled );
    } else {
        updateLog( job );
        Project *tp = job->project();
        ScheduleManager *tm = job->manager();
        updateProject( tp, tm, mp, sm );
        sm->setCalculationResult( ScheduleManager::CalculationDone );
    }
    sm->setScheduling( false );

    m_jobs.removeAt( m_jobs.indexOf( job ) );
    if ( m_jobs.isEmpty() ) {
        m_synctimer.stop();
    }
    emit sigCalculationFinished( mp, sm );

    disconnect(this, SIGNAL(sigCalculationStarted(Project*, ScheduleManager*)), mp, SIGNAL(sigCalculationStarted(Project*, ScheduleManager*)));
    disconnect(this, SIGNAL(sigCalculationFinished(Project*, ScheduleManager*)), mp, SIGNAL(sigCalculationFinished(Project*, ScheduleManager* )));

    job->deleteLater();
}


#include "KPlatoRCPSPlugin.moc"
