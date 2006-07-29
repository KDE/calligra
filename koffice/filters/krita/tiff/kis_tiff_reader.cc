/*
 *  Copyright (c) 2005-2006 Cyrille Berger <cberger@cberger.net>
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
 
#include "kis_tiff_reader.h"

#include <kdebug.h>

#include <kis_iterators_pixel.h>
#include <kis_paint_device.h>

#include "kis_tiff_stream.h"

    uint KisTIFFReaderTarget8bit::copyDataToChannels( Q_UINT32 x, Q_UINT32 y, Q_UINT32 dataWidth, TIFFStreamBase* tiffstream)
    {
        KisHLineIterator it = paintDevice() -> createHLineIterator(x, y, dataWidth, true);
        double coeff = Q_UINT8_MAX / (double)( pow(2, sourceDepth() ) - 1 );
//         kdDebug(41008) << " depth expension coefficient : " << coeff << endl;
        while (!it.isDone()) {
            Q_UINT8 *d = it.rawData();
            Q_UINT8 i;
            for(i = 0; i < nbColorsSamples() ; i++)
            {
                d[poses()[i]] = (Q_UINT8)( tiffstream->nextValue() * coeff );
            }
            postProcessor()->postProcess8bit( d);
            if(transform()) cmsDoTransform(transform(), d, d, 1);
            d[poses()[i]] = Q_UINT8_MAX;
            for(int k = 0; k < nbExtraSamples(); k++)
            {
                if(k == alphaPos())
                    d[poses()[i]] = (Q_UINT32) ( tiffstream->nextValue() * coeff );
                else
                    tiffstream->nextValue();
            }
            ++it;
        }
        return 1;
    }
    uint KisTIFFReaderTarget16bit::copyDataToChannels( Q_UINT32 x, Q_UINT32 y, Q_UINT32 dataWidth, TIFFStreamBase* tiffstream)
    {
        KisHLineIterator it = paintDevice() -> createHLineIterator(x, y, dataWidth, true);
        double coeff = Q_UINT16_MAX / (double)( pow(2, sourceDepth() ) - 1 );
//         kdDebug(41008) << " depth expension coefficient : " << coeff << endl;
        while (!it.isDone()) {
            Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
            Q_UINT8 i;
            for(i = 0; i < nbColorsSamples(); i++)
            {
                d[poses()[i]] = (Q_UINT16)( tiffstream->nextValue() * coeff );
            }
            postProcessor()->postProcess16bit( d);
            if(transform()) cmsDoTransform(transform(), d, d, 1);
            d[poses()[i]] = Q_UINT16_MAX;
            for(int k = 0; k < nbExtraSamples(); k++)
            {
                if(k == alphaPos())
                    d[poses()[i]] = (Q_UINT16) ( tiffstream->nextValue() * coeff );
                else
                    tiffstream->nextValue();
            }
            ++it;
        }
        return 1;
    }
    
    uint KisTIFFReaderFromPalette::copyDataToChannels(Q_UINT32 x, Q_UINT32 y, Q_UINT32 dataWidth,  TIFFStreamBase* tiffstream)
    {
        KisHLineIterator it = paintDevice() -> createHLineIterator(x, y, dataWidth, true);
        while (!it.isDone()) {
            Q_UINT16* d = reinterpret_cast<Q_UINT16 *>(it.rawData());
            uint32 index = tiffstream->nextValue();
            d[2] = m_red[index];
            d[1] = m_green[index];
            d[0] = m_blue[index];
            d[3] = Q_UINT16_MAX;
            ++it;
        }
        return 1;
    }
