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
#include "HTTPStream.h"
#include "DataProvider.h"
#include "TemplateProvider.h"

#include "Index.h"


/*! @short Kexi Web Forms namespace
 * This namespace contains code related to Kexi Web Forms daemon
 *
 * Kexi Web Forms daemon allows users to modify a Kexi database
 * even if they don't have Kexi Installed
 */
namespace KexiWebForms {
    void indexCallback(RequestData* req) {
        HTTPStream stream(req);
        google::TemplateDictionary* dict = initTemplate("index.tpl");

        QString tables;
        for (int i = 0; i < gConnection->tableNames().size(); ++i) {
            tables.append("<li><a href=\"/read/").append(gConnection->tableNames().at(i));
            tables.append("\">").append(gConnection->tableNames().at(i)).append("</a></li>");
        }
        dict->SetValue("TABLES", tables.toUtf8().constData());

        renderTemplate(dict, stream);
    }

    // Index Handler
    IndexHandler::IndexHandler() : Handler(indexCallback) {}

}
