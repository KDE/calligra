/***************************************************************************
 * kexidbmodule.cpp
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

#include "kexidbmodule.h"
#include "kexidbdrivermanager.h"
#include "kexidbconnection.h"

//#include <api/object.h>
//#include <api/variant.h>
#include <main/manager.h>

#include <kdebug.h>

// The as version() published versionnumber of this kross-module.
#define KROSS_KEXIDB_VERSION 1

extern "C"
{
    /**
     * Exported an loadable function as entry point to use
     * the \a KexiDBModule.
     */
    Kross::Api::Object* KDE_EXPORT init_module(Kross::Api::Manager* manager)
    {
        return new Kross::KexiDB::KexiDBModule(manager);
    }
}

using namespace Kross::KexiDB;

KexiDBModule::KexiDBModule(Kross::Api::Manager* /*manager*/)
    : Kross::Api::Module("KexiDB")
    //, m_manager(manager)
{
    //kDebug() << "Kross::KexiDB::KexiDBModule Ctor" << endl;
    addChild( new Kross::Api::Variant(KROSS_KEXIDB_VERSION), "version" );
    addChild( new KexiDBDriverManager() );
}

KexiDBModule::~KexiDBModule()
{
    //kDebug() << "Kross::KexiDB::KexiDBModule Dtor" << endl;
}

const QString KexiDBModule::getClassName() const
{
    return "Kross::KexiDB::KexiDBModule";
}

Kross::Api::Object::Ptr KexiDBModule::get(const QString& name, void* p)
{
    if(name == "KexiDBConnection") {
        ::KexiDB::Connection* connection = (::KexiDB::Connection*)p;
        if(connection)
            return new KexiDBConnection(connection);
    }
    return 0;
}
