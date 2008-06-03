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

#include <QString>
#include <QStringList>

#include <KDebug>

#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "DataProvider.h"
#include "HTTPStream.h"
#include "Request.h"

#include "UpdateView.h"

namespace KexiWebForms {
    namespace UpdateView {
        void show(RequestData* req) {
			HTTPStream stream(req);
            google::TemplateDictionary dict("update");

			// FIXME: Two copies of the same string? Hm...
			QString requestedTable = Request::requestUri(req);
			QString requestedId = Request::requestUri(req);

			// FIXME: Strange things can happen here
			requestedTable.remove(0, 8);
			requestedTable.remove(requestedTable.indexOf('/'), requestedTable.size());
			dict.SetValue("TABLENAME", requestedTable.toLatin1().constData());

			// FIXME: And here too
			requestedId.remove(0, requestedId.lastIndexOf('/')+1);
			dict.SetValue("ROW", requestedId.toLatin1().constData());


			// FIXME: Can this code be improved?
			// -- Build a connection data
			if (Request::request(req, "dataSent") == "true") {
				kDebug() << "Updating field" << endl;

				QStringList fieldsList = Request::request(req, "tableFields").split("|:|");

				QString query("UPDATE ");
				query.append(requestedTable).append(" ");

				bool first = true;

				QStringListIterator fieldsIterator(fieldsList);
				while (fieldsIterator.hasNext()) {
					QString currentData = fieldsIterator.next();

					if (first)
						first = false;
					else
						query.append(", ");
					query.append("SET ").append(currentData).append("='");
					query.append(Request::request(req, currentData)).append("'");
				}

				// FIXME: That's obvious
				query.append(" WHERE id='").append(requestedId).append("'");
				dict.SetValue("DEBUG_QUERY", query.toLatin1().constData());
				gConnection->executeQuery(query);
			} else {
				kDebug() << "Showing fields" << endl;
				// Build query -- FIXME: Use QuerySchema object here, it's better
				QString query("SELECT * FROM ");
				query.append(requestedTable);
				query.append(" WHERE id='").append(requestedId).append("'");
				dict.SetValue("DEBUG_QUERY", query.toLatin1().constData());

				KexiDB::Cursor* cursor = gConnection->executeQuery(query);

				if (cursor) {
					QString formData;
					QStringList fieldsList; 
					KexiDB::QuerySchema* schema = gConnection->tableSchema(requestedTable)->query();
					// XXX: There should be only one entry...
					while (cursor->moveNext()) {
						for (int i = 0; i < cursor->fieldCount(); i++) {
							formData.append("<tr>");
							// Get field names and put them as labels in the form
							formData.append("<td>").append(schema->field(i)->captionOrName()).append("</td>");

							// Create a field
							formData.append("<td><input type=\"text\" name=\"").append(schema->field(i)->name()).append("\" value=\"");
							formData.append(cursor->value(i).toString()).append("\"/></td>");

							formData.append("</tr>");

							fieldsList << schema->field(i)->name();
						}
					}
					dict.SetValue("TABLEFIELDS", fieldsList.join("|:|").toLatin1().constData());
					dict.SetValue("FORMDATA", formData.toLatin1().constData());
				}
			} 
			
			
			//
			// Produce the final output	
			//
			std::string output;			
            google::Template* tpl = google::Template::GetTemplate("update.tpl", google::DO_NOT_STRIP);
			tpl->Expand(&output, &dict);
			stream << output << webend;
        }
    }
}
