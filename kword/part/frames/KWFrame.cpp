/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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

#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWCopyShape.h"
#include "KWOutlineShape.h"
#include "KoTextAnchor.h"

#include <KoXmlWriter.h>

KWFrame::KWFrame(KoShape *shape, KWFrameSet *parent, int pageNumber)
// Initialize member vars here. This ensures they are all initialized, since it's
// easier to compare this list with the member vars list (compiler ensures order).
        : m_shape(shape),
        m_frameBehavior(KWord::AutoExtendFrameBehavior),
        m_copyToEverySheet(true),
        m_newFrameBehavior(KWord::NoFollowupFrame),
        m_runAroundSide(KWord::BiggestRunAroundSide),
        m_runAround(KWord::RunAround),
        m_runAroundDistance(1.0),
        m_anchor(0),
        m_anchoredPageNumber(pageNumber),
        m_frameSet(parent),
        m_outline(0)
{
    Q_ASSERT(shape);
    shape->setApplicationData(this);
    if (parent)
        parent->addFrame(this);
}

KWFrame::~KWFrame()
{
    m_shape = 0; // no delete is needed as the shape deletes us.
    if (m_frameSet) {
        bool justMe = m_frameSet->frameCount() == 1;
        m_frameSet->removeFrame(this); // first remove me so we won't get double deleted.
        if (justMe)
            delete m_frameSet;
        m_frameSet = 0;
    }
    delete m_outline;
}

void KWFrame::setFrameSet(KWFrameSet *fs)
{
    if (fs == m_frameSet)
        return;
    if (m_frameSet)
        m_frameSet->removeFrame(this);
    m_frameSet = fs;
    if (fs)
        fs->addFrame(this);
}

void KWFrame::copySettings(const KWFrame *frame)
{
    setFrameBehavior(frame->frameBehavior());
    setNewFrameBehavior(frame->newFrameBehavior());
    setFrameOnBothSheets(frame->frameOnBothSheets());
    setRunAroundDistance(frame->runAroundDistance());
    setRunAroundSide(frame->runAroundSide());
    setTextRunAround(frame->textRunAround());
    shape()->copySettings(frame->shape());
    // TODO copy-shape
}

void KWFrame::saveOdf(KoShapeSavingContext & context)
{
    if (m_anchor)
        return;

    // TODO I think a anchor-type needs to be added to the shape
    // but I'm not sure how kword does handle that stuff.
    shape()->saveOdf(context);
}

void KWFrame::setShape(KoShape *shape)
{
    if (m_shape == shape) return;
    shape->copySettings(m_shape);
    m_shape->setApplicationData(0);
    m_shape->deleteLater();
    m_shape = shape;
    m_shape->setApplicationData(this);
    emit m_frameSet->frameAdded(this);
}

bool KWFrame::isCopy() const
{
    return dynamic_cast<KWCopyShape*>(shape());
}

void KWFrame::setOutlineShape(KWOutlineShape *outline)
{
    m_outline = outline;
}

void KWFrame::attachAnchor(KoTextAnchor *anchor)
{
    m_anchor = anchor;
}

KoTextAnchor *KWFrame::getAnchor()
{
    return m_anchor;
}
