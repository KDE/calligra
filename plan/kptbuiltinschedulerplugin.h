/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTBUILTINSCHEDULERPLUGIN_H
#define KPTBUILTINSCHEDULERPLUGIN_H

#include "kplato_export.h"
#include "kptschedulerplugin.h"

#include "kptschedule.h"

#include <KoXmlReader.h>

#include <QThread>
#include <QMutex>
#include <QTimer>


class KLocale;


namespace KPlato
{

class KPlatoScheduler;
class Project;
class ScheduleManager;
class Node;
class XMLLoaderObject;

class KPLATO_EXPORT BuiltinSchedulerPlugin : public SchedulerPlugin
{
    Q_OBJECT
public:
    BuiltinSchedulerPlugin(QObject *parent);
    virtual ~BuiltinSchedulerPlugin();

    virtual QString description() const;
    /// Calculate the project
    virtual void calculate( Project &project, ScheduleManager *sm, bool nothread = false );

signals:
    void sigCalculationStarted( Project*, ScheduleManager* );
    void sigCalculationFinished( Project*, ScheduleManager* );
    void maxProgress( int, ScheduleManager* );
    void sigProgress( int, ScheduleManager* );

protected slots:
    void slotStarted( SchedulerThread *job );
    void slotFinished( SchedulerThread *job );
};


class KPlatoScheduler : public SchedulerThread
{
    Q_OBJECT

public:
    KPlatoScheduler( Project *project, ScheduleManager *sm, QObject *parent = 0 );
    ~KPlatoScheduler();

    KLocale *locale() const;

public slots:
    /// Stop scheduling.
    virtual void stopScheduling();
    /// Halt scheduling
    virtual void haltScheduling() { m_haltScheduling = true; stopScheduling(); }

protected:
    void run();

};

} //namespace KPlato

#endif
