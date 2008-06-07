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

#include <QString>

#include <shttpd.h>

#include "Request.h"

namespace KexiWebForms {
    namespace Request {
        /**
         * Create a QMap containing all GET/POST request data
         * and make it accessible by name
         */
        QString request(RequestData* req, const char* name) {
            char value[4096];
            shttpd_get_var(name, req->in.buf, req->in.len, value, sizeof(value));
            return QString(value);
        }

        QString request(RequestData* req, const QString& name) {
            return request(req, name.toLatin1().constData());
        }

        QString requestUri(RequestData* req) {
            return QString(shttpd_get_env(req, "REQUEST_URI"));
        }
    }
}
