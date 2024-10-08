/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include "KoAlphaColorSpace.h"

#include <limits.h>
#include <stdlib.h>

#include <QBitArray>
#include <QImage>

#include <KLocalizedString>

#include "KoChannelInfo.h"
#include "KoCompositeOpAlphaDarken.h"
#include "KoCompositeOpCopy2.h"
#include "KoCompositeOpErase.h"
#include "KoCompositeOpOver.h"
#include "KoID.h"
#include "KoIntegerMaths.h"
#include <colorprofiles/KoDummyColorProfile.h>

namespace
{
const quint8 PIXEL_MASK = 0;

class CompositeClear : public KoCompositeOp
{
public:
    CompositeClear(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_CLEAR, i18n("Clear"), KoCompositeOp::categoryMix())
    {
    }

public:
    using KoCompositeOp::composite;

    void composite(quint8 *dst,
                   qint32 dststride,
                   const quint8 *src,
                   qint32 srcstride,
                   const quint8 *maskRowStart,
                   qint32 maskstride,
                   qint32 rows,
                   qint32 cols,
                   quint8 opacity,
                   const QBitArray &channelFlags) const override
    {
        Q_UNUSED(src);
        Q_UNUSED(srcstride);
        Q_UNUSED(opacity);
        Q_UNUSED(channelFlags);

        quint8 *d;
        qint32 linesize;

        if (rows <= 0 || cols <= 0)
            return;

        if (maskRowStart == nullptr) {
            linesize = sizeof(quint8) * cols;
            d = dst;
            while (rows-- > 0) {
                memset(d, OPACITY_TRANSPARENT_U8, linesize);
                d += dststride;
            }
        } else {
            while (rows-- > 0) {
                const quint8 *mask = maskRowStart;

                d = dst;

                for (qint32 i = cols; i > 0; --i, ++d) {
                    // If the mask tells us to completely not
                    // blend this pixel, continue.
                    if (mask != nullptr) {
                        if (mask[0] == OPACITY_TRANSPARENT_U8) {
                            ++mask;
                            continue;
                        }
                        ++mask;
                    }
                    // linesize is uninitialized here, so it just crashes
                    // memset(d, OPACITY_TRANSPARENT, linesize);
                }
                dst += dststride;
                src += srcstride;
                maskRowStart += maskstride;
            }
        }
    }
};

class CompositeSubtract : public KoCompositeOp
{
public:
    CompositeSubtract(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_SUBTRACT, i18n("Subtract"), KoCompositeOp::categoryArithmetic())
    {
    }

public:
    using KoCompositeOp::composite;

    void composite(quint8 *dst,
                   qint32 dststride,
                   const quint8 *src,
                   qint32 srcstride,
                   const quint8 *maskRowStart,
                   qint32 maskstride,
                   qint32 rows,
                   qint32 cols,
                   quint8 opacity,
                   const QBitArray &channelFlags) const override
    {
        Q_UNUSED(opacity);
        Q_UNUSED(channelFlags);

        quint8 *d;
        const quint8 *s;
        qint32 i;

        if (rows <= 0 || cols <= 0)
            return;

        while (rows-- > 0) {
            const quint8 *mask = maskRowStart;
            d = dst;
            s = src;

            for (i = cols; i > 0; --i, ++d, ++s) {
                // If the mask tells us to completely not
                // blend this pixel, continue.
                if (mask != nullptr) {
                    if (mask[0] == OPACITY_TRANSPARENT_U8) {
                        ++mask;
                        continue;
                    }
                    ++mask;
                }

                if (d[PIXEL_MASK] <= s[PIXEL_MASK]) {
                    d[PIXEL_MASK] = OPACITY_TRANSPARENT_U8;
                } else {
                    d[PIXEL_MASK] -= s[PIXEL_MASK];
                }
            }

            dst += dststride;
            src += srcstride;

            if (maskRowStart) {
                maskRowStart += maskstride;
            }
        }
    }
};

class CompositeMultiply : public KoCompositeOp
{
public:
    CompositeMultiply(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_MULT, i18n("Multiply"), KoCompositeOp::categoryArithmetic())
    {
    }

public:
    using KoCompositeOp::composite;

    void composite(quint8 *dst,
                   qint32 dststride,
                   const quint8 *src,
                   qint32 srcstride,
                   const quint8 *maskRowStart,
                   qint32 maskstride,
                   qint32 rows,
                   qint32 cols,
                   quint8 opacity,
                   const QBitArray &channelFlags) const override
    {
        Q_UNUSED(opacity);
        Q_UNUSED(channelFlags);

        quint8 *destination;
        const quint8 *source;
        qint32 i;

        if (rows <= 0 || cols <= 0)
            return;

        while (rows-- > 0) {
            const quint8 *mask = maskRowStart;
            destination = dst;
            source = src;

            for (i = cols; i > 0; --i, ++destination, ++source) {
                // If the mask tells us to completely not
                // blend this pixel, continue.
                if (mask != nullptr) {
                    if (mask[0] == OPACITY_TRANSPARENT_U8) {
                        ++mask;
                        continue;
                    }
                    ++mask;
                }

                // here comes the math
                destination[PIXEL_MASK] = KoColorSpaceMaths<quint8>::multiply(destination[PIXEL_MASK], source[PIXEL_MASK]);
            }

            dst += dststride;
            src += srcstride;

            if (maskRowStart) {
                maskRowStart += maskstride;
            }
        }
    }
};
}

KoAlphaColorSpace::KoAlphaColorSpace()
    : KoColorSpaceAbstract<AlphaU8Traits>("ALPHA", i18n("Alpha mask"))
{
    addChannel(new KoChannelInfo(i18n("Alpha"), 0, 0, KoChannelInfo::ALPHA, KoChannelInfo::UINT8));

    addCompositeOp(new KoCompositeOpOver<AlphaU8Traits>(this));
    addCompositeOp(new CompositeClear(this));
    addCompositeOp(new KoCompositeOpErase<AlphaU8Traits>(this));
    addCompositeOp(new KoCompositeOpCopy2<AlphaU8Traits>(this));
    addCompositeOp(new CompositeSubtract(this));
    addCompositeOp(new CompositeMultiply(this));
    addCompositeOp(new KoCompositeOpAlphaDarken<AlphaU8Traits>(this));

    m_profile = new KoDummyColorProfile;
}

KoAlphaColorSpace::~KoAlphaColorSpace()
{
    delete m_profile;
    m_profile = nullptr;
}

void KoAlphaColorSpace::fromQColor(const QColor &c, quint8 *dst, const KoColorProfile * /*profile*/) const
{
    dst[PIXEL_MASK] = c.alpha();
}

void KoAlphaColorSpace::toQColor(const quint8 *src, QColor *c, const KoColorProfile * /*profile*/) const
{
    c->setRgba(qRgba(255, 255, 255, src[PIXEL_MASK]));
}

quint8 KoAlphaColorSpace::difference(const quint8 *src1, const quint8 *src2) const
{
    // Arithmetic operands smaller than int are converted to int automatically
    return qAbs(src2[PIXEL_MASK] - src1[PIXEL_MASK]);
}

quint8 KoAlphaColorSpace::differenceA(const quint8 *src1, const quint8 *src2) const
{
    return difference(src1, src2);
}

QString KoAlphaColorSpace::channelValueText(const quint8 *pixel, quint32 channelIndex) const
{
    Q_ASSERT(channelIndex < channelCount());
    quint32 channelPosition = channels().at(channelIndex)->pos();

    return QString().setNum(pixel[channelPosition]);
}

QString KoAlphaColorSpace::normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex) const
{
    Q_ASSERT(channelIndex < channelCount());
    quint32 channelPosition = channels().at(channelIndex)->pos();

    return QString().setNum(static_cast<float>(pixel[channelPosition]) / UINT8_MAX);
}

void KoAlphaColorSpace::convolveColors(quint8 **colors,
                                       qreal *kernelValues,
                                       quint8 *dst,
                                       qreal factor,
                                       qreal offset,
                                       qint32 nColors,
                                       const QBitArray &channelFlags) const
{
    qreal totalAlpha = 0;

    while (nColors--) {
        qreal weight = *kernelValues;

        if (weight != 0) {
            totalAlpha += (*colors)[PIXEL_MASK] * weight;
        }
        ++colors;
        ++kernelValues;
    }

    if (channelFlags.isEmpty() || channelFlags.testBit(PIXEL_MASK))
        dst[PIXEL_MASK] = CLAMP((totalAlpha / factor) + offset, 0, SCHAR_MAX);
}

QImage KoAlphaColorSpace::convertToQImage(const quint8 *data,
                                          qint32 width,
                                          qint32 height,
                                          const KoColorProfile * /*dstProfile*/,
                                          KoColorConversionTransformation::Intent /*renderingIntent*/,
                                          KoColorConversionTransformation::ConversionFlags /*conversionFlags*/) const
{
    QImage img(width, height, QImage::Format_Indexed8);
    QVector<QRgb> table;
    for (int i = 0; i < 256; ++i)
        table.append(qRgb(i, i, i));
    img.setColorTable(table);

    quint8 *data_img;
    for (int i = 0; i < height; ++i) {
        data_img = img.scanLine(i);
        for (int j = 0; j < width; ++j)
            data_img[j] = *(data++);
    }

    return img;
}

KoColorSpace *KoAlphaColorSpace::clone() const
{
    return new KoAlphaColorSpace();
}

bool KoAlphaColorSpace::preferCompositionInSourceColorSpace() const
{
    return true;
}
