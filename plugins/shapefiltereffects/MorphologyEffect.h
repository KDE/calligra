/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MORPHOLOGYEFFECT_H
#define MORPHOLOGYEFFECT_H

#include "KoFilterEffect.h"
#include <QPointF>

#define MorphologyEffectId "feMorphology"

class KoFilterEffectLoadingContext;

/// A morphology filter effect
class MorphologyEffect : public KoFilterEffect
{
public:
    /// Morphology operator type
    enum Operator { Erode, Dilate };

    MorphologyEffect();

    /// Returns the morphology radius
    QPointF morphologyRadius() const;

    /// Sets the morphology radius
    void setMorphologyRadius(const QPointF &radius);

    /// Returns the morphology operator
    Operator morphologyOperator() const;

    /// Sets the morphology operator
    void setMorphologyOperator(Operator op);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    QPointF m_radius;
    Operator m_operator;
};

#endif // MORPHOLOGYEFFECT_H
