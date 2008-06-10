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

#ifndef KEXIWEBFORMS_SERVER_H
#define KEXIWEBFORMS_SERVER_H

#include "Request.h"
#include "ServerConfig.h"

struct shttpd_ctx;

namespace KexiWebForms {

    /*!
     * @short Wrapper for shttpd
     * This class is basically a C++ wrapper around shttpd
     * shttpd has a pure C api, having it wrapped in C++ layer seemed
     * convenient, the only drawback is that we still need to use
     * function pointers for certain tasks
     *
     * This class is a Singleton
     */
    class Server {
    public:
        ~Server();

        /*!
         * Return the unique instance of Server
         * @return Pointer to Server object
         */
        static Server* instance();

        /*!
         * Initialize the server
         * @param ServerConfig*: a pointer to the ServerConfig structure
         * @return a boolean indicating if initialization has succeeded
         */
        bool init(ServerConfig*);

        /*!
         * Run the server main loop, this is a blocking function
         * @return boolean true if something went well, false on errors
         */
        bool run();

        // FIXME: Do not expose shttpd data structures
        /*!
         * Register a new URI handler with a given URI pattern and
         * callback function
         * @param const char*: the URI pattern
         * @param void(*f)(RequestData*): the callback function
         */
        void registerHandler(const char*, void(*f)(RequestData*));

        /*!
         * @return a pointer to the currently used ServerConfig structure
         */
        ServerConfig* config() const;
    protected:
        Server();
    private:
        static Server* m_instance;
        bool m_initialized;
        ServerConfig* m_config;
        shttpd_ctx* m_ctx;
    };

}

#endif /* KEXIWEBFORMS_SERVER_H */
