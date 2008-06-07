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
#include <google/template.h>

#include "Request.h"
#include "Server.h"
#include "DataProvider.h"
#include "HTTPStream.h"

#include "Index.h"

namespace KexiWebForms {

    namespace Index {

        void show(RequestData* req) {
            HTTPStream stream(req);
            google::TemplateDictionary dict("index");

            dict.SetValue("TITLE", gConnection->data()->fileName().toLatin1().constData());

            QString tables;
            for (int i = 0; i < gConnection->tableNames().size(); ++i) {
                tables.append("<li><a href=\"/view/").append(gConnection->tableNames().at(i));
                tables.append("\">").append(gConnection->tableNames().at(i)).append("</a></li>");
            }
            dict.SetValue("TABLES", tables.toLatin1().constData());


			// Render the template
			std::string output;	
            google::Template* tpl = google::Template::GetTemplate("index.tpl", google::DO_NOT_STRIP);
            tpl->Expand(&output, &dict);
            stream << output << webend;
        }

    }

}
