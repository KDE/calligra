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
#include <kptduration.h>

int main( int /*argc*/, char /***argv */) {
    // new project
    KPTProject *p = new KPTProject();

    // new task with name Task A
    // this task does not have any duration assigned to it, that be calculated from
    // its subtasks.
    KPTTask *ta = new KPTTask();
    ta->setName("Task A");
    p->addChildNode(ta);
    kdDebug() << "Creating Task A\n";
    kdDebug() << "  Project\n";
    kdDebug() << "  +-- > Task A\n";

    // new task with name Task B
    // Effort initialized in one line
    KPTTask *tb = new KPTTask();
    tb->setName("Task B");
    p->addChildNode(tb);
    tb->setEffort( new KPTEffort(KPTDuration(2,0)));
    kdDebug() << "\n";
    kdDebug() << "Added task B with effort of 2 hours \n";
    kdDebug() << "  Project\n";
    kdDebug() << "  +-- > Task A\n";
    kdDebug() << "  +-- > Task B\n";
    kdDebug() << "          -> "<< tb->getExpectedDuration()->toString() << endl;

    // new subtask, we let it be a subtask of taskA, and name it Subtask A1
    KPTTask *ta1 = new KPTTask();
    ta1->setEffort(new KPTEffort(KPTDuration(1,0)));
    ta1->setName("Subtask A1");
    ta->addChildNode(ta1);
    kdDebug() << "\n";
    kdDebug() << "Added subtask A1 with effort 1 hour\n";
    kdDebug() << "  Project\n";
    kdDebug() << "  +-- > Task A\n";
    kdDebug() << "  |       -> "<< ta->getExpectedDuration()->toString() << " (implied)" << endl;
    kdDebug() << "  |       +-- > Task A1\n";
    kdDebug() << "  |               -> "<< ta1->getExpectedDuration()->toString() << endl;
    kdDebug() << "  +-- > Task B\n";
    kdDebug() << "          -> "<< tb->getExpectedDuration()->toString() << endl;

    // Make task B dependent on the finish of subtask A1, and make it start 1 hour 45 minutes after 
    // Subtask A1 has finished.
    tb->addDependChildNode(ta1, START_ON_DATE, FINISH_START, KPTDuration(1,45));
    kdDebug() << "\n";
    kdDebug() << "Setting Task B to start 1h45m after A finished" << endl;
    kdDebug() << "  Task A1 -- Task B" << endl;

    // How long is the project suppost to be running?
    // Task A1 takes 1 hour
    // Task A therefor also takes 1 hour (only one subtask)
    // After task A1 finishes we wait for 1 3/4 hour and then start task B
    // Task B takes 2 hours
    // Total: 4 hour 45 minutes.

    kdDebug() << "\n";
    kdDebug() << "Total running time: " << p->getExpectedDuration()->toString() << endl;
     
    return 0;
}
