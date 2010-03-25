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
    foreach ( KPlatoRCPSScheduler *s, m_jobs ) {
        s->haltScheduling();
    }
}

void KPlatoRCPSPlugin::calculate( KPlato::Project &project, KPlato::ScheduleManager *sm, bool nothread )
{
//    qDebug()<<"KPlatoRCPSPlugin::calculate:";
    foreach ( KPlatoRCPSScheduler *j, m_jobs ) {
        if ( j->manager() == sm ) {
            return;
        }
    }
    sm->setScheduling( true );
    KPlatoRCPSScheduler *job = new KPlatoRCPSScheduler( &project, sm );
    connect( job, SIGNAL(logError( KPlato::Schedule*, QString, int)), SLOT(logError( KPlato::Schedule*, QString, int)));
    connect( job, SIGNAL(logWarning( KPlato::Schedule*, QString, int)), SLOT(logWarning( KPlato::Schedule*, QString, int)));
    connect( job, SIGNAL(logInfo( KPlato::Schedule*, QString, int)), SLOT(logInfo( KPlato::Schedule*, QString, int)));
    connect( job, SIGNAL(logDebug( KPlato::Schedule*, QString, int)), SLOT(logDebug( KPlato::Schedule*, QString, int)));

    project.changed( sm );
    int result = job->kplatoToRCPS();
    if ( result != 0 ) {
        sm->expected()->logError( i18n( "Failed to build a valid RCPS project: %1", result, 0 ) );
        sm->setCalculationResult( ScheduleManager::CalculationError );
        job->slotFinished();
        return;
    }
    QStringList lst = job->missingFunctions();
    if ( ! lst.isEmpty() ) {
        result = KMessageBox::warningContinueCancelList( 0, i18nc( "@info", "<b>This scheduler does not support all the requested scheduling functionality.</b>" ), lst );
        if ( result == KMessageBox::Cancel ) {
            sm->setCalculationResult( ScheduleManager::CalculationCanceled );
            job->slotFinished();
            return;
        }
    }
    m_jobs << job;
    connect( job, SIGNAL( jobStarted( KPlatoRCPSScheduler* ) ), SLOT( slotStarted( KPlatoRCPSScheduler* ) ) );
    connect( job, SIGNAL( jobFinished( KPlatoRCPSScheduler* ) ), SLOT( slotFinished( KPlatoRCPSScheduler* ) ) );
    
    connect(&project, SIGNAL(nodeToBeRemoved(Node*)), SLOT(stopAllCalculations()));
    connect(&project, SIGNAL(resourceToBeRemoved(const Resource*)), SLOT(stopAllCalculations()));
    connect(&project, SIGNAL(resourceGroupToBeRemoved(const ResourceGroup*)), SLOT(stopAllCalculations()));
    connect(&project, SIGNAL(calendarToBeRemoved(const Calendar*)), SLOT(stopAllCalculations()));
    connect(&project, SIGNAL(nodeToBeRemoved(Node*)), SLOT(stopAllCalculations()));
    connect(&project, SIGNAL(nodeToBeRemoved(Node*)), SLOT(stopAllCalculations()));

    //FIXME
    if ( true /*nothread*/ ) {
        job->doRun();
    } else {
        job->start();
    }
}

void KPlatoRCPSPlugin::logError( KPlato::Schedule *cs, QString s, int phase )
{
    if ( cs ) {
        cs->logError( s, phase );
    }
}

void KPlatoRCPSPlugin::logWarning( KPlato::Schedule *cs, QString s, int phase )
{
    if ( cs ) {
        cs->logWarning( s, phase );
    }
}

void KPlatoRCPSPlugin::logInfo( KPlato::Schedule *cs, QString s, int phase )
{
    if ( cs ) {
        cs->logInfo( s, phase );
    }
}

void KPlatoRCPSPlugin::logDebug( KPlato::Schedule *cs, QString s, int phase )
{
    if ( cs ) {
        cs->logDebug( s, phase );
    }
}

void KPlatoRCPSPlugin::stopAllCalculations()
{
    foreach ( KPlatoRCPSScheduler *s, m_jobs ) {
        stopCalculation( s );
    }
}

void KPlatoRCPSPlugin::stopCalculation( KPlatoRCPSScheduler *sch )
{
    if ( sch ) {
        disconnect( sch, SIGNAL( jobFinished( KPlatoRCPSScheduler* ) ), this, SLOT( slotFinished( KPlatoRCPSScheduler* ) ) );
        connect(this, SIGNAL(stopScheduling()), sch, SLOT(stopScheduling()));
        emit stopScheduling();
        sch->wait( 20000 ); // wait max 20 seconds. FIXME: if it fails, what to do?
        slotFinished( sch );
    }
}

void KPlatoRCPSPlugin::stopCalculation( ScheduleManager *sm )
{
    KPlatoRCPSScheduler *s = 0;
    foreach ( s, m_jobs ) {
        if ( s->manager() == sm ) {
            sm->setCalculationResult( ScheduleManager::CalculationStopped );
            stopCalculation( s );
            break;
        }
    }
}

void KPlatoRCPSPlugin::slotStarted( KPlatoRCPSScheduler *job )
{
//    qDebug()<<"KPlatoRCPSPlugin::slotStarted:";
}

void KPlatoRCPSPlugin::slotFinished( KPlatoRCPSScheduler *job )
{
    //qDebug()<<"KPlatoRCPSPlugin::slotFinished:"<<job->manager()<<job->manager()->calculationResult();
    if ( job->manager()->calculationResult() == ScheduleManager::CalculationRunning ) {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        job->kplatoFromRCPS();
        job->manager()->setCalculationResult( ScheduleManager::CalculationDone );
        QApplication::restoreOverrideCursor();
    }
    m_jobs.removeAt( m_jobs.indexOf( job ) );
    if ( job->manager() ) {
        job->manager()->setScheduling( false );
    }
    job->deleteLater();
}


#include "KPlatoRCPSPlugin.moc"
