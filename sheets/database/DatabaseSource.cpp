/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatabaseSource.h"

#include <QString>

using namespace Calligra::Sheets;

class DatabaseSource::Private
{
public:
    QString databaseName;
};

DatabaseSource::DatabaseSource()
        : d(new Private)
{
}

DatabaseSource::~DatabaseSource()
{
    delete d;
}
