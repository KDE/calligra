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
 
#include "kis_jpeg_converter.h"

#include <stdio.h>

extern "C" {
#include <iccjpeg.h>
}

#include <QFile>

#include <kapplication.h>
#include <KoDocumentInfo.h>

#include <kio/netaccess.h>

#include <kis_abstract_colorspace.h>
#include <kis_colorspace_factory_registry.h>
#include <kis_doc.h>
#include <kis_image.h>
#include <kis_iterators_pixel.h>
#include <kis_paint_layer.h>
#include <kis_group_layer.h>
#include <kis_meta_registry.h>
#include <kis_profile.h>

#include <kis_exif_io.h>

extern "C" {
#include <libexif/exif-loader.h>
#include <libexif/exif-utils.h>
}

#define ICC_MARKER  (JPEG_APP0 + 2) /* JPEG marker code for ICC */
#define ICC_OVERHEAD_LEN  14    /* size of non-profile data in APP2 */
#define MAX_BYTES_IN_MARKER  65533  /* maximum data len of a JPEG marker */
#define MAX_DATA_BYTES_IN_MARKER  (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)

namespace {
    
    J_COLOR_SPACE getColorTypeforColorSpace( KisColorSpace * cs)
    {
        if ( cs->id() == KisID("GRAYA") || cs->id() == KisID("GRAYA16") )
        {
            return JCS_GRAYSCALE;
        }
        if ( cs->id() == KisID("RGBA") || cs->id() == KisID("RGBA16") )
        {
            return JCS_RGB;
        }
        if ( cs->id() == KisID("CMYK") || cs->id() == KisID("CMYK16") )
        {
            return JCS_CMYK;
        }
        kDebug(41008) << "Cannot export images in " + cs->id().name() + " yet.\n";
        return JCS_UNKNOWN;
    }

    QString getColorSpaceForColorType(J_COLOR_SPACE color_type) {
        kDebug(41008) << "color_type = " << color_type << endl;
        if(color_type == JCS_GRAYSCALE)
        {
            return "GRAYA";
        } else if(color_type == JCS_RGB) {
            return "RGBA";
        } else if(color_type == JCS_CMYK) {
            return "CMYK";
        }
        return "";
    }

}

KisJPEGConverter::KisJPEGConverter(KisDoc *doc, KisUndoAdapter *adapter)
{
    m_doc = doc;
    m_adapter = adapter;
    m_job = 0;
    m_stop = false;
}

KisJPEGConverter::~KisJPEGConverter()
{
}

KisImageBuilder_Result KisJPEGConverter::decode(const KUrl& uri)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    // open the file
    FILE *fp = fopen(QFile::encodeName(uri.path()), "rb");
    if (!fp)
    {
        return (KisImageBuilder_RESULT_NOT_EXIST);
    }
    jpeg_stdio_src(&cinfo, fp);
    
    jpeg_save_markers (&cinfo, JPEG_COM, 0xFFFF);
    /* Save APP0..APP15 markers */
    for (int m = 0; m < 16; m++)
        jpeg_save_markers (&cinfo, JPEG_APP0 + m, 0xFFFF);

    
//     setup_read_icc_profile(&cinfo);
    // read header
    jpeg_read_header(&cinfo, true);
    
    // start reading
    jpeg_start_decompress(&cinfo);
    
    // Get the colorspace
    QString csName = getColorSpaceForColorType(cinfo.out_color_space);
    if(csName.isEmpty()) {
        kDebug(41008) << "unsupported colorspace : " << cinfo.out_color_space << endl;
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    uchar* profile_data;
    uint profile_len;
    KisProfile* profile = 0;
    QByteArray profile_rawdata;
    if( read_icc_profile (&cinfo, &profile_data, &profile_len))
    {
        profile_rawdata.resize(profile_len);
        memcpy(profile_rawdata.data(), profile_data, profile_len);
        cmsHPROFILE hProfile = cmsOpenProfileFromMem(profile_data, (DWORD)profile_len);

        if (hProfile != (cmsHPROFILE) NULL) {
            profile = new KisProfile( profile_rawdata);
            Q_CHECK_PTR(profile);
            kDebug(41008) << "profile name: " << profile->productName() << " profile description: " << profile->productDescription() << " information sur le produit: " << profile->productInfo() << endl;
            if(!profile->isSuitableForOutput())
            {
                kDebug(41008) << "the profile is not suitable for output and therefore cannot be used in krita, we need to convert the image to a standard profile" << endl; // TODO: in ko2 popup a selection menu to inform the user
            }
        }
    }
    
    // Retrieve a pointer to the colorspace
    KisColorSpace* cs;
    if (profile && profile->isSuitableForOutput())
    {
        kDebug(41008) << "image has embedded profile: " << profile -> productName() << "\n";
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(csName, profile);
    }
    else
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID(csName,""),"");

    if(cs == 0)
    {
        kDebug(41008) << "unknown colorspace" << endl;
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    
    // Create the cmsTransform if needed 
    
    cmsHTRANSFORM transform = 0;
    if(profile && !profile->isSuitableForOutput())
    {
        transform = cmsCreateTransform(profile->profile(), cs->colorSpaceType(),
                                       cs->getProfile()->profile() , cs->colorSpaceType(),
                                       INTENT_PERCEPTUAL, 0);
    }
    
    // Creating the KisImageSP
    if( ! m_img) {
        m_img = new KisImage(m_doc->undoAdapter(),  cinfo.image_width,  cinfo.image_height, cs, "built image");
        Q_CHECK_PTR(m_img);
        if(profile && !profile->isSuitableForOutput())
        {
            m_img -> addAnnotation( KisAnnotationSP(new KisAnnotation( profile->productName(), "", profile_rawdata)) );
        }
    }

    KisPaintLayerSP layer = KisPaintLayerSP(new KisPaintLayer(m_img.data(), m_img -> nextLayerName(), quint8_MAX));
    
    // Read exif information if any
    
    // Read data
    JSAMPROW row_pointer = new JSAMPLE[cinfo.image_width*cinfo.num_components];
    
    for (; cinfo.output_scanline < cinfo.image_height;) {
        KisHLineIterator it = layer->paintDevice()->createHLineIterator(0, cinfo.output_scanline, cinfo.image_width, true);
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        quint8 *src = row_pointer;
        switch(cinfo.out_color_space)
        {
            case JCS_GRAYSCALE:
                while (!it.isDone()) {
                    quint8 *d = it.rawData();
                    d[0] = *(src++);
                    if(transform) cmsDoTransform(transform, d, d, 1);
                    d[1] = quint8_MAX;
                    ++it;
                }
                break;
            case JCS_RGB:
                while (!it.isDone()) {
                    quint8 *d = it.rawData();
                    d[2] = *(src++);
                    d[1] = *(src++);
                    d[0] = *(src++);
                    if(transform) cmsDoTransform(transform, d, d, 1);
                    d[3] = quint8_MAX;
                    ++it;
                }
                break;
            case JCS_CMYK:
                while (!it.isDone()) {
                    quint8 *d = it.rawData();
                    d[0] = quint8_MAX - *(src++);
                    d[1] = quint8_MAX - *(src++);
                    d[2] = quint8_MAX - *(src++);
                    d[3] = quint8_MAX - *(src++);
                    if(transform) cmsDoTransform(transform, d, d, 1);
                    d[4] = quint8_MAX;
                    ++it;
                }
                break;
            default:
                return KisImageBuilder_RESULT_UNSUPPORTED;
        }
    }
    
    m_img->addLayer(KisLayerSP(layer.data()), m_img->rootLayer(), KisLayerSP(0));
    
    // Read exif information

    kDebug(41008) << "Looking for exif information" << endl;
    
    for (jpeg_saved_marker_ptr marker = cinfo.marker_list; marker != NULL; marker = marker->next) {
        kDebug(41008) << "Marker is " << marker->marker << endl;
        if (marker->marker != (JOCTET) (JPEG_APP0 + 1) ||
            marker->data_length < 14)
            continue; /* Exif data is in an APP1 marker of at least 14 octets */

        if (GETJOCTET (marker->data[0]) != (JOCTET) 0x45 ||
            GETJOCTET (marker->data[1]) != (JOCTET) 0x78 ||
            GETJOCTET (marker->data[2]) != (JOCTET) 0x69 ||
            GETJOCTET (marker->data[3]) != (JOCTET) 0x66 ||
            GETJOCTET (marker->data[4]) != (JOCTET) 0x00 ||
            GETJOCTET (marker->data[5]) != (JOCTET) 0x00)
            continue; /* no Exif header */
        kDebug(41008) << "Found exif information of length : "<< marker->data_length << endl;
        KisExifIO exifIO(layer->paintDevice()->exifInfo());
        exifIO.readExifFromMem( marker->data , marker->data_length );
        // Interpret orientation tag
        ExifValue v;
        if( layer->paintDevice()->exifInfo()->getValue("Orientation", v) && v.type() == ExifValue::EXIF_TYPE_SHORT)
        {
            switch(v.asShort(0)) //
            {
                case 2:
                    layer->paintDevice()->mirrorY();
                    break;
                case 3:
                    image()->rotate(180, 0);
                    break;
                case 4:
                    layer->paintDevice()->mirrorX();
                    break;
                case 5:
                    image()->rotate(90, 0);
                    layer->paintDevice()->mirrorY();
                    break;
                case 6:
                    image()->rotate(90, 0);
                    break;
                case 7:
                    image()->rotate(90, 0);
                    layer->paintDevice()->mirrorX();
                    break;
                case 8:
                    image()->rotate(270, 0);
                    break;
                default:
                    break;
            }
            v.setValue(0, (quint16)1);
            layer->paintDevice()->exifInfo()->setValue("Orientation", v);
        }
        break;
    }

    // Finish decompression
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    delete row_pointer;
    return KisImageBuilder_RESULT_OK;
}



KisImageBuilder_Result KisJPEGConverter::buildImage(const KUrl& uri)
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
        KUrl uriTF;
        uriTF.setPath( tmpFile );
        result = decode(uriTF);
        KIO::NetAccess::removeTempFile(tmpFile);
    }

    return result;
}


KisImageSP KisJPEGConverter::image()
{
    return m_img;
}


KisImageBuilder_Result KisJPEGConverter::buildFile(const KUrl& uri, KisPaintLayerSP layer, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, KisJPEGOptions options, KisExifInfo* exifInfo)
{
    if (!layer)
        return KisImageBuilder_RESULT_INVALID_ARG;

    KisImageSP img = KisImageSP(layer -> image());
    if (!img)
        return KisImageBuilder_RESULT_EMPTY;

    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!uri.isLocalFile())
        return KisImageBuilder_RESULT_NOT_LOCAL;
    // Open file for writing
    FILE *fp = fopen(QFile::encodeName(uri.path()), "wb");
    if (!fp)
    {
        return (KisImageBuilder_RESULT_FAILURE);
    }
    uint height = img->height();
    uint width = img->width();
    // Initialize structure
    struct jpeg_compress_struct cinfo;
    jpeg_create_compress(&cinfo);
    // Initialize error output
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    // Initialize output stream
    jpeg_stdio_dest(&cinfo, fp);
    
    cinfo.image_width = width;  // image width and height, in pixels
    cinfo.image_height = height;
    cinfo.input_components = img->colorSpace()->nColorChannels(); // number of color channels per pixel */
    J_COLOR_SPACE color_type = getColorTypeforColorSpace(img->colorSpace());
    if(color_type == JCS_UNKNOWN)
    {
        KIO::del(uri);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    cinfo.in_color_space = color_type;   // colorspace of input image

    
    // Set default compression parameters
    jpeg_set_defaults(&cinfo);
    // Customize them
    jpeg_set_quality(&cinfo, options.quality, true);
    
    if(options.progressive)
    {
        jpeg_simple_progression (&cinfo);
    }
    
    // Start compression
    jpeg_start_compress(&cinfo, true);
    // Save exif information if any available
    if(exifInfo)
    {
        kDebug(41008) << "Trying to save exif information" << endl;
        KisExifIO exifIO(exifInfo);
        unsigned char* exif_data;
        unsigned int exif_size;
        exifIO.saveExifToMem( &exif_data, &exif_size);
        kDebug(41008) << "Exif information size is " << exif_size << endl;
        if (exif_size < MAX_DATA_BYTES_IN_MARKER)
        {
            jpeg_write_marker(&cinfo, JPEG_APP0 + 1, exif_data, exif_size);
        } else {
            kDebug(41008) << "exif information couldn't be saved." << endl;
        }
    }
    

    // Save annotation
    vKisAnnotationSP_it it = annotationsStart;
    while(it != annotationsEnd) {
        if (!(*it) || (*it) -> type() == QString()) {
            kDebug(41008) << "Warning: empty annotation" << endl;
            ++it;
            continue;
        }

        kDebug(41008) << "Trying to store annotation of type " << (*it) -> type() << " of size " << (*it) -> annotation() . size() << endl;

        if ((*it) -> type().startsWith("krita_attribute:")) { // Attribute
            // FIXME
            kDebug(41008) << "can't save this annotation : " << (*it) -> type() << endl;
        } else { // Profile
            //char* name = new char[(*it)->type().length()+1];
            write_icc_profile(& cinfo, (uchar*)(*it)->annotation().data(), (*it)->annotation().size());
        }
        ++it;
    }

    
    // Write data information
    
    JSAMPROW row_pointer = new JSAMPLE[width*cinfo.input_components];
    int color_nb_bits = 8 * layer->paintDevice()->pixelSize() / layer->paintDevice()->nChannels();
    
    for (; cinfo.next_scanline < height;) {
        KisHLineIterator it = layer->paintDevice()->createHLineIterator(0, cinfo.next_scanline, width, false);
        quint8 *dst = row_pointer;
        switch(color_type)
        {
            case JCS_GRAYSCALE:
                if(color_nb_bits == 16)
                {
                    while (!it.isDone()) {
                        const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
                        *(dst++) = d[0] / quint8_MAX;
                        ++it;
                    }
                } else {
                    while (!it.isDone()) {
                        const quint8 *d = it.rawData();
                        *(dst++) = d[0];
                        ++it;
                    }
                }
                break;
            case JCS_RGB:
                if(color_nb_bits == 16)
                {
                    while (!it.isDone()) {
                        const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
                        *(dst++) = d[2] / quint8_MAX;
                        *(dst++) = d[1] / quint8_MAX;
                        *(dst++) = d[0] / quint8_MAX;
                        ++it;
                    }
                } else {
                    while (!it.isDone()) {
                        const quint8 *d = it.rawData();
                        *(dst++) = d[2];
                        *(dst++) = d[1];
                        *(dst++) = d[0];
                        ++it;
                    }
                }
                break;
            case JCS_CMYK:
                if(color_nb_bits == 16)
                {
                    while (!it.isDone()) {
                        const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
                        *(dst++) = quint8_MAX - d[0] / quint8_MAX;
                        *(dst++) = quint8_MAX - d[1] / quint8_MAX;
                        *(dst++) = quint8_MAX - d[2] / quint8_MAX;
                        *(dst++) = quint8_MAX - d[3] / quint8_MAX;
                        ++it;
                    }
                } else {
                    while (!it.isDone()) {
                        const quint8 *d = it.rawData();
                        *(dst++) = quint8_MAX - d[0];
                        *(dst++) = quint8_MAX - d[1];
                        *(dst++) = quint8_MAX - d[2];
                        *(dst++) = quint8_MAX - d[3];
                        ++it;
                    }
                }
                break;
            default:
                KIO::del(uri);
                return KisImageBuilder_RESULT_UNSUPPORTED;
        }
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    

    // Writting is over
    jpeg_finish_compress(&cinfo);
    fclose(fp);
    
    delete row_pointer;
    // Free memory
    jpeg_destroy_compress(&cinfo);
    
    return KisImageBuilder_RESULT_OK;
}


void KisJPEGConverter::cancel()
{
    m_stop = true;
}

#include "kis_jpeg_converter.moc"

