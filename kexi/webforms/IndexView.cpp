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
#include <sstream>
#include <google/template.h>
#include <shttpd.h>

#include "Server.h"
#include "IndexView.h"
#include "DataProvider.h"

namespace KexiWebForms {

    namespace IndexView {

        void show(struct shttpd_arg* arg) {
            shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
            shttpd_printf(arg, "%s", "Content-Type: text/html\r\n\r\n");

            google::TemplateDictionary dict("index");
            dict.SetValue("TITLE", gConnection->data()->fileName().toLatin1().constData());

            std::ostringstream tables;
            // FIXME: Ugly usage of temporary objects
            for (int i = 0; i < gConnection->tableNames().size(); ++i)
                tables << "<li>" << gConnection->tableNames().at(i).toLatin1().constData() << "</li>";
            dict.SetValue("TABLES", tables.str());

            // FIXME: That's horrible
            std::ostringstream file;
            file << Server::instance()->config()->webRoot.toLatin1().constData() << "/index.tpl";

            google::Template* tpl = google::Template::GetTemplate(file.str(), google::DO_NOT_STRIP);
            std::string output;
            tpl->Expand(&output, &dict);
            shttpd_printf(arg, "%s", output.c_str());

            arg->flags |= SHTTPD_END_OF_OUTPUT;
        }

    }

}
