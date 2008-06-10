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

#ifndef KEXIWEBFORMS_DELETE_H
#define KEXIWEBFORMS_DELETE_H

struct RequestData;

namespace KexiWebForms {

    /*! @short Callback function for Delete handler */
    namespace Delete {
        /*!
         * This namespace contains a callback function bound to '/delete/*' request URI
         *
         * This function expects a uri with the following format:
         * '/delete/\<table name\>/\<pkey name\>/\<pkey value\>'
         *
         * For example: /delete/books/id/1
         * In a browser address edit field: http://localhost:8080/delete/books/id/1
         *
         * This "Query String" will be parsed by this method and used to
         * build a QuerySchema object with a 'WHERE' clause (required) and passed
         * to deleteRow to perform row deletion
         *
         * @param RequestData: a pointer to a RequestData structure @see RequestData
         * @see KexiWebForms::RequestData
         */
        void show(RequestData*);
    }
}

#endif /* KEXIWEBFORMS_DELETE_H */
