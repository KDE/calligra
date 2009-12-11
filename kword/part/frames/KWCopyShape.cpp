/* This file is part of the KDE project
 * Copyright (C) 2006, 2009 Thomas Zander <zander@kde.org>
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

#include "KWCopyShape.h"
#include "KWPage.h"
#include "KWPageTextInfo.h"
#include "KWPageManager.h"

#include <KoShapeBorderModel.h>
#include <KoViewConverter.h>
#include <KoTextShapeData.h>

#include <QPainter>
#include <QPainterPath>
// #include <KDebug>

KWCopyShape::KWCopyShape(KoShape *original, const KWPageManager *pageManager)
        : m_original(original),
        m_pageManager(pageManager)
{
    setSize(m_original->size());
}

KWCopyShape::~KWCopyShape()
{
}

void KWCopyShape::paint(QPainter &painter, const KoViewConverter &converter)
{
    painter.setClipRect(QRectF(QPointF(0, 0), converter.documentToView(size()))
                        .adjusted(-2, -2, 2, 2), // adjust for anti aliassing.
                        Qt::IntersectClip);
    if (m_pageManager) {
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(m_original->userData());
        if (data) {
            KWPage currentPage = m_pageManager->page(this);
            KWPageTextInfo info(currentPage);
            data->relayoutFor(info);
        }
    }

    painter.save();
    m_original->paint(painter, converter);
    painter.restore();
    if (m_original->border())
        m_original->border()->paintBorder(m_original, painter, converter);
}

void KWCopyShape::paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas)
{
    m_original->paintDecorations(painter, converter, canvas);
}

QPainterPath KWCopyShape::outline() const
{
    return m_original->outline();
}

void KWCopyShape::saveOdf(KoShapeSavingContext &context) const
{
    // TODO
}

bool KWCopyShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
    return false; // TODO
}

