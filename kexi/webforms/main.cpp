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

#include <cstdlib>

#include <KDebug>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KUniqueApplication>

#include "Server.h"
#include "ServerConfig.h"

using namespace KexiWebForms;

int main(int argc, char **argv) {
    KAboutData aboutData("kwebforms", NULL, ki18n("Web Forms Daemon"),
                         "0.1", ki18n("Exports Kexi Forms to standard HTML pages"),
                         KAboutData::License_GPL_V2,
                         ki18n("(C) Copyright 2008, Lorenzo Villani"));
    
    KCmdLineArgs::init(argc, argv, &aboutData);
    
    KCmdLineOptions options;
    options.add("port <port>", ki18n("Listen port"), "8080");
    options.add("webroot <directory>", ki18n("Web Root"), ".");
    options.add("https <port>", ki18n("HTTPS listen port"));
    options.add("cert <path>", ki18n("Path to SSL certificate file"));
    options.add("key <path>", ki18n("Path to SSL key file"));
    options.add("file", ki18n("Path to Kexi database file"));
    
    KCmdLineArgs::addCmdLineOptions(options);

    KUniqueApplication::addCmdLineOptions();
    KUniqueApplication app(false);

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();


    // General set up
    ServerConfig serverConfig = {
        args->getOption("port"),
        args->getOption("webroot"),
    };

    // SSL
    if (args->isSet("https")) {
        kDebug() << "Initializing SSL...";
        if (!args->isSet("cert")) {
            kError() << "You must specify both certificate and key file in order to use SSL support";
            exit(1);
        } else {
            serverConfig.certPath = args->getOption("cert");
        }
        serverConfig.https = args->getOption("https");
    }

    Server server;    
    if (server.run(serverConfig))
        return 0;
    else
        return 1;
}

