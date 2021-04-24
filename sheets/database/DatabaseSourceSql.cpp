/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatabaseSourceSql.h"

#include <QString>

using namespace Calligra::Sheets;

class DatabaseSourceSql::Private
{
public:
    Private() : parseSqlStatement(false) {}

    QString sqlStatement;
    bool parseSqlStatement;
};

DatabaseSourceSql::DatabaseSourceSql()
        : d(new Private)
{
}

DatabaseSourceSql::~DatabaseSourceSql()
{
    delete d;
}
