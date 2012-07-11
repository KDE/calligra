/*
 *  Copyright (c) 2009 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "psd_saver.h"

#include <kapplication.h>

#include <kio/netaccess.h>
#include <kio/deletejob.h>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>
#include <KoColorProfile.h>
#include <KoCompositeOp.h>

#include <kis_annotation.h>
#include <kis_types.h>
#include <kis_paint_layer.h>
#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_group_layer.h>
#include <kis_paint_device.h>
#include <kis_transaction.h>

#include "psd.h"
#include "psd_header.h"
#include "psd_colormode_block.h"
#include "psd_utils.h"
#include "psd_resource_section.h"
#include "psd_layer_section.h"
#include "psd_resource_block.h"
#include "psd_image_data.h"



QPair<PSDColorMode, quint16> colormodelid_to_psd_colormode(const QString &colorSpaceId, const QString &colorDepthId)
{
    PSDColorMode colorMode = UNKNOWN;
    if (colorSpaceId == RGBAColorModelID.id()) {
        colorMode = RGB;
    }
    else if (colorSpaceId == CMYKAColorModelID.id()) {
        colorMode = CMYK;
    }
    else if (colorSpaceId == GrayAColorModelID.id()) {
        colorMode = Grayscale;
    }
    else if (colorSpaceId == LABAColorModelID.id()) {
        colorMode = Lab;
    }

    quint16 depth = 0;

    if (colorDepthId ==  Integer8BitsColorDepthID.id()) {
        depth = 8;
    }
    else if (colorDepthId == Integer16BitsColorDepthID.id()) {
        depth = 16;
    }
    else if (colorDepthId == Float32BitsColorDepthID.id()) {
        depth = 32;
    }
    else if (colorDepthId == Float32BitsColorDepthID.id()) {
        depth = 32;
    }

    return QPair<PSDColorMode, quint16>(colorMode, depth);
}



PSDSaver::PSDSaver(KisDoc2 *doc)
{
    m_doc = doc;
    m_job = 0;
    m_stop = false;
}

PSDSaver::~PSDSaver()
{
}

KisImageWSP PSDSaver::image()
{
    return m_image;
}


KisImageBuilder_Result PSDSaver::buildFile(const KUrl& uri, KisPaintLayerSP layer)
{
    if (!layer)
        return KisImageBuilder_RESULT_INVALID_ARG;

    KisImageWSP image = layer->image();
    if (!image)
        return KisImageBuilder_RESULT_EMPTY;

    if (uri.isEmpty())
        return KisImageBuilder_RESULT_NO_URI;

    if (!uri.isLocalFile())
        return KisImageBuilder_RESULT_NOT_LOCAL;

    // Open file for writing
    QFile f(uri.toLocalFile());
    f.open(QIODevice::WriteOnly);

    PSDHeader header;
    header.version = 2;
    header.nChannels = image->colorSpace()->channelCount();
    header.width = image->width();
    header.height = image->height();

    QPair<PSDColorMode, quint16> colordef = colormodelid_to_psd_colormode(image->colorSpace()->colorModelId().id(),
                                                                          image->colorSpace()->colorDepthId().id());
    header.colormode = colordef.first;
    header.channelDepth = colordef.second;

    if (!header.write(&f)) return KisImageBuilder_RESULT_FAILURE;

    f.close();

    return KisImageBuilder_RESULT_OK;
}


void PSDSaver::cancel()
{
    m_stop = true;
}

#include "psd_saver.moc"

