/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef RGBCOMPOSITEOPCLEAR_H
#define RGBCOMPOSITEOPCLEAR_H

#include <KoCompositeOp.h>

template<class _CSTraits>
class RgbCompositeOpClear : public KoCompositeOp
{
    typedef typename _CSTraits::channels_type channels_type;
    typedef typename KoColorSpaceMathsTraits<typename _CSTraits::channels_type>::compositetype compositetype;

public:
    RgbCompositeOpClear(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_CLEAR, i18n("Clear"), "")
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
        Q_UNUSED(opacity);
        Q_UNUSED(srcRowStart);
        Q_UNUSED(srcRowStride);
        Q_UNUSED(maskRowStart);
        Q_UNUSED(maskRowStride);

        qint32 channelSize = sizeof(channels_type);
        qint32 linesize = _CSTraits::channels_nb * channelSize * numColumns;

        if (channelFlags.isEmpty()) {
            quint8 *d = dstRowStart;
            while (rows-- > 0) {
                memset(d, 0, linesize);
                d += dstRowStride;
            }
        } else {
            channels_type *d = reinterpret_cast<channels_type *>(dstRowStart);
            while (rows-- > 0) {
                for (int channel = 0; channel < MAX_CHANNEL_RGB; channel++) {
                    if (channelFlags.testBit(channel)) {
                        memset(d, 0, channelSize);
                    }
                    d++;
                }
            }
        }
    }
};

#endif
