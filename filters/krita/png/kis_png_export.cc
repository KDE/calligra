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
#include <kdialog.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <kis_doc.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_progress_display_interface.h>

#include "kis_png_converter.h"

typedef KGenericFactory<KisPNGExport> KisPNGExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritapngexport, KisPNGExportFactory("kofficefilters"))

KisPNGExport::KisPNGExport(KoFilter *, const char *, const QStringList&) : KoFilter(parent)
{
}

KisPNGExport::~KisPNGExport()
{
}

KoFilter::ConversionStatus KisPNGExport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug(41008) << "Png export! From: " << from << ", To: " << to << "\n";
    
    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    
    KDialog* kdb = new KDialog(0, i18n("PNG Export Options"), KDialog::Ok | KDialog::Cancel);
 
    KisWdgOptionsPNG* wdg = new KisWdgOptionsPNG(kdb);
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

    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KUrl url;
    url.setPath(filename);

    KisImageSP img = output->currentImage();

    KisPNGConverter kpc(output, output->undoAdapter());

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    KisImageBuilder_Result res;

    KisPaintDeviceSP pd = KisPaintDeviceSP(new KisPaintDevice(*img->projection()));
    KisPaintLayerSP l = KisPaintLayerSP(new KisPaintLayer(img.data(), "projection", OPACITY_OPAQUE, pd));
    
    if ( (res = kpc.buildFile(url, l, beginIt, endIt, compression, interlace, alpha)) == KisImageBuilder_RESULT_OK) {
        kDebug(41008) << "success !" << endl;
        return KoFilter::OK;
    }
    kDebug(41008) << " Result = " << res << endl;
    return KoFilter::InternalError;
}

#include "kis_png_export.moc"

