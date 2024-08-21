/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RGBCOMPOSITEOPVALUE_H
#define RGBCOMPOSITEOPVALUE_H

#include "KoColorConversions.h"
#include <KoCompositeOp.h>

#define SCALE_TO_FLOAT(v) KoColorSpaceMaths<channels_type, float>::scaleToA(v)
#define SCALE_FROM_FLOAT(v) KoColorSpaceMaths<float, channels_type>::scaleToA(v)

template<class _CSTraits>
class RgbCompositeOpValue : public KoCompositeOp
{
    typedef typename _CSTraits::channels_type channels_type;
    typedef typename KoColorSpaceMathsTraits<typename _CSTraits::channels_type>::compositetype compositetype;

public:
    RgbCompositeOpValue(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_VALUE, i18n("Value"), "")
    {
    }

    using KoCompositeOp::composite;

    void composite(quint8 *dstRowStart,
                   qint32 dstRowStride,
                   const quint8 *srcRowStart,
                   qint32 srcRowStride,
                   const quint8 *maskRowStart,
                   qint32 maskRowStride,
                   qint32 rows,
                   qint32 numColumns,
                   quint8 opacity,
                   const QBitArray &channelFlags) const override
    {
        while (rows > 0) {
            const quint8 *mask = maskRowStart;
            const channels_type *src = reinterpret_cast<const channels_type *>(srcRowStart);
            channels_type *dst = reinterpret_cast<channels_type *>(dstRowStart);

            for (int i = numColumns; i > 0; --i) {
                channels_type srcAlpha = src[_CSTraits::alpha_pos];
                channels_type dstAlpha = dst[_CSTraits::alpha_pos];

                srcAlpha = qMin(srcAlpha, dstAlpha);

                // apply the alphamask
                if (mask != nullptr) {
                    if (*mask != OPACITY_OPAQUE_U8) {
                        channels_type tmpOpacity = KoColorSpaceMaths<quint8, channels_type>::scaleToA(*mask);
                        srcAlpha = KoColorSpaceMaths<channels_type>::multiply(srcAlpha, tmpOpacity);
                    }
                    mask++;
                }

                if (srcAlpha != NATIVE_OPACITY_TRANSPARENT) {
                    if (opacity != OPACITY_OPAQUE_U8) {
                        channels_type tmpOpacity = KoColorSpaceMaths<quint8, channels_type>::scaleToA(opacity);
                        srcAlpha = KoColorSpaceMaths<channels_type>::multiply(src[_CSTraits::alpha_pos], tmpOpacity);
                    }

                    channels_type srcBlend;

                    if (dstAlpha == NATIVE_OPACITY_OPAQUE) {
                        srcBlend = srcAlpha;
                    } else {
                        channels_type newAlpha = dstAlpha + KoColorSpaceMaths<channels_type>::multiply(NATIVE_OPACITY_OPAQUE - dstAlpha, srcAlpha);
                        dst[_CSTraits::alpha_pos] = newAlpha;

                        if (newAlpha != 0) {
                            srcBlend = KoColorSpaceMaths<channels_type>::divide(srcAlpha, newAlpha);
                        } else {
                            srcBlend = srcAlpha;
                        }
                    }

                    float dstRed = SCALE_TO_FLOAT(dst[_CSTraits::red_pos]);
                    float dstGreen = SCALE_TO_FLOAT(dst[_CSTraits::green_pos]);
                    float dstBlue = SCALE_TO_FLOAT(dst[_CSTraits::blue_pos]);

                    float srcHue;
                    float srcSaturation;
                    float srcValue;
                    float dstHue;
                    float dstSaturation;
                    float dstValue;

                    float srcRed = SCALE_TO_FLOAT(src[_CSTraits::red_pos]);
                    float srcGreen = SCALE_TO_FLOAT(src[_CSTraits::green_pos]);
                    float srcBlue = SCALE_TO_FLOAT(src[_CSTraits::blue_pos]);

                    RGBToHSV(srcRed, srcGreen, srcBlue, &srcHue, &srcSaturation, &srcValue);
                    RGBToHSV(dstRed, dstGreen, dstBlue, &dstHue, &dstSaturation, &dstValue);

                    HSVToRGB(dstHue, dstSaturation, srcValue, &srcRed, &srcGreen, &srcBlue);

                    if (channelFlags.isEmpty() || channelFlags.testBit(_CSTraits::red_pos)) {
                        dst[_CSTraits::red_pos] = KoColorSpaceMaths<channels_type>::blend(SCALE_FROM_FLOAT(srcRed), SCALE_FROM_FLOAT(dstRed), srcBlend);
                    }
                    if (channelFlags.isEmpty() || channelFlags.testBit(_CSTraits::green_pos)) {
                        dst[_CSTraits::green_pos] = KoColorSpaceMaths<channels_type>::blend(SCALE_FROM_FLOAT(srcGreen), SCALE_FROM_FLOAT(dstGreen), srcBlend);
                    }
                    if (channelFlags.isEmpty() || channelFlags.testBit(_CSTraits::blue_pos)) {
                        dst[_CSTraits::blue_pos] = KoColorSpaceMaths<channels_type>::blend(SCALE_FROM_FLOAT(srcBlue), SCALE_FROM_FLOAT(dstBlue), srcBlend);
                    }
                }

                src += _CSTraits::channels_nb;
                dst += _CSTraits::channels_nb;
            }

            rows--;
            srcRowStart += srcRowStride;
            dstRowStart += dstRowStride;
            if (maskRowStart) {
                maskRowStart += maskRowStride;
            }
        }
    }
};

#endif
