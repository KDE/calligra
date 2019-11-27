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

#ifndef COLORINGFILTEREFFECT_H
#define COLORINGFILTEREFFECT_H

#include <KoFilterEffect.h>

#define ColoringFilterEffectId "ColoringFilterEffectId"

class ColoringFilterEffect : public KoFilterEffect
{
public:
    ColoringFilterEffect();
    ~ColoringFilterEffect() override;
    void save(KoXmlWriter& writer) override;
    bool load(const KoXmlElement& element, const KoFilterEffectLoadingContext& context) override;
    QImage processImage(const QImage& image, const KoFilterEffectRenderContext& context) const override;
    void setColoring(qreal red, qreal green, qreal blue, qreal luminance, qreal contrast);
    qreal red() const;
    qreal green() const;
    qreal blue() const;
    qreal luminance() const;
    qreal contrast() const;

private:
    qreal m_red;
    qreal m_green;
    qreal m_blue;
    qreal m_luminance;
    qreal m_contrast;
};

#endif // COLORINGFILTEREFFECT_H
