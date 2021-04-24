/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatabaseSourceQuery.h"

#include <QString>

using namespace Calligra::Sheets;

class DatabaseSourceQuery::Private
{
public:
    QString queryName;
};

DatabaseSourceQuery::DatabaseSourceQuery()
        : d(new Private)
{
}

DatabaseSourceQuery::~DatabaseSourceQuery()
{
    delete d;
}
