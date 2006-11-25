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
//#include "kexiapppart.h"

#include "core/keximainwindow.h"
#include "core/kexiproject.h"
#include "core/kexi.h"
#include "kexidb/connection.h"

#include <kdebug.h>

using namespace Scripting;

KexiAppMainWindow::KexiAppMainWindow(KexiMainWindow* mainwindow)
    : QObject()
    , m_mainwindow(mainwindow)
{
    setObjectName("KexiApp");
}

KexiAppMainWindow::~KexiAppMainWindow()
{
}

KexiProject* KexiAppMainWindow::project()
{
    return m_mainwindow->project();
}

bool KexiAppMainWindow::isConnected()
{
    return project() ? project()->isConnected() : false;
}

#if 0
Kross::Api::Object::Ptr KexiAppMainWindow::getConnection()
{
    ::KexiDB::Connection* connection = project() ? project()->dbConnection() : 0;
    if(! connection) {
        kDebug() << "No connection established." << endl;
        return 0;
    }
    Kross::Api::Module::Ptr module = Kross::Api::Manager::scriptManager()->loadModule("kexidb");
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
#endif

#include "kexiappmainwindow.moc"
