/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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
#include <qpainter.h>
#include <klocale.h>
#include <kglobal.h>

#include <gline.h>

#include <float.h>  // DBL_BLAH

GLine::GLine(const QPoint &a, const QPoint &b, const QString &name) : GObject(name),
                                                                      m_a(a), m_b(b) {
}

GLine::GLine(const FxPoint &a, const FxPoint &b, const QString &name) : GObject(name),
                                                                      m_a(a), m_b(b) {
}

GLine::GLine(const QString &name) : GObject(name) {
}

GLine::GLine(const GLine &rhs) : GObject(rhs), m_a(rhs.a()), m_b(rhs.b()) {
}

GLine::GLine(const QDomElement &element) :
    GObject(element.namedItem(QString::fromLatin1("gobject")).toElement()) {

    if(!isOk())
        return;

    bool ok=true;
    static const QString &tagGLine=KGlobal::staticQString("gline");
    static const QString &tagStart=KGlobal::staticQString("start");
    static const QString &tagEnd=KGlobal::staticQString("end");
    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");

    if(element.tagName()!=tagGLine) {
        setOk(false);
        return;
    }

    QDomElement point=element.namedItem(tagStart).toElement();
    if(!point.isNull()) {
        m_a.setX(point.attribute(attrX).toDouble(&ok));
        if(!ok)
            m_a.setX(0.0);

        m_a.setY(point.attribute(attrY).toDouble(&ok));
        if(!ok)
            m_a.setX(0.0);
    }
    else {
        m_a.setX(0.0);
        m_a.setY(0.0);
    }

    point=element.namedItem(tagEnd).toElement();
    if(!point.isNull()) {
        m_b.setX(point.attribute(attrX).toDouble(&ok));
        if(!ok)
            m_b.setX(0.0);

        m_b.setY(point.attribute(attrY).toDouble(&ok));
        if(!ok)
            m_b.setX(0.0);
    }
    else {
        m_b.setX(0.0);
        m_b.setY(0.0);
    }
}

GLine *GLine::clone() const {
    return new GLine(*this);
}

GLine *GLine::instantiate(const QDomElement &element) const {
    return new GLine(element);
}

QDomElement GLine::save(QDomDocument &doc) const {

    static const QString &tagGLine=KGlobal::staticQString("gline");
    static const QString &tagStart=KGlobal::staticQString("start");
    static const QString &tagEnd=KGlobal::staticQString("end");
    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");

    QDomElement e=doc.createElement(tagGLine);
    QDomElement point=doc.createElement(tagStart);
    point.setAttribute(attrX, QString::number(m_a.x(), 'g', DBL_MANT_DIG));
    point.setAttribute(attrY, QString::number(m_a.y(), 'g', DBL_MANT_DIG));
    e.appendChild(point);
    point=doc.createElement(tagEnd);
    point.setAttribute(attrX, QString::number(m_b.x(), 'g', DBL_MANT_DIG));
    point.setAttribute(attrY, QString::number(m_b.y(), 'g', DBL_MANT_DIG));
    e.appendChild(point);
    e.appendChild(GObject::save(doc));
    return e;
}

void GLine::draw(QPainter &p, const QRect &rect, bool toPrinter) const {

    if(dirty())
        recalculate();

    if(state()==GObject::Deleted || (toPrinter && state()==GObject::Invisible))
        return;

    if(!rect.intersects(boundingRect()))
        return;

    p.save();
    p.setPen(pen()); // adjust the pen if it's invisible (light grey)
    p.drawLine(m_a.pxX(), m_a.pxY(), m_b.pxX(), m_b.pxY());
    p.restore();
}

void GLine::drawHandles(QPainter &p, const QRect &/*rect*/, QList<QRect> *handles) const {

    if(dirty())
        recalculate();

    p.save();
    p.setPen(Qt::black);
    p.setBrush(Qt::black);

    int size;
    int offset;
    if(state()==Handles) {
        size=GraphiteGlobal::self()->handleSize();
        offset=GraphiteGlobal::self()->handleOffset();
    }
    else if(state()==Rot_Handles) {
        size=GraphiteGlobal::self()->rotHandleSize();
        offset=GraphiteGlobal::self()->rotHandleOffset();
    }
    else
        return; // no need to draw handles - shouldn't happen

    QRect *r1=new QRect(m_a.pxX()-offset, m_a.pxY()-offset, size, size);
    QRect *r2=new QRect(m_b.pxX()-offset, m_b.pxY()-offset, size, size);
    QRect *r3=0L;

    if(boundingRect().width()>GraphiteGlobal::self()->thirdHandleTrigger() ||
       boundingRect().height()>GraphiteGlobal::self()->thirdHandleTrigger())
        r3=new QRect(m_a.pxX()+Graphite::double2Int((m_b.x()-m_a.x())*0.5)-offset,
                     m_a.pxY()+Graphite::double2Int((m_b.y()-m_a.y())*0.5)-offset,
                     size, size);

    if(state()==Handles) {
        p.drawRect(*r1);
        p.drawRect(*r2);
        if(r3)
            p.drawRect(*r3);
    }
    else {
        p.drawEllipse(*r1);
        p.drawEllipse(*r2);
        if(r3)
            p.drawEllipse(*r3);
    }

    if(handles) {
        handles->append(r1);
        handles->append(r2);
        if(r3)
            handles->append(r3);
    }
    else {
        delete r1;
        delete r2;
        delete r3;
    }
    p.restore();
}

void GLine::recalculate() const {
    m_a.recalculate();
    m_b.recalculate();
    GObject::recalculate();
}

const GLine *GLine::hit(const QPoint &p) const {

    if(dirty())
        recalculate();

    int fBorder=GraphiteGlobal::self()->fuzzyBorder();
    QRect fuzzyRect(boundingRect().x()-fBorder, boundingRect().y()-fBorder,
                    boundingRect().width()+fBorder, boundingRect().height()+fBorder);
    if(!fuzzyRect.contains(p))
        return 0L;

    if(p==m_a.pxPoint())
        return this;
    else if(p==m_b.pxPoint())
        return this;
    else {
        double dx=m_b.x()-m_a.x();
        double dy=m_b.y()-m_a.y();
        double r=std::sqrt( dx*dx + dy*dy );
        int ir=Graphite::double2Int(r);
        double alpha1=std::asin( Graphite::abs(dy)/r );
        double alpha;

        if(dx>=0) {
            if(dy>=0)
                alpha=-alpha1;
            else
                alpha=alpha1;
        }
        else {
            if(dy>=0)
                alpha=alpha1+M_PI;
            else
                alpha=M_PI-alpha1;
        }

        // make it easier for the user to select something by
        // adding a (configurable) "fuzzy zone" :)
        int w=Graphite::double2Int(static_cast<double>(pen().width())/2.0);
        int tmp1=fBorder-w;
        int tmp2=2*fBorder+w+1;
        QRect fuzzyZone=QRect( Graphite::min( m_a.pxX(), m_a.pxX() + ir ) - tmp1,
                               m_a.pxY() - tmp1, ir + tmp2, tmp2 );
        // Don't change the original point!
        QPoint tmp3(p);
        Graphite::rotatePoint(tmp3, -alpha, m_a.pxPoint());

        if(fuzzyZone.contains(tmp3))
            return this;
    }
    return 0L;
}

bool GLine::intersects(const QRect &r) const {

    if(dirty())
        recalculate();

    if(r.contains(m_a.pxPoint()) || r.contains(m_b.pxPoint()))
        return true;
    else if(r.intersects(boundingRect()))
        return true;
    else {
        // f(x)=mx+d
        double m=(m_b.y()-m_a.y())/(m_b.x()-m_a.x());
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

    if(!boundingRectDirty())
        return GObject::boundingRect();

    if(dirty())
        recalculate();
    setBoundingRect(QRect( Graphite::min(m_a.pxX(), m_b.pxX()), Graphite::min(m_a.pxY(), m_b.pxY()),
                           Graphite::abs(m_a.pxX()-m_b.pxX()), Graphite::abs(m_a.pxY()-m_b.pxY())));
    setBoundingRectDirty(false);
    return GObject::boundingRect();
}

GObjectM9r *GLine::createM9r(GraphitePart *part, GraphiteView *view,
                             const GObjectM9r::Mode &mode) const {
    // Yes, this const_cast is ugly, I know
    return new GLineM9r(const_cast<GLine*>(this), mode, part, view, i18n("Line"));
}

void GLine::setOrigin(const FxPoint &origin) {

    m_b.setX(m_b.x()-m_a.x()+origin.x());
    m_b.setY(m_b.y()-m_a.y()+origin.y());
    m_a=origin;
    setBoundingRectDirty();
}

void GLine::moveX(const double &dx) {

    m_a.setX(m_a.x()+dx);
    m_b.setX(m_b.x()+dx);
    setBoundingRectDirty();
}

void GLine::moveY(const double &dy) {

    m_a.setY(m_a.y()+dy);
    m_b.setY(m_b.y()+dy);
    setBoundingRectDirty();
}

void GLine::move(const double &dx, const double &dy) {

    moveX(dx);
    moveY(dy);
}

void GLine::rotate(const FxPoint &center, const double &angle) {

    Graphite::rotatePoint(m_a, angle, center);
    Graphite::rotatePoint(m_b, angle, center);
    setBoundingRectDirty();
}

void GLine::scale(const FxPoint &origin, const double &xfactor, const double &yfactor) {

    Graphite::scalePoint(m_a, xfactor, yfactor, origin);
    Graphite::scalePoint(m_b, xfactor, yfactor, origin);
    setBoundingRectDirty();
}

void GLine::resize(const FxRect &boundingRect) {

    setBoundingRect(boundingRect.pxRect());
    setBoundingRectDirty(false);
    m_a=boundingRect.topLeft();
    m_b=boundingRect.bottomRight();
}


GLineM9r::GLineM9r(GLine *line, const Mode &mode, GraphitePart *part,
                   GraphiteView *view, const QString &type) :
    G1DObjectM9r(line, mode, part, view, type), m_line(line) {
    m_line->setState(GObject::Handles);
}

GLineM9r::~GLineM9r() {
    if(m_line->state()==GObject::Handles || m_line->state()==GObject::Rot_Handles)
        m_line->setState(GObject::Visible);
}

bool GLineM9r::mouseMoveEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // ###
    return false;
}

bool GLineM9r::mousePressEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // ###
    qDebug("FOOOOOOOOOOOOOOOOO");
    return false;
}

bool GLineM9r::mouseReleaseEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // ###
    return false;
}

bool GLineM9r::mouseDoubleClickEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // ###
    return false;
}

bool GLineM9r::keyPressEvent(QKeyEvent */*e*/, QRect &/*dirty*/) {
    // ###
    return false;
}

bool GLineM9r::keyReleaseEvent(QKeyEvent */*e*/, QRect &/*dirty*/) {
    // We don't need that one for lines... hmmm ...
    return false;
}

#include <gline.moc>
