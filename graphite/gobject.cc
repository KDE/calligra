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

#include <kdialogbase.h>
#include <klocale.h>
#include <kiconloader.h>

#include <gobject.h>
#include <graphitefactory.h>


KDialogBase *GObjectM9r::createPropertyDialog(QWidget *parent) {
    return new KDialogBase(KDialogBase::IconList,
			   i18n("Change Properties"),
			   KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel,
			   KDialogBase::Ok, parent, "property dia", true, true);
}

KDialogBase *G1DObjectM9r::createPropertyDialog(QWidget *parent) {

    KDialogBase *dia=GObjectM9r::createPropertyDialog(parent);
    if(!dia)
	return 0L;

    // TODO - add "pen" page and connect the SLOTs...
    /*QFrame *frame=*/dia->addPage(i18n("Pen"), i18n("Pen Settings"),
			       BarIcon("exec", GraphiteFactory::global()));
    return dia;
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
    e.setAttribute("angle", m_angle);
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

    if(parent!=this)   // it's illegal to be oneselves parent! (parent==0L -> no parent :)
	m_parent=parent;
}

GObject::GObject(const QString &name) : m_name(name) {

    m_state=Visible;
    m_parent=0L;
    m_zoom=100;
    m_angle=0.0;
    m_boundingRectDirty=true;
    m_fillStyle=Brush;
    m_ok=true;
}

GObject::GObject(const GObject &rhs) :  m_name(rhs.name()),
    m_state(rhs.state()), m_parent(0L), m_zoom(rhs.zoom()), m_angle(rhs.angle()),
    m_boundingRectDirty(true), m_fillStyle(rhs.fillStyle()), m_brush(rhs.brush()),
    m_gradient(rhs.gradient()), m_pen(rhs.pen()), m_ok(true) {
}

GObject::GObject(const QDomElement &element) : m_parent(0L), m_zoom(100),
					       m_boundingRectDirty(true), m_ok(false) {

    if(element.tagName()!="gobject")
	return;

    bool ok;
    static QString tagName=QString::fromLatin1("name");
    static QString tagState=QString::fromLatin1("state");
    static QString tagAngle=QString::fromLatin1("angle");
    static QString tagFormat=QString::fromLatin1("format");
    static QString tagFillStyle=QString::fromLatin1("fillStyle");
    static QString tagBrushStyle=QString::fromLatin1("brushStyle");
    static QString tagBrushColor=QString::fromLatin1("brushColor");
    static QString tagGradient=QString::fromLatin1("gradient");
    static QString tagColorA=QString::fromLatin1("colorA");
    static QString tagColorB=QString::fromLatin1("colorB");
    static QString tagType=QString::fromLatin1("type");
    static QString tagXFactor=QString::fromLatin1("xfactor");
    static QString tagYFactor=QString::fromLatin1("yfactor");
    static QString tagNCols=QString::fromLatin1("ncols");
    static QString tagPen=QString::fromLatin1("pen");

    if(element.hasAttribute(tagName))
	m_name=element.attribute(tagName);
    else
	m_name="no name";

    m_state=static_cast<State>(element.attribute(tagState).toInt(&ok));
    if(!ok)
	m_state=Visible;

    m_angle=element.attribute(tagAngle).toDouble(&ok);
    if(!ok)
	m_angle=0.0;

    QDomElement format=element.namedItem(tagFormat).toElement();
    if(!format.isNull()) {
	m_fillStyle=static_cast<FillStyle>(format.attribute(tagFillStyle).toInt(&ok));
	if(!ok)
	    m_fillStyle=Brush;
	
	int tmp=format.attribute(tagBrushStyle).toInt(&ok);
	if(!ok)
	    tmp=0;
	m_brush.setStyle(static_cast<QBrush::BrushStyle>(tmp));
	if(format.hasAttribute(tagBrushColor))
	    m_brush.setColor(QColor(format.attribute(tagBrushColor)));
	
	QDomElement gradient=format.namedItem(tagGradient).toElement();
	if(!gradient.isNull()) {
	    if(gradient.hasAttribute(tagColorA))
		m_gradient.ca=QColor(gradient.attribute(tagColorA));
	    if(gradient.hasAttribute(tagColorB))
		m_gradient.cb=QColor(gradient.attribute(tagColorB));
	
	    m_gradient.type=static_cast<KImageEffect::GradientType>(gradient.attribute(tagType).toInt(&ok));
	    if(!ok)
		m_gradient.type=static_cast<KImageEffect::GradientType>(0);
	
	    m_gradient.xfactor=gradient.attribute(tagXFactor).toInt(&ok);
	    if(!ok)
		m_gradient.xfactor=1;
	    m_gradient.yfactor=gradient.attribute(tagYFactor).toInt(&ok);
	    if(!ok)
		m_gradient.yfactor=1;
	    m_gradient.ncols=gradient.attribute(tagNCols).toInt(&ok);
	    if(!ok)
		m_gradient.ncols=1;	
	}
	else {
	    m_gradient.type=static_cast<KImageEffect::GradientType>(0);
	    m_gradient.xfactor=1;
	    m_gradient.yfactor=1;
	    m_gradient.ncols=1;
	}
	
	QDomElement pen=format.namedItem(tagPen).toElement();
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
    m_ok=true;   // CTOR was successful :)
}
#include <gobject.moc>
