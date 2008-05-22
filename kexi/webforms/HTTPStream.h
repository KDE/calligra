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

#ifndef KEXI_WEBFORMS_HTTPSTREAM_H
#define KEXI_WEBFORMS_HTTPSTREAM_H

#include <shttpd.h>

#include <string>

#include <QString>

namespace KexiWebForms {

    typedef struct shttpd_arg Request;

    // Create a new type
    typedef struct {} HTTPEndOfStream;

    class HTTPStream {
    public:
        HTTPStream(Request*);
        HTTPStream& operator<<(const char*);
        HTTPStream& operator<<(const std::string&);
        HTTPStream& operator<<(const QString&);
        // TODO: review this
        void operator<<(HTTPEndOfStream);
    private:
        bool m_headerModified;
        QString m_contentbuf;
        Request* m_request;
    };

    extern HTTPEndOfStream webend;
}

#endif
