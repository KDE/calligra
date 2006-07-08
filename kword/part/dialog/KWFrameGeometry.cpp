/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "KWFrameGeometry.h"
#include "KWDocument.h"
#include "frame/KWFrame.h"

KWFrameGeometry::KWFrameGeometry(FrameConfigSharedState *state)
    : m_state(state),
    m_frame(0)
{
    m_state->addUser();
    widget.setupUi(this);
    KoUnit::Unit unit = m_state->document()->unit();
    widget.left->setUnit(unit);
    widget.left->setMinimum(0.0);
    widget.top->setUnit(unit);
    widget.top->setMinimum(0.0);
    widget.width->setUnit(unit);
    widget.width->setMinimum(0.0);
    widget.height->setUnit(unit);
    widget.height->setMinimum(0.0);
    widget.leftMargin->setUnit(unit);
    widget.leftMargin->setMinimum(0.0);
    widget.rightMargin->setUnit(unit);
    widget.rightMargin->setMinimum(0.0);
    widget.bottomMargin->setUnit(unit);
    widget.bottomMargin->setMinimum(0.0);
    widget.topMargin->setUnit(unit);
    widget.topMargin->setMinimum(0.0);
}

KWFrameGeometry::~KWFrameGeometry() {
    m_state->removeUser();
}

void KWFrameGeometry::open(KWFrame* frame) {
    m_frame = frame;
    open(frame->shape());
    // TODO rest
}

void KWFrameGeometry::open(KoShape *shape) {
    QPointF absolutePosition = shape->absolutePosition();
    widget.left->changeValue(absolutePosition.x());
    widget.top->changeValue(absolutePosition.y());
    widget.width->changeValue(shape->size().width());
    widget.height->changeValue(shape->size().height());
}

void KWFrameGeometry::save() {
    KWFrame *frame = m_frame;
    if(frame == 0) {
        frame = m_state->frame();
        m_state->markFrameUsed();
    }
    Q_ASSERT(frame);
    QPointF pos(widget.left->value(), widget.top->value());
    frame->shape()->setAbsolutePosition(pos);
}

KAction *KWFrameGeometry::createAction() {
    return 0; // TODO
}


#include "KWFrameGeometry.moc"
