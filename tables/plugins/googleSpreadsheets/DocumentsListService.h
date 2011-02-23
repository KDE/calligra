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

//TODO
/*
    * adding folder-handling
    * create new Document
    * adding support for revisions
*/


#ifndef DOCUMENTSLISTSERVICE_H
#define DOCUMENTSLISTSERVICE_H

#include "ServiceManager.h"

#include <QObject>


class GoogleDocument;
class QNetworkReply;
class KTemporaryFile;


class DocumentsListService : public QObject
{
    Q_OBJECT
    
public:
    explicit DocumentsListService(QObject *parent = 0);
    virtual ~DocumentsListService();
    
    bool isAuthenticated() const;
    const QString &authToken() const;
    void setAuthToken(QString token);
    GoogleServices::DocType docType() const;
    
    const QList<GoogleDocument*> &documentsList() const;
    GoogleDocument *currentDocument() const ;
    const QStringList formatsForExport(const QString &docType);

public Q_SLOTS:
    void getDocumentsListMetadata();
    void getDocumentsList();
    void downloadDocument(int entryNr, QString format);
    void openDocument(int entryNr);    
    void uploadDocument(QString filePath);
    //void requestRenameDocument(int entryNr);
    void deleteDocument(int entryNr);
    void updateOpenDocument(QByteArray data, QString &fileExt, bool force);
    
    void setDocType(int type);
    
private Q_SLOTS:
    void handleDocListReply(QNetworkReply *reply, NetworkActions::Action netAction);

Q_SIGNALS:
    void updateDocListDialog(QList<GoogleDocument*> &entryList, QString &title);
    void waitForDocList(bool b);
    
private: 
    class Private;
    Private * const d;
};


#endif //DOCUMENTSLISTSERVICE_H
