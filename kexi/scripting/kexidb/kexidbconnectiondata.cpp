/***************************************************************************
 * kexidbconnectiondata.cpp
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kexidbconnectiondata.h"
//#include "kexidbconnection.h"
//#include "kexidbdriver.h"
//#include "../api/exception.h"

#include <qvariant.h>
#include <klocale.h>
//#include <kdebug.h>

using namespace Kross::KexiDB;

/* KexiDBConnectionDataManager */

KexiDBConnectionDataManager::KexiDBConnectionDataManager()
    : Kross::Api::Class<KexiDBConnectionDataManager>("KexiDBConnectionDataManager")
{
    addFunction("createConnectionData",
        &KexiDBConnectionDataManager::createConnectionData,
        Kross::Api::ArgumentList(),
        i18n("Create a new KexiDBConnectionData object and return it.")
    );
}

KexiDBConnectionDataManager::~KexiDBConnectionDataManager()
{
    for(QValueList<KexiDBConnectionData*>::Iterator it = m_datas.begin(); it != m_datas.end(); ++it)
        delete *it;
    //m_datas.clear();
}

const QString KexiDBConnectionDataManager::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnectionDataManager";
}

const QString KexiDBConnectionDataManager::getDescription() const
{
    return i18n("KexiDB::ConnectionData wrapper for database specific "
                "connection data, e.g. host, port.");
}

Kross::Api::Object* KexiDBConnectionDataManager::createConnectionData(Kross::Api::List*)
{
    ::KexiDB::ConnectionData* c = new ::KexiDB::ConnectionData();
    KexiDBConnectionData* data = new KexiDBConnectionData(c);
    m_datas.append(data);
    return data;
}

/* KexiDBConnectionData */

KexiDBConnectionData::KexiDBConnectionData(::KexiDB::ConnectionData* data)
    : Kross::Api::Class<KexiDBConnectionData>("KexiDBConnectionData")
    , m_data(data)
{
    addFunction("connName", &KexiDBConnectionData::connName,
        Kross::Api::ArgumentList(),
        i18n("Return connection name.")
    );
    addFunction("setConnName", &KexiDBConnectionData::setConnName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set connection name.")
    );
    addFunction("description", &KexiDBConnectionData::description,
        Kross::Api::ArgumentList(),
        i18n("Return description.")
    );
    addFunction("setDescription", &KexiDBConnectionData::setDescription,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set description.")
    );
    addFunction("driverName", &KexiDBConnectionData::driverName,
        Kross::Api::ArgumentList(),
        i18n("Return drivername.")
    );
    addFunction("setDriverName", &KexiDBConnectionData::setDriverName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set drivername.")
    );
    addFunction("localSocketFileName", &KexiDBConnectionData::localSocketFileName,
        Kross::Api::ArgumentList(),
        i18n("Return local socket filename.")
    );
    addFunction("hostName", &KexiDBConnectionData::hostName,
        Kross::Api::ArgumentList(),
        i18n("Return hostname.")
    );
    addFunction("setHostName", &KexiDBConnectionData::setHostName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set hostname.")
    );
    addFunction("port", &KexiDBConnectionData::port,
        Kross::Api::ArgumentList(),
        i18n("Return port.")
    );
    addFunction("setPort", &KexiDBConnectionData::setPort,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"),
        i18n("Set port.")
    );
    addFunction("password", &KexiDBConnectionData::password,
        Kross::Api::ArgumentList(),
        i18n("Return password.")
    );
    addFunction("setPassword", &KexiDBConnectionData::setPassword,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set password.")
    );
    addFunction("userName", &KexiDBConnectionData::userName,
        Kross::Api::ArgumentList(),
        i18n("Return username.")
    );
    addFunction("setUserName", &KexiDBConnectionData::setUserName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set username.")
    );
    addFunction("fileName", &KexiDBConnectionData::fileName,
        Kross::Api::ArgumentList(),
        i18n("Return filename.")
    );
    addFunction("setFileName", &KexiDBConnectionData::setFileName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Set filename.")
    );
    addFunction("dbPath", &KexiDBConnectionData::dbPath,
        Kross::Api::ArgumentList(),
        i18n("Return database path.")
    );
    addFunction("dbFileName", &KexiDBConnectionData::dbFileName,
        Kross::Api::ArgumentList(),
        i18n("Return database filename.")
    );
    addFunction("serverInfoString", &KexiDBConnectionData::serverInfoString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool", Kross::Api::Variant::create(QVariant(true))),
        i18n("Return a user-friendly string representation.")
    );
}

KexiDBConnectionData::~KexiDBConnectionData()
{
    //delete m_data;
}

const QString KexiDBConnectionData::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnectionData";
}

const QString KexiDBConnectionData::getDescription() const
{
    return i18n("KexiDB::ConnectionData wrapper for database "
                "specific connection data, e.g. host, port.");
}

::KexiDB::ConnectionData* KexiDBConnectionData::getConnectionData() const
{
    return m_data;
}

Kross::Api::Object* KexiDBConnectionData::connName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->connName,
           "Kross::KexiDB::Connection::connName::String");
}

Kross::Api::Object* KexiDBConnectionData::setConnName(Kross::Api::List* args)
{
    m_data->connName = Kross::Api::Variant::toString(args->item(0));
    //TODO implementate NULL object return usage!
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::description(Kross::Api::List*)
{
    /*TODO
    return Kross::Api::Variant::create(m_data->description,
           "Kross::KexiDB::Connection::description::String");
    */
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::setDescription(Kross::Api::List*)
{
    //TODO m_data->description = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::driverName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->driverName,
           "Kross::KexiDB::Connection::driverName::String");
}

Kross::Api::Object* KexiDBConnectionData::setDriverName(Kross::Api::List* args)
{
    m_data->driverName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::localSocketFileName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->localSocketFileName,
           "Kross::KexiDB::Connection::localSocketFileName::String");
}

Kross::Api::Object* KexiDBConnectionData::hostName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->hostName,
           "Kross::KexiDB::Connection::hostName::String");
}

Kross::Api::Object* KexiDBConnectionData::setHostName(Kross::Api::List* args)
{
    m_data->hostName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::port(Kross::Api::List*)
{
    return Kross::Api::Variant::create((uint)m_data->port,
           "Kross::KexiDB::Connection::port::UInt");
}

Kross::Api::Object* KexiDBConnectionData::setPort(Kross::Api::List* args)
{
    m_data->port = Kross::Api::Variant::toUInt(args->item(0));
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::password(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->password,
           "Kross::KexiDB::Connection::password::String");
}

Kross::Api::Object* KexiDBConnectionData::setPassword(Kross::Api::List* args)
{
    m_data->password = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::userName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->userName,
           "Kross::KexiDB::Connection::userName::String");
}

Kross::Api::Object* KexiDBConnectionData::setUserName(Kross::Api::List* args)
{
    m_data->userName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object* KexiDBConnectionData::fileName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->fileName(),
           "Kross::KexiDB::Connection::fileName::String");
}

Kross::Api::Object* KexiDBConnectionData::setFileName(Kross::Api::List* args)
{
    m_data->setFileName( Kross::Api::Variant::toString(args->item(0)) );
    //TODO return empty object OR true OR new hostname, not null...
    //return 0;
    return Kross::Api::Variant::create(true);
}

Kross::Api::Object* KexiDBConnectionData::dbPath(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->dbPath(),
           "Kross::KexiDB::Connection::dbPath::String");
}

Kross::Api::Object* KexiDBConnectionData::dbFileName(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_data->dbFileName(),
           "Kross::KexiDB::Connection::dbFileName::String");
}

Kross::Api::Object* KexiDBConnectionData::serverInfoString(Kross::Api::List*)
{
    //TODO
    return 0;
}

