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
    this->addFunction0< Kross::Api::Variant >("caption", this, &KexiDBConnectionData::caption);
    this->addFunction1< void, Kross::Api::Variant >("setCaption", this, &KexiDBConnectionData::setCaption);

    this->addFunction0< Kross::Api::Variant >("description", this, &KexiDBConnectionData::description);
    this->addFunction1< void, Kross::Api::Variant >("setDescription", this, &KexiDBConnectionData::setDescription);

    this->addFunction0< Kross::Api::Variant >("driverName", this, &KexiDBConnectionData::driverName);
    this->addFunction1< void, Kross::Api::Variant >("setDriverName", this, &KexiDBConnectionData::setDriverName);

    this->addFunction0< Kross::Api::Variant >("localSocketFileUsed", this, &KexiDBConnectionData::localSocketFileUsed);
    this->addFunction1< void, Kross::Api::Variant >("setLocalSocketFileUsed", this, &KexiDBConnectionData::setLocalSocketFileUsed);

    this->addFunction0< Kross::Api::Variant >("localSocketFileName", this, &KexiDBConnectionData::localSocketFileName);
    this->addFunction1< void, Kross::Api::Variant >("setLocalSocketFileName", this, &KexiDBConnectionData::setLocalSocketFileName);

    this->addFunction0< Kross::Api::Variant >("databaseName", this, &KexiDBConnectionData::databaseName);
    this->addFunction1< void, Kross::Api::Variant >("setDatabaseName", this, &KexiDBConnectionData::setDatabaseName);

    this->addFunction0< Kross::Api::Variant >("hostName", this, &KexiDBConnectionData::hostName);
    this->addFunction1< void, Kross::Api::Variant >("setHostName", this, &KexiDBConnectionData::setHostName);

    this->addFunction0< Kross::Api::Variant >("port", this, &KexiDBConnectionData::port);
    this->addFunction1< void, Kross::Api::Variant >("setPort", this, &KexiDBConnectionData::setPort);

    this->addFunction0< Kross::Api::Variant >("password", this, &KexiDBConnectionData::password);
    this->addFunction1< void, Kross::Api::Variant >("setPassword", this, &KexiDBConnectionData::setPassword);

    this->addFunction0< Kross::Api::Variant >("userName", this, &KexiDBConnectionData::userName);
    this->addFunction1< void, Kross::Api::Variant >("setUserName", this, &KexiDBConnectionData::setUserName);

    this->addFunction0< Kross::Api::Variant >("fileName", this, &KexiDBConnectionData::fileName);
    this->addFunction1< void, Kross::Api::Variant >("setFileName", this, &KexiDBConnectionData::setFileName);

    this->addFunction0< Kross::Api::Variant >("dbPath", this, &KexiDBConnectionData::dbPath);
    this->addFunction0< Kross::Api::Variant >("dbFileName", this, &KexiDBConnectionData::dbFileName);
    this->addFunction0< Kross::Api::Variant >("serverInfoString", this, &KexiDBConnectionData::serverInfoString);
}

KexiDBConnectionData::~KexiDBConnectionData()
{
    //delete m_data;
}

const QString KexiDBConnectionData::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnectionData";
}

const QString KexiDBConnectionData::caption() const { return m_data->caption; }
void KexiDBConnectionData::setCaption(const QString& name) { m_data->caption = name; }

const QString KexiDBConnectionData::description() const { return m_data->description; }
void KexiDBConnectionData::setDescription(const QString& desc) { m_data->description = desc; }

const QString KexiDBConnectionData::driverName() const { return m_data->driverName; }
void KexiDBConnectionData::setDriverName(const QString& driver) { m_data->driverName = driver; }

bool KexiDBConnectionData::localSocketFileUsed() const { return m_data->useLocalSocketFile; }
void KexiDBConnectionData::setLocalSocketFileUsed(bool used) { m_data->useLocalSocketFile = used; }
const QString KexiDBConnectionData::localSocketFileName() const { return m_data->localSocketFileName; }
void KexiDBConnectionData::setLocalSocketFileName(const QString& socketfilename) { m_data->localSocketFileName = socketfilename; }

const QString KexiDBConnectionData::databaseName() const { return m_dbname; }
void KexiDBConnectionData::setDatabaseName(const QString& dbname) { m_dbname = dbname; }

const QString KexiDBConnectionData::hostName() const { return m_data->hostName; }
void KexiDBConnectionData::setHostName(const QString& hostname) { m_data->hostName = hostname; }

int KexiDBConnectionData::port() const { return m_data->port; }
void KexiDBConnectionData::setPort(int p) { m_data->port = p; }

const QString KexiDBConnectionData::password() const { return m_data->password; }
void KexiDBConnectionData::setPassword(const QString& passwd) { m_data->password = passwd; }

const QString KexiDBConnectionData::userName() const { return m_data->userName; }
void KexiDBConnectionData::setUserName(const QString& username) { m_data->userName = username; }

const QString KexiDBConnectionData::fileName() const { return m_data->fileName(); }
void KexiDBConnectionData::setFileName(const QString& filename) { m_data->setFileName(filename); }

const QString KexiDBConnectionData::dbPath() const { return m_data->dbPath(); }
const QString KexiDBConnectionData::dbFileName() const { return m_data->dbFileName(); }
const QString KexiDBConnectionData::serverInfoString() const { return m_data->serverInfoString(true); }
