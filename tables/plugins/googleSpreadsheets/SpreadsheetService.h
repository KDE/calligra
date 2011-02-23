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


#ifndef SPREADSHEETSERVICE_H
#define SPREADSHEETSERVICE_H

#include "ServiceManager.h"

#include <QObject>


class GoogleDocument;
class QNetworkReply;


class SpreadsheetService : public QObject
{
    Q_OBJECT
    
public:
    explicit SpreadsheetService(QObject *parent = 0);
    virtual ~SpreadsheetService();
        
    bool isAuthenticated() const;
    const QString &authToken() const;
    void setAuthToken(const QString token);
    
public Q_SLOTS:
    void openSpreadsheet(GoogleDocument *doc);
    void getWorksheets();
    void getCellsForSheet(QString& cellsFeedUrl);

private Q_SLOTS:
    void handleSpreadsheetsReply(QNetworkReply *reply, NetworkActions::Action netAction);

private:
    class Private;
    Private * const d;
};

#endif //SPREADSHEETSERVICE_H
