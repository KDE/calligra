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

#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "DataProvider.h"
#include "HTTPStream.h"
#include "Request.h"

#include "Delete.h"

namespace KexiWebForms {
    namespace Delete {
        void show(RequestData* req) {
            HTTPStream stream(req);
            google::TemplateDictionary dict("delete");


            /*
             * Retrieve requested table and pkey
             */
            QStringList queryString = Request::requestUri(req).split("/");
            QString requestedTable = queryString.at(2);
            QString pkeyName = queryString.at(3);
            QString pkeyValue = queryString.at(4);

            /// @fixme: It seems to crash when table doesn't exist
            KexiDB::QuerySchema schema(*gConnection->tableSchema(requestedTable));
            schema.addToWhereExpression(schema.field(pkeyName), QVariant(pkeyValue));
            
            KexiDB::Cursor* cursor = gConnection->executeQuery(schema);
            cursor->moveNext();
            KexiDB::RecordData data(cursor->fieldCount());
            if (cursor->deleteRow(data)) {
                dict.SetValue("ERROR", "row deleted");
            } else {
                kError() << " ========== ERROR ============" << endl;
                gConnection->debugError();
                dict.SetValue("ERROR", cursor->serverErrorMsg().toLatin1().constData());
            }
            //dict.SetValue("DEBUG_QUERY", gConnection->debugString().toLatin1().constData());

            // Render template
            std::string output;
            google::Template* tpl = google::Template::GetTemplate("delete.tpl", google::DO_NOT_STRIP);
            tpl->Expand(&output, &dict);
            stream << output << webend;
        }
    }
}
