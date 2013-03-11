#ifndef CALLIGRALIBS_KOCOLORSPACE_H
#define CALLIGRALIBS_KOCOLORSPACE_H

#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QDebug>

class KoMixColorsOp
{
public:
    virtual void mixColors(const quint8 * const*colors, const qint16 *weights, quint32 nColors, quint8 *dst) const {}
};

class KoConvolutionOp
{
public:
    virtual void convolveColors(const quint8* const* colors, const qreal* kernelValues, quint8 *dst, qreal factor, qreal offset, qint32 nColors, const QBitArray & channelFlags) const {}
};

class KoChannelInfo
{
};

class KoColorSpaceFactory
{
};

class KoColorSpace
{
public:
    KoColorSpace(const QString &id, const QString &name, KoMixColorsOp* mixColorsOp, KoConvolutionOp* convolutionOp) {}
    virtual QList<KoChannelInfo *> channels() const = 0;
    virtual quint32 channelCount() const = 0;
    virtual quint32 colorChannelCount() const = 0;
    virtual QBitArray channelFlags(bool color = true, bool alpha = false) const = 0;
    virtual quint32 pixelSize() const = 0;
    virtual QString channelValueText(const quint8 *pixel, quint32 channelIndex) const = 0;
    virtual QString normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex) const = 0;
    virtual void normalisedChannelsValue(const quint8 *pixel, QVector<float> &channels) const = 0;
    virtual void fromNormalisedChannelsValue(quint8 *pixel, const QVector<float> &values) const = 0;
    virtual quint8 scaleToU8(const quint8 * srcPixel, qint32 channelPos) const = 0;
    virtual quint16 scaleToU16(const quint8 * srcPixel, qint32 channelPos) const = 0;
    virtual void singleChannelPixel(quint8 *dstPixel, const quint8 *srcPixel, quint32 channelIndex) const = 0;
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    //virtual KoID colorModelId() const = 0;
    //virtual KoID colorDepthId() const = 0;
    virtual bool profileIsCompatible(const KoColorProfile* profile) const = 0;
    //virtual bool willDegrade(ColorSpaceIndependence independence) const = 0;
    virtual bool hasCompositeOp(const QString & id) const = 0;
    //virtual QList<KoCompositeOp*> compositeOps() const = 0;
    //virtual const KoCompositeOp * compositeOp(const QString & id) const = 0;
    //virtual void addCompositeOp(const KoCompositeOp * op) = 0;
    virtual bool hasHighDynamicRange() const = 0;
    virtual const KoColorProfile * profile() const = 0;
    virtual void fromQColor(const QColor& color, quint8 *dst, const KoColorProfile * profile = 0) const = 0;
    virtual void toQColor(const quint8 *src, QColor *c, const KoColorProfile * profile = 0) const = 0;
    //virtual QImage convertToQImage(const quint8 *data, qint32 width, qint32 height, const KoColorProfile *  dstProfile, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const = 0;
    virtual quint8 *allocPixelBuffer(quint32 numPixels, bool clear = false, quint8 defaultvalue = 0) const = 0;
    virtual void toLabA16(const quint8 * src, quint8 * dst, quint32 nPixels) const = 0;
    virtual void fromLabA16(const quint8 * src, quint8 * dst, quint32 nPixels) const = 0;
    virtual void toRgbA16(const quint8 * src, quint8 * dst, quint32 nPixels) const = 0;
    virtual void fromRgbA16(const quint8 * src, quint8 * dst, quint32 nPixels) const = 0;
    //virtual KoColorConversionTransformation* createColorConverter(const KoColorSpace * dstColorSpace, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const = 0;
    //virtual bool convertPixelsTo(const quint8 * src, quint8 * dst, const KoColorSpace * dstColorSpace, quint32 numPixels, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const = 0;
    virtual quint8 opacityU8(const quint8 * pixel) const = 0;
    virtual qreal opacityF(const quint8 * pixel) const = 0;
    virtual void setOpacity(quint8 * pixels, quint8 alpha, qint32 nPixels) const = 0;
    virtual void setOpacity(quint8 * pixels, qreal alpha, qint32 nPixels) const = 0;
    virtual void multiplyAlpha(quint8 * pixels, quint8 alpha, qint32 nPixels) const = 0;
    virtual void applyAlphaU8Mask(quint8 * pixels, const quint8 * alpha, qint32 nPixels) const = 0;
    virtual void applyInverseAlphaU8Mask(quint8 * pixels, const quint8 * alpha, qint32 nPixels) const = 0;
    virtual void applyAlphaNormedFloatMask(quint8 * pixels, const float * alpha, qint32 nPixels) const = 0;
    virtual void applyInverseNormedFloatMask(quint8 * pixels, const float * alpha, qint32 nPixels) const = 0;
    //virtual KoColorTransformation *createBrightnessContrastAdjustment(const quint16 *transferValues) const = 0;
    //virtual KoColorTransformation *createDesaturateAdjustment() const = 0;
    //virtual KoColorTransformation *createPerChannelAdjustment(const quint16 * const* transferValues) const = 0;
    //virtual KoColorTransformation *createDarkenAdjustment(qint32 shade, bool compensate, qreal compensation) const = 0;
    //virtual KoColorTransformation *createInvertTransformation() const = 0;
    virtual quint8 difference(const quint8* src1, const quint8* src2) const = 0;
    virtual KoMixColorsOp* mixColorsOp() const = 0;
    virtual KoConvolutionOp* convolutionOp() const = 0;
    virtual quint8 intensity8(const quint8 * src) const = 0;
    //virtual KoID mathToolboxId() const = 0;
    //virtual void bitBlt(const KoColorSpace* srcSpace, const KoCompositeOp::ParameterInfo& params, const KoCompositeOp* op, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const = 0;
    //virtual void colorToXML(const quint8* pixel, QDomDocument& doc, QDomElement& colorElt) const = 0;
    virtual void colorFromXML(quint8* pixel, const QDomElement& elt) const = 0;
    //virtual KoColorTransformation* createColorTransformation(const QString & id, const QHash<QString, QVariant> & parameters) const = 0;
};

#if 0
inline QDebug operator<<(QDebug dbg, const KoColorSpace *cs)
{
    dbg.nospace() << cs->name() << " (" << cs->colorModelId().id() << "," << cs->colorDepthId().id() << " )";
    return dbg.space();
}
#endif

#endif
