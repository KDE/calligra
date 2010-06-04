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

KPLATO_SCHEDULERPLUGIN_EXPORT(KPlatoRCPSPlugin)

using namespace KPlato;

KPlatoRCPSPlugin::KPlatoRCPSPlugin( QObject * parent, const QVariantList & )
    : KPlato::SchedulerPlugin(parent)
{
    kDebug()<<rcps_version();
    KLocale *locale = KGlobal::locale();
    if ( locale ) {
        locale->insertCatalog( "kplatorcpsplugin" );
    }
}

KPlatoRCPSPlugin::~KPlatoRCPSPlugin()
{
}

QStringList KPlatoRCPSPlugin::missingFunctions( Project &project, ScheduleManager *sm ) const
{
    bool alap = false;
    bool timeconstraint = false;
    foreach ( const Node *n, project.allNodes() ) {
        if ( n->constraint() == Node::ALAP ) {
            alap = true;
        } else if ( n->constraint() != Node::ASAP ) {
            timeconstraint = true;
        }
    }
    QStringList lst;
    if ( sm->schedulingDirection() ) {
        lst << i18nc( "@item:inlistbox", "Scheduling backwards from target end time is not supported." );
        lst << i18nc( "@item:inlistbox", "The project will be scheduled forward from target start time." );
        lst << QString();
    }
    if ( timeconstraint ) {
        lst << i18nc( "@item:inlistbox", "Scheduling tasks with time constraints is not supported." );
        lst << i18nc( "@item:inlistbox", "Tasks will be scheduled As Soon As Possible (ASAP)." );
        lst << QString();
    }
    if ( alap ) {
        lst << i18nc( "@item:inlistbox", "Scheduling tasks As Late As Possible (ALAP) is not supported." );
        lst << i18nc( "@item:inlistbox", "Tasks will be scheduled As Soon As Possible (ASAP)." );
    }
    return lst;
}

void KPlatoRCPSPlugin::calculate( KPlato::Project &project, KPlato::ScheduleManager *sm, bool nothread )
{
    foreach ( SchedulerThread *j, m_jobs ) {
        if ( j->manager() == sm ) {
            return;
        }
    }
    QStringList lst = missingFunctions( project, sm );
    if ( ! lst.isEmpty() ) {
        int result = KMessageBox::warningContinueCancelList( 0, i18nc( "@info", "<b>This scheduler does not support all the requested scheduling functionality.</b>" ), lst );
        if ( result == KMessageBox::Cancel ) {
            sm->setCalculationResult( KPlato::ScheduleManager::CalculationCanceled );
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

void KPlatoRCPSPlugin::slotStarted( SchedulerThread *job )
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
