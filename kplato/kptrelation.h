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

#include "kptduration.h"
#include "defs.h"

#include <qstring.h>

class KPTNode;
class KPTProject;
class KPTPertCanvas;
class QCanvas;
class QDomElement;

/**
  * The relation class couples a 2 nodes together which are dependent on each other.
  * If for example you have a project to build a house, the node that represents the 
  * adding of the roof is dependent on the node that represents the building of the walls.
  * The roof can't be put up if the walls are not there yet.
  * We actually have a number of TimingRelations so this relation can be used in different manners.
  */
class KPTRelation {
public:

    KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt, TimingRelation tr, KPTDuration lag);
    KPTRelation(KPTNode *parent=0, KPTNode *child=0, TimingType tt=START_ON_DATE, TimingRelation tr=FINISH_START);
    
    /** 
    *  When deleted the relation will remove itself from 
    *  the parent- and child nodes lists
    */
    ~KPTRelation();

    void setTimingType(TimingType );
    TimingType timingType() const { return m_timingType; }
    void setTimingRelation(TimingRelation );
    TimingRelation timingRelation() const { return m_timingRelation; }

    /** returns the lag.
    *  The lag of a relation is the time it takes between the parent starting/stopping
    *  and the start of the child.
    */
    const KPTDuration &lag() const { return m_lag; }

    /**
     * @return The parent dependent node.
     */
    KPTNode *parent() const { return m_parent; }
    /**
     * @return The child dependent node.
     */
    KPTNode *child() const { return m_child; }

    enum Result {
        SUCCESS = 0l,
        HASCHILDREN = 1l,
        NOTIMPL = 2l
    };

    bool load(QDomElement &element, KPTProject &project);
    void save(QDomElement &element) const;
    
protected: // variables
    KPTNode *m_parent;
    KPTNode *m_child;
    TimingType m_timingType;
    TimingRelation m_timingRelation;
    KPTDuration m_lag;

private:
    QString m_parentId;
    
#ifndef NDEBUG
public:
    void printDebug(QCString indent);
#endif

};
#endif
