/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

#include <gobject.h>
#include <kaction.h>


GObject::~GObject() {
}

QDomElement GObject::save(QDomDocument &doc) const {

    // A GObject is saved to a node which is stored inside
    // its parent. That way the "tree" can be built up on
    // loading, again... Each object saves its data and
    // calls the save method of its superclass (e.g. GLine
    // would call GObject). The returned DOM Element has
    // to be saved. On loading, this Element is passed to
    // the XML CTOR of the superclass :)
    QDomElement e=doc.createElement("gobject");
    e.setAttribute("name", m_name);
    e.setAttribute("state", m_state);
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

void GObject::setParent(GObject *parent) {

    if(parent!=this)
	m_parent=parent;
}

GObject::GObject(const QString &name) : m_name(name) {

    m_state=Visible;
    boundingRectDirty=true;
    m_fillStyle=Brush;
}

GObject::GObject(const GObject &rhs) :  m_name(rhs.name()),
    m_state(rhs.state()), boundingRectDirty(true), m_fillStyle(rhs.fillStyle()),
    m_brush(rhs.brush()), m_gradient(rhs.gradient()), m_pen(rhs.pen()) {
}

GObject::GObject(const QDomElement &element) {

    bool ok;

    if(element.hasAttribute("name"))
	m_name=element.attribute("name");
    else
	m_name="no valid name";
    
    m_state=static_cast<State>(element.attribute("state").toInt(&ok));
    if(!ok)
	m_state=Visible;
    
    QDomElement format=element.namedItem("format").toElement();
    if(!format.isNull()) {
	m_fillStyle=static_cast<FillStyle>(format.attribute("fillStyle").toInt(&ok));
	if(!ok)
	    m_fillStyle=Brush;
	
	int tmp=format.attribute("brushStyle").toInt(&ok);
	if(!ok)
	    tmp=0;
	m_brush.setStyle(static_cast<QBrush::BrushStyle>(tmp));
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
