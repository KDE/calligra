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

#include <QCheckBox>
#include <QSlider>

#include <kapplication.h>
#include <kdialog.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_group_layer.h>
#include <kis_paint_layer.h>

#include <kis_node_visitor.h>

#include "kis_jpeg_converter.h"
#include "kis_wdg_options_jpeg.h"

#include "kis_meta_data_store.h"

class KisExternalLayer;

class KisExifInfoVisitor : public KisNodeVisitor
{
    public:
        using KisNodeVisitor::visit;
    
        KisExifInfoVisitor() :
            m_exifInfo(0),
            m_countPaintLayer(0)
        { }
    public:

        virtual bool visit(KisExternalLayer*)
        {
            return true;
        }

        virtual bool visit(KisPaintLayer* layer) {
            m_countPaintLayer++;
            if(not layer->metaData()->empty())
            {
                m_exifInfo = layer->metaData();
            }
            return true;
        }
        virtual bool visit(KisGroupLayer* layer)
        {
            dbgFile <<"Visiting on grouplayer" << layer->name() <<"";
            return visitAll( layer, true );
        }
        virtual bool visit(KisAdjustmentLayer* ) {  return true; }
    public:
        inline uint countPaintLayer() { return m_countPaintLayer; }
        inline KisMetaData::Store* exifInfo() {return m_exifInfo; }
    private:
        KisMetaData::Store* m_exifInfo;
        uint m_countPaintLayer;
};


typedef KGenericFactory<KisJPEGExport> KisJPEGExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritajpegexport, KisJPEGExportFactory("kofficefilters"))

KisJPEGExport::KisJPEGExport(QObject *parent, const QStringList&) : KoFilter(parent)
{
}

KisJPEGExport::~KisJPEGExport()
{
}

KoFilter::ConversionStatus KisJPEGExport::convert(const QByteArray& from, const QByteArray& to)
{
    dbgFile <<"JPEG export! From:" << from <<", To:" << to <<"";

    if (from != "application/x-krita")
        return KoFilter::NotImplemented;


    KDialog* kdb = new KDialog(0);
    kdb->setWindowTitle( i18n("JPEG Export Options") );
    kdb->setButtons( KDialog::Ok | KDialog::Cancel );

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

    KisDoc2 *output = dynamic_cast<KisDoc2*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();

    if (!output)
        return KoFilter::CreationError;


    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KUrl url;
    url.setPath(filename);

    KisImageSP img = output->image();
    Q_CHECK_PTR(img);

    KisJPEGConverter kpc(output, output->undoAdapter());

    KisPaintDeviceSP pd = new KisPaintDevice(*img->projection());
    KisPaintLayerSP l = new KisPaintLayer(img, "projection", OPACITY_OPAQUE, pd);

    vKisAnnotationSP_it beginIt = img->beginAnnotations();
    vKisAnnotationSP_it endIt = img->endAnnotations();
    KisImageBuilder_Result res;

    KisExifInfoVisitor eIV;
    eIV.visit( img->rootLayer().data() );

    KisMetaData::Store* eI = 0;
    if(eIV.countPaintLayer() == 1)
        eI = eIV.exifInfo();

    if ( (res = kpc.buildFile(url, l, beginIt, endIt, options, eI)) == KisImageBuilder_RESULT_OK) {
        dbgFile <<"success !";
        return KoFilter::OK;
    }
    dbgFile <<" Result =" << res;
    return KoFilter::InternalError;
}

#include <kis_jpeg_export.moc>

