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

    
    class TIFFStream {
        public:
            TIFFStream( uint16 depth) : m_depth(depth) {};
            virtual uint32 nextValueBelow16() =0;
            virtual uint32 nextValueBelow32() =0;
            virtual uint32 nextValueAbove32() =0;
            virtual void restart() =0;
        protected:
            uint16 m_depth;
    };
    class TIFFStreamContig : public TIFFStream {
        public:
            TIFFStreamContig( uint8* src, uint16 depth ) : TIFFStream(depth), m_src(src) { restart(); };
            virtual uint32 nextValueBelow16()
            {
                register uint8 remain;
                register uint32 value;
                remain = m_depth;
                value = 0;
                while (remain > 0)
                {
                    register uint8 toread;
                    toread = remain;
                    if (toread > m_posinc) toread = m_posinc;
                    remain -= toread;
                    m_posinc -= toread;
                    value = (value << toread) | (( (*m_srcit) >> (m_posinc) ) & ( ( 1 << toread ) - 1 ) );
                    if (m_posinc == 0)
                    {
                        m_srcit++;
                        m_posinc=8;
                    }
                }
                return value;
            }
            virtual uint32 nextValueBelow32()
            {
                register uint8 remain;
                register uint32 value;
                remain = m_depth;
                value = 0;
                while (remain > 0)
                {
                    register uint8 toread;
                    toread = remain;
                    if (toread > m_posinc) toread = m_posinc;
                    remain -= toread;
                    m_posinc -= toread;
                    value = (value) | ( (( (*m_srcit) >> (m_posinc) ) & ( ( 1 << toread ) - 1 ) ) << ( m_depth - 8 - remain ) );
                    if (m_posinc == 0)
                    {
                        m_srcit++;
                        m_posinc=8;
                    }
                }
                return value;
            }
            virtual uint32 nextValueAbove32()
            {
                register uint8 remain;
                register uint32 value;
                remain = m_depth;
                value = 0;
                while (remain > 0)
                {
                    register uint8 toread;
                    toread = remain;
                    if (toread > m_posinc) toread = m_posinc;
                    remain -= toread;
                    m_posinc -= toread;
                    if(remain < 32 )
                    {
                        value = (value) | ( (( (*m_srcit) >> (m_posinc) ) & ( ( 1 << toread ) - 1 ) ) << ( 24 - remain ) );
                    }
                    if (m_posinc == 0)
                    {
                        m_srcit++;
                        m_posinc=8;
                    }
                }
                return value;
            }
            virtual void restart()
            {
                m_srcit = m_src;
                m_posinc = 8;
            }
        private:
            uint8* m_src;
            uint8* m_srcit;
            uint8 m_posinc;
    };
    
    class TIFFStreamSeperate : public TIFFStream {
        public:
            TIFFStreamSeperate( uint8** srcs, uint8 nb_samples ,uint16 depth) : TIFFStream(depth), m_nb_samples(nb_samples)
            {
                streams = new TIFFStreamContig*[nb_samples];
                for(uint8 i = 0; i < m_nb_samples; i++)
                {
                    streams[i] = new TIFFStreamContig(srcs[i], depth);
                }
                restart();
            }
            ~TIFFStreamSeperate()
            {
                for(uint8 i = 0; i < m_nb_samples; i++)
                {
                    delete streams[i];
                }
                delete[] streams;
            }
            virtual uint32 nextValueBelow16()
            {
                uint32 value = streams[ m_current_sample ]->nextValueBelow16();
                if( (++m_current_sample) >= m_nb_samples)
                    m_current_sample = 0;
                return value;
            }
            virtual uint32 nextValueBelow32()
            {
                uint32 value = streams[ m_current_sample ]->nextValueBelow32();
                if( (++m_current_sample) >= m_nb_samples)
                    m_current_sample = 0;
                return value;
            }
            virtual uint32 nextValueAbove32()
            {
                uint32 value = streams[ m_current_sample ]->nextValueAbove32();
                if( (++m_current_sample) >= m_nb_samples)
                    m_current_sample = 0;
                return value;
            }
            virtual void restart()
            {
                m_current_sample = 0;
                for(uint8 i = 0; i < m_nb_samples; i++)
                {
                    streams[i]->restart();
                }
            }
        private:
            TIFFStreamContig** streams;
            uint8 m_current_sample, m_nb_samples;
    };
    
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

    
    QString getColorSpaceForColorType(uint16 color_type, uint16 color_nb_bits, TIFF *image, uint16 &nbchannels, uint16 extrasamplescount) {
        if(color_type == PHOTOMETRIC_MINISWHITE || color_type == PHOTOMETRIC_MINISBLACK)
        {
            if(nbchannels == 0) nbchannels = 1;
            if(color_nb_bits <= 8)
            {
                    return "GRAYA";
            } else {
                    return "GRAYA16";
            }
        } else if(color_type == PHOTOMETRIC_RGB ) {
            if(nbchannels == 0) nbchannels = 3;
            if(color_nb_bits <= 8)
            {
                return "RGBA";
            } else {
                return "RGBA16";
            }
        } else if(color_type == PHOTOMETRIC_SEPARATED ) {
            if(nbchannels == 0) nbchannels = 4;
            // SEPARATED is in general CMYK but not allways, so we check
            uint16 inkset;
            if((TIFFGetField(image, TIFFTAG_INKSET, &inkset) == 0)){
                kdDebug() <<  "Image does not define the inkset." << endl;
                inkset = 2;
            }
            if(inkset !=  INKSET_CMYK)
            {
                kdDebug() << "Unsupported inkset (right now, only CMYK is supported)" << endl;
                char** ink_names;
                uint16 numberofinks;
                if( TIFFGetField(image, TIFFTAG_INKNAMES, &ink_names) && TIFFGetField(image, TIFFTAG_NUMBEROFINKS, &numberofinks) )
                {
                    kdDebug() << "Inks are : " << endl;
                    for(uint i = 0; i < numberofinks; i++)
                    {
                        kdDebug() << ink_names[i] << endl;
                    }
                } else {
                    kdDebug() << "inknames aren't defined !" << endl;
                    // To be able to read stupid adobe files, if there are no information about inks and four channels, then it's a CMYK file :
                    if( nbchannels - extrasamplescount != 4)
                    {
                        return "";
                    }
                }
            }
            if(color_nb_bits <= 8)
            {
                return "CMYK";
            } else {
                return "CMYKA16";
            }
        } else if(color_type == PHOTOMETRIC_CIELAB ) {
            if(nbchannels == 0) nbchannels = 3;
            return "LABA"; // TODO add support for a 8bit LAB colorspace when it is written
        } else if(color_type ==  PHOTOMETRIC_PALETTE) {
            if(nbchannels == 0) nbchannels = 2;
            // <-- we will convert the index image to RGBA16 as the palette is allways on 16bits colors
            return "RGBA16";
        }
        return "";
    }
    
    typedef int32 (*copyTransform)(int32);
    
    int32 cTidentity(int32 v)
    {
        return v;
    }
    
    int32 cTinvert8(int32 v)
    {
        return Q_UINT8_MAX - v;
    }
    
    int32 cTinvert16(int32 v)
    {
        return Q_UINT16_MAX - v;
    }
    
    void copyDataToChannels( KisHLineIterator it, TIFFStream* tiffstream, int8 alphapos, uint8 depth, uint8 nbcolorssamples, uint8 extrasamplescount, Q_UINT8* poses, copyTransform transform = cTidentity)
    {
       if( depth <= 8 )
        {
            double coeff = Q_UINT8_MAX / (double)( ( 1 << depth ) - 1 );
//             kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
            while (!it.isDone()) {
                Q_UINT8 *d = it.rawData();
                Q_UINT8 i;
                for(i = 0; i < nbcolorssamples; i++)
                {
                    d[poses[i]] = transform( (Q_UINT32) ( tiffstream->nextValueBelow16() * coeff ) );
                }
                d[poses[i]] = Q_UINT8_MAX;
                for(int k = 0; k < extrasamplescount; k++)
                {
                    if(k == alphapos)
                        d[poses[i]] = (Q_UINT32) ( tiffstream->nextValueBelow16() * coeff );
                    else
                        tiffstream->nextValueBelow16();
                }
                ++it;
            }
        } else if( depth < 16 ) {
            double coeff = Q_UINT16_MAX / (double)( ( 1 << depth ) - 1 );
//             kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
            while (!it.isDone()) {
                Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                Q_UINT8 i;
                for(i = 0; i < nbcolorssamples; i++)
                {
                    d[poses[i]] = transform( (Q_UINT32) ( tiffstream->nextValueBelow16() * coeff ) );
                }
                d[poses[i]] = Q_UINT16_MAX;
                for(int k = 0; k < extrasamplescount; k++)
                {
                    if(k == alphapos)
                        d[poses[i]] = (Q_UINT32) ( tiffstream->nextValueBelow16() * coeff );
                    else
                        tiffstream->nextValueBelow16();
                }
                ++it;
            }
        } else if( depth < 32 ) {
            double coeff = Q_UINT16_MAX / (double)( ( 1 << depth ) - 1 );
//             kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
            while (!it.isDone()) {
                Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                Q_UINT8 i;
                for(i = 0; i < nbcolorssamples; i++)
                {
                    d[poses[i]] = (Q_UINT16) ( tiffstream->nextValueBelow32() * coeff );
                }
                d[poses[i]] = Q_UINT16_MAX;
                for(int k = 0; k < extrasamplescount; k++)
                {
                    if(k == alphapos)
                        d[poses[i]] = (Q_UINT16) ( tiffstream->nextValueBelow32() * coeff );
                    else
                        tiffstream->nextValueBelow32();
                }
                ++it;
            }
        } else {
            Q_UINT32 coeff =  1 << ( 16 );
//             kdDebug() << " depth expension coefficient : " << ((Q_UINT32)coeff) << endl;
            while (!it.isDone()) {
                Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                Q_UINT8 i;
                for(i = 0; i < nbcolorssamples; i++)
                {
                    d[poses[i]] = tiffstream->nextValueAbove32() / coeff;
                }
                d[poses[i]] = Q_UINT16_MAX;
                for(int k = 0; k < extrasamplescount; k++)
                {
                    if(k == alphapos)
                        d[poses[i]] = tiffstream->nextValueAbove32() / coeff;
                    else
                        tiffstream->nextValueBelow32();
                }
                ++it;
            }
        }
    }
    void convertFromTIFFData( KisHLineIterator it, TIFFStream* tiffstream, int8 alphapos, uint16 color_type, uint16 depth, uint8 nbcolorssamples, uint8 extrasamplescount, uint16 *red, uint16 *green, uint16 *blue)
    {
        switch(color_type)
        {
            case PHOTOMETRIC_MINISWHITE:
            {
                Q_UINT8 poses[]={ 0, 1};
                if(depth > 8)
                    copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, cTinvert16);
                else
                    copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses, cTinvert8);
            }
            break;
            case PHOTOMETRIC_MINISBLACK:
            {
                Q_UINT8 poses[]={ 0, 1};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses);
            }
            break;
            case PHOTOMETRIC_CIELAB:
            {
                Q_UINT8 poses[]={ 0, 1, 2, 3};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses);
            }
            break;
            case PHOTOMETRIC_RGB:
            {
                Q_UINT8 poses[]={ 2, 1, 0, 3};
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses);
            }
            break;
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
                copyDataToChannels(it, tiffstream, alphapos, depth, nbcolorssamples, extrasamplescount, poses);
            }
            break;
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
    do {
        kdDebug() << "Read new sub-image" << endl;
        readTIFFDirectory(image);
    } while (TIFFReadDirectory(image));
    // Freeing memory
    TIFFClose(image);

}

KisImageBuilder_Result KisTIFFConverter::readTIFFDirectory( TIFF* image)
{
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
        depth = 1;
    }
    // Determine the number of channels (usefull to know if a file has an alpha or not
    uint16 nbchannels;
    if(TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &nbchannels) == 0){
        kdDebug() << "Image has an undefined number of samples per pixel" << endl;
        nbchannels = 0;
    }
    // Get the number of extrasamples and information about them
    uint16 *sampleinfo, extrasamplescount;
    if(TIFFGetField(image, TIFFTAG_EXTRASAMPLES, &extrasamplescount, &sampleinfo) == 0)
    {
        extrasamplescount = 0;
    }
    // Determine the colorspace
    uint16 color_type;
    if(TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &color_type) == 0){
        kdDebug() << "Image has an undefined photometric interpretation" << endl;
        color_type = PHOTOMETRIC_MINISWHITE;
    }
    QString csName = getColorSpaceForColorType(color_type, depth, image, nbchannels, extrasamplescount);
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
        QByteArray rawdata;
        rawdata.resize(EmbedLen);
        memcpy(rawdata.data(), EmbedBuffer, EmbedLen);
        profile = new KisProfile(rawdata);
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
    // Creating the KisImageSP
    if( ! m_img ) {
        m_img = new KisImage(m_doc->undoAdapter(), width, height, cs, "built image");
        Q_CHECK_PTR(m_img);
        if(profile)
        {
            m_img -> addAnnotation( profile->annotation() );
        }
    } else {
        if( m_img->width() < width || m_img->height() < height)
        {
            Q_UINT32 newwidth = (m_img->width() < width) ? width : m_img->width();
            Q_UINT32 newheight = (m_img->height() < height) ? height : m_img->height();
            m_img->resize(newwidth, newheight, false);
        }
    }
    KisPaintLayer* layer = new KisPaintLayer(m_img, m_img -> nextLayerName(), Q_UINT8_MAX);
    m_img->addLayer(layer, m_img->rootLayer(), 0);
    tdata_t buf = 0;
    tdata_t* ps_buf = 0; // used only for planar configuration seperated
    TIFFStream* tiffstream;
    if(TIFFIsTiled(image))
    {
        kdDebug() << "tiled image" << endl;
        uint32 tileWidth, tileHeight;
        uint32 x, y;
        TIFFGetField(image, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(image, TIFFTAG_TILELENGTH, &tileHeight);
        if(planarconfig == PLANARCONFIG_CONTIG)
        {
            buf = _TIFFmalloc(TIFFTileSize(image));
            tiffstream = new TIFFStreamContig((uint8*)buf, depth);
        } else {
            ps_buf = new tdata_t[nbchannels];
            for(uint i = 0; i < nbchannels; i++)
            {
                ps_buf[i] = _TIFFmalloc(TIFFTileSize(image)/nbchannels);
            }
            tiffstream = new TIFFStreamSeperate( (uint8**) ps_buf, nbchannels, depth);
        }

        uint32 linewidth = (tileWidth * depth * nbchannels) / 8;
        kdDebug() << linewidth << " " << nbchannels << " " << layer->paintDevice()->colorSpace()->nColorChannels() << endl;
        for (y = 0; y < height; y+= tileHeight)
        {
            for (x = 0; x < width; x += tileWidth)
            {
                kdDebug() << "Reading tile x = " << x << " y = " << y << endl;
                if( planarconfig == PLANARCONFIG_CONTIG )
                {
                    TIFFReadTile(image, buf, x, y, 0, (tsize_t) -1);
                } else {
                    for(uint i = 0; i < nbchannels; i++)
                    {
                        TIFFReadTile(image, ps_buf[i], x, y, 0, i);
                    }
                }
                uint32 realTileWidth =  (x + tileWidth) < width ? tileWidth : width - x;
                for (uint yintile = 0; y + yintile < height && yintile < tileHeight; yintile++) {
                    KisHLineIterator it = layer -> paintDevice() -> createHLineIterator(x, y + yintile, realTileWidth, true);
                    convertFromTIFFData( it, tiffstream, alphapos, color_type, depth, nbchannels - extrasamplescount, extrasamplescount, red, green, blue);
                }
                tiffstream->restart();
            }
        }
    } else {
        kdDebug() << "striped image" << endl;
        uint32 strip_height;
        TIFFGetField( image, TIFFTAG_ROWSPERSTRIP, &strip_height );
        tsize_t stripsize = TIFFStripSize(image);
        if(planarconfig == PLANARCONFIG_CONTIG)
        {
            buf = _TIFFmalloc(stripsize);
            tiffstream = new TIFFStreamContig((uint8*)buf, depth);
        } else {
            ps_buf = new tdata_t[nbchannels];
            for(uint i = 0; i < nbchannels; i++)
            {
                ps_buf[i] = _TIFFmalloc(stripsize/nbchannels);
            }
            tiffstream = new TIFFStreamSeperate( (uint8**) ps_buf, nbchannels, depth);
        } 
        uint32 y = 0;
        for (y = 0; y < height; y++)
        {
            if( planarconfig == PLANARCONFIG_CONTIG )
            {
                TIFFReadScanline(image, buf, y, (tsize_t) -1);
            } else {
                for(uint i = 0; i < nbchannels; i++)
                {
                    TIFFReadScanline(image, ps_buf[i], y, i);
                }
            }
            KisHLineIterator it = layer -> paintDevice() -> createHLineIterator(0, y, width, true);
            convertFromTIFFData( it, tiffstream, alphapos, color_type, depth, nbchannels - extrasamplescount, extrasamplescount,  red, green, blue);
            tiffstream->restart();
        }
    }
    delete tiffstream;
    if( planarconfig == PLANARCONFIG_CONTIG )
    {
        _TIFFfree(buf);
    } else {
        for(uint i = 0; i < nbchannels; i++)
        {
            _TIFFfree(ps_buf[i]);
        }
        delete[] ps_buf;
    }

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


KisImageBuilder_Result KisTIFFConverter::buildFile(const KURL& uri, KisImageSP img, KisTIFFOptions options)
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
    
#endif
    return KisImageBuilder_RESULT_OK;
}


void KisTIFFConverter::cancel()
{
    m_stop = true;
}

#include "kis_tiff_converter.moc"
