/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatabaseSourceTable.h"

#include <QString>

using namespace Calligra::Sheets;

class DatabaseSourceTable::Private
{
public:
    QString tableName;
};

DatabaseSourceTable::DatabaseSourceTable()
        : d(new Private)
{
}

DatabaseSourceTable::~DatabaseSourceTable()
{
    delete d;
}
