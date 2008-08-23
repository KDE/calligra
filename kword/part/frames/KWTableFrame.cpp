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

#include "KWTableFrame.h"
#include "KWFrameSet.h"

#include <KoViewConverter.h>
#include <KoTextShapeData.h>

#include <kdebug.h>

KWTableFrame::KWTableFrame(KoShape *shape, KWFrameSet *parent)
    : KWFrame(shape, parent),
    m_sortingId( -1 ),
    m_lastHeight(shape->size().height()),
    m_minimumFrameHeight(m_lastHeight),
    m_canGrow(true)
{
}

KWTableFrame::~KWTableFrame() {
}

bool KWTableFrame::canAutoGrow() {
    if(!m_canGrow)
        return false;
    if(shape()->size().height() - m_lastHeight < -0.2) { // shape shrunk!
        m_canGrow = false;
        m_minimumFrameHeight = shape()->size().height();
    }
    return m_canGrow;
}

void KWTableFrame::allowToGrow() {
    m_canGrow = true;
    m_lastHeight = shape()->size().height();
}

void KWTableFrame::autoShrink(qreal requestedHeight) {
//kDebug() <<"autoShrink requested:" << requestedHeight <<", min:" << m_minimumFrameHeight <<", last:" << m_lastHeight;
    QSizeF size = shape()->size();
    if( qAbs(m_lastHeight - size.height()) > 1E-6) { // if not equal
        m_minimumFrameHeight = size.height();
        m_lastHeight = size.height();
        return;
    }
    // TODO make the following work for rotated / skewed frames as well.  The position should be updated.
    shape()->setSize(QSizeF(size.width(), qMax(requestedHeight, m_minimumFrameHeight)));
    m_lastHeight = size.height();
}

