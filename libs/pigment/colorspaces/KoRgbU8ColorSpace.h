/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KORGBU8COLORSPACE_H
#define KORGBU8COLORSPACE_H

#include <QColor>

#include "KoColorModelStandardIds.h"
#include "KoSimpleColorSpace.h"
#include "KoSimpleColorSpaceFactory.h"

struct KoBgrU8Traits;

/**
 * The alpha mask is a special color strategy that treats all pixels as
 * alpha value with a color common to the mask. The default color is white.
 */
class KoRgbU8ColorSpace : public KoSimpleColorSpace<KoBgrU8Traits>
{
public:
    KoRgbU8ColorSpace();

    ~KoRgbU8ColorSpace() override;

    static QString colorSpaceId();

    virtual KoColorSpace *clone() const;

    void fromQColor(const QColor &color, quint8 *dst, const KoColorProfile *profile = nullptr) const override;

    void toQColor(const quint8 *src, QColor *c, const KoColorProfile *profile = nullptr) const override;

    void toHSY(const QVector<qreal> &channelValues, qreal *hue, qreal *sat, qreal *luma) const override;
    QVector<qreal> fromHSY(qreal *hue, qreal *sat, qreal *luma) const override;
    void toYUV(const QVector<qreal> &channelValues, qreal *y, qreal *u, qreal *v) const override;
    QVector<qreal> fromYUV(qreal *y, qreal *u, qreal *v) const override;
};

class KoRgbU8ColorSpaceFactory : public KoSimpleColorSpaceFactory
{
public:
    KoRgbU8ColorSpaceFactory()
        : KoSimpleColorSpaceFactory("RGBA", i18n("RGB (8-bit integer/channel, unmanaged)"), true, RGBAColorModelID, Integer8BitsColorDepthID, 8)
    {
    }

    KoColorSpace *createColorSpace(const KoColorProfile *) const override
    {
        return new KoRgbU8ColorSpace();
    }
};

#endif
