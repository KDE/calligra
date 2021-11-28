/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatabaseManager.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN DatabaseManager::Private
{
public:
    static int s_id;
};

int DatabaseManager::Private::s_id = 1;


DatabaseManager::DatabaseManager()
        : d(new Private)
{
}

DatabaseManager::~DatabaseManager()
{
    delete d;
}

QString DatabaseManager::createUniqueName() const
{
    return "database-" + QString::number(Private::s_id++);
}


