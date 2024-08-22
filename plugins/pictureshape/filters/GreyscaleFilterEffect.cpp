/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "GreyscaleFilterEffect.h"
#include <KoFilterEffectRenderContext.h>

#include <QImage>

GreyscaleFilterEffect::GreyscaleFilterEffect()
    : KoFilterEffect(GreyscaleFilterEffectId, "Grayscale effect")
{
}

GreyscaleFilterEffect::~GreyscaleFilterEffect() = default;

void GreyscaleFilterEffect::save(KoXmlWriter & /*writer*/)
{
}

bool GreyscaleFilterEffect::load(const KoXmlElement & /*element*/, const KoFilterEffectLoadingContext & /*context*/)
{
    return true;
}

QImage GreyscaleFilterEffect::processImage(const QImage &image, const KoFilterEffectRenderContext &context) const
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);

    const int bottom = context.filterRegion().bottom();
    const int left = context.filterRegion().left();
    const int right = context.filterRegion().right();
    const int width = result.width();
    const QRgb *src = (const QRgb *)image.constBits();
    QRgb *dst = (QRgb *)result.bits();

    for (int row = context.filterRegion().top(); row < bottom; ++row) {
        for (int col = left; col < right; ++col) {
            int index = row * width + col;
            const QRgb &s = src[index];
            const int red = qRed(s);
            const int green = qGreen(s);
            const int blue = qBlue(s);
            const int alpha = qAlpha(s);
            const int grayValue = (red * 11 + green * 16 + blue * 5) / 32;
            dst[index] = qRgba(grayValue, grayValue, grayValue, alpha);
        }
    }
    return result;
}
