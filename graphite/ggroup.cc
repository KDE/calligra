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

    if(!isOk())
        return;

    static const QString &tagChildren=KGlobal::staticQString("children");

    m_iterator=new QListIterator<GObject>(m_members);

    QDomElement children=element.namedItem(tagChildren).toElement();
    if(children.isNull()) {
        setOk(false);
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

void GGroup::setDirty() {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setDirty();
    GObject::setDirty();
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
    setBoundingRectDirty();
    return true;
}

bool GGroup::unplugChild(GObject *child) {

    if(child==0L)
        return false;
    child->setParent(0L);
    setBoundingRectDirty();
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

    if(dirty())
        recalculate();

    // first go and check the boundign rect!
    if(!rect.intersects(boundingRect()))
        return;

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->draw(p, rect, toPrinter);
}

const GObject *GGroup::hit(const QPoint &p) const {

    if(dirty())
        recalculate();

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

    if(dirty())
        recalculate();

    if(r.intersects(boundingRect()))
        return true;
    return false;
}

const QRect &GGroup::boundingRect() const {

    if(!boundingRectDirty())
        return GObject::boundingRect();

    if(dirty())
        recalculate();

    if(m_members.isEmpty()) {
        setBoundingRect(QRect(0, 0, 0, 0));
        setBoundingRectDirty(false);
        return GObject::boundingRect();
    }

    QListIterator<GObject> it(m_members);
    QRect br=it.current()->boundingRect();
    ++it;
    for( ; it!=0L; ++it) {
        QRect r=it.current()->boundingRect();
        if(r.top()<br.top())
            br.setTop(r.top());
        if(r.left()<br.left())
            br.setLeft(r.left());
        if(r.bottom()>br.bottom())
            br.setBottom(r.bottom());
        if(r.right()>br.right())
            br.setRight(r.right());
    }
    setBoundingRect(br);
    setBoundingRectDirty(false);
    return GObject::boundingRect();
}

GObjectM9r *GGroup::createM9r(GraphitePart *part, GraphiteView *view,
                              const GObjectM9r::Mode &mode) {
    return new GGroupM9r(this, mode, part, view, i18n("Group"));
}

const FxPoint GGroup::origin() const {
    return FxPoint(boundingRect().topLeft());
}

void GGroup::setOrigin(const FxPoint &o) {

    double dx=o.x()-static_cast<double>(origin().x());
    double dy=o.y()-static_cast<double>(origin().y());
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->move(dx, dy);
    setBoundingRectDirty();
}

void GGroup::moveX(const double &dx) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->moveX(dx);
    setBoundingRectDirty();
}

void GGroup::moveY(const double &dy) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->moveY(dy);
    setBoundingRectDirty();
}

void GGroup::move(const double &dx, const double &dy) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->move(dx, dy);
    setBoundingRectDirty();
}

void GGroup::rotate(const FxPoint &center, const double &ang) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->rotate(center, ang);
    setAngle(Graphite::normalizeRad(angle()+ang));
    setBoundingRectDirty();
}

void GGroup::scale(const FxPoint &origin, const double &xfactor, const double &yfactor) {

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->scale(origin, xfactor, yfactor);
    setBoundingRectDirty();
}

void GGroup::resize(const FxRect &brect) {

    double dx=brect.width()-static_cast<double>(boundingRect().width());
    double dy=brect.height()-static_cast<double>(boundingRect().height());
    double xfactor=static_cast<double>(dx)/static_cast<double>(boundingRect().width());
    double yfactor=static_cast<double>(dy)/static_cast<double>(boundingRect().height());
    scale(origin(), xfactor, yfactor);
    move(dx, dy);
}

void GGroup::setState(const State s) {

    if(state()==s)
        return;
    GObject::setState(s);
    if(s==GObject::Handles || s==GObject::Rot_Handles)
        return;

    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setState(s);
}

void GGroup::setFillStyle(const FillStyle &fs) {

    if(fillStyle()==fs)
        return;
    GObject::setFillStyle(fs);
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setFillStyle(fs);
}

void GGroup::setBrush(const QBrush &b) {

    if(brush()==b)
        return;
    GObject::setBrush(b);
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setBrush(b);
}

void GGroup::setGradient(const Gradient &g) {

    if(gradient()==g)
        return;
    GObject::setGradient(g);
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setGradient(g);
}

void GGroup::setPen(const QPen &p) {

    if(pen()==p)
        return;
    GObject::setPen(p);
    QListIterator<GObject> it(m_members);
    for( ; it!=0L; ++it)
        it.current()->setPen(p);
}

void GGroup::recalculate() const {
    // not much to do here, because the group members are all
    // "dirty" and know when to recalculte themselves.
    GObject::recalculate();
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
    m_group->drawHandles(p, handles());
}

bool GGroupM9r::mouseMoveEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // TODO
    return false;
}

bool GGroupM9r::mousePressEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {

    // TODO
    // test
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    kdDebug(37001) << "Name: " << gobject()->name() << endl;
    kdDebug(37001) << "Pen: color: " << gobject()->pen().color().name() << " width: "
                   << gobject()->pen().width() << " style: "
                   << (int)gobject()->pen().style() << endl;
    kdDebug(37001) << "Fill Style: " << gobject()->fillStyle() << endl;
    kdDebug(37001) << "Brush: color: " << gobject()->brush().color().name()
                   << " style: " << (int)gobject()->brush().style() << endl;
    kdDebug(37001) << "Gradient: color a: " << gobject()->gradient().ca.name()
                   << " color b: " << gobject()->gradient().cb.name()
                   << " type: " << (int)gobject()->gradient().type
                   << " xfactor: " << gobject()->gradient().xfactor
                   << " yfactor: " << gobject()->gradient().yfactor
                   << endl;
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    createPropertyDialog();
    exec();
    kdDebug(37001) << "XXXXXXXXXXXXXXXXX Properties XXXXXXXXXXXXXXXXX" << endl;
    kdDebug(37001) << "Name: " << gobject()->name() << endl;
    kdDebug(37001) << "Pen: color: " << gobject()->pen().color().name() << " width: "
                   << gobject()->pen().width() << " style: "
                   << (int)gobject()->pen().style() << endl;
    kdDebug(37001) << "Fill Style: " << gobject()->fillStyle() << endl;
    kdDebug(37001) << "Brush: color: " << gobject()->brush().color().name()
                   << " style: " << (int)gobject()->brush().style() << endl;
    kdDebug(37001) << "Gradient: color a: " << gobject()->gradient().ca.name()
                   << " color b: " << gobject()->gradient().cb.name()
                   << " type: " << (int)gobject()->gradient().type
                   << " xfactor: " << gobject()->gradient().xfactor
                   << " yfactor: " << gobject()->gradient().yfactor
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
