/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "WatermarkFilterEffect.h"
#include <KoFilterEffectRenderContext.h>
#include <QColor>
#include <kcolorutils.h>

WatermarkFilterEffect::WatermarkFilterEffect()
    : KoFilterEffect(WatermarkFilterEffectId, "Watermark Effect")
{
}

WatermarkFilterEffect::~WatermarkFilterEffect() = default;

void WatermarkFilterEffect::save(KoXmlWriter & /*writer*/)
{
}

bool WatermarkFilterEffect::load(const KoXmlElement & /*element*/, const KoFilterEffectLoadingContext & /*context*/)
{
    return true;
}

QImage WatermarkFilterEffect::processImage(const QImage &image, const KoFilterEffectRenderContext &context) const
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    QRgb *pixel = reinterpret_cast<QRgb *>(result.bits());
    const int right = context.filterRegion().right();
    const int bottom = context.filterRegion().bottom();
    const int width = result.width();

    for (int row = context.filterRegion().top(); row < bottom; ++row) {
        for (int col = context.filterRegion().left(); col < right; ++col) {
            //             const QColor currentPixel = pixel[row * width + col];
            //             const QColor currentPixelLighter = KColorUtils::lighten(currentPixel, 0.75);
            //             pixel[row * width + col] = currentPixelLighter.rgb();

            quint32 color = pixel[row * width + col];
            quint32 rgb = 0x00FFFFFF & color; // get rgb value without alpha
            quint32 alpha = (color >> 25) << 24; // get alpha value only and divide it by two
            pixel[row * width + col] = alpha | rgb; // set rgb and alpha values to the pixel
        }
    }
    return result;
}
