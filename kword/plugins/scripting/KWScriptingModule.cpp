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

#include "KWScriptingModule.h"

#include <QPointer>
#include <kapplication.h>
#include <kdebug.h>

#include <KoDocumentAdaptor.h>
#include <KoApplicationAdaptor.h>

#include <KWDocument.h>
#include <KWView.h>

extern "C"
{
    QObject* krossmodule()
    {
        return new KWScriptingModule();
    }
}

/// \internal d-pointer class.
class KWScriptingModule::Private
{
    public:
        QPointer<KWView> view;
        QPointer<KWDocument> doc;
};

KWScriptingModule::KWScriptingModule()
    : QObject()
    , d( new Private() )
{
    setObjectName("KWordScriptingModule");
    d->view = 0;
	d->doc = 0;
}

KWScriptingModule::~KWScriptingModule()
{
    delete d;
}

KWDocument* KWScriptingModule::doc()
{
    if(! d->doc)
        d->doc = d->view ? d->view->kwdocument() : new KWDocument(0, this);
    return d->doc;
}

void KWScriptingModule::setView(KWView* view)
{
    d->view = view;
}

QObject* KWScriptingModule::application()
{
    return KApplication::kApplication()->findChild< KoApplicationAdaptor* >();
}

QObject* KWScriptingModule::document()
{
    return doc()->findChild< KoDocumentAdaptor* >();
}

#include "KWScriptingModule.moc"
