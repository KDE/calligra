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

#include "SpreadsheetService.h"
#include "GoogleSpreadsheetsPlugin.h"
#include "DocumentsListService.h"
#include "GoogleDocument.h"

#include "part/Doc.h"
#include "part/View.h"
#include "Map.h"

#include <KTemporaryFile>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QApplication>

#include <QDebug>


struct Worksheet
{
    QString etag;
    QString id;
    QString edited;
    QString title;
    //QString listUrl;
    QString cellsUrl;
    QString selfEditUrl;
    int colCount;
    int rowCount;
    KTemporaryFile sheetContent;
    Worksheet() : colCount(0), rowCount(0) {}
};

class SpreadsheetService::Private
{
    SpreadsheetService *q;
    
public:
    explicit Private(QObject *parent, SpreadsheetService *qq)
        : srvMgr(0),
        document(0)
    {
        q = qq;
        srvMgr = dynamic_cast<ServiceManager*>(parent);
    }
    
    ~Private()
    {
        clearWorksheetsList();
    }
    
    ServiceManager *srvMgr;
    
    GoogleDocument *document;
    
    QString authToken;
    
    //Worksheets-Feed Data
    QString sheetsListEtag;
    QString sheetsListId;         //GDATA-API: (worksheets)key == DocumentsList-Feed-Entry->id
    QString selfUrl;
    QString title;      // == DocumentsListEntry->title
    
    QList<Worksheet*> worksheets;
    
    QXmlStreamReader xmlReader;
    
    bool parseWorksheetsFeed(QByteArray &input);
    void parseWorksheetsEntry();
    QString errorString();
    
    void retrieveWorksheetsList(QNetworkReply *reply);
    void retrieveCellsForSheet(QNetworkReply *reply);
    
    inline void clearWorksheetsList() {
        foreach (Worksheet *sheet, worksheets)
            delete sheet;
        worksheets.clear();
    }
    
    inline void finishProgress(QNetworkReply *reply) 
    {
        reply->deleteLater();
        srvMgr->plugin()->endProgress();
        QApplication::restoreOverrideCursor();
    }
};

/********************************
    Private-Defs    
********************************/
bool SpreadsheetService::Private::parseWorksheetsFeed(QByteArray& input)
{
    xmlReader.clear();
    xmlReader.addData(input);
    
    if (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "feed") {
            sheetsListEtag = xmlReader.attributes().value("gd:etag").toString();
            while (xmlReader.readNextStartElement()) {
                if (xmlReader.name() == "id") {
                    QString tmp = xmlReader.readElementText();
                    int idx = tmp.indexOf("/private/full");
                    tmp = tmp.left(idx);
                    idx = tmp.lastIndexOf('/');
                    sheetsListId = tmp.mid(idx + 1);
                }
                else if (xmlReader.name() == "title") {
                    title = xmlReader.readElementText();
                }
                else if (xmlReader.name() == "link") {
                    if (xmlReader.attributes().value("rel") == "self")
                        selfUrl = xmlReader.attributes().value("href").toString();
                    xmlReader.skipCurrentElement();
                }
                else if (xmlReader.name() == "entry") {
                    parseWorksheetsEntry();
                }
                else{
                    xmlReader.skipCurrentElement();
                }
            }
        }
        else
            xmlReader.raiseError(QObject::tr("This is not a GData-Worksheets-Feed!"));
    }

    return !xmlReader.error();
}

void SpreadsheetService::Private::parseWorksheetsEntry()
{
    Q_ASSERT(xmlReader.isStartElement() && xmlReader.name() == "entry");
    
    Worksheet *sheet = new Worksheet;
    sheet->etag = xmlReader.attributes().value("gd:etag").toString();
    while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "id") {
            sheet->id = xmlReader.readElementText();
        }
        else if (xmlReader.name() == "edited")
            sheet->edited = xmlReader.readElementText();
        else if (xmlReader.name() == "title")
            sheet->title = xmlReader.readElementText();
        else if (xmlReader.name() == "link") {
            if (xmlReader.attributes().value("rel") == "http://schemas.google.com/spreadsheets/2006#cellsfeed")
                sheet->cellsUrl = xmlReader.attributes().value("href").toString();
            else if (xmlReader.attributes().value("rel") == "edit")
                sheet->selfEditUrl = xmlReader.attributes().value("href").toString();
            xmlReader.skipCurrentElement();
        }
        else if (xmlReader.name() == "colCount")
            sheet->colCount = QString(xmlReader.readElementText()).toInt();
        else if (xmlReader.name() == "rowCount")
            sheet->rowCount = QString(xmlReader.readElementText()).toInt();
        else
            xmlReader.skipCurrentElement();
    }
    worksheets.append(sheet);
}

QString SpreadsheetService::Private::errorString()
{
    return QObject::tr("%1\ncharOffset %2").arg(xmlReader.errorString()).arg(xmlReader.characterOffset());
}

void SpreadsheetService::Private::retrieveWorksheetsList(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 200) {
        clearWorksheetsList();
        QByteArray data = reply->readAll();
        if (parseWorksheetsFeed(data)) {
            finishProgress(reply);
            q->getCellsForSheet(worksheets.at(0)->cellsUrl);
        }
        else {
            //TODO
        }
    }
    else if (httpStatusCode == 304 || httpStatusCode == 412) {
        finishProgress(reply);
    }

}

void SpreadsheetService::Private::retrieveCellsForSheet(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpStatusCode == 200) {
        QByteArray content = reply->readAll();
        KTemporaryFile &tmpFile = worksheets.at(0)->sheetContent;
        tmpFile.setSuffix(".atom");
        if (tmpFile.open()) {
            tmpFile.write(content);
            tmpFile.close();

            KSpread::View *view = dynamic_cast<KSpread::View*>(srvMgr->plugin()->parentView());
            Q_ASSERT(view);//TODO can happen
            //KoDocument *doc = view->koDocument();
            KSpread::Doc *doc = view->doc();
            KSpread::Map *map = doc->map();
            Q_ASSERT(map);//TODO can happen
            QList<KSpread::Sheet*> unusedSheets = map->sheetList();
            QString tmpSheet(tmpFile.fileName());
            map->addNewSheet(tmpSheet);
            foreach (KSpread::Sheet *sheet, unusedSheets) {
                map->removeSheet(sheet);
            }
            KUrl url(tmpFile.fileName());
            
            bool ok = doc->openUrl(url);

            if (ok) {
                map->removeSheet(map->findSheet(tmpSheet));
            }
            else
                ;//TODO
        }
        else
        ;//TODO
    }
    
    finishProgress(reply);
}




/********************************
    SpreadsheetService-Defs    
********************************/
SpreadsheetService::SpreadsheetService(QObject* parent)
    : QObject(parent), 
    d(new Private(parent, this))
{
    connect(d->srvMgr, SIGNAL(spreadsheetsReplyFinished(QNetworkReply*,NetworkActions::Action)), 
            this, SLOT(handleSpreadsheetsReply(QNetworkReply*,NetworkActions::Action)));
}

SpreadsheetService::~SpreadsheetService()
{
    delete d;
}

bool SpreadsheetService::isAuthenticated() const
{
    return !d->authToken.isEmpty();
}

const QString &SpreadsheetService::authToken() const
{
    return d->authToken;
}

void SpreadsheetService::setAuthToken(const QString token)
{
    d->authToken = token;
}

void SpreadsheetService::openSpreadsheet(GoogleDocument *doc)
{       
        if (!d->document)
            d->document = doc;
        else if (d->document != doc) {
            d->clearWorksheetsList();
            d->sheetsListEtag.clear();
            d->document = doc;
        }
        d->srvMgr->plugin()->disconnectKoDocsSignals();
        
        getWorksheets();
}

void SpreadsheetService::getWorksheets()
{
    if (!isAuthenticated())
        d->srvMgr->authenticateService(GoogleServices::Wise);
    
    QNetworkRequest request(QUrl(d->document->worksheetsUrl()));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");

    //when Worksheets are retrieved before, check if they were updated
    if (!d->sheetsListEtag.isEmpty())
        request.setRawHeader("If-None-Match", d->sheetsListEtag.toUtf8());//TODO works??
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/atom+xml");
    request.setRawHeader("Authorization", d->authToken.toUtf8());
    
    QNetworkReply *reply = d->srvMgr->netManager()->get(request);
    d->srvMgr->addRequest(reply, GoogleServices::Wise, NetworkActions::GetWorksheetsFeed);
    d->srvMgr->plugin()->startProgress(0, 0, "Opening Spreadsheet '" + d->document->title() + "'...");
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void SpreadsheetService::getCellsForSheet(QString &cellsFeedUrl)
{
    QUrl url(cellsFeedUrl);
    QNetworkRequest request(url);
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("User-Agent", "CalligraSuite");
    //TODO etag / if-match-header
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/atom+xml");
    request.setRawHeader("Authorization", d->authToken.toUtf8());
    
    QNetworkReply *reply = d->srvMgr->netManager()->get(request);
    d->srvMgr->addRequest(reply, GoogleServices::Wise, NetworkActions::GetCellsFeed);
    d->srvMgr->plugin()->startProgress(0, 0, "Opening Spreadsheet '" + d->title 
                                      + "'...");
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void SpreadsheetService::handleSpreadsheetsReply(QNetworkReply* reply, 
                                                        NetworkActions::Action netAction)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatusCode != 200) {
        qDebug() << "###G_Spread_Serv::handleSpreadsheetsReply\thttpStatusCode=" << httpStatusCode;
        qDebug() << "###G_Spread_Serv::handleSpreadsheetsReply\thttpReasonPhraseAttr=" << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
    }
    if (httpStatusCode == 401) {
        d->finishProgress(reply);
        d->clearWorksheetsList();
        //d->document = 0;
        d->srvMgr->authenticateService(GoogleServices::Wise, QString("Authorization reqiered!\n"
                                        "You have to login to Google Spreadsheets Service."));
        return;
    }

    switch (netAction) {
//     case NetworkActions::OpenSpreadsheet:
//         openSpreadsheet(reply);
//         break;
    case NetworkActions::GetWorksheetsFeed:
        d->retrieveWorksheetsList(reply);
        break;
    case NetworkActions::GetCellsFeed:
        d->retrieveCellsForSheet(reply);
        break;
    default:
        d->finishProgress(reply);
    }
}


#include "SpreadsheetService.moc"
