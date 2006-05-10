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

#include <config.h>
#include <lcms.h>

#include <QFile>

#include <kapplication.h>
#include <KoDocumentInfo.h>

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

#include "kis_tiff_reader.h"
#include "kis_tiff_ycbcr_reader.h"
#include "kis_tiff_stream.h"
#include "kis_tiff_writer_visitor.h"

#include <kis_ycbcr_u8_colorspace.h> // TODO: in the future YCbCrU8/16 will be moved in a plugins and we won't need anymore that cludge
#include <kis_ycbcr_u16_colorspace.h>

namespace {

    QString getColorSpaceForColorType(uint16 color_type, uint16 color_nb_bits, TIFF *image, uint16 &nbchannels, uint16 &extrasamplescount, uint8 &destDepth) {
        if(color_type == PHOTOMETRIC_MINISWHITE || color_type == PHOTOMETRIC_MINISBLACK)
        {
            if(nbchannels == 0) nbchannels = 1;
            extrasamplescount = nbchannels - 1; // FIX the extrasamples count in case of
            if(color_nb_bits <= 8)
            {
                destDepth = 8;
                return "GRAYA";
            } else {
                destDepth = 16;
                return "GRAYA16";
            }
        } else if(color_type == PHOTOMETRIC_RGB  /*|| color_type == */ ) {
            if(nbchannels == 0) nbchannels = 3;
            extrasamplescount = nbchannels - 3; // FIX the extrasamples count in case of
            if(color_nb_bits <= 8)
            {
                destDepth = 8;
                return "RGBA";
            } else {
                destDepth = 16;
                return "RGBA16";
            }
        } else if(color_type == PHOTOMETRIC_YCBCR ) {
            if(nbchannels == 0) nbchannels = 3;
            extrasamplescount = nbchannels - 3; // FIX the extrasamples count in case of
            if(color_nb_bits <= 8)
            {
                destDepth = 8;
                return "YCbCrAU8";
            } else {
                destDepth = 16;
                return "YCbCrAU16";
            }
        } else if(color_type == PHOTOMETRIC_SEPARATED ) {
            if(nbchannels == 0) nbchannels = 4;
            // SEPARATED is in general CMYK but not allways, so we check
            uint16 inkset;
            if((TIFFGetField(image, TIFFTAG_INKSET, &inkset) == 0)){
                kDebug(41008) <<  "Image does not define the inkset." << endl;
                inkset = 2;
            }
            if(inkset !=  INKSET_CMYK)
            {
                kDebug(41008) << "Unsupported inkset (right now, only CMYK is supported)" << endl;
                char** ink_names;
                uint16 numberofinks;
                if( TIFFGetField(image, TIFFTAG_INKNAMES, &ink_names) && TIFFGetField(image, TIFFTAG_NUMBEROFINKS, &numberofinks) )
                {
                    kDebug(41008) << "Inks are : " << endl;
                    for(uint i = 0; i < numberofinks; i++)
                    {
                        kDebug(41008) << ink_names[i] << endl;
                    }
                } else {
                    kDebug(41008) << "inknames aren't defined !" << endl;
                    // To be able to read stupid adobe files, if there are no information about inks and four channels, then it's a CMYK file :
                    if( nbchannels - extrasamplescount != 4)
                    {
                        return "";
                    }
                }
            }
            if(color_nb_bits <= 8)
            {
                destDepth = 8;
                return "CMYK";
            } else {
                destDepth = 16;
                return "CMYKA16";
            }
        } else if(color_type == PHOTOMETRIC_CIELAB || color_type == PHOTOMETRIC_ICCLAB ) {
            destDepth = 16;
            if(nbchannels == 0) nbchannels = 3;
            extrasamplescount = nbchannels - 3; // FIX the extrasamples count in case of
            return "LABA"; // TODO add support for a 8bit LAB colorspace when it is written
        } else if(color_type ==  PHOTOMETRIC_PALETTE) {
            destDepth = 16;
            if(nbchannels == 0) nbchannels = 2;
            extrasamplescount = nbchannels - 2; // FIX the extrasamples count in case of
            // <-- we will convert the index image to RGBA16 as the palette is allways on 16bits colors
            return "RGBA16";
        }
        return "";
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

KisImageBuilder_Result KisTIFFConverter::decode(const KUrl& uri)
{
    kDebug(41008) << "Start decoding TIFF File" << endl;
    // Opent the TIFF file
    TIFF *image = 0;
    if((image = TIFFOpen(QFile::encodeName(uri.path()), "r")) == NULL){
        kDebug(41008) << "Could not open the file, either it doesn't exist, either it is not a TIFF : " << uri.path() << endl;
        if (image) TIFFClose(image);
        return (KisImageBuilder_RESULT_BAD_FETCH);
    }
    do {
        kDebug(41008) << "Read new sub-image" << endl;
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
        kDebug(41008) <<  "Image does not define its width" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    if(TIFFGetField(image, TIFFTAG_IMAGELENGTH, &height) == 0){
        kDebug(41008) <<  "Image does not define its height" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    uint16 depth;
    if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &depth) == 0)){
        kDebug(41008) <<  "Image does not define its depth" << endl;
        depth = 1;
    }
    // Determine the number of channels (usefull to know if a file has an alpha or not
    uint16 nbchannels;
    if(TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &nbchannels) == 0){
        kDebug(41008) << "Image has an undefined number of samples per pixel" << endl;
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
        kDebug(41008) << "Image has an undefined photometric interpretation" << endl;
        color_type = PHOTOMETRIC_MINISWHITE;
    }
    uint8 dstDepth;
    QString csName = getColorSpaceForColorType(color_type, depth, image, nbchannels, extrasamplescount, dstDepth);
    if(csName.isEmpty()) {
        kDebug(41008) << "Image has an unsupported colorspace : " << color_type << " for this depth : "<< depth << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    kDebug(41008) << "Colorspace is : " << csName << " with a depth of " << depth << " and with a nb of channels of " << nbchannels << endl;
    
    // Read image profile
    kDebug() << "Reading profile" << endl;
    KisProfile* profile = 0;
    DWORD EmbedLen;
    LPBYTE EmbedBuffer;

    if (TIFFGetField(image, TIFFTAG_ICCPROFILE, &EmbedLen, &EmbedBuffer)) {
        kDebug(41008) << "Profile found" << endl;
        QByteArray rawdata;
        rawdata.resize(EmbedLen);
        memcpy(rawdata.data(), EmbedBuffer, EmbedLen);
        profile = new KisProfile(rawdata);
    } else {
        kDebug(41008) << "No Profile found" << endl;
    }
    
    // Retrieve a pointer to the colorspace
    KisColorSpace* cs = 0;
    if( csName == "YCbCrAU8" ) { // TODO: in the future YCbCrU8/16 will be moved in a plugins and we won't need anymore that cludge
        if (profile && profile->isSuitableForOutput())
        {
            kDebug(41008) << "image has embedded profile: " << profile -> productName() << "\n";
            cs = new KisYCbCrU8ColorSpace( KisMetaRegistry::instance()->csRegistry(), profile);
        } else {
            cs = new KisYCbCrU8ColorSpace( KisMetaRegistry::instance()->csRegistry(), 0);
        }
    } else if( csName == "YCbCrAU16") {
        if (profile && profile->isSuitableForOutput())
        {
            kDebug(41008) << "image has embedded profile: " << profile -> productName() << "\n";
            cs = new KisYCbCrU16ColorSpace( KisMetaRegistry::instance()->csRegistry(), profile);
        } else {
            cs = new KisYCbCrU16ColorSpace( KisMetaRegistry::instance()->csRegistry(), 0);
        }
    } else if (profile && profile->isSuitableForOutput())
    {
        kDebug(41008) << "image has embedded profile: " << profile -> productName() << "\n";
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(csName, profile);
    }
    else
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID(csName,""),"");

    if(cs == 0) {
        kDebug(41008) << "Colorspace " << csName << " is not available, please check your installation." << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    
    // Create the cmsTransform if needed 
    cmsHTRANSFORM transform = 0;
    if(profile && !profile->isSuitableForOutput())
    {
        kDebug(41008) << "The profile can't be used in krita, need conversion" << endl;
        transform = cmsCreateTransform(profile->profile(), cs->colorSpaceType(),
                                       cs->getProfile()->profile() , cs->colorSpaceType(),
                                       INTENT_PERCEPTUAL, 0);
    }

    
    // Check if there is an alpha channel
    int8 alphapos = -1; // <- no alpha
    // Check which extra is alpha if any
    kDebug(41008) << "There are " << nbchannels << " channels and " << extrasamplescount << " extra channels" << endl;
    if(sampleinfo) // index images don't have any sampleinfo, and therefor sampleinfo == 0
    {
        for(int i = 0; i < extrasamplescount; i ++)
        {
            kDebug(41008) << i << " " << extrasamplescount << " "  << (cs->nColorChannels()) <<  nbchannels << " " << sampleinfo[i] << endl;
            if(sampleinfo[i] == EXTRASAMPLE_ASSOCALPHA)
            {
                // XXX: dangelo: the color values are already multiplied with
                // the alpha value.  This needs to be reversed later (postprocessor?)
                alphapos = i;
            }
    
            if (sampleinfo[i] == EXTRASAMPLE_UNASSALPHA)
            {
                // color values are not premultiplied with alpha, and can be used as they are.
                alphapos = i;
            }
        }
    }
    
    // Read META Information
    KoDocumentInfo * info = m_doc->documentInfo();
    char* text;
    if (TIFFGetField(image, TIFFTAG_ARTIST, &text)) {
        info->setAuthorInfo("creator", text);
    }
    if (TIFFGetField(image, TIFFTAG_DOCUMENTNAME, &text)) {
        info->setAboutInfo("title", text);
    }
    if (TIFFGetField(image,TIFFTAG_IMAGEDESCRIPTION,&text) ) {
        info->setAboutInfo("description", text);
    }
    
    
    // Get the planar configuration
    uint16 planarconfig;
    if(TIFFGetField(image, TIFFTAG_PLANARCONFIG, &planarconfig) == 0)
    {
        kDebug(41008) <<  "Plannar configuration is not define" << endl;
        TIFFClose(image);
        return KisImageBuilder_RESULT_INVALID_ARG;
    }
    // Creating the KisImageSP
    if( ! m_img ) {
        m_img = new KisImage(m_doc->undoAdapter(), width, height, cs, "built image");
        Q_CHECK_PTR(m_img);
        m_img->blockSignals(true); // Don't send out signals while we're building the image
        if(profile)
        {
            m_img -> addAnnotation( profile->annotation() );
        }
    } else {
        if( m_img->width() < (qint32)width || m_img->height() < (qint32)height)
        {
            quint32 newwidth = (m_img->width() < (qint32)width) ? width : m_img->width();
            quint32 newheight = (m_img->height() < (qint32)height) ? height : m_img->height();
            m_img->resize(newwidth, newheight, false);
        }
    }
    KisPaintLayer* layer = new KisPaintLayer(m_img.data(), m_img -> nextLayerName(), quint8_MAX);
    tdata_t buf = 0;
    tdata_t* ps_buf = 0; // used only for planar configuration seperated
    TIFFStreamBase* tiffstream;
    
    KisTIFFReaderBase* tiffReader = 0;
    
    quint8 poses[5];
    KisTIFFPostProcessor* postprocessor = 0;
    
    // Configure poses
    uint8 nbcolorsamples = nbchannels - extrasamplescount;
    switch(color_type)
    {
        case PHOTOMETRIC_MINISWHITE:
        {
            poses[0] = 0; poses[1] = 1;
            postprocessor = new KisTIFFPostProcessorInvert(nbcolorsamples);
        }
        break;
        case PHOTOMETRIC_MINISBLACK:
        {
            poses[0] = 0; poses[1] = 1;
            postprocessor = new KisTIFFPostProcessor(nbcolorsamples);
        }
        break;
        case PHOTOMETRIC_CIELAB:
        {
            poses[0] = 0; poses[1] = 1; poses[2] = 2; poses[3] = 3;
            postprocessor = new KisTIFFPostProcessor(nbcolorsamples);
        }
        break;
        case PHOTOMETRIC_ICCLAB:
        {
            poses[0] = 0; poses[1] = 1; poses[2] = 2; poses[3] = 3;
            postprocessor = new KisTIFFPostProcessorICCLABtoCIELAB(nbcolorsamples);
        }
        break;
        case PHOTOMETRIC_RGB:
        {
            poses[0] = 2; poses[1] = 1; poses[2] = 0; poses[3] = 3;
            postprocessor = new KisTIFFPostProcessor(nbcolorsamples);
        }
        break;
        case PHOTOMETRIC_SEPARATED:
        {
            poses[0] = 0; poses[1] = 1; poses[2] = 2; poses[3] = 3; poses[4] = 4;
            postprocessor = new KisTIFFPostProcessor(nbcolorsamples);
        }
        break;
        default:
        break;
    }
    
    
    // Initisalize tiffReader
    uint16 * lineSizeCoeffs = new uint16[nbchannels];
    uint16 vsubsampling = 1;
    uint16 hsubsampling = 1;
    for(uint i = 0; i < nbchannels; i++)
    {
        lineSizeCoeffs[i] = 1;
    }
    if( color_type == PHOTOMETRIC_PALETTE)
    {
        uint16 *red; // No need to free them they are free by libtiff
        uint16 *green;
        uint16 *blue;
        if ((TIFFGetField(image, TIFFTAG_COLORMAP, &red, &green, &blue)) == 0)
        {
            kDebug(41008) <<  "Indexed image does not define a palette" << endl;
            TIFFClose(image);
            return KisImageBuilder_RESULT_INVALID_ARG;
        }

        tiffReader = new KisTIFFReaderFromPalette( layer->paintDevice(), red, green, blue, poses, alphapos, depth, nbcolorsamples, extrasamplescount, transform, postprocessor);
    } else if(color_type == PHOTOMETRIC_YCBCR ) {
        TIFFGetFieldDefaulted( image, TIFFTAG_YCBCRSUBSAMPLING, &hsubsampling, &vsubsampling );
        lineSizeCoeffs[1] = hsubsampling;
        lineSizeCoeffs[2] = hsubsampling;
        uint16 position;
        TIFFGetFieldDefaulted( image, TIFFTAG_YCBCRPOSITIONING, &position  );
        if( dstDepth == 8 )
        {
            tiffReader = new KisTIFFYCbCrReaderTarget8Bit(layer->paintDevice(), poses, alphapos, depth, nbcolorsamples, extrasamplescount, transform, postprocessor, hsubsampling, vsubsampling, (KisTIFFYCbCr::Position)position);
        } else if( dstDepth == 16 )
        {
            tiffReader = new KisTIFFYCbCrReaderTarget16Bit( layer->paintDevice(), poses, alphapos, depth, nbcolorsamples, extrasamplescount, transform, postprocessor, hsubsampling, vsubsampling, (KisTIFFYCbCr::Position)position);
        }
    } else if(dstDepth == 8)
    {
        tiffReader = new KisTIFFReaderTarget8bit( layer->paintDevice(), poses, alphapos, depth, nbcolorsamples, extrasamplescount, transform, postprocessor);
    } else if(dstDepth == 16) {
        tiffReader = new KisTIFFReaderTarget16bit( layer->paintDevice(), poses, alphapos, depth, nbcolorsamples, extrasamplescount, transform, postprocessor);
    }
    
    if(TIFFIsTiled(image))
    {
        kDebug(41008) << "tiled image" << endl;
        uint32 tileWidth, tileHeight;
        uint32 x, y;
        TIFFGetField(image, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(image, TIFFTAG_TILELENGTH, &tileHeight);
        uint32 linewidth = (tileWidth * depth * nbchannels) / 8;
        if(planarconfig == PLANARCONFIG_CONTIG)
        {
            buf = _TIFFmalloc(TIFFTileSize(image));
            if(depth < 16)
            {
                tiffstream = new TIFFStreamContigBelow16((uint8*)buf, depth, linewidth);
            } else if(depth < 32)
            {
                tiffstream = new TIFFStreamContigBelow32((uint8*)buf, depth, linewidth);
            } else {
                tiffstream = new TIFFStreamContigAbove32((uint8*)buf, depth, linewidth);
            }
        } else {
            ps_buf = new tdata_t[nbchannels];
            uint32 * lineSizes = new uint32[nbchannels];
            uint16 baseSize = TIFFTileSize(image)/nbchannels;
            for(uint i = 0; i < nbchannels; i++)
            {
                ps_buf[i] = _TIFFmalloc(baseSize);
                lineSizes[i] = baseSize / lineSizeCoeffs[i];
            }
            tiffstream = new TIFFStreamSeperate( (uint8**) ps_buf, nbchannels, depth, lineSizes);
            delete [] lineSizes;
        }
        kDebug(41008) << linewidth << " " << nbchannels << " " << layer->paintDevice()->colorSpace()->nColorChannels() << endl;
        for (y = 0; y < height; y+= tileHeight)
        {
            for (x = 0; x < width; x += tileWidth)
            {
                kDebug(41008) << "Reading tile x = " << x << " y = " << y << endl;
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
                for (uint yintile = 0; y + yintile < height && yintile < tileHeight/vsubsampling; ) {
                    tiffReader->copyDataToChannels( x, y + yintile , realTileWidth, tiffstream);
                    yintile += 1;
                    tiffstream->moveToLine( yintile );
                }
                tiffstream->restart();
            }
        }
    } else {
        kDebug(41008) << "striped image" << endl;
        tsize_t stripsize = TIFFStripSize(image);
        uint32 rowsPerStrip;
        TIFFGetFieldDefaulted(image, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);
        kDebug() << rowsPerStrip << " " << height << endl;
        rowsPerStrip = qMin(rowsPerStrip, height); // when TIFFNumberOfStrips(image) == 1 it might happen that rowsPerStrip is incorrectly set
        if(planarconfig == PLANARCONFIG_CONTIG)
        {
            buf = _TIFFmalloc(stripsize);
            if(depth < 16)
            {
                tiffstream = new TIFFStreamContigBelow16((uint8*)buf, depth, stripsize/rowsPerStrip);
            } else if(depth < 32)
            {
                tiffstream = new TIFFStreamContigBelow32((uint8*)buf, depth, stripsize/rowsPerStrip);
            } else {
                tiffstream = new TIFFStreamContigAbove32((uint8*)buf, depth, stripsize/rowsPerStrip);
            }
        } else {
            ps_buf = new tdata_t[nbchannels];
            uint32 scanLineSize = stripsize/rowsPerStrip;
            kDebug(41008) << " scanLineSize for each plan = " << scanLineSize << endl;
            uint32 * lineSizes = new uint32[nbchannels];
            for(uint i = 0; i < nbchannels; i++)
            {
                ps_buf[i] = _TIFFmalloc(stripsize);
                lineSizes[i] = scanLineSize / lineSizeCoeffs[i];
            }
            tiffstream = new TIFFStreamSeperate( (uint8**) ps_buf, nbchannels, depth, lineSizes);
            delete [] lineSizes;
        }

        kDebug(41008) << "Scanline size = " << TIFFRasterScanlineSize(image) << " / strip size = " << TIFFStripSize(image) << " / rowsPerStrip = " << rowsPerStrip << " stripsize/rowsPerStrip = " << stripsize/rowsPerStrip << endl;
        uint32 y = 0;
        kDebug(41008) << " NbOfStrips = " << TIFFNumberOfStrips(image) << " rowsPerStrip = " << rowsPerStrip << " stripsize = " << stripsize << endl;
        for (uint32 strip = 0; y < height; strip++)
        {
            if( planarconfig == PLANARCONFIG_CONTIG )
            {
                TIFFReadEncodedStrip(image, TIFFComputeStrip( image, y, 0 ) , buf, (tsize_t) -1);
            } else {
                for(uint i = 0; i < nbchannels; i++)
                {
                    TIFFReadEncodedStrip(image, TIFFComputeStrip( image, y, i ), ps_buf[i], (tsize_t) -1);
                }
            }
            for( uint32 yinstrip = 0 ; yinstrip < rowsPerStrip && y < height ; )
            {
                uint linesreaded = tiffReader->copyDataToChannels( 0, y, width, tiffstream);
                y += linesreaded;
                yinstrip += linesreaded;
                tiffstream->moveToLine( yinstrip );
            }
            tiffstream->restart();
        }
    }
    tiffReader->finalize();
    delete lineSizeCoeffs;
    delete tiffReader;
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
    
    // Convert YCbCr to RGB
    if( csName == "YCbCrAU8" ) { // TODO: in the future YCbCrU8/16 will be moved in a plugins and we won't need anymore that cludge
        KisColorSpace * dstCS = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID("RGBA", ""), "");
        m_img->convertTo(dstCS);
    } else if( csName == "YCbCrAU16") {
        KisColorSpace * dstCS = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID("RGBA16", ""), "");
        m_img->convertTo(dstCS);
    }

    m_img->addLayer(KisLayerSP(layer), m_img->rootLayer(), KisLayerSP(0));
    return KisImageBuilder_RESULT_OK;
}

KisImageBuilder_Result KisTIFFConverter::buildImage(const KUrl& uri)
{
    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!KIO::NetAccess::exists(uri, false, qApp -> activeWindow())) {
        return KisImageBuilder_RESULT_NOT_EXIST;
    }

    // We're not set up to handle asynchronous loading at the moment.
    KisImageBuilder_Result result = KisImageBuilder_RESULT_FAILURE;
    QString tmpFile;

    if (KIO::NetAccess::download(uri, tmpFile, qApp -> activeWindow())) {
        KUrl uriTF;
        uriTF.setPath( tmpFile );
        result = decode(uriTF);
        KIO::NetAccess::removeTempFile(tmpFile);
    }

    return result;
}


KisImageSP KisTIFFConverter::image()
{
    return m_img;
}


KisImageBuilder_Result KisTIFFConverter::buildFile(const KUrl& uri, KisImageSP img, KisTIFFOptions options)
{
    kDebug(41008) << "Start writing TIFF File" << endl;
    if (!img)
        return KisImageBuilder_RESULT_EMPTY;

    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!uri.isLocalFile())
        return KisImageBuilder_RESULT_NOT_LOCAL;
    
    // Open file for writing
    TIFF *image;
    if((image = TIFFOpen(QFile::encodeName(uri.path()), "w")) == NULL){
        kDebug(41008) << "Could not open the file for writting " << uri.path() << endl;
        TIFFClose(image);
        return (KisImageBuilder_RESULT_FAILURE);
    }

    // Set the document informations
    KoDocumentInfo * info = m_doc->documentInfo();
    QString title = info->aboutInfo("title");
    if(!title.isEmpty())
    {
        TIFFSetField(image, TIFFTAG_DOCUMENTNAME, title.toAscii().data());
    }
    QString abstract = info->aboutInfo("description");
    if(!abstract.isEmpty())
    {
        TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, abstract.toAscii().data());
    }
    QString author = info->authorInfo("creator");
    if(!author.isEmpty())
    {
        TIFFSetField(image, TIFFTAG_ARTIST, author.toAscii().data());
    }
    
    KisTIFFWriterVisitor* visitor = new KisTIFFWriterVisitor(image, &options);
    KisGroupLayer* root = dynamic_cast<KisGroupLayer*>(img->rootLayer().data());
    if(root == 0)
    {
        KIO::del(uri);
        TIFFClose(image);
        return KisImageBuilder_RESULT_FAILURE;
    }
    if(!visitor->visit( root ))
    {
        KIO::del(uri);
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
