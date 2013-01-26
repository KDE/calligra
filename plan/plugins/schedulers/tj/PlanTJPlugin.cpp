/* This file is part of the KDE project
 * Copyright (C) 2009, 2011 Dag Andersen <danders@get2net.dk>
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

#include "PlanTJPlugin.h"

#include "kptschedulerplugin_macros.h"

#include "PlanTJScheduler.h"

#include "kptproject.h"
#include "kptschedule.h"

#include "kptdebug.h"

#include <QApplication>
#include <KMessageBox>

KPLATO_SCHEDULERPLUGIN_EXPORT(PlanTJPlugin)

using namespace KPlato;

PlanTJPlugin::PlanTJPlugin( QObject * parent, const QVariantList & )
    : KPlato::SchedulerPlugin(parent)
{
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "plantjplugin" );
    }
    m_granularities << (long unsigned int) 5 * 60 * 1000
                    << (long unsigned int) 15 * 60 * 1000
                    << (long unsigned int) 30 * 60 * 1000
                    << (long unsigned int) 60 * 60 * 1000;
}

PlanTJPlugin::~PlanTJPlugin()
{
}

QString PlanTJPlugin::description() const
{
    return i18nc( "@info:whatsthis", "<title>TaskJuggler Scheduler</title>"
                    "<para>This is a slightly modified version of the scheduler used in TaskJuggler."
                    " It has been enhanced to handle resource units.</para>"
                    "<para>Scheduling backwards is simulated by scheduling all tasks as late as possible.</para>"
                    "<para><note>Plan does not utilize all of its functionality.</note></para>"
                );
}

int PlanTJPlugin::capabilities() const
{
    return SchedulerPlugin::AvoidOverbooking | SchedulerPlugin::ScheduleForward | SchedulerPlugin::ScheduleBackward;
}

ulong PlanTJPlugin::currentGranularity() const
{
    ulong v = m_granularities.value( m_granularity );
    return qMax( v, (ulong)300000 ); // minimum 5 min
}

void PlanTJPlugin::calculate( KPlato::Project &project, KPlato::ScheduleManager *sm, bool nothread )
{
    foreach ( SchedulerThread *j, m_jobs ) {
        if ( j->manager() == sm ) {
            return;
        }
    }
    sm->setScheduling( true );

    PlanTJScheduler *job = new PlanTJScheduler( &project, sm, currentGranularity() );
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

void PlanTJPlugin::stopAllCalculations()
{
    foreach ( SchedulerThread *s, m_jobs ) {
        stopCalculation( s );
    }
}

void PlanTJPlugin::stopCalculation( SchedulerThread *sch )
{
    if ( sch ) {
         //FIXME: this should just call stopScheduling() and let the job finish "normally"
        disconnect( sch, SIGNAL( jobFinished( PlanTJScheduler* ) ), this, SLOT( slotFinished( PlanTJScheduler* ) ) );
        sch->stopScheduling();
        // wait max 20 seconds.
        sch->mainManager()->setCalculationResult( ScheduleManager::CalculationStopped );
        if ( ! sch->wait( 20000 ) ) {
            sch->deleteLater();
            m_jobs.removeAt( m_jobs.indexOf( sch ) );
        } else {
            slotFinished( sch );
        }
    }
}

void PlanTJPlugin::slotStarted( SchedulerThread */*job*/ )
{
//    kDebug(planDbg())<<"PlanTJPlugin::slotStarted:";
}

void PlanTJPlugin::slotFinished( SchedulerThread *j )
{
    PlanTJScheduler *job = static_cast<PlanTJScheduler*>( j );
    Project *mp = job->mainProject();
    ScheduleManager *sm = job->mainManager();
    //kDebug(planDbg())<<"PlanTJPlugin::slotFinished:"<<mp<<sm<<job->isStopped();
    if ( job->isStopped() ) {
        sm->setCalculationResult( ScheduleManager::CalculationCanceled );
    } else {
        updateLog( job );
        if ( job->result > 0 ) {
            sm->setCalculationResult( ScheduleManager::CalculationError );
        } else {
            Project *tp = job->project();
            ScheduleManager *tm = job->manager();
            updateProject( tp, tm, mp, sm );
            sm->setCalculationResult( ScheduleManager::CalculationDone );
        }
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


#include "PlanTJPlugin.moc"
