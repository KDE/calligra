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

#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>


class GoogleSpreadsheetsPlugin;
class DocumentsListService;
class SpreadsheetService;
class QNetworkAccessManager;
class QNetworkReply;


namespace NetworkActions {
    enum Action {
        Nothing,
        AuthWritely,        //Authenticate DocumentsList-Service
        AuthWise,           //Authenticate Spreadsheets-Service
        GetDocListMetadata, //requesting the Documents List Metadata-Feed for the logged-in user
        GetDocList,         //requesting the users DocumentsList
        RenameDocument,     //updates Documents-MetaData - Rename Title
        DeleteDocument,     //deletes a Document
        OpenDocument,       //exports Doc as temp. ODF and opens it in current View
        OpenSpreadsheet,    //opens a Spreadsheet via the Spreadsheet-API and opens it in current KSpread-View
        GetWorksheetsFeed,  //requesting the Worksheet-Feed of a Spreadsheet
        GetCellsFeed,       //requesting a Cell-Feed for a Worksheet(single Sheet)
        ExportDocument,     //exporting a Document or arbitrary file
        UpdateDocument,     //updates the opened Document
        UploadDocument      //uploads a Document or arbitrary file to Google-Docs
        //...
    };
}

namespace GoogleServices {
    enum Service {
        none,
        ClientLogin,        //Authentication-Service for installed Applications
        Writely,            //DocumentsList-Service
        Wise                //Spreadsheet-Service
    };
    
    enum DocType {
        All,
        TextDocument,
        Spreadsheet,
        Presentation,
        Drawing,
        PDF
    };
    
    const QString GOOGLE_CLIENTLOGIN_URL = "https://www.google.com/accounts/ClientLogin";
    const QString GOOGLE_DOCUMENTSLIST_BASE_URL = "https://docs.google.com/feeds/default/private/full";
    const QString GOOGLE_SPREADSHEETS_BASE_URL = "https://spreadsheets.google.com/feeds";
}


class ServiceManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ServiceManager(QObject *parent = 0);
    virtual ~ServiceManager();

    void authenticateService(GoogleServices::Service service, const QString errorMsg = QString());
    void addRequest(QNetworkReply* reply, GoogleServices::Service service, 
                           NetworkActions::Action netAction);
    
    QNetworkAccessManager *netManager() const;
    GoogleSpreadsheetsPlugin *plugin() const;
    DocumentsListService *documentsListService() const;
    SpreadsheetService *spreadsheetService() const;

private Q_SLOTS:
    void slotReplyFinished(QNetworkReply *reply);
    void slotProgress(qint64 bytesTransfered, qint64 bytesTotal);

Q_SIGNALS:
    void docListReplyFinished(QNetworkReply *reply, NetworkActions::Action netAction);
    void spreadsheetsReplyFinished(QNetworkReply *reply, NetworkActions::Action netAction);
    
private:
    class Private;
    Private * const d;
};

#endif //GOOGLESERVICECONTROLLER_H
