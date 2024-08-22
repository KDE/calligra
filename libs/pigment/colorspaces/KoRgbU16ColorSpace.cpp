/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoRgbU16ColorSpace.h"

#include <limits.h>
#include <stdlib.h>

#include <QBitArray>
#include <QImage>

#include <KLocalizedString>

#include "KoChannelInfo.h"
#include "KoID.h"
#include "KoIntegerMaths.h"

#include "KoColorConversions.h"

KoRgbU16ColorSpace::KoRgbU16ColorSpace()
    : KoSimpleColorSpace<KoBgrU16Traits>(colorSpaceId(), i18n("RGB (16-bit integer/channel, unmanaged)"), RGBAColorModelID, Integer16BitsColorDepthID)
{
}

KoRgbU16ColorSpace::~KoRgbU16ColorSpace() = default;

QString KoRgbU16ColorSpace::colorSpaceId()
{
    return QString("RGBA16");
}

KoColorSpace *KoRgbU16ColorSpace::clone() const
{
    return new KoRgbU16ColorSpace();
}

void KoRgbU16ColorSpace::fromQColor(const QColor &c, quint8 *dst, const KoColorProfile * /*profile*/) const
{
    QVector<qreal> channelValues;
    channelValues << c.blueF() << c.greenF() << c.redF() << c.alphaF();
    fromNormalisedChannelsValue(dst, channelValues);
}

void KoRgbU16ColorSpace::toQColor(const quint8 *src, QColor *c, const KoColorProfile * /*profile*/) const
{
    QVector<qreal> channelValues(4);
    normalisedChannelsValue(src, channelValues);
    c->setRgbF(channelValues[2], channelValues[1], channelValues[0], channelValues[3]);
}
void KoRgbU16ColorSpace::toHSY(const QVector<qreal> &channelValues, qreal *hue, qreal *sat, qreal *luma) const
{
    RGBToHSY(channelValues[0], channelValues[1], channelValues[2], hue, sat, luma);
}

QVector<qreal> KoRgbU16ColorSpace::fromHSY(qreal *hue, qreal *sat, qreal *luma) const
{
    QVector<qreal> channelValues(4);
    HSYToRGB(*hue, *sat, *luma, &channelValues[0], &channelValues[1], &channelValues[2]);
    channelValues[3] = 1.0;
    return channelValues;
}

void KoRgbU16ColorSpace::toYUV(const QVector<qreal> &channelValues, qreal *y, qreal *u, qreal *v) const
{
    RGBToYUV(channelValues[0], channelValues[1], channelValues[2], y, u, v);
}

QVector<qreal> KoRgbU16ColorSpace::fromYUV(qreal *y, qreal *u, qreal *v) const
{
    QVector<qreal> channelValues(4);
    YUVToRGB(*y, *u, *v, &channelValues[0], &channelValues[1], &channelValues[2]);
    channelValues[3] = 1.0;
    return channelValues;
}
