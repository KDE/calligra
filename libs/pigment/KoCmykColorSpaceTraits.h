/*
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_CMYK_COLORSPACE_TRAITS_H_
#define _KO_CMYK_COLORSPACE_TRAITS_H_

#include <QVector>

#include "KoColorSpaceConstants.h"
#include "KoColorSpaceMaths.h"
#include "KoColorSpaceTraits.h"

/**
 * Base class for CMYK traits, it provides some convenient functions to
 * access CMYK channels through an explicit API.
 */
template<typename _channels_type_>
struct KoCmykTraits : public KoColorSpaceTrait<_channels_type_, 5, 4> {
    typedef _channels_type_ channels_type;
    typedef KoColorSpaceTrait<_channels_type_, 5, 4> parent;

    static const qint32 c_pos = 0;
    static const qint32 m_pos = 1;
    static const qint32 y_pos = 2;
    static const qint32 k_pos = 3;

    /**
     * An CMYK pixel
     */
    struct Pixel {
        channels_type cyan;
        channels_type magenta;
        channels_type yellow;
        channels_type black;
        channels_type alpha;
    };
    /// @return the Cyan component
    inline static channels_type C(quint8 *data)
    {
        channels_type *d = parent::nativeArray(data);
        return d[c_pos];
    }
    /// Set the Cyan component
    inline static void setC(quint8 *data, channels_type nv)
    {
        channels_type *d = parent::nativeArray(data);
        d[c_pos] = nv;
    }
    /// @return the Magenta component
    inline static channels_type M(quint8 *data)
    {
        channels_type *d = parent::nativeArray(data);
        return d[m_pos];
    }
    /// Set the Magenta component
    inline static void setM(quint8 *data, channels_type nv)
    {
        channels_type *d = parent::nativeArray(data);
        d[m_pos] = nv;
    }
    /// @return the Yellow component
    inline static channels_type Y(quint8 *data)
    {
        channels_type *d = parent::nativeArray(data);
        return d[y_pos];
    }
    /// Set the Yellow component
    inline static void setY(quint8 *data, channels_type nv)
    {
        channels_type *d = parent::nativeArray(data);
        d[y_pos] = nv;
    }
    /// @return the Key component
    inline static channels_type k(quint8 *data)
    {
        channels_type *d = parent::nativeArray(data);
        return d[k_pos];
    }
    /// Set the Key component
    inline static void setK(quint8 *data, channels_type nv)
    {
        channels_type *d = parent::nativeArray(data);
        d[k_pos] = nv;
    }
};

struct KoCmykU8Traits : public KoCmykTraits<quint8> {
};

struct KoCmykU16Traits : public KoCmykTraits<quint16> {
};

#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

struct KoCmykF16Traits : public KoCmykTraits<half> {
};

#endif

struct KoCmykF32Traits : public KoCmykTraits<float> {
};

struct KoCmykF64Traits : public KoCmykTraits<double> {
};

#endif
