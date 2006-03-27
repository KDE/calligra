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

#ifndef KIS_YCBCR_U8_COLORSPACE_H
#define KIS_YCBCR_U8_COLORSPACE_H

#include <kis_u8_base_colorspace.h>

#define LUMA_RED 0.2989
#define LUMA_GREEN 0.587
#define LUMA_BLUE 0.114

//TODO: for 1.6/2.0 move it to a krita/colorspaces/YCbCr_u8 and //i18nise it

class KisYCbCrU8ColorSpace : public KisU8BaseColorSpace
{
    public:
        KisYCbCrU8ColorSpace(KisColorSpaceFactoryRegistry* parent, KisProfile* p);
        ~KisYCbCrU8ColorSpace();
        virtual bool willDegrade(ColorSpaceIndependence )
        {
            return false;
        };
    public:
        void setPixel(quint8 *pixel, quint8 Y, quint8 Cb, quint8 Cr, quint8 alpha) const;
        void getPixel(const quint8 *pixel, quint8 *Y, quint8 *Cb, quint8 *Cr, quint8 *alpha) const;

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
        void compositeCopy(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);

    private:
#define CLAMP_TO_8BITCHANNEL(a) CLAMP(a, 0, quint8_MAX)
        inline quint8 computeRed(quint8 Y, quint8 /*Cb*/, quint8 Cr)
        {
            return (quint8)( CLAMP_TO_8BITCHANNEL( (Cr - 128)* (2-2*LUMA_RED) + Y )  );
        }
        inline quint8 computeGreen(quint8 Y, quint8 Cb, quint8 Cr)
        {
            return (quint8)( CLAMP_TO_8BITCHANNEL( (Y - LUMA_BLUE * computeBlue(Y,Cb,Cr) - LUMA_RED * computeRed(Y,Cb,Cr) ) / LUMA_GREEN ) );
        }
        inline quint8 computeBlue(quint8 Y, quint8 Cb, quint8 /*Cr*/)
        {
            return (quint8)( CLAMP_TO_8BITCHANNEL( (Cb - 128)*(2 - 2 * LUMA_BLUE) + Y) );
        }
        inline quint8 computeY( quint8 r, quint8 b, quint8 g)
        {
            return (quint8)( CLAMP_TO_8BITCHANNEL( LUMA_RED*r + LUMA_GREEN*g + LUMA_BLUE*b ) );
        }
        inline quint8 computeCb( quint8 r, quint8 b, quint8 g)
        {
            return (quint8)( CLAMP_TO_8BITCHANNEL( (b - computeY(r,g,b))/(2-2*LUMA_BLUE) + 128) );
        }
        inline quint8 computeCr( quint8 r, quint8 b, quint8 g)
        {
            return (quint8)( CLAMP_TO_8BITCHANNEL( (r - computeY(r,g,b))/(2-2*LUMA_RED) + 128) );
        }
#undef CLAMP_TO_8BITCHANNEL

        static const quint8 PIXEL_Y = 0;
        static const quint8 PIXEL_Cb = 1;
        static const quint8 PIXEL_Cr = 2;
        static const quint8 PIXEL_ALPHA = 3;

        struct Pixel {
            quint8 Y;
            quint8 Cb;
            quint8 Cr;
            quint8 alpha;
        };
};

#endif
