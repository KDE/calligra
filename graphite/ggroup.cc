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

#include <klocale.h>
#include <kglobal.h>

#include <ggroup.h>
#include <gobjectfactory.h>

// test
#include <graphiteview.h>
#include <kdialogbase.h>
#include <kdebug.h>


GGroup::GGroup(const QString &name) : GObject(name), m_iterator(0L) {
    m_iterator=new QListIterator<GObject>(m_members);
}

GGroup::GGroup(const GGroup &rhs) : GObject(rhs), m_iterator(0L) {

    m_iterator=new QListIterator<GObject>(m_members);

    for(const GObject *object=rhs.firstChild(); object!=0L; object=rhs.nextChild()) {
        const GObject *cloned=object->clone();
        if(cloned!=0L) {
            if(cloned->state()!=GObject::Deleted) {
                cloned->setParent(this);
                m_members.append(cloned);
            }
            else
                delete cloned;
        }
    }
}

GGroup::GGroup(const QDomElement &element) :
    GObject(element.namedItem(QString::fromLatin1("gobject")).toElement()),
    m_iterator(0L) {

    if(!m_ok)
        return;

    static const QString &tagChildren=KGlobal::staticQString("children");

    m_iterator=new QListIterator<GObject>(m_members);

    QDomElement children=element.namedItem(tagChildren).toElement();
    if(children.isNull()) {
        m_ok=false;
        return;
    }

    QDomElement e=children.firstChild().toElement();
    for( ; !e.isNull(); e=e.nextSibling().toElement()) {
        const GObject *object=GObjectFactory::self()->create(e);
        if(object!=0L) {
            if(object->isOk()) {
                object->setParent(this);
                m_members.append(object);
            }
            else
                delete object;
        }
    }
}

GGroup::~GGroup() {

    delete m_iterator;
    m_iterator=0L;
}

bool GGroup::isOk() const {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L && it.current()->isOk(); ++it);

    if(it==0L)
        return true;
    else
        return false;
}

void GGroup::setOk(bool ok) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setOk(ok);
}

GObject *GGroup::clone() const {
    return new GGroup(*this);
}

GObject *GGroup::instantiate(const QDomElement &element) const {
    return new GGroup(element);
}

bool GGroup::plugChild(GObject *child, const Position &pos) {

    if(child==0L)
        return false;

    child->setParent(this);

    if(pos==GObject::First)
        m_members.prepend(child);
    else if(pos==GObject::Last)
        m_members.append(child);
    else {
        int index=m_members.findRef(m_iterator->current());
        if(index!=-1)
            m_members.insert(index, child);
        else
            m_members.append(child);
    }
    m_boundingRectDirty=true;
    return true;
}

bool GGroup::unplugChild(GObject *child) {

    if(child==0L)
        return false;
    child->setParent(0L);
    m_boundingRectDirty=true;
    return m_members.removeRef(child);
}

const GObject *GGroup::firstChild() const {
    return m_iterator->toFirst();
}

const GObject *GGroup::nextChild() const {
    ++m_iterator;
    return m_iterator->current();
}

const GObject *GGroup::lastChild() const {
    return m_iterator->toLast();
}

const GObject *GGroup::prevChild() const {
    --m_iterator;
    return m_iterator->current();
}

const GObject *GGroup::current() const {
    return m_iterator->current();
}

QDomElement GGroup::save(QDomDocument &doc) const {

    static const QString &tagChildren=KGlobal::staticQString("children");
    static const QString &tagGGroup=KGlobal::staticQString("ggroup");

    QDomElement element=doc.createElement(tagGGroup);
    QDomElement children=doc.createElement(tagChildren);

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it) {
        if(it.current()->state()!=GObject::Deleted)
            children.appendChild(it.current()->save(doc));
    }
    element.appendChild(children);
    element.appendChild(GObject::save(doc));
    return element;
}

void GGroup::draw(QPainter &p, const QRect &rect, bool toPrinter) {

    // first go and check the boundign rect!
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->draw(p, rect, toPrinter);
}

void GGroup::recalculate() {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->recalculate();
    m_boundingRectDirty=true;
}

const GObject *GGroup::hit(const QPoint &p) const {

    QListIterator<GObject> it(m_members);
    it.toLast();
    for( ; it!=0L; --it) {
        if(it.current()->hit(p))
            return it.current();
    }
    if(boundingRect().contains(p))
        return this;
    return 0L;
}

bool GGroup::intersects(const QRect &r) const {

    if(r.intersects(boundingRect()))
        return true;
    return false;
}

const QRect &GGroup::boundingRect() const {

    if(!m_boundingRectDirty)
        return m_boundingRect;

    if(m_members.isEmpty()) {
        m_boundingRect=QRect(0, 0, 0, 0);
        m_boundingRectDirty=true;
        return m_boundingRect;
    }

    QListIterator<GObject> it(m_members);
    m_boundingRect=it.current()->boundingRect();
    ++it;
    for( ; it!=0L; ++it) {
        QRect r=it.current()->boundingRect();
        if(r.top()<m_boundingRect.top())
            m_boundingRect.setTop(r.top());
        if(r.left()<m_boundingRect.left())
            m_boundingRect.setLeft(r.left());
        if(r.bottom()>m_boundingRect.bottom())
            m_boundingRect.setBottom(r.bottom());
        if(r.right()>m_boundingRect.right())
            m_boundingRect.setRight(r.right());
    }
    m_boundingRectDirty=false;
    return m_boundingRect;
}

GObjectM9r *GGroup::createM9r(GraphitePart *part, GraphiteView *view,
                              const GObjectM9r::Mode &mode) {
    return new GGroupM9r(this, mode, part, view, i18n("Group"));
}

const QPoint GGroup::origin() const {
    return boundingRect().topLeft();
}

void GGroup::setOrigin(const QPoint &o) {

    int dx=o.x()-origin().x();
    int dy=o.y()-origin().y();
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->move(dx, dy);
    m_boundingRectDirty=true;
}

void GGroup::moveX(const int &dx) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->moveX(dx);
    m_boundingRectDirty=true;
}

void GGroup::moveY(const int &dy) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->moveY(dy);
    m_boundingRectDirty=true;
}

void GGroup::move(const int &dx, const int &dy) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->move(dx, dy);
    m_boundingRectDirty=true;
}

void GGroup::rotate(const QPoint &center, const double &angle) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->rotate(center, angle);
    m_angle+=angle;
    m_angle=Graphite::normalizeRad(m_angle);
    m_boundingRectDirty=true;
}

void GGroup::scale(const QPoint &origin, const double &xfactor, const double &yfactor) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->scale(origin, xfactor, yfactor);
    m_boundingRectDirty=true;
}

void GGroup::resize(const QRect &brect) {

    int dx=brect.width()-boundingRect().width();
    int dy=brect.height()-boundingRect().height();
    double xfactor=static_cast<double>(dx)/static_cast<double>(boundingRect().width());
    double yfactor=static_cast<double>(dy)/static_cast<double>(boundingRect().height());
    scale(origin(), xfactor, yfactor);
    move(dx, dy);
}

void GGroup::setState(const State state) {

    if(m_state==state)
        return;
    m_state=state;
    if(state==GObject::Handles || state==GObject::Rot_Handles)
        return;

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setState(state);
}

void GGroup::setFillStyle(const FillStyle &fillStyle) {

    if(m_fillStyle==fillStyle)
        return;
    m_fillStyle=fillStyle;
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setFillStyle(fillStyle);
}

void GGroup::setBrush(const QBrush &brush) {

    if(m_brush==brush)
        return;
    m_brush=brush;
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setBrush(brush);
}

void GGroup::setGradient(const Gradient &gradient) {

    if(m_gradient==gradient)
        return;
    m_gradient=gradient;
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setGradient(gradient);
}

void GGroup::setPen(const QPen &pen) {

    if(m_pen==pen)
        return;
    m_pen=pen;
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setPen(pen);
}


GGroupM9r::GGroupM9r(GGroup *group, const Mode &mode, GraphitePart *part,
                     GraphiteView *view, const QString &type) :
    G2DObjectM9r(group, mode, part, view, type), m_group(group) {
    m_group->setState(GObject::Handles);
}

GGroupM9r::~GGroupM9r() {
    if(m_group->state()==GObject::Handles || m_group->state()==GObject::Rot_Handles)
        m_group->setState(GObject::Visible);
}

void GGroupM9r::draw(QPainter &p) {
    m_group->drawHandles(p, m_handles);
}

bool GGroupM9r::mouseMoveEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // TODO
    return false;
}

bool GGroupM9r::mousePressEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {

    // TODO
    // test
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    kdDebug(37001) << "Name: " << m_object->name() << endl;
    kdDebug(37001) << "Pen: color: " << m_object->pen().color().name() << " width: "
                   << m_object->pen().width() << " style: "
                   << (int)m_object->pen().style() << endl;
    kdDebug(37001) << "Fill Style: " << m_object->fillStyle() << endl;
    kdDebug(37001) << "Brush: color: " << m_object->brush().color().name()
                   << " style: " << (int)m_object->brush().style() << endl;
    kdDebug(37001) << "Gradient: color a: " << m_object->gradient().ca.name()
                   << " color b: " << m_object->gradient().cb.name()
                   << " type: " << (int)m_object->gradient().type
                   << " xfactor: " << m_object->gradient().xfactor
                   << " yfactor: " << m_object->gradient().yfactor
                   << endl;
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    createPropertyDialog();
    exec();
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    kdDebug(37001) << "Name: " << m_object->name() << endl;
    kdDebug(37001) << "Pen: color: " << m_object->pen().color().name() << " width: "
                   << m_object->pen().width() << " style: "
                   << (int)m_object->pen().style() << endl;
    kdDebug(37001) << "Fill Style: " << m_object->fillStyle() << endl;
    kdDebug(37001) << "Brush: color: " << m_object->brush().color().name()
                   << " style: " << (int)m_object->brush().style() << endl;
    kdDebug(37001) << "Gradient: color a: " << m_object->gradient().ca.name()
                   << " color b: " << m_object->gradient().cb.name()
                   << " type: " << (int)m_object->gradient().type
                   << " xfactor: " << m_object->gradient().xfactor
                   << " yfactor: " << m_object->gradient().yfactor
                   << endl;
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    return false;
}

bool GGroupM9r::mouseReleaseEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // TODO
    return false;
}

bool GGroupM9r::mouseDoubleClickEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // TODO
    return false;
}

bool GGroupM9r::keyPressEvent(QKeyEvent */*e*/, QRect &/*dirty*/) {
    // TODO
    return false;
}

bool GGroupM9r::keyReleaseEvent(QKeyEvent */*e*/, QRect &/*dirty*/) {
    // TODO
    return false;
}

#include <ggroup.moc>
