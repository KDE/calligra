/* This file is part of the KDE project
   Copyright (C) 2009-2010 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef SIMPLEFIELD_H
#define SIMPLEFIELD_H

#include <QString>
#include <kexidb/queryschema.h>
#include <KoXmlWriter.h>

class SimpleField
{
public:
    SimpleField();
    SimpleField(KexiDB::QueryColumnInfo *);
    void save(KoXmlWriter &writer);

    QString name;
    QString type;
    bool pkey;
    bool notnull;
};

#endif // SIMPLEFIELD_H
