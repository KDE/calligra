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

#ifndef KEXIWEBFORMS_HTTPSTREAM_H
#define KEXIWEBFORMS_HTTPSTREAM_H

#include <QString>
#include <string>

#include "Request.h"

struct RequestData;

namespace KexiWebForms {

    /*!
     * This empty structure is used to mark the end of an HTTP Stream
     */
    typedef struct {} HTTPEndOfStream;

    /*!
     * HTTPStream exposes shttpd_printf functions as a
     * C++ stream
     *
     * @todo implement operators to allow modifications
     * of HTTP headers to be sent back the client
     */
    class HTTPStream {
    public:
        /*!
         * The constructor
         * @param RequestData a pointer to a RequestData structure
         * @see KexiWebForms::RequestData
         */
        HTTPStream(RequestData*);

        /*!
         * Writes const* char on the stream
         * @param const char* the string to be written
         * @return an HTTPStream reference
         */
        HTTPStream& operator<<(const char*);

        /*!
         * Writes a C++ std::string on the stream
         * @param const std::string& the string to be written
         * @return an HTTPStream reference
         */
        HTTPStream& operator<<(const std::string&);

        /*!
         * Writes const* char on the stream
         * @param const char* the string to be written
         * @return an HTTPStream reference
         */
        HTTPStream& operator<<(const QString&);

        /*!
         * Stop buffering data and send it to the client
         *
         * @param HTTPEndOfStream an HTTPEndOfStream object
         * @see KexiWebForms::HTTPEndOfStream
         *
         * @todo we return void to this function just to ensure that
         * we can't append more data, it's not correct and probably not
         * portable, investigate
         */
        void operator<<(HTTPEndOfStream);
    private:
        bool m_headerModified;
        QString m_contentbuf;
        RequestData* m_request;
    };

    
    /*!
     * Defines a globally usable HTTPEndOfStream object
     */ 
    extern HTTPEndOfStream webend;
}

#endif /* KEXIWEBFORMS_HTTPSTREAM_H */
