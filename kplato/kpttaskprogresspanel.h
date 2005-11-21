/* This file is part of the KDE project
   Copyright (C) 2004 - 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTTASKPROGRESSPANEL_H
#define KPTTASKPROGRESSPANEL_H

#include "kpttaskprogresspanelbase.h"
#include "kpttask.h"

class KCommand;

namespace KPlato
{

class Part;
class StandardWorktime;

class TaskProgressPanelImpl : public TaskProgressPanelBase {
    Q_OBJECT
public:
    TaskProgressPanelImpl(QWidget *parent=0, const char *name=0, WFlags f=0);
    
    void enableWidgets();

signals:
    void changed();
    
public slots:
    void slotChanged();
    void slotStartedChanged(bool state);
    void slotFinishedChanged(bool state);
    void slotPercentFinishedChanged(int value);
};

class TaskProgressPanel : public TaskProgressPanelImpl {
    Q_OBJECT
public:
    TaskProgressPanel(Task &task, StandardWorktime *workTime=0, QWidget *parent=0, const char *name=0);

    KCommand *buildCommand(Part *part);
    
    bool ok();

protected:
    void setEstimateScales( int day );
    
private:
    Task &m_task;
    int m_dayLength;
    struct Task::Progress m_progress;
};

}  //KPlato namespace

#endif // TASKPROGRESSPANEL_H
