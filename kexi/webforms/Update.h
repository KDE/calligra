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

#ifndef KEXIWEBFORMS_UPDATE_H
#define KEXIWEBFORMS_UPDATE_H

#include "Handler.h"

struct RequestData;

namespace KexiWebForms {

    /*!
     * This function is used to update a row in a given database table
     * This function uses the request URI to determine the table and the
     * row to update.
     *
     * This function is bound to the '/update/<*>' request URI pattern and
     * expects it to be in this form: /update/\<table\>/\<pkey name\>/\<pkey value\>
     *
     * This function expects POST parameter dataSent set to "true" to run
     * the real update.
     * If not given, it shows a form which allows the user to modify contents
     * of the specified row
     *
     * @param RequestData: pointer to a RequestData structure
     * @see KexiWebForms::RequestData
     */
    void updateCallback(RequestData*);
    
    class UpdateHandler : public Handler {
    public:
        UpdateHandler();
        virtual ~UpdateHandler() {}
    };
    
}

#endif /* KEXIWEBFORMS_UPDATE_H */
