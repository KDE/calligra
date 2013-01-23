/*
 * imageshare.cpp -- Part of Krita
 *
 * Copyright (c) 2012 Boudewijn Rempt (boud@valdyas.org)
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "imageshare.h"

#include <QUrl>
#include <QDesktopServices>

#include <klocale.h>
#include <kcomponentdata.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kpluginfactory.h>
#include <kstandardaction.h>
#include <kactioncollection.h>

#include <kis_debug.h>
#include <kis_image.h>
#include <kis_view2.h>
#include <kis_doc2.h>

#include "o2deviantart.h"
#include "dlg_login.h"
#include "submitdlg.h"
#include <KoUpdater.h>

K_PLUGIN_FACTORY(ImageShareFactory, registerPlugin<ImageShare>();)
K_EXPORT_PLUGIN(ImageShareFactory("krita"))

ImageShare::ImageShare(QObject *parent, const QVariantList &)
        : KParts::Plugin(parent)
        , m_view(0)
        , m_deviantArt(0)
        , m_stash(0)
        , m_submitDlg(0)
{
    if (parent->inherits("KisView2")) {
        setXMLFile(KStandardDirs::locate("data", "kritaplugins/imageshare.rc"), true);

        KAction *action  = new KAction(i18n("Share on Deviant Art..."), this);
        actionCollection()->addAction("imageshare", action);
        connect(action, SIGNAL(triggered()), this, SLOT(slotImageShare()));

        m_view = qobject_cast<KisView2*>(parent);
        m_submitDlg = new SubmitDlg(m_view);
        m_submitDlg->submitDlg()->txtTitle->setText(m_view->document()->url().fileName().split(".").first());
        // TODO change this once we have this information from dA... broken response is fun...
        m_submitDlg->submitDlg()->lblRemaining->setVisible(false);
        connect(m_submitDlg, SIGNAL(accepted()), SLOT(performUpload()));
    }
}

ImageShare::~ImageShare()
{
    m_view = 0;
}

QObject* ImageShare::stash()
{
    m_deviantArt = new O2DeviantART(this);
    m_deviantArt->setClientId("272");
    m_deviantArt->setClientSecret("a8464938f858f68661c4246347f09b62");
    connect(m_deviantArt, SIGNAL(openBrowser(QUrl)), SIGNAL(openBrowser(QUrl)));
    connect(m_deviantArt, SIGNAL(closeBrowser()), SIGNAL(closeBrowser()));
    connect(m_deviantArt, SIGNAL(linkingSucceeded()), SIGNAL(linkingSucceeded()));
    if (!m_deviantArt->linked()) {
        QTimer::singleShot(0, m_deviantArt, SLOT(link()));
    }
    m_stash = new Stash(m_deviantArt, this);
    return m_stash;
}

void ImageShare::slotImageShare()
{
    if(!m_submitDlg)
        return;
    m_deviantArt = new O2DeviantART(this);
    m_deviantArt->setClientId("272");
    m_deviantArt->setClientSecret("a8464938f858f68661c4246347f09b62");

    connect(m_deviantArt, SIGNAL(openBrowser(QUrl)), SLOT(slotOpenBrowser(QUrl)));
    connect(m_deviantArt, SIGNAL(closeBrowser()), SLOT(slotCloseBrowser()));
    connect(m_deviantArt, SIGNAL(linkingSucceeded()), SLOT(showSubmit()));

    qDebug() << "slotImageShare" << m_deviantArt->token() << m_deviantArt->linked();

    if (!m_deviantArt->linked()) {
        m_deviantArt->link();
    }
    else {
        showSubmit();
    }
}

void ImageShare::slotOpenBrowser(const QUrl &url)
{
    qDebug() << "openBrowser" << url << m_deviantArt->token();
    DlgLogin dlgLogin(m_deviantArt);
    dlgLogin.setLoginUrl(url);
    dlgLogin.exec();
}

void ImageShare::slotCloseBrowser()
{
    qDebug() << "close browser" << m_deviantArt->token();
}

void ImageShare::showSubmit()
{
    m_stash = new Stash(m_deviantArt, this);
    connect(m_stash, SIGNAL(callFinished(Stash::Call,bool)), SLOT(testCallCompleted(Stash::Call,bool)));
    connect(m_stash, SIGNAL(submissionsChanged()), SLOT(submissionsChanged()));
    connect(m_stash, SIGNAL(uploadProgress(int,qint64,qint64)), SLOT(uploadProgress(int,qint64,qint64)));
    connect(m_stash, SIGNAL(newSubmission(int,QString,int)), SLOT(newSubmission(int,QString,int)));
    // This will need doing... once deviantArt fixes the call
    //connect(m_stash, SIGNAL(availableSpaceChanged()), SLOT(availableSpaceChanged()));
    m_stash->testCall();
}

void ImageShare::testCallCompleted(Stash::Call, bool result)
{
    if(!result) {
        qDebug() << Q_FUNC_INFO << "if this happens, something failed and we'll need to start over...";
    }
    disconnect(m_stash, SIGNAL(callFinished(Stash::Call,bool)), this, SLOT(testCallCompleted(Stash::Call,bool)));
    QTimer::singleShot(100, m_stash, SLOT(delta()));
    m_submitDlg->open();
}

void ImageShare::performUpload()
{
    QString folderId;
    // First item is non-folder, so if it's that, don't assign a folder
    if(m_submitDlg->submitDlg()->folderList->currentIndex() != 0) {
        folderId = m_submitDlg->submitDlg()->folderList->itemData(m_submitDlg->submitDlg()->folderList->currentIndex()).toString();
    }
    m_progressUpdater = m_view->createProgressUpdater();
    m_progressUpdater->start(100, i18n("Uploading to Sta.sh"));
    m_progressSubtask = m_progressUpdater->startSubtask(1, i18n("Uploading to Sta.sh"));
    m_progressSubtask->setRange(0, 1);
    qApp->processEvents();
    connect(m_stash, SIGNAL(callFinished(Stash::Call,bool)), SLOT(submitCallCompleted(Stash::Call,bool)));
    m_stash->submit(m_view->image(), m_view->document()->url().fileName(), m_submitDlg->submitDlg()->txtTitle->text(), m_submitDlg->submitDlg()->txtComments->toPlainText(), m_submitDlg->submitDlg()->txtKeywords->text().split(","), folderId);
}

void ImageShare::submissionsChanged()
{
    m_submitDlg->submitDlg()->folderList->clear();
    m_submitDlg->submitDlg()->folderList->addItem(i18n("No folder"));
    foreach(const Submission& sub, m_stash->submissions()) {
        if(sub.isFolder) {
            m_submitDlg->submitDlg()->folderList->addItem(sub.title, sub.folderId);
        }
    }
}

void ImageShare::uploadProgress(int, qint64 bytesSent, qint64 bytesTotal)
{
    if(m_progressUpdater) {
        if(m_progressSubtask->max == 1) {
            m_progressSubtask->setRange(0, bytesTotal);
        }
        m_progressSubtask->setValue(bytesSent);
    }
}

void ImageShare::submitCallCompleted(Stash::Call, bool result)
{
    qDebug() << Q_FUNC_INFO;
    m_progressUpdater->deleteLater();
}

void ImageShare::availableSpaceChanged()
{
    if(m_submitDlg) {
        m_submitDlg->submitDlg()->lblRemaining->setText(i18n("Remaining space: %1", m_stash->availableSpace()));
    }
}

void ImageShare::newSubmission(int stashId, QString folder, int folderId)
{
    emit sharingSuccessful(i18n("The image was successfully uploaded to DeviantArt Sta.sh"), QString("http://sta.sh/0%1").arg(stashId, 0, 36));
}

#include "imageshare.moc"
