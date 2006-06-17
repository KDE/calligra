/***************************************************************************
 * kexiappmodule.cpp
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

#include "kexiappmodule.h"
#include "kexiappmainwindow.h"

#include "core/keximainwindow.h"

#include <api/object.h>
#include <api/qtobject.h>
#include <main/manager.h>

#include <kdebug.h>

// The as version() published versionnumber of this kross-module.
#define KROSS_KEXIAPP_VERSION 1

extern "C"
{
    /**
     * Exported an loadable function as entry point to use
     * the \a KexiAppModule.
     */
    Kross::Api::Object* KDE_EXPORT init_module(Kross::Api::Manager* manager)
    {
        return new Kross::KexiApp::KexiAppModule(manager);
    }
}

namespace Kross { namespace KexiApp {

    /// \internal
    class KexiAppModulePrivate
    {
        public:
            Kross::Api::Manager* manager;
    };

}}

using namespace Kross::KexiApp;

KexiAppModule::KexiAppModule(Kross::Api::Manager* manager)
    : Kross::Api::Module("KexiApp")
    , d(new KexiAppModulePrivate())
{
    kdDebug() << "Kross::KexiApp::KexiAppModule Ctor" << endl;

    d->manager = manager;

    Kross::Api::Object::Ptr mainwinobject = manager->getChild("KexiMainWindow");
    if(mainwinobject) {
        Kross::Api::QtObject* mainwinqtobject = dynamic_cast< Kross::Api::QtObject* >( mainwinobject.data() );
        if(mainwinqtobject) {
            ::KexiMainWindow* mainwin = dynamic_cast< ::KexiMainWindow* >( mainwinqtobject->getObject() );
            if(mainwin) {
                addChild( new Kross::Api::Variant(KROSS_KEXIAPP_VERSION), "version" );
                addChild( new KexiAppMainWindow(mainwin) );
                return;
            }
            else kdDebug()<<"Kross::KexiApp::KexiAppModule: Failed to determinate KexiMainWindow instance"<<endl;
        }
        else kdDebug()<<"Kross::KexiApp::KexiAppModule: Failed to cast 'KexiMainWindow' to a QtObject"<<endl;
    }
    else kdDebug()<<"Kross::KexiApp::KexiAppModule: No such object 'KexiMainWindow'"<<endl;

    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("There was no 'KexiMainWindow' published.") );
}

KexiAppModule::~KexiAppModule()
{
    kdDebug() << "Kross::KexiApp::KexiAppModule Dtor" << endl;
    delete d;
}


const QString KexiAppModule::getClassName() const
{
    return "Kross::KexiApp::KexiAppModule";
}

