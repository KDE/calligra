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

#ifndef kptterminalnode_h
#define kptterminalnode_h

#include "kptnode.h"

#include <qdom.h>

#include <kdebug.h>


class KPTTerminalNode : public KPTNode {
public:
    /**
     * A terminal node must have an owning node.
     * Make sure that we know what it is.
     * @param owner The KPTNode object that has this as its start/end node.
     */
    KPTTerminalNode(KPTNode* owner) { m_parent = owner; }

    virtual int type() const { return KPTNode::Type_TerminalNode; }


    /* make concrete versions of abstract functions */
    /**
     * The expected Duration is the expected time to complete a Task,
     * Project, etc. For an  individual Task, this will calculate the
     * expected duration by querying  the Distribution of the Task. If the
     * Distribution is a simple RiskNone, the value  will equal the mode
     * Duration, but for other Distributions like RiskHigh, the value will
     * have to be calculated. For a Project or Subproject, the expected
     * Duration is calculated by PERT/CPM.
     */
    virtual KPTDuration *getExpectedDuration()
	{ return const_cast<KPTDuration*>( &KPTDuration::zeroDuration ); }

    virtual uint getExpectedDuration(int)
	{ return 0; }

    /**
     * Instead of using the expected duration, generate a random value
     * using the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    virtual KPTDuration *getRandomDuration()
	{ return const_cast<KPTDuration*>( &KPTDuration::zeroDuration ); }

    /**
     * Calculate the start time, use startTime() for the actually started time.
     */
    virtual KPTDateTime *getStartTime()
	{ return new KPTDateTime(); }

    /**
     * Retrieve the calculated float of this node
     */
    virtual KPTDuration *getFloat()
	{ return const_cast<KPTDuration*>( &KPTDuration::zeroDuration ); }

    /**
     * Load and save
     */
    virtual bool load(QDomElement &e) {
        earliestStart = KPTDateTime::fromString(e.attribute("earlieststart"));
        latestFinish = KPTDateTime::fromString(e.attribute("latestfinish"));
        return true;
    }

    virtual void save(QDomElement &e)  {
        e.setAttribute("earlieststart", earliestStart.toString());
        e.setAttribute("latestfinish", latestFinish.toString());
    }

protected:
    /**
     * A terminal node should not have a start/end node. Perhaps this function
     * should throw an exception.
     * @return Null pointer.
     */
    virtual KPTNode* start_node(){ return 0; }

    /**
     * A terminal node should not have a start/end node. Perhaps this function
     * should throw an exception.
     * @return Null pointer.
     */
    virtual KPTNode* end_node(){ return 0; }

#ifndef NDEBUG
public:
    void printDebug(bool /*children*/, QCString indent)
        {
            indent += "  ";
            kdDebug()<<indent<<" Earliest start: "<<earliestStart.toString()<<endl;
            kdDebug()<<indent<<" Latest finish: "<<earliestStart.toString()<<endl;
        }
#endif

};

#endif
