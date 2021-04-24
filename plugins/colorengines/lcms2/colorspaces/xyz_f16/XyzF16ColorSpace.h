/*
*  SPDX-FileCopyrightText: 2007 Cyrille Berger (cberger@cberger.net)
*
* SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KIS_XYZ_F16_COLORSPACE_H_
#define KIS_XYZ_F16_COLORSPACE_H_

#include <LcmsColorSpace.h>

#define TYPE_XYZA_HALF_FLT (FLOAT_SH(1)|COLORSPACE_SH(PT_XYZ)|EXTRA_SH(1)|CHANNELS_SH(3)|BYTES_SH(2))

#include <KoColorModelStandardIds.h>

struct KoXyzF16Traits;

class XyzF16ColorSpace : public LcmsColorSpace<KoXyzF16Traits>
{
public:

    XyzF16ColorSpace(const QString &name, KoColorProfile *p);

    virtual bool willDegrade(ColorSpaceIndependence independence) const;

    virtual KoID colorModelId() const
    {
        return XYZAColorModelID;
    }

    virtual KoID colorDepthId() const
    {
        return Float32BitsColorDepthID;
    }

    virtual KoColorSpace *clone() const;

    virtual void colorToXML(const quint8 *pixel, QDomDocument &doc, QDomElement &colorElt) const;

    virtual void colorFromXML(quint8* pixel, const QDomElement& elt) const;
    virtual void toHSY(const QVector<double> &channelValues, qreal *hue, qreal *sat, qreal *luma) const;
    virtual QVector <double> fromHSY(qreal *hue, qreal *sat, qreal *luma) const;
    virtual void toYUV(const QVector<double> &channelValues, qreal *y, qreal *u, qreal *v) const;
    virtual QVector <double> fromYUV(qreal *y, qreal *u, qreal *v) const;

    static QString colorSpaceId()
    {
        return QString("XYZAF16");
    }

    virtual bool hasHighDynamicRange() const
    {
        return true;
    }
};

class XyzF16ColorSpaceFactory : public LcmsColorSpaceFactory
{
public:

    XyzF16ColorSpaceFactory()
        : LcmsColorSpaceFactory(TYPE_XYZA_HALF_FLT, cmsSigXYZData)
    {
    }

    virtual QString id() const
    {
        return XyzF16ColorSpace::colorSpaceId();
    }

    virtual QString name() const
    {
        return i18n("XYZ (32-bit float/channel)");
    }

    virtual bool userVisible() const
    {
        return true;
    }

    virtual KoID colorModelId() const
    {
        return XYZAColorModelID;
    }

    virtual KoID colorDepthId() const
    {
        return Float16BitsColorDepthID;
    }

    virtual int referenceDepth() const
    {
        return 16;
    }

    virtual KoColorSpace *createColorSpace(const KoColorProfile *p) const
    {
        return new XyzF16ColorSpace(name(), p->clone());
    }

    virtual QString defaultProfile() const
    {
        return "XYZ identity built-in";
    }

    virtual bool isHdr() const
    {
        return true;
    }
};

#endif
