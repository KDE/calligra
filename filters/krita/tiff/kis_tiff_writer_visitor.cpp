/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#include "kis_tiff_writer_visitor.h"

#include <kmessagebox.h>
#include <klocale.h>

#include <kis_annotation.h>
#include <KoColorSpace.h>
#include <kis_group_layer.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_types.h>

#include "kis_tiff_converter.h"

namespace {
    bool writeColorSpaceInformation( TIFF* image, KoColorSpace * cs, uint16& color_type )
    {
        if ( cs->id() == KoID("GRAYA") || cs->id() == KoID("GRAYA16") )
        {
            color_type = PHOTOMETRIC_MINISBLACK;
            return true;
        }
        if ( cs->id() == KoID("RGBA") || cs->id() == KoID("RGBA16") )
        {
            color_type = PHOTOMETRIC_RGB;
            return true;
        }
        if ( cs->id() == KoID("CMYK") || cs->id() == KoID("CMYKA16") )
        {
            color_type = PHOTOMETRIC_SEPARATED;
            TIFFSetField(image, TIFFTAG_INKSET, INKSET_CMYK);
            return true;
        }
        if ( cs->id() == KoID("LABA") )
        {
            color_type = PHOTOMETRIC_CIELAB;
            return true;
        }

        KMessageBox::error(0, i18n("Cannot export images in %1.\n").arg(cs->id().name()) ) ;
        return false;

    }
}

KisTIFFWriterVisitor::KisTIFFWriterVisitor(TIFF*img, KisTIFFOptions* options) : m_image(img), m_options(options)
{
}

KisTIFFWriterVisitor::~KisTIFFWriterVisitor()
{
}

bool KisTIFFWriterVisitor::saveAlpha() { return m_options->alpha; }

bool KisTIFFWriterVisitor::copyDataToStrips( KisHLineIterator it, tdata_t buff, uint8 depth, uint8 nbcolorssamples, quint8* poses)
{
    if(depth == 16)
    {
        quint16 *dst = reinterpret_cast<quint16 *>(buff);
        while (!it.isDone()) {
            const quint16 *d = reinterpret_cast<const quint16 *>(it.rawData());
            int i;
            for(i = 0; i < nbcolorssamples; i++)
            {
                *(dst++) = d[poses[i]];
            }
            if(saveAlpha()) *(dst++) = d[poses[i]];
            ++it;
        }
        return true;
    } else if(depth == 8) {
        quint8 *dst = reinterpret_cast<quint8 *>(buff);
        while (!it.isDone()) {
            const quint8 *d = it.rawData();
            int i;
            for(i = 0; i < nbcolorssamples; i++)
            {
                *(dst++) = d[poses[i]];
            }
            if(saveAlpha()) *(dst++) = d[poses[i]];
            ++it;
        }
        return true;
    }
    return false;
}


bool KisTIFFWriterVisitor::visit(KisPaintLayer *layer)
{
    kDebug(41008) << "visiting on paint layer " << layer->name() << "\n";
    KisPaintDeviceSP pd = layer->paintDevice();
    // Save depth
    int depth = 8 * pd->pixelSize() / pd->nChannels();
    TIFFSetField(image(), TIFFTAG_BITSPERSAMPLE, depth);
    // Save number of samples
    if(saveAlpha())
    {
        TIFFSetField(image(), TIFFTAG_SAMPLESPERPIXEL, pd->nChannels());
        uint16 sampleinfo[1] = { EXTRASAMPLE_UNASSALPHA };
        TIFFSetField(image(), TIFFTAG_EXTRASAMPLES, 1, sampleinfo);
    } else {
        TIFFSetField(image(), TIFFTAG_SAMPLESPERPIXEL, pd->nChannels() - 1);
        TIFFSetField(image(), TIFFTAG_EXTRASAMPLES, 0);
    }
    // Save colorspace information
    uint16 color_type;
    if(!writeColorSpaceInformation(image(), pd->colorSpace(), color_type))
    { // unsupported colorspace
        return false;
    }
    TIFFSetField(image(), TIFFTAG_PHOTOMETRIC, color_type);
    TIFFSetField(image(), TIFFTAG_IMAGEWIDTH, layer->image()->width());
    TIFFSetField(image(), TIFFTAG_IMAGELENGTH, layer->image()->height());

    // Set the compression options
    TIFFSetField(image(), TIFFTAG_COMPRESSION, m_options->compressionType);
    TIFFSetField(image(), TIFFTAG_FAXMODE, m_options->faxMode);
    TIFFSetField(image(), TIFFTAG_JPEGQUALITY, m_options->jpegQuality);
    TIFFSetField(image(), TIFFTAG_ZIPQUALITY, m_options->deflateCompress);
    TIFFSetField(image(), TIFFTAG_PIXARLOGQUALITY, m_options->pixarLogCompress);
    
    // Set the predictor
    TIFFSetField(image(), TIFFTAG_PREDICTOR, m_options->predictor);

    // Use contiguous configuration
    TIFFSetField(image(), TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    // Use 8 rows per strip
    TIFFSetField(image(), TIFFTAG_ROWSPERSTRIP, 8);

    // Save profile
    KoColorProfile* profile = pd->colorSpace()->getProfile();
    if(profile)
    {
        QByteArray ba = profile->rawData();
        TIFFSetField(image(), TIFFTAG_ICCPROFILE, ba.size(),ba.data());
    }
    tsize_t stripsize = TIFFStripSize(image());
    tdata_t buff = _TIFFmalloc(stripsize);
    qint32 height = layer->image()->height();
    qint32 width = layer->image()->width();
    bool r = true;
    for (int y = 0; y < height; y++) {
        KisHLineIterator it = layer->paintDevice()->createHLineIterator(0, y, width, false);
        switch(color_type)
        {
            case PHOTOMETRIC_MINISBLACK:
            {
                quint8 poses[]={ 0,1 };
                r = copyDataToStrips(it, buff, depth, 1, poses);
            }
                break;
            case PHOTOMETRIC_RGB:
            {
                quint8 poses[]={ 2, 1, 0, 3};
                r = copyDataToStrips(it, buff, depth, 3, poses);
            }
                break;
            case PHOTOMETRIC_SEPARATED:
            {
                quint8 poses[]={ 0, 1, 2, 3, 4 };
                r = copyDataToStrips(it, buff, depth, 4, poses);
            }
                break;
            case PHOTOMETRIC_CIELAB:
            {
                quint8 poses[]={ 0, 1, 2, 3 };
                r = copyDataToStrips(it, buff, depth, 3, poses);
            }
                break;
            return false;
        }
        if(!r) return false;
        TIFFWriteScanline(image(), buff, y, (tsample_t) -1);
    }
    _TIFFfree(buff);
    TIFFWriteDirectory(image());
    return true;
}
bool KisTIFFWriterVisitor::visit(KisGroupLayer *layer)
{
    kDebug(41008) << "Visiting on grouplayer " << layer->name() << "\n";
    KisLayerSP child = layer->firstChild();
    while (child) {
        child->accept(*this);
        child = child->nextSibling();
    }
    return true;
}

bool KisTIFFWriterVisitor::visit(KisPartLayer *)
{
    return true;
}
