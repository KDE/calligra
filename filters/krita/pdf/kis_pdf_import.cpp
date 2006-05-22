/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#include "kis_pdf_import.h"

// poppler's headers
#include <poppler-qt.h>

// Qt's headers
#include <qfile.h>
#include <qimage.h> // TODO that too
#include <qradiobutton.h>

// KDE's headers
#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>
#include <knuminput.h>
#include <kpassdlg.h>

#include <kio/netaccess.h>

// koffice's headers
#include <KoFilterChain.h>

// krita's headers
#include <kis_doc.h>
#include <kis_colorspace.h>
#include <kis_colorspace_factory_registry.h>
#include <kis_group_layer.h>
#include <kis_image.h>
#include <kis_meta_registry.h>
#include <kis_paint_layer.h>

// plugins's headers
#include "kis_pdf_import_widget.h"

typedef KGenericFactory<KisPDFImport, KoFilter> PDFImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritapdfimport, PDFImportFactory("kofficefilters"))

KisPDFImport::KisPDFImport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

KisPDFImport::~KisPDFImport()
{
}

KisPDFImport::ConversionStatus KisPDFImport::convert(const QCString& , const QCString& )
{
    QString filename = m_chain -> inputFile();
    kdDebug(41008) << "Importing using PDFImport!" << filename << endl;
    
    if (filename.isEmpty())
        return KoFilter::FileNotFound;
    
    
    KURL url;
    url.setPath(filename);

    if (!KIO::NetAccess::exists(url, false, qApp -> mainWidget())) {
        return KoFilter::FileNotFound;
    }

    // We're not set up to handle asynchronous loading at the moment.
    QString tmpFile;
    if (KIO::NetAccess::download(url, tmpFile, qApp -> mainWidget())) {
        url.setPath( tmpFile );
    }
    
    Poppler::Document* pdoc = Poppler::Document::load( QFile::encodeName(url.path() ) );
    

    if ( !pdoc)
    {
        kdDebug(41008) << "Error when reading the PDF" << endl;
        return KoFilter::StorageCreationError;
    }


    while( pdoc->isLocked() )
    {
        QCString password;
        int result = KPasswordDialog::getPassword(password, i18n("A password is required to read that pdf"));
        if (result == KPasswordDialog::Accepted)
        {
            pdoc->unlock(password);
        } else {
            kdDebug(41008) << "Password canceled" << endl;
            return KoFilter::StorageCreationError;
        }
    }

    KDialogBase* kdb = new KDialogBase(0, "", false, i18n("PDF Import Options"), KDialogBase::Ok | KDialogBase::Cancel);
    
    KisPDFImportWidget* wdg = new KisPDFImportWidget(pdoc, kdb);
    kdb->setMainWidget(wdg);
    kapp->restoreOverrideCursor();
    if(kdb->exec() == QDialog::Rejected)
    {
        delete pdoc;
        delete kdb;
        return KoFilter::StorageCreationError; // FIXME Cancel doesn't exist :(
    }
    
    // Init kis's doc
    KisDoc * doc = dynamic_cast<KisDoc*>(m_chain -> outputDocument());
    if (!doc)
    {
        delete pdoc;
        delete kdb;
        return KoFilter::CreationError;
    }

    doc -> prepareForImport();
    // Create the krita image
    KisColorSpace* cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID("RGBA"), "");
    int width = wdg->intWidth->value();
    int height = wdg->intHeight->value();
    KisImageSP img = new KisImage(doc->undoAdapter(), width, height, cs, "built image");
    img->blockSignals(true); // Don't send out signals while we're building the image
    
    // create a layer
    QValueList<int> pages = wdg->pages();
    for(QValueList<int>::const_iterator it = pages.begin(); it != pages.end(); ++it)
    {
        KisPaintLayer* layer = new KisPaintLayer(img, QString(i18n("Page %1")).arg( QString::number(*it) + 1), Q_UINT8_MAX);
        layer->paintDevice()->convertFromQImage( pdoc->getPage( *it )->renderToImage(wdg->intHorizontal->value(), wdg->intVertical->value() ), "");
        img->addLayer(layer, img->rootLayer(), 0);
    }
    
    img->blockSignals(false);
    doc -> setCurrentImage( img);
    
    KIO::NetAccess::removeTempFile(tmpFile);
    
    delete pdoc;
    delete kdb;
    return KoFilter::OK;
}

