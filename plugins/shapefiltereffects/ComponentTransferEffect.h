/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COMPONENTTRANSFEREFFECT_H
#define COMPONENTTRANSFEREFFECT_H

#include "KoFilterEffect.h"

#define ComponentTransferEffectId "feComponentTransfer"

/// A component transfer effect
class ComponentTransferEffect : public KoFilterEffect
{
public:
    /// the different transfer functions
    enum Function {
        Identity,
        Table,
        Discrete,
        Linear,
        Gamma
    };

    /// the different color channels
    enum Channel {
        ChannelR,
        ChannelG,
        ChannelB,
        ChannelA
    };

    ComponentTransferEffect();

    /// Returns the component transfer function of the specified channel
    Function function(Channel channel) const;

    /// Sets the component transfer function to use for the specified channel
    void setFunction(Channel channel, Function function);

    /// Returns the lookup table for the specified channel
    QList<qreal> tableValues(Channel channel) const;

    /// Sets the lookup table for the specified channel
    void setTableValues(Channel channel, QList<qreal> tableValues);

    /// Sets the slope for the specified channel
    void setSlope(Channel channel, qreal slope);

    /// Returns the slope for the specified channel
    qreal slope(Channel channel) const;

    /// Sets the intercept for the specified channel
    void setIntercept(Channel channel, qreal intercept);

    /// Returns the intercept for the specified channel
    qreal intercept(Channel channel) const;

    /// Sets the amplitude for the specified channel
    void setAmplitude(Channel channel, qreal amplitude);

    /// Returns the amplitude for the specified channel
    qreal amplitude(Channel channel) const;

    /// Sets the exponent for the specified channel
    void setExponent(Channel channel, qreal exponent);

    /// Returns the exponent for the specified channel
    qreal exponent(Channel channel) const;

    /// Sets the offset for the specified channel
    void setOffset(Channel channel, qreal offset);

    /// Returns the offset for the specified channel
    qreal offset(Channel channel) const;

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    /// loads channel transfer function from given xml element
    void loadChannel(Channel channel, const KoXmlElement &element);

    /// saves channel transfer function to given xml writer
    void saveChannel(Channel channel, KoXmlWriter &writer);

    /// transfers color channel
    qreal transferChannel(Channel channel, qreal value) const;

    struct Data {
        Data()
            : function(Identity)
            , slope(1.0)
            , intercept(0.0)
            , amplitude(1.0)
            , exponent(1.0)
            , offset(0.0)
        {
        }

        Function function; ///< the component transfer function
        QList<qreal> tableValues; ///< lookup table for table or discrete function
        qreal slope; ///< slope for linear function
        qreal intercept; ///< intercept for linear function
        qreal amplitude; ///< amplitude for gamma function
        qreal exponent; ///< exponent for gamma function
        qreal offset; ///< offset for gamma function
    };

    Data m_data[4];
};

#endif // COMPONENTTRANSFEREFFECT_H
