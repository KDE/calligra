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

#ifndef KEXIWEBFORMS_VIEW_EXTJS_OBJECT_H
#define KEXIWEBFORMS_VIEW_EXTJS_OBJECT_H

#include <QHash>

#include <pion/net/HTTPResponseWriter.hpp>

#include "View.h"

class QString;

namespace KexiWebForms {

namespace View {

    /**
     * @brief WebService handling delete operations
     *
     * This service is responsible to delete a particular record
     */
    class Objects : public View {
    public:
        Objects() {}
        virtual ~Objects() {}

        virtual void view(const QHash<QString, QString>&, pion::net::HTTPResponseWriterPtr);
    };

}
}

#endif /* KEXIWEBFORMS_VIEW_EXTJS_OBJECT_H */
