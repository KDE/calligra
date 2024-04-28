/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2007 Emanuele Tamponi <emanuele@valinor.it>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KO_INVERT_COLOR_TRANSFORMATION_H
#define KO_INVERT_COLOR_TRANSFORMATION_H

#include "KoColorTransformation.h"

class KoInvertColorTransformation : public KoColorTransformation
{
public:
    KoInvertColorTransformation(const KoColorSpace *cs)
        : m_colorSpace(cs)
        , m_psize(cs->pixelSize())
    {
    }

    void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const override
    {
        quint16 m_rgba[4];
        while (nPixels--) {
            m_colorSpace->toRgbA16(src, reinterpret_cast<quint8 *>(m_rgba), 1);
            m_rgba[0] = KoColorSpaceMathsTraits<quint16>::max - m_rgba[0];
            m_rgba[1] = KoColorSpaceMathsTraits<quint16>::max - m_rgba[1];
            m_rgba[2] = KoColorSpaceMathsTraits<quint16>::max - m_rgba[2];
            m_colorSpace->fromRgbA16(reinterpret_cast<quint8 *>(m_rgba), dst, 1);
            src += m_psize;
            dst += m_psize;
        }
    }

private:
    const KoColorSpace *m_colorSpace;
    quint32 m_psize;
};

#endif
