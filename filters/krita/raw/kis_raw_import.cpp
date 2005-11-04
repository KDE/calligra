/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
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

#include <qstring.h>
#include <qfile.h>

#include <kgenericfactory.h>
#include <kdialogbase.h>

#include <koDocument.h>
#include <koFilterChain.h>

#include "kis_types.h"
#include "kis_raw_import.h"
#include "kis_doc.h"
#include "kis_image.h"
#include "kis_meta_registry.h"
#include "kis_layer.h"
#include "kis_annotation.h"
#include "kis_colorspace_factory_registry.h"
#include "kis_iterators_pixel.h"
#include "kis_abstract_colorspace.h"
#include "kis_paint_device.h"
#include "wdgrawimport.h"

typedef KGenericFactory<KisRawImport, KoFilter> KisRawImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkrita_raw_import, KisRawImportFactory("kofficefilters"))

KisRawImport::KisRawImport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

KisRawImport::~KisRawImport()
{
}

KoFilter::ConversionStatus KisRawImport::convert(const QCString& from, const QCString& to)
{
    if (from != "image/x-raw" || to != "application/x-krita") {
        return KoFilter::NotImplemented;
    }

    kdDebug() << "\n\n\nKrita importing from Raw\n";

    KisDoc * doc = dynamic_cast<KisDoc*>(m_chain -> outputDocument());
    if (!doc) {
        return KoFilter::CreationError;
    }

    doc -> prepareForImport();

    QString filename = m_chain -> inputFile();

    if (filename.isEmpty()) {
        return KoFilter::FileNotFound;
    }

    // Show dialog
    KDialogBase * dialog = new KDialogBase();
    dialog->setMainWidget(new WdgRawImport(dialog));
    dialog->exec();
    delete dialog;


    // Prepare image
    KisColorSpace *cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID("", ""),"" );

    if (cs == 0) {
        return KoFilter::InternalError;
    }

    doc -> undoAdapter() -> setUndo(false);

    int imageWidth, imageHeight = 0;
    QString imageName = "";
    KisImageSP image = new KisImage(doc->undoAdapter(), imageWidth, imageHeight, cs, imageName);

    if (image == 0) {
        return KoFilter::CreationError;
    }

    KisLayerSP layer = image -> layerAdd(image -> nextLayerName(), OPACITY_OPAQUE);

    if (layer == 0) {
        return KoFilter::CreationError;
    }

    doc -> setCurrentImage(image);
    doc -> undoAdapter() -> setUndo(true);
    doc -> setModified(false);

    return KoFilter::OK;
}

#include "kis_raw_import.moc"

