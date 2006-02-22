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

#include "kis_tiff_stream.h"

//     typedef void (*copyTransform)(Q_UINT8*, int, cmsHTRANSFORM);
    
/*    void cTconvertUsingLcms(Q_UINT8* v, int pixelsize, cmsHTRANSFORM transform)
    {
        cmsDoTransform(transform, v, v, pixelsize);
    }*/
    
#if 0
    
#define Kr 0.2126
#define Kb 0.0722
    
/*    template<typename TYPE, Q_UINT32 TYPE_MAX>
    void cTYcBcRtoRGB(Q_UINT8* v, int , cmsHTRANSFORM )
    {
        TYPE* d = (TYPE*)v;
        double Eyq = (d[0]-(double)(TYPE_MAX/16 + 1)) / (double)(TYPE_MAX - 5);
        double Ebq = (d[1]-(double)(TYPE_MAX/2 + 1)) / (double)(TYPE_MAX - 1);
        double Erq = (d[2]-(double)(TYPE_MAX/2 + 1) ) / (double)(TYPE_MAX - 1);
        d[2] = 0;//(TYPE)round(TYPE_MAX * (2. * (1. - Kr) * Erq + Eyq));
        d[1] = 0;//(TYPE)round(TYPE_MAX * (2. * (1. - Kb) * Ebq + Eyq));
        d[0] = (TYPE)round(TYPE_MAX * (Eyq - 2. * (Kr * (1. - Kr) * Erq + Kb * (1. - Kr) * Ebq)));
//         d[0] = TYPE_MAX;
    }*/
    
    void cTTest(Q_UINT8* d, int , cmsHTRANSFORM )
    {
        Q_UINT8 v = d[1];
        d[0] = v;
        d[1] = v;
        d[2] = v;
//         Q_UINT8 Y = d[0]; Q_UINT8 U = d[1]; Q_UINT8 V = d[2];
//         d[2] = 126; //Y + U;
//         d[1] = 126; //Y - 0.51*U - 0.186*V;
//         d[0] = 126; //Y + V;
        
/*        double Eyq = ((double)d[0]-16) / 219.0;
        double Ebq = ((double)d[1]-128) / 224.0;
        double Erq = ((double)d[2]-128) / 224.0;

        d[2] = round(255 * (2 * (1-Kr) * Erq + Eyq));
        d[1] = round(255 * (2 * (1-Kb) * Ebq + Eyq));
        d[0] = round(255 * (Eyq - 2 * (Kr * (1 - Kr) * Erq + Kb * (1 -Kr) * Ebq)));
        d[0] = 126;*/
    }
#endif

    void KisTIFFReaderTarget8bit::copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream)
    {
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
    }
    void KisTIFFReaderTarget16bit::copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream)
    {
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
    }
    
    void KisTIFFReaderFromPalette::copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream)
    {
            while (!it.isDone()) {
                Q_UINT16* d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                uint32 index = tiffstream->nextValue();
                d[2] = m_red[index];
                d[1] = m_green[index];
                d[0] = m_blue[index];
                d[3] = Q_UINT16_MAX;
                ++it;
            }

    }

#if 0
    void convertFromTIFFData( KisHLineIterator it, TIFFStream* tiffstream, int8 alphapos, uint16 color_type, uint16 depth, uint8 nbcolorssamples, uint8 extrasamplescount, uint16 *red, uint16 *green, uint16 *blue, KisProfile* profile, cmsHTRANSFORM transform )
    {
        switch(color_type)
        {
            case PHOTOMETRIC_MINISWHITE:
            {
                Q_UINT8 poses[]={ 0, 1};
                if(depth > 8)
                    copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform, cTinvert16);
                else
                    copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform, cTinvert8);
            }
            break;
            case PHOTOMETRIC_MINISBLACK:
            {
                Q_UINT8 poses[]={ 0, 1};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform);
            }
            break;
            case PHOTOMETRIC_CIELAB:
            {
                Q_UINT8 poses[]={ 0, 1, 2, 3};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform);
            }
            break;
            case PHOTOMETRIC_ICCLAB:
            {
                Q_UINT8 poses[]={ 0, 1};
                if(depth > 8)
                    copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform, cTICCLABtoCIELAB16);
                else
                    copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform, cTICCLABtoCIELAB8);
            }
            break;
            case PHOTOMETRIC_RGB:
            {
                Q_UINT8 poses[]={ 2, 1, 0, 3};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform);
            }
            break;
#if 0
            case PHOTOMETRIC_YCBCR:
            {
                Q_UINT8 poses[]={ 0, 1, 2, 3};
/*                if(profile)
                {
                    if(depth > 8)
                        copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, cTtoLAB, cmsCreateTransform( profile->profile(), TYPE_YCbCr_16, cmsCreateLabProfile(NULL), TYPE_Lab_16, INTENT_PERCEPTUAL, 0) );
                    else
                        copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, cTtoLAB, cmsCreateTransform( profile->profile(), TYPE_YCbCr_8, cmsCreateLabProfile(NULL), TYPE_Lab_16, INTENT_PERCEPTUAL, 0) );
                } else {*/
                        copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses,  cTTest/*cTYcBcRtoRGB<Q_UINT8, Q_UINT8_MAX>*/ );
//                     if(depth > 8)
//                         copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, cTYcBcRtoRGB<Q_UINT16, Q_UINT16_MAX> );
//                     else
//                         copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses,  cTTest/*cTYcBcRtoRGB<Q_UINT8, Q_UINT8_MAX>*/ );
//                 }
            }
#endif
            case PHOTOMETRIC_PALETTE:
            {
                if(depth <= 8)
                {
                    while (!it.isDone()) {
                        Q_UINT16* d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        uint32 index = tiffstream->nextValueBelow16();
                        d[2] = red[index];
                        d[1] = green[index];
                        d[0] = blue[index];
                        d[3] = Q_UINT16_MAX;
                        ++it;
                    }
                } else if(depth < 16)
                {
                    while (!it.isDone()) {
                        Q_UINT16* d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        uint32 index = tiffstream->nextValueBelow16();
                        d[2] = red[index];
                        d[1] = green[index];
                        d[0] = blue[index];
                        d[3] = Q_UINT16_MAX;
                        ++it;
                    }
                } else if(depth < 32)
                {
                    while (!it.isDone()) {
                        Q_UINT16* d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        uint32 index = tiffstream->nextValueBelow32();
                        d[2] = red[index];
                        d[1] = green[index];
                        d[0] = blue[index];
                        d[3] = Q_UINT16_MAX;
                        ++it;
                    }
                }
            }
            case PHOTOMETRIC_SEPARATED: // it means CMYK
            {
                Q_UINT8 poses[]={ 0, 1, 2, 3, 4};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, transform);
            }
            break;
        }
    }
}

#endif
