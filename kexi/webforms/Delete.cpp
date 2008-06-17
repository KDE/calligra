/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <string>

#include <QString>

#include <KDebug>

#include <kexidb/utils.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "DataProvider.h"
#include "HTTPStream.h"
#include "Request.h"

#include "Delete.h"

namespace KexiWebForms {
    void deleteCallback(RequestData* req) {
        HTTPStream stream(req);
        google::TemplateDictionary dict("delete");


        /*
         * Retrieve requested table and pkey
         */
        QStringList queryString = Request::requestUri(req).split("/");
        QString requestedTable = queryString.at(2);
        QString pkeyName = queryString.at(3);
        QString pkeyValue = queryString.at(4);
        dict.SetValue("TABLENAME", requestedTable.toLatin1().constData());

        kDebug() << "Trying to delete row..." << endl;
        if (KexiDB::deleteRow(*gConnection, gConnection->tableSchema(requestedTable),
                              pkeyName, pkeyValue)) {
            dict.ShowSection("SUCCESS");
            dict.SetValue("MESSAGE", "Row deleted successfully");
        } else {
            dict.ShowSection("ERROR");
            dict.SetValue("MESSAGE", "Error while trying to delete row!");
        }

        // Render template
        std::string output;
        google::Template* tpl = google::Template::GetTemplate("delete.tpl", google::DO_NOT_STRIP);
        tpl->Expand(&output, &dict);
        stream << output << webend;
    }


    // Delete Handler
    DeleteHandler::DeleteHandler() : Handler(deleteCallback) {}
}
