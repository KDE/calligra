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

#ifndef KPLATOWORK_TASKCOMPLETIONDIALOG_H
#define KPLATOWORK_TASKCOMPLETIONDIALOG_H

#include "kplatowork_export.h"
#include "ui_taskcompletionpanel.h"

#include "workpackage.h"

#include <KDialog>
#include <klocale.h>
#include <kdebug.h>

#include <QWidget>

class QUndoCommand;

namespace KPlato {
    class ScheduleManager;
}

namespace KPlatoWork
{

class TaskCompletionPanel;

class KPLATOWORK_EXPORT TaskCompletionDialog : public KDialog
{
    Q_OBJECT
public:
    explicit TaskCompletionDialog( WorkPackage &p, ScheduleManager *sm, QWidget *parent=0 );

    QUndoCommand *buildCommand();

protected slots:
    void slotChanged( bool );

private:
    TaskCompletionPanel *m_panel;
};

class KPLATOWORK_EXPORT TaskCompletionPanel : public QWidget, public Ui::TaskCompletionPanel
{
    Q_OBJECT
public:
    explicit TaskCompletionPanel( WorkPackage &p, ScheduleManager *sm, QWidget *parent=0 );

    QUndoCommand *buildCommand();

    void enableWidgets();

signals:
    void changed( bool );

public slots:
    void slotChanged();
    void optionChanged( int id );
    void slotStartedChanged(bool state);
    void slotFinishedChanged(bool state);
    void slotPercentFinishedChanged(int value);
    void slotStartTimeChanged( const QDateTime &dt );
    void slotFinishTimeChanged( const QDateTime &dt );
    void slotEntryChanged();
    void slotSelectionChanged( const QItemSelection &sel );

    void slotEntryAdded( const QDate date );

    void slotEditmodeChanged( int );

protected slots:
    void slotCalculateEffort();
    
protected:
    void setFinished();

    WorkPackage *m_package;
    Completion m_completion;
    int m_dayLength;
    
    Duration scheduledEffort;
};

}  //KPlatoWork namespace


#endif
