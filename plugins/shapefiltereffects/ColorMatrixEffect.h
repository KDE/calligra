/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COLORMATRIXEFFECT_H
#define COLORMATRIXEFFECT_H

#include "KoFilterEffect.h"
#include <QVector>

#define ColorMatrixEffectId "feColorMatrix"

/// A color matrix effect
class ColorMatrixEffect : public KoFilterEffect
{
public:
    enum Type { Matrix, Saturate, HueRotate, LuminanceAlpha };

    ColorMatrixEffect();

    /// Returns the type of the color matrix
    Type type() const;

    /// Returns the size of the color matrix
    static int colorMatrixSize();

    /// Returns the row count of the color matrix
    static int colorMatrixRowCount();

    /// Returns the column count of the color matrix
    static int colorMatrixColumnCount();

    QVector<qreal> colorMatrix() const;

    /// Sets a color matrix and changes type to Matrix
    void setColorMatrix(const QVector<qreal> &matrix);

    /// Sets a saturate value and changes type to Saturate
    void setSaturate(qreal value);

    /// Returns the saturate value if type == Saturate
    qreal saturate() const;

    /// Sets a hue rotate value and changes type to HueRotate
    void setHueRotate(qreal value);

    /// Returns the saturate value if type == HueRotate
    qreal hueRotate() const;

    /// Sets luminance alpha an changes type to LuminanceAlpha
    void setLuminanceAlpha();

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    /// sets color matrix to identity matrix
    void setIdentity();

    Type m_type; ///< the color matrix type
    QVector<qreal> m_matrix; ///< the color matrix to apply
    qreal m_value; ///< the value (saturate or hueRotate)
};

#endif // COLORMATRIXEFFECT_H
