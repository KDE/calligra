/*
 *  Copyright (c) 2005 Adrian Page <adrian@pagenet.plus.com>
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

#include <QString>
#include <QFile>
#include <Q3MemArray>

#include <kgenericfactory.h>
#include <KoDocument.h>
#include <KoFilterChain.h>

#include <half.h>
#include <ImfRgbaFile.h>
//#include <ImfStringAttribute.h>
//#include <ImfMatrixAttribute.h>
//#include <ImfArray.h>
//#include <drawImage.h>

#include <iostream>

#include "kis_types.h"
#include "kis_openexr_import.h"
#include "kis_doc.h"
#include "kis_image.h"
#include "kis_meta_registry.h"
#include "kis_layer.h"
#include "kis_paint_layer.h"
#include "kis_annotation.h"
#include "kis_colorspace_factory_registry.h"
#include "kis_iterators_pixel.h"
#include "kis_rgb_f32_colorspace.h"
#include "kis_rgb_f16half_colorspace.h"

using namespace std;
using namespace Imf;
using namespace Imath;

typedef KGenericFactory<KisOpenEXRImport> KisOpenEXRImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkrita_openexr_import, KisOpenEXRImportFactory("kofficefilters"))

KisOpenEXRImport::KisOpenEXRImport(QObject *parent, const QStringList&) : KoFilter(parent)
{
}

KisOpenEXRImport::~KisOpenEXRImport()
{
}

KoFilter::ConversionStatus KisOpenEXRImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "image/x-exr" || to != "application/x-krita") {
        return KoFilter::NotImplemented;
    }

    kDebug(41008) << "\n\n\nKrita importing from OpenEXR\n";

    KisDoc * doc = dynamic_cast<KisDoc*>(m_chain -> outputDocument());
    if (!doc) {
        return KoFilter::CreationError;
    }

    doc -> prepareForImport();

    QString filename = m_chain -> inputFile();

    if (filename.isEmpty()) {
        return KoFilter::FileNotFound;
    }

    RgbaInputFile file(QFile::encodeName(filename));
    Box2i dataWindow = file.dataWindow();
    Box2i displayWindow = file.displayWindow();

    kDebug(41008) << "Data window: " << QRect(dataWindow.min.x, dataWindow.min.y, dataWindow.max.x - dataWindow.min.x + 1, dataWindow.max.y - dataWindow.min.y + 1) << endl;
    kDebug(41008) << "Display window: " << QRect(displayWindow.min.x, displayWindow.min.y, displayWindow.max.x - displayWindow.min.x + 1, displayWindow.max.y - displayWindow.min.y + 1) << endl;

    int imageWidth = displayWindow.max.x - displayWindow.min.x + 1;
    int imageHeight = displayWindow.max.y - displayWindow.min.y + 1;

    QString imageName = "Imported from OpenEXR";

    int dataWidth  = dataWindow.max.x - dataWindow.min.x + 1;
    int dataHeight = dataWindow.max.y - dataWindow.min.y + 1;

    KisColorSpace *cs = static_cast<KisColorSpace *>((KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID("RGBAF16HALF", ""),"")));

    if (cs == 0) {
        return KoFilter::InternalError;
    }

    doc -> undoAdapter() -> setUndo(false);

    KisImageSP image = KisImageSP(new KisImage(doc->undoAdapter(), imageWidth, imageHeight, cs, imageName));

    if (!image) {
        return KoFilter::CreationError;
    }

    KisPaintLayerSP layer = KisPaintLayerSP(dynamic_cast<KisPaintLayer*>(image->newLayer(image -> nextLayerName(), OPACITY_OPAQUE).data()));

    if (!layer) {
        return KoFilter::CreationError;
    }

    Q3MemArray<Rgba> pixels(dataWidth);

    for (int y = 0; y < dataHeight; ++y) {

        file.setFrameBuffer(pixels.data() - dataWindow.min.x - (dataWindow.min.y + y) * dataWidth, 1, dataWidth);
        file.readPixels(dataWindow.min.y + y);

        KisHLineIterator it = layer->paintDevice()->createHLineIterator(dataWindow.min.x, dataWindow.min.y + y, dataWidth, true);
        Rgba *rgba = pixels.data();

        while (!it.isDone()) {

            // XXX: For now unmultiply the alpha, though compositing will be faster if we
            // keep it premultiplied.
            half unmultipliedRed = rgba -> r;
            half unmultipliedGreen = rgba -> g;
            half unmultipliedBlue = rgba -> b;

            if (rgba -> a >= HALF_EPSILON) {
                unmultipliedRed /= rgba -> a;
                unmultipliedGreen /= rgba -> a;
                unmultipliedBlue /= rgba -> a;
            }
            setPixel(it.rawData(), unmultipliedRed, unmultipliedGreen, unmultipliedBlue, rgba -> a);
            ++it;
            ++rgba;
        }
    }

    layer->setDirty();
    doc -> setCurrentImage(image);
    doc -> undoAdapter() -> setUndo(true);
    doc -> setModified(false);

    return KoFilter::OK;
}

void KisOpenEXRImport::setPixel(quint8 *dst, half red, half green, half blue, half alpha) const
{
    struct Pixel {
        half blue;
        half green;
        half red;
        half alpha;
    };
    Pixel *dstPixel = reinterpret_cast<Pixel *>(dst);

    dstPixel->red = red;
    dstPixel->green = green;
    dstPixel->blue = blue;
    dstPixel->alpha = alpha;
}

#include "kis_openexr_import.moc"

