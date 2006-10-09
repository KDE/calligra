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

#define UNSTABLE_POPPLER_QT4
#include "kis_pdf_import.h"

// poppler's headers
#include <poppler-qt4.h>

// Qt's headers
#include <qfile.h>
#include <qimage.h>
#include <qradiobutton.h>

// KDE's headers
#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kgenericfactory.h>
#include <knuminput.h>
#include <kpassworddialog.h>

#include <kio/netaccess.h>

// koffice's headers
#include <KoFilterChain.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

// krita's headers
#include <kis_doc.h>
#include <kis_group_layer.h>
#include <kis_image.h>
#include <kis_meta_registry.h>
#include <kis_paint_layer.h>

// plugins's headers
#include "kis_pdf_import_widget.h"

typedef KGenericFactory<KisPDFImport, KoFilter> PDFImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritapdfimport, PDFImportFactory("kofficefilters"))

KisPDFImport::KisPDFImport(QObject* parent, const QStringList&) : KoFilter(parent)
{
}

KisPDFImport::~KisPDFImport()
{
}

KisPDFImport::ConversionStatus KisPDFImport::convert(const QByteArray& , const QByteArray& )
{
    QString filename = m_chain -> inputFile();
    kDebug(41008) << "Importing using PDFImport!" << filename << endl;
    
    if (filename.isEmpty())
        return KoFilter::FileNotFound;
    
    
    KUrl url;
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
        kDebug(41008) << "Error when reading the PDF" << endl;
        return KoFilter::StorageCreationError;
    }


    while( pdoc->isLocked() )
    {
        QByteArray password;
        int result = KPasswordDialog::getPassword(0, password, i18n("A password is required to read that pdf"));
        if (result == KPasswordDialog::Accepted)
        {
            pdoc->unlock(password, password); //TODO: should probably ask for two passwords ? but that would be weird
        } else {
            kDebug(41008) << "Password canceled" << endl;
            return KoFilter::StorageCreationError;
        }
    }

    KDialog* kdb = new KDialog(0);
    kdb->setCaption( i18n("PDF Import Options") );
    kdb->setModal(false);
    
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
    KoColorSpace* cs = KisMetaRegistry::instance()->csRegistry()->colorSpace(KoID("RGBA"), "");
    int width = wdg->intWidth->value();
    int height = wdg->intHeight->value();
    KisImageSP img = new KisImage(doc->undoAdapter(), width, height, cs, "built image");
    img->blockSignals(true); // Don't send out signals while we're building the image
    
    // create a layer
    QList<int> pages = wdg->pages();
    for(QList<int>::const_iterator it = pages.begin(); it != pages.end(); ++it)
    {
        KisPaintLayer* layer = new KisPaintLayer(img.data(), QString(i18n("Page %1")).arg( QString::number(*it) + 1), quint8_MAX);
        layer->paintDevice()->convertFromQImage( pdoc->page( *it )->splashRenderToImage(wdg->intHorizontal->value(), wdg->intVertical->value() ), "");
        img->addLayer(layer, img->rootLayer(), 0);
    }
    
    img->blockSignals(false);
    doc -> setCurrentImage( img);
    
    KIO::NetAccess::removeTempFile(tmpFile);
    
    delete pdoc;
    delete kdb;
    return KoFilter::OK;
}

#include "kis_pdf_import.moc"
