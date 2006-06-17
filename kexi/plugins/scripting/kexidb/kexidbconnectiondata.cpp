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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "kexidbconnectiondata.h"

#include <qvariant.h>

using namespace Kross::KexiDB;

KexiDBConnectionData::KexiDBConnectionData(::KexiDB::ConnectionData* data)
    : Kross::Api::Class<KexiDBConnectionData>("KexiDBConnectionData")
    , m_data(data)
{
    addFunction("caption", &KexiDBConnectionData::caption);
    addFunction("setCaption", &KexiDBConnectionData::setCaption);

    addFunction("description", &KexiDBConnectionData::description);
    addFunction("setDescription", &KexiDBConnectionData::setDescription);

    addFunction("driverName", &KexiDBConnectionData::driverName);
    addFunction("setDriverName", &KexiDBConnectionData::setDriverName);

    addFunction("localSocketFileUsed", &KexiDBConnectionData::localSocketFileUsed);
    addFunction("setLocalSocketFileUsed", &KexiDBConnectionData::setLocalSocketFileUsed);

    addFunction("localSocketFileName", &KexiDBConnectionData::localSocketFileName);
    addFunction("setLocalSocketFileName", &KexiDBConnectionData::setLocalSocketFileName);

    addFunction("databaseName", &KexiDBConnectionData::databaseName);
    addFunction("setDatabaseName", &KexiDBConnectionData::setDatabaseName);

    addFunction("hostName", &KexiDBConnectionData::hostName);
    addFunction("setHostName", &KexiDBConnectionData::setHostName);

    addFunction("port", &KexiDBConnectionData::port);
    addFunction("setPort", &KexiDBConnectionData::setPort);

    addFunction("password", &KexiDBConnectionData::password);
    addFunction("setPassword", &KexiDBConnectionData::setPassword);

    addFunction("userName", &KexiDBConnectionData::userName);
    addFunction("setUserName", &KexiDBConnectionData::setUserName);

    addFunction("fileName", &KexiDBConnectionData::fileName);
    addFunction("setFileName", &KexiDBConnectionData::setFileName);

    addFunction("dbPath", &KexiDBConnectionData::dbPath);
    addFunction("dbFileName", &KexiDBConnectionData::dbFileName);

    addFunction("serverInfoString", &KexiDBConnectionData::serverInfoString);
}

KexiDBConnectionData::~KexiDBConnectionData()
{
    //delete m_data;
}

const QString KexiDBConnectionData::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnectionData";
}

Kross::Api::Object::Ptr KexiDBConnectionData::caption(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->caption);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setCaption(Kross::Api::List::Ptr args)
{
    m_data->caption = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::description(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->description);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setDescription(Kross::Api::List::Ptr args)
{
    m_data->description = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::driverName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->driverName);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setDriverName(Kross::Api::List::Ptr args)
{
    m_data->driverName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::localSocketFileUsed(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->useLocalSocketFile);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setLocalSocketFileUsed(Kross::Api::List::Ptr args)
{
    m_data->useLocalSocketFile = Kross::Api::Variant::toBool(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::localSocketFileName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->localSocketFileName);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setLocalSocketFileName(Kross::Api::List::Ptr args)
{
    m_data->localSocketFileName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::databaseName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_dbname);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setDatabaseName(Kross::Api::List::Ptr args)
{
    m_dbname = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::hostName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->hostName);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setHostName(Kross::Api::List::Ptr args)
{
    m_data->hostName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::port(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant((uint)m_data->port);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setPort(Kross::Api::List::Ptr args)
{
    m_data->port = Kross::Api::Variant::toUInt(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::password(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->password);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setPassword(Kross::Api::List::Ptr args)
{
    m_data->password = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::userName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->userName);
}

Kross::Api::Object::Ptr KexiDBConnectionData::setUserName(Kross::Api::List::Ptr args)
{
    m_data->userName = Kross::Api::Variant::toString(args->item(0));
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::fileName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->fileName());
}

Kross::Api::Object::Ptr KexiDBConnectionData::setFileName(Kross::Api::List::Ptr args)
{
    m_data->setFileName( Kross::Api::Variant::toString(args->item(0)) );
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnectionData::dbPath(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->dbPath());
}

Kross::Api::Object::Ptr KexiDBConnectionData::dbFileName(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_data->dbFileName());
}

Kross::Api::Object::Ptr KexiDBConnectionData::serverInfoString(Kross::Api::List::Ptr args)
{
    bool user = args->count() > 0 ? Kross::Api::Variant::toBool(args->item(0)) : true;
    return new Kross::Api::Variant(m_data->serverInfoString(user));
}
