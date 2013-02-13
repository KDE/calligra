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

#ifndef KASTEN_IF_USERLISTABLE_H
#define KASTEN_IF_USERLISTABLE_H

// Qt
#include <QtCore/QtPlugin>

template <class T> class QList;


namespace Kasten2
{

class Person;


namespace If
{

// TODO: can owner change? Later perhaps, if storage will be movable
class UserListable
{
  public:
    virtual ~UserListable();

  public: // get
    virtual Person owner() const = 0;
    virtual QList<Person> userList() const = 0;

  public: // set/action
//     virtual void setVersion( DocumentVersionId id ) = 0;
//     virtual void revertToVersionByIndex( int versionIndex ) = 0;

  public: // signal
//     virtual void ownerChange( const Person& newOwner ) = 0;
    virtual void usersAdded( const QList<Person>& newUserList ) = 0;
    virtual void usersRemoved( const QList<Person>& newUserList ) = 0;
};

inline UserListable::~UserListable() {}

}
}

Q_DECLARE_INTERFACE( Kasten2::If::UserListable, "org.kde.kasten2.if.userlistable/1.0" )

#endif
