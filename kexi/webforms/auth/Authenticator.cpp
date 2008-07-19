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

#include <KDebug>


#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>

#include <pion/net/PionUser.hpp>
#include <pion/net/HTTPAuth.hpp>

#include "Permission.h"
#include "DataProvider.h"

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
        KexiDB::TableSchema* table = gConnection->tableSchema("kexi__users");
        
        if (!table) {
            // the table doesn't exist, create it
            kDebug() << "kexi__users table does not exist, creating it" << endl;
            KexiDB::TableSchema* kexi__users = new KexiDB::TableSchema("kexi__users");
            //kexi__users->setNative(true);
            KexiDB::Field* id = new KexiDB::Field("u_id", KexiDB::Field::Integer);
            id->setAutoIncrement(true);
            id->setPrimaryKey(true);
            kexi__users->insertField(0, id);
            KexiDB::Field* name = new KexiDB::Field("u_name", KexiDB::Field::Text);
            kexi__users->insertField(1, name);
            KexiDB::Field* password = new KexiDB::Field("u_password", KexiDB::Field::Text);
            kexi__users->insertField(2, password);
            KexiDB::Field* create = new KexiDB::Field("u_create", KexiDB::Field::Boolean);
            kexi__users->insertField(3, create);
            KexiDB::Field* read = new KexiDB::Field("u_read", KexiDB::Field::Boolean);
            kexi__users->insertField(4, read);
            KexiDB::Field* update = new KexiDB::Field("u_update", KexiDB::Field::Boolean);
            kexi__users->insertField(5, update);
            KexiDB::Field* fdelete = new KexiDB::Field("u_delete", KexiDB::Field::Boolean);
            kexi__users->insertField(6, fdelete);
            KexiDB::Field* fquery = new KexiDB::Field("u_query", KexiDB::Field::Boolean);
            kexi__users->insertField(7, fquery);

            if (!gConnection->createTable(kexi__users)) {
                // Table was not created, fatal error
                kError() << "Failed to create system table kexi__users" << endl;
                kError() << "Error string: " << gConnection->errorMsg() << endl;
                delete kexi__users;
                return false;
            } else {
                // Table was created, create two standard accounts
                KexiDB::QuerySchema query(*kexi__users);
                KexiDB::Cursor* cursor = gConnection->prepareQuery(query);
                KexiDB::RecordData recordData(kexi__users->fieldCount());
                KexiDB::RowEditBuffer editBuffer(true);
                // root
                QVariant vtrue(true);
                QVariant vfalse(false);
                kDebug() << "Creating user root with password root" << endl;
                QVariant user_root("root");
                QVariant password_root("root");
                editBuffer.insert(*query.columnInfo(name->name()), user_root);
                editBuffer.insert(*query.columnInfo(password->name()), password_root);
                editBuffer.insert(*query.columnInfo(create->name()), vtrue);
                editBuffer.insert(*query.columnInfo(read->name()), vtrue);
                editBuffer.insert(*query.columnInfo(update->name()), vtrue);
                editBuffer.insert(*query.columnInfo(fdelete->name()), vtrue);
                editBuffer.insert(*query.columnInfo(fquery->name()), vtrue);
                kDebug() << "Registering user within database" << endl;
                if (cursor->insertRow(recordData, editBuffer)) {
                    kDebug() << "Succeeded" << endl;
                    User* u = new User("root", "root");
                    m_users.append(*u);
                    m_auth->addUser(u->name().toUtf8().constData(), u->password().toUtf8().constData());
                } else {
                    kError() << "An error occurred" << endl;
                    return false;
                }

                // anonymous
                kDebug() << "Creating user anonymous with password guest" << endl;
                QVariant user_anonymous("anonymous");
                QVariant password_anonymous("guest");
                editBuffer.insert(*query.columnInfo(name->name()), user_anonymous);
                editBuffer.insert(*query.columnInfo(password->name()), password_anonymous);
                editBuffer.insert(*query.columnInfo(create->name()), vfalse);
                editBuffer.insert(*query.columnInfo(read->name()), vfalse);
                editBuffer.insert(*query.columnInfo(update->name()), vfalse);
                editBuffer.insert(*query.columnInfo(fdelete->name()), vfalse);
                editBuffer.insert(*query.columnInfo(fquery->name()), vfalse);
                if (cursor->insertRow(recordData, editBuffer)) {
                    kDebug() << "Succeeded" << endl;
                    User* u = new User("anonymous", "guest");
                    m_users.append(*u);
                    m_auth->addUser(u->name().toUtf8().constData(), u->password().toUtf8().constData());
                } else {
                    kError() << "An error occurred" << endl;
                    return false;
                }
                gConnection->deleteCursor(cursor);
            }
        } else {
            // load stuff from the store, create appropriated User objects, store them within
            // Authenticator
            KexiDB::QuerySchema query(*table);
            KexiDB::Cursor* cursor = gConnection->executeQuery(query);
            while (cursor->moveNext()) {
                // Skip id
                QString username(cursor->value(1).toString());
                QString password(cursor->value(2).toString());
                QList<Permission> perms;
                
                if (cursor->value(3).toBool()) perms.append(CREATE);
                if (cursor->value(4).toBool()) perms.append(READ);
                if (cursor->value(5).toBool()) perms.append(UPDATE);
                if (cursor->value(6).toBool()) perms.append(DELETE);
                if (cursor->value(7).toBool()) perms.append(QUERY);

                User* u = new User(username, password, perms);
                m_users.append(*u);
                m_auth->addUser(u->name().toUtf8().constData(), u->password().toUtf8().constData());
                kDebug() << "Loaded user " << username << " from store" << endl;
            }
        }

        return true;
    }
    
    User Authenticator::authenticate(const char* name, const char* password) {
        for (int i = 0; i < m_users.size(); ++i) {
            User u = m_users.at(i);
            if ((u.name() == name) && (u.password() == password)) {
                return u;
            }
        }
        return User("anonymous", "guest");
    }

    User Authenticator::authenticate(const std::string& name, const std::string& password) {
        return authenticate(name.c_str(), password.c_str());
    }

    User Authenticator::authenticate(pion::net::PionUserPtr p) {
        return authenticate(p->getUsername(), p->getPassword());
    }

}
}
