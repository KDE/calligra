/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2005-2006 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2004, 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_LAB_DARKEN_COLOR_TRANSFORMATION_H_
#define _KO_LAB_DARKEN_COLOR_TRANSFORMATION_H_

#include "KoColorSpace.h"
#include "KoColorTransformation.h"

#include <cstring>

template<typename _lab_channels_type_>
struct KoLabDarkenColorTransformation : public KoColorTransformation {
    KoLabDarkenColorTransformation(qint32 shade, bool compensate, qreal compensation, const KoColorSpace *colorspace)
        : m_colorSpace(colorspace)
        , m_shade(shade)
        , m_compensate(compensate)
        , m_compensation(compensation)
    {
    }
    void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const override
    {
        memcpy(dst, src, sizeof(quint32));
        QColor c;

        for (unsigned int i = 0; i < nPixels * m_colorSpace->pixelSize(); i += m_colorSpace->pixelSize()) {
            if (m_compensate) {
                m_colorSpace->toQColor(src + i, &c);
                c.setRed((c.red() * m_shade) / (m_compensation * 255));
                c.setGreen((c.green() * m_shade) / (m_compensation * 255));
                c.setBlue((c.blue() * m_shade) / (m_compensation * 255));
                m_colorSpace->fromQColor(c, dst + i);
            } else {
                m_colorSpace->toQColor(src + i, &c);
                c.setRed((c.red() * m_shade) / 255);
                c.setGreen((c.green() * m_shade) / 255);
                c.setBlue((c.blue() * m_shade) / 255);
                m_colorSpace->fromQColor(c, dst + i);
            }
        }
    }
    const KoColorSpace *m_colorSpace;
    const KoColorConversionTransformation *m_defaultToLab;
    const KoColorConversionTransformation *m_defaultFromLab;
    qint32 m_shade;
    bool m_compensate;
    qreal m_compensation;
};

#endif
