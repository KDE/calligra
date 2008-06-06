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

            QString requestedTable = Request::requestUri(req);
            // FIXME: Mhhh, nasty things can happen with that
            requestedTable.remove(0, 6);
            dict.SetValue("TABLENAME", requestedTable.toLatin1().constData());

			QString tableData;
			KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);
            KexiDB::Cursor* cursor = gConnection->executeQuery(*tableSchema);

            if (cursor) {
				// Create labels with field name
				tableData.append("<tr>");
				for (int i = 0; i < cursor->fieldCount(); i++) {
					tableData.append("<td><strong>");
					tableData.append(tableSchema->query()->field(i)->captionOrName());
					tableData.append("</strong></td>");
				}
				tableData.append("</tr>");

				// Fill the table
				int row = 1;
                while (cursor->moveNext()) {
                    tableData.append("<tr>");
                    for (int i = 0; i < cursor->fieldCount(); i++) {
                        tableData.append("<td>");
                        tableData.append(cursor->value(i).toString());
                        tableData.append("</td>");
                    }
					tableData.append("<td><a href=\"/update/").append(requestedTable);
					tableData.append("/").append(QVariant(row).toString()).append("\">Edit</a></td>");
                    tableData.append("</tr>");
					row++;
                }
                dict.SetValue("TABLEDATA", tableData.toLatin1().constData());
            } else {
                dict.SetValue("TABLEDATA", "Error in " __FILE__); // mhh...
            }


			// Render the template
			std::string output;
            google::Template* tpl = google::Template::GetTemplate("table.tpl", google::DO_NOT_STRIP);
            tpl->Expand(&output, &dict);
            stream << output << webend;
        }
    }
}
