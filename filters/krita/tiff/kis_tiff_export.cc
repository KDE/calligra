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

#include "kis_tiff_export.h"

#include <qcheckbox.h>
#include <qslider.h>
//Added by qt3to4:
#include <Q3CString>

#include <kapplication.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <kis_doc.h>
#include <kis_group_layer.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_progress_display_interface.h>

#include "kis_tiff_converter.h"
#include "kis_dlg_options_tiff.h"
#include "kis_wdg_options_tiff.h"

typedef KGenericFactory<KisTIFFExport, KoFilter> KisTIFFExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritatiffexport, KisTIFFExportFactory("kofficefilters"))

KisTIFFExport::KisTIFFExport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

KisTIFFExport::~KisTIFFExport()
{
}

KoFilter::ConversionStatus KisTIFFExport::convert(const Q3CString& from, const Q3CString& to)
{
    kDebug(41008) << "Tiff export! From: " << from << ", To: " << to << "\n";
    
    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    
    KisDlgOptionsTIFF* kdb = new KisDlgOptionsTIFF(0, "options dialog for tiff");
 
    if(kdb->exec() == QDialog::Rejected)
    {
        return KoFilter::OK; // FIXME Cancel doesn't exist :(
    }
    
    KisTIFFOptions options = kdb->options();

    delete kdb;

    KisDoc *output = dynamic_cast<KisDoc*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();
    
    if (!output)
        return KoFilter::CreationError;
    
    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KURL url;
    url.setPath(filename);

    KisImageSP img;
    
    if(options.flatten)
    {
        img = new KisImage(0, output->currentImage()->width(), output->currentImage()->height(), output->currentImage()->colorSpace(), "");
        KisPaintDeviceSP pd = new KisPaintDevice(*output->currentImage()->projection());
        KisPaintLayerSP l = new KisPaintLayer(img, "projection", OPACITY_OPAQUE, pd);
        img->addLayer(l.data(), img->rootLayer(), 0);
    } else {
        img = output->currentImage();
    }
    

    KisTIFFConverter ktc(output, output->undoAdapter());
/*    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();*/
    KisImageBuilder_Result res;
    if ( (res = ktc.buildFile(url, img, options)) == KisImageBuilder_RESULT_OK) {
        kDebug(41008) << "success !" << endl;
        return KoFilter::OK;
    }
    kDebug(41008) << " Result = " << res << endl;
    return KoFilter::InternalError;
}

#include <kis_tiff_export.moc>

