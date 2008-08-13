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

#include <pion/net/HTTPResponseWriter.hpp>


#include "../../model/Database.h"
#include "TemplateProvider.h"

#include "Delete.h"

using namespace pion::net;

namespace KexiWebForms { // begin namespace KexiWebForms
namespace View {         // begin namespace View
    
    void Delete::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
        QString requestedTable(d["kwebforms__table"]);
        QString pkeyName(d["kwebforms__pkey"]);
        QString pkeyValue(d["kwebforms__pkeyValue"]);
        
        setValue("TABLENAME", requestedTable);

        if (KexiWebForms::Model::Database::deleteRow(requestedTable, pkeyName, pkeyValue)) {
            m_dict->ShowSection("SUCCESS");
            setValue("MESSAGE", "Row deleted successfully");
        } else {
            m_dict->ShowSection("ERROR");
            /// @todo retrieve proper error message
            setValue("MESSAGE", "Error while trying to delete row!");
        }
        
        renderTemplate(m_dict, writer);
    }

} // end namespace View
} // end namespace KexiWebForms
