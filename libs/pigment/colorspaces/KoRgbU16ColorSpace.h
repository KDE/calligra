/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KORGBU16COLORSPACE_H
#define KORGBU16COLORSPACE_H

#include <QColor>

#include "KoColorModelStandardIds.h"
#include "KoSimpleColorSpace.h"
#include "KoSimpleColorSpaceFactory.h"

struct KoBgrU16Traits;

/**
 * The alpha mask is a special color strategy that treats all pixels as
 * alpha value with a color common to the mask. The default color is white.
 */
class KoRgbU16ColorSpace : public KoSimpleColorSpace<KoBgrU16Traits>
{
public:
    KoRgbU16ColorSpace();
    ~KoRgbU16ColorSpace() override;

    static QString colorSpaceId();

    virtual KoColorSpace *clone() const;

    void fromQColor(const QColor &color, quint8 *dst, const KoColorProfile *profile = nullptr) const override;

    void toQColor(const quint8 *src, QColor *c, const KoColorProfile *profile = nullptr) const override;

    void toHSY(const QVector<qreal> &channelValues, qreal *hue, qreal *sat, qreal *luma) const override;
    QVector<qreal> fromHSY(qreal *hue, qreal *sat, qreal *luma) const override;
    void toYUV(const QVector<qreal> &channelValues, qreal *y, qreal *u, qreal *v) const override;
    QVector<qreal> fromYUV(qreal *y, qreal *u, qreal *v) const override;
};

class KoRgbU16ColorSpaceFactory : public KoSimpleColorSpaceFactory
{
public:
    KoRgbU16ColorSpaceFactory()
        : KoSimpleColorSpaceFactory(KoRgbU16ColorSpace::colorSpaceId(),
                                    i18n("RGB (16-bit integer/channel, unmanaged)"),
                                    true,
                                    RGBAColorModelID,
                                    Integer16BitsColorDepthID,
                                    16)
    {
    }

    KoColorSpace *createColorSpace(const KoColorProfile *) const override
    {
        return new KoRgbU16ColorSpace();
    }
};

#endif
