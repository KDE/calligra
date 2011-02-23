/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "GoogleSpreadsheetsPlugin.h"
#include "ServiceManager.h"
#include "DocumentsListService.h"
#include "SpreadsheetService.h"
#include "GoogleDocument.h"

#include "KoView.h"
#include "KoMainWindow.h"
#include "KoProgressBar.h"

#include "part/Doc.h"
#include "part/View.h"
#include "Map.h"

#include <KAction>
#include <KLocale>
#include <KGenericFactory>
#include <KComponentData>
#include <KActionCollection>
#include <KStatusBar>
#include <KTemporaryFile>

#include <KShortcut>

#include <QLabel>

#include <QStringList>
#include <QDebug>
#include <QTimer>


class GoogleSpreadsheetsPlugin::Private
{
public:
    explicit Private(QObject *parent)
        : parentView(0),
        srvMgr(0),
        //progressUpdater(0),
        progressBar(0),
        statusMessage(0),
        currentDocsTmpFile(0),
        loadingDoc(false)
    {
        //parent Typ= KoView / KSpread-View (part/View.h)
        if (parent)
            parentView = dynamic_cast<KoView*>(parent);
    }
    
    ~Private()
    {
        if (srvMgr)
            delete srvMgr;
        
        if (progressBar) {
            delete progressBar;
        }
        
        if (statusMessage)
            delete statusMessage;
        
        if (currentDocsTmpFile)
            delete currentDocsTmpFile;
    }
    
    KoView *parentView;
    ServiceManager *srvMgr;
    KoProgressBar *progressBar;
    QLabel *statusMessage;
    
    KTemporaryFile *currentDocsTmpFile;
    QString currentDocsTmpFileName;
    bool loadingDoc;
    
    inline void deleteProgressBar() 
    {
        parentView->removeStatusBarItem(progressBar);
        delete progressBar;
        progressBar = 0;
    }
    
    inline void deleteStatusMessage() 
    {
        parentView->removeStatusBarItem(statusMessage);
        delete statusMessage;
        statusMessage = 0;
        parentView->statusBar()->clearMessage();
    }
};



typedef KGenericFactory<GoogleSpreadsheetsPlugin> GoogleSpreadsheetsPluginFactory;
K_EXPORT_COMPONENT_FACTORY(kspread_plugin_googlespreadsheets, GoogleSpreadsheetsPluginFactory("googlespreadsheets"))


GoogleSpreadsheetsPlugin::GoogleSpreadsheetsPlugin(QObject* parent, const QStringList& list)
    : KParts::Plugin(parent),
    d(new Private(parent))
{
    Q_UNUSED(list);

    setComponentData(GoogleSpreadsheetsPlugin::componentData());
    
    KAction* action = new KAction(i18n("&Open Google Spreadsheet..."), this);
    actionCollection()->addAction("google_spreadsheets", action);
    connect(action, SIGNAL(triggered()), this, SLOT(activate()));
    
    action = new KAction(i18n("Open &Documents List..."), this);
    actionCollection()->addAction("show_doclist", action);
    action->setEnabled(false);
    action->setVisible(false);
}

GoogleSpreadsheetsPlugin::~GoogleSpreadsheetsPlugin()
{
    delete d;
}

KoView *GoogleSpreadsheetsPlugin::parentView() const
{
    return d->parentView;
}

void GoogleSpreadsheetsPlugin::setProgressMaximum(int maximum)
{
    d->progressBar->setMaximum(maximum);
}

void GoogleSpreadsheetsPlugin::activate()
{
    if (d->srvMgr)
        d->srvMgr->documentsListService()->getDocumentsList();
    else
        d->srvMgr = new ServiceManager(this);

    d->srvMgr->authenticateService(GoogleServices::Writely);
    if (d->srvMgr->documentsListService()->isAuthenticated()) {
        d->srvMgr->documentsListService()->getDocumentsList();
    }
    else {
        delete d->srvMgr;
        d->srvMgr = 0;
    }
}

void GoogleSpreadsheetsPlugin::setProgress(int value)
{
    d->progressBar->setValue(value);
}

void GoogleSpreadsheetsPlugin::enableMenuActions()
{
    DocumentsListService *docListSrv = d->srvMgr->documentsListService();
    if (docListSrv) {
        KAction *a = dynamic_cast<KAction*>(actionCollection()->action("show_doclist"));
        a->setEnabled(true);
        a->setVisible(true);
    }
}

void GoogleSpreadsheetsPlugin::startProgress(int maximum/*=0*/, int minimum/*=0*/, QString msg/*QString*/)
{
    if (d->progressBar)
        d->deleteProgressBar();

    if (d->statusMessage)
        d->deleteStatusMessage();

    d->progressBar = new KoProgressBar();
    d->progressBar->setMaximum(maximum);
    d->progressBar->setMinimum(minimum);
    d->parentView->addStatusBarItem(d->progressBar);
    
    setStatusMessage(msg);
}

int GoogleSpreadsheetsPlugin::progressMaximum() const
{
    if (d->progressBar)
        return d->progressBar->maximum();
    
    return -1;
}

void GoogleSpreadsheetsPlugin::endProgress()
{
    if (d->progressBar)
        d->deleteProgressBar();
    
    if (d->statusMessage)
        d->deleteStatusMessage();
}

void GoogleSpreadsheetsPlugin::setStatusMessage(QString msg)
{
    if (d->statusMessage)
        d->deleteStatusMessage();
    
    d->statusMessage = new QLabel(msg);
    d->parentView->addStatusBarItem(d->statusMessage);
}

void GoogleSpreadsheetsPlugin::addToRecentUrl()
{
//     if (m_currentSpreadsheet) {
//         //TODO
//         //mainWindow->addRecentURL(_url); from KoDocument.cpp:1261
//     }
}

void GoogleSpreadsheetsPlugin::openExportedDocument(QByteArray& data, QString& docTitle)
{
    KSpread::View *view = dynamic_cast<KSpread::View*>(d->parentView);
    Q_ASSERT(view);//TODO can happen
    KSpread::Doc *doc = view->doc();
    
    if (d->currentDocsTmpFile) {
        KoMainWindow *mw = dynamic_cast<KoMainWindow*>(d->parentView->mainWindow());
        if (doc->isModified())
            mw->slotFileSave();
        doc->removeAutoSaveFiles();
        delete d->currentDocsTmpFile;
    }
    
    // tempfile will be deleted if it goes out of scope
    d->currentDocsTmpFile = new KTemporaryFile;
    
    d->currentDocsTmpFile->setSuffix(".ods"); // OpenDocument spreadsheet
    
    if (d->currentDocsTmpFile->open()) {
        d->currentDocsTmpFile->write(data);
        d->currentDocsTmpFile->close();

        // open spreadsheet in kspread
        KUrl url(d->currentDocsTmpFile->fileName());

        //TODO dirty
        KSpread::Map *map = doc->map();
        Q_ASSERT(map);//TODO can happen
        //TODO check if doc is empty
        QList<KSpread::Sheet*> sheetList = map->sheetList(); 
        QString tmpSheet = d->currentDocsTmpFile->fileName();
        map->addNewSheet(tmpSheet);
        foreach (KSpread::Sheet *sheet, sheetList) {
            map->removeSheet(sheet);
        }
        
        bool ok = doc->openUrl(url);
        Q_ASSERT(ok);//TODO can happen
        
        map->removeSheet(map->findSheet(tmpSheet));

        doc->setTitleModified(docTitle, false);
        d->currentDocsTmpFileName = d->currentDocsTmpFile->fileName();
        
        connect(doc, SIGNAL(completed()), this, SLOT(slotCompleted()));
        connect(doc, SIGNAL(modified(bool)), this, SLOT(slotModified(bool)));
    }
}

void GoogleSpreadsheetsPlugin::disconnectKoDocsSignals()
{
    KSpread::View *view = dynamic_cast<KSpread::View*>(d->parentView);
    KSpread::Doc *doc = view->doc();
    disconnect(doc, SIGNAL(completed()), this, SLOT(slotCompleted()));
    disconnect(doc, SIGNAL(modified(bool)), this, SLOT(slotModified(bool)));
}

void GoogleSpreadsheetsPlugin::slotModified(bool b)
{
    QString title = d->srvMgr->documentsListService()->currentDocument()->title();
    KSpread::View *view = dynamic_cast<KSpread::View*>(d->parentView);
    view->doc()->setTitleModified(title, b);
}

void GoogleSpreadsheetsPlugin::slotCompleted(bool forceUpdate/*=false*/)
{
    KSpread::View *view = dynamic_cast<KSpread::View*>(d->parentView);
    KSpread::Doc *doc = view->doc();

    int pos = d->currentDocsTmpFileName.lastIndexOf(".");
    QString ext = d->currentDocsTmpFileName.mid(pos + 1);
    QFile file(d->currentDocsTmpFileName);
    if (file.isOpen()) {
        file.flush();
        QTimer::singleShot(2000, this, SLOT(slotCompleted()));
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "###PLUGIN::slotCompleted\tOpen tmpFile failed!!";
        return;
    }
    QByteArray data = file.readAll();
    file.close();
    
    d->srvMgr->documentsListService()->updateOpenDocument(data, ext, forceUpdate);
}

#include "GoogleSpreadsheetsPlugin.moc"
