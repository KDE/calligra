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
#include <kis_group_layer.h>
#include <kis_paint_layer.h>

#include "kis_tiff_stream.h"
#include "kis_tiff_writer_visitor.h"

namespace {

    const Q_UINT8 PIXEL_BLUE = 0;
    const Q_UINT8 PIXEL_GREEN = 1;
    const Q_UINT8 PIXEL_RED = 2;
    const Q_UINT8 PIXEL_ALPHA = 3;

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
        KisImageBuilder_Result result = readTIFFDirectory(image);
        if(result != KisImageBuilder_RESULT_OK){
            return result;
        }
    } while (TIFFReadDirectory(image));
    // Freeing memory
    TIFFClose(image);
    return KisImageBuilder_RESULT_OK;
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
        if( m_img->width() < (Q_INT32)width || m_img->height() < (Q_INT32)height)
        {
            Q_UINT32 newwidth = (m_img->width() < (Q_INT32)width) ? width : m_img->width();
            Q_UINT32 newheight = (m_img->height() < (Q_INT32)height) ? height : m_img->height();
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
                    TIFFReadTile(image, buf, x, y, 0, (tsample_t) -1);
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
                TIFFReadScanline(image, buf, y, (tsample_t) -1);
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
    kdDebug() << "Start writing TIFF File" << endl;
    if (!img)
        return KisImageBuilder_RESULT_EMPTY;

    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!uri.isLocalFile())
        return KisImageBuilder_RESULT_NOT_LOCAL;
    
    // Open file for writing
    TIFF *image;
    if((image = TIFFOpen(uri.path().ascii(), "w")) == NULL){
        kdDebug() << "Could not open the file for writting " << uri.path() << endl;
        TIFFClose(image);
        return (KisImageBuilder_RESULT_FAILURE);
    }

    // Set the compression options
    TIFFSetField(image, TIFFTAG_COMPRESSION, options.compressionType);
    TIFFSetField(image, TIFFTAG_FAXMODE, options.faxMode);
    TIFFSetField(image, TIFFTAG_JPEGQUALITY, options.jpegQuality);
    TIFFSetField(image, TIFFTAG_ZIPQUALITY, options.deflateCompress);
    TIFFSetField(image, TIFFTAG_PIXARLOGQUALITY, options.pixarLogCompress);
    
    // Set the predictor
    TIFFSetField(image, TIFFTAG_PREDICTOR, options.predictor);
    // Set the document informations
    KoDocumentInfo * info = m_doc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    QString title = aboutPage->title();
    if(title != "")
    {
        TIFFSetField(image, TIFFTAG_DOCUMENTNAME, title.ascii());
    }
    QString abstract = aboutPage->abstract();
    if(abstract != "")
    {
        TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, abstract.ascii());
    }
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    QString author = authorPage->fullName();
    if(author != "")
    {
        TIFFSetField(image, TIFFTAG_ARTIST, author.ascii());
    }
    // Use contiguous configuration
    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    // Use 8 rows per strip
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, 8);
    
    KisTIFFWriterVisitor* visitor = new KisTIFFWriterVisitor(image, options.alpha);
    KisGroupLayer* root = dynamic_cast<KisGroupLayer*>(img->rootLayer().data());
    if(root == 0)
    {
        TIFFClose(image);
        return KisImageBuilder_RESULT_FAILURE;
    }
    if(!visitor->visit( root ))
    {
        TIFFClose(image);
        return KisImageBuilder_RESULT_FAILURE;
    }
    
    TIFFClose(image);
    return KisImageBuilder_RESULT_OK;
}


void KisTIFFConverter::cancel()
{
    m_stop = true;
}

#include "kis_tiff_converter.moc"
