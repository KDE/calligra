/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BLENDEFFECT_H
#define BLENDEFFECT_H

#include "KoFilterEffect.h"

#define BlendEffectId "feBlend"

/// A color matrix effect
class BlendEffect : public KoFilterEffect
{
public:
    enum BlendMode { Normal, Multiply, Screen, Darken, Lighten };

    BlendEffect();

    /// Returns the type of the color matrix
    BlendMode blendMode() const;

    /// Sets the blend mode
    void setBlendMode(BlendMode blendMode);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    QImage processImages(const QVector<QImage> &images, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    BlendMode m_blendMode; ///< the blend mode
};

#endif // BLENDEFFECT_H
