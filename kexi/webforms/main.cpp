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

#include <QFile>
#include <QVariant>

#include <KDebug>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KUniqueApplication>

#include <pion/PionConfig.hpp>
#include <pion/PionPlugin.hpp>

#include <pion/net/PionUser.hpp>
#include <pion/net/WebServer.hpp>
#include <pion/net/HTTPBasicAuth.hpp>
#include <pion/net/HTTPCookieAuth.hpp>


#include "ShutdownManager.hpp"

#include "model/DataProvider.h"

#include "controller.h.in"

#include "BlobService.h"
#include "auth/Authenticator.h"

using namespace pion::net;
using namespace KexiWebForms;

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

    pion::net::WebServer server(QVariant(args->getOption("port")).toUInt());

    // Plugins
    try {
        pion::PionPlugin::addPluginDirectory(PION_PLUGINS_DIRECTORY);
        server.loadService("/f", "FileService");
        server.setServiceOption("/f", "directory", args->getOption("webroot").toLatin1().constData());
        server.setServiceOption("/f", "cache", "0");
        server.setServiceOption("/f", "scan", "0");
    } catch (pion::PionPlugin::DirectoryNotFoundException&) {
        kError() << "Default plug-ins directory does not exist!" << endl;
        return 1;
    } catch (pion::net::WebServer::ServiceNotFoundException&) {
        kError() << "Could not find FileService, are you have the right plugins in " PION_PLUGINS_DIRECTORY << endl;
        return 1;
    }

    // Other Services
    BlobService blobService;
    
    // Auth
    PionUserManagerPtr userMan(new PionUserManager());
    HTTPAuthPtr auth(new HTTPCookieAuth(userMan));
    KexiWebForms::Auth::Authenticator::init(auth);
    
    // Bind controller
    Controller controller;
    server.addService("/", &controller);
    
    // Restrict CRUD operations (and BlobService) to registered users
    // filtered using our permissions manager
    auth->addRestrict("/read");
    auth->addRestrict("/create");
    auth->addRestrict("/update");
    auth->addRestrict("/delete");
    auth->addRestrict("/blob");
    
    // File and blob service
    server.addService("/blob", &blobService);
    
    server.start();
    server.setAuthentication(auth);
    main_shutdown_manager.wait();

    /// @todo don't always return 0
    return 0;
}

