/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOLABCOLORSPACE_H
#define KOLABCOLORSPACE_H

#include <QColor>

#include "KoColorModelStandardIds.h"
#include "KoSimpleColorSpace.h"
#include "KoSimpleColorSpaceFactory.h"

struct KoLabU16Traits;

/**
 * Basic and simple implementation of the LAB colorspace
 */
class KoLabColorSpace : public KoSimpleColorSpace<KoLabU16Traits>
{
public:
    KoLabColorSpace();

    ~KoLabColorSpace() override;

    static QString colorSpaceId();

    virtual KoColorSpace *clone() const;

    void fromQColor(const QColor &color, quint8 *dst, const KoColorProfile *profile = nullptr) const override;

    void toQColor(const quint8 *src, QColor *c, const KoColorProfile *profile = nullptr) const override;

    void toHSY(const QVector<qreal> &channelValues, qreal *hue, qreal *sat, qreal *luma) const override;
    QVector<qreal> fromHSY(qreal *hue, qreal *sat, qreal *luma) const override;
    void toYUV(const QVector<qreal> &channelValues, qreal *y, qreal *u, qreal *v) const override;
    QVector<qreal> fromYUV(qreal *y, qreal *u, qreal *v) const override;

private:
    static const quint32 CHANNEL_L = 0;
    static const quint32 CHANNEL_A = 1;
    static const quint32 CHANNEL_B = 2;
    static const quint32 CHANNEL_ALPHA = 3;
    static const quint32 MAX_CHANNEL_L = 0xff00;
    static const quint32 MAX_CHANNEL_AB = 0xffff;
    static const quint32 CHANNEL_AB_ZERO_OFFSET = 0x8000;
};

class KoLabColorSpaceFactory : public KoSimpleColorSpaceFactory
{
public:
    KoLabColorSpaceFactory()
        : KoSimpleColorSpaceFactory("LABA", i18n("L*a*b* (16-bit integer/channel, unmanaged)"), true, LABAColorModelID, Integer16BitsColorDepthID, 16)
    {
    }

    KoColorSpace *createColorSpace(const KoColorProfile *) const override
    {
        return new KoLabColorSpace();
    }
};

#endif
