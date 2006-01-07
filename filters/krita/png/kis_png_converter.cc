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
#include <kis_group_layer.h>

namespace {

    const Q_UINT8 PIXEL_BLUE = 0;
    const Q_UINT8 PIXEL_GREEN = 1;
    const Q_UINT8 PIXEL_RED = 2;
    const Q_UINT8 PIXEL_ALPHA = 3;

    
    int getColorTypeforColorSpace( KisColorSpace * cs , bool alpha)
    {
        if ( cs->id() == KisID("GRAYA") || cs->id() == KisID("GRAYA16") )
        {
            return alpha ? PNG_COLOR_TYPE_GRAY_ALPHA : PNG_COLOR_TYPE_GRAY;
        }
        if ( cs->id() == KisID("RGBA") || cs->id() == KisID("RGBA16") )
        {
            return alpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
        }

        kdDebug(41008) << "Cannot export images in " + cs->id().name() + " yet.\n";
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
        } else if(color_type ==  PNG_COLOR_TYPE_PALETTE) {
            return "RGBA"; // <-- we will convert the index image to RGBA
        }
        return "";
    }


    void fillText(png_text* p_text, char* key, QString& text)
    {
        p_text->compression = PNG_TEXT_COMPRESSION_zTXt;
        p_text->key = key;
        char* textc = new char[text.length()+1];
        strcpy(textc, text.ascii());
        p_text->text = textc;
        p_text->text_length = text.length()+1;
    }

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
    kdDebug(41008) << "Start decoding PNG File" << endl;
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
    
    KisProfile* profile = 0;
    if(png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &proflen))
    {
        QByteArray profile_rawdata;
        // XXX: Hardcoded for icc type -- is that correct for us?
        if (QString::compare(profile_name, "icc") == 0) {
            profile_rawdata.resize(proflen);
            memcpy(profile_rawdata.data(), profile_data, proflen);
            profile = new KisProfile(profile_rawdata);
            Q_CHECK_PTR(profile);
        }
    }

    // Retrieve a pointer to the colorspace
    KisColorSpace* cs;
    if (profile)
    {
        kdDebug(41008) << "image has embedded profile: " << profile -> productName() << "\n";
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(csName, profile);
    }
    else
        cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID(csName,""),"");

    if(cs == 0)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
    }
    
    png_text* text_ptr;
    int num_comments;
    png_get_text(png_ptr, info_ptr, &text_ptr, &num_comments);
    KoDocumentInfo * info = m_doc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author"));
    kdDebug(41008) << "There are " << num_comments << " comments in the text" << endl;
    for(int i = 0; i < num_comments; i++)
    {
        kdDebug(41008) << "key is " << text_ptr[i].key << " containing " << text_ptr[i].text << endl;
        if(QString::compare(text_ptr[i].key, "title") == 0)
        {
                aboutPage->setTitle(text_ptr[i].text);
        } else if(QString::compare(text_ptr[i].key, "abstract")  == 0)
        {
                aboutPage->setAbstract(text_ptr[i].text);
        } else if(QString::compare(text_ptr[i].key, "author") == 0)
        {
                authorPage->setFullName(text_ptr[i].text);
        }
    }
    
    // Read image data
    png_bytepp row_pointers = new png_bytep[height];
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    for(png_uint_32 y=0; y < height; y++)
    {
        row_pointers[y] = new png_byte[rowbytes];
    }
    
    // Read the palette if the file is indexed
    png_colorp palette ;
    int num_palette;
    if(color_type == PNG_COLOR_TYPE_PALETTE) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
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
            m_img -> addAnnotation( profile->annotation() );
        }
    }

    KisPaintLayer* layer = new KisPaintLayer(m_img, m_img -> nextLayerName(), Q_UINT8_MAX);
    m_img->addLayer(layer, m_img->rootLayer(), 0);
    for (png_uint_32 y = 0; y < height; y++) {
        KisHLineIterator it = layer -> paintDevice() -> createHLineIterator(0, y, width, true);
        switch(color_type)
        {
            case PNG_COLOR_TYPE_GRAY:
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
            case PNG_COLOR_TYPE_RGB:
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
            case PNG_COLOR_TYPE_PALETTE:
                switch(color_nb_bits)
                {
                    case 8:
                    {
                        Q_UINT8 *src = row_pointers[y];
                        while (!it.isDone()) {
                            Q_UINT8 *d = it.rawData();
                            png_color c = palette[*(src++)];
                            d[2] = c.red;
                            d[1] = c.green;
                            d[0] = c.blue;
                            d[3] = Q_UINT8_MAX;
                            ++it;
                        }
                        break;
                    }
                    default: // TODO:support for 1,2 and 4 bits
                        return KisImageBuilder_RESULT_UNSUPPORTED;
                }
                
                break;
            default:
                return KisImageBuilder_RESULT_UNSUPPORTED;
        }
    }

    // Freeing memory
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    
    for (png_uint_32 y = 0; y < height; y++) {
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

    return result;
}


KisImageSP KisPNGConverter::image()
{
    return m_img;
}


KisImageBuilder_Result KisPNGConverter::buildFile(const KURL& uri, KisPaintLayerSP layer, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, int compression, bool interlace, bool alpha)
{
    kdDebug(41008) << "Start writing PNG File" << endl;
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
    
    int interlacetype = interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
    
    png_set_IHDR(png_ptr, info_ptr,
                 width,
                 height,
                 color_nb_bits,
                 color_type, interlacetype,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_ABSOLUTE);
    
    // Save annotation
    vKisAnnotationSP_it it = annotationsStart;
    while(it != annotationsEnd) {
        if (!(*it) || (*it) -> type() == QString()) {
            kdDebug(41008) << "Warning: empty annotation" << endl;
            ++it;
            continue;
        }

        kdDebug(41008) << "Trying to store annotation of type " << (*it) -> type() << " of size " << (*it) -> annotation() . size() << endl;

        if ((*it) -> type().startsWith("krita_attribute:")) { // Attribute
            // FIXME: it should be possible to save krita_attributes in the "CHUNKs"
            kdDebug(41008) << "can't save this annotation : " << (*it) -> type() << endl;
        } else { // Profile
            char* name = new char[(*it)->type().length()+1];
            strcpy(name, (*it)->type().ascii());
            png_set_iCCP(png_ptr, info_ptr, name, PNG_COMPRESSION_TYPE_BASE, (char*)(*it)->annotation().data(), (*it) -> annotation() . size());
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
    // Write the PNG
//     png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    
    // Fill the data structure
    png_byte** row_pointers= new png_byte*[height];
    
    for (int y = 0; y < height; y++) {
        KisHLineIterator it = layer->paintDevice()->createHLineIterator(0, y, width, false);
        row_pointers[y] = new png_byte[width*layer->paintDevice()->pixelSize()];
        switch(color_type)
        {
            case PNG_COLOR_TYPE_GRAY:
            case PNG_COLOR_TYPE_GRAY_ALPHA:
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
            case PNG_COLOR_TYPE_RGB:
            case PNG_COLOR_TYPE_RGB_ALPHA:
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
        delete[] row_pointers[y];
    }
    delete[] row_pointers;

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

