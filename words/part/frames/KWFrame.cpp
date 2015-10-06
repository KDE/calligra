/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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
#include "KWPage.h"

KWFrame::KWFrame(KoShape *shape, KWFrameSet *parent)
        : m_shape(shape),
        m_anchoredFrameOffset(0.0),
        m_frameSet(parent)
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
