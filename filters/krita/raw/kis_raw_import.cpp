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
#include <qprocess.h>
#include <qimage.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <kgenericfactory.h>
#include <kdialogbase.h>

#include <koDocument.h>
#include <koFilterChain.h>

#include "kis_config.h"
#include "kis_cmb_idlist.h"
#include "kis_types.h"
#include "kis_raw_import.h"
#include "kis_doc.h"
#include "kis_image.h"
#include "kis_meta_registry.h"
#include "kis_layer.h"
#include "kis_annotation.h"
#include "kis_profile.h"
#include "kis_colorspace_factory_registry.h"
#include "kis_iterators_pixel.h"
#include "kis_abstract_colorspace.h"
#include "kis_paint_device.h"
#include "wdgrawimport.h"

typedef KGenericFactory<KisRawImport, KoFilter> KisRawImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkrita_raw_import, KisRawImportFactory("kofficefilters"))

KisRawImport::KisRawImport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
    m_dialog = new KDialogBase();
    m_page = new WdgRawImport(m_dialog);
    m_dialog -> setMainWidget(m_page);
    QObject::connect(m_page->bnPreview, SIGNAL(clicked()), this, SLOT(slotUpdatePreview()));
    QObject::connect(m_page->grpColorSpace, SIGNAL(clicked( int )), this, SLOT(slotFillCmbProfiles()));
    QObject::connect(m_page->grpChannelDepth, SIGNAL(clicked( int )), this, SLOT(slotFillCmbProfiles()));
    
    KisConfig cfg;
    QString monitorProfileName = cfg.monitorProfile();
    m_monitorProfile = KisMetaRegistry::instance()->csRegistry()->getProfileByName(monitorProfileName);
    
    
}

KisRawImport::~KisRawImport()
{
    delete m_dialog;
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
    m_dialog->exec();

    // Determine settings

    // Prepare image
    KisColorSpace *cs;// = KisMetaRegistry::instance()->csRegistry()->getColorSpace(KisID("", ""),"" );

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

void KisRawImport::slotUpdatePreview()
{
    QByteArray data = getImageData(createArgumentList(true));
    QImage img;
    if (m_page->grpChannelDepth->selectedId() == 0) {
        // 8 bits
        img = QImage(data);
    } else {
        // 16 bits
        KisColorSpace * cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("RGBA16"), profile() );
        KisPaintDeviceImpl * dev = new KisPaintDeviceImpl(cs, "preview");
        QSize sz = determineSize(data);
        dev->writeBytes(data.data(), 0, 0, sz.width(), sz.height());
        img = dev->convertToQImage(m_monitorProfile);
    }

    m_page->lblPreview->setPixmap(img);
}


QByteArray KisRawImport::getImageData( QStringList arguments )
{
    return QByteArray();
}

QStringList KisRawImport::createArgumentList(bool forPreview)
{
    return QStringList();
}

QSize KisRawImport::determineSize( QByteArray data ) const
{
    return QSize(0, 0);
}

KisProfile * KisRawImport::profile()
{
    QValueVector<KisProfile *>  profileList = KisMetaRegistry::instance()->csRegistry()->profilesFor( getColorSpace() );
    KisID id = m_page -> cmbProfile -> currentItem();
    return profileList.at(index);
}

void KisRawImport::slotFillCmbProfiles()
{
    KisID s = getColorSpace();
    
    KisColorSpaceFactory * csf = KisMetaRegistry::instance()->csRegistry() -> get(s);
    m_page -> cmbProfile -> clear();
    QValueVector<KisProfile *>  profileList = KisMetaRegistry::instance()->csRegistry()->profilesFor( csf );
        QValueVector<KisProfile *> ::iterator it;
        for ( it = profileList.begin(); it != profileList.end(); ++it ) {
        m_page -> cmbProfile -> insertItem((*it) -> productName());
    }
}

KisID KisRawImport::getColorSpace()
{
    if (m_page->radioRGB->isChecked()) {
        if (m_page->radio16->isChecked()) {
            return KisID( "RGBA16" );
        }
    }
    else {
        if (m_page->radio16->isChecked()) {
            return KisID( "GRAYA16" );
        }
        else {
            return KisID( "GRAYA" );
        }
    }
    return KisID("RGBA");
}

#include "kis_raw_import.moc"

