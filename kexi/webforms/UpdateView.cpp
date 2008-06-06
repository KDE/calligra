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

#include <kexidb/roweditbuffer.h>
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

			KexiDB::QuerySchema schema(*gConnection->tableSchema(requestedTable));
			schema.addToWhereExpression(schema.field("id"), QVariant(requestedId));
			KexiDB::Cursor* cursor = gConnection->executeQuery(schema);


			// FIXME: Can this code be improved?
			if (Request::request(req, "dataSent") == "true" && cursor) {
				kDebug() << "Updating field" << endl;

				KexiDB::RowEditBuffer editBuffer(false);
				KexiDB::RecordData recordData;
				QStringList fieldsList = Request::request(req, "tableFields").split("|:|");
				QStringListIterator iterator(fieldsList);
				
				while (iterator.hasNext()) {
					QString curField = iterator.next();
					QVariant curValue(Request::request(req, curField));
					editBuffer.insert(curField, curValue);
					kDebug() << curField << "=" << curValue;
				}
				cursor->updateRow(recordData, editBuffer);
			} 

			kDebug() << "Showing fields" << endl;
			if (cursor) {
				QString formData;
				QStringList fieldsList; 
				KexiDB::QuerySchema* schema = gConnection->tableSchema(requestedTable)->query();
				// TODO: There should be only one entry...
				while (cursor->moveNext()) {
					for (int i = 0; i < cursor->fieldCount(); i++) {
						formData.append("<tr>");
						formData.append("<td>").append(schema->field(i)->captionOrName()).append("</td>");
						formData.append("<td><input type=\"text\" name=\"");
						formData.append(schema->field(i)->name()).append("\" value=\"");
						formData.append(cursor->value(i).toString()).append("\"/></td>");
						formData.append("</tr>");
						fieldsList << schema->field(i)->name();
					}
				}
				dict.SetValue("TABLEFIELDS", fieldsList.join("|:|").toLatin1().constData());
				dict.SetValue("FORMDATA", formData.toLatin1().constData());
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
