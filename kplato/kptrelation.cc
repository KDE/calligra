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
#include "kptrelation.h"
#include "defs.h"

#include "kptnode.h"
#include "kptproject.h"
#include "kptcanvasitem.h"

#include <qcanvas.h>
#include <qdom.h>

#include <kdebug.h>

KPTRelation::KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt, TimingRelation tr, KPTDuration lag) {
    m_parent=parent;
    m_child=child;
    m_timingType=tt;
    m_timingRelation=tr;
    m_lag=lag;
}

KPTRelation::KPTRelation(KPTNode *parent, KPTNode *child, TimingType tt, TimingRelation tr) {
    m_parent=parent;
    m_child=child;
    m_timingType=tt;
    m_timingRelation=tr;
    m_lag=KPTDuration();
}

KPTRelation::~KPTRelation() {
}

void KPTRelation::setTimingType(TimingType tt) {
    m_timingType=tt;
}
void KPTRelation::setTimingRelation(TimingRelation tr) {
    m_timingRelation=tr;
}


bool KPTRelation::load(QDomElement &element, KPTProject &project) {
    bool ok = false;
    int id;
    id = element.attribute("parent-id","-1").toInt(&ok);
    if (id == -1 || !(m_parent = project.node(id)))
        return false;
    id = element.attribute("child-id","-1").toInt(&ok);
    if (id == -1 || !(m_child = project.node(id)))
        return false;

    //m_timingType = element.attribute("timingtype");
    QString tr = element.attribute("timingrelation");
    if ( tr == "Finish-Start" )
        m_timingRelation = FINISH_START;
    else if ( tr == "Finish-Finish" )
        m_timingRelation = FINISH_FINISH;
    else if ( tr == "Start-Start" )
        m_timingRelation = START_START;
    else
        m_timingRelation = FINISH_START;

    m_lag = KPTDuration(); //m_lag.set( KPTDuration(QDateTime::fromString(element.attribute("lag"))) );

    if (!m_parent->addDependChildNode(this)) {
        kdError()<<k_funcinfo<<"Failed to add relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
        return false;
    }
    if (!m_child->addDependParentNode(this)) {
        m_parent->delDependChildNode(this, false/*do not delete*/);
        kdError()<<k_funcinfo<<"Failed to add relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
        return false;
    }

    kdDebug()<<k_funcinfo<<"Added relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
    return true;
}


void KPTRelation::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("relation");
    element.appendChild(me);

    me.setAttribute("parent-id", m_parent->id());
    me.setAttribute("child-id", m_child->id());
    //me.setAttribute("timingtype", m_timingType);
    QString tr = "Finish-Start";
    switch (m_timingRelation) {
        case FINISH_START:
            tr = "Finish-Start";
            break;
        case FINISH_FINISH:
            tr = "Finish-Finish";
            break;
        case START_START:
            tr = "Start-Start";
            break;
    }
    me.setAttribute("timingrelation", tr);
    //me.setAttribute("lag", m_lag.toString());
}

#ifndef NDEBUG
void KPTRelation::printDebug(QCString indent) {
    indent += "  ";
    kdDebug()<<indent<<"  Parent: "<<m_parent->name()<<endl;
    kdDebug()<<indent<<"  Child: "<<m_child->name()<<endl;
    kdDebug()<<indent<<"  Timing type: "<<m_timingType<<endl;
    kdDebug()<<indent<<"  Relation type: "<<m_timingRelation<<endl;
}
#endif

