/* This file is part of the KDE project
  Copyright (C) 2009, 2010 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTSCHEDULERPLUGIN_H
#define KPTSCHEDULERPLUGIN_H

#include "kplatokernel_export.h"

#include "kptschedule.h"

#include "KoXmlReader.h"

#include <QObject>
#include <QString>
#include <QMutex>
#include <QThread>
#include <QTimer>

class QDomDocument;
class KLocale;

namespace KPlato
{

class SchedulerThread;
class Project;
class ScheduleManager;
class Node;
class XMLLoaderObject;

/**
 SchedulerPlugin is the base class for project calculation plugins.

 Sub-class SchedulerThread to do the actual calculation, then re-implement calculate()
 to calculate the project, and slotFinished() to fetch the result into your project.
 
 There is two ways to show progress:
 <ul>
 <li> Connect the SchedulerThread::maxProgressChanged() to ScheduleManager::setMaxProgress() and
      and SchedulerThread::progressChanged() to ScheduleManager::setProgress().
      Note that too many progress signals too often may choke the ui thread.
 <li> Start the m_synctimer. This will fetch progress and log messages every 500 ms (by default).
 </ul>

 When the thread has finished scheduling, data can be fetched from its temporary project
 into the real project by calling the updateProject() method.
*/
class KPLATOKERNEL_EXPORT SchedulerPlugin : public QObject
{
    Q_OBJECT
public:
    SchedulerPlugin(QObject *parent);
    virtual ~SchedulerPlugin();

    /// Localized name
    QString name() const;
    /// Name is normally set by the plugin loader, from Name in the desktop file
    void setName( const QString &name );
    /// Localized comment
    QString comment() const;
    /// Comment is normally set by the plugin loader, from Comment in the desktop file
    void setComment( const QString &name );
    /// A more elaborate description suitable for use in what's this
    virtual QString description() const { return QString(); }
    /// The schedulers capabilities
    enum Capabilities {
        AvoidOverbooking = 1,
        AllowOverbooking = 2,
        ScheduleForward = 4,
        ScheduleBackward = 8
    };
    /// Return the schedulers capabilities.
    /// By default returns all capabilities
    virtual int capabilities() const;
    /// Stop calculation of the schedule @p sm. Current result may be used.
    void stopCalculation( ScheduleManager *sm );
    /// Terminate calculation of the schedule @p sm. No results will be available.
    void haltCalculation( ScheduleManager *sm );
    
    /// Stop calculation of the scheduling @p job. Current result may be used.
    virtual void stopCalculation( SchedulerThread *job );
    /// Terminate calculation of the scheduling @p job. No results will be available.
    virtual void haltCalculation( SchedulerThread *job );
    
    /// Calculate the project
    virtual void calculate( Project &project, ScheduleManager *sm, bool nothread = false ) = 0;

protected slots:
    virtual void slotSyncData();

protected:
    void updateProject( const Project *tp, const ScheduleManager *tm, Project *mp, ScheduleManager *sm ) const;
    void updateNode( const Node *tn, Node *mn, long sid, XMLLoaderObject &status ) const;
    void updateResource( const KPlato::Resource *tr, Resource *r, XMLLoaderObject &status ) const;
    void updateAppointments( const Project *tp, const ScheduleManager *tm, Project *mp, ScheduleManager *sm, XMLLoaderObject &status ) const;

    void updateProgress();
    void updateLog();
    void updateLog( SchedulerThread *job );

protected:
    QTimer m_synctimer;
    QList<SchedulerThread*> m_jobs;

private:
    class Private;
    Private *d;
};

/**
 SchedulerThread is a basic class used to implement project calculation in a separate thread.
 The scheduling thread is meant to run on a private copy of the project to avoid that the ui thread
 changes the data while calculations are going on.
 
 The constructor creates a KoXmlDocument m_pdoc of the project that can be used to
 create a private project. This should be done in the reimplemented run() method.
 
 When the calculations are done the signal jobFinished() is emitted. This can be used to
 fetch data from the private calculated project into the actual project.
 
 To track progress, the progress() method should be called from the ui thread with
 an appropriate interval to avoid overload of the ui thread.
 The progressChanged() signal may also be used but note that async signal handling are very slow
 so it may affect the ui threads performance too much.
*/
class KPLATOKERNEL_EXPORT SchedulerThread : public QThread
{
    Q_OBJECT
public:
    SchedulerThread( Project *project, ScheduleManager *manager, QObject *parent );
    ~SchedulerThread();

    Project *mainProject() const { return m_mainproject; }
    ScheduleManager *mainManager() const { return m_mainmanager; }
    
    Project *project() const;
    ScheduleManager *manager() const;

    /// Run with no thread
    void doRun();
    
    /// The scheduling is stopping
    bool isStopped() const { return m_stopScheduling; }
    /// The scheduling is halting
    bool isHalted() const { return m_haltScheduling; }

    int maxProgress() const;
    int progress() const;
    QList<Schedule::Log> log();

    QMap<int, QString> phaseNames() const;

    /// Save the @p project into @p document
    static void saveProject( Project *project, QDomDocument &document );
    /// Load the @p project from @p document
    static bool loadProject( Project *project, const KoXmlDocument &document );

    ///Add a scheduling error log message
    void logError( Node *n, Resource *r, const QString &msg, int phase = -1 );
    ///Add a scheduling warning log message
    void logWarning( Node *n, Resource *r, const QString &msg, int phase = -1 );
    ///Add a scheduling information log message
    void logInfo( Node *n, Resource *r, const QString &msg, int phase = -1 );
    ///Add a scheduling debug log message
    void logDebug( Node *n, Resource *r, const QString &msg, int phase = -1 );

signals:
    /// Job has started
    void jobStarted( SchedulerThread *job );
    /// Job is finished
    void jobFinished( SchedulerThread *job );

    /// Maximum progress value has changed
    void maxProgressChanged( int value, ScheduleManager *sm = 0 );
    /// Progress has changed
    void progressChanged( int value, ScheduleManager *sm = 0 );

public slots:
    /// Stop scheduling. Result may still be used.
    virtual void stopScheduling();
    /// Halt scheduling. Discard result.
    virtual void haltScheduling();


protected slots:
    virtual void slotStarted();
    virtual void slotFinished();

    void setMaxProgress( int );
    void setProgress( int );

    void slotAddLog( Schedule::Log log );

protected:
    /// Re-implement to do the job
    virtual void run() {}

protected:
    /// The actual project to be calculated. Not accessed outside constructor.
    Project *m_mainproject;
    /// The actual schedule manager to be calculated. Not accessed outside constructor.
    ScheduleManager *m_mainmanager;
    /// The schedule manager identity
    QString m_mainmanagerId;
    
    /// The temporary project
    Project *m_project;
    mutable QMutex m_projectMutex;
    /// The temporary schedule manager
    ScheduleManager *m_manager;
    mutable QMutex m_managerMutex;

    bool m_stopScheduling; /// Stop asap, preliminary result may be used
    bool m_haltScheduling; /// Stop and discrad result. Delete yourself.
    
    KoXmlDocument m_pdoc;

    int m_maxprogress;
    mutable QMutex m_maxprogressMutex;
    int m_progress;
    mutable QMutex m_progressMutex;
    QList<Schedule::Log> m_logs;
    mutable QMutex m_logMutex;
};

} //namespace KPlato

#endif
