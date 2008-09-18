/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIQUERYPARAMETERS_H
#define KEXIQUERYPARAMETERS_H

#include <kexi_export.h>
#include <kexidb/queryschema.h>

//! @short Utilities providing GUI for getting query parameters
class KEXIEXTWIDGETS_EXPORT KexiQueryParameters
{
public:
    /*! Asks for query parameters using KInputDialog, one dialog per query parameter
     (see @ref KexiDB::QuerySchema::parameters()). The type of each dialog depends
     on the type of query parameter.
     \return list of values obtained from the user
     \a ok is set to true on success and to false on failure. */
    //! @todo do not use KInputDialog - switch to more powerful custom dialog
    //! @todo offer option to display one dialog (form) with all the parameters
    //! @todo support more types (using validators)
    //! @todo support defaults
    //! @todo support validation rules, e.g. min/max value, unsigned
    //! @todo support Enum type (list of strings, need support for keys and user-visible strings)
    static QList<QVariant> getParameters(QWidget *parent, const KexiDB::Driver &driver,
                                         KexiDB::QuerySchema& querySchema, bool &ok);
};

#endif // KEXIDBCONNECTIONWIDGET_H
