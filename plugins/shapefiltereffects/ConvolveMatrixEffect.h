/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CONVOLVEMATRIXEFFECT_H
#define CONVOLVEMATRIXEFFECT_H

#include "KoFilterEffect.h"
#include <QPointF>
#include <QVector>

#define ConvolveMatrixEffectId "feConvolveMatrix"

class KoFilterEffectLoadingContext;

/// A convolve matrix effect
class ConvolveMatrixEffect : public KoFilterEffect
{
public:
    /// Edge mode, i.e. how the kernel behaves at image edges
    enum EdgeMode {
        Duplicate, ///< duplicates colors at the edges
        Wrap, ///< takes the colors at the opposite edge
        None ///< uses values of zero for each color channel
    };

    ConvolveMatrixEffect();

    /// Returns the order of the kernel matrix
    QPoint order() const;

    /// Sets the order of the kernel matrix
    void setOrder(const QPoint &order);

    /// Returns the kernel matrix
    QVector<qreal> kernel() const;

    /// Sets the kernel matrix
    void setKernel(const QVector<qreal> &kernel);

    /// Returns the divisor
    qreal divisor() const;

    /// Sets the divisor
    void setDivisor(qreal divisor);

    /// Returns the bias
    qreal bias() const;

    /// Sets the bias
    void setBias(qreal bias);

    /// Returns the target cell within the kernel
    QPoint target() const;

    /// Sets the target cell within the kernel
    void setTarget(const QPoint &target);

    /// Returns edge mode
    EdgeMode edgeMode() const;

    /// Sets the edge mode
    void setEdgeMode(EdgeMode edgeMode);

    /// Returns if alpha values are preserved
    bool isPreserveAlphaEnabled() const;

    /// Enables/disables preserving alpha values
    void enablePreserveAlpha(bool on);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    void setDefaults();

    QPoint m_order; ///< the dimension of the kernel
    QVector<qreal> m_kernel; ///< the kernel
    qreal m_divisor; ///< the divisor
    qreal m_bias; ///< the bias
    QPoint m_target; ///< target cell within the kernel
    EdgeMode m_edgeMode; ///< the edge mode
    QPointF m_kernelUnitLength; ///< the kernel unit length
    bool m_preserveAlpha; ///< indicates if original alpha values are left intact
};

#endif // CONVOLVEMATRIXEFFECT_H
