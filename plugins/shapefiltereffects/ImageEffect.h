/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef IMAGEEFFECT_H
#define IMAGEEFFECT_H

#include "KoFilterEffect.h"
#include <QImage>

#define ImageEffectId "feImage"

/// An image offset effect
class ImageEffect : public KoFilterEffect
{
public:
    ImageEffect();

    /// Returns the image
    QImage image() const;

    /// Sets the image
    void setImage(const QImage &image);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    QImage m_image;
    QRectF m_bound;
};

#endif // IMAGEEFFECT_H
