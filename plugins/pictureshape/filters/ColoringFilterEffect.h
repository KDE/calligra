/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2016 Camilla Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    void save(KoXmlWriter &writer) override;
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
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
