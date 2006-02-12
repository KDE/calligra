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
#include <netinet/in.h>

#include <qstring.h>
#include <qfile.h>
#include <qimage.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qeventloop.h>

#include <knuminput.h>
#include <kgenericfactory.h>
#include <kdialogbase.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kprocess.h>

#include <KoDocument.h>
#include <KoFilterChain.h>

#include "imageviewer.h"
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
#include "kis_paint_layer.h"
#include "wdgrawimport.h"

typedef KGenericFactory<KisRawImport, KoFilter> KisRawImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkrita_raw_import, KisRawImportFactory("kofficefilters"))

KisRawImport::KisRawImport(KoFilter *, const char *, const QStringList&)
    : KoFilter()
    , m_data(0)
    , m_process(0)
{
    m_dialog = new KDialogBase();
    m_dialog->enableButtonApply(false);
    m_page = new WdgRawImport(m_dialog);
    m_dialog -> setMainWidget(m_page);

    connect(m_page->bnPreview, SIGNAL(clicked()), this, SLOT(slotUpdatePreview()));
    connect(m_page->grpColorSpace, SIGNAL(clicked( int )), this, SLOT(slotFillCmbProfiles()));
    connect(m_page->grpChannelDepth, SIGNAL(clicked( int )), this, SLOT(slotFillCmbProfiles()));

    KisConfig cfg;
    QString monitorProfileName = cfg.monitorProfile();
    m_monitorProfile = KisMetaRegistry::instance()->csRegistry()->getProfileByName(monitorProfileName);

    slotFillCmbProfiles();
}

KisRawImport::~KisRawImport()
{
    delete m_dialog;
    delete m_process;
}

KoFilter::ConversionStatus KisRawImport::convert(const QCString& from, const QCString& to)
{
    if (from != "image/x-raw" || to != "application/x-krita") {
        return KoFilter::NotImplemented;
    }

    kdDebug(41008) << "Krita importing from Raw\n";

    KisDoc * doc = dynamic_cast<KisDoc*>(m_chain -> outputDocument());
    if (!doc) {
        return KoFilter::CreationError;
    }

    doc -> prepareForImport();

    QString filename = m_chain -> inputFile();

    if (filename.isEmpty()) {
        return KoFilter::FileNotFound;
    }

    slotUpdatePreview();

    // Show dialog
    m_dialog->setCursor(Qt::ArrowCursor);
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    if (m_dialog->exec() == QDialog::Accepted) {

        QApplication::setOverrideCursor(Qt::waitCursor);
        doc -> undoAdapter() -> setUndo(false);

        getImageData(createArgumentList(false));

        KisImageSP image = 0;
        KisPaintLayerSP layer = 0;
        KisPaintDeviceSP device = 0;

        QApplication::restoreOverrideCursor();
        if (m_page->radio8->isChecked()) {
        // 8 bits

            QImage img;
            img.loadFromData(*m_data);

            KisColorSpace * cs = 0;
            if (m_page->radioGray->isChecked()) {
                cs  = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("GRAYA"), profile() );
            }
            else {
               cs  = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("RGBA"), profile() );
            }
            if (cs == 0) { kdDebug() << "No CS\n"; return KoFilter::InternalError; }

            image = new KisImage(doc->undoAdapter(), img.width(), img.height(), cs, filename);
            if (image == 0) return KoFilter::CreationError;

            layer = dynamic_cast<KisPaintLayer*>( image->newLayer(image -> nextLayerName(), OPACITY_OPAQUE).data() );
            if (layer == 0) return KoFilter::CreationError;

            device = layer->paintDevice();
            if (device == 0) return KoFilter::CreationError;

            device->convertFromQImage(img, "");

        } else {
        // 16 bits

            Q_UINT32 startOfImagedata;
            QSize sz = determineSize(&startOfImagedata);

            kdDebug(41008) << "Total bytes: " << m_data->size()
                    << "\n start of image data: " << startOfImagedata
                    << "\n bytes for pixels left: " << m_data->size() - startOfImagedata
                    << "\n total pixels: " << sz.width() * sz.height()
                    << "\n total pixel bytes: " << sz.width() * sz.height() * 6
                    << "\n total necessary bytes: " << (sz.width() * sz.height() * 6) + startOfImagedata
                    << "\n";


            char * data = m_data->data() + startOfImagedata;

            KisColorSpace * cs = 0;
            if (m_page->radioGray->isChecked()) {
                cs  = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("GRAYA16"), profile() );
            }
            else {
                cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("RGBA16"), profile() );
            }
            if (cs == 0) return KoFilter::InternalError;

            image = new KisImage( doc->undoAdapter(), sz.width(), sz.height(), cs, filename);
            if (image == 0)return KoFilter::CreationError;

            layer = dynamic_cast<KisPaintLayer*> (image->newLayer(image -> nextLayerName(), OPACITY_OPAQUE).data());
            if (layer == 0) return KoFilter::CreationError;

            device = layer->paintDevice();
            if (device == 0) return KoFilter::CreationError;

            // Copy the colordata to the pixels
            int pos = 0;

            for (int line = 0; line < sz.height(); ++line) {
                KisHLineIterator it = device->createHLineIterator(0, line, sz.width(), true);

                while (!it.isDone()) {
                    if (m_page->radioGray->isChecked()) {
                        Q_UINT16 d = (Q_INT16)*(data + pos);
                        d = ntohs(d);
                        memcpy(it.rawData(), &d, 2);
                        pos += 2;
                    }
                    else {
                        for (int c = 0; c < 6; c+=2) {
                            Q_UINT16 d = (Q_INT16)*(data + pos);
                            d = ntohs(d);
                            memcpy(it.rawData() + c, &d, 2);
                            pos += 2;
                        }
                    }
                    cs->setAlpha(it.rawData(), OPACITY_OPAQUE, 1);
                    ++it;
                }
            }
        }

        kdDebug() << "going to set image\n";
        doc -> setCurrentImage(image);
        doc -> undoAdapter() -> setUndo(true);
        doc -> setModified(false);
        kdDebug() << "everything ok\n";

        QApplication::restoreOverrideCursor();
        return KoFilter::OK;
    }

    QApplication::restoreOverrideCursor();
    return KoFilter::UserCancelled;
}




void KisRawImport::slotUpdatePreview()
{
    QApplication::setOverrideCursor(Qt::waitCursor);
    getImageData(createArgumentList(true));

    kdDebug(41008) << "Retrieved " << m_data->size() << " bytes of image data\n";

    if (m_data->isNull()) return;

    QImage img;

    if (m_page->radio8->isChecked()) {
        // 8 bits
        img.loadFromData(*m_data);

    } else {
        // 16 bits

        Q_UINT32 startOfImagedata;
        QSize sz = determineSize(&startOfImagedata);

        kdDebug(41008) << "Total bytes: " << m_data->size()
                  << "\n start of image data: " << startOfImagedata
                  << "\n bytes for pixels left: " << m_data->size() - startOfImagedata
                  << "\n total pixels: " << sz.width() * sz.height()
                  << "\n total pixel bytes: " << sz.width() * sz.height() * 6
                  << "\n total necessary bytes: " << (sz.width() * sz.height() * 6) + startOfImagedata
                  << "\n";

        char * data = m_data->data() + startOfImagedata;

        KisColorSpace * cs = 0;
        if (m_page->radioGray->isChecked()) {
            cs  = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("GRAYA16"), profile() );
        }
        else {
            cs = KisMetaRegistry::instance()->csRegistry()->getColorSpace( KisID("RGBA16"), profile() );
        }
        KisPaintDevice * dev = new KisPaintDevice(cs, "preview");
            // Copy the colordata to the pixels
        int pos = 0;

        for (int line = 0; line < sz.height(); ++line) {
            KisHLineIterator it = dev->createHLineIterator(0, line, sz.width(), true);

            while (!it.isDone()) {
                if (m_page->radioGray->isChecked()) {
                    Q_UINT16 d = (Q_INT16)*(data + pos);
                    d = ntohs(d);
                    memcpy(it.rawData(), &d, 2);
                    pos += 2;
                }
                else {
                    for (int c = 0; c < 6; c+=2) {
                        Q_UINT16 d = (Q_INT16)*(data + pos);
                        d = ntohs(d);
                        memcpy(it.rawData() + c, &d, 2);
                        pos += 2;
                    }
                }
                cs->setAlpha(it.rawData(), OPACITY_OPAQUE, 1);
                ++it;
            }
        }

        img = dev->convertToQImage(m_monitorProfile);
    }

    m_page->lblPreview->setImage(img);
    QApplication::restoreOverrideCursor();
}


void KisRawImport::getImageData( QStringList arguments )
{
    //    delete m_process;
    delete m_data;

    kdDebug(41008) << "getImageData " << arguments.join(" ") << "\n";
    KProcess process (this);
    m_data = new QByteArray(0);

    for (QStringList::iterator it = arguments.begin(); it != arguments.end(); ++it) {
        process << *it;
    }

    process.setUseShell(true);
    connect(&process, SIGNAL(receivedStdout(KProcess *, char *, int)), this, SLOT(slotReceivedStdout(KProcess *, char *, int)));
    connect(&process, SIGNAL(receivedStderr(KProcess *, char *, int)), this, SLOT(slotReceivedStderr(KProcess *, char *, int)));
    connect(&process, SIGNAL(processExited(KProcess *)), this, SLOT(slotProcessDone()));


    kdDebug(41008) << "Starting process\n";

    if (!process.start(KProcess::NotifyOnExit, KProcess::AllOutput)) {
        KMessageBox::error( 0, i18n("Cannot convert RAW files because the dcraw executable could not be started."));
    }
    while (process.isRunning()) {
        //kdDebug(41008) << "Waiting...\n";
        qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
        //process.wait(2);
    }

    if (process.normalExit()) {
        kdDebug(41008) << "Return value of process: " << process.exitStatus() << "\n";
    }
    else {
        kdDebug(41008) << "Process did not exit normally. Exit signal: " << process.exitSignal() << "\n";
    }

}


void KisRawImport::slotProcessDone()
{
    kdDebug(41008) << "process done!\n";
}

void KisRawImport::slotReceivedStdout(KProcess *, char *buffer, int buflen)
{
    //kdDebug(41008) << "stdout received " << buflen << " bytes on stdout.\n";
    //kdDebug(41008) << QString::fromAscii(buffer, buflen) << "\n";
    int oldSize = m_data->size();
    m_data->resize(oldSize + buflen, QGArray::SpeedOptim);
    memcpy(m_data->data() + oldSize, buffer, buflen);
}

void KisRawImport::slotReceivedStderr(KProcess *, char *buffer, int buflen)
{
    QByteArray b(buflen);
    memcpy(b.data(), buffer, buflen);
    kdDebug(41008) << QString(b) << "\n";
    //KMessageBox::error(0, i18n("dcraw says: ") + QString(b));
}


QStringList KisRawImport::createArgumentList(bool forPreview)
{
    QStringList args;

    args.append("dcraw"); // XXX: Create a kritadcraw so we can count on it being available

    //args.append("-v"); // Verbose

    args.append("-c"); // Write to stdout

    if (forPreview) {
        args.append("-h"); // Fast, half size image
    }

    if (m_page->radio8->isChecked()) {
        args.append("-2"); // 8 bits
    }
    else {
        args.append("-4"); // 16 bits
    }

    if (m_page->radioGray->isChecked()) {
        args.append("-d"); // Create grayscale image
    }

    if (m_page->chkCameraColors->isChecked()) {
        args.append("-m"); // Use camera raw colors instead of sRGB
    }

    if (m_page->radioAutomatic->isChecked()) {
        args.append("-a"); // Automatic white balancing
    }
    else {
        args.append("-w"); // Use camera white balance, if present
    }

    if (m_page->chkFourColorRGB->isChecked()) {
        args.append("-f"); // Interpolate RGB as four colors
    }

    if (!m_page->chkClip->isChecked()) {
        args.append("-n"); // Do not clip colors
    }

    args.append("-b " + QString::number(m_page->dblBrightness->value()));
    args.append("-k " + QString::number(m_page->dblBlackpoint->value()));

    KisProfile * pf  = profile();
    if (!pf->filename().isNull()) {
        // Use the user-set profile, if it's not an lcms internal
        // profile. This does not add the profile to the image, we
        // need to do that later.
        args.append("-p " + pf->filename());
    }

    // Don't forget the filename
    args.append(m_chain -> inputFile());

    return args;
}

QSize KisRawImport::determineSize(Q_UINT32 * startOfImageData)
{
    if (m_data->isNull() || m_data->size() < 2048) {
        * startOfImageData = 0;
        return QSize(0,0);
    }

    QString magick = QString::fromAscii(m_data->data(), 2);
    if (magick != "P6") {
        kdDebug(41008) << " Bad magick! " << magick << "\n";
        *startOfImageData = 0;
        return QSize(0,0);
    }

    // Find the third newline that marks the header end in a dcraw generated ppm.
    Q_UINT32 i = 0;
    Q_UINT32 counter = 0;

    while (true) {
        if (counter == 3) break;
        if (m_data->data()[i] == '\n') {
            counter++;
        }
        ++i;
    }

    QString size = QStringList::split("\n", QString::fromAscii(m_data->data(), i))[1];
    kdDebug(41008) << "Header: " << QString::fromAscii(m_data->data(), i) << "\n";
    QStringList sizelist = QStringList::split(" ", size);
    Q_INT32 w = sizelist[0].toInt();
    Q_INT32 h = sizelist[1].toInt();

    *startOfImageData = i;
    return QSize(w, h);

}

KisProfile * KisRawImport::profile()
{
    return KisMetaRegistry::instance()->csRegistry()->getProfileByName(m_page->cmbProfile->currentText());
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
