/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COMPOSITEEFFECT_H
#define COMPOSITEEFFECT_H

#include "KoFilterEffect.h"

#define CompositeEffectId "feComposite"

/// A flood fill effect
class CompositeEffect : public KoFilterEffect
{
public:
    enum Operation { CompositeOver, CompositeIn, CompositeOut, CompositeAtop, CompositeXor, Arithmetic };

    CompositeEffect();

    /// Returns the composite operation
    Operation operation() const;

    /// Sets the composite operation
    void setOperation(Operation op);

    /// Returns the arithmetic values
    const qreal *arithmeticValues() const;

    /// Sets the arithmetic values
    void setArithmeticValues(qreal *values);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    QImage processImages(const QVector<QImage> &images, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    Operation m_operation;
    qreal m_k[4];
};

#endif // COMPOSITEEFFECT_H
