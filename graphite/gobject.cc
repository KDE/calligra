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
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <kdialogbase.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcolorbtn.h>

#include <gobject.h>
#include <graphitefactory.h>


GObjectM9r::~GObjectM9r() {

    if(m_dialog!=0L) {
	delete m_dialog;
	m_dialog=0L;
    }
}

KDialogBase *GObjectM9r::createPropertyDialog(QWidget *parent) {

    if(m_dialog==0L)
	m_dialog=new KDialogBase(KDialogBase::IconList,
				 i18n("Change Properties"),
				 KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel,
				 KDialogBase::Ok, parent, "property dia", true, true);
    return m_dialog;
}


void G1DObjectM9r::setPenStyle(int style) {

    QPen p=m_object->pen();
    p.setStyle(static_cast<Qt::PenStyle>(style));
    m_object->setPen(p);
}

void G1DObjectM9r::setPenWidth(int width) {

    QPen p=m_object->pen();
    p.setWidth(width);
    m_object->setPen(p);
}

void G1DObjectM9r::setPenColor(const QColor &color) {

    QPen p=m_object->pen();
    p.setColor(color);
    m_object->setPen(p);
}

KDialogBase *G1DObjectM9r::createPropertyDialog(QWidget *parent) {

    GObjectM9r::createPropertyDialog(parent);
    if(!m_dialog)
	return 0L;

    QFrame *frame=m_dialog->addPage(i18n("Pen"), i18n("Pen Settings"),
			       BarIcon("exec", 32, KIcon::DefaultState, GraphiteFactory::global()));

    QGridLayout *grid=new QGridLayout(frame, 3, 2, KDialog::marginHint(), KDialog::spacingHint());

    QLabel *label=new QLabel(i18n("Width:"), frame);
    grid->addWidget(label, 0, 0);

    QSpinBox *width=new QSpinBox(1, 100, 1, frame);
    width->setValue(m_object->pen().width());
    connect(width, SIGNAL(valueChanged(int)), this, SLOT(setPenWidth(int)));
    grid->addWidget(width, 0, 1);

    label=new QLabel(i18n("Color:"), frame);
    grid->addWidget(label, 1, 0);

    KColorButton *color=new KColorButton(m_object->pen().color(), frame);
    connect(color, SIGNAL(changed(const QColor &)), this,
	    SLOT(setPenColor(const QColor &)));
    grid->addWidget(color, 1, 1);

    label=new QLabel(i18n("Style:"), frame);
    grid->addWidget(label, 2, 0);

    QComboBox *style=new QComboBox(frame);
    QPainter painter;
    QPixmap *pm;
    for(int i=0; i<6; ++i) {
	pm=new QPixmap(70, 15);
	pm->fill();
	painter.begin(pm);
	painter.setPen(QPen(QColor(Qt::black),
			    static_cast<unsigned int>(2),
			    static_cast<Qt::PenStyle>(i)));
	painter.drawLine(0, 7, 70, 7);
	painter.end();
	style->insertItem(*pm);
    }
    connect(style, SIGNAL(activated(int)), this, SLOT(setPenStyle(int)));
    style->setCurrentItem(m_object->pen().style());
    grid->addWidget(style, 2, 1);

    return m_dialog;
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

GObject::GObject(const QString &name) : m_name(name), m_state(Visible), m_parent(0L),
    m_zoom(100), m_angle(0.0), m_boundingRectDirty(true), m_fillStyle(Brush), m_ok(true) {

    m_gradient.type=KImageEffect::VerticalGradient;
    m_gradient.xfactor=1;
    m_gradient.yfactor=1;
    m_gradient.ncols=1;
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
    static QString attrName=QString::fromLatin1("name");
    static QString attrState=QString::fromLatin1("state");
    static QString attrAngle=QString::fromLatin1("angle");
    static QString tagFormat=QString::fromLatin1("format");
    static QString attrFillStyle=QString::fromLatin1("fillStyle");
    static QString attrBrushStyle=QString::fromLatin1("brushStyle");
    static QString attrBrushColor=QString::fromLatin1("brushColor");
    static QString tagGradient=QString::fromLatin1("gradient");
    static QString attrColorA=QString::fromLatin1("colorA");
    static QString attrColorB=QString::fromLatin1("colorB");
    static QString attrType=QString::fromLatin1("type");
    static QString attrXFactor=QString::fromLatin1("xfactor");
    static QString attrYFactor=QString::fromLatin1("yfactor");
    static QString attrNCols=QString::fromLatin1("ncols");
    static QString tagPen=QString::fromLatin1("pen");

    if(element.hasAttribute(attrName))
	m_name=element.attribute(attrName);
    else
	m_name="no name";

    m_state=static_cast<State>(element.attribute(attrState).toInt(&ok));
    if(!ok || m_state==Handles || m_state==Rot_Handles)
	m_state=Visible;

    m_angle=element.attribute(attrAngle).toDouble(&ok);
    if(!ok)
	m_angle=0.0;

    QDomElement format=element.namedItem(tagFormat).toElement();
    if(!format.isNull()) {
	m_fillStyle=static_cast<FillStyle>(format.attribute(attrFillStyle).toInt(&ok));
	if(!ok)
	    m_fillStyle=Brush;
	
	int tmp=format.attribute(attrBrushStyle).toInt(&ok);
	if(!ok)
	    tmp=0;
	m_brush.setStyle(static_cast<QBrush::BrushStyle>(tmp));
	if(format.hasAttribute(attrBrushColor))
	    m_brush.setColor(QColor(format.attribute(attrBrushColor)));
	
	QDomElement gradient=format.namedItem(tagGradient).toElement();
	if(!gradient.isNull()) {
	    if(gradient.hasAttribute(attrColorA))
		m_gradient.ca=QColor(gradient.attribute(attrColorA));
	    if(gradient.hasAttribute(attrColorB))
		m_gradient.cb=QColor(gradient.attribute(attrColorB));
	
	    m_gradient.type=static_cast<KImageEffect::GradientType>(gradient.attribute(attrType).toInt(&ok));
	    if(!ok)
		m_gradient.type=static_cast<KImageEffect::GradientType>(0);
	
	    m_gradient.xfactor=gradient.attribute(attrXFactor).toInt(&ok);
	    if(!ok)
		m_gradient.xfactor=1;
	    m_gradient.yfactor=gradient.attribute(attrYFactor).toInt(&ok);
	    if(!ok)
		m_gradient.yfactor=1;
	    m_gradient.ncols=gradient.attribute(attrNCols).toInt(&ok);
	    if(!ok)
		m_gradient.ncols=1;	
	}
	else {
	    m_gradient.type=KImageEffect::VerticalGradient;
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
	m_gradient.type=KImageEffect::VerticalGradient;
	m_gradient.xfactor=1;
	m_gradient.yfactor=1;
	m_gradient.ncols=1;	
    }
    m_ok=true;   // CTOR has been successful :)
}
#include <gobject.moc>
