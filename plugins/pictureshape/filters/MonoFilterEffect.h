/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MONOFILTEREFFECT_H
#define MONOFILTEREFFECT_H

#include <KoFilterEffect.h>

#define MonoFilterEffectId "MonoFilterEffectId"

class MonoFilterEffect : public KoFilterEffect
{
public:
    MonoFilterEffect();
    ~MonoFilterEffect() override;
    void save(KoXmlWriter &writer) override;
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
};

#endif // MONOFILTEREFFECT_H
