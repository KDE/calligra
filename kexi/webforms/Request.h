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

#ifndef KEXIWEBFORMS_REQUEST_H
#define KEXIWEBFORMS_REQUEST_H

#include <shttpd.h>

class QString;

namespace KexiWebForms {
    /*! Just another name for the shttpd_arg structure */
    typedef struct shttpd_arg RequestData;

    /*!
     * @short Utilities functions
     *
     * This namespace contains some utilities functions to get
     * actual Request URI and to retrieve variables passed with
     * GET or POST requests
     */
    namespace Request {
        /*!
         * Retrieve the value of a POST or GET parameter
         *
         * @param RequestData a pointer to a RequestData structure
         *
         * @return The value of the chosen parameter, an empty string
         * if it can't be found
         */
        QString request(RequestData*, const char*);

        /*!
         * Same as above, but accepting a const QString& instead of const char*
         */
        QString request(RequestData*, const QString&);

        /*!
         * Retrieve the request URI, for example if in the browser you write
         * http://localhost:8080/delete/books/id/1
         * the request URI would be '/delete/books/id/1'
         *
         * @param RequestData pointer to a RequestData structure
         *
         * @return a QString containing the request URI
         */
        QString requestUri(RequestData*);
    }
}

#endif /* KEXIWEBFORMS_REQUEST_H */
