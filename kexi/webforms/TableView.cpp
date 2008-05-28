/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>
   Time-stamp: <2008-05-28 16:00:27 lorenzo>

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
#include <sstream>

#include <QString>
#include <QPointer>

#include <google/template.h>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "Request.h"
#include "Server.h"
#include "HTTPStream.h"
#include "DataProvider.h"

#include "TableView.h"

namespace KexiWebForms {
    namespace TableView {
        void show(RequestData* req) {
            HTTPStream stream(req);
            google::TemplateDictionary dict("table");

            QString requestedPage = Request::requestUri(req);
            // FIXME: Mhhh, nasty things can happen with that
            requestedPage.remove(0, 6);
            dict.SetValue("TABLENAME", requestedPage.toLatin1().constData());

            // rough code to access table data...
            // FIXME: Change this piece of code...
            QString query("SELECT * FROM ");
            query.append(requestedPage);

            KexiDB::Cursor* cursor = gConnection->executeQuery(query);

            if (cursor) {
                std::ostringstream tabledata;
                while (cursor->moveNext()) {
                    tabledata << "<tr>";
                    for (int i = 0; i < cursor->fieldCount(); i++) {
                        tabledata << "<td>" << cursor->value(i).toString().toLatin1().constData() << "</td>";
                    }
                    tabledata << "</tr>";
                }
                dict.SetValue("TABLEDATA", tabledata.str().c_str());
            } else {
                dict.SetValue("TABLEDATA", "Error in " __FILE__); // mhh...
            }
            
            std::ostringstream file;
            file << Server::instance()->config()->webRoot.toLatin1().constData() << "/table.tpl";
            google::Template* tpl = google::Template::GetTemplate(file.str(), google::DO_NOT_STRIP);
            
            std::string output;
            tpl->Expand(&output, &dict);

            stream << output << webend;
        }
    }
}
