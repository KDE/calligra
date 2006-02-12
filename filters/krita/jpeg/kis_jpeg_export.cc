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

#include "kis_jpeg_export.h"

#include <qcheckbox.h>
#include <qslider.h>

#include <kapplication.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <kis_doc.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_progress_display_interface.h>

#include "kis_jpeg_converter.h"
#include "kis_wdg_options_jpeg.h"

typedef KGenericFactory<KisJPEGExport, KoFilter> KisJPEGExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritajpegexport, KisJPEGExportFactory("kofficefilters"))

KisJPEGExport::KisJPEGExport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

KisJPEGExport::~KisJPEGExport()
{
}

KoFilter::ConversionStatus KisJPEGExport::convert(const QCString& from, const QCString& to)
{
    kdDebug(41008) << "JPEG export! From: " << from << ", To: " << to << "\n";
    
    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    
    KDialogBase* kdb = new KDialogBase(0, "", false, i18n("JPEG Export Options"), KDialogBase::Ok | KDialogBase::Cancel);
 
    KisWdgOptionsJPEG* wdg = new KisWdgOptionsJPEG(kdb);
    kdb->setMainWidget(wdg);
    kapp->restoreOverrideCursor();
    if(kdb->exec() == QDialog::Rejected)
    {
        return KoFilter::OK; // FIXME Cancel doesn't exist :(
    }
    KisJPEGOptions options;
    options.progressive = wdg->progressive->isChecked();
    options.quality = wdg->qualityLevel->value();
    
    delete kdb;
    // XXX: Add dialog about flattening layers here

    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KURL url(filename);

    KisImageSP img = output->currentImage();
    Q_CHECK_PTR(img);

    KisJPEGConverter kpc(output, output->undoAdapter());

    KisPaintLayerSP l = new KisPaintLayer(img, "projection", OPACITY_OPAQUE, img->projection());

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    KisImageBuilder_Result res;
    if ( (res = kpc.buildFile(url, l, beginIt, endIt, options)) == KisImageBuilder_RESULT_OK) {
        kdDebug(41008) << "success !" << endl;
        return KoFilter::OK;
    }
    kdDebug(41008) << " Result = " << res << endl;
    return KoFilter::InternalError;
}

#include <kis_jpeg_export.moc>

