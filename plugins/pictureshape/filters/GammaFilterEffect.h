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

#ifndef GAMMAFILTEREFFECT_H
#define GAMMAFILTEREFFECT_H

#include <KoFilterEffect.h>

#define GammaFilterEffectId "GammaFilterEffectId"

class GammaFilterEffect : public KoFilterEffect
{
public:
    GammaFilterEffect();
    ~GammaFilterEffect() override;
    void save(KoXmlWriter& writer) override;
    bool load(const KoXmlElement& element, const KoFilterEffectLoadingContext& context) override;
    QImage processImage(const QImage& image, const KoFilterEffectRenderContext& context) const override;
    void setGamma(qreal gamma);
    qreal gamma() const;

private:
    qreal m_gamma;
};

#endif // GAMMAFILTEREFFECT_H
