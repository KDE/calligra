/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIPARTDATASOURCE_H
#define KEXIPARTDATASOURCE_H

class KexiProject;
namespace KexiDB
{
class FieldList;
class Cursor;
}

namespace KexiPart
{
class DataSourcePrivate;
class Item;
class Part;

/**
 * this class provides a datasource framework for e.g. tables and queries
 * using this framework one can query for
 *  - a list of datasources
 *  - the fileds in datasources
 *  - variables (e.g. query variables)
 */
class KEXICORE_EXPORT DataSource
{
public:
    DataSource(Part *part);
    virtual ~DataSource();

    /**
     * @returns a list of fileds for the datasource
     * @arg id is the document id for the source
     */
    virtual KexiDB::FieldList *fields(KexiProject *project, const KexiPart::Item &i) = 0;

    /**
     * @returns the cursor
     */
    virtual KexiDB::Cursor *cursor(KexiProject *project, const KexiPart::Item &i, bool buffer) = 0;

    /**
     * @returns the part providing this datasource
     */
    Part *part() const;

private:
    DataSourcePrivate *d;
};

}

#endif

