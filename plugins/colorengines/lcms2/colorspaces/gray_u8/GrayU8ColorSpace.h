/*
 *  SPDX-FileCopyrightText: 2004-2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_GRAY_COLORSPACE_H_
#define KIS_GRAY_COLORSPACE_H_

#include "KoColorModelStandardIds.h"
#include <KLocalizedString>
#include <KoColorSpaceTraits.h>
#include <LcmsColorSpace.h>

typedef KoColorSpaceTrait<quint8, 2, 1> GrayAU8Traits;

class GrayAU8ColorSpace : public LcmsColorSpace<GrayAU8Traits>
{
public:
    GrayAU8ColorSpace(const QString &name, KoColorProfile *p);

    bool willDegrade(ColorSpaceIndependence) const override
    {
        return false;
    }

    KoID colorModelId() const override
    {
        return GrayAColorModelID;
    }

    KoID colorDepthId() const override
    {
        return Integer8BitsColorDepthID;
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
        return "GRAYA";
    }
};

class GrayAU8ColorSpaceFactory : public LcmsColorSpaceFactory
{
public:
    GrayAU8ColorSpaceFactory()
        : LcmsColorSpaceFactory(TYPE_GRAYA_8, cmsSigGrayData)
    {
    }

    QString id() const override
    {
        return GrayAU8ColorSpace::colorSpaceId();
    }

    QString name() const override
    {
        return i18n("Grayscale (8-bit integer/channel)");
    }

    KoID colorModelId() const override
    {
        return GrayAColorModelID;
    }

    KoID colorDepthId() const override
    {
        return Integer8BitsColorDepthID;
    }

    int referenceDepth() const override
    {
        return 8;
    }

    bool userVisible() const override
    {
        return true;
    }

    KoColorSpace *createColorSpace(const KoColorProfile *p) const override
    {
        return new GrayAU8ColorSpace(name(), p->clone());
    }

    QString defaultProfile() const override
    {
        return "gray built-in";
    }
};

#endif // KIS_STRATEGY_COLORSPACE_GRAYSCALE_H_
