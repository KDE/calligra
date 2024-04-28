/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef WATERMARKFILTEREFFECT_H
#define WATERMARKFILTEREFFECT_H

#include <KoFilterEffect.h>

#define WatermarkFilterEffectId "WatermarkFilterEffectId"

class WatermarkFilterEffect : public KoFilterEffect
{
public:
    WatermarkFilterEffect();
    ~WatermarkFilterEffect() override;

    void save(KoXmlWriter &writer) override;
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
};

#endif // WATERMARKFILTEREFFECT_H
