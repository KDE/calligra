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
        m_submitDlg->submitDlg()->txtTitle->setText(m_view->document()->url().fileName());
        connect(m_submitDlg, SIGNAL(accepted()), SLOT(performUpload()));
    }
}

ImageShare::~ImageShare()
{
    m_view = 0;
}

void ImageShare::slotImageShare()
{
    if(!m_submitDlg)
        return;
    m_deviantArt = new O2DeviantART(this);
    m_deviantArt->setClientId("272");
    m_deviantArt->setClientSecret("a8464938f858f68661c4246347f09b62");

    connect(m_deviantArt, SIGNAL(openBrowser(QUrl)), SLOT(openBrowser(QUrl)));
    connect(m_deviantArt, SIGNAL(closeBrowser()), SLOT(closeBrowser()));
    connect(m_deviantArt, SIGNAL(linkingSucceeded()), SLOT(showSubmit()));

    qDebug() << "slotImageShare" << m_deviantArt->token() << m_deviantArt->linked();

    if (!m_deviantArt->linked()) {
        m_deviantArt->link();
    }
    else {
        showSubmit();
    }
}

void ImageShare::openBrowser(const QUrl &url)
{
    qDebug() << "openBrowser" << url << m_deviantArt->token();
    DlgLogin dlgLogin(m_deviantArt);
    dlgLogin.setLoginUrl(url);
    dlgLogin.exec();
}

void ImageShare::closeBrowser()
{
    qDebug() << "close browser" << m_deviantArt->token();
}

void ImageShare::showSubmit()
{
    m_stash = new Stash(m_deviantArt, this);
    connect(m_stash, SIGNAL(callFinished(Stash::Call,bool)), SLOT(testCallCompleted(Stash::Call,bool)));
    connect(m_stash, SIGNAL(submissionsChanged()), SLOT(submissionsChanged()));
    m_stash->testCall();
}

void ImageShare::testCallCompleted(Stash::Call, bool result)
{
    if(!result) {
        qDebug() << Q_FUNC_INFO << "if this happens, something failed and we'll need to start over...";
    }
    disconnect(m_stash, SIGNAL(callFinished(Stash::Call,bool)), this, SLOT(testCallCompleted(Stash::Call,bool)));
    m_stash->delta();
    m_submitDlg->open();
}

void ImageShare::performUpload()
{
    QString folderId;
    // First item is non-folder, so if it's that, don't assign a folder
    if(m_submitDlg->submitDlg()->folderList->currentIndex() != 0) {
        folderId = m_submitDlg->submitDlg()->folderList->itemData(m_submitDlg->submitDlg()->folderList->currentIndex()).toString();
    }
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

#include "imageshare.moc"
