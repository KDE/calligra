/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "OffsetEffect.h"
#include "KoFilterEffectLoadingContext.h"
#include "KoFilterEffectRenderContext.h"
#include "KoViewConverter.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"
#include <KLocalizedString>
#include <QPainter>

OffsetEffect::OffsetEffect()
    : KoFilterEffect(OffsetEffectId, i18n("Offset"))
    , m_offset(0, 0)
{
}

QPointF OffsetEffect::offset() const
{
    return m_offset;
}

void OffsetEffect::setOffset(const QPointF &offset)
{
    m_offset = offset;
}

QImage OffsetEffect::processImage(const QImage &image, const KoFilterEffectRenderContext &context) const
{
    if (m_offset.x() == 0.0 && m_offset.y() == 0.0)
        return image;

    // transform from bounding box coordinates
    QPointF offset = context.toUserSpace(m_offset);
    // transform to view coordinates
    offset = context.viewConverter()->documentToView(offset);

    QImage result(image.size(), image.format());
    result.fill(qRgba(0, 0, 0, 0));

    QPainter p(&result);
    p.drawImage(context.filterRegion().topLeft() + offset, image, context.filterRegion());
    return result;
}

bool OffsetEffect::load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context)
{
    if (element.tagName() != id())
        return false;

    if (element.hasAttribute("dx"))
        m_offset.rx() = element.attribute("dx").toDouble();
    if (element.hasAttribute("dy"))
        m_offset.ry() = element.attribute("dy").toDouble();

    m_offset = context.convertFilterPrimitiveUnits(m_offset);

    return true;
}

void OffsetEffect::save(KoXmlWriter &writer)
{
    writer.startElement(OffsetEffectId);

    saveCommonAttributes(writer);

    if (m_offset.x() != 0.0)
        writer.addAttribute("dx", m_offset.x());
    if (m_offset.y() != 0.0)
        writer.addAttribute("dy", m_offset.x());

    writer.endElement();
}
