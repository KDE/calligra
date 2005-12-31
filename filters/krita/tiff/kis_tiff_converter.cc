/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
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
 
 // A big thank to Glenn Randers-Pehrson for it's wonderfull documentation of libpng available at http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
#include "kis_tiff_converter.h"

#include <stdio.h>

#include <kapplication.h>
#include <koDocumentInfo.h>

#include <kio/netaccess.h>

#include <kis_abstract_colorspace.h>
#include <kis_colorspace_factory_registry.h>
#include <kis_doc.h>
#include <kis_image.h>
#include <kis_iterators_pixel.h>
#include <kis_layer.h>
#include <kis_meta_registry.h>
#include <kis_profile.h>
#include <kis_paint_layer.h>

namespace {

    const Q_UINT8 PIXEL_BLUE = 0;
    const Q_UINT8 PIXEL_GREEN = 1;
    const Q_UINT8 PIXEL_RED = 2;
    const Q_UINT8 PIXEL_ALPHA = 3;

    
    int16 getColorTypeforColorSpace( KisColorSpace * cs )
    {
        if ( cs->id() == KisID("GRAYA") || cs->id() == KisID("GRAYA16") )
        {
            return PHOTOMETRIC_MINISBLACK;
        }
        if ( cs->id() == KisID("RGBA") || cs->id() == KisID("RGBA16") )
        {
            return PHOTOMETRIC_RGB;
        }
        if ( cs->id() == KisID("CMYKA") || cs->id() == KisID("CMYKA16") )
        {
            return PHOTOMETRIC_SEPARATED;
        }
        if ( cs->id() == KisID("LABA") )
        {
            return PHOTOMETRIC_CIELAB;
        }

        kdDebug() << "Cannot export images in " + cs->id().name() + " yet.\n";
        return -1;

    }

    
    QString getColorSpaceForColorType(uint16 color_type, uint16 color_nb_bits, TIFF *image) {
        if(color_type == PHOTOMETRIC_MINISWHITE || color_type == PHOTOMETRIC_MINISBLACK)
        {
            if(color_nb_bits <= 8)
            {
                    return "GRAYA";
            } else if(color_nb_bits <= 32){ // WARNING from 17 to 32 bits, it's highly experimental
                    return "GRAYA16";
            }
        } else if(color_type == PHOTOMETRIC_RGB ) {
            if(color_nb_bits <= 8)
            {
                return "RGBA";
            } else if(color_nb_bits <= 16){
                return "RGBA16";
            }
        } else if(color_type == PHOTOMETRIC_SEPARATED ) {
            // SEPARATED is in general CMYK but not allways
            uint16 inkset;
            if((TIFFGetField(image, TIFFTAG_INKSET, &inkset) == 0)){
                kdDebug() <<  "Image does not define the inkset." << endl;
                return "";
            }
            if(inkset !=  INKSET_CMYK)
            {
                kdDebug() << "Unsupported inkset (right now, only CMYK is supported)" << endl;
                return "";
            }
            if(color_nb_bits <= 8)
            {
                return "CMYK";
            } else if(color_nb_bits <= 16){
                return "CMYKA16";
            }
        } else if(color_type == PHOTOMETRIC_CIELAB || color_type == PHOTOMETRIC_ICCLAB ) {
            // TODO: support for PHOTOMETRIC_ITULAB
            return "LABA"; // TODO add support for a 8bit LAB colorspace when it is written
        } else if(color_type ==  PHOTOMETRIC_PALETTE) {
            // <-- we will convert the index image to RGBA16 as the index is allways on 16bits colors
            return "RGBA16";
        }
        return "";
    }
    uint32 ddReader(Q_UINT8** srcit, uint16 depth, uint8 &posinc)
    {
        register uint8 remain;
        register uint32 value;

        remain = depth;
        value = 0;

        while (remain > 0)
        {
            register uint8 toread;

            toread = remain;
            if (toread > posinc) toread = posinc;

            remain -= toread;
            posinc -= toread;

            value = (value << toread) | (( (**srcit) >> (posinc) ) & ( ( 1 << toread ) - 1 ) );

            if (posinc == 0)
            {
                *srcit = *srcit + 1;
                posinc=8;
            }
        }
        return value;
    }
    void convertFromTIFFData( KisHLineIterator it, uint8* src, int8 alphapos, uint16 color_type, uint16 depth, uint8 extrasamplescount, uint16 *red, uint16 *green, uint16 *blue)
    {
        switch(color_type)
        {
            case PHOTOMETRIC_MINISWHITE:
                if(depth == 16)
                {
                    Q_UINT16 *srcit = reinterpret_cast<Q_UINT16 *>(src);
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = Q_UINT16_MAX - *(srcit++);
                        if(alphapos != -1)
                        {
                            d[1] = *(srcit + alphapos);
                        } else {
                            d[1] = Q_UINT16_MAX;
                        }
                        ++it;
                    }
                } else if( depth == 8) {
                    Q_UINT8 *srcit = src;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[0] = Q_UINT8_MAX - *(srcit++);
                        if(alphapos != -1)
                        {
                            d[1] = *(srcit + alphapos);
                        } else {
                            d[1] = Q_UINT8_MAX;
                        }
                        ++it;
                    }
                } else if( depth < 8 ) {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT8 coeff = Q_UINT8_MAX >> (depth - 1);
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                            Q_UINT8 *d = it.rawData();
                            d[0] = Q_UINT8_MAX - (ddReader(&srcit, depth, posinc))*coeff;
                            d[1] = Q_UINT8_MAX;
                            for(int i = 0; i < extrasamplescount; i++)
                            {
                                if(i == alphapos)
                                    d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                                else
                                    (ddReader(&srcit, depth, posinc));
                            }
                            ++it;
                    }
                } else if( depth < 16 ) {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT16 coeff =  1 << ( 16 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = Q_UINT16_MAX - (ddReader(&srcit, depth, posinc)) *coeff;
                        d[1] = Q_UINT16_MAX;
                        for(uint i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                }
                break;
            case PHOTOMETRIC_MINISBLACK:
                if(depth == 16)
                {
                    Q_UINT16 *srcit = reinterpret_cast<Q_UINT16 *>(src);
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = *(srcit++);
                        if(alphapos != -1)
                        {
                            d[1] = *(srcit + alphapos);
                        } else {
                            d[1] = Q_UINT16_MAX;
                        }
                        ++it;
                    }
                }
                else if( depth == 8)
                {
                    Q_UINT8 *srcit = src;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[0] = *(srcit++);
                        if(alphapos != -1)
                        {
                            d[1] = *(srcit + alphapos);
                        } else {
                            d[1] = Q_UINT8_MAX;
                        }
                        ++it;
                    }
                }
                else if( depth < 8 )
                {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT8 coeff = 1 << ( 8 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[0] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[1] = Q_UINT8_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                } else if( depth < 16 ) {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT16 coeff =  1 << ( 16 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = (ddReader(&srcit, depth, posinc)) *coeff;
                        d[1] = Q_UINT16_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                } else if( depth > 16 ) {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT32 coeff =  1 << ( depth - 16 );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = (ddReader(&srcit, depth, posinc)) / coeff;
                        d[1] = Q_UINT16_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                }
                break;
            case PHOTOMETRIC_CIELAB:
            case PHOTOMETRIC_RGB:
                if(depth == 16)
                {
                    Q_UINT16 *srcit = reinterpret_cast<Q_UINT16 *>(src);
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[2] = *(srcit++);
                        d[1] = *(srcit++);
                        d[0] = *(srcit++);
                        if(alphapos != -1)
                        {
                            d[3] = *(srcit + alphapos);
                        } else {
                            d[3] = Q_UINT16_MAX;
                        }
                        srcit+= extrasamplescount;
                        ++it;
                    }
                }
                else if( depth == 8)
                {
                    Q_UINT8 *srcit = src;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[2] = *(srcit++);
                        d[1] = *(srcit++);
                        d[0] = *(srcit++);
                        if(alphapos != -1)
                        {
                            d[3] = *(srcit + alphapos);
                        } else {
                            d[3] = Q_UINT8_MAX;
                        }
                        srcit+= extrasamplescount;
                        ++it;
                    }
                }
                else if( depth < 8 )
                {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT8 coeff = 1 << ( 8 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[2] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[0] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[3] = Q_UINT8_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[3] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                } else if( depth < 16 ) {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT16 coeff =  1 << ( 16 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[2] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[0] = (ddReader(&srcit, depth, posinc)) *coeff;
                        d[3] = Q_UINT16_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[3] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                } 
            case PHOTOMETRIC_PALETTE:
            {
                Q_UINT8* srcit = src;
                while (!it.isDone()) {
                    Q_UINT16* d = (Q_UINT16*)it.rawData();
                    uint8 index = *(srcit++);
                    d[2] = red[index];
                    d[1] = green[index];
                    d[0] = blue[index];
                    d[3] = Q_UINT16_MAX;
                    ++it;
                }
            }
            case PHOTOMETRIC_SEPARATED: // it means CMYK
                if(depth == 16)
                {
                    Q_UINT16 *srcit = reinterpret_cast<Q_UINT16 *>(src);
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = *(srcit++);
                        d[1] = *(srcit++);
                        d[2] = *(srcit++);
                        d[3] = *(srcit++);
                        if(alphapos != -1)
                        {
                            d[4] = *(srcit + alphapos);
                        } else {
                            d[4] = Q_UINT16_MAX;
                        }
                        srcit+= extrasamplescount;
                        ++it;
                    }
                }
                else if(depth == 8)
                {
                    Q_UINT8 *srcit = src;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[0] = *(srcit++);
                        d[1] = *(srcit++);
                        d[2] = *(srcit++);
                        d[3] = *(srcit++);
                        if(alphapos != -1)
                        {
                            d[4] = *(srcit + alphapos);
                        } else {
                            d[4] = Q_UINT16_MAX;
                        }
                        srcit+= extrasamplescount;
                        ++it;
                    }
                }
                else if( depth < 8 )
                {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT8 coeff = 1 << ( 8 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[0] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[2] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[3] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[4] = Q_UINT8_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[4] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                } else if( depth < 16 ) {
                    uint8 posinc = 8;
                    Q_UINT8 *srcit = src;
                    Q_UINT16 coeff =  1 << ( 16 - depth );
                    kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[1] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[2] = (ddReader(&srcit, depth, posinc))*coeff;
                        d[3] = (ddReader(&srcit, depth, posinc)) *coeff;
                        d[4] = Q_UINT16_MAX;
                        for(int i = 0; i < extrasamplescount; i++)
                        {
                            if(i == alphapos)
                                d[4] = (ddReader(&srcit, depth, posinc))*coeff;
                            else
                                (ddReader(&srcit, depth, posinc));
                        }
                        ++it;
                    }
                } 
/*            case PHOTOMETRIC_ICCLAB:
                // TODO: convert from ICCLAB to CIELAB
                switch(depth)
                {
                    case 16:
                    {
                        Q_UINT16 *srcit = reinterpret_cast<Q_UINT16 *>(src);
                        while (!it.isDone()) {
                            Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                            d[0] = *(srcit++);
                            d[1] = *(srcit++);
                            d[2] = *(srcit++);
                            if(alphapos != -1)
                            {
                                d[3] = *(srcit + alphapos);
                            } else {
                                d[3] = Q_UINT16_MAX;
                            }
                            srcit+= extrasamplescount;
                            ++it;
                        }
                    }
                    case 8:
                    {
                        Q_UINT8 *srcit = src;
                        while (!it.isDone()) {
                            Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                            d[0] = *(srcit++) / Q_UINT8_MAX;
                            d[1] = *(srcit++) / Q_UINT8_MAX;
                            d[2] = *(srcit++) / Q_UINT8_MAX;
                            if(alphapos != -1)
                            {
                                d[3] = *(srcit + alphapos);
                            } else {
                                d[3] = Q_UINT16_MAX;
                            }
                            srcit+= extrasamplescount;
                            ++it;
                        }
                    }
                }
                break;*/
        }
    }
}

KisTIFFConverter::KisTIFFConverter(KisDoc *doc, KisUndoAdapter *adapter)
{
    m_doc = doc;
    m_adapter = adapter;
    m_job = 0;
    m_stop = false;
}

KisTIFFConverter::~KisTIFFConverter()
{
}

KisImageBuilder_Result KisTIFFConverter::decode(const KURL& uri)
{
    kdDebug() << "Start decoding TIFF File" << endl;
    // Opent the TIFF file
    TIFF *image;
    if((image = TIFFOpen(uri.path().ascii(), "r")) == NULL){
        kdDebug() << "Could not open the file, either it doesn't exist, either it is not a TIFF : " << uri.path() << endl;
        TIFFClose(image);
        return (KisImageBuilder_RESULT_BAD_FETCH);
    }
    // Read information about the tiff
    uint32 width, height;
    if(TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &width) == 0){
        kdDebug() <<  "Image does not define its width" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    if(TIFFGetField(image, TIFFTAG_IMAGELENGTH, &height) == 0){
        kdDebug() <<  "Image does not define its height" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    uint16 depth;
    if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &depth) == 0)){
        kdDebug() <<  "Image does not define its depth" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    // Determine the number of channels (usefull to know if a file has an alpha or not
    uint16 nbchannels;
    if(TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &nbchannels) == 0){
        kdDebug() << "Image has an undefined number of samples per pixel" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    // Determine the colorspace
    uint16 color_type;
    if(TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &color_type) == 0){
        kdDebug() << "Image has an undefined photometric interpretation" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    QString csName = getColorSpaceForColorType(color_type, depth, image);
    if(csName == "") {
        kdDebug() << "Image has an unsupported colorspace : " << color_type << " for this depth : "<< depth << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    kdDebug() << "Colorspace is : " << csName << " with a depth of " << depth << " and with a nb of channels of " << nbchannels << endl;
    
    // Read image profile
    KisProfile* profile = 0;
    DWORD EmbedLen;
    LPBYTE EmbedBuffer;

    if (TIFFGetField(image, TIFFTAG_ICCPROFILE, &EmbedLen, &EmbedBuffer)) {
        cmsHPROFILE hProfile = cmsOpenProfileFromMem(EmbedBuffer, EmbedLen);
        if(hProfile != NULL)
        {
            profile = new KisProfile(hProfile);
            Q_CHECK_PTR(profile);
        }
    }

    // Retrieve a pointer to the colorspace
    KisColorSpace* cs;
    if (profile)
    {
        kdDebug() << "image has embedded profile: " << profile -> productName() << "\n";
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(csName, profile);
    }
    else
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID(csName,""),"");

    if(cs == 0) {
        kdDebug() << "Colorspace " << csName << " is not available, please check your installation." << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }

    // Check if there is an alpha channel
    uint16 *sampleinfo, extrasamplescount;
    if(TIFFGetField(image, TIFFTAG_EXTRASAMPLES, &extrasamplescount, &sampleinfo) == 0)
    {
        extrasamplescount = 0;
    }
    int8 alphapos = -1; // <- no alpha
    // Check which extra is alpha if any
    kdDebug() << "There are " << nbchannels << " channels and " << extrasamplescount << " extra channels" << endl;
    for(int i = 0; i < extrasamplescount; i ++)
    {
        kdDebug() << i << " " << extrasamplescount << " "  << (cs->nColorChannels()) <<  nbchannels << " " << sampleinfo[i] << endl;
        if(sampleinfo[i] == EXTRASAMPLE_ASSOCALPHA)
        {
            alphapos = i;
        }
    }
    
    // Read META Information
    KoDocumentInfo * info = m_doc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author"));
    char* text;
    if (TIFFGetField(image, TIFFTAG_ARTIST, &text)) {
        authorPage->setFullName(text);
    }
    if (TIFFGetField(image, TIFFTAG_DOCUMENTNAME, &text)) {
        aboutPage->setTitle(text);
    }
    if (TIFFGetField(image,TIFFTAG_IMAGEDESCRIPTION,&text) ) {
        aboutPage->setAbstract( text );
    }
    
    // Read pallete if the file is indexed
    uint16 *red; // No need to free them they are free by libtiff
    uint16 *green;
    uint16 *blue;
    if(color_type == PHOTOMETRIC_PALETTE) {
        if ((TIFFGetField(image, TIFFTAG_COLORMAP, &red, &green, &blue)) == 0)
        {
            kdDebug() <<  "Indexed image does not define a palette" << endl;
            TIFFClose(image);
            return KisImageBuilder_RESULT_INVALID_ARG;
        }
    }
    
    // Get the planar configuration
    uint16 planarconfig;
    if(TIFFGetField(image, TIFFTAG_PLANARCONFIG, &planarconfig) == 0)
    {
        kdDebug() <<  "Plannar configuration is not define" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    // Get the fill order
/*    uint8 fillorder;
    if(TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0)
    {
        kdDebug() <<  "Fill order is not define" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }*/
    // Creating the KisImageSP
    if( ! m_img) {
        m_img = new KisImage(m_doc->undoAdapter(), width, height, cs, "built image");
        Q_CHECK_PTR(m_img);
        if(profile)
        {
            m_img -> addAnnotation( profile->annotation() );
        }
    }
    KisPaintLayer* layer = new KisPaintLayer(m_img, m_img -> nextLayerName(), Q_UINT8_MAX);
    m_img->addLayer(layer, m_img->rootLayer(), 0);
    if(TIFFIsTiled(image))
    {
        kdDebug() << "tiled image" << endl;
        uint32 tileWidth, tileHeight;
        uint32 x, y;
        TIFFGetField(image, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(image, TIFFTAG_TILELENGTH, &tileHeight);
        tdata_t buf = _TIFFmalloc(TIFFTileSize(image));
        uint32 linewidth = (tileWidth * depth * nbchannels) / 8;
        kdDebug() << linewidth << " " << nbchannels << " " << layer->paintDevice()->colorSpace()->nColorChannels() << endl;
        for (y = 0; y < height; y+= tileHeight)
        {
            for (x = 0; x < width; x += tileWidth)
            { // TODO support for different planarity
                if( planarconfig == PLANARCONFIG_CONTIG )
                {
                    TIFFReadTile(image, buf, x, y, 0, 0);
                } else {
                    kdDebug() <<  "Plannar configuration seperated is not supported" << endl;
                    TIFFClose(image);
                    return KisImageBuilder_RESULT_UNSUPPORTED;
                }
                uint32 realTileWidth =  (x + tileWidth) < width ? tileWidth : ((x + tileWidth) - width);
                for (uint yintile = 0; y + yintile < height && yintile < tileHeight; yintile++) {
                    KisHLineIterator it = layer -> paintDevice() -> createHLineIterator(x, y + yintile, realTileWidth, true);
                    convertFromTIFFData( it, (uint8*)buf + yintile * linewidth, alphapos, color_type, depth, extrasamplescount, red, green, blue);
                }
                
            }
        }
        _TIFFfree(buf);
    } else {
        kdDebug() << "striped image" << endl;
        uint32 strip_height;
        TIFFGetField( image, TIFFTAG_ROWSPERSTRIP, &strip_height );
        tsize_t stripsize = TIFFStripSize(image);
        tdata_t buf = _TIFFmalloc(stripsize);
        uint32 y = 0;
        uint32 linewidth = (uint32)ceil((width * depth * nbchannels) / 8.);
        kdDebug() << " linewidth = " << width << " * " << depth << " * " << nbchannels << " / 8 = " << linewidth << " TIFFScanlineSize = " << TIFFScanlineSize(image) << endl;
//         if(depth < 8){ linewidth++; }
        for (y = 0; y < height; y++)
        { // TODO support for different planarity
            if( planarconfig == PLANARCONFIG_CONTIG )
            {
                TIFFReadScanline(image, buf, y, (tsize_t) -1);
            } else {
                kdDebug() <<  "Plannar configuration seperated is not supported" << endl;
                TIFFClose(image);
                return KisImageBuilder_RESULT_UNSUPPORTED;
            }
            KisHLineIterator it = layer -> paintDevice() -> createHLineIterator(0, y, width, true);
            convertFromTIFFData( it, (uint8*)buf, alphapos, color_type, depth, extrasamplescount,  red, green, blue);
        }
#if 0
        for (strip = 0; strip < TIFFNumberOfStrips(image); strip++)
        { // TODO support for different planarity
            if( planarconfig == PLANARCONFIG_CONTIG )
            {
                TIFFReadEncodedStrip(image, strip, buf, (tsize_t) -1);
            } else {
                kdDebug() <<  "Plannar configuration seperated is not supported" << endl;
                TIFFClose(image);
                return KisImageBuilder_RESULT_UNSUPPORTED;
            }
/*            if( fillorder == FILLORDER_LSB2MSB)
            {
                convertMSB2LSB(buf, stripsize );
            }*/
            for (uint yinstrip = 0; y < height && yinstrip < strip_height; y++, yinstrip++) {
                KisHLineIterator it = layer -> paintDevice() -> createHLineIterator(0, y, width, true);
                convertFromTIFFData( it, (uint8*)buf + (yinstrip * linewidth), alphapos, color_type, depth, extrasamplescount,  red, green, blue);
            }
        }
#endif
        _TIFFfree(buf);
    }

    // Freeing memory
    TIFFClose(image);
    return KisImageBuilder_RESULT_OK;
}

KisImageBuilder_Result KisTIFFConverter::buildImage(const KURL& uri)
{
    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!KIO::NetAccess::exists(uri, false, qApp -> mainWidget())) {
        return KisImageBuilder_RESULT_NOT_EXIST;
    }

    // We're not set up to handle asynchronous loading at the moment.
    KisImageBuilder_Result result = KisImageBuilder_RESULT_FAILURE;
    QString tmpFile;

    if (KIO::NetAccess::download(uri, tmpFile, qApp -> mainWidget())) {
        result = decode(tmpFile);
        KIO::NetAccess::removeTempFile(tmpFile);
    }

    return result;
}


KisImageSP KisTIFFConverter::image()
{
    return m_img;
}


KisImageBuilder_Result KisTIFFConverter::buildFile(const KURL& uri, KisPaintLayerSP layer, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, int compression, bool interlace, bool alpha)
{
#if 0
    kdDebug() << "Start writing TIFF File" << endl;
    if (!layer)
        return KisImageBuilder_RESULT_INVALID_ARG;

    KisImageSP img = layer -> image();
    if (!img)
        return KisImageBuilder_RESULT_EMPTY;

    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!uri.isLocalFile())
        return KisImageBuilder_RESULT_NOT_LOCAL;
    // Open file for writing
    FILE *fp = fopen(uri.path().ascii(), "wb");
    if (!fp)
    {
        return (KisImageBuilder_RESULT_FAILURE);
    }
    int height = img->height();
    int width = img->width();
    // Initialize structures
    png_structp png_ptr =  png_create_write_struct(TIFF_LIBTIFF_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
    if (!png_ptr) return (KisImageBuilder_RESULT_FAILURE);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return (KisImageBuilder_RESULT_FAILURE);
    }

    // If an error occurs during writing, libpng will jump here
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return (KisImageBuilder_RESULT_FAILURE);
    }
    // Initialize the writing
    png_init_io(png_ptr, fp);
    // Setup the progress function
// FIXME    png_set_write_status_fn(png_ptr, progress);
//     setProgressTotalSteps(100/*height*/);
            

    /* set the zlib compression level */
    png_set_compression_level(png_ptr, compression);

    /* set other zlib parameters */
    png_set_compression_mem_level(png_ptr, 8);
    png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_method(png_ptr, 8);
    png_set_compression_buffer_size(png_ptr, 8192);
    
    int color_nb_bits = 8 * layer->paintDevice()->pixelSize() / layer->paintDevice()->nChannels();
    int color_type = getColorTypeforColorSpace(img->colorSpace(), alpha);
    
    if(color_type == -1)
    {
        return KisImageBuilder_RESULT_UNSUPPORTED;
    }
    
    int interlacetype = interlace ? TIFF_INTERLACE_ADAM7 : TIFF_INTERLACE_NONE;
    
    png_set_IHDR(png_ptr, info_ptr,
                 width,
                 height,
                 color_nb_bits,
                 color_type, interlacetype,
                 TIFF_COMPRESSION_TYPE_DEFAULT, TIFF_FILTER_TYPE_DEFAULT);
    
    png_set_sRGB(png_ptr, info_ptr, TIFF_sRGB_INTENT_ABSOLUTE);
    
    // Save annotation
    vKisAnnotationSP_it it = annotationsStart;
    while(it != annotationsEnd) {
        if (!(*it) || (*it) -> type() == QString()) {
            kdDebug() << "Warning: empty annotation" << endl;
            ++it;
            continue;
        }

        kdDebug() << "Trying to store annotation of type " << (*it) -> type() << " of size " << (*it) -> annotation() . size() << endl;

        if ((*it) -> type().startsWith("krita_attribute:")) { // Attribute
            // FIXME: it should be possible to save krita_attributes in the "CHUNKs"
            kdDebug() << "can't save this annotation : " << (*it) -> type() << endl;
        } else { // Profile
            char* name = new char[(*it)->type().length()+1];
            strcpy(name, (*it)->type().ascii());
            png_set_iCCP(png_ptr, info_ptr, name, TIFF_COMPRESSION_TYPE_BASE, (char*)(*it)->annotation().data(), (*it) -> annotation() . size());
        }
        ++it;
    }

    // read comments from the document information
    png_text texts[3];
    int nbtexts = 0;
    KoDocumentInfo * info = m_doc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    QString title = aboutPage->title();
    if(title != "")
    {
        fillText(texts+nbtexts, "title", title);
        nbtexts++;
    }
    QString abstract = aboutPage->abstract();
    if(abstract != "")
    {
        fillText(texts+nbtexts, "abstract", abstract);
        nbtexts++;
    }
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    QString author = authorPage->fullName();
    if(author != "")
    {
        fillText(texts+nbtexts, "author", author);
        nbtexts++;
    }
    
    png_set_text(png_ptr, info_ptr, texts, nbtexts);
    
    // Save the information to the file
    png_write_info(png_ptr, info_ptr);
    png_write_flush(png_ptr);
    // Write the TIFF
//     png_write_png(png_ptr, info_ptr, TIFF_TRANSFORM_IDENTITY, NULL);
    
    // Fill the data structure
    png_byte** row_pointers= new png_byte*[height];
    
    for (int y = 0; y < height; y++) {
        KisHLineIterator it = layer->paintDevice()->createHLineIterator(0, y, width, false);
        row_pointers[y] = new png_byte[width*layer->paintDevice()->pixelSize()];
        switch(color_type)
        {
            case TIFF_COLOR_TYPE_GRAY:
            case TIFF_COLOR_TYPE_GRAY_ALPHA:
                if(color_nb_bits == 16)
                {
                    Q_UINT16 *dst = reinterpret_cast<Q_UINT16 *>(row_pointers[y]);
                    while (!it.isDone()) {
                        const Q_UINT16 *d = reinterpret_cast<const Q_UINT16 *>(it.rawData());
                        *(dst++) = d[0];
                        if(alpha) *(dst++) = d[1];
                        ++it;
                    }
                } else {
                    Q_UINT8 *dst = row_pointers[y];
                    while (!it.isDone()) {
                        const Q_UINT8 *d = it.rawData();
                        *(dst++) = d[0];
                        if(alpha) *(dst++) = d[1];
                        ++it;
                    }
                }
                break;
            case TIFF_COLOR_TYPE_RGB:
            case TIFF_COLOR_TYPE_RGB_ALPHA:
                if(color_nb_bits == 16)
                {
                    Q_UINT16 *dst = reinterpret_cast<Q_UINT16 *>(row_pointers[y]);
                    while (!it.isDone()) {
                        const Q_UINT16 *d = reinterpret_cast<const Q_UINT16 *>(it.rawData());
                        *(dst++) = d[2];
                        *(dst++) = d[1];
                        *(dst++) = d[0];
                        if(alpha) *(dst++) = d[3];
                        ++it;
                    }
                } else {
                    Q_UINT8 *dst = row_pointers[y];
                    while (!it.isDone()) {
                        const Q_UINT8 *d = it.rawData();
                        *(dst++) = d[2];
                        *(dst++) = d[1];
                        *(dst++) = d[0];
                        if(alpha) *(dst++) = d[3];
                        ++it;
                    }
                }
                break;
            default:
                return KisImageBuilder_RESULT_UNSUPPORTED;
        }
    }
    
    png_write_image(png_ptr, row_pointers);

    // Writting is over
    png_write_end(png_ptr, info_ptr);
    
    // Free memory
    png_destroy_write_struct(&png_ptr, &info_ptr);
    for (int y = 0; y < height; y++) {
        delete row_pointers[y];
    }
    delete row_pointers;

    fclose(fp);
    
    return KisImageBuilder_RESULT_OK;
#endif
}


void KisTIFFConverter::cancel()
{
    m_stop = true;
}

#include "kis_tiff_converter.moc"
