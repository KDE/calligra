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

#include <QDir>
#include <KDebug>
#include <KLocale>
#include <KGlobal>
#include <KUniqueApplication>

#include <google/template.h>

#include <shttpd.h>

#include "Server.h"
#include "Request.h"

namespace KexiWebForms {

    class ServerStatic : public Server {
    public:
        ServerStatic() : Server() {}
    };
    
K_GLOBAL_STATIC( ServerStatic, s_instance )

    Server* Server::instance() {
        return s_instance;
    }

    Server::Server() : m_ctx(NULL) {}

    Server::~Server() {
        closeCtx();
    }

    void Server::closeCtx()
    {
        if (m_ctx)
            shttpd_fini(m_ctx);
        m_ctx = 0;
    }

    bool Server::init(const ServerConfig& config) {
        kDebug() << "Initializing HTTP server...";

        closeCtx();
        m_ctx = shttpd_init();
        if (!m_ctx) {
            kError() << "HTTP Server not correctly initialized, aborting";
            return false;
        }

        kDebug() << "Setting to listen on port " << config.ports << endl;
        shttpd_set_option(m_ctx, "ports", config.ports.toUtf8().constData());

        if (QDir(config.webRoot).exists()) {
            kDebug() << "Webroot is " << config.webRoot << endl;
            shttpd_set_option(m_ctx, "root", QFile::encodeName(config.webRoot).constData());
        } else {
            kError() << i18n("Webroot %1 does not exist! Aborting", config.webRoot) << endl;
            closeCtx();
            return false;
        }

        // SSL certificate
        if (!config.https.isEmpty()) {
            if (!config.certPath.isEmpty()) {
                if (QFile(config.certPath).exists()) {
                    shttpd_set_option(m_ctx, "ssl_cert", QFile::encodeName(config.certPath).constData());
                } else {
                    kError() << "Certificate file does not exist! Aborting" << endl;
                    closeCtx();
                    return false;
                }
            }
        }

        // Do not show directory listings by default
        if (config.dirList) {
            kDebug() << "Enabling directory listing..." << endl;
            shttpd_set_option(m_ctx, "dir_list", "1");
        } else {
            shttpd_set_option(m_ctx, "dir_list", "0");
        }

        m_config = config;
        return true;
    }

    bool Server::run() {
        if (!m_ctx) {
            kError() << "Server was not initiliazed" << endl;
            return false;
        }
        for (;;) {
            shttpd_poll(m_ctx, 1000);
            KUniqueApplication::processEvents();
            /// @note Tricky way to clean cache on template updates...
            google::Template::ReloadAllIfChanged();
        }
    }

    void Server::registerHandler(const char* handler, void(*f)(RequestData*)) {
        if (f) {
          kDebug() << "Registering handler for: " << handler << endl;
          shttpd_register_uri(m_ctx, handler, f, NULL);
        }
    }

    const ServerConfig& Server::config() const {
        return m_config;
    }
}
