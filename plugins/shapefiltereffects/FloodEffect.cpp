/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FloodEffect.h"
#include "KoFilterEffectRenderContext.h"
#include "KoViewConverter.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"
#include <KLocalizedString>
#include <QPainter>

FloodEffect::FloodEffect()
    : KoFilterEffect(FloodEffectId, i18n("Flood fill"))
    , m_color(Qt::black)
{
}

QColor FloodEffect::floodColor() const
{
    return m_color;
}

void FloodEffect::setFloodColor(const QColor &color)
{
    m_color = color;
}

QImage FloodEffect::processImage(const QImage &image, const KoFilterEffectRenderContext &context) const
{
    QImage result = image;
    QPainter painter(&result);
    painter.fillRect(context.filterRegion(), m_color);

    return result;
}

bool FloodEffect::load(const KoXmlElement &element, const KoFilterEffectLoadingContext &)
{
    if (element.tagName() != id())
        return false;

    m_color = Qt::black;

    if (element.hasAttribute("flood-color")) {
        QString colorStr = element.attribute("flood-color").trimmed();
        if (colorStr.startsWith(QLatin1String("rgb("))) {
            QStringList channels = colorStr.mid(4, colorStr.length() - 5).split(',');
            float r = channels[0].toDouble();
            if (channels[0].contains('%'))
                r /= 100.0;
            else
                r /= 255.0;
            float g = channels[1].toDouble();
            if (channels[1].contains('%'))
                g /= 100.0;
            else
                g /= 255.0;
            float b = channels[2].toDouble();
            if (channels[2].contains('%'))
                b /= 100.0;
            else
                b /= 255.0;
            m_color.setRgbF(r, g, b);

        } else {
            m_color.setNamedColor(colorStr);
        }
        // TODO: add support for currentColor
    }

    if (element.hasAttribute("flood-opacity"))
        m_color.setAlphaF(element.attribute("flood-opacity").toDouble());

    return true;
}

void FloodEffect::save(KoXmlWriter &writer)
{
    writer.startElement(FloodEffectId);

    saveCommonAttributes(writer);

    writer.addAttribute("flood-color", m_color.name());
    if (m_color.alpha() < 255)
        writer.addAttribute("flood-opacity", QString("%1").arg(m_color.alphaF()));

    writer.endElement();
}
