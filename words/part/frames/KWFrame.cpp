/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2011 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWPage.h"

KWFrame::KWFrame(KoShape *shape, KWFrameSet *parent)
    : m_shape(shape)
    , m_anchoredFrameOffset(0.0)
    , m_frameSet(parent)
{
    Q_ASSERT(shape);
    Q_ASSERT(parent);
    if (shape->applicationData()) {
        delete shape->applicationData();
    }
    shape->setApplicationData(this);
    parent->addShape(shape);
}

KWFrame::~KWFrame()
{
    m_frameSet->cleanupShape(m_shape);
    m_frameSet->removeShape(m_shape);
}
