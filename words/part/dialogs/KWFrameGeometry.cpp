/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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
#include "frames/KWTextFrame.h"
#include "frames/KWTextFrameSet.h"

#include <KoShape.h>

#include <kdebug.h>

KWFrameGeometry::KWFrameGeometry(FrameConfigSharedState *state)
        : m_state(state),
        m_frame(0),
        m_blockSignals(false)
{
    m_state->addUser();
    widget.setupUi(this);
    setUnit(m_state->document()->unit());
    widget.xPos->setMinimum(0.0);
    widget.yPos->setMinimum(0.0);
    widget.width->setMinimum(0.0);
    widget.height->setMinimum(0.0);
    widget.leftMargin->setMinimum(0.0);
    widget.rightMargin->setMinimum(0.0);
    widget.bottomMargin->setMinimum(0.0);
    widget.topMargin->setMinimum(0.0);

    widget.keepAspect->setKeepAspectRatio(m_state->keepAspectRatio());

    connect(widget.leftMargin, SIGNAL(valueChangedPt(qreal)), this, SLOT(syncMargins(qreal)));
    connect(widget.rightMargin, SIGNAL(valueChangedPt(qreal)), this, SLOT(syncMargins(qreal)));
    connect(widget.bottomMargin, SIGNAL(valueChangedPt(qreal)), this, SLOT(syncMargins(qreal)));
    connect(widget.topMargin, SIGNAL(valueChangedPt(qreal)), this, SLOT(syncMargins(qreal)));

    connect(widget.width, SIGNAL(valueChangedPt(qreal)), this, SLOT(widthChanged(qreal)));
    connect(widget.height, SIGNAL(valueChangedPt(qreal)), this, SLOT(heightChanged(qreal)));

    connect(m_state, SIGNAL(keepAspectRatioChanged(bool)), widget.keepAspect, SLOT(setKeepAspectRatio(bool)));
    connect(widget.keepAspect, SIGNAL(keepAspectRatioChanged(bool)), this, SLOT(updateAspectRatio(bool)));

    connect(widget.positionSelector, SIGNAL(positionSelected(KoFlake::Position)),
            this, SLOT(setGeometryAlignment(KoFlake::Position)));

}

KWFrameGeometry::~KWFrameGeometry()
{
    m_state->removeUser();
}

void KWFrameGeometry::open(KWFrame *frame)
{
    m_frame = frame;
    open(frame->shape());
    // TODO rest
}

void KWFrameGeometry::open(KoShape *shape)
{
    m_originalPosition = shape->absolutePosition();
    m_originalSize = shape->size();
    QPointF position = shape->absolutePosition(widget.positionSelector->position());
    widget.xPos->changeValue(position.x());
    widget.yPos->changeValue(position.y());
    widget.width->changeValue(m_originalSize.width());
    widget.height->changeValue(m_originalSize.height());

    connect(widget.protectSize, SIGNAL(stateChanged(int)),
            this, SLOT(protectSizeChanged(int)));

    if (shape->isGeometryProtected()) {
        widget.protectSize->setCheckState(Qt::Checked);
        KWTextFrame *tf = dynamic_cast<KWTextFrame*>(shape->applicationData());
        if (tf && static_cast<KWTextFrameSet*>(tf->frameSet())->textFrameSetType() != KWord::OtherTextFrameSet)
            widget.protectSize->setEnabled(false); // auto-generated frame, can't edit
    }

    connect(widget.xPos, SIGNAL(valueChanged(double)), this, SLOT(updateShape()));
    connect(widget.yPos, SIGNAL(valueChanged(double)), this, SLOT(updateShape()));
    connect(widget.width, SIGNAL(valueChanged(double)), this, SLOT(updateShape()));
    connect(widget.height, SIGNAL(valueChanged(double)), this, SLOT(updateShape()));
}

void KWFrameGeometry::updateShape()
{
    if (m_blockSignals) return;
    KWFrame *frame = m_frame;
    if (frame == 0) {
        frame = m_state->frame();
        m_state->markFrameUsed();
    }
    Q_ASSERT(frame);
    frame->shape()->update();
    QPointF pos(widget.xPos->value(), widget.yPos->value());
    frame->shape()->setAbsolutePosition(pos, widget.positionSelector->position());
    QSizeF size(widget.width->value(), widget.height->value());
    frame->shape()->setSize(size);
    frame->shape()->update();
}

void KWFrameGeometry::protectSizeChanged(int protectSizeState)
{
    KWFrame *frame = m_frame;
    if (frame == 0) {
        frame = m_state->frame();
        m_state->markFrameUsed();
    }
    Q_ASSERT(frame);
    bool lock = (protectSizeState == Qt::Checked);
    frame->shape()->setGeometryProtected(lock);
    widget.xPos->setDisabled(lock);
    widget.yPos->setDisabled(lock);
    widget.width->setDisabled(lock);
    widget.height->setDisabled(lock);
    widget.keepAspect->setDisabled(lock);
}

void KWFrameGeometry::syncMargins(qreal value)
{
    if (! widget.synchronize->isChecked())
        return;

    widget.leftMargin->changeValue(value);
    widget.topMargin->changeValue(value);
    widget.rightMargin->changeValue(value);
    widget.bottomMargin->changeValue(value);
}

void KWFrameGeometry::save()
{
    // no-op now
}

void KWFrameGeometry::cancel()
{
    KWFrame *frame = m_frame;
    if (frame == 0) {
        frame = m_state->frame();
        m_state->markFrameUsed();
    }
    Q_ASSERT(frame);
    frame->shape()->setAbsolutePosition(m_originalPosition);
    frame->shape()->setSize(m_originalSize);
}

void KWFrameGeometry::widthChanged(qreal value)
{
    if (! m_state->keepAspectRatio())  return;
    if (m_blockSignals) return;
    m_blockSignals = true;
    widget.height->changeValue(m_originalSize.height() / m_originalSize.width() * value);
    m_blockSignals = false;
}

void KWFrameGeometry::heightChanged(qreal value)
{
    if (! m_state->keepAspectRatio())  return;
    if (m_blockSignals) return;
    m_blockSignals = true;
    widget.width->changeValue(m_originalSize.width() / m_originalSize.height() * value);
    m_blockSignals = false;
}

void KWFrameGeometry::setUnit(KoUnit unit)
{
    widget.xPos->setUnit(unit);
    widget.yPos->setUnit(unit);
    widget.width->setUnit(unit);
    widget.height->setUnit(unit);
    widget.leftMargin->setUnit(unit);
    widget.topMargin->setUnit(unit);
    widget.rightMargin->setUnit(unit);
    widget.bottomMargin->setUnit(unit);
}

void KWFrameGeometry::setGeometryAlignment(KoFlake::Position position)
{
    KWFrame *frame = m_frame;
    if (frame == 0) {
        frame = m_state->frame();
        m_state->markFrameUsed();
    }
    QPointF pos = frame->shape()->absolutePosition(position);
    m_blockSignals = true;
    widget.xPos->changeValue(pos.x());
    widget.yPos->changeValue(pos.y());
    m_blockSignals = false;
}

void KWFrameGeometry::updateAspectRatio(bool keep)
{
    m_state->setKeepAspectRatio(keep);
    if (keep)
        widget.height->changeValue(m_originalSize.height() / m_originalSize.width() * widget.width->value());
}

