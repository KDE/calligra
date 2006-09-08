/*
 *  Copyright (c) 2005-2006 Cyrille Berger <cberger@cberger.net>
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

#include <KoFilterChain.h>

#include <kis_colorspace.h>
#include <kis_doc.h>
#include <kis_image.h>
#include <kis_iterators_pixel.h>
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
    kdDebug(41008) << "Png export! From: " << from << ", To: " << to << "\n";
    
    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    
    KDialogBase* kdb = new KDialogBase(0, "", false, i18n("PNG Export Options"), KDialogBase::Ok | KDialogBase::Cancel);
    
    KisImageSP img = output->currentImage();
    KisPaintDeviceSP pd = new KisPaintDevice(*img->projection());
    KisPaintLayerSP l = new KisPaintLayer(img, "projection", OPACITY_OPAQUE, pd);
 
    KisRectIteratorPixel it = l->paintDevice()->createRectIterator(0,0, img->width(), img->height(), false);
    KisColorSpace* cs = l->paintDevice()->colorSpace();
    bool isThereAlpha = false;
    while( !it.isDone() )
    {
        if(cs->getAlpha( it.rawData() ) != 255)
        {
            isThereAlpha = true;
            break;
        }
        ++it;
    }
    
    KisWdgOptionsPNG* wdg = new KisWdgOptionsPNG(kdb);
    wdg->alpha->setChecked(isThereAlpha);
    wdg->alpha->setEnabled(isThereAlpha);
    kdb->setMainWidget(wdg);
    kapp->restoreOverrideCursor();
    if(kdb->exec() == QDialog::Rejected)
    {
        return KoFilter::OK; // FIXME Cancel doesn't exist :(
    }

    bool alpha = wdg->alpha->isChecked();
    bool interlace = wdg->interlacing->isChecked();
    int compression = wdg->compressionLevel->value();
    
    delete kdb;


    KURL url;
    url.setPath(filename);

    KisPNGConverter kpc(output, output->undoAdapter());

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    KisImageBuilder_Result res;

    
    if ( (res = kpc.buildFile(url, l, beginIt, endIt, compression, interlace, alpha)) == KisImageBuilder_RESULT_OK) {
        kdDebug(41008) << "success !" << endl;
        return KoFilter::OK;
    }
    kdDebug(41008) << " Result = " << res << endl;
    return KoFilter::InternalError;
}

#include <kis_png_export.moc>

