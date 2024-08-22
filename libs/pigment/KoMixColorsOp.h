/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KO_MIX_COLORS_OP_H
#define KO_MIX_COLORS_OP_H

#include <QtTypes>

/**
 * Base class of the mix color operation. It's defined by
 * sum(colors[i] * weights[i]) / 255. You access the KoMixColorsOp
 * of a colorspace by calling KoColorSpace::mixColorsOp.
 */
class KoMixColorsOp
{
public:
    virtual ~KoMixColorsOp() = default;
    /**
     * Mix the colors.
     * @param colors a pointer toward the source pixels
     * @param weights the coefficient of the source pixels (if you want
     *                to average the sum of weights must be equal to 255)
     * @param nColors the number of pixels in the colors array
     * @param dst the destination pixel
     *
     * @code
     * quint8* colors[nColors];
     * colors[0] = ptrToFirstPixel;
     * colors[1] = ptrToSecondPixel;
     * ...
     * colors[nColors-1] = ptrToLastPixel;
     * qint16 weights[nColors];
     * weights[0] = firstWeight;
     * weights[1] = secondWeight;
     * ...
     * weights[nColors-1] = lastWeight;
     *
     * mixColors(colors, weights, nColors, ptrToDestinationPixel);
     * @endcode
     */
    virtual void mixColors(const quint8 *const *colors, const qint16 *weights, quint32 nColors, quint8 *dst) const = 0;
    virtual void mixColors(const quint8 *colors, const qint16 *weights, quint32 nColors, quint8 *dst) const = 0;
};

#endif
