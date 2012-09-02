/*
 *  Copyright (c) 2004,2007-2009 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#if defined(_WIN32) || defined(_WIN64)
#include <stdlib.h>
#define srand48 srand
inline double drand48() {
    return double(rand()) / RAND_MAX;
}
#endif

#include "kis_auto_brush.h"

#include <kis_debug.h>
#include <math.h>

#include <QRect>
#include <QDomElement>
#include <QtConcurrentMap>

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include "kis_datamanager.h"
#include "kis_paint_device.h"

#include "kis_mask_generator.h"
#include "kis_boundary.h"

// 3x3 supersampling
#define SUPERSAMPLING 3

struct MaskProcessor
{
    MaskProcessor(KisFixedPaintDeviceSP device, const KoColorSpace* cs, qreal randomness, qreal density,
           double centerX, double centerY, double invScaleX, double invScaleY, double angle,
           KisMaskGenerator* shape)
    : m_device(device)
    , m_cs(cs)
    , m_randomness(randomness)
    , m_density(density)
    , m_pixelSize(cs->pixelSize())
    , m_centerX(centerX)
    , m_centerY(centerY)
    , m_invScaleX(invScaleX)
    , m_invScaleY(invScaleY)
    , m_shape(shape)
    {

        m_cosa = cos(angle);
        m_sina = sin(angle);
    }

    void operator()(QRect& rect)
    {
        process(rect);
    }

    void process(QRect& rect){
        qreal random = 1.0;
        quint8* dabPointer = m_device->data() + rect.y() * rect.width() * m_pixelSize;
        quint8 alphaValue = OPACITY_TRANSPARENT_U8;
        // this offset is needed when brush size is smaller then fixed device size
        int offset = (m_device->bounds().width() - rect.width()) * m_pixelSize;
        int supersample = (m_shape->shouldSupersample() ? SUPERSAMPLING : 1);
        double invss = 1.0 / supersample;
        int samplearea = supersample * supersample;
        for (int y = rect.y(); y < rect.y() + rect.height(); y++) {
            for (int x = rect.x(); x < rect.x() + rect.width(); x++) {
                int value = 0;
                for (int sy = 0; sy < supersample; sy++) {
                    for (int sx = 0; sx < supersample; sx++) {
                        double x_ = (x + sx * invss - m_centerX) * m_invScaleX;
                        double y_ = (y + sy * invss - m_centerY) * m_invScaleY;
                        double maskX = m_cosa * x_ - m_sina * y_;
                        double maskY = m_sina * x_ + m_cosa * y_;
                        value += m_shape->valueAt(maskX, maskY);
                    }
                }
                if (supersample != 1) value /= samplearea;

                if (m_randomness!= 0.0){
                    random = (1.0 - m_randomness) + m_randomness * float(rand()) / RAND_MAX;
                }

                alphaValue = quint8( (OPACITY_OPAQUE_U8 - value) * random);

                // avoid computation of random numbers if density is full
                if (m_density != 1.0){
                    // compute density only for visible pixels of the mask
                    if (alphaValue != OPACITY_TRANSPARENT_U8){
                        if ( !(m_density >= drand48()) ){
                            alphaValue = OPACITY_TRANSPARENT_U8;
                        }
                    }
                }

                m_cs->applyAlphaU8Mask(dabPointer, &alphaValue, 1);
                dabPointer += m_pixelSize;
            }//endfor x
            dabPointer += offset;
        }//endfor y
    }

    KisFixedPaintDeviceSP m_device;
    const KoColorSpace* m_cs;
    qreal m_randomness;
    qreal m_density;
    quint32 m_pixelSize;
    double m_centerX;
    double m_centerY;
    double m_invScaleX;
    double m_invScaleY;
    double m_cosa;
    double m_sina;
    KisMaskGenerator* m_shape;
};

struct SSEMaskProcessor
{
    SSEMaskProcessor(KisFixedPaintDeviceSP device, const KoColorSpace* cs, qreal randomness, qreal density,
           double centerX, double centerY, double invScaleX, double invScaleY, double angle,
           KisMaskGenerator* shape)
    : m_device(device)
    , m_cs(cs)
    , m_randomness(randomness)
    , m_density(density)
    , m_pixelSize(cs->pixelSize())
    , m_centerX(centerX)
    , m_centerY(centerY)
    , m_invScaleX(invScaleX)
    , m_invScaleY(invScaleY)
    , m_shape(shape)
    {

        m_cosa = cos(angle);
        m_sina = sin(angle);
    }

    void operator()(QRect& rect)
    {
        process(rect);
    }

    void process(QRect& rect){
        qreal random = 1.0;
        quint8* dabPointer = m_device->data() + rect.y() * rect.width() * m_pixelSize;
        quint8 alphaValue = OPACITY_TRANSPARENT_U8;
        
        float *buffer;
        float *bufferPointer;
        __m128 rCurrentIndices, increment;
        __m128 rCenterX, rX_, rInvScaleX, rCosa, rSina, rXr, rYr, rCosaY_, rSinaY_, rXCoeff, rYCoeff, rNorm1,
        rNorm2, rOne, r255, rFade, rN, rTransformedFadeX, rTransformedFadeY, rNormFade, rDividend, rCircleMask;

        int width = rect.width();

        buffer = (float*) _mm_malloc(width*sizeof(float),16);

        float xCoeff = 2.0 / width;
        float yCoeff = 2.0 / width * 2.0f;

        float transformedFadeX = 0.004f;
        float transformedFadeY = 0.004f;
        
        // this offset is needed when brush size is smaller then fixed device size
        int offset = (m_device->bounds().width() - rect.width()) * m_pixelSize; 
        for (int y = rect.y(); y < rect.y() + rect.height(); y++) {
            bufferPointer = buffer;
            
            float y_ = (y - m_centerY) * m_invScaleY;
            float sinay_ = m_sina * y_;
            float cosay_ = m_cosa * y_; 


            rCurrentIndices = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);
            increment = _mm_set1_ps(4.0f);
            rCenterX = _mm_set1_ps(m_centerX);
            rInvScaleX = _mm_set1_ps(m_invScaleX);
            rCosa = _mm_set1_ps(m_cosa);
            rSina = _mm_set1_ps(m_sina);
            rCosaY_ = _mm_set1_ps(cosay_);
            rSinaY_ = _mm_set1_ps(sinay_);
            rXCoeff = _mm_set1_ps(xCoeff);
            rYCoeff = _mm_set1_ps(yCoeff);
            rOne = _mm_set1_ps(1.0f);
            r255 = _mm_set1_ps(255.0f);
            rTransformedFadeX = _mm_set1_ps(transformedFadeX);
            rTransformedFadeY = _mm_set1_ps(transformedFadeY);
            for (int i=0; i < width; i+=4){
                rX_ = _mm_sub_ps(rCurrentIndices, rCenterX);
                rX_ = _mm_mul_ps(rX_, rInvScaleX);

                rXr = _mm_mul_ps(rX_, rCosa);
                rXr = _mm_add_ps(rXr, rCosaY_);

                rYr = _mm_mul_ps(rX_, rSina);
                rYr = _mm_add_ps(rYr, rCosaY_);

                // float n = norme(xr * d->xcoef, yr * d->ycoef);
                rNorm1 = _mm_mul_ps(rXr, rXCoeff);
                rNorm1 = _mm_mul_ps(rNorm1, rNorm1);
                
                rNorm2 = _mm_mul_ps(rYr, rYCoeff);
                rNorm2 = _mm_mul_ps(rNorm2, rNorm2);

                rN = _mm_add_ps(rNorm1, rNorm2);
                
                // normeFade = norme(xr * d->transformedFadeX, yr * d->transformedFadeY)
                rNorm1 = _mm_mul_ps(rXr, rTransformedFadeX);
                rNorm1 = _mm_mul_ps(rNorm1, rNorm1);
                
                rNorm2 = _mm_mul_ps(rYr, rTransformedFadeY);
                rNorm2 = _mm_mul_ps(rNorm2, rNorm2);

                rNormFade = _mm_add_ps(rNorm1, rNorm2);
                
                //255 * n * (normeFade - 1) / (normeFade - n)
                rFade = _mm_sub_ps(rNormFade, rOne);
                rFade = _mm_mul_ps(rFade, rN);
                rFade = _mm_mul_ps(rFade, r255);
                
                rDividend = _mm_sub_ps(rNormFade, rN);
                rFade = _mm_div_ps(rFade, rDividend);
                rFade = _mm_min_ps(rFade, r255);
                
                // Mask out the inner circe of the mask
                rCircleMask = _mm_cmpgt_ps(rNormFade, rOne);
                rFade = _mm_and_ps(rFade, rCircleMask);

                _mm_store_ps(bufferPointer,rFade);
                _mm_store_ps(bufferPointer,rCurrentIndices);
                
                rCurrentIndices = _mm_add_ps(rCurrentIndices, increment);

                bufferPointer += 4;
            }
            
            for (int x = 0; x < width; x++) {

                if (m_randomness!= 0.0){
                    random = (1.0 - m_randomness) + m_randomness * float(rand()) / RAND_MAX;
                }

                alphaValue = quint8( (OPACITY_OPAQUE_U8 - buffer[x]) * random);

                // avoid computation of random numbers if density is full
                if (m_density != 1.0){
                    // compute density only for visible pixels of the mask
                    if (alphaValue != OPACITY_TRANSPARENT_U8){
                        if ( !(m_density >= drand48()) ){
                            alphaValue = OPACITY_TRANSPARENT_U8;
                        }
                    }
                }

                m_cs->applyAlphaU8Mask(dabPointer, &alphaValue, 1);
                dabPointer += m_pixelSize;
            }//endfor x
            dabPointer += offset;
        }//endfor y
        _mm_free(buffer);
    }

    KisFixedPaintDeviceSP m_device;
    const KoColorSpace* m_cs;
    qreal m_randomness;
    qreal m_density;
    quint32 m_pixelSize;
    double m_centerX;
    double m_centerY;
    double m_invScaleX;
    double m_invScaleY;
    double m_cosa;
    double m_sina;
    KisMaskGenerator* m_shape;
};

struct KisAutoBrush::Private {
    KisMaskGenerator* shape;
    qreal randomness;
    qreal density;
    int idealThreadCountCached;
    mutable QVector<quint8> precomputedQuarter;
};

KisAutoBrush::KisAutoBrush(KisMaskGenerator* as, qreal angle, qreal randomness, qreal density)
        : KisBrush()
        , d(new Private)
{
    d->shape = as;
    d->randomness = randomness;
    d->density = density;
    d->idealThreadCountCached = QThread::idealThreadCount();
    setBrushType(MASK);
    setWidth(d->shape->width());
    setHeight(d->shape->height());
    setAngle(angle);
    QImage image = createBrushPreview();
    setImage(image);
}

KisAutoBrush::~KisAutoBrush()
{
    delete d->shape;
    delete d;
}



void KisAutoBrush::generateMaskAndApplyMaskOrCreateDab(KisFixedPaintDeviceSP dst,
        KisBrush::ColoringInformation* coloringInformation,
        double scaleX, double scaleY, double angle,
        const KisPaintInformation& info,
        double subPixelX , double subPixelY, qreal softnessFactor) const
{
    Q_UNUSED(info);

    // Generate the paint device from the mask
    const KoColorSpace* cs = dst->colorSpace();
    quint32 pixelSize = cs->pixelSize();

    // mask dimension methods already includes KisBrush::angle()
    int dstWidth = maskWidth(scaleX, angle);
    int dstHeight = maskHeight(scaleY, angle);

    angle += KisBrush::angle();

    // if there's coloring information, we merely change the alpha: in that case,
    // the dab should be big enough!
    if (coloringInformation) {

        // old bounds
        QRect oldBounds = dst->bounds();

        // new bounds. we don't care if there is some extra memory occcupied.
        dst->setRect(QRect(0, 0, dstWidth, dstHeight));

        if (dstWidth * dstHeight <= oldBounds.width() * oldBounds.height()) {
            // just clear the data in dst,
            memset(dst->data(), OPACITY_TRANSPARENT_U8, dstWidth * dstHeight * dst->pixelSize());
        } else {
            // enlarge the data
            dst->initialize();
        }
    } else {
        if (dst->data() == 0 || dst->bounds().isEmpty()) {
            qWarning() << "Creating a default black dab: no coloring info and no initialized paint device to mask";
            dst->clear(QRect(0, 0, dstWidth, dstHeight));
        }
        Q_ASSERT(dst->bounds().width() >= dstWidth && dst->bounds().height() >= dstHeight);
    }

    quint8* dabPointer = dst->data();

    quint8* color = 0;
    if (coloringInformation) {
        if (dynamic_cast<PlainColoringInformation*>(coloringInformation)) {
            color = const_cast<quint8*>(coloringInformation->color());
        }
    }

    int rowWidth = dst->bounds().width();

    double invScaleX = 1.0 / scaleX;
    double invScaleY = 1.0 / scaleY;

    double centerX = dstWidth  * 0.5 - 0.5 + subPixelX;
    double centerY = dstHeight * 0.5 - 0.5 + subPixelY;

    // the results differ, sometimes this code is faster, sometimes it is not
    // more investigation is probably needed
    // valueAt is costly similary to interpolation is some cases
    if (false && isBrushSymmetric(angle) && (dynamic_cast<PlainColoringInformation*>(coloringInformation))){
        // round eg. 14.3 to 15 so that we can interpolate
        // we have to add one pixel because of subpixel precision (see the centerX, centerY computation)
        // and add one pixel because of interpolation
        int halfWidth = qRound((dstWidth - centerX) ) + 2;
        int halfHeight = qRound((dstHeight - centerY) ) + 2;

        int size = halfWidth * halfHeight;
        if (d->precomputedQuarter.size() != size)
        {
            d->precomputedQuarter.resize(size);
        }

        // precompute the table for interpolation
        int pos = 0;
        d->shape->setSoftness(softnessFactor);
        int supersample = d->shape->shouldSupersample() ? SUPERSAMPLING : 1;
        double invss = 1.0 / supersample;
        int samplearea = supersample * supersample;
        for (int y = 0; y < halfHeight; y++){
            for (int x = 0; x < halfWidth; x++, pos++){
                int value = 0;
                for (int sy = 0; sy < supersample; sy++) {
                    for (int sx = 0; sx < supersample; sx++) {
                        double maskX = (x + sx * invss) * invScaleX;
                        double maskY = (y + sy * invss) * invScaleY;
                        value += d->shape->valueAt(maskX, maskY);
                    }
                }
                if (supersample != 1) value /= samplearea;
                d->precomputedQuarter[pos] = value;
            }
        }

        qreal random = 1.0;
        quint8 alphaValue = OPACITY_TRANSPARENT_U8;
        for (int y = 0; y < dstHeight; y++) {
            for (int x = 0; x < dstWidth; x++) {

                double maskX = (x - centerX);
                double maskY = (y - centerY);

                if (coloringInformation) {
                    if (color) {
                        memcpy(dabPointer, color, pixelSize);
                    } else {
                        memcpy(dabPointer, coloringInformation->color(), pixelSize);
                        coloringInformation->nextColumn();
                    }
                }
                if (d->randomness != 0.0){
                    random = (1.0 - d->randomness) + d->randomness * qreal(rand()) / RAND_MAX;
                }

                alphaValue = quint8( ( OPACITY_OPAQUE_U8 - interpolatedValueAt(maskX, maskY,d->precomputedQuarter,halfWidth) ) * random);
                if (d->density != 1.0){
                    // compute density only for visible pixels of the mask
                    if (alphaValue != OPACITY_TRANSPARENT_U8){
                        if ( !(d->density >= drand48()) ){
                            alphaValue = OPACITY_TRANSPARENT_U8;
                        }
                    }
                }

                cs->setOpacity(dabPointer, alphaValue, 1);
                dabPointer += pixelSize;

            }//endfor x
            //printf("\n");

            if (!color && coloringInformation) {
                coloringInformation->nextRow();
            }
            //TODO: this never happens probably?
            if (dstWidth < rowWidth) {
                dabPointer += (pixelSize * (rowWidth - dstWidth));
            }

        }//endfor y

    } else
    {
        d->shape->setSoftness( softnessFactor );

        for (int y = 0; y < dstHeight; y++) {
            for (int x = 0; x < dstWidth; x++) {

                if (coloringInformation) {
                    if (color) {
                        memcpy(dabPointer, color, pixelSize);
                    } else {
                        memcpy(dabPointer, coloringInformation->color(), pixelSize);
                        coloringInformation->nextColumn();
                    }
                }
                dabPointer += pixelSize;
             }//endfor x
             if (!color && coloringInformation) {
                coloringInformation->nextRow();
             }
        }//endfor y

        SSEMaskProcessor s(dst, cs, d->randomness, d->density, centerX, centerY, invScaleX, invScaleY, angle, d->shape);
        int jobs = d->idealThreadCountCached;
        if(dstHeight > 100 && jobs >= 4) {
            int splitter = dstHeight/jobs;
            QVector<QRect> rects;
            for(int i = 0; i < jobs - 1; i++) {
                rects << QRect(0, i*splitter, dstWidth, splitter);
            }
            rects << QRect(0, (jobs - 1)*splitter, dstWidth, dstHeight - (jobs - 1)*splitter);
            QtConcurrent::blockingMap(rects, s);
        } else {
            QRect rect(0, 0, dstWidth, dstHeight);
            s.process(rect);
        }
    }//else
}


void KisAutoBrush::toXML(QDomDocument& doc, QDomElement& e) const
{
    QDomElement shapeElt = doc.createElement("MaskGenerator");
    d->shape->toXML(doc, shapeElt);
    e.appendChild(shapeElt);
    e.setAttribute("type", "auto_brush");
    e.setAttribute("spacing", QString::number(spacing()));
    e.setAttribute("angle", QString::number(KisBrush::angle()));
    e.setAttribute("randomness", QString::number(d->randomness));
    e.setAttribute("density", QString::number(d->density));
    KisBrush::toXML(doc, e);
}

QImage KisAutoBrush::createBrushPreview()
{
    srand(0);
    srand48(0);
    int width = maskWidth(1.0, 0.0);
    int height = maskHeight(1.0, 0.0);

    KisPaintInformation info(QPointF(width * 0.5, height * 0.5), 0.5, 0, 0, KisVector2D::Zero(), 0, 0);

    KisFixedPaintDeviceSP fdev = new KisFixedPaintDevice( KoColorSpaceRegistry::instance()->rgb8() );
    fdev->setRect(QRect(0, 0, width, height));
    fdev->initialize();

    mask(fdev,KoColor(Qt::black, fdev->colorSpace()),1.0, 1.0, 0.0, info);
    return fdev->convertToQImage(0);
}


const KisMaskGenerator* KisAutoBrush::maskGenerator() const
{
    return d->shape;
}

qreal KisAutoBrush::density() const
{
    return d->density;
}

qreal KisAutoBrush::randomness() const
{
    return d->randomness;
}


bool KisAutoBrush::isBrushSymmetric(double angle) const
{
    // small brushes compute directly
    if (d->shape->height() < 3 ) return false;
    // even spikes are symmetric
    if ((d->shape->spikes() % 2) != 0) return false;
    // main condition, if not rotated or use optimization for rotated circles - rotated circle is circle again
    if ( angle == 0.0 || ( ( d->shape->type() == KisMaskGenerator::CIRCLE ) && ( d->shape->width() == d->shape->height() ) ) ) return true;
    // in other case return false
    return false;
}


quint8 KisAutoBrush::interpolatedValueAt(double x, double y,const QVector<quint8> &precomputedQuarter,int width) const
{
    x = qAbs(x);
    y = qAbs(y);

    double x_i = floor(x);
    double x_f = x - x_i;
    double x_f_r = 1.0 - x_f;

    double y_i = floor(y);
    double y_f = fabs(y - y_i);
    double y_f_r = 1.0 - y_f;

    return (x_f_r * y_f_r * valueAt(x_i , y_i, precomputedQuarter, width) +
            x_f   * y_f_r * valueAt(x_i + 1, y_i, precomputedQuarter, width) +
            x_f_r * y_f   * valueAt(x_i,  y_i + 1, precomputedQuarter, width) +
            x_f   * y_f   * valueAt(x_i + 1,  y_i + 1, precomputedQuarter, width));
}


void KisAutoBrush::setImage(const QImage& image)
{
    m_image = image;
    clearScaledBrushes();
}

QPainterPath KisAutoBrush::outline() const
{
    bool simpleOutline = (d->density < 1.0);
    if (simpleOutline){
        QPainterPath path;
        QRectF brushBoundingbox(0,0,width(), height());
        if (maskGenerator()->type() == KisMaskGenerator::CIRCLE) {
            path.addEllipse(brushBoundingbox);
        } else // if (maskGenerator()->type() == KisMaskGenerator::RECTANGLE)
        {
            path.addRect(brushBoundingbox);
        }

        return path;
    }

    return KisBrush::boundary()->path();
}
