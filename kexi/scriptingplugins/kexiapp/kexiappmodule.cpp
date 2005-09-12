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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kexiappmodule.h"
#include "kexiappmainwindow.h"

#include "core/keximainwindow.h"

//#include <api/variant.h>
#include <api/qtobject.h>
#include <main/manager.h>

#include <kdebug.h>

extern "C"
{
    /**
     * Exported an loadable function as entry point to use
     * the \a KexiAppModule.
     */
    Kross::Api::Object* init_module(Kross::Api::Manager* manager)
    {
        return new Kross::KexiApp::KexiAppModule(manager);
    }
};

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
    : Kross::Api::Module<KexiAppModule>("KexiApp")
    , d(new KexiAppModulePrivate())
{
    d->manager = manager;

    kdDebug()<<"KexiAppModule::KexiAppModule 1"<<endl;
    Kross::Api::Object::Ptr mainwinobject = ((Kross::Api::Object*)manager)->getChild("KexiMainWindow");
    if(mainwinobject) {
        kdDebug()<<"KexiAppModule::KexiAppModule 2"<<endl;
        Kross::Api::QtObject* mainwinqtobject = (Kross::Api::QtObject*)( mainwinobject.data() );
        if(mainwinqtobject) {
            kdDebug()<<"KexiAppModule::KexiAppModule 3"<<endl;
            ::KexiMainWindow* mainwin = (::KexiMainWindow*)( mainwinqtobject->getObject() );
            if(mainwin) {
                kdDebug()<<"KexiAppModule::KexiAppModule 4"<<endl;
                addChild( new KexiAppMainWindow(mainwin) );
                return;
            }
        }
    }

    kdDebug()<<"KexiAppModule::KexiAppModule 5"<<endl;
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("There was no 'KexiMainWindow' published.") );
}

KexiAppModule::~KexiAppModule()
{
    delete d;
}


const QString KexiAppModule::getClassName() const
{
    return "Kross::KexiApp::KexiAppModule";
}

const QString KexiAppModule::getDescription() const
{
    return QString("The KexiAppModule object provides access to a running Kexi "
                   "application. This module is only avaiable in from within "
                   "Kexi embedded scripting code.");
}

