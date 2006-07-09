/***************************************************************************
 * kexiappmainwindow.cpp
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

#include "kexiappmainwindow.h"
#include "kexiapppart.h"

#include "core/keximainwindow.h"
#include "core/kexiproject.h"
#include "core/kexi.h"
#include "kexidb/connection.h"

#include "main/manager.h"

//#include <kdebug.h>

namespace Kross { namespace KexiApp {

    /// \internal
    class KexiAppMainWindowPrivate
    {
        public:
            KexiMainWindow* mainwindow;

            KexiProject* project() {
                KexiProject* project = mainwindow->project();
                if(! project)
                    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("No project loaded.") );
                return project;
            }
    };

}}

using namespace Kross::KexiApp;

KexiAppMainWindow::KexiAppMainWindow(KexiMainWindow* mainwindow)
    : Kross::Api::Class<KexiAppMainWindow>("KexiAppMainWindow")
    , d(new KexiAppMainWindowPrivate())
{
    d->mainwindow = mainwindow;

    addFunction("isConnected", &KexiAppMainWindow::isConnected);
    addFunction("getConnection", &KexiAppMainWindow::getConnection);

    addFunction("getPartItems", &KexiAppMainWindow::getPartItems);
    addFunction("openPartItem", &KexiAppMainWindow::openPartItem);

    //addFunction("hasObject", &KexiAppMainWindow::hasObject);
    //addFunction("getObject", &KexiAppMainWindow::getObject);
    //addFunction("getObjects", &KexiAppMainWindow::getObjects);
    //addFunction("openObject", &KexiAppMainWindow::openObject);
    //addFunction("hasObject", &KexiAppMainWindow::closeObject);
}

KexiAppMainWindow::~KexiAppMainWindow()
{
    delete d;
}

const QString KexiAppMainWindow::getClassName() const
{
    return "Kross::KexiApp::KexiAppMainWindow";
}

Kross::Api::Object::Ptr KexiAppMainWindow::isConnected(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant( QVariant(d->project()->isConnected(), 0) );
}

Kross::Api::Object::Ptr KexiAppMainWindow::getConnection(Kross::Api::List::Ptr)
{
    ::KexiDB::Connection* connection = d->project()->dbConnection();
    if(! connection)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("No connection established.") );
    Kross::Api::Module::Ptr module = Kross::Api::Manager::scriptManager()->loadModule("krosskexidb");
    if(! module)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("Could not load \"krosskexidb\" module.") );
    return module->get("KexiDBConnection", connection);
}

Kross::Api::Object::Ptr KexiAppMainWindow::getPartItems(Kross::Api::List::Ptr args)
{
    QString mimetype = Kross::Api::Variant::toString(args->item(0));
    if(mimetype.isNull()) return 0; // just to be sure...
    KexiPart::ItemDict* items = d->project()->itemsForMimeType( mimetype.latin1() );
    if(! items) return 0;
    return new Kross::Api::ListT<Kross::KexiApp::KexiAppPartItem, ::KexiPart::Item>( *items );
}

Kross::Api::Object::Ptr KexiAppMainWindow::openPartItem(Kross::Api::List::Ptr args)
{
    KexiAppPartItem* partitem = Kross::Api::Object::fromObject<KexiAppPartItem>(args->item(0));
    bool openingCancelled;
    KexiDialogBase* dialog = partitem ? d->mainwindow->openObject( partitem->item(),
        Kexi::DataViewMode, openingCancelled ) : 0;
    return new Kross::Api::Variant( QVariant(dialog != 0, 0) );
}

