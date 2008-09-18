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

#include "../IView.h"

class QString;

namespace KexiWebForms {
namespace View {

/*!
 * @brief Output the database content as a JSON stream
 *
 * This service inspects the database to retrieve table and query names and
 * produces a JSON document to be loaded from an ExtJS tree view
 */
class Objects : public IView {
public:
    Objects() {}
    virtual ~Objects() {}

    virtual void view(const QHash<QString, QString>&, pion::net::HTTPResponseWriterPtr);
};

}
}

#endif /* KEXIWEBFORMS_VIEW_EXTJS_OBJECT_H */
