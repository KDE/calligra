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
#include <magickexport.h>
#include <kgenericfactory.h>
#include <koDocument.h>
#include <koFilterChain.h>

#include <kis_doc.h>
#include <kis_paint_layer.h>
#include <kis_image.h>
#include <kis_annotation.h>
#include <kis_types.h>
#include <kis_image_magick_converter.h>

typedef KGenericFactory<MagickExport, KoFilter> MagickExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritamagickexport, MagickExportFactory("kofficefilters"))

MagickExport::MagickExport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

MagickExport::~MagickExport()
{
}

KoFilter::ConversionStatus MagickExport::convert(const QCString& from, const QCString& to)
{
    kdDebug(41008) << "magick export! From: " << from << ", To: " << to << "\n";
    
    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    // XXX: Add dialog about flattening layers here

    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KURL url(filename);

    KisPaintLayerSP dst;

    KisImageSP img = new KisImage(*output->currentImage());
    Q_CHECK_PTR(img);

    // Don't store this information in the document's undo adapter
    bool undo = output->undoAdapter()->undo();
    output->undoAdapter()->setUndo(false);

    KisImageMagickConverter ib(output, output->undoAdapter());

    img->flatten();

    dst = (KisPaintLayer*)img->activeLayer().data();
    Q_ASSERT(dst);
    
    output->undoAdapter()->setUndo(undo);

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    if (ib.buildFile(url, dst, beginIt, endIt) == KisImageBuilder_RESULT_OK) {
        return KoFilter::OK;
    }
    return KoFilter::InternalError;
}

#include <magickexport.moc>

