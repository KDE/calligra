/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "person.h"
#include "person_p.h"


namespace Kasten2
{

static const struct EgoDataStruct { const char* name; const char* faceIconName; }
EgoData[] =
{
    {"Ego", "face-smile"},
    {"Joe Developer", "face-surprise"},
    {"Konqui", "face-laugh"},
    {"Hans Entwickler", "user-identity"}
};
static const int lastEgoDataIndex = sizeof(EgoData)/sizeof(EgoDataStruct) - 1;
static int currentEgoDataIndex = 0;

Person Person::createEgo()
{
    const EgoDataStruct* currentEgoData = &EgoData[currentEgoDataIndex];
    const Person result( QLatin1String(currentEgoData->name),
                         KIcon(QLatin1String(currentEgoData->faceIconName)) );
//     if( currentEgoDataIndex < lastEgoDataIndex )
//         ++currentEgoDataIndex;
    return result;
}

void Person::setEgoId( int egoId )
{
    if( lastEgoDataIndex < egoId )
        egoId = lastEgoDataIndex;
    currentEgoDataIndex = egoId;
}


Person::Person( const QString& name, const KIcon& faceIcon )
 : d( new Private(name,faceIcon) )
{
}
Person::Person()
 : d( new Private(QString(),KIcon()) )
{
}
Person::Person( const Person& other )
 : d( other.d )
{
}
bool Person::operator==( const Person& other ) const
{
    return (name() == other.name()) && !name().isEmpty();
}

Person& Person::operator=( const Person& other )
{
    d = other.d;
    return *this;
}

QString Person::name()   const { return d->name(); }
KIcon Person::faceIcon() const { return d->faceIcon(); }

Person::~Person() {}

}
