/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RGBCOMPOSITEOPCOLOR_H
#define RGBCOMPOSITEOPCOLOR_H

#include "KoColorConversions.h"
#include "KoColorSpaceMaths.h"
#include <KoCompositeOpAlphaBase.h>

#define SCALE_TO_FLOAT(v) KoColorSpaceMaths<channels_type, float>::scaleToA(v)
#define SCALE_FROM_FLOAT(v) KoColorSpaceMaths<float, channels_type>::scaleToA(v)

template<class _CSTraits>
class RgbCompositeOpColor : public KoCompositeOpAlphaBase<_CSTraits, RgbCompositeOpColor<_CSTraits>, true>
{
    typedef typename _CSTraits::channels_type channels_type;
    typedef typename KoColorSpaceMathsTraits<typename _CSTraits::channels_type>::compositetype compositetype;

public:
    RgbCompositeOpColor(KoColorSpace *cs)
        : KoCompositeOpAlphaBase<_CSTraits, RgbCompositeOpColor<_CSTraits>, true>(cs, COMPOSITE_COLOR, i18n("Color"), KoCompositeOp::categoryMisc())
    {
    }

    inline static channels_type selectAlpha(channels_type srcAlpha, channels_type dstAlpha)
    {
        return qMin(srcAlpha, dstAlpha);
    }

    inline static void
    composeColorChannels(channels_type srcBlend, const channels_type *src, channels_type *dst, bool allChannelFlags, const QBitArray &channelFlags)
    {
        float dstRed = SCALE_TO_FLOAT(dst[_CSTraits::red_pos]);
        float dstGreen = SCALE_TO_FLOAT(dst[_CSTraits::green_pos]);
        float dstBlue = SCALE_TO_FLOAT(dst[_CSTraits::blue_pos]);

        float srcHue;
        float srcSaturation;
        float srcLightness;
        float dstHue;
        float dstSaturation;
        float dstLightness;

        float srcRed = SCALE_TO_FLOAT(src[_CSTraits::red_pos]);
        float srcGreen = SCALE_TO_FLOAT(src[_CSTraits::green_pos]);
        float srcBlue = SCALE_TO_FLOAT(src[_CSTraits::blue_pos]);

        RGBToHSL(srcRed, srcGreen, srcBlue, &srcHue, &srcSaturation, &srcLightness);
        RGBToHSL(dstRed, dstGreen, dstBlue, &dstHue, &dstSaturation, &dstLightness);
        HSLToRGB(srcHue, srcSaturation, dstLightness, &srcRed, &srcGreen, &srcBlue);

        if (allChannelFlags || channelFlags.testBit(_CSTraits::red_pos)) {
            dst[_CSTraits::red_pos] = KoColorSpaceMaths<channels_type>::blend(SCALE_FROM_FLOAT(srcRed), SCALE_FROM_FLOAT(dstRed), srcBlend);
        }
        if (allChannelFlags || channelFlags.testBit(_CSTraits::green_pos)) {
            dst[_CSTraits::green_pos] = KoColorSpaceMaths<channels_type>::blend(SCALE_FROM_FLOAT(srcGreen), SCALE_FROM_FLOAT(dstGreen), srcBlend);
        }
        if (allChannelFlags || channelFlags.testBit(_CSTraits::blue_pos)) {
            dst[_CSTraits::blue_pos] = KoColorSpaceMaths<channels_type>::blend(SCALE_FROM_FLOAT(srcBlue), SCALE_FROM_FLOAT(dstBlue), srcBlend);
        }
    }
};

#endif
