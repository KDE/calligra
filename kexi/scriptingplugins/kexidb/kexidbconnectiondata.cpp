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

#include <qvariant.h>

using namespace Kross::KexiDB;

KexiDBConnectionData::KexiDBConnectionData(::KexiDB::ConnectionData* data)
    : Kross::Api::Class<KexiDBConnectionData>("KexiDBConnectionData")
    , m_data(data)
{
    addFunction("caption", &KexiDBConnectionData::caption);
    addFunction("setCaption", &KexiDBConnectionData::setCaption,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("description", &KexiDBConnectionData::description);
    addFunction("setDescription", &KexiDBConnectionData::setDescription,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("driverName", &KexiDBConnectionData::driverName);
    addFunction("setDriverName", &KexiDBConnectionData::setDriverName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("localSocketFileName", &KexiDBConnectionData::localSocketFileName);

    addFunction("hostName", &KexiDBConnectionData::hostName);
    addFunction("setHostName", &KexiDBConnectionData::setHostName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("port", &KexiDBConnectionData::port);
    addFunction("setPort", &KexiDBConnectionData::setPort,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"));

    addFunction("password", &KexiDBConnectionData::password);
    addFunction("setPassword", &KexiDBConnectionData::setPassword,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("userName", &KexiDBConnectionData::userName);
    addFunction("setUserName", &KexiDBConnectionData::setUserName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("fileName", &KexiDBConnectionData::fileName);
    addFunction("setFileName", &KexiDBConnectionData::setFileName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("dbPath", &KexiDBConnectionData::dbPath);
    addFunction("dbFileName", &KexiDBConnectionData::dbFileName);

    addFunction("serverInfoString", &KexiDBConnectionData::serverInfoString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(bool(true))));
}

KexiDBConnectionData::~KexiDBConnectionData()
{
    //delete m_data;
}

const QString KexiDBConnectionData::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnectionData";
}

::KexiDB::ConnectionData* KexiDBConnectionData::getConnectionData() const
{
    return m_data;
}

Kross::Api::Object::Ptr KexiDBConnectionData::caption(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->caption,
           "Kross::KexiDB::Connection::caption::String");
}

Kross::Api::Object::Ptr KexiDBConnectionData::setCaption(Kross::Api::List::Ptr args)
{
    m_data->caption = Kross::Api::Variant::toString(args->item(0));
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

