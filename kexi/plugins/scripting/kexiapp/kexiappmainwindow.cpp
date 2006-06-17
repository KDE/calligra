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

    this->addFunction0<Kross::Api::Variant>("isConnected", this, &KexiAppMainWindow::isConnected);
    this->addFunction0<Kross::Api::Object>("getConnection", this, &KexiAppMainWindow::getConnection);

    this->addFunction1<Kross::Api::List, Kross::Api::Variant>("getPartItems", this, &KexiAppMainWindow::getPartItems);
    this->addFunction1<Kross::Api::Variant, KexiAppPartItem>("openPartItem", this, &KexiAppMainWindow::openPartItem);
}

KexiAppMainWindow::~KexiAppMainWindow()
{
    delete d;
}

const QString KexiAppMainWindow::getClassName() const
{
    return "Kross::KexiApp::KexiAppMainWindow";
}

bool KexiAppMainWindow::isConnected()
{
    return d->project()->isConnected();
}

Kross::Api::Object::Ptr KexiAppMainWindow::getConnection()
{
    ::KexiDB::Connection* connection = d->project()->dbConnection();
    if(! connection)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("No connection established.") );
    Kross::Api::Module::Ptr module = Kross::Api::Manager::scriptManager()->loadModule("krosskexidb");
    if(! module)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("Could not load \"krosskexidb\" module.") );
    return module->get("KexiDBConnection", connection);
}

Kross::Api::List* KexiAppMainWindow::getPartItems(const QString& mimetype)
{
    if(mimetype.isNull()) return 0; // just to be sure...
    KexiPart::ItemDict* items = d->project()->itemsForMimeType( mimetype.latin1() );
    if(! items) return 0;
    return new Kross::Api::ListT<KexiAppPartItem>( *items );
}

bool KexiAppMainWindow::openPartItem(KexiAppPartItem* partitem)
{
    bool openingCancelled;
    KexiDialogBase* dialog = partitem
        ? d->mainwindow->openObject(partitem->item(), Kexi::DataViewMode, openingCancelled)
        : 0;
    return (dialog != 0 && ! openingCancelled);
}
