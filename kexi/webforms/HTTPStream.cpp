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
#include "HTTPStream.h"

namespace KexiWebForms {

    HTTPStream::HTTPStream(RequestData* req) : m_request(req) {}

    HTTPStream& HTTPStream::operator<<(const char* str) {
        m_contentbuf.append(str);
        return *this;
    }

    HTTPStream& HTTPStream::operator<<(const std::string& str) {
        m_contentbuf.append(str.c_str());
        return *this;
    }

    HTTPStream& HTTPStream::operator<<(const QString& str) {
        m_contentbuf.append(str);
        return *this;
    }

    void HTTPStream::operator<<(HTTPEndOfStream eos) {
        if (!m_headerModified) {
            shttpd_printf(m_request, "%s", "HTTP/1.1 200 OK\r\n");
            shttpd_printf(m_request, "%s", "Content-Type: text/html\r\n\r\n");
        }
        shttpd_printf(m_request, "%s", m_contentbuf.toUtf8().constData());
        m_request->flags |= SHTTPD_END_OF_OUTPUT;
    }


    HTTPEndOfStream webend;
}
