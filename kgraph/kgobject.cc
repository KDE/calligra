/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

// kinda dummy file at the moment :)
#include <kgobject.h>
#include <qdom.h>


KGObject::~KGObject() {
}

QDomElement KGObject::save(const QDomDocument &/*doc*/) const {
    return QDomElement();
}

void KGObject::setOrigin(const QPoint &/*origin*/) {
}

void KGObject::moveX(const int &/*dx*/) {
}

void KGObject::moveY(const int &/*dy*/) {
}

void KGObject::move(const QSize &/*d*/) {
}

void KGObject::setState(const STATE /*state*/) {
}

void KGObject::setTemporaryGroup(const KGGroup */*group*/) {
}

void KGObject::setFillStyle(const FILL_STYLE &/*fillStyle*/) {
}

void KGObject::setGroup(const KGGroup */*group*/) {
}

void KGObject::setBrush(const QBrush &/*brush*/) {
}

void KGObject::setGradient(const Gradient &/*gradient*/) {
}

void KGObject::setPen(const QPen &/*pen*/) {
}

void KGObject::setName(const QString &/*name*/) {
}

#include <kgobject.moc>
