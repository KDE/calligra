/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIWEBFORMS_AUTH_USER_H
#define KEXIWEBFORMS_AUTH_USER_H

#include <QList>
#include <QString>

#include "Permission.h"

namespace KexiWebForms {
namespace Auth {

    class User {
    public:
        User(const char* name, const char* password) :
            m_name(name), m_password(password) {}

        User(const char* name, const char* password, const QList<Permission>& perms) :
            m_name(name), m_password(password), m_perms(perms) {}

        QString name() const;
        QString password() const;
        QList<Permission> permissions() const;
    private:
        QString m_name;
        QString m_password;
        QList<Permission> m_perms;
    };

} // end namespace Auth
} // end namespace KexiWebForms

#endif /* KEXIWEBFORMS_AUTH_USER_H */