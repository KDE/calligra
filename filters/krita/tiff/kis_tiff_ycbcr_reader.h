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

#ifndef _KIS_TIFF_YCBCR_READER_H_
#define _KIS_TIFF_YCBCR_READER_H_

#include "kis_tiff_reader.h"

namespace KisTIFFYCbCr {
    enum Position {
        POSITION_CENTERED = 1,
        POSITION_COSITED = 2
    };
}

class KisTIFFYCbCrReaderTarget8Bit : public KisTIFFReaderBase {
    public:
        /**
         * @param hsub horizontal subsampling of Cb and Cr
         * @param hsub vertical subsampling of Cb and Cr
         */
        KisTIFFYCbCrReaderTarget8Bit( KisPaintDeviceSP device, Q_UINT8* poses, int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount,  cmsHTRANSFORM transformProfile, KisTIFFPostProcessor* postprocessor, uint16 hsub, uint16 vsub, KisTIFFYCbCr::Position position );
        ~KisTIFFYCbCrReaderTarget8Bit();
        virtual uint copyDataToChannels( Q_UINT32 x, Q_UINT32 y, Q_UINT32 dataWidth, TIFFStreamBase* tiffstream);
        virtual void finalize();
    private:
        Q_UINT8* m_bufferCb;
        Q_UINT8* m_bufferCr;
        Q_UINT32 m_bufferWidth, m_bufferHeight;
        uint16 m_hsub;
        uint16 m_vsub;
        KisTIFFYCbCr::Position m_position;

};

class KisTIFFYCbCrReaderTarget16Bit : public KisTIFFReaderBase {
    public:
        /**
         * @param hsub horizontal subsampling of Cb and Cr
         * @param hsub vertical subsampling of Cb and Cr
         */
        KisTIFFYCbCrReaderTarget16Bit( KisPaintDeviceSP device, Q_UINT8* poses, int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount,  cmsHTRANSFORM transformProfile, KisTIFFPostProcessor* postprocessor, uint16 hsub, uint16 vsub, KisTIFFYCbCr::Position position );
        ~KisTIFFYCbCrReaderTarget16Bit();
        virtual uint copyDataToChannels( Q_UINT32 x, Q_UINT32 y, Q_UINT32 dataWidth, TIFFStreamBase* tiffstream);
        virtual void finalize();
    private:
        Q_UINT16* m_bufferCb;
        Q_UINT16* m_bufferCr;
        Q_UINT32 m_bufferWidth, m_bufferHeight;
        uint16 m_hsub;
        uint16 m_vsub;
        KisTIFFYCbCr::Position m_position;

};


#endif
