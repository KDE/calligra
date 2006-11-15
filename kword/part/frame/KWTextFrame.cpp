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

#include "KWTextFrame.h"
#include "KWTextFrameSet.h"

#include <KoViewConverter.h>
#include <KoTextShapeData.h>

#include <QTextFrame>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextFragment>
#include <kdebug.h>

KWTextFrame::KWTextFrame(KoShape *shape, KWTextFrameSet *parent)
    : KWFrame(shape, parent),
    m_sortingId( -1 )
{
}

KWTextFrame::~KWTextFrame() {
}

bool KWTextFrame::operator<(const KWTextFrame &other) const {
    QPointF pos = shape()->absolutePosition();
    QRectF bounds = other.shape()->boundingRect();
    if(m_sortingId >= 0 && other.sortingId() >= 0) {
        return m_sortingId > other.sortingId();
    }

    // reverse the next 2 return values if the frameset is RTL
    if(pos.x() > bounds.right()) return false;
    if(pos.x() < bounds.left()) return true;

    // check the Y position. Y is greater only when it is below the other frame.
    if(pos.y() > bounds.bottom()) return false;
    if(pos.y() < bounds.top()) return true;

    // my center lies inside other. Lets check the topleft pos.
    if(shape()->boundingRect().top() > bounds.top()) return false;
    return true;
}
