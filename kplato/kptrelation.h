/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
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

#ifndef KPTRELATION_H
#define KPTRELATION_H

#include "kptduration.h"

#include <QString>

class Q3Canvas;
class QDomElement;

namespace KPlato
{

class Node;
class Project;
class PertCanvas;

/**
  * The relation class couples a 2 nodes together which are dependent on each other.
  * If for example you have a project to build a house, the node that represents the 
  * adding of the roof is dependent on the node that represents the building of the walls.
  * The roof can't be put up if the walls are not there yet.
  * We actually have a number of relationtypes so this relation can be used in different manners.
  */
class Relation {
public:
    enum Type { None, FinishStart, FinishFinish, StartStart };

    Relation(Node *parent, Node *child, Type type, Duration lag);
    Relation(Node *parent=0, Node *child=0, Type type=FinishStart);
    Relation(Relation *rel);
    
    /** 
    *  When deleted the relation will remove itself from 
    *  the parent- and child nodes lists
    */
    virtual ~Relation();

    void setType(Type );
    Type type() const { return m_type; }

    /** returns the lag.
    *  The lag of a relation is the time it takes between the parent starting/stopping
    *  and the start of the child.
    */
    const Duration &lag() const { return m_lag; }
    void setLag(Duration lag) { m_lag = lag; }

    /**
     * @return The parent dependent node.
     */
    Node *parent() const { return m_parent; }
    /**
     * @return The child dependent node.
     */
    Node *child() const { return m_child; }

    enum Result {
        SUCCESS = 0l,
        HASCHILDREN = 1l,
        NOTIMPL = 2l
    };

    bool load(QDomElement &element, Project &project);
    void save(QDomElement &element) const;
    
protected: // variables
    Node *m_parent;
    Node *m_child;
    Type m_type;
    Duration m_lag;

private:
    QString m_parentId;
    
#ifndef NDEBUG
public:
    void printDebug(QByteArray indent);
#endif

};

class ProxyRelation : public Relation
{
public:
    ProxyRelation(Node *parent, Node *child, Relation::Type type, Duration lag) 
    : Relation(parent, child, type, lag) 
    {}

    ~ProxyRelation() { m_parent = 0; m_child = 0;}
};

}  //KPlato namespace

#endif
