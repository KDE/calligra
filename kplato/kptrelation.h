/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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

#ifndef kptrelation_h
#define kptrelation_h

#include <qdatetime.h> 
#include "kptnode.h"

class KPTRelation {
    public:

        KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt, timingRelation tr, QTime lag);
        KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt=START_ON_DATE, timingRelation tr=FINISH_START);
        ~KPTRelation();

        void timingType(KPTNode::TimingType );
        KPTNode::TimingType timingType() { return m_timingType; }
        void timingRelation(KPTNode::TimingRelation );
        KPTNode::TimingRelation timingRelation() { return m_timingRelation; }

        /** returns the lag.
         *  The lag of a relation is the time it takes between the parent starting/stopping
         *  and the start of the child.
         */
        QTime lag() { return m_lag; }
        
    protected: // variables
        KPTNode *m_parent;
        KPTNode *m_child;
        KPTNode::TimingType m_timingTime;
        KPTNode::TimingRelation m_timingRelation;
        QTime m_lag;
};
#endif
