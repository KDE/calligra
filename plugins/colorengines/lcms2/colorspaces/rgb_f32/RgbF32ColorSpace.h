/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KORGBF32COLORSPACE_H_
#define KORGBF32COLORSPACE_H_

#include "KoColorModelStandardIds.h"
#include "LcmsColorSpace.h"

struct KoRgbF32Traits;

class RgbF32ColorSpace : public LcmsColorSpace<KoRgbF32Traits>
{
public:
    RgbF32ColorSpace(const QString &name, KoColorProfile *p);

    bool willDegrade(ColorSpaceIndependence independence) const override;

    KoID colorModelId() const override
    {
        return RGBAColorModelID;
    }

    KoID colorDepthId() const override
    {
        return Float32BitsColorDepthID;
    }

    virtual KoColorSpace *clone() const;

    void colorToXML(const quint8 *pixel, QDomDocument &doc, QDomElement &colorElt) const override;

    void colorFromXML(quint8 *pixel, const QDomElement &elt) const override;
    void toHSY(const QVector<double> &channelValues, qreal *hue, qreal *sat, qreal *luma) const override;
    QVector<double> fromHSY(qreal *hue, qreal *sat, qreal *luma) const override;
    void toYUV(const QVector<double> &channelValues, qreal *y, qreal *u, qreal *v) const override;
    QVector<double> fromYUV(qreal *y, qreal *u, qreal *v) const override;

    static QString colorSpaceId()
    {
        return QString("RGBAF32");
    }

    bool hasHighDynamicRange() const override
    {
        return true;
    }
};

class RgbF32ColorSpaceFactory : public LcmsColorSpaceFactory
{
public:
    RgbF32ColorSpaceFactory()
        : LcmsColorSpaceFactory(TYPE_RGBA_FLT, cmsSigRgbData)
    {
    }

    QString id() const override
    {
        return RgbF32ColorSpace::colorSpaceId();
    }

    QString name() const override
    {
        return i18n("RGBA (32-bit floating/channel)");
    }

    bool userVisible() const override
    {
        return true;
    }

    KoID colorModelId() const override
    {
        return RGBAColorModelID;
    }

    KoID colorDepthId() const override
    {
        return Float32BitsColorDepthID;
    }

    int referenceDepth() const override
    {
        return 32;
    }

    KoColorSpace *createColorSpace(const KoColorProfile *p) const override
    {
        return new RgbF32ColorSpace(name(), p->clone());
    }

    QString defaultProfile() const override
    {
        return "sRGB-elle-V2-g10.icc";
    }

    bool isHdr() const override
    {
        return true;
    }
};

#endif
