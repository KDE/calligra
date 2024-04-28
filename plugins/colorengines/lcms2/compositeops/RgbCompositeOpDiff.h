/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RGBCOMPOSITEOPDIFF_H
#define RGBCOMPOSITEOPDIFF_H

#include <KoCompositeOpAlphaBase.h>

#define AbsoluteValue(x) ((x) < 0 ? -(x) : (x))

template<class _CSTraits>
class RgbCompositeOpDiff : public KoCompositeOpAlphaBase<_CSTraits, RgbCompositeOpDiff<_CSTraits>, true>
{
    typedef typename _CSTraits::channels_type channels_type;
    typedef typename KoColorSpaceMathsTraits<typename _CSTraits::channels_type>::compositetype compositetype;

public:
    RgbCompositeOpDiff(KoColorSpace *cs)
        : KoCompositeOpAlphaBase<_CSTraits, RgbCompositeOpDiff<_CSTraits>, true>(cs, COMPOSITE_DIFF, i18n("Diff"), KoCompositeOp::categoryMisc())
    {
    }

    inline static channels_type selectAlpha(channels_type srcAlpha, channels_type dstAlpha)
    {
        return qMin(srcAlpha, dstAlpha);
    }

    inline static void
    composeColorChannels(channels_type srcBlend, const channels_type *src, channels_type *dst, bool allChannelFlags, const QBitArray &channelFlags)
    {
        for (uint i = 0; i < _CSTraits::channels_nb; i++) {
            if ((int)i != _CSTraits::alpha_pos && (allChannelFlags || channelFlags.testBit(i))) {
                channels_type diff = (channels_type)AbsoluteValue(src[i] - (compositetype)dst[i]);

                dst[i] = KoColorSpaceMaths<channels_type>::blend(diff, dst[i], srcBlend);
            }
        }
    }
};

#endif
