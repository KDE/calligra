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
#include <qapp.h>

#include <gline.h>


GLine::GLine(const QPoint &a, const QPoint &b, const QString &name) : GObject(name),
								      m_a(a), m_b(b) {
}

GLine::GLine(const QString &name) : GObject(name) {
}

GLine::GLine(const GLine &rhs) : GObject(rhs), m_a(rhs.a()), m_b(rhs.b()) {
}

GLine::GLine(const QDomElement &element) : GObject(element.namedItem("gobject").toElement()) {

    // TODO - check whether the gobject was initialized correctly (isOk())
    //bool ok;
    //static QString tagName=QString::fromLatin1("name");

    //if(m_ok) // loading was successful

    /*
    if(element.hasAttribute(tagName))
	m_name=element.attribute(tagName);
    else
	m_name="no name";

    m_state=static_cast<State>(element.attribute(tagState).toInt(&ok));
    if(!ok)
	m_state=Visible;

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
    */
}

GLine *GLine::clone() const {
    return new GLine(*this);
}

GLine *GLine::instantiate(const QDomElement &element) const {
    return new GLine(element);
}

QDomElement GLine::save(QDomDocument &doc) const {

    // TODO
    QDomElement e=doc.createElement("gline");
    e.appendChild(GObject::save(doc));
    return e;
}

void GLine::draw(const QPainter &/*p*/, const QRegion &/*reg*/, const bool /*toPrinter*/) const {
    // TODO
}

const GLine *GLine::hit(const QPoint &/*p*/) const {
    // TODO
    return 0L;
}

const bool GLine::intersects(const QRect &/*r*/) const {
    // TODO
    return false;
}

const QRect &GLine::boundingRect() const {
    // TODO - check flag!
    return m_boundingRect;
}

GObjectM9r *GLine::createM9r(const GObjectM9r::Mode &mode) {
    return new GLineM9r(this, mode);
}

void GLine::setOrigin(const QPoint &/*origin*/) {
    // TODO
}

void GLine::moveX(const int &/*dx*/) {
    // TODO
}

void GLine::moveY(const int &/*dy*/) {
    // TODO
}

void GLine::move(const int &/*dx*/, const int &/*dy*/) {
    // TODO
}

void GLine::rotate(const QPoint &/*center*/, const double &/*angle*/) {
    // TODO
}

void GLine::scale(const QPoint &/*origin*/, const double &/*xfactor*/, const double &/*yfactor*/) {
    // TODO
}

void GLine::resize(const QRect &/*boundingRect*/) {
    // TODO
}


GLineM9r::GLineM9r(GLine *line, const Mode &mode) : GObjectM9r(mode), m_line(line) {

    QApplication::setOverrideCursor(Qt::crossCursor);
    m_line->setState(GObject::Handles);
}

GLineM9r::~GLineM9r() {

    m_line->setState(GObject::Visible);
    QApplication::restoreOverrideCursor();
}

void GLineM9r::draw(const QPainter &p, const QRegion &reg, const bool toPrinter) const {
    m_line->draw(p, reg, toPrinter);
}

const bool GLineM9r::mouseMoveEvent(QMouseEvent */*e*/, const GraphiteView */*view*/,
				    QRect &/*dirty*/) {
    // TODO
    return false;
}

const bool GLineM9r::mousePressEvent(QMouseEvent */*e*/, const GraphiteView */*view*/,
				     QRect &/*dirty*/) {
    // TODO
    return false;
}

const bool GLineM9r::mouseReleaseEvent(QMouseEvent */*e*/, const GraphiteView */*view*/,
				       QRect &/*dirty*/) {
    // TODO
    return false;
}

const bool GLineM9r::mouseDoubleClickEvent(QMouseEvent */*e*/, const GraphiteView */*view*/,
					   QRect &/*dirty*/) {
    // TODO
    return false;
}

const bool GLineM9r::keyPressEvent(QKeyEvent */*e*/, const GraphiteView */*view*/,
				   QRect &/*dirty*/) {
    // TODO
    return false;
}

const bool GLineM9r::keyReleaseEvent(QKeyEvent */*e*/, const GraphiteView */*view*/,
				     QRect &/*dirty*/) {
    // TODO
    return false;
}
#include <gline.moc>
