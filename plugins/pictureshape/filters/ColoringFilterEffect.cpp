/* This file is part of the KDE project
 * Copyright (C) 2016 Camilla Boemann <cbo@boemann.dk>
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


#include "ColoringFilterEffect.h"
#include <KoFilterEffectRenderContext.h>

#include <QImage>

#include <QDebug>

ColoringFilterEffect::ColoringFilterEffect()
    : KoFilterEffect(ColoringFilterEffectId, "Coloring effect")
    , m_red(0)
    , m_green(0)
    , m_blue(0)
    , m_luminance(0)
    , m_contrast(0)
{
}

ColoringFilterEffect::~ColoringFilterEffect()
{
}

void ColoringFilterEffect::save(KoXmlWriter& /*writer*/)
{
}

bool ColoringFilterEffect::load(const KoXmlElement& /*element*/, const KoFilterEffectLoadingContext& /*context*/)
{
    return true;
}

void ColoringFilterEffect::setColoring(qreal red, qreal green, qreal blue, qreal luminance, qreal contrast)
{
    m_red =red;
    m_green = green;
    m_blue = blue;
    m_contrast = contrast;
    m_luminance = luminance;
}

qreal ColoringFilterEffect::red() const
{
    return m_red;
}

qreal ColoringFilterEffect::green() const
{
    return m_green;
}

qreal ColoringFilterEffect::blue() const
{
    return m_blue;
}

qreal ColoringFilterEffect::luminance() const
{
    return m_luminance;
}

qreal ColoringFilterEffect::contrast() const
{
    return m_contrast;
}

QImage ColoringFilterEffect::processImage(const QImage& image, const KoFilterEffectRenderContext& context) const
{
    if (m_red==0 && m_green==0 && m_blue==0 && m_contrast==0 && m_luminance==0)
        return image;

    QImage result = image.convertToFormat(QImage::Format_ARGB32);

    const int bottom = context.filterRegion().bottom();
    const int left = context.filterRegion().left();
    const int right = context.filterRegion().right();
    const int width = result.width();
    const QRgb *src = (const QRgb*)image.constBits();
    QRgb *dst = (QRgb*)result.bits();

    const int max = 255;
    const int maxHalf = 128;
    const qreal slope = m_contrast >=0 ? (maxHalf / (maxHalf - m_contrast*(maxHalf -1)))
                               : ((maxHalf + m_contrast*(maxHalf -1)) / maxHalf);
    const int redAdd = (1-slope) * maxHalf + (m_red + m_luminance) * max;
    const int greenAdd = (1-slope) * maxHalf + (m_green + m_luminance) * max;
    const int blueAdd = (1-slope) * maxHalf + (m_blue + m_luminance) * max;

    for (int row = context.filterRegion().top(); row < bottom; ++row) {
        int index = row * width + left;
        for (int col = left; col < right; ++col, ++index) {
            const QRgb &s = src[index];
            const int red = slope * qRed(s) + redAdd;
            const int green = slope * qGreen(s) + greenAdd;
            const int blue = slope * qBlue(s) + blueAdd;
            const int alpha = qAlpha(s);
            dst[index] = qRgba(qBound(0, red, max), qBound(0, green, max), qBound(0, blue, max), alpha);
        }
    }
    return result;
}
