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

#include "DocumentsListService.h"
#include "DocumentsListDialog.h"
#include "GoogleSpreadsheetsPlugin.h"
#include "GoogleDocument.h"
#include "SpreadsheetService.h"

#include <KoView.h>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamAttributes>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <QDebug>


class DocumentsListService::Private
{
public:
    DocumentsListService *q;
    
    explicit Private(QObject *parent, DocumentsListService *qq);
    
    ~Private()
    {
        clearDocumentsList();
    }
    
    ServiceManager *srvMgr;
    
    GoogleDocument *currentDocument;
    QList<GoogleDocument*> documents;
    
    QString authToken;
    QString docsListEtag;
    QString docsListTitle;
    
    //Documents List User Metadata
    QMultiHash<QString, QString> exportFormats;
    QMultiHash<QString, QString> importFormats;
    QHash<QString, QString> fileMimeTypes; //TODO mimetypes for files from KDE-Service???
    QString metadataEtag;
    int quotaBytesTotal;
    int quotaBytesUsed;
    int quotaBytesUsedTrash;
    int maxUploadSizeDocuments;
    int maxUploadSizeSpreadsheets;
    int maxUploadSizePresentations;
    int maxUploadSizeDrawings;
    int maxUploadSizePdf;
    int maxUploadSizeFile;
    
    GoogleServices::DocType docType;
    bool docListDialogOpen;
    QString dlDocTitle;
    
    QXmlStreamReader xmlReader; //??TODO use KoXMLReader / Writer??
    
    bool parseDocListFeed(QByteArray &input);
    void parseDocListEntry(bool update = false);
    bool parseUpdateFeed(QByteArray &input);
    bool parseMetadata(QByteArray &input);
    QString errorString();
    
    void showDocumentsListDialog();
    QNetworkReply *createDownlaodRequest(int entryNr, QString format, QString msg = QString());
    void saveFile(QNetworkReply *reply);
    void retrieveDocumentsList(QNetworkReply *reply);
    void retrieveDownloadedDocument(QNetworkReply* reply, NetworkActions::Action netAction);
    void handleUploadReply(QNetworkReply *reply);
    void handleDeleteReply(QNetworkReply *reply);
    void handleUpdateReply(QNetworkReply *reply);
    void retrieveMetadata(QNetworkReply *reply);
    
    inline void finishProgress(QNetworkReply *reply/*, QString msg = ""*/)
    {
//qDebug() << "###FINISHED_PROGRESS::caller=" << msg;
        reply->deleteLater();
        srvMgr->plugin()->endProgress();
        QApplication::restoreOverrideCursor();
    }
    
    inline void clearDocumentsList() {
        foreach (GoogleDocument *doc, documents)
            delete doc;
        documents.clear();
    }
};

/********************************
    Private-Defs    
********************************/
DocumentsListService::Private::Private(QObject* parent, DocumentsListService *qq)
    : srvMgr(0),
    currentDocument(0),
    quotaBytesTotal(-1),
    quotaBytesUsed(-1),
    quotaBytesUsedTrash(-1),    
    maxUploadSizeDocuments(-1),
    maxUploadSizeSpreadsheets(-1),
    maxUploadSizePresentations(-1),
    maxUploadSizeDrawings(-1),
    maxUploadSizePdf(-1),
    maxUploadSizeFile(-1),
    docType(GoogleServices::Spreadsheet),
    docListDialogOpen(false)
{
    q = qq;
    srvMgr = dynamic_cast<ServiceManager*>(parent);
    
    //spreadsheets
    fileMimeTypes.insert("text/csv", "cvs");
    fileMimeTypes.insertMulti("text/tab-separated-values", "tsv");
    fileMimeTypes.insertMulti("text/tab-separated-values", "tab");
    fileMimeTypes.insert("application/x-vnd.oasis.opendocument.spreadsheet", "ods");
    fileMimeTypes.insert("application/vnd.ms-excel", "xls");
    fileMimeTypes.insert("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "xlsx");
    //Documents
    fileMimeTypes.insertMulti("text/html", "html");
    fileMimeTypes.insertMulti("text/html", "htm");
    fileMimeTypes.insert("application/vnd.oasis.opendocument.text", "odt");
    fileMimeTypes.insert("application/msword", "doc");
    fileMimeTypes.insert("application/vnd.openxmlformats-officedocument.wordprocessingml.document", "docx");
    fileMimeTypes.insert("application/rtf", "rtf");
    fileMimeTypes.insert("text/rtf", "rtf");
    fileMimeTypes.insert("application/vnd.sun.xml.writer", "swx");
    fileMimeTypes.insert("text/plain", "txt");
    fileMimeTypes.insert("application/pdf", "pdf");
    //Presentations
    fileMimeTypes.insertMulti("application/vnd.ms-powerpoint", "ppt");
    fileMimeTypes.insertMulti("application/vnd.ms-powerpoint", "pps");
    //Drawings
    fileMimeTypes.insert("image/svg+xml", "svg");
    //Arbitrary files
    fileMimeTypes.insert("application/zip", "zip");
    fileMimeTypes.insert("application/x-msmetafile", "wmf");
    fileMimeTypes.insert("image/gif", "gif");
    fileMimeTypes.insert("image/bmp", "bmp");
    fileMimeTypes.insert("image/png", "png");
    fileMimeTypes.insertMulti("image/jpeg", "jpg");
    fileMimeTypes.insertMulti("image/jpeg", "jpeg");
    fileMimeTypes.insert("application/x-shockwave-flash", "swf");
}

bool DocumentsListService::Private::parseDocListFeed(QByteArray& input)
{
    xmlReader.clear();
    xmlReader.addData(input);

    if (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "feed") {
            docsListEtag = xmlReader.attributes().value("gd:etag").toString();
            while (xmlReader.readNextStartElement()) {
                if (xmlReader.name() == "id") {
                    xmlReader.skipCurrentElement();
                }
                else if (xmlReader.name() == "title") {
                    docsListTitle = xmlReader.readElementText();
                }
                else if (xmlReader.name() == "entry") {
                    parseDocListEntry();
                }
                else{
                    xmlReader.skipCurrentElement();
                }
            }
        }
        else
            xmlReader.raiseError(QObject::tr("This is not a GData-DocList-Feed!"));
    }

    return !xmlReader.error();
}

void DocumentsListService::Private::parseDocListEntry(bool update/*=false*/)
{
    Q_ASSERT(xmlReader.isStartElement() && xmlReader.name() == "entry");
 
    GoogleDocument *doc;
    if (update)
        doc = currentDocument;
    else
        doc = new GoogleDocument;
    doc->setEtag(xmlReader.attributes().value("gd:etag").toString());
    while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "id")
            doc->setId(xmlReader.readElementText());
        else if (xmlReader.name() == "published")
            doc->setPublished(xmlReader.readElementText());
        else if (xmlReader.name() == "updated")
            doc->setUpdated(xmlReader.readElementText());
        else if (xmlReader.name() == "edited")
            doc->setEdited(xmlReader.readElementText());
        else if (xmlReader.name() == "category") {
            if (xmlReader.attributes().value("scheme") == "http://schemas.google.com/g/2005#kind")
                doc->setDocType(xmlReader.attributes().value("label").toString());
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "title")
            doc->setTitle(xmlReader.readElementText());
        else if (xmlReader.name() == "content") {
            doc->setExportUrl(xmlReader.attributes().value("src").toString());
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "link") {
            if (xmlReader.attributes().value("rel") == "http://schemas.google.com/spreadsheets/2006#worksheetsfeed")
                doc->setWorksheetsUrl(xmlReader.attributes().value("href").toString());
            else if (xmlReader.attributes().value("rel") == "http://schemas.google.com/spreadsheets/2006#tablesfeed")
                doc->setTablesUrl(xmlReader.attributes().value("href").toString());
            else if (xmlReader.attributes().value("rel") == "edit")
                doc->setEditUrl(xmlReader.attributes().value("href").toString());
            else if (xmlReader.attributes().value("rel") == "edit-media")
                doc->setEditMediaUrl(xmlReader.attributes().value("href").toString());
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "author") {
            while (xmlReader.readNextStartElement()) {
                if (xmlReader.name() == "email")
                    doc->setAuthor(xmlReader.readElementText());
                else
                    xmlReader.skipCurrentElement();
            }
        }
        else if (xmlReader.name() == "lastModifiedBy") {
            while (xmlReader.readNextStartElement()) {
                if (xmlReader.name() == "email")
                    doc->setLastModifiedBy(xmlReader.readElementText());
                else
                    xmlReader.skipCurrentElement();
            }
        }
        else
            xmlReader.skipCurrentElement();
    }
    
    if (!update)
        documents.append(doc);
}

bool DocumentsListService::Private::parseUpdateFeed(QByteArray& input)
{
    xmlReader.clear();
    xmlReader.addData(input);
    
    if (xmlReader.readNextStartElement() && xmlReader.name() == "entry") {
        parseDocListEntry(true);
    }
    else
        xmlReader.raiseError(QObject::tr("This is not a Documents-List-Update-Feed!"));
    
    return !xmlReader.error();
}

bool DocumentsListService::Private::parseMetadata(QByteArray& input)
{   
    xmlReader.clear();
    xmlReader.addData(input);
    
    if (!metadataEtag.isEmpty()) {
        metadataEtag.clear();
        importFormats.clear();
        exportFormats.clear();
    }
    
    if (xmlReader.readNextStartElement() && xmlReader.name() == "entry") {
        metadataEtag = xmlReader.attributes().value("gd:etag").toString();
        while (xmlReader.readNextStartElement()) {
            if (xmlReader.name() == "quotaBytesTotal")
                quotaBytesTotal = xmlReader.readElementText().toInt();
            else if (xmlReader.name() == "quotaBytesUsed")
                quotaBytesUsed = xmlReader.readElementText().toInt();
            else if (xmlReader.name() == "quotaBytesUsedInTrash")
                quotaBytesUsedTrash = xmlReader.readElementText().toInt();
            else if (xmlReader.name() == "importFormat") {
                QString key = xmlReader.attributes().value("source").toString();
                QString value = xmlReader.attributes().value("target").toString();
                importFormats.insert(key, value);
                xmlReader.skipCurrentElement();
            }
            else if (xmlReader.name() == "exportFormat") {
                QString key = xmlReader.attributes().value("source").toString();
                QString value = xmlReader.attributes().value("target").toString();
                exportFormats.insert(key, value);
                xmlReader.skipCurrentElement();
            }
            else if (xmlReader.name() == "maxUploadSize") {
                QString kind = xmlReader.attributes().value("kind").toString();
                if (kind == "document")
                    maxUploadSizeDocuments = xmlReader.readElementText().toInt();
                else if (kind == "spreadsheet")
                    maxUploadSizeSpreadsheets = xmlReader.readElementText().toInt();
                else if (kind == "presentation")
                    maxUploadSizePresentations = xmlReader.readElementText().toInt();
                else if (kind == "drawing")
                    maxUploadSizeDrawings = xmlReader.readElementText().toInt();
                else if (kind == "pdf")
                    maxUploadSizePdf = xmlReader.readElementText().toInt();
                else if (kind == "file")
                    maxUploadSizeFile = xmlReader.readElementText().toInt();
                else
                    xmlReader.skipCurrentElement();
            }
            else
                xmlReader.skipCurrentElement();
        }
    }
    else
        xmlReader.raiseError(QObject::tr("This is not a Documents-List-Metadata-Feed!"));
    
    return !xmlReader.error();
}


QString DocumentsListService::Private::errorString()
{
    return QObject::tr("%1\ncharOffset %2").arg(xmlReader.errorString()).arg(xmlReader.characterOffset());
}

void DocumentsListService::Private::showDocumentsListDialog()
{
    DocumentsListDialog dialog(srvMgr->plugin()->parentView(), q);
    
    dialog.showDocumentsList(documents, docsListTitle);
    docListDialogOpen = true;
    dialog.exec();
    docListDialogOpen = false;
}

QNetworkReply *DocumentsListService::Private::createDownlaodRequest(int entryNr, QString format, QString msg)
{
    if (authToken.isEmpty())
        srvMgr->authenticateService(GoogleServices::Writely);
    
    GoogleDocument *doc = documents.at(entryNr);
    if (!doc) {
        //TODO error
        return 0;
    }

    QString formatStr;
    if (doc->docType() == "document")
        formatStr = "&exportFormat=" + format + "&format=" + format;
    else if (doc->docType() == "presentation" || doc->docType() == "spreadsheet")
        formatStr = "&exportFormat=" + format;

    QNetworkRequest request(QUrl(doc->exportUrl() + formatStr));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/atom+xml");
    
    if (doc->docType() == "spreadsheet") {
        SpreadsheetService *spreadSrv = srvMgr->spreadsheetService();
        if (!spreadSrv->isAuthenticated())
            srvMgr->authenticateService(GoogleServices::Wise);
            
        request.setRawHeader("Authorization", spreadSrv->authToken().toUtf8());
    }
    else {
        request.setRawHeader("Authorization", authToken.toUtf8());
    }
    
    dlDocTitle = doc->title();

    QNetworkReply *reply = srvMgr->netManager()->get(request);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), srvMgr, SLOT(slotProgress(qint64,qint64)));
    srvMgr->plugin()->startProgress(0, 0, msg);
    return reply;
}

void DocumentsListService::Private::saveFile(QNetworkReply* reply)
{
    if (reply->hasRawHeader("Content-Disposition")) {
        QString fileExt = reply->rawHeader("Content-Disposition");
        int pos = fileExt.lastIndexOf(".");
        fileExt = QString(fileExt.mid(pos)).remove('\"');
        QString fileTitel = dlDocTitle;
        QString path = QDir::homePath() + "/" + fileTitel + fileExt;

        QWidget *parentWidget = dynamic_cast<QWidget*>(srvMgr->plugin()->parentView());
        QString filePath = QFileDialog::getSaveFileName(parentWidget, "Export document to...", 
                    path); //TODO file-Filter
        if (!filePath.isEmpty()) {
            QByteArray data = reply->readAll();
            
            QFile file(filePath);
            file.open(QIODevice::ReadWrite); //TODO 
            file.write(data);
            file.close();
        }
        
        dlDocTitle.clear();
    }
}

void DocumentsListService::Private::retrieveDocumentsList(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 200) {
        QString currentDocsEtag;
        if (currentDocument)
            currentDocsEtag = currentDocument->etag();
        
        clearDocumentsList();
        QByteArray data = reply->readAll();
        if (parseDocListFeed(data)) {
            finishProgress(reply);
            if (!currentDocsEtag.isEmpty()) {
                foreach (GoogleDocument *doc, documents) {
                    if (currentDocsEtag == doc->etag())
                        currentDocument = doc;
                }
            }
            if (docListDialogOpen)
                emit q->updateDocListDialog(documents, docsListTitle);
            else
                showDocumentsListDialog();
        }
        else {
            //TODO
        }
    }
    else if (httpStatusCode == 304 || httpStatusCode == 412) {
        finishProgress(reply);
        showDocumentsListDialog();
    }
}

void DocumentsListService::Private::retrieveDownloadedDocument(QNetworkReply* reply, 
                                                               NetworkActions::Action netAction)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 200) {
        switch (netAction) {
        case NetworkActions::OpenDocument: {
            QByteArray data = reply->readAll();
            srvMgr->plugin()->openExportedDocument(data, dlDocTitle);

            dlDocTitle.clear();
            break;
        }
        case NetworkActions::ExportDocument:
            saveFile(reply);
            break;
        default:
            ;
        }
    }
    else if (httpStatusCode == 302) {
        //TODO spreadsheets
        QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        QNetworkRequest request(url);
        request.setRawHeader("GData-Version", "3.0");
        request.setRawHeader("User-Agent", "CalligraSuite");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/atom+xml");
        request.setRawHeader("Authorization", authToken.toUtf8());
        finishProgress(reply);
        reply = srvMgr->netManager()->get(request);
        srvMgr->addRequest(reply, GoogleServices::Writely, netAction);
        connect(reply, SIGNAL(uploadProgress(qint64,qint64)), srvMgr, SLOT(slotProgress(qint64,qint64)));
        srvMgr->plugin()->startProgress(0, 0);
        return;
    }
    else if (httpStatusCode == 417 || httpStatusCode == 500) {
        dlDocTitle.clear();
    }
    
    finishProgress(reply);
}

void DocumentsListService::Private::handleUploadReply(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 201) {
        finishProgress(reply);
        if (docListDialogOpen)
            q->getDocumentsList();
        return;
    }
    else if (httpStatusCode == 400) {
        //TODO
    }

    finishProgress(reply);
}

void DocumentsListService::Private::handleDeleteReply(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 200) {
        finishProgress(reply);
        q->getDocumentsList();
        return;
    }
    else if (httpStatusCode == 412) { // 412 - Precondition Failed(Etag mismatch)
        //TODO error
    }

    finishProgress(reply);
}

void DocumentsListService::Private::handleUpdateReply(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    QByteArray data = reply->readAll();
    if (httpStatusCode == 200) {
        if (!parseUpdateFeed(data)) {
            //TODO error
        }
    }
    else if (httpStatusCode == 412) { // 412 - Precondition Failed(Etag mismatch)
        QString str = "This document was modified by someone else!\nDo you still want updating \"" 
            + currentDocument->title() + "\".\nAll changes made by others will be overwritten!";
        QWidget *parentWidget = dynamic_cast<QWidget*>(srvMgr->plugin()->parentView());
        QMessageBox::StandardButton btn = QMessageBox::warning(parentWidget, 
                            tr("Updating Document..."), str, QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes) {
            finishProgress(reply);
            srvMgr->plugin()->slotCompleted(true);
            return;
        }
    }

    finishProgress(reply);
}

void DocumentsListService::Private::retrieveMetadata(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 200) {
        QByteArray data = reply->readAll();
        if (!parseMetadata(data)) {
            //TODO
qDebug() << "###G_DocList_Serv::Private::retrieveMetadata\txmlerror=" << errorString();
        }
    }

    finishProgress(reply);
}




/********************************
    DocumentsListService-Defs    
********************************/
DocumentsListService::DocumentsListService(QObject* parent)
    : QObject(parent),
    d(new Private(parent, this))
{
    connect(d->srvMgr, SIGNAL(docListReplyFinished(QNetworkReply*,NetworkActions::Action)), 
            this, SLOT(handleDocListReply(QNetworkReply*,NetworkActions::Action)));
}

DocumentsListService::~DocumentsListService()
{ 
    delete d;
}

bool DocumentsListService::isAuthenticated() const
{
    return !d->authToken.isEmpty();
}

const QString &DocumentsListService::authToken() const
{
    return d->authToken;
}

void DocumentsListService::setAuthToken(QString token)
{
    d->authToken = token;
}

GoogleServices::DocType DocumentsListService::docType() const
{
    return d->docType;
}

const QList<GoogleDocument*> &DocumentsListService::documentsList() const
{
    return d->documents;
}

GoogleDocument *DocumentsListService::currentDocument() const
{
    return d->currentDocument;
}

const QStringList DocumentsListService::formatsForExport(const QString& docType)
{
    if (d->metadataEtag.isEmpty())
        getDocumentsListMetadata();
    
    QStringList mimetypes, formats;
    if ((docType == "document") || (docType == "spreadsheet") || (docType == "presentation")) {
        mimetypes = d->exportFormats.values(docType);
        foreach (const QString &mimetype, mimetypes)
            formats << d->fileMimeTypes.value(mimetype);
    }
    else {
        QString suffix = d->fileMimeTypes.value(docType);
        if (!suffix.isEmpty())
            formats << suffix;
        else
            formats << docType;
    }
    
    formats.removeDuplicates();
    return formats;
}

void DocumentsListService::getDocumentsListMetadata()
{
    if (!isAuthenticated())
        d->srvMgr->authenticateService(GoogleServices::Writely);
    
    QNetworkRequest request(QUrl("https://docs.google.com/feeds/metadata/default"));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/atom+xml");
    request.setRawHeader("Authorization", d->authToken.toUtf8());
    
    QNetworkReply *reply = d->srvMgr->netManager()->get(request);
    d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::GetDocListMetadata);
    d->srvMgr->plugin()->startProgress(0, 0, "Retrieving Documents-List Metadata...");
    
    //QEventLoop to make this request nearly synchronous, because we have to wait for the reply
    //before we can continue processing
    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void DocumentsListService::getDocumentsList()
{
    if (!isAuthenticated())
        d->srvMgr->authenticateService(GoogleServices::Writely);
    
    if (d->docListDialogOpen)
        emit waitForDocList(true);

    QString reqUrl(GoogleServices::GOOGLE_DOCUMENTSLIST_BASE_URL);
    
    switch (d->docType) {
    case GoogleServices::TextDocument:
        reqUrl += "/-/document";
        break;
    case GoogleServices::Spreadsheet:
        reqUrl += "/-/spreadsheet";
        break;
    case GoogleServices::Presentation:
        reqUrl += "/-/presentation";
        break;
    case GoogleServices::Drawing:
        reqUrl += "/-/drawing";
        break;
    case GoogleServices::PDF:
        reqUrl += "/-/pdf";
        break;
    case GoogleServices::All:
    default:
        ;
    }
    
    QUrl url(reqUrl);
    QNetworkRequest request(url);
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    
    //when DocumentsList is retrieved before, check if it had updated
    if (!d->docsListEtag.isEmpty())
        request.setRawHeader("If-None-Match", d->docsListEtag.toUtf8()); //don't work with 'If-None-Match', even 'If-Match'
        
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/atom+xml");
    request.setRawHeader("Authorization", d->authToken.toUtf8());

    QNetworkReply *reply = d->srvMgr->netManager()->get(request);
    d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::GetDocList);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    d->srvMgr->plugin()->startProgress(0, 0, "Retrieving Documents-List...");
}

void DocumentsListService::downloadDocument(int entryNr, QString format)
{
    QNetworkReply *reply = d->createDownlaodRequest(entryNr, format, "Downloading Document...");
    if (reply != 0) {
        d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::ExportDocument);
    }
//TODO error
}

void DocumentsListService::openDocument(int entryNr)
{
    d->currentDocument = d->documents.at(entryNr);
    QString str = "Open via Spreadsheet-API?";
    QWidget *parentWidget = dynamic_cast<QWidget*>(d->srvMgr->plugin()->parentView());
    QMessageBox::StandardButton btn = QMessageBox::question(parentWidget, 
                            tr("Opening Spreadsheet..."), str, QMessageBox::Yes | QMessageBox::No);
    if (btn == QMessageBox::Yes)
        d->srvMgr->spreadsheetService()->openSpreadsheet(d->currentDocument);
    else {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QNetworkReply *reply = d->createDownlaodRequest(entryNr, "ods", "Opening Document...");
        if (reply != 0) {
            d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::OpenDocument);
        }
    }
//TODO error
}

void DocumentsListService::uploadDocument(QString filePath)
{
    if (d->metadataEtag.isEmpty())
        getDocumentsListMetadata();
    
    if (!isAuthenticated())
        d->srvMgr->authenticateService(GoogleServices::Writely);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << QString("Could not open %1 for writing: %2\n").arg(filePath).arg(file.errorString());
        return;
    }
    QFileInfo fileInfo(file);
    
    if (d->metadataEtag.isEmpty())
        getDocumentsListMetadata();
    
    QString suffix = fileInfo.suffix().toLower();
    QString mimetype = d->fileMimeTypes.key(suffix);
    if (mimetype.isEmpty()) {
        //TODO
        mimetype = "application/file"; //???
    }
    
    bool arbitrary = false;
    QString docType = d->importFormats.value(mimetype);
    if (docType.isEmpty()) {
        arbitrary = true;
        docType = "file"; //???
    }
    
    QString reqUrl = GoogleServices::GOOGLE_DOCUMENTSLIST_BASE_URL;
    if (arbitrary)
        reqUrl += "/?convert=false";

    QUrl url(reqUrl);
    QNetworkRequest request(url);
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/related; boundary=END_OF_PART");
    request.setRawHeader("Authorization", d->authToken.toUtf8());
    request.setRawHeader("Slug", fileInfo.fileName().toUtf8());
    
    QByteArray reqData(QString("--END_OF_PART\r\n").toUtf8());
    reqData += "Content-Type: application/atom+xml\r\n\r\n";
    reqData += "<?xml version='1.0' encoding='UTF-8'?>\r\n";
    reqData += "<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:docs=\"http://schemas.google.com/docs/2007\">\r\n";
    if (!arbitrary) {//TODO test
        reqData += "<category scheme=\"http://schemas.google.com/g/2005#kind\" ";
        reqData += "term=\"http://schemas.google.com/docs/2007#" + docType.toUtf8() + "\"/>\r\n";
    }
    reqData += "<title>" + QString(fileInfo.baseName()).toUtf8() + "</title>\r\n";
    reqData += "<docs:writersCanInvite value=\"true\"/>\r\n";
    reqData += "</entry>\r\n\r\n";
    reqData += QString("--END_OF_PART\r\n").toUtf8();
    reqData += "Content-Type: " + mimetype.toUtf8() + "\r\n\r\n";
    
    reqData += file.readAll();
    
    reqData += "\r\n\r\n--END_OF_PART--";
    
    request.setHeader(QNetworkRequest::ContentLengthHeader, reqData.length());

    QNetworkReply *reply = d->srvMgr->netManager()->post(request, reqData);
    d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::UploadDocument);
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), d->srvMgr, SLOT(slotProgress(qint64,qint64)));
    d->srvMgr->plugin()->startProgress(0, 0, "Uploading Document...");
}

// void DocumentsListService::requestRenameDocument(int entryNr)
// {
//     if (!isAuthenticated())
//         d->srvMgr->authenticateService(GoogleServices::writely);
//     
//     
// }

void DocumentsListService::deleteDocument(int entryNr/*, bool permanently*/)
{
    if (!isAuthenticated())
        d->srvMgr->authenticateService(GoogleServices::Writely);
    
    GoogleDocument *doc = d->documents.at(entryNr);
    if (!doc) {
        return;
    }

//     QString reqUrl = doc->editUrl;
//     if (permanently)
//         reqUrl += "?delete=true";
    
    QNetworkRequest request(QUrl(doc->editUrl()));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    request.setRawHeader("If-Match", doc->etag().toUtf8());
    request.setRawHeader("Authorization", d->authToken.toUtf8());
   
    QNetworkReply *reply = d->srvMgr->netManager()->deleteResource(request);
    d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::DeleteDocument);
    d->srvMgr->plugin()->startProgress(0, 0, "Deleting Document...");
}

void DocumentsListService::updateOpenDocument(QByteArray data, QString& fileExt, bool force/*=false*/)
{
    if (!isAuthenticated())
        d->srvMgr->authenticateService(GoogleServices::Writely);
    
    if (!d->currentDocument || data.isEmpty()) {
        //TODO
        return;
    }
    
    QString mimetype = d->fileMimeTypes.key(fileExt.toLower());
    if (mimetype.isEmpty()) {
        //TODO
        return;
    }

    QUrl url(d->currentDocument->editMediaUrl());
qDebug() << "!!!!!!!!G_DocList_Serv::updateOpenDocument\treq-url=" << url.toString();
    QNetworkRequest request(url);
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    request.setHeader(QNetworkRequest::ContentTypeHeader, mimetype.toUtf8());
    
    QString etag;
    if (force)
        etag = "*";
    else
        etag = d->currentDocument->etag();
    request.setRawHeader("If-Match", etag.toUtf8());
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/related; boundary=END_OF_PART");
    request.setRawHeader("Authorization", d->authToken.toUtf8());
    //request.setRawHeader("Slug", fileInfo.fileName().toUtf8());
    
//     QByteArray reqData(QString("--END_OF_PART\r\n").toUtf8());
//     reqData += "Content-Type: application/atom+xml\r\n\r\n";
//     reqData += "<?xml version='1.0' encoding='UTF-8'?>\r\n";
//     reqData += "<entry xmlns=\"http://www.w3.org/2005/Atom\">";// "xmlns:docs=\"http://schemas.google.com/docs/2007\">";
//     reqData += "xmlns:gd=\"http://schemas.google.com/g/2005\" gd:etag=\"" + d->currentDocument->etag().toUtf8() + "\"\r\n";
//     reqData += "<category scheme=\"http://schemas.google.com/g/2005#kind\" ";
//     reqData += "term=\"http://schemas.google.com/docs/2007#" + d->currentDocument->docType().toUtf8() + "\"/>\r\n";
//     //reqData += "<title>" + d->currentDocument->title().toUtf8() + "</title>\r\n";
//     //reqData += "<updated>" +  + "</updated";
//     reqData += "</entry>\r\n\r\n";
//     reqData += QString("--END_OF_PART\r\n").toUtf8();
//     reqData += "Content-Type: " + mimetype.toUtf8() + "\r\n\r\n";
//     
//     reqData += data;
//     reqData += "\r\n\r\n--END_OF_PART";
//    reqData += "\r\n\r\n--END_OF_PART--";
    
    QByteArray reqData;
    reqData += data;
    request.setHeader(QNetworkRequest::ContentLengthHeader, reqData.length());

    QNetworkReply *reply = d->srvMgr->netManager()->put(request, reqData);
    d->srvMgr->addRequest(reply, GoogleServices::Writely, NetworkActions::UpdateDocument);
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), d->srvMgr, SLOT(slotProgress(qint64,qint64)));
    d->srvMgr->plugin()->startProgress(0, 0, "Updating Document...");
}

void DocumentsListService::setDocType(int type)
{
    d->docType = static_cast<GoogleServices::DocType>(type);    
    getDocumentsList();
}

void DocumentsListService::handleDocListReply(QNetworkReply* reply, 
                                                    NetworkActions::Action netAction)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatusCode != 200) {
        qDebug() << "###G_DocList_Serv::handleDocListReply\thttpStatusCode=" << httpStatusCode;
        qDebug() << "###G_DocList_Serv::handleDocListReply\thttpReasonPhraseAttr=" << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
    }
    
    if (httpStatusCode == 401) {
        d->authToken.clear();
        d->finishProgress(reply);
        //TODO auth for spreadsheets-service
        d->srvMgr->authenticateService(GoogleServices::Writely, QString("Authorization reqiered!\n"
                                            "You have to login to Google Documents List Service."));
        return;
    }

    switch (netAction) {
    case NetworkActions::GetDocList:
        d->retrieveDocumentsList(reply);
        break;
    case NetworkActions::ExportDocument:
    case NetworkActions::OpenDocument:
        d->retrieveDownloadedDocument(reply, netAction);
        break;
    case NetworkActions::UploadDocument:
        d->handleUploadReply(reply);
        break;
    case NetworkActions::DeleteDocument:
        d->handleDeleteReply(reply);
        break;
    case NetworkActions::GetDocListMetadata:
        d->retrieveMetadata(reply);
        break;
    case NetworkActions::UpdateDocument:
        d->handleUpdateReply(reply);
        break;
    case NetworkActions::Nothing:
        //TODO error-handling
    default:
        d->finishProgress(reply);
    }
}

#include "DocumentsListService.moc"
