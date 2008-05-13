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

// FIXME: remove this
#include <shttpd.h>


#include "DataProvider.h"

using namespace KexiWebForms;

// FIXME: Testing
static void indexH(struct shttpd_arg* arg) {
	shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
	shttpd_printf(arg, "%s", "Content-Type: text/html\r\n\r\n");
	shttpd_printf(arg, "%s", "<html><body>");
	shttpd_printf(arg, "%s", "<h1>Kexi Web Forms daemon</h1></body></html>");
	arg->flags |= SHTTPD_END_OF_OUTPUT;
}

int main(int argc, char **argv) {
    KCmdLineArgs::init(argc, argv,
                       new KAboutData ("kwebforms", NULL, ki18n("Web Forms Daemon"),
                                       "0.1", ki18n("Exports Kexi Forms to standard HTML pages"),
                                       KAboutData::License_GPL_V2,
                                       ki18n("(C) Copyright 2008, Lorenzo Villani")));
    
    KCmdLineOptions options;
    options.add("port <port>", ki18n("Listen port"), "8080");
    options.add("webroot <directory>", ki18n("Web Root"), ".");
    options.add("https <port>", ki18n("HTTPS listen port"));
    options.add("cert <path>", ki18n("Path to SSL certificate file"));
    options.add("key <path>", ki18n("Path to SSL key file"));
    options.add("dirlist", ki18n("Enable directory listing"));
    options.add("file <file>", ki18n("Path to Kexi database file"));
    
    KCmdLineArgs::addCmdLineOptions(options);

    KUniqueApplication::addCmdLineOptions();
    KUniqueApplication app(false);

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();


    // General set up
    ServerConfig serverConfig = {
        args->getOption("port"),
        args->getOption("webroot"),
        args->isSet("dirlist")
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

    if (args->isSet("file")) {
        serverConfig.dbPath = args->getOption("file");
    } else {
        kError() << "You must specifiy a Kexi file path";
        exit(1);
    }

    Server server(&serverConfig);

    // Setup server
    // FIXME: This code is put here just for testing
    // I need to define a better API to handle this stuff
    server.registerHandler("/", indexH);

    // Test model
    // FIXME: This code is here just for testing
    DataProvider provider(serverConfig.dbPath);
    kDebug() << "Databases: " << provider.getTables();

    if (server.run())
        return 0;
    else
        return 1;
}

