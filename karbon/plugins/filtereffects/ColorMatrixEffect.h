/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef COLORMATRIXEFFECT_H
#define COLORMATRIXEFFECT_H

#include "KoFilterEffect.h"

#define ColorMatrixEffectId "feColorMatrix"
#define ColorMatrixElements 20

/// A color matrix effect
class ColorMatrixEffect : public KoFilterEffect
{
public:
    enum Type {
        Matrix,
        Saturate,
        HueRotate,
        LuminanceAlpha
    };

    ColorMatrixEffect();

    /// Returns the type of the color matrix
    Type type() const;

    const qreal * colorMatrix() const;

    /// Sets a color matrix and changes type to Matrix
    void setColorMatrix(qreal *colorMatrix);

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
    virtual QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const;
    /// reimplemented from KoFilterEffect
    virtual bool load(const KoXmlElement &element, const QMatrix &matrix);
    /// reimplemented from KoFilterEffect
    virtual void save(KoXmlWriter &writer);

private:
    /// sets color matrix to identity matrix
    void setIdentity();

    Type m_type;        ///< the color matrix type
    qreal m_matrix[ColorMatrixElements]; ///< the color matrix to apply
    qreal m_value;      ///< the value (saturate or hueRotate)
};

#endif // COLORMATRIXEFFECT_H
