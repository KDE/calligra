/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2007 Emanuele Tamponi <emanuele@valinor.it>
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KO_COMPOSITE_OP_COPY_H
#define KO_COMPOSITE_OP_COPY_H

/**
 * Generic implementation of the COPY composite op.
 * Used automatically by all colorspaces that derive from KoColorSpaceAbstract.
 */
class KoCompositeOpCopy : public KoCompositeOp
{
    using KoCompositeOp::composite;

public:
    explicit KoCompositeOpCopy(KoColorSpace *cs)
        : KoCompositeOp(cs, COMPOSITE_COPY, i18n("Copy"), KoCompositeOp::categoryMix())
    {
    }

public:
    void composite(quint8 *dstRowStart,
                   qint32 dstRowStride,
                   const quint8 *srcRowStart,
                   qint32 srcRowStride,
                   const quint8 *maskRowStart,
                   qint32 maskRowStride,
                   qint32 rows,
                   qint32 numColumns,
                   quint8 opacity,
                   const QBitArray &channelFlags) const
    {
        Q_UNUSED(channelFlags);
        Q_UNUSED(opacity);

        const KoColorSpace *cs = colorSpace();
        qint32 bytesPerPixel = cs->pixelSize();

        qint32 srcInc = (srcRowStride == 0) ? 0 : bytesPerPixel;

        quint8 *dst = dstRowStart;
        const quint8 *src = srcRowStart;
        const quint8 *mask = maskRowStart;

        if (maskRowStart != 0) {
            while (rows > 0) {
                quint8 *dstN = dst;
                const quint8 *srcN = src;
                const quint8 *maskN = mask;
                qint32 columns = numColumns;

                while (columns > 0) {
                    if (*maskN != 0) {
                        memcpy(dstN, srcN, bytesPerPixel);
                    }

                    dstN += bytesPerPixel;
                    srcN += srcInc; // if srcRowStride == 0, don't move the pixel
                    maskN++; // byte
                    columns--;
                }

                dst += dstRowStride;
                src += srcRowStride;
                mask += maskRowStride;
                --rows;
            }
        } else {
            while (rows > 0) {
                if (srcInc == 0) {
                    quint8 *dstN = dst;
                    qint32 columns = numColumns;
                    while (columns > 0) {
                        memcpy(dstN, src, bytesPerPixel);
                        dstN += bytesPerPixel;
                        columns--;
                    }
                } else {
                    memcpy(dst, src, numColumns * bytesPerPixel);
                }

                // XXX: what is the reason for this code? I think we should copy the alpha channel as well.
                // if (opacity != OPACITY_OPAQUE) {
                //    cs->multiplyAlpha(dst, opacity, numColumns);
                //}

                dst += dstRowStride;
                src += srcRowStride;
                --rows;
            }
        }
    }
};

#endif
