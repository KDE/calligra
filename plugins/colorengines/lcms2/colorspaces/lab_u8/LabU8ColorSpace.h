/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LabU8ColorSpace_H_
#define LabU8ColorSpace_H_

#include "KoColorModelStandardIds.h"
#include "LcmsColorSpace.h"

#define TYPE_LABA_8 (COLORSPACE_SH(PT_Lab) | CHANNELS_SH(3) | BYTES_SH(1) | EXTRA_SH(1))

struct KoLabU8Traits;

class LabU8ColorSpace : public LcmsColorSpace<KoLabU8Traits>
{
public:
    LabU8ColorSpace(const QString &name, KoColorProfile *p);
    bool willDegrade(ColorSpaceIndependence independence) const override;
    QString normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex) const override;

    static QString colorSpaceId()
    {
        return QString("LABAU8");
    }

    KoID colorModelId() const override
    {
        return LABAColorModelID;
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

private:
    static const quint32 MAX_CHANNEL_L = 100;
    static const quint32 MAX_CHANNEL_AB = 255;
    static const quint32 CHANNEL_AB_ZERO_OFFSET = 128;
};

class LabU8ColorSpaceFactory : public LcmsColorSpaceFactory
{
public:
    LabU8ColorSpaceFactory()
        : LcmsColorSpaceFactory(TYPE_LABA_8, cmsSigLabData)
    {
    }

    bool userVisible() const override
    {
        return true;
    }

    QString id() const override
    {
        return LabU8ColorSpace::colorSpaceId();
    }

    QString name() const override
    {
        return i18n("L*a*b* (8-bit integer/channel)");
    }

    KoID colorModelId() const override
    {
        return LABAColorModelID;
    }

    KoID colorDepthId() const override
    {
        return Integer8BitsColorDepthID;
    }

    int referenceDepth() const override
    {
        return 8;
    }

    KoColorSpace *createColorSpace(const KoColorProfile *p) const override
    {
        return new LabU8ColorSpace(name(), p->clone());
    }

    QString defaultProfile() const override
    {
        return "Lab identity built-in";
    }
};

#endif
