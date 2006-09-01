/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
 * Boston, MA 02110-1301, USA.
*/
#include "kptrelation.h"

#include "kptnode.h"
#include "kptproject.h"
#include "kptcanvasitem.h"

#include <q3canvas.h>
#include <qdom.h>

#include <kdebug.h>

namespace KPlato
{

Relation::Relation(Node *parent, Node *child, Type type, Duration lag) {
    m_parent=parent;
    m_child=child;
    m_type=type;
    m_lag=lag;
}

Relation::Relation(Node *parent, Node *child, Type type) {
    m_parent=parent;
    m_child=child;
    m_type=type;
    m_lag=Duration();
}

Relation::Relation(Relation *rel) {
    m_parent=rel->parent();
    m_child=rel->child();
    m_type=rel->type();
    m_lag=rel->lag();
}

Relation::~Relation() {
    //kDebug()<<k_funcinfo<<"parent: "<<(m_parent ? m_parent->name():"none")<<" child: "<<(m_child ? m_child->name():"None")<<endl;
    if (m_parent)
        m_parent->takeDependChildNode(this);
    if (m_child)
        m_child->takeDependParentNode(this);
}

void Relation::setType(Type type) {
    m_type=type;
}


bool Relation::load(QDomElement &element, Project &project) {
    m_parent = project.findNode(element.attribute("parent-id"));
    if (m_parent == 0) {
        return false;
    }
    m_child = project.findNode(element.attribute("child-id"));
    if (m_child == 0) {
        return false;
    }
    if (m_child == m_parent) {
        kDebug()<<k_funcinfo<<"child == parent"<<endl;
        return false;
    }
    if (!m_parent->legalToLink(m_child))
        return false;
        
    QString tr = element.attribute("type");
    if ( tr == "Finish-Start" )
        m_type = FinishStart;
    else if ( tr == "Finish-Finish" )
        m_type = FinishFinish;
    else if ( tr == "Start-Start" )
        m_type = StartStart;
    else
        m_type = FinishStart;

    m_lag = Duration::fromString(element.attribute("lag"));

    if (!m_parent->addDependChildNode(this)) {
        kError()<<k_funcinfo<<"Failed to add relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
        return false;
    }
    if (!m_child->addDependParentNode(this)) {
        m_parent->delDependChildNode(this, false/*do not delete*/);
        kError()<<k_funcinfo<<"Failed to add relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
        return false;
    }

    //kDebug()<<k_funcinfo<<"Added relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
    return true;
}


void Relation::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("relation");
    element.appendChild(me);

    me.setAttribute("parent-id", m_parent->id());
    me.setAttribute("child-id", m_child->id());
    QString type = "Finish-Start";
    switch (m_type) {
        case FinishStart:
            type = "Finish-Start";
            break;
        case FinishFinish:
            type = "Finish-Finish";
            break;
        case StartStart:
            type = "Start-Start";
            break;
    }
    me.setAttribute("type", type);
    me.setAttribute("lag", m_lag.toString());
}

#ifndef NDEBUG
void Relation::printDebug(QByteArray indent) {
    indent += "  ";
    kDebug()<<indent<<"  Parent: "<<m_parent->name()<<endl;
    kDebug()<<indent<<"  Child: "<<m_child->name()<<endl;
    kDebug()<<indent<<"  Type: "<<m_type<<endl;
}
#endif

}  //KPlato namespace
