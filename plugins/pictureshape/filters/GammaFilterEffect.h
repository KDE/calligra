/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2016 Camilla Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GAMMAFILTEREFFECT_H
#define GAMMAFILTEREFFECT_H

#include <KoFilterEffect.h>

#define GammaFilterEffectId "GammaFilterEffectId"

class GammaFilterEffect : public KoFilterEffect
{
public:
    GammaFilterEffect();
    ~GammaFilterEffect() override;
    void save(KoXmlWriter &writer) override;
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    void setGamma(qreal gamma);
    qreal gamma() const;

private:
    qreal m_gamma;
};

#endif // GAMMAFILTEREFFECT_H
