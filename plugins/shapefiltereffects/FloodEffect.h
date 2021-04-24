/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FLOODEFFECT_H
#define FLOODEFFECT_H

#include "KoFilterEffect.h"
#include <QColor>

#define FloodEffectId "feFlood"

class KoFilterEffectLoadingContext;

/// A flood fill effect
class FloodEffect : public KoFilterEffect
{
public:
    FloodEffect();

    QColor floodColor() const;
    void setFloodColor(const QColor &color);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    QColor m_color;
};

#endif // FLOODEFFECT_H
