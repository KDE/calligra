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


GMoveCmd::GMoveCmd(GObject *object, const QString &name) : KCommand(name),
                                                           m_object(object) {
}

GMoveCmd::GMoveCmd(GObject *object, const QString &name, const FxPoint &from,
                   const FxPoint &to) : KCommand(name), m_object(object),
                                       m_from(from), m_to(to) {
}

void GMoveCmd::execute() {

    if(m_object)
        m_object->setOrigin(m_to);
}

void GMoveCmd::unexecute() {

    if(m_object)
        m_object->setOrigin(m_from);
}


GResizeCmd::GResizeCmd(GObject *object, const QString &name) : KCommand(name),
                                                               m_object(object) {
}

GResizeCmd::GResizeCmd(GObject *object, const QString &name, const FxRect &oldSize,
                       const FxRect &newSize) : KCommand(name), m_object(object),
                                               m_old(oldSize), m_new(newSize) {
}

void GResizeCmd::execute() {

    if(m_object)
        m_object->resize(m_new);
}

void GResizeCmd::unexecute() {

    if(m_object)
        m_object->resize(m_old);
}


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


GSetBrushCmd::GSetBrushCmd(GObject *object, const QString &name) : KCommand(name),
                                                                   m_object(object) {
}

GSetBrushCmd::GSetBrushCmd(GObject *object, const QString &name, const QBrush &oldBrush,
                           const QBrush &newBrush) : KCommand(name), m_object(object),
                                                     m_old(oldBrush), m_new(newBrush) {
}

void GSetBrushCmd::execute() {

    if(m_object)
        m_object->setBrush(m_new);
}

void GSetBrushCmd::unexecute() {

    if(m_object)
        m_object->setBrush(m_old);
}


GSetPenCmd::GSetPenCmd(GObject *object, const QString &name) : KCommand(name),
                                                               m_object(object) {
}

GSetPenCmd::GSetPenCmd(GObject *object, const QString &name, const QPen &oldPen,
                       const QPen &newPen) : KCommand(name), m_object(object),
                                                 m_old(oldPen), m_new(newPen) {
}

void GSetPenCmd::execute() {

    if(m_object)
        m_object->setPen(m_new);
}

void GSetPenCmd::unexecute() {

    if(m_object)
        m_object->setPen(m_old);
}


GSetNameCmd::GSetNameCmd(GObject *object, const QString &name) : KCommand(name),
                                                                 m_object(object) {
}

GSetNameCmd::GSetNameCmd(GObject *object, const QString &name, const QString &oldName,
                         const QString &newName) : KCommand(name), m_object(object),
                                                   m_old(oldName), m_new(newName) {
}

void GSetNameCmd::execute() {

    if(m_object)
        m_object->setName(m_new);
}

void GSetNameCmd::unexecute() {

    if(m_object)
        m_object->setName(m_old);
}


GSetFillStyleCmd::GSetFillStyleCmd(GObject *object, const QString &name)
    : KCommand(name), m_object(object) {
}

GSetFillStyleCmd::GSetFillStyleCmd(GObject *object, const QString &name,
                                   const GObject::FillStyle &oldFillStyle,
                                   const GObject::FillStyle &newFillStyle)
    : KCommand(name), m_object(object), m_old(oldFillStyle), m_new(newFillStyle) {
}

void GSetFillStyleCmd::execute() {

    if(m_object)
        m_object->setFillStyle(m_new);
}

void GSetFillStyleCmd::unexecute() {

    if(m_object)
        m_object->setFillStyle(m_old);
}


GSetGradientCmd::GSetGradientCmd(GObject *object, const QString &name)
    : KCommand(name), m_object(object) {
}

GSetGradientCmd::GSetGradientCmd(GObject *object, const QString &name,
                                 const Gradient &oldGradient,
                                 const Gradient &newGradient)
    : KCommand(name), m_object(object), m_old(oldGradient), m_new(newGradient) {
}

void GSetGradientCmd::execute() {

    if(m_object)
        m_object->setGradient(m_new);
}

void GSetGradientCmd::unexecute() {

    if(m_object)
        m_object->setGradient(m_old);
}
