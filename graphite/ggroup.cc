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
#include <ggroup.h>


GGroup::GGroup(const QString &name) : GObject(name) {
    // TODO
}

GGroup::GGroup(const GGroup &rhs) : GObject(rhs) {
    // TODO
}

GGroup::GGroup(const QDomElement &element) : GObject(element.namedItem("gobject").toElement()) {
    // TODO
}

const bool GGroup::isOk() const {
    // TODO
    return false;
}

void GGroup::setOk(const bool &/*ok*/) {
    // TODO
}

GObject *GGroup::clone() const {
    // TODO
    return 0L;
}

GObject *GGroup::instantiate(const QDomElement &/*element*/) const {
    // TODO
    return 0L;
}

const bool GGroup::plugChild(GObject */*child*/, const Position &/*pos*/) {
    // TODO
    return false;
}

const bool GGroup::unplugChild(GObject */*child*/, const Position &/*pos*/) {
    // TODO
    return false;
}

const GObject *GGroup::firstChild() {
    // TODO
    return 0L;
}

const GObject *GGroup::nextChild() {
    // TODO
    return 0L;
}

const GObject *GGroup::lastChild() {
    // TODO
    return 0L;
}

const GObject *GGroup::prevChild() {
    // TODO
    return 0L;
}

const GObject *GGroup::current() {
    // TODO
    return 0L;
}

QDomElement GGroup::save(QDomDocument &/*doc*/) const {
    // TODO
    return QDomElement();
}

void GGroup::draw(QPainter &/*p*/, QRegion &/*reg*/, const bool /*toPrinter*/) {
    // TODO
}

void GGroup::drawHandles(QPainter &/*p*/) {
    // TODO
}

void GGroup::setZoom(const short &/*zoom*/) {
    // TODO
}

const GObject *GGroup::hit(const QPoint &/*p*/) const {
    // TODO
    return 0L;
}

const bool GGroup::intersects(const QRect &/*r*/) const {
    // TODO
    return false;
}

const QRect &GGroup::boundingRect() const {
    // TODO
    return m_boundingRect;
}

GObjectM9r *GGroup::createM9r(const GObjectM9r::Mode &/*mode*/) {
    // TODO
    return 0L;
}

const QPoint GGroup::origin() const {
    // TODO
    return m_boundingRect.topLeft();
}

void GGroup::setOrigin(const QPoint &/*origin*/) {
    // TODO
}

void GGroup::moveX(const int &/*dx*/) {
    // TODO
}

void GGroup::moveY(const int &/*dy*/) {
    // TODO
}

void GGroup::move(const int &/*dx*/, const int &/*dy*/) {
    // TODO
}

void GGroup::rotate(const QPoint &/*center*/, const double &/*angle*/) {
    // TODO
}

void GGroup::setAngle(const double &/*angle*/) {
    // TODO
}

void GGroup::scale(const QPoint &/*origin*/, const double &/*xfactor*/, const double &/*yfactor*/) {
    // TODO
}

void GGroup::resize(const QRect &/*boundingRect*/) {
    // TODO
}

void GGroup::setState(const State /*state*/) {
    // TODO
}

void GGroup::setFillStyle(const FillStyle &/*fillStyle*/) {
    // TODO
}

void GGroup::setBrush(const QBrush &/*brush*/) {
    // TODO
}

void GGroup::setGradient(const Gradient &/*gradient*/) {
    // TODO
}

void GGroup::setPen(const QPen &/*pen*/) {
    // TODO
}
