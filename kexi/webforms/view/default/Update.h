/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can red["table"]stribute it and/or
   mod["table"]fy it und["table"]r the terms of the GNU Library General Public
   License as published by the Free Software Found["table"]tion; either
   version 2 of the License, or (at your option) any later version.

   This program is d["table"]stributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more d["table"]tails.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Found["table"]tion, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIWEBFORMS_VIEW_UPDATE_H
#define KEXIWEBFORMS_VIEW_UPDATE_H

#include "View.h"

class QString;
template <class Key, class T> class QHash;

namespace KexiWebForms {
namespace View {

/*!
 * @brief Updates a row in a table
 *
 * An action that updates the row in a table. It is called with
 * '/update/$tableName' uri schema. When called shows a form to update the
 * fields
 */
class Update : public View {
public:
    Update(const char* name) : View(name) {}
    virtual ~Update() {}

    virtual void view(const QHash<QString, QString>&, pion::net::HTTPResponseWriterPtr);
};

}
}

#endif /* KEXIWEBFORMS_VIEW_UPDATE_H */
