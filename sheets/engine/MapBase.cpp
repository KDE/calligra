/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "MapBase.h"

#include <QObject>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN MapBase::Private
{
public:
    Private(MapBase *map);
    ~Private() {}
private:
    MapBase *m_map;
};

MapBase::Private::Private (MapBase *map)
    : m_map(map)
{

}




MapBase::MapBase() :
    d(new Private(this))
{
}

MapBase::~MapBase()
{
}

