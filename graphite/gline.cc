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
#include <qpainter.h>

#include <gline.h>


GLine::GLine(const QPoint &a, const QPoint &b, const QString &name) : GObject(name),
								      m_a(a), m_b(b) {
}

GLine::GLine(const QString &name) : GObject(name) {
}

GLine::GLine(const GLine &rhs) : GObject(rhs), m_a(rhs.a()), m_b(rhs.b()) {
}

GLine::GLine(const QDomElement &element) : GObject(element.namedItem("gobject").toElement()) {

    if(!m_ok)
	return;

    bool ok;
    static QString tagStart=QString::fromLatin1("start");
    static QString tagEnd=QString::fromLatin1("end");
    static QString attrX=QString::fromLatin1("x");
    static QString attrY=QString::fromLatin1("y");

    if(element.tagName()!="gline") {
	m_ok=false;
	return;
    }

    QDomElement point=element.namedItem(tagStart).toElement();
    if(!point.isNull()) {
	m_a.setX(point.attribute(attrX).toInt(&ok));
	if(!ok)
	    m_a.setX(0);
	
	m_a.setY(point.attribute(attrY).toInt(&ok));
	if(!ok)
	    m_a.setX(0);
    }
    else {
	m_a.setX(0);
	m_a.setY(0);
    }

    point=element.namedItem(tagEnd).toElement();
    if(!point.isNull()) {
	m_b.setX(point.attribute(attrX).toInt(&ok));
	if(!ok)
	    m_b.setX(0);
	
	m_b.setY(point.attribute(attrY).toInt(&ok));
	if(!ok)
	    m_b.setX(0);
    }
    else {
	m_b.setX(0);
	m_b.setY(0);
    }
}

GLine *GLine::clone() const {
    return new GLine(*this);
}

GLine *GLine::instantiate(const QDomElement &element) const {
    return new GLine(element);
}

QDomElement GLine::save(QDomDocument &doc) const {

    QDomElement e=doc.createElement("gline");
    QDomElement point=doc.createElement("start");
    point.setAttribute("x", m_a.x());
    point.setAttribute("y", m_a.y());
    e.appendChild(point);
    point=doc.createElement("end");
    point.setAttribute("x", m_b.x());
    point.setAttribute("y", m_b.y());
    e.appendChild(point);
    e.appendChild(GObject::save(doc));
    return e;
}

void GLine::draw(QPainter &p, const QRegion &reg, const bool toPrinter) {

    if(m_state!=GObject::Visible && (!toPrinter && m_state!=GObject::Invisible))
	return;

    if(!reg.contains(boundingRect()))
	return;

    p.save();
    p.setPen(m_pen);
    p.drawLine(m_a, m_b);
    p.restore();
}

const GLine *GLine::hit(const QPoint &p) const {

    if(p==m_a)
	return this;
    else if(p==m_b)
	return this;
    else {
	double dx=static_cast<double>(m_b.x()-m_a.x());
	double dy=static_cast<double>(m_b.y()-m_a.y());
	double r=std::sqrt( dx*dx + dy*dy );
	int ir=double2Int(r);
	double alpha=std::asin( dy/r );

	// make it easier for the user to select something by
	// adding a (configurable) "fuzzy zone" :)
        QRect fuzzyZone=QRect( QMIN( m_a.x(), m_a.x()+ir ) - GraphiteGlobal::self()->fuzzyBorder(),
			       m_a.y() - GraphiteGlobal::self()->fuzzyBorder(),
			       ir + 2 * GraphiteGlobal::self()->fuzzyBorder(),
			       2 * GraphiteGlobal::self()->fuzzyBorder());
	// Don't change the original point!
	QPoint tmp=p;
	rotatePoint(tmp, alpha, m_a);
	
	if(fuzzyZone.contains(tmp))
	    return this;	
    }
    return 0L;
}

const bool GLine::intersects(const QRect &r) const {

    if(r.contains(m_a) || r.contains(m_b))
	return true;
    else if(r.intersects(m_boundingRect))
	return true;
    else {
	// f(x)=mx+d
	double m=static_cast<double>(m_b.y()-m_a.y())/static_cast<double>(m_b.x()-m_a.x());
	double d=m_a.y()-m*m_a.x();
	
	// top
	double i=(static_cast<double>(r.top())-d)/m;
	if(i>=r.left() && i<=r.right())
	    return true;
	
	// bottom
	i=(static_cast<double>(r.bottom())-d)/m;
	if(i>=r.left() && i<=r.right())
	    return true;
	
	// left
	i=m*static_cast<double>(r.left())+d;
	if(i>=r.top() && i<=r.bottom())
	    return true;
	
	// right
	i=m*static_cast<double>(r.right())+d;
	if(i>=r.top() && i<=r.bottom())
	    return true;	
    }
    return false;
}

const QRect &GLine::boundingRect() const {

    if(!m_boundingRectDirty)
	return m_boundingRect;
    m_boundingRect=QRect( QMIN(m_a.x(), m_b.x()), QMIN(m_a.y(), m_b.y()),
			  QABS(m_a.x()-m_b.x()), QABS(m_a.y()-m_b.y()));
    m_boundingRectDirty=false;
    return m_boundingRect;				
}

GObjectM9r *GLine::createM9r(const GObjectM9r::Mode &mode) {
    return new GLineM9r(this, mode);
}

void GLine::setOrigin(const QPoint &origin) {

    m_b.setX(m_b.x()-m_a.x()+origin.x());
    m_b.setY(m_b.y()-m_a.y()+origin.y());
    m_a=origin;
    m_boundingRectDirty=true;
}

void GLine::moveX(const int &dx) {

    m_a.setX(m_a.x()+dx);
    m_b.setX(m_b.x()+dx);
    m_boundingRectDirty=true;
}

void GLine::moveY(const int &dy) {

    m_a.setY(m_a.y()+dy);
    m_b.setY(m_b.y()+dy);
    m_boundingRectDirty=true;
}

void GLine::move(const int &dx, const int &dy) {

    moveX(dx);
    moveY(dy);
}

void GLine::rotate(const QPoint &center, const double &angle) {

    rotatePoint(m_a, angle, center);
    rotatePoint(m_b, angle, center);
    m_boundingRectDirty=true;
}

void GLine::scale(const QPoint &origin, const double &xfactor, const double &yfactor) {

    scalePoint(m_a, xfactor, yfactor, origin);
    scalePoint(m_b, xfactor, yfactor, origin);
    m_boundingRectDirty=true;
}

void GLine::resize(const QRect &boundingRect) {

    m_boundingRect=boundingRect;
    m_boundingRectDirty=false;
    m_a=boundingRect.topLeft();
    m_b=boundingRect.bottomRight();
}


GLineM9r::GLineM9r(GLine *line, const Mode &mode) : G1DObjectM9r(mode), m_line(line) {

    QApplication::setOverrideCursor(Qt::crossCursor);
    m_line->setState(GObject::Handles);
}

GLineM9r::~GLineM9r() {

    m_line->setState(GObject::Visible);
    QApplication::restoreOverrideCursor();
}

void GLineM9r::draw(QPainter &p, const QRegion &reg, const bool toPrinter) {
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
