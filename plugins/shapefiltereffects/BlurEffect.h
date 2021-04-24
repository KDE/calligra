/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BLUREFFECT_H
#define BLUREFFECT_H

#include "KoFilterEffect.h"
#include <QPointF>

#define BlurEffectId "feGaussianBlur"

/// A gaussian blur effect
class BlurEffect : public KoFilterEffect
{
public:
    BlurEffect();

    QPointF deviation() const;
    void setDeviation(const QPointF &deviation);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    QPointF m_deviation;
};

#endif // BLUREFFECT_H
