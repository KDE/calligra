/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2004, 2012 Dag Andersen <danders@get2net.dk>

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
#include "kptdebug.h"

#include <QDomDocument>
#include <QStringList>


namespace KPlato
{

Relation::Relation(Node *parent, Node *child, Type type, Duration lag) {
    m_parent=parent;
    m_child=child;
    m_type=type;
    m_lag=lag;
    //kDebug(planDbg())<<this;
}

Relation::Relation(Node *parent, Node *child, Type type) {
    m_parent=parent;
    m_child=child;
    m_type=type;
    m_lag=Duration();
    //kDebug(planDbg())<<this;
}

Relation::Relation(Relation *rel) {
    m_parent=rel->parent();
    m_child=rel->child();
    m_type=rel->type();
    m_lag=rel->lag();
    //kDebug(planDbg())<<this;
}

Relation::~Relation() {
    //kDebug(planDbg())<<"("<<this<<") parent:"<<(m_parent ? m_parent->name():"none")<<" child:"<<(m_child ? m_child->name():"None");
    if (m_parent)
        m_parent->takeDependChildNode(this);
    if (m_child)
        m_child->takeDependParentNode(this);
}

void Relation::setType(Type type) {
    m_type=type;
}

void Relation::setType( const QString &type )
{
    int t = typeList().indexOf( type );
    if ( t == -1 ) {
        t = FinishStart;
    }
    m_type = static_cast<Type>( t );
}

QString Relation::typeToString( bool trans ) const
{
    return typeList( trans ).at( m_type );
}

QStringList Relation::typeList( bool trans )
{
    //NOTE: must match enum
    QStringList lst;
    lst << ( trans ? i18n( "Finish-Start" ) : "Finish-Start" );
    lst << ( trans ? i18n( "Finish-Finish" ) : "Finish-Finish" );
    lst << ( trans ? i18n( "Start-Start" ) : "Start-Start" );
    return lst;
}

void Relation::setParent( Node* node )
{
    Q_ASSERT( ! m_parent );
    m_parent = node;
}

void Relation::setChild( Node* node )
{
    Q_ASSERT( ! m_child );
    m_child = node;
}


bool Relation::load(KoXmlElement &element, Project &project) {
    m_parent = project.findNode(element.attribute("parent-id"));
    if (m_parent == 0) {
        return false;
    }
    m_child = project.findNode(element.attribute("child-id"));
    if (m_child == 0) {
        return false;
    }
    if (m_child == m_parent) {
        kDebug(planDbg())<<"child == parent";
        return false;
    }
    if (m_child == m_parent) {
        kDebug(planDbg())<<"child == parent";
        return false;
    }
    if (!m_parent->legalToLink(m_child))
        return false;
        
    setType( element.attribute("type") );

    m_lag = Duration::fromString(element.attribute("lag"));

    if (!m_parent->addDependChildNode(this)) {
        kError()<<"Failed to add relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
        return false;
    }
    if (!m_child->addDependParentNode(this)) {
        m_parent->takeDependChildNode(this);
        kError()<<"Failed to add relation: Child="<<m_child->name()<<" parent="<<m_parent->name()<<endl;
        return false;
    }

    //kDebug(planDbg())<<"Added relation: Child="<<m_child->name()<<" parent="<<m_parent->name();
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
        default:
            break;
    }
    me.setAttribute("type", type);
    me.setAttribute("lag", m_lag.toString());
}

#ifndef NDEBUG
void Relation::printDebug(const QByteArray& _indent) { 
    QString indent = _indent;
    indent += "  ";
    kDebug(planDbg())<<indent<<"  Parent:"<<m_parent->name();
    kDebug(planDbg())<<indent<<"  Child:"<<m_child->name();
    kDebug(planDbg())<<indent<<"  Type:"<<m_type;
}
#endif


}  //KPlato namespace

QDebug operator<<( QDebug dbg, const KPlato::Relation *r )
{
    return dbg<<(*r);
}

QDebug operator<<( QDebug dbg, const KPlato::Relation &r )
{
    KPlato::Node *parent = r.parent();
    KPlato::Node *child = r.child();
    QString type = "FS";
    switch ( r.type() ) {
    case KPlato::Relation::StartStart: type = "SS"; break;
    case KPlato::Relation::FinishFinish: type = "FF"; break;
    default: break;
    }

    KPlato::Duration lag = r.lag();
    dbg<<"Relation["<<parent->name()<<"->"<<child->name()<<type;
    if ( lag != 0 ) {
        dbg<<lag.toString( KPlato::Duration::Format_HourFraction );
    }
    dbg <<']';
    return dbg;
}
