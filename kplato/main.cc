/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "defs.h"
#include "kptproject.h"
#include "kpttask.h"

#include <kdebug.h>
#include <qdatetime.h>

int main( int /*argc*/, char /***argv */) {
    // new project
    KPTProject *p = new KPTProject();

    // new task with name Task A
    // this task does not have any duration assigned to it, that will have to be calculated from
    // its subtasks.
    KPTTask *ta = new KPTTask();
    ta->setName("Task A");
    p->addChildNode(ta);

    // new task with name Task B
    // Effort initialized in one line
    KPTTask *tb = new KPTTask();
    ta->addEffort( new KPTEffort(QDateTime(*(new QDate(1,0,0)), *(new QTime(2,0,0)))));
    tb->setName("Task B");
    p->addChildNode(tb);

    // new subtask, we let it be a subtask of taskA, and name it Subtask A1
    // effort initialized the easy to read way
    KPTTask *ta1 = new KPTTask();
    ta1->addEffort(new KPTEffort);
    ta1->effort()->expected() = QDateTime(*(new QDate(1,0,0)),*( new QTime(1,0,0)));
    ta1->setName("Subtask A1");
    p->addChildNode(ta1);

    // Make task B dependent on the finish of subtask A1, and make it start 1 hour 45 minutes after 
    // Subtask A1 has finished.
    tb->addDependChildNode(ta1, START_ON_DATE, FINISH_START, new QDateTime(*(new QDate(1,0,0)), *(new QTime(1,45))));


    // How long is the project suppost to be running?
    // Task A1 takes 1 hour
    // Task A therefor also takes 1 hour (only one subtask)
    // After task A1 finishes we wait for 1 3/4 hour and then start task B
    // Task B takes 2 hours
    // Total: 4 hour 45 minutes.

    kdDebug() << "Total running time: " << p->getExpectedDuration() << endl;
     
    return 0;
}
