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
#include "kis_png_converter.h"

#include <stdio.h>

#include <kapplication.h>
#include <kio/netaccess.h>

#include <kis_abstract_colorspace.h>
#include <kis_colorspace_factory_registry.h>
#include <kis_doc.h>
#include <kis_image.h>
#include <kis_iterators_pixel.h>
#include <kis_layer.h>
#include <kis_meta_registry.h>
#include <kis_profile.h>

namespace {

    const Q_UINT8 PIXEL_BLUE = 0;
    const Q_UINT8 PIXEL_GREEN = 1;
    const Q_UINT8 PIXEL_RED = 2;
    const Q_UINT8 PIXEL_ALPHA = 3;

    
    int getColorTypeforColorSpace( KisColorSpace * cs )
    {
        if ( cs->id() == KisID("GRAYA") || cs->id() == KisID("GRAYA16") ) return PNG_COLOR_TYPE_GRAY_ALPHA;
        if ( cs->id() == KisID("RGBA") || cs->id() == KisID("RGBA16") ) return PNG_COLOR_TYPE_RGB_ALPHA;

        kdDebug() << "Cannot export images in " + cs->id().name() + " yet.\n";
        return -1;

    }

    
    QString getColorSpaceForColorType(int color_type,int color_nb_bits) {
        if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        {
            switch(color_nb_bits)
            {
                case 8:
                    return "GRAYA";
                case 16:
                    return "GRAYA16";
            }
        } else if(color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_RGB) {
            switch(color_nb_bits)
            {
                case 8:
                    return "RGBA";
                case 16:
                    return "RGBA16";
            }
        }
        return "";
    }

/*    KisProfileSP getProfileForProfileInfo(const Image * image, KisAbstractColorSpace * cs)
    {
    }

    void setAnnotationsForImage(const Image * src, KisImageSP image)
    {
    }

    void exportAnnotationsForImage(Image * dst, vKisAnnotationSP_it& it, vKisAnnotationSP_it& annotationsEnd)
    {
    }

    MagickBooleanType monitor(const char *text, const ExtendedSignedIntegralType, const ExtendedUnsignedIntegralType, ExceptionInfo *)
    {
        KApplication *app = KApplication::kApplication();

        Q_ASSERT(app);

        if (app -> hasPendingEvents())
            app -> processEvents();

        printf("%s\n", text);
        return MagickTrue;
    }*/
}

KisPNGConverter::KisPNGConverter(KisDoc *doc, KisUndoAdapter *adapter)
{
    m_doc = doc;
    m_adapter = adapter;
    m_job = 0;
    m_stop = false;
}

KisPNGConverter::~KisPNGConverter()
{
}

KisImageBuilder_Result KisPNGConverter::decode(const KURL& uri)
{
    // open the file
    FILE *fp = fopen(uri.path().ascii(), "rb");
    if (!fp)
    {
        return (KisImageBuilder_RESULT_NOT_EXIST);
    }
    png_byte signature[8];
    fread(signature, 1, 8, fp);
    if (!png_check_sig(signature, 8))
    {
        return (KisImageBuilder_RESULT_BAD_FETCH);
    }

    // Initialize the internal structures
    png_structp png_ptr =  png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
    if (!KisImageBuilder_RESULT_FAILURE)
        return (KisImageBuilder_RESULT_FAILURE);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return (KisImageBuilder_RESULT_FAILURE);
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return (KisImageBuilder_RESULT_FAILURE);
    }

    // Catch errors
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return (KisImageBuilder_RESULT_FAILURE);
    }
    
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    
    // read all PNG info up to image data
    png_read_info(png_ptr, info_ptr);

    // Read information about the png
    png_uint_32 width, height;
    int color_nb_bits, color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &color_nb_bits, &color_type, NULL, NULL, NULL);

    // Determine the colorspace
    QString csName = getColorSpaceForColorType(color_type, color_nb_bits);
    if(csName == "") {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    bool hasalpha = (color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_GRAY_ALPHA);
    
    // Read image profile
    png_charp profile_name, profile_data;
    int compression_type;
    png_uint_32 proflen;
    
    png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &proflen);
    
    KisProfile* profile = 0;
    QByteArray profile_rawdata;
    // XXX: Hardcoded for icc type -- is that correct for us?
    if (QString::compare(profile_name, "icc") == 0) {
       
        profile_rawdata.resize(proflen);
        memcpy(profile_rawdata.data(), profile_data, proflen);

        cmsHPROFILE hProfile = cmsOpenProfileFromMem(profile_data, (DWORD)proflen);

        if (hProfile != (cmsHPROFILE) NULL) {
            profile = new KisProfile(hProfile, profile_rawdata);
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

    if(cs == 0)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    int channels = png_get_channels(png_ptr, info_ptr);
    // Read image data
    png_bytepp row_pointers = new png_bytep[height];
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    for(int y=0; y < height; y++)
    {
        row_pointers[y] = new png_byte[rowbytes];
    }
//     png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL );
//     png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr); // By using this function libpng will take care of freeing memory
    png_read_image(png_ptr, row_pointers);
    
    // Finish reading the file
    png_read_end(png_ptr, end_info);
    fclose(fp);
    
    // Creating the KisImageSP
    if( ! m_img) {
        m_img = new KisImage(m_doc->undoAdapter(), width, height, cs, "built image");
        Q_CHECK_PTR(m_img);
        if(profile)
        {
            m_img -> addAnnotation( new KisAnnotation(profile_name, "", profile_rawdata) );
        }
    }

    KisLayerSP layer = new KisLayer(m_img, m_img -> nextLayerName(), Q_UINT8_MAX);
    m_img->add(layer, 0);
    for (int y = 0; y < height; y++) {
        KisHLineIterator it = layer -> createHLineIterator(0, y, width, true);
        switch(color_type)
        {
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                if(color_nb_bits == 16)
                {
                    Q_UINT16 *src = reinterpret_cast<Q_UINT16 *>(row_pointers[y]);
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[0] = *(src++);
                        if(hasalpha) d[1] = *(src++);
                        else d[1] = Q_UINT16_MAX;
                        ++it;
                    }
                } else {
                    Q_UINT8 *src = row_pointers[y];
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[0] = *(src++);
                        if(hasalpha) d[1] = *(src++);
                        else d[1] = Q_UINT8_MAX;
                        ++it;
                    }
                }
                //FIXME:should be able to read 1 and 4 bits depth and scale them to 8 bits
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                if(color_nb_bits == 16)
                {
                    Q_UINT16 *src = reinterpret_cast<Q_UINT16 *>(row_pointers[y]);
                    while (!it.isDone()) {
                        Q_UINT16 *d = reinterpret_cast<Q_UINT16 *>(it.rawData());
                        d[2] = *(src++);
                        d[1] = *(src++);
                        d[0] = *(src++);
                        if(hasalpha) d[3] = *(src++);
                        else d[3] = Q_UINT16_MAX;
                        ++it;
                    }
                } else {
                    Q_UINT8 *src = row_pointers[y];
                    while (!it.isDone()) {
                        Q_UINT8 *d = it.rawData();
                        d[2] = *(src++);
                        d[1] = *(src++);
                        d[0] = *(src++);
                        if(hasalpha) d[3] = *(src++);
                        else d[3] = Q_UINT8_MAX;
                        ++it;
                    }
                }
                break;
            default:
                return KisImageBuilder_RESULT_UNSUPPORTED;
        }
    }
    
    // Freeing memory
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    
    for (int y = 0; y < height; y++) {
        delete row_pointers[y];
    }
    delete row_pointers;
    
    return KisImageBuilder_RESULT_OK;

}

KisImageBuilder_Result KisPNGConverter::buildImage(const KURL& uri)
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

    return KisImageBuilder_RESULT_OK;
}


KisImageSP KisPNGConverter::image()
{
    return m_img;
}


KisImageBuilder_Result KisPNGConverter::buildFile(const KURL& uri, KisLayerSP layer, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd)
{
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
    png_structp png_ptr =  png_create_write_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
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
            
    // FIXME: see if it a usefull option to offer : png_set_filter(png_ptr, 0, PNG_FILTER_NONE  | PNG_FILTER_VALUE_NONE | PNG_FILTER_SUB   | PNG_FILTER_VALUE_SUB  | PNG_FILTER_UP    | PNG_FILTER_VALUE_UP   | PNG_FILTER_AVE   | PNG_FILTER_VALUE_AVE  | PNG_FILTER_PAETH | PNG_FILTER_VALUE_PAETH| PNG_ALL_FILTERS);

    /* set the zlib compression level */
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION); // FIXME: allow configuration

    /* set other zlib parameters */
    png_set_compression_mem_level(png_ptr, 8);
    png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_method(png_ptr, 8);
    png_set_compression_buffer_size(png_ptr, 8192);
    
    int color_nb_bits = 8 * layer -> pixelSize() / layer -> nChannels();
    int color_type = getColorTypeforColorSpace(img->colorSpace());
    
    if(color_type == -1)
    {
        return KisImageBuilder_RESULT_UNSUPPORTED;
    }
    kdDebug() << " Nb bits : " << color_nb_bits << " color_type : " << color_type << endl;
    png_set_IHDR(png_ptr, info_ptr,
                 width,
                 height,
                 color_nb_bits,
                 color_type, PNG_INTERLACE_NONE, //FIXME: interlace needs to be configurable
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_ABSOLUTE); // FIXME: configurable, need to interstand what it is first
    
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
            char* name = new char[(*it)->type().length()];
            strcpy(name, (*it)->type().ascii());
            png_set_iCCP(png_ptr, info_ptr, name, PNG_COMPRESSION_TYPE_BASE, (char*)(*it)->annotation().data(), (*it) -> annotation() . size());
        }
        ++it;
    }

    // FIXME: offers comments in configuration dialog
    
    // Save the information to the file
    png_write_info(png_ptr, info_ptr);
    png_write_flush(png_ptr);
    // Write the PNG
//     png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    
    // Fill the data structure
    png_byte** row_pointers= new png_byte*[height];
    
    for (int y = 0; y < height; y++) {
        KisHLineIterator it = layer -> createHLineIterator(0, y, width, false);
        row_pointers[y] = new png_byte[width*layer -> pixelSize()];
        switch(color_type)
        {
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                if(color_nb_bits == 16)
                {
                    Q_UINT16 *dst = reinterpret_cast<Q_UINT16 *>(row_pointers[y]);
                    while (!it.isDone()) {
                        const Q_UINT16 *d = reinterpret_cast<const Q_UINT16 *>(it.rawData());
                        *(dst++) = d[0];
                        *(dst++) = d[1];
                        ++it;
                    }
                } else {
                    Q_UINT8 *dst = row_pointers[y];
                    while (!it.isDone()) {
                        const Q_UINT8 *d = it.rawData();
                        *(dst++) = d[0];
                        *(dst++) = d[1];
                        ++it;
                    }
                }
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                if(color_nb_bits == 16)
                {
                    Q_UINT16 *dst = reinterpret_cast<Q_UINT16 *>(row_pointers[y]);
                    while (!it.isDone()) {
                        const Q_UINT16 *d = reinterpret_cast<const Q_UINT16 *>(it.rawData());
                        *(dst++) = d[2];
                        *(dst++) = d[1];
                        *(dst++) = d[0];
                        *(dst++) = d[3];
                        ++it;
                    }
                } else {
                    Q_UINT8 *dst = row_pointers[y];
                    while (!it.isDone()) {
                        const Q_UINT8 *d = it.rawData();
                        *(dst++) = d[2];
                        *(dst++) = d[1];
                        *(dst++) = d[0];
                        *(dst++) = d[3];
                        ++it;
                    }
                }
                break;
            default:
                return KisImageBuilder_RESULT_UNSUPPORTED;
        }
    }
    
    png_write_image(png_ptr, row_pointers);


    
    png_write_flush(png_ptr);
    // Writting is over
    png_write_end(png_ptr, info_ptr);
    png_write_flush(png_ptr);

    // Free memory
    png_destroy_write_struct(&png_ptr, &info_ptr);
    for (int y = 0; y < height; y++) {
        delete row_pointers[y];
    }
    delete row_pointers;

    fclose(fp);
    
    return KisImageBuilder_RESULT_OK;
}


void KisPNGConverter::cancel()
{
    m_stop = true;
}

void KisPNGConverter::progress(png_structp png_ptr, png_uint_32 row_number, int pass)
{
    if(png_ptr == NULL || row_number > PNG_MAX_UINT || pass > 7) return;
//     setProgress(row_number);
}


#include "kis_png_converter.moc"

