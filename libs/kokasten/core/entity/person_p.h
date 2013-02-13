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


#ifndef PERSON_P_H
#define PERSON_P_H

#include "person.h"

//KDE
#include <KIcon>
//Qt
#include <QtCore/QString>


namespace Kasten2
{

class Person::Private : public KShared
{
  public:
    Private( const QString& name, const KIcon& faceIcon );
    ~Private();

  public:
    QString name() const;
    KIcon faceIcon() const;

  protected:
    QString mName;
    KIcon mFaceIcon;
};

inline Person::Private::Private( const QString& name, const KIcon& faceIcon )
 : mName( name ), mFaceIcon( faceIcon ) {}

inline QString Person::Private::name()   const { return mName; }
inline KIcon Person::Private::faceIcon() const { return mFaceIcon; }

inline Person::Private::~Private() {}

}

#endif
