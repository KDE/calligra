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

#include <gprimitivecmds.h>


GRotateCmd::GRotateCmd(GObject *object, const QString &name) : KCommand(name),
                                                               m_object(object) {
}

GRotateCmd::GRotateCmd(GObject *object, const QString &name, const FxPoint &center,
                       const double &angle) : KCommand(name), m_object(object),
                                              m_center(center), m_angle(angle) {
}

void GRotateCmd::execute() {
    if(m_object)
        m_object->rotate(m_center, m_angle);
}

void GRotateCmd::unexecute() {
    if(m_object)
        m_object->rotate(m_center, -m_angle);
}


GScaleCmd::GScaleCmd(GObject *object, const QString &name) : KCommand(name),
                                                             m_object(object) {
}

GScaleCmd::GScaleCmd(GObject *object, const QString &name,
                     const FxPoint &origin, const double &xfactor,
                     const double &yfactor) : KCommand(name), m_object(object),
                                              m_origin(origin), m_xfactor(xfactor),
                                              m_yfactor(yfactor) {
}

void GScaleCmd::execute() {
    if(m_object)
        m_object->scale(m_origin, m_xfactor, m_yfactor);
}

void GScaleCmd::unexecute() {
    if(m_object)
        m_object->scale(m_origin, 1/m_xfactor, 1/m_yfactor);
}
