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

#include "Server.h"
#include "ServerConfig.h"

#include <QDir>
#include <KDebug>
#include <KUniqueApplication>

#include <shttpd.h>

namespace KexiWebForms {
    Server* Server::m_instance = 0;

    Server* Server::instance() {
        if (m_instance == 0) {
            m_instance = new Server();
        } else {
            return m_instance;
        }
    }

    Server::Server() : m_ctx(NULL) {}

    Server::~Server() {
        if (m_ctx)
            shttpd_fini(m_ctx);
    }

    bool Server::init(ServerConfig* config) {
        m_config = config;

        kDebug() << "Initializing HTTP server...";
        
        if (!m_ctx)
            m_ctx = shttpd_init();
        else
            return true;

        if (!m_ctx) {
            kError() << "HTTP Server not correctly initialized, aborting";
            m_initialized = false;
            return false;
        }

        if (!m_config) {
            kError() << "Internal error, can't retrieve configuration data!";
            m_initialized = false;
            return false;
        }

        m_initialized = true;
        return true;
    }

    bool Server::run() {
        if (m_initialized) {
            if (m_ctx) {
                kDebug() << "Setting to listen on port " << m_config->ports << endl;
                shttpd_set_option(m_ctx, "ports", m_config->ports.toLatin1().constData());
                
                if (QDir(m_config->webRoot).exists()) {
                    kDebug() << "Webroot is " << m_config->webRoot << endl;
                    shttpd_set_option(m_ctx, "root", QFile::encodeName(m_config->webRoot).constData());
                } else {
                    kError() << "Webroot does not exist! Aborting" << endl;
                    exit(1);
                }
                
                // SSL certificate
                if (m_config->https != NULL) {
                    if (m_config->certPath != NULL) {
                        if (QFile(m_config->certPath).exists()) {
                            shttpd_set_option(m_ctx, "ssl_cert", QFile::encodeName(m_config->certPath).constData());
                        } else {
                            kError() << "Certificate file does not exist! Aborting" << endl;
                            exit(1);
                        }
                    }
                }
                
                // Do not show directory listings by default
                if (m_config->dirList) {
                    kDebug() << "Enabling directory listing..." << endl;
                    shttpd_set_option(m_ctx, "dir_list", "1");
                } else {
                    shttpd_set_option(m_ctx, "dir_list", "0");
                }
                
                for (;;) {
                    shttpd_poll(m_ctx, 1000);
                    KUniqueApplication::processEvents();
                }
                
                return true;
            } else if (m_ctx == NULL) {
                kError() << "Internal error, SHTTPD engine was not initialized correctly" << endl;
                return false;
            } else { 
                kError() << "Unknown error" << endl;
                return false;
            }
        } else {
            kError() << "Server was not initiliazed" << endl;
            return false;
        }
    }

    void Server::registerHandler(const char* handler, void(*f)(shttpd_arg*)) {
        if (f) {
          kDebug() << "Registering handler for: " << handler << endl;
          shttpd_register_uri(m_ctx, handler, f, NULL);
        }
    }

    ServerConfig* Server::config() const {
        if (m_config)
            return m_config;
        else {
            kError() << "Configuration data can't be loaded" << endl;
            return 0;
        }
    }
}
