/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "Module.h"
#include "Page.h"

#include <QPointer>
#include <kapplication.h>
#include <kdebug.h>

#include <KoDocumentAdaptor.h>
#include <KoApplicationAdaptor.h>

#include <KWDocument.h>
#include <KWView.h>
#include <KWPage.h>

extern "C"
{
    QObject* krossmodule()
    {
        return new Scripting::Module();
    }
}

using namespace Scripting;

namespace Scripting {

    /// \internal d-pointer class.
    class Module::Private
    {
        public:
            QPointer<KWView> view;
            QPointer<KWDocument> doc;
    };
}

Module::Module(QObject* parent)
    : QObject(parent)
    , d( new Private() )
{
    setObjectName("KWordScriptingModule");
    d->view = 0;
	d->doc = 0;
}

Module::~Module()
{
    delete d;
}

KWDocument* Module::doc()
{
    if(! d->doc)
        d->doc = d->view ? d->view->kwdocument() : new KWDocument(0, this);
    return d->doc;
}

void Module::setView(KWView* view)
{
    d->view = view;
}

QObject* Module::application()
{
    return KApplication::kApplication()->findChild< KoApplicationAdaptor* >();
}

QObject* Module::document()
{
    return doc()->findChild< KoDocumentAdaptor* >();
}

int Module::pageCount()
{
    //TODO is this always equal to doc()->pageManager()->lastPageNumber() ? I don't believe so :-/
    return doc()->pageManager()->pageCount();
}

QObject* Module::page(int pageNumber)
{
    KWPage* page = doc()->pageManager()->page(pageNumber);
    return page ? new Page(this, page) : 0;
}

QObject* Module::insertPage( int afterPageNum )
{
    //TODO check for doc()->pageManager()->onlyAllowAppend()
    KWPage* page = const_cast<KWPageManager*>( doc()->pageManager() )->insertPage(afterPageNum);
    return page ? new Page(this, page) : 0;
}

void Module::removePage( int pageNumber )
{
    //TODO remove also the wrapper? and what's about pages that are "Spread" (page that represents 2 pagenumbers)?
    const_cast<KWPageManager*>( doc()->pageManager() )->removePage(pageNumber);
}

int Module::startPage()
{
    return doc()->pageManager()->startPage();
}

void Module::setStartPage(int pageNumber)
{
    //TODO this is evil since it changes page(int pageNumber) above... we need a more persistent way to deal with pages!
     const_cast<KWPageManager*>( doc()->pageManager() )->setStartPage(pageNumber);
}

#include "Module.moc"
