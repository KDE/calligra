/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <magick/api.h>

#include <QFile>
#include <qfileinfo.h>
#include <QString>

#include <kdeversion.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include <QColor>

#include "kis_types.h"
#include "kis_global.h"
#include "kis_doc.h"
#include "kis_image.h"
#include "kis_layer.h"
#include "kis_undo_adapter.h"
#include "kis_image_magick_converter.h"
#include "kis_meta_registry.h"
#include "kis_colorspace_factory_registry.h"
#include "kis_iterators_pixel.h"
#include "kis_colorspace.h"
#include "kis_profile.h"
#include "kis_annotation.h"
#include "kis_paint_layer.h"
#include "kis_group_layer.h"
#include "kis_paint_device.h"

#include "../../../config.h"

namespace {

    const quint8 PIXEL_BLUE = 0;
    const quint8 PIXEL_GREEN = 1;
    const quint8 PIXEL_RED = 2;
    const quint8 PIXEL_ALPHA = 3;

    static const quint8 PIXEL_CYAN = 0;
    static const quint8 PIXEL_MAGENTA = 1;
    static const quint8 PIXEL_YELLOW = 2;
    static const quint8 PIXEL_BLACK = 3;
    static const quint8 PIXEL_CMYK_ALPHA = 4;

    static const quint8 PIXEL_GRAY = 0;
    static const quint8 PIXEL_GRAY_ALPHA = 1;

    /**
     * Make this more flexible -- although... ImageMagick
     * isn't that flexible either.
     */
    QString getColorSpaceName(ColorspaceType type, unsigned long imageDepth = 8)
    {

        if (type == GRAYColorspace) {
            if (imageDepth == 8)
                return "GRAYA";
            else if ( imageDepth == 16 )
                return "GRAYA16" ;
        }
        else if (type == CMYKColorspace) {
            if (imageDepth == 8)
                return "CMYK";
            else if ( imageDepth == 16 ) {
                return "CMYK16";
            }
        }
        else if (type == LABColorspace) {
            kDebug(41008) << "Lab!\n";
            return "LABA";
        }
        else if (type == RGBColorspace || type == sRGBColorspace || type == TransparentColorspace) {
            if (imageDepth == 8)
                return "RGBA";
            else if (imageDepth == 16)
                return "RGBA16";
        }
        return "";

    }

    ColorspaceType getColorTypeforColorSpace( KisColorSpace * cs )
    {
        if ( cs->id() == KisID("GRAYA") || cs->id() == KisID("GRAYA16") ) return GRAYColorspace;
        if ( cs->id() == KisID("RGBA") || cs->id() == KisID("RGBA16") ) return RGBColorspace;
        if ( cs->id() == KisID("CMYK") || cs->id() == KisID("CMYK16") ) return CMYKColorspace;
        if ( cs->id() == KisID("LABA") ) return LABColorspace;

        kDebug(41008) << "Cannot export images in " + cs->id().name() + " yet.\n";
        return RGBColorspace;

    }

    KisProfile * getProfileForProfileInfo(const Image * image)
    {
#ifndef HAVE_MAGICK6
        return 0;
#else

        if (image->profiles == NULL)
            return  0;

        const char *name;
        const StringInfo *profile;

        KisProfile * p = 0;

        ResetImageProfileIterator(image);
        for (name = GetNextImageProfile(image); name != (char *) NULL; )
        {
            profile = GetImageProfile(image, name);
            if (profile == (StringInfo *) NULL)
                continue;

            // XXX: Hardcoded for icc type -- is that correct for us?
            if (QString::compare(name, "icc") == 0) {
                QByteArray rawdata;
                rawdata.resize(profile->length);
                memcpy(rawdata.data(), profile->datum, profile->length);

                p = new KisProfile(rawdata);
                if (p == 0)
                    return 0;
            }
            name = GetNextImageProfile(image);
        }
        return p;
#endif
    }

    void setAnnotationsForImage(const Image * src, KisImageSP image)
    {
#ifndef HAVE_MAGICK6
        return;
#else
        if (src->profiles == NULL)
            return;

        const char *name = 0;
        const StringInfo *profile;
        KisAnnotation* annotation = 0;

        // Profiles and so
        ResetImageProfileIterator(src);
        while((name = GetNextImageProfile(src))) {
            profile = GetImageProfile(src, name);
            if (profile == (StringInfo *) NULL)
                continue;

            // XXX: icc will be written seperately?
            if (QString::compare(name, "icc") == 0)
                continue;

            QByteArray rawdata;
            rawdata.resize(profile->length);
            memcpy(rawdata.data(), profile->datum, profile->length);

            annotation = new KisAnnotation(QString(name), "", rawdata);
            Q_CHECK_PTR(annotation);

            image -> addAnnotation(KisAnnotationSP(annotation));
        }

        // Attributes, since we have no hint on if this is an attribute or a profile
        // annotation, we prefix it with 'krita_attribute:'. XXX This needs to be rethought!
        // The joys of imagemagick. From at version 6.2.1 (dfaure has 6.2.0 and confirms the
        // old way of doing things) they changed the src -> attributes
        // to void* and require us to use the iterator functions. So we #if around that, *sigh*
#if MagickLibVersion >= 0x621
        const ImageAttribute * attr;
        ResetImageAttributeIterator(src);
        while ( (attr = GetNextImageAttribute(src)) ) {
#else
            ImageAttribute * attr = src -> attributes;
            while (attr) {
#endif
                QByteArray rawdata;
                int len = strlen(attr -> value) + 1;
                rawdata.resize(len);
                memcpy(rawdata.data(), attr -> value, len);

                annotation = new KisAnnotation(
                    QString("krita_attribute:%1").arg(QString(attr -> key)), "", rawdata);
                Q_CHECK_PTR(annotation);

                image -> addAnnotation(KisAnnotationSP(annotation));
#if MagickLibVersion < 0x620
                attr = attr -> next;
#endif
            }

#endif
        }
    }

    void exportAnnotationsForImage(Image * dst, vKisAnnotationSP_it& it, vKisAnnotationSP_it& annotationsEnd)
    {
#ifndef HAVE_MAGICK6
        return;
#else
        while(it != annotationsEnd) {
            if (!(*it) || (*it) -> type() == QString()) {
                    kDebug(41008) << "Warning: empty annotation" << endl;
                    ++it;
                    continue;
            }

            kDebug(41008) << "Trying to store annotation of type " << (*it) -> type() << " of size " << (*it) -> annotation() . size() << endl;

            if ((*it) -> type().startsWith("krita_attribute:")) { // Attribute
                if (!SetImageAttribute(dst,
                                        (*it) -> type().mid(strlen("krita_attribute:")).ascii(),
                                        (*it) -> annotation() . data()) ) {
                        kDebug(41008) << "Storing of attribute " << (*it) -> type() << "failed!\n";
                    }
            } else { // Profile
                    if (!ProfileImage(dst, (*it) -> type().ascii(),
                                    (unsigned char*)(*it) -> annotation() . data(),
                                    (*it) -> annotation() . size(), MagickFalse)) {
                        kDebug(41008) << "Storing failed!" << endl;
                    }
            }
            ++it;
        }
#endif
    }


    void InitGlobalMagick()
    {
        static bool init = false;

        if (!init) {
            KApplication *app = KApplication::kApplication();

            InitializeMagick(*app -> argv());
            atexit(DestroyMagick);
            init = true;
        }
    }

    /*
     * ImageMagick progress monitor callback.  Unfortunately it doesn't support passing in some user
     * data which complicates things quite a bit.  The plan was to allow the user start multiple
     * import/scans if he/she so wished.  However, without passing user data it's not possible to tell
     * on which task we have made progress on.
     *
     * Additionally, ImageMagick is thread-safe, not re-entrant... i.e. IM does not relinquish held
     * locks when calling user defined callbacks, this means that the same thread going back into IM
     * would deadlock since it would try to acquire locks it already holds.
     */
#ifdef HAVE_MAGICK6
    MagickBooleanType monitor(const char *text, const ExtendedSignedIntegralType, const ExtendedUnsignedIntegralType, ExceptionInfo *)
    {
        KApplication *app = KApplication::kApplication();

        Q_ASSERT(app);

        if (app -> hasPendingEvents())
            app -> processEvents();

        printf("%s\n", text);
        return MagickTrue;
    }
#else
    unsigned int monitor(const char *text, const ExtendedSignedIntegralType, const ExtendedUnsignedIntegralType, ExceptionInfo *)
    {
        KApplication *app = KApplication::kApplication();

        Q_ASSERT(app);

        if (app -> hasPendingEvents())
            app -> processEvents();

        printf("%s\n", text);
        return true;
    }
#endif



KisImageMagickConverter::KisImageMagickConverter(KisDoc *doc, KisUndoAdapter *adapter)
{
    InitGlobalMagick();
    init(doc, adapter);
    SetMonitorHandler(monitor);
    m_stop = false;
}

KisImageMagickConverter::~KisImageMagickConverter()
{
}

KisImageBuilder_Result KisImageMagickConverter::decode(const KUrl& uri, bool isBlob)
{
    Image *image;
    Image *images;
    ExceptionInfo ei;
    ImageInfo *ii;

    if (m_stop) {
        m_img = 0;
        return KisImageBuilder_RESULT_INTR;
    }

    GetExceptionInfo(&ei);
    ii = CloneImageInfo(0);

    if (isBlob) {

        // TODO : Test.  Does BlobToImage even work?
        Q_ASSERT(uri.isEmpty());
        images = BlobToImage(ii, &m_data[0], m_data.size(), &ei);
    } else {

        qstrncpy(ii -> filename, QFile::encodeName(uri.path()), MaxTextExtent - 1);

        if (ii -> filename[MaxTextExtent - 1]) {
            emit notifyProgressError();
            return KisImageBuilder_RESULT_PATH;
        }

        images = ReadImage(ii, &ei);

    }

    if (ei.severity != UndefinedException)
        CatchException(&ei);

    if (images == 0) {
        DestroyImageInfo(ii);
        DestroyExceptionInfo(&ei);
        emit notifyProgressError();
        return KisImageBuilder_RESULT_FAILURE;
    }

    emit notifyProgressStage(i18n("Importing..."), 0);

    m_img = 0;

    while ((image = RemoveFirstImageFromList(&images))) {
        ViewInfo *vi = OpenCacheView(image);

        // Determine image depth -- for now, all channels of an imported image are of the same depth
        unsigned long imageDepth = image->depth;
        kDebug(41008) << "Image depth: " << imageDepth << "\n";

        QString csName;
        KisColorSpace * cs = 0;
        ColorspaceType colorspaceType;

        // Determine image type -- rgb, grayscale or cmyk
        if (GetImageType(image, &ei) == GrayscaleType || GetImageType(image, &ei) == GrayscaleMatteType) {
            if (imageDepth == 8)
                csName = "GRAYA";
            else if ( imageDepth == 16 )
                csName = "GRAYA16" ;
            colorspaceType = GRAYColorspace;
        }
        else {
            colorspaceType = image->colorspace;
            csName = getColorSpaceName(image -> colorspace, imageDepth);
        }

        kDebug(41008) << "image has " << csName << " colorspace\n";
        
        KisProfile * profile = getProfileForProfileInfo(image);
        if (profile)
        {
            kDebug(41008) << "image has embedded profile: " << profile -> productName() << "\n";
            cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(csName, profile);
        }
        else
            cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID(csName,""),"");

        if (!cs) {
            kDebug(41008) << "Krita does not support colorspace " << image -> colorspace << "\n";
            CloseCacheView(vi);
            DestroyImage(image);
            DestroyExceptionInfo(&ei);
            DestroyImageList(images);
            DestroyImageInfo(ii);
            emit notifyProgressError();
            return KisImageBuilder_RESULT_UNSUPPORTED_COLORSPACE;
        }

        if( ! m_img) {
            m_img = new KisImage(m_doc->undoAdapter(), image -> columns, image -> rows, cs, "built image");
            Q_CHECK_PTR(m_img);
            m_img->blockSignals(true); // Don't send out signals while we're building the image
            
            // XXX I'm assuming seperate layers won't have other profile things like EXIF
            setAnnotationsForImage(image, m_img);
        }

        if (image -> columns && image -> rows) {

            // Opacity (set by the photoshop import filter)
            quint8 opacity = OPACITY_OPAQUE;
            const ImageAttribute * attr = GetImageAttribute(image, "[layer-opacity]");
            if (attr != 0) {
                opacity = quint8_MAX - Downscale(QString(attr->value).toInt());
            }

            KisPaintLayerSP layer = KisPaintLayerSP(0);

            attr = GetImageAttribute(image, "[layer-name]");
            if (attr != 0) {
                layer = new KisPaintLayer(m_img.data(), attr->value, opacity);
            }
            else {
                layer = new KisPaintLayer(m_img.data(), m_img -> nextLayerName(), opacity);
            }

            Q_ASSERT(layer);

            // Layerlocation  (set by the photoshop import filter)
            qint32 x_offset = 0;
            qint32 y_offset = 0;

            attr = GetImageAttribute(image, "[layer-xpos]");
            if (attr != 0) {
                x_offset = QString(attr->value).toInt();
            }

            attr = GetImageAttribute(image, "[layer-ypos]");
            if (attr != 0) {
                y_offset = QString(attr->value).toInt();
            }


            for (quint32 y = 0; y < image->rows; y ++)
            {
                const PixelPacket *pp = AcquireCacheView(vi, 0, y, image->columns, 1, &ei);

                if(!pp)
                {
                    CloseCacheView(vi);
                    DestroyImageList(images);
                    DestroyImageInfo(ii);
                    DestroyExceptionInfo(&ei);
                    emit notifyProgressError();
                    return KisImageBuilder_RESULT_FAILURE;
                }

                IndexPacket * indexes = GetCacheViewIndexes(vi);

                KisHLineIteratorPixel hiter = layer->paintDevice()->createHLineIterator(0, y, image->columns, true);

                if (colorspaceType== CMYKColorspace) {
                    if (imageDepth == 8) {
                        int x = 0;
                        while (!hiter.isDone())
                        {
                            quint8 *ptr= hiter.rawData();
                            *(ptr++) = Downscale(pp->red); // cyan
                            *(ptr++) = Downscale(pp->green); // magenta
                            *(ptr++) = Downscale(pp->blue); // yellow
                            *(ptr++) = Downscale(indexes[x]); // Black
// XXX: Warning! This ifdef messes up the paren matching big-time!
#ifdef HAVE_MAGICK6
                            if (image->matte != MagickFalse) {
#else
                            if (image->matte == true) {
#endif
                                *(ptr++) = OPACITY_OPAQUE - Downscale(pp->opacity);
                            }
                            else {
                                *(ptr++) = OPACITY_OPAQUE;
                            }
                            ++x;
                            pp++;
                            ++hiter;
                        }
                    }
                }
                else if (colorspaceType == LABColorspace) {
                    while(! hiter.isDone())
                    {
                        quint16 *ptr = reinterpret_cast<quint16 *>(hiter.rawData());
                        
                        *(ptr++) = ScaleQuantumToShort(pp->red);
                        *(ptr++) = ScaleQuantumToShort(pp->green);
                        *(ptr++) = ScaleQuantumToShort(pp->blue);
                        *(ptr++) = 65535/*OPACITY_OPAQUE*/ - ScaleQuantumToShort(pp->opacity);

                        pp++;
                        ++hiter;
                    }
                }
                else if (colorspaceType == RGBColorspace ||
                             colorspaceType == sRGBColorspace ||
                             colorspaceType == TransparentColorspace)
                    {
                        if (imageDepth == 8) {
                            while(! hiter.isDone())
                            {
                                quint8 *ptr= hiter.rawData();
                                // XXX: not colorstrategy and bitdepth independent
                                *(ptr++) = Downscale(pp->blue);
                                *(ptr++) = Downscale(pp->green);
                                *(ptr++) = Downscale(pp->red);
                                *(ptr++) = OPACITY_OPAQUE - Downscale(pp->opacity);

                                pp++;
                                ++hiter;
                            }
                        }
                        else if (imageDepth == 16) {
                            while(! hiter.isDone())
                            {
                                quint16 *ptr = reinterpret_cast<quint16 *>(hiter.rawData());
                                // XXX: not colorstrategy independent
                                *(ptr++) = ScaleQuantumToShort(pp->blue);
                                *(ptr++) = ScaleQuantumToShort(pp->green);
                                *(ptr++) = ScaleQuantumToShort(pp->red);
                                *(ptr++) = 65535/*OPACITY_OPAQUE*/ - ScaleQuantumToShort(pp->opacity);

                                pp++;
                                ++hiter;
                            }
                        }
                    }
                    else if ( colorspaceType == GRAYColorspace) {
                        if (imageDepth == 8) {
                            while(! hiter.isDone())
                            {
                                quint8 *ptr= hiter.rawData();
                                // XXX: not colorstrategy and bitdepth independent
                                *(ptr++) = Downscale(pp->blue);
                                *(ptr++) = OPACITY_OPAQUE - Downscale(pp->opacity);

                                pp++;
                                ++hiter;
                            }
                        }
                        else if (imageDepth == 16) {
                            while(! hiter.isDone())
                            {
                                quint16 *ptr = reinterpret_cast<quint16 *>(hiter.rawData());
                                // XXX: not colorstrategy independent
                                *(ptr++) = ScaleQuantumToShort(pp->blue);
                                *(ptr++) = 65535/*OPACITY_OPAQUE*/ - ScaleQuantumToShort(pp->opacity);

                                pp++;
                                ++hiter;
                            }
                        }
                    }

                    emit notifyProgress(y * 100 / image->rows);

                    if (m_stop) {
                        CloseCacheView(vi);
                        DestroyImage(image);
                        DestroyImageList(images);
                        DestroyImageInfo(ii);
                        DestroyExceptionInfo(&ei);
                        m_img = 0;
                        return KisImageBuilder_RESULT_INTR;
                    }
                }
                m_img->addLayer(KisLayerSP(layer.data()), m_img->rootLayer());
                layer->paintDevice()->move(x_offset, y_offset);
            }

            emit notifyProgressDone();
            CloseCacheView(vi);
            DestroyImage(image);
        }

        emit notifyProgressDone();
        DestroyImageList(images);
        DestroyImageInfo(ii);
        DestroyExceptionInfo(&ei);
        return KisImageBuilder_RESULT_OK;
    }

    KisImageBuilder_Result KisImageMagickConverter::buildImage(const KUrl& uri)
    {
        if (uri.isEmpty())
            return KisImageBuilder_RESULT_NO_URI;

        if (!KIO::NetAccess::exists(uri, false, qApp -> mainWidget())) {
            return KisImageBuilder_RESULT_NOT_EXIST;
        }

        KisImageBuilder_Result result = KisImageBuilder_RESULT_FAILURE;
        QString tmpFile;

        if (KIO::NetAccess::download(uri, tmpFile, qApp -> mainWidget())) {
            KUrl uriTF;
            uriTF.setPath( tmpFile );
            result = decode(uriTF, false);
            KIO::NetAccess::removeTempFile(tmpFile);
        }

        return result;
    }


    KisImageSP KisImageMagickConverter::image()
    {
        return m_img;
    }

    void KisImageMagickConverter::init(KisDoc *doc, KisUndoAdapter *adapter)
    {
        m_doc = doc;
        m_adapter = adapter;
        m_job = 0;
    }

    KisImageBuilder_Result KisImageMagickConverter::buildFile(const KUrl& uri, KisPaintLayerSP layer, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd)
    {
        Image *image;
        ExceptionInfo ei;
        ImageInfo *ii;

        if (!layer)
            return KisImageBuilder_RESULT_INVALID_ARG;

        KisImageSP img = KisImageSP(layer->image());
        if (!img)
            return KisImageBuilder_RESULT_EMPTY;

        if (uri.isEmpty())
            return KisImageBuilder_RESULT_NO_URI;

        if (!uri.isLocalFile())
            return KisImageBuilder_RESULT_NOT_LOCAL;


        quint32 layerBytesPerChannel = layer->paintDevice()->pixelSize() / layer->paintDevice()->nChannels();

        GetExceptionInfo(&ei);

        ii = CloneImageInfo(0);

        qstrncpy(ii -> filename, QFile::encodeName(uri.path()), MaxTextExtent - 1);

        if (ii -> filename[MaxTextExtent - 1]) {
            emit notifyProgressError();
            return KisImageBuilder_RESULT_PATH;
        }

        if (!img -> width() || !img -> height())
            return KisImageBuilder_RESULT_EMPTY;

        if (layerBytesPerChannel < 2) {
            ii->depth = 8;
        }
        else {
            ii->depth = 16;
        }

        ii->colorspace = getColorTypeforColorSpace(layer->paintDevice()->colorSpace());

        image = AllocateImage(ii);
        SetImageColorspace(image, ii->colorspace);
        image -> columns = img -> width();
        image -> rows = img -> height();

        kDebug(41008) << "Saving with colorspace " << image->colorspace << ", (" << layer->paintDevice()->colorSpace()->id().name() << ")\n";
        kDebug(41008) << "IM Image thinks it has depth: " << image->depth << "\n";

#ifdef HAVE_MAGICK6
        //    if ( layer-> hasAlpha() )
        image -> matte = MagickTrue;
        //    else
        //        image -> matte = MagickFalse;
#else
        //    image -> matte = layer -> hasAlpha();
        image -> matte = true;
#endif

        qint32 y, height, width;

        height = img -> height();
        width = img -> width();

        bool alpha = true;
        QString ext = QFileInfo(QFile::encodeName(uri.path())).extension(false).upper();
        if (ext == "BMP") {
            alpha = false;
            qstrncpy(ii->magick, "BMP2", MaxTextExtent - 1);
        }
        else if (ext == "RGB") {
            qstrncpy(ii->magick, "SGI", MaxTextExtent - 1);
        }

        for (y = 0; y < height; y++) {

            // Allocate pixels for this scanline
            PixelPacket * pp = SetImagePixels(image, 0, y, width, 1);

            if (!pp) {
                DestroyExceptionInfo(&ei);
                DestroyImage(image);
                emit notifyProgressError();
                return KisImageBuilder_RESULT_FAILURE;

            }

            KisHLineIterator it = layer->paintDevice()->createHLineIterator(0, y, width, false);
            if (alpha)
                SetImageType(image, TrueColorMatteType);
            else
                SetImageType(image,  TrueColorType);

            if (image->colorspace== CMYKColorspace) {

                IndexPacket * indexes = GetIndexes(image);
                int x = 0;
                if (layerBytesPerChannel == 2) {
                    while (!it.isDone()) {

                        const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
                        pp -> red = ScaleShortToQuantum(d[PIXEL_CYAN]);
                        pp -> green = ScaleShortToQuantum(d[PIXEL_MAGENTA]);
                        pp -> blue = ScaleShortToQuantum(d[PIXEL_YELLOW]);
                        if (alpha)
                            pp -> opacity = ScaleShortToQuantum(65535/*OPACITY_OPAQUE*/ - d[PIXEL_CMYK_ALPHA]);
                        indexes[x] = ScaleShortToQuantum(d[PIXEL_BLACK]);
                        x++;
                        pp++;
                        ++it;
                    }
                }
                else {
                    while (!it.isDone()) {

                        quint8 * d = it.rawData();
                        pp -> red = Upscale(d[PIXEL_CYAN]);
                        pp -> green = Upscale(d[PIXEL_MAGENTA]);
                        pp -> blue = Upscale(d[PIXEL_YELLOW]);
                        if (alpha)
                            pp -> opacity = Upscale(OPACITY_OPAQUE - d[PIXEL_CMYK_ALPHA]);

                        indexes[x]= Upscale(d[PIXEL_BLACK]);

                        x++;
                        pp++;
                        ++it;
                    }
                }
            }
            else if (image->colorspace== RGBColorspace ||
                     image->colorspace == sRGBColorspace ||
                     image->colorspace == TransparentColorspace)
            {
                if (layerBytesPerChannel == 2) {
                    while (!it.isDone()) {

                        const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
                        pp -> red = ScaleShortToQuantum(d[PIXEL_RED]);
                        pp -> green = ScaleShortToQuantum(d[PIXEL_GREEN]);
                        pp -> blue = ScaleShortToQuantum(d[PIXEL_BLUE]);
                        if (alpha)
                            pp -> opacity = ScaleShortToQuantum(65535/*OPACITY_OPAQUE*/ - d[PIXEL_ALPHA]);

                        pp++;
                        ++it;
                    }
                }
                else {
                    while (!it.isDone()) {

                        quint8 * d = it.rawData();
                        pp -> red = Upscale(d[PIXEL_RED]);
                        pp -> green = Upscale(d[PIXEL_GREEN]);
                        pp -> blue = Upscale(d[PIXEL_BLUE]);
                        if (alpha)
                            pp -> opacity = Upscale(OPACITY_OPAQUE - d[PIXEL_ALPHA]);

                        pp++;
                        ++it;
                    }
                }
            }
            else if (image->colorspace == GRAYColorspace)
            {
                SetImageType(image, GrayscaleMatteType);
                if (layerBytesPerChannel == 2) {
                    while (!it.isDone()) {

                        const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
                        pp -> red = ScaleShortToQuantum(d[PIXEL_GRAY]);
                        pp -> green = ScaleShortToQuantum(d[PIXEL_GRAY]);
                        pp -> blue = ScaleShortToQuantum(d[PIXEL_GRAY]);
                        if (alpha)
                            pp -> opacity = ScaleShortToQuantum(65535/*OPACITY_OPAQUE*/ - d[PIXEL_GRAY_ALPHA]);

                        pp++;
                        ++it;
                    }
                }
                else {
                    while (!it.isDone()) {
                        quint8 * d = it.rawData();
                        pp -> red = Upscale(d[PIXEL_GRAY]);
                        pp -> green = Upscale(d[PIXEL_GRAY]);
                        pp -> blue = Upscale(d[PIXEL_GRAY]);
                        if (alpha)
                            pp -> opacity = Upscale(OPACITY_OPAQUE - d[PIXEL_GRAY_ALPHA]);

                        pp++;
                        ++it;
                    }
                }
            }
            else {
                kDebug(41008) << "Unsupported image format\n";
                return KisImageBuilder_RESULT_INVALID_ARG;
            }

            emit notifyProgressStage(i18n("Saving..."), y * 100 / height);

#ifdef HAVE_MAGICK6
            if (SyncImagePixels(image) == MagickFalse)
                kDebug(41008) << "Syncing pixels failed\n";
#else
            if (!SyncImagePixels(image))
                kDebug(41008) << "Syncing pixels failed\n";
#endif
        }

        // set the annotations
        exportAnnotationsForImage(image, annotationsStart, annotationsEnd);

        // XXX: Write to a temp file, then have Krita use KIO to copy temp
        // image to remote location.

        WriteImage(ii, image);
        DestroyExceptionInfo(&ei);
        DestroyImage(image);
        emit notifyProgressDone();
        return KisImageBuilder_RESULT_OK;
    }

    void KisImageMagickConverter::ioData(KIO::Job *job, const QByteArray& data)
    {
        if (data.isNull() || data.isEmpty()) {
            emit notifyProgressStage(i18n("Loading..."), 0);
            return;
        }

        if (m_data.empty()) {
            Image *image;
            ImageInfo *ii;
            ExceptionInfo ei;

            ii = CloneImageInfo(0);
            GetExceptionInfo(&ei);
            image = PingBlob(ii, data.data(), data.size(), &ei);

            if (image == 0 || ei.severity == BlobError) {
                DestroyExceptionInfo(&ei);
                DestroyImageInfo(ii);
                job -> kill();
                emit notifyProgressError();
                return;
            }

            DestroyImage(image);
            DestroyExceptionInfo(&ei);
            DestroyImageInfo(ii);
            emit notifyProgressStage(i18n("Loading..."), 0);
        }

        Q_ASSERT(data.size() + m_data.size() <= m_size);
        memcpy(&m_data[m_data.size()], data.data(), data.count());
        m_data.resize(m_data.size() + data.count());
        emit notifyProgressStage(i18n("Loading..."), m_data.size() * 100 / m_size);

        if (m_stop)
            job -> kill();
    }

    void KisImageMagickConverter::ioResult(KIO::Job *job)
    {
        m_job = 0;

        if (job -> error())
            emit notifyProgressError();

        decode(KUrl(), true);
    }

    void KisImageMagickConverter::ioTotalSize(KIO::Job * /*job*/, KIO::filesize_t size)
    {
        m_size = size;
        m_data.reserve(size);
        emit notifyProgressStage(i18n("Loading..."), 0);
    }

    void KisImageMagickConverter::cancel()
    {
        m_stop = true;
    }

    /**
     * @name readFilters
     * @return Provide a list of file formats the application can read.
     */
    QString KisImageMagickConverter::readFilters()
    {
        QString s;
        QString all;
        QString name;
        QString description;
        unsigned long matches;

#ifdef HAVE_MAGICK6
#ifdef HAVE_OLD_GETMAGICKINFOLIST
        const MagickInfo **mi;
        mi = GetMagickInfoList("*", &matches);
#else // HAVE_OLD_GETMAGICKINFOLIST
        ExceptionInfo ei;
        GetExceptionInfo(&ei);
        const MagickInfo **mi;
        mi = GetMagickInfoList("*", &matches, &ei);
        DestroyExceptionInfo(&ei);
#endif // HAVE_OLD_GETMAGICKINFOLIST
#else // HAVE_MAGICK6
        const MagickInfo *mi;
        ExceptionInfo ei;
        GetExceptionInfo(&ei);
        mi = GetMagickInfo("*", &ei);
        DestroyExceptionInfo(&ei);
#endif // HAVE_MAGICK6

        if (!mi)
            return s;

#ifdef HAVE_MAGICK6
        for (unsigned long i = 0; i < matches; i++) {
            const MagickInfo *info = mi[i];
            if (info -> stealth)
                continue;

            if (info -> decoder) {
                name = info -> name;
                description = info -> description;
                kDebug(41008) << "Found import filter for: " << name << "\n";

                if (!description.isEmpty() && !description.contains('/')) {
                    all += "*." + name.lower() + " *." + name + " ";
                    s += "*." + name.lower() + " *." + name + "|";
                    s += i18n(description.utf8());
                    s += "\n";
                }
            }
        }
#else
        for (; mi; mi = reinterpret_cast<const MagickInfo*>(mi -> next)) {
            if (mi -> stealth)
                continue;
            if (mi -> decoder) {
                name = mi -> name;
                description = mi -> description;
                kDebug(41008) << "Found import filter for: " << name << "\n";

                if (!description.isEmpty() && !description.contains('/')) {
                    all += "*." + name.lower() + " *." + name + " ";
                    s += "*." + name.lower() + " *." + name + "|";
                    s += i18n(description.utf8());
                    s += "\n";
                }
            }
        }
#endif

        all += "|" + i18n("All Images");
        all += "\n";

        return all + s;
    }

    QString KisImageMagickConverter::writeFilters()
    {
        QString s;
        QString all;
        QString name;
        QString description;
        unsigned long matches;

#ifdef HAVE_MAGICK6
#ifdef HAVE_OLD_GETMAGICKINFOLIST
        const MagickInfo **mi;
        mi = GetMagickInfoList("*", &matches);
#else // HAVE_OLD_GETMAGICKINFOLIST
        ExceptionInfo ei;
        GetExceptionInfo(&ei);
        const MagickInfo **mi;
        mi = GetMagickInfoList("*", &matches, &ei);
        DestroyExceptionInfo(&ei);
#endif // HAVE_OLD_GETMAGICKINFOLIST
#else // HAVE_MAGICK6
        const MagickInfo *mi;
        ExceptionInfo ei;
        GetExceptionInfo(&ei);
        mi = GetMagickInfo("*", &ei);
        DestroyExceptionInfo(&ei);
#endif // HAVE_MAGICK6

        if (!mi) {
            kDebug(41008) << "Eek, no magick info!\n";
            return s;
        }

#ifdef HAVE_MAGICK6
        for (unsigned long i = 0; i < matches; i++) {
            const MagickInfo *info = mi[i];
            kDebug(41008) << "Found export filter for: " << info -> name << "\n";
            if (info -> stealth)
                continue;

            if (info -> encoder) {
                name = info -> name;

                description = info -> description;

                if (!description.isEmpty() && !description.contains('/')) {
                    all += "*." + name.lower() + " *." + name + " ";
                    s += "*." + name.lower() + " *." + name + "|";
                    s += i18n(description.utf8());
                    s += "\n";
                }
            }
        }
#else
        for (; mi; mi = reinterpret_cast<const MagickInfo*>(mi -> next)) {
            kDebug(41008) << "Found export filter for: " << mi -> name << "\n";
            if (mi -> stealth)
                continue;

            if (mi -> encoder) {
                name = mi -> name;

                description = mi -> description;

                if (!description.isEmpty() && !description.contains('/')) {
                    all += "*." + name.lower() + " *." + name + " ";
                    s += "*." + name.lower() + " *." + name + "|";
                    s += i18n(description.utf8());
                    s += "\n";
                }
            }
        }
#endif


        all += "|" + i18n("All Images");
        all += "\n";

        return all + s;
    }

#include "kis_image_magick_converter.moc"

