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

#include <kdebug.h>

KWTextFrame::KWTextFrame(KoShape *shape, KWTextFrameSet *parent, int pageNumber)
        : KWFrame(shape, parent, pageNumber),
        m_sortingId(-1),
        m_lastHeight(shape->size().height()),
        m_minimumFrameHeight(m_lastHeight),
        m_canGrow(true)
{
    Q_ASSERT(shape);
    if (KWord::isHeaderFooter(parent)) {
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
        Q_ASSERT(data);
        data->setResizeMethod(KoTextShapeDataBase::AutoGrowHeight);
    }
}

KWTextFrame::~KWTextFrame()
{
}

bool KWTextFrame::canAutoGrow()
{
#if 0
    if (!m_canGrow)
        return false;
    if (shape()->size().height() - m_lastHeight < -0.2) { // shape shrunk!
        m_canGrow = false;
        m_minimumFrameHeight = shape()->size().height();
    }
    return m_canGrow;
#else
    Q_ASSERT(false);
    return false;
#endif
}

void KWTextFrame::allowToGrow()
{
#if 0
    m_canGrow = true;
    m_lastHeight = shape()->size().height();
#else
    Q_ASSERT(false);
#endif
}

void KWTextFrame::autoShrink(qreal requestedHeight)
{
#if 0
//kDebug() <<"autoShrink requested:" << requestedHeight <<", min:" << m_minimumFrameHeight <<", last:" << m_lastHeight;
    QSizeF size = shape()->size();
    if (qAbs(m_lastHeight - size.height()) > 1E-6) {  // if not equal
        m_minimumFrameHeight = size.height();
        m_lastHeight = size.height();
        return;
    }
    // TODO make the following work for rotated / skewed frames as well.  The position should be updated.
    shape()->setSize(QSizeF(size.width(), qMax(requestedHeight, m_minimumFrameHeight)));
    m_lastHeight = size.height();
#else
    Q_ASSERT(false);
#endif
}

void KWTextFrame::setMinimumFrameHeight(qreal minimumFrameHeight)
{
    m_minimumFrameHeight = minimumFrameHeight;
}

qreal KWTextFrame::minimumFrameHeight() const
{
    return m_minimumFrameHeight;
}
    