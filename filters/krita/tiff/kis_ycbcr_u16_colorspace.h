/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KIS_YCBCR_U16_COLORSPACE_H
#define KIS_YCBCR_U16_COLORSPACE_H

#include <kis_u16_base_colorspace.h>

#define LUMA_RED 0.2989
#define LUMA_GREEN 0.587
#define LUMA_BLUE 0.114

//TODO: for 1.6/2.0 move it to a krita/colorspaces/YCbCr_u8 and //i18nise it

class KisYCbCrU16ColorSpace : public KisU16BaseColorSpace
{
    public:
        KisYCbCrU16ColorSpace(KisColorSpaceFactoryRegistry* parent, KisProfile* p);
        ~KisYCbCrU16ColorSpace();
        virtual bool willDegrade(ColorSpaceIndependence )
        {
            return false;
        };
    public:
        void setPixel(quint8 *pixel, quint16 Y, quint16 Cb, quint16 Cr, quint16 alpha) const;
        void getPixel(const quint8 *pixel, quint16 *Y, quint16 *Cb, quint16 *Cr, quint16 *alpha) const;

        virtual void fromQColor(const QColor& c, quint8 *dst, KisProfile * profile = 0);
        virtual void fromQColor(const QColor& c, quint8 opacity, quint8 *dst, KisProfile * profile = 0);

        virtual void toQColor(const quint8 *src, QColor *c, KisProfile * profile = 0);
        virtual void toQColor(const quint8 *src, QColor *c, quint8 *opacity, KisProfile * profile = 0);

        virtual quint8 difference(const quint8 *src1, const quint8 *src2);
        virtual void mixColors(const quint8 **colors, const quint8 *weights, quint32 nColors, quint8 *dst) const;

        virtual Q3ValueVector<KisChannelInfo *> channels() const;
        virtual quint32 nChannels() const;
        virtual quint32 nColorChannels() const;
        virtual quint32 pixelSize() const;

        virtual QImage convertToQImage(const quint8 *data, qint32 width, qint32 height,
                                       KisProfile *  dstProfile,
                                       qint32 renderingIntent,
                                       float exposure = 0.0f);

        virtual KisCompositeOpList userVisiblecompositeOps() const;

    protected:

        virtual void bitBlt(quint8 *dst,
                            qint32 dstRowStride,
                            const quint8 *src,
                            qint32 srcRowStride,
                            const quint8 *srcAlphaMask,
                            qint32 maskRowStride,
                            quint8 opacity,
                            qint32 rows,
                            qint32 cols,
                            const KisCompositeOp& op);

        void compositeOver(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
        void compositeErase(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);

    private:
#define CLAMP_TO_16BITCHANNEL(a) CLAMP(a, 0, quint16_MAX)
        inline quint16 computeRed(quint16 Y, quint16 /*Cb*/, quint16 Cr)
        {
            return (quint16)( CLAMP_TO_16BITCHANNEL( (Cr - 32768)* (2-2*LUMA_RED) + Y )  );
        }
        inline quint16 computeGreen(quint16 Y, quint16 Cb, quint16 Cr)
        {
            return (quint16)( CLAMP_TO_16BITCHANNEL( (Y - LUMA_BLUE * computeBlue(Y,Cb,Cr) - LUMA_RED * computeRed(Y,Cb,Cr) ) / LUMA_GREEN ) );
        }
        inline quint16 computeBlue(quint16 Y, quint16 Cb, quint16 /*Cr*/)
        {
            return (quint16)( CLAMP_TO_16BITCHANNEL( (Cb - 32768)*(2 - 2 * LUMA_BLUE) + Y) );
        }
        inline quint16 computeY( quint16 r, quint16 b, quint16 g)
        {
            return (quint16)( CLAMP_TO_16BITCHANNEL( LUMA_RED*r + LUMA_GREEN*g + LUMA_BLUE*b ) );
        }
        inline quint16 computeCb( quint16 r, quint16 b, quint16 g)
        {
            return (quint16)( CLAMP_TO_16BITCHANNEL( (b - computeY(r,g,b))/(2-2*LUMA_BLUE) + 32768) );
        }
        inline quint16 computeCr( quint16 r, quint16 b, quint16 g)
        {
            return (quint8)( CLAMP_TO_16BITCHANNEL( (r - computeY(r,g,b))/(2-2*LUMA_RED) + 32768) );
        }
#undef CLAMP_TO_16BITCHANNEL
        
        static const quint8 PIXEL_Y = 0;
        static const quint8 PIXEL_Cb = 1;
        static const quint8 PIXEL_Cr = 2;
        static const quint8 PIXEL_ALPHA = 3;

        struct Pixel {
            quint16 Y;
            quint16 Cb;
            quint16 Cr;
            quint16 alpha;
        };
};

#endif
