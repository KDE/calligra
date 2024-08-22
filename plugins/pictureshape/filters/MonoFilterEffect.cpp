/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "MonoFilterEffect.h"
#include <KoFilterEffectRenderContext.h>

#include <QImage>

MonoFilterEffect::MonoFilterEffect()
    : KoFilterEffect(MonoFilterEffectId, "Mono Effect")
{
}

MonoFilterEffect::~MonoFilterEffect() = default;

void MonoFilterEffect::save(KoXmlWriter & /*writer*/)
{
}

bool MonoFilterEffect::load(const KoXmlElement & /*element*/, const KoFilterEffectLoadingContext & /*context*/)
{
    return true;
}

QImage MonoFilterEffect::processImage(const QImage &image, const KoFilterEffectRenderContext &context) const
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    QRgb *pixel = reinterpret_cast<QRgb *>(result.bits());
    const int right = context.filterRegion().right();
    const int bottom = context.filterRegion().bottom();
    const int width = result.width();
    for (int row = context.filterRegion().top(); row < bottom; ++row) {
        for (int col = context.filterRegion().left(); col < right; ++col) {
            const QRgb currentPixel = pixel[row * width + col];
            const int red = qRed(currentPixel);
            const int green = qGreen(currentPixel);
            const int blue = qBlue(currentPixel);
            const int alpha = qAlpha(currentPixel);
            const int monoValue = ((red * 11 + green * 16 + blue * 5) / 32) / 127 * 255;
            pixel[row * width + col] = qRgba(monoValue, monoValue, monoValue, alpha);
        }
    }
    return result;
}
