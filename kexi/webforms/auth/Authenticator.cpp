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

#include <pion/net/HTTPAuth.hpp>

#include "Permission.h"

#include "Authenticator.h"

namespace KexiWebForms {
namespace Auth {
    Authenticator* Authenticator::m_instance = 0;
    
    void Authenticator::init(pion::net::HTTPAuthPtr p) {
        if (!m_instance)
            m_instance = new Authenticator(p);
        m_instance->loadStore();
    }

    Authenticator* Authenticator::getInstance() {
        if (m_instance)
            return m_instance;
        else // ouch!
            return NULL;
    }

    // fictional loadStore, returns a fixed list of users
    bool Authenticator::loadStore() {
        User* u = new User("root", "root");
        u->addPermission(CAN_CREATE);
        u->addPermission(CAN_READ);
        u->addPermission(CAN_UPDATE);
        u->addPermission(CAN_DELETE);

        u = new User("restricted", "restricted");

        m_users.append(*u);
        m_auth->addUser(u->name().toUtf8().constData(), u->password().toUtf8().constData());
        
        m_users.append(*u);
        m_auth->addUser(u->name().toUtf8().constData(), u->password().toUtf8().constData());

        return true;
    }
    
    User Authenticator::authenticate(const char* name, const char* password) {
        for (int i = 0; i < m_users.size(); ++i) {
            User u = m_users.at(i);
            if ((u.name() == name) && (u.password() == password)) {
                return u;
            }
        }
        User* u = new User("anonymous", "guest");
        return *u;
    }

}
}