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

#ifndef KPLATORCPSPLUGIN_H
#define KPLATORCPSPLUGIN_H

#include "kptschedulerplugin.h"

#include <QVariantList>

class KJob;
class KPlatoRCPSScheduler;

namespace KPlato
{
    class Project;
    class ScheduleManager;
    class Schedule;
}

class KPlatoRCPSPlugin : public KPlato::SchedulerPlugin 
{
    Q_OBJECT

public:
    KPlatoRCPSPlugin( QObject * parent,  const QVariantList & );
    ~KPlatoRCPSPlugin();

    /// Calculate the project
    virtual void calculate( KPlato::Project &project, KPlato::ScheduleManager *sm, bool nothread = false );

    void stopCalculation( KPlato::ScheduleManager *sm );

public slots:
    void stopAllCalculations();
    void stopCalculation( KPlatoRCPSScheduler *sch );

protected slots:
    void slotStarted( KPlatoRCPSScheduler *job );
    void slotFinished( KPlatoRCPSScheduler *job );
    
    void logError( KPlato::Schedule*, QString, int );
    void logWarning( KPlato::Schedule*, QString, int );
    void logInfo( KPlato::Schedule*, QString, int );
    void logDebug( KPlato::Schedule*, QString, int );

private:
    QList<KPlatoRCPSScheduler*> m_jobs;
};


#endif // KPLATORCPSPLUGIN_H
