/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

#include <qdom.h>

#include <kgobject.h>
#include <kggroup.h>
#include <kaction.h>
#include <kgobjectpool.h>
#include <kggrouppool.h>


KGObject::~KGObject() {

    if(m_group)
	m_group->removeMember(this);
    if(tmpGroup)
	tmpGroup->removeMember(this);
}

QDomElement KGObject::save(QDomDocument &doc) const {

    QDomElement e=doc.createElement("kgobject");
    e.setAttribute("name", m_name);
    e.setAttribute("state", m_state);
    if(m_group || tmpGroup) {
	QDomElement groups=doc.createElement("groups");
	if(m_group)
	    groups.setAttribute("group", m_group->id());
	if(tmpGroup)
	    groups.setAttribute("tmpGroup", tmpGroup->id());
	e.appendChild(groups);
    }
    QDomElement format=doc.createElement("format");
    format.setAttribute("fillStyle", m_fillStyle);
    format.setAttribute("brushStyle", m_brush.style());
    format.setAttribute("brushColor", m_brush.color().name());
    QDomElement gradient=doc.createElement("gradient");
    gradient.setAttribute("colorA", m_gradient.ca.name());
    gradient.setAttribute("colorB", m_gradient.cb.name());
    gradient.setAttribute("type", m_gradient.type);
    gradient.setAttribute("xfactor", m_gradient.xfactor);
    gradient.setAttribute("yfactor", m_gradient.yfactor);
    gradient.setAttribute("ncols", m_gradient.ncols);
    format.appendChild(gradient);
    format.appendChild(doc.createElement("pen", m_pen));
    e.appendChild(format);		
    return e;
}

const bool KGObject::setTemporaryGroup(KGGroup *group) {

    if(group==m_group)
	return false;
    group->addMember(this);
    tmpGroup=group;
    return true;
}

const bool KGObject::setGroup(KGGroup *group) {

    if(group==tmpGroup)
	return false;
    group->addMember(this);
    m_group=group;
    return true;
}

KGObject::KGObject(const KGObjectPool * const pool, const QString &name) :
    QObject(0L, name.local8Bit()), m_pool(pool), m_name(name) {

    m_state=Visible;
    tmpGroup=0L;
    m_group=0L;
    boundingRectDirty=true;
    popup=0L;
    m_fillStyle=Brush;
}

KGObject::KGObject(const KGObject &rhs) :
    QObject(0L, rhs.name().local8Bit()), m_pool(rhs.pool()), m_name(rhs.name()),
    m_state(rhs.state()), tmpGroup(const_cast<KGGroup*>(rhs.temporaryGroup())),
    m_group(const_cast<KGGroup*>(rhs.group())), boundingRectDirty(true),
    m_fillStyle(rhs.fillStyle()), m_brush(rhs.brush()), m_gradient(rhs.gradient()),
    m_pen(rhs.pen()) {

    popup=new KActionCollection(*rhs.popupActions());
}

KGObject::KGObject(const KGObjectPool * const pool, const QDomElement &element) :
    QObject(), m_pool(pool) {

    bool ok;

    if(element.hasAttribute("name"))
	m_name=element.attribute("name");
    else
	m_name="no valid name";
    QObject::setName(m_name.local8Bit());

    m_state=static_cast<State>(element.attribute("state").toInt(&ok));
    if(!ok)
	m_state=Visible;

    QDomElement groups=element.namedItem("groups").toElement();
    if(!groups.isNull()) {
	int id=groups.attribute("group").toInt(&ok);
	if(!ok)
	    m_group=0L;
	else
	    m_group=m_pool->groupPool()->find(id);  // wow - this is hacky :)
	
	id=groups.attribute("tmpGroup").toInt(&ok);
	if(!ok)
	    tmpGroup=0L;
	else
	    tmpGroup=m_pool->groupPool()->find(id);  // wow - this is hacky :)
    }
    else {
	tmpGroup=0L;
	m_group=0L;
    }

    QDomElement format=element.namedItem("format").toElement();
    if(!format.isNull()) {
	m_fillStyle=static_cast<FillStyle>(format.attribute("fillStyle").toInt(&ok));
	if(!ok)
	    m_fillStyle=Brush;
	
	int tmp=format.attribute("brushStyle").toInt(&ok);
	if(!ok)
	    tmp=0;
	m_brush.setStyle(static_cast<BrushStyle>(tmp));
	if(format.hasAttribute("brushColor"))
	    m_brush.setColor(QColor(format.attribute("brushColor")));
	
	QDomElement gradient=format.namedItem("gradient").toElement();
	if(!gradient.isNull()) {
	    if(gradient.hasAttribute("colorA"))
		m_gradient.ca=QColor(gradient.attribute("colorA"));
	    if(gradient.hasAttribute("colorB"))
		m_gradient.cb=QColor(gradient.attribute("colorB"));
	
	    m_gradient.type=static_cast<KImageEffect::GradientType>(gradient.attribute("type").toInt(&ok));
	    if(!ok)
		m_gradient.type=static_cast<KImageEffect::GradientType>(0);
	
	    m_gradient.xfactor=gradient.attribute("xfactor").toInt(&ok);
	    if(!ok)
		m_gradient.xfactor=1;
	    m_gradient.yfactor=gradient.attribute("yfactor").toInt(&ok);
	    if(!ok)
		m_gradient.yfactor=1;
	    m_gradient.ncols=gradient.attribute("ncols").toInt(&ok);
	    if(!ok)
		m_gradient.ncols=1;	
	}
	else {
	    m_gradient.type=static_cast<KImageEffect::GradientType>(0);
	    m_gradient.xfactor=1;
	    m_gradient.yfactor=1;
	    m_gradient.ncols=1;
	}
	
	QDomElement pen=format.namedItem("pen").toElement();
	if(!pen.isNull())
	    m_pen=pen.toPen();	
    }
    else {
	m_fillStyle=Brush;
	m_gradient.type=static_cast<KImageEffect::GradientType>(0);
	m_gradient.xfactor=1;
	m_gradient.yfactor=1;
	m_gradient.ncols=1;	
    }	
}
#include <kgobject.moc>
