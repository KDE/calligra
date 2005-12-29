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

#include "kis_png_export.h"

#include <qcheckbox.h>
#include <qslider.h>

#include <kapplication.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>

#include <koFilterChain.h>

#include <kis_doc.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_progress_display_interface.h>

#include "kis_png_converter.h"
#include "kis_wdg_options_png.h"

typedef KGenericFactory<KisPNGExport, KoFilter> KisPNGExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritapngexport, KisPNGExportFactory("kofficefilters"))

KisPNGExport::KisPNGExport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

KisPNGExport::~KisPNGExport()
{
}

KoFilter::ConversionStatus KisPNGExport::convert(const QCString& from, const QCString& to)
{
    kdDebug() << "Png export! From: " << from << ", To: " << to << "\n";
    
    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    
    KDialogBase* kdb = new KDialogBase(0, "", false, i18n("PNG Export Options"), KDialogBase::Ok | KDialogBase::Cancel);
 
    KisWdgOptionsPNG* wdg = new KisWdgOptionsPNG(kdb);
    kdb->setMainWidget(wdg);
    kapp->restoreOverrideCursor();
    if(kdb->exec() == KDialogBase::Cancel)
    {
        return KoFilter::OK; // FIXME Cancel doesn't exist :(
    }

    bool alpha = wdg->alpha->isChecked();
    bool interlace = wdg->interlacing->isChecked();
    int compression = wdg->compressionLevel->value();
    
    delete kdb;
    // XXX: Add dialog about flattening layers here

    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KURL url(filename);

    KisLayerSP dst;

    KisImageSP img = new KisImage(*output->currentImage());
    Q_CHECK_PTR(img);

    // Don't store this information in the document's undo adapter
    bool undo = output->undoAdapter()->undo();
    output->undoAdapter()->setUndo(false);

    KisPNGConverter kpc(output, output->undoAdapter());

    img->flatten();

    dst = img->activeLayer();
    Q_ASSERT(dst);
    
    output->undoAdapter()->setUndo(undo);

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    KisImageBuilder_Result res;
    if ( (res = kpc.buildFile(url, (KisPaintLayer*)dst.data(), beginIt, endIt, compression, interlace, alpha)) == KisImageBuilder_RESULT_OK) {
        kdDebug() << "success !" << endl;
        return KoFilter::OK;
    }
    kdDebug() << " Result = " << res << endl;
    return KoFilter::InternalError;
}

#include <kis_png_export.moc>

