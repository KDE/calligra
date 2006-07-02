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

KWFrame::KWFrame(KoShape *shape, KWFrameSet *parent)
      // Initialize member vars here. This ensures they are all initialized, since it's
      // easier to compare this list with the member vars list (compiler ensures order).

    : m_shape(shape),
    m_frameBehavior( KWord::AutoExtendFrameBehavior ),
    m_copyToEverySheet( true ),
    m_newFrameBehavior( KWord::NoFollowupFrame ),
    m_runAroundSide( KWord::BiggestRunAroundSide ),
    m_runAroundDistance( 1.0 ),
    m_isCopy(false),
    m_frameSet( parent )
{
    if(parent)
        parent->addFrame(this);
}

KWFrame::~KWFrame() {
    delete m_shape;
    m_shape = 0;
}

void KWFrame::setFrameSet(KWFrameSet *fs) {
    if(m_frameSet)
        m_frameSet->removeFrame(this);
    m_frameSet = fs;
    fs->addFrame(this);
}
