/***************************************************************************
 * kexidbconnectiondata.cpp
 * This file is part of the KDE project
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
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(QVariant(true))),
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
    return i18n("KexiDB::ConnectionData wrapper for database-"
                "specific connection data, e.g. host, port.");
}

::KexiDB::ConnectionData* KexiDBConnectionData::getConnectionData() const
{
    return m_data;
}

Kross::Api::Object::Ptr KexiDBConnectionData::connName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->connName,
           "Kross::KexiDB::Connection::connName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setConnName(Kross::Api::List::Ptr args)
{
    m_data->connName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::description(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->description,
           "Kross::KexiDB::Connection::description::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setDescription(Kross::Api::List::Ptr args)
{
    m_data->description = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::driverName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->driverName,
           "Kross::KexiDB::Connection::driverName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setDriverName(Kross::Api::List::Ptr args)
{
    m_data->driverName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::localSocketFileName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->localSocketFileName,
           "Kross::KexiDB::Connection::localSocketFileName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::hostName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->hostName,
           "Kross::KexiDB::Connection::hostName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setHostName(Kross::Api::List::Ptr args)
{
    m_data->hostName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::port(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant((uint)m_data->port,
           "Kross::KexiDB::Connection::port::UInt");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setPort(Kross::Api::List::Ptr args)
{
    m_data->port = Kross::Api::Variant::toUInt(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::password(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->password,
           "Kross::KexiDB::Connection::password::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setPassword(Kross::Api::List::Ptr args)
{
    m_data->password = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::userName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->userName,
           "Kross::KexiDB::Connection::userName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setUserName(Kross::Api::List::Ptr args)
{
    m_data->userName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::fileName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->fileName(),
           "Kross::KexiDB::Connection::fileName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setFileName(Kross::Api::List::Ptr args)
{
    m_data->setFileName( Kross::Api::Variant::toString(args->item(0)) );
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::dbPath(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->dbPath(),
           "Kross::KexiDB::Connection::dbPath::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::dbFileName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->dbFileName(),
           "Kross::KexiDB::Connection::dbFileName::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::serverInfoString(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(m_data->serverInfoString(args->item(0)),
           "Kross::KexiDB::Connection::serverInfoString::String");
}

