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

#include "kpttask.h"

#include <kdebug.h>

KPTTask::KPTTask() : KPTNode(), m_resource() {
    m_resource.setAutoDelete(true);
    m_nodeType=TASK;
}

KPTTask::~KPTTask() {
}

KPTDuration *KPTTask::getExpectedDuration() {
    // I have no idea if this is correct...
    KPTDuration *ed= new KPTDuration();
    if(m_effort) {
        ed->add(m_effort->expected());
    } else {
        QListIterator<KPTNode> it(m_nodes); // iterator for employee list
        for ( ; it.current(); ++it ) {
            KPTNode *node = it.current();
            KPTDuration *childDuration = node->getExpectedDuration();
            ed->add(*childDuration);
            delete childDuration;
        } 
    }
    return ed;
}

KPTDuration *KPTTask::getRandomDuration() {
    return 0L;
}

KPTDuration *KPTTask::getStartTime() {
    if(m_startTime == KPTDuration()) {
        // starttime not set, ask our parents!
        // TODO
        return 0L;
    } else {
        return new KPTDuration(m_startTime);
    }
}

KPTDuration *KPTTask::getFloat() {
    return 0L;
}

void KPTTask::addResource( KPTResourceGroup *resource ) {
}

void KPTTask::removeResource( KPTResourceGroup *resource ){
   // always auto remove
}

void KPTTask::removeResource( int number ){
   // always auto remove
}

void KPTTask::insertResource( unsigned int index, KPTResourceGroup *resource ) {
}

