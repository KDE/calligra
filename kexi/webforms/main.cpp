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

#include <KDebug>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KUniqueApplication>

#include <QFile>

#include <google/template.h>
#include <pion/net/WebServer.hpp>

#include "PatternServer.h"
#include "ShutdownManager.hpp"

//#include "Server.h"
//#include "ServerConfig.h"
#include "DataProvider.h"

#include "IndexService.h"
//#include "Query.h"
#include "CRUD.h"

using namespace KexiWebForms;
using namespace pion::net;


int main(int argc, char **argv) {
    KCmdLineArgs::init(argc, argv,
                       new KAboutData ("kwebforms", NULL, ki18n("Web Forms Daemon"),
                                       "0.1", ki18n("Exports Kexi Forms to standard HTML pages"),
                                       KAboutData::License_GPL_V2,
                                       ki18n("(C) Copyright 2008, Lorenzo Villani")));
     
    KCmdLineOptions options;
    options.add("port <port>", ki18n("Listen port"), "8080");
    options.add("webroot <directory>", ki18n("Web Root (used also as template root directory)"), "./");
    options.add("https <port>", ki18n("HTTPS listen port"));
    options.add("cert <path>", ki18n("Path to SSL certificate file"));
    options.add("key <path>", ki18n("Path to SSL key file"));
    options.add("dirlist", ki18n("Enable directory listing"));
    options.add("file <file>", ki18n("Path to Kexi database file"));
    
    KCmdLineArgs::addCmdLineOptions(options);

    KUniqueApplication::addCmdLineOptions();
    KUniqueApplication app(true);

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    if (args->isSet("file")) {
        if (!initDatabase(args->getOption("file"))) {
            kError() << "Something went wrong while initializing database..." << endl;
            return 1;
        }
    } else {
        kError() << "You must specifiy a Kexi file path";
        return 1;
    }

    // Set template root directory equal to root directory
    google::Template::SetTemplateRootDirectory(QFile::encodeName(args->getOption("webroot")).constData());

    pion::net::PatternServer server(8080);
    
    IndexService indexService("index.tpl");
    CreateService createService("create.tpl");
    ReadService readService("read.tpl");
    UpdateService updateService("update.tpl");
    DeleteService deleteService("delete.tpl");
    
    server.addService("/", &indexService);
    server.addService("/create/(.*)", &createService);
    server.addService("/read/(.*)", &readService);
    server.addService("/update/(.*)", &updateService);
    server.addService("/delete/(.*)", &deleteService);
    server.start();
    
    main_shutdown_manager.wait();

    return 0;
}

