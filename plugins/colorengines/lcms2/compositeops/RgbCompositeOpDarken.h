/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RGBCOMPOSITEOPDARKEN_H
#define RGBCOMPOSITEOPDARKEN_H

#include <KoCompositeOp.h>

template<class _CSTraits>
class RgbCompositeOpDarken : public KoCompositeOp
{
    typedef typename _CSTraits::channels_type channels_type;
    typedef typename KoColorSpaceMathsTraits<typename _CSTraits::channels_type>::compositetype compositetype;

public:
    RgbCompositeOpDarken(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_DARKEN, i18n("Darken"), "")
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
                        dst[KoBgrU8Traits::alpha_pos] = newAlpha;

                        if (newAlpha != 0) {
                            srcBlend = KoColorSpaceMaths<channels_type>::divide(srcAlpha, newAlpha);
                        } else {
                            srcBlend = srcAlpha;
                        }
                    }

                    for (int channel = 0; channel < MAX_CHANNEL_RGB; channel++) {
                        if (channelFlags.isEmpty() || channelFlags.testBit(_CSTraits::alpha_pos)) {
                            channels_type srcColor = src[channel];
                            channels_type dstColor = dst[channel];

                            srcColor = qMin(srcColor, dstColor);

                            channels_type newColor = KoColorSpaceMaths<channels_type>::blend(srcColor, dstColor, srcBlend);

                            dst[channel] = newColor;
                        }
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
