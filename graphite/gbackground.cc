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

#include <gbackground.h>
//#include <gobjectfactory.h>


GBackground::GBackground(const QDomElement &element) :
    GGroup(element.namedItem(QString::fromLatin1("ggroup")).toElement()), m_transparent(false) {
}

GObject *GBackground::clone() const {
    return new GBackground(*this);
}

GObject *GBackground::instantiate(const QDomElement &element) const {
    return new GBackground(element);
}

QDomElement GBackground::save(QDomDocument &doc) const {

    static const QString &tagGBackground=KGlobal::staticQString("gbackground");
    QDomElement element=doc.createElement(tagGBackground);
    element.appendChild(GGroup::save(doc));
    return element;
}

void GBackground::draw(QPainter &p, const QRect &rect, bool toPrinter) {

    if(dirty())
        recalculate();

    // do we really have to draw ourselves?
    if(!rect.intersects(boundingRect()))
        return;

    // okay, let's draw a nice background
    if(!toPrinter && !transparent()) {
        p.save();
        if(fillStyle()==GObject::Brush) {
            p.setBrush(brush());
        }
        //else {
            // okay, we have a bg gradient...
            // let's wait for Dirk's answer
        //}
        p.restore();
    }
    GGroup::draw(p, rect, toPrinter);
}

const GObject *GBackground::hit(const QPoint &/*p*/) const {

    /*
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
    */
    return 0L;
}

bool GBackground::intersects(const QRect &r) const {

    if(dirty())
        recalculate();

    if(r.intersects(boundingRect()))
        return true;
    return false;
}

GObjectM9r *GBackground::createM9r(GraphitePart *part, GraphiteView *view,
                              const GObjectM9r::Mode &mode) {
    return new GBackgroundM9r(this, mode, part, view, i18n("Background"));
}


GBackgroundM9r::GBackgroundM9r(GBackground *background, const Mode &mode, GraphitePart *part,
                     GraphiteView *view, const QString &type) :
    G2DObjectM9r(background, mode, part, view, type), m_background(background) {
}

bool GBackgroundM9r::mousePressEvent(QMouseEvent */*e*/, QRect &/*dirty*/) {
    // TODO -- RMB popup
    return false;
}

#include <gbackground.moc>
