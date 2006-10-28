/*
 * This file is part of KSpread
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
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

#include "ScriptingModule.h"

#include <Doc.h>
#include <View.h>
#include <ViewAdaptor.h>
#include <Sheet.h>
#include <SheetAdaptor.h>
#include <Map.h>

#include <KoDocumentAdaptor.h>
#include <KoApplicationAdaptor.h>

#include <kapplication.h>
#include <kdebug.h>

extern "C"
{
    QObject* krossmodule()
    {
        return new ScriptingModule();
    }
}

class ScriptingModule::Private
{
	public:
		KSpread::View* view;
		KSpread::Doc* doc;
};

ScriptingModule::ScriptingModule(KSpread::View* view)
	: QObject()
	, d( new Private() )
{
	setObjectName("KSpreadScriptingModule");
	d->view = view;
	d->doc = view ? view->doc() : new KSpread::Doc(0, this);
}

ScriptingModule::~ScriptingModule()
{
	delete d;
}

QObject* ScriptingModule::application()
{
	return KApplication::kApplication()->findChild< KoApplicationAdaptor* >();
}

QObject* ScriptingModule::document()
{
	return d->doc->findChild< KoDocumentAdaptor* >();
}

QObject* ScriptingModule::view()
{
	//return d->view->findChild< KSpread::ViewAdaptor* >();
	return 0;
}

QObject* ScriptingModule::currentSheet()
{
	KSpread::Sheet* sheet = d->doc ? d->doc->displaySheet() : 0;
	return sheet ? sheet->findChild< KSpread::SheetAdaptor* >() : 0;
}

QObject* ScriptingModule::sheetByName(const QString& name)
{
	if(d->doc && d->doc->map())
		foreach(KSpread::Sheet* sheet, d->doc->map()->sheetList())
			if(sheet->sheetName() == name)
				return sheet->findChild< KSpread::SheetAdaptor* >();
	return 0;
}

QStringList ScriptingModule::sheetNames()
{
	QStringList names;
	foreach(KSpread::Sheet* sheet, d->doc->map()->sheetList())
		names.append(sheet->sheetName());
	return names;
}

bool ScriptingModule::fromXML(const QString& xml)
{
	KoXmlDocument xmldoc;
#ifdef KOXML_USE_QDOM
	if(! xmldoc.setContent(xml, true))
		return false;
#else
#warning Problem with KoXmlReader conversion!
	kWarning() << "Problem with KoXmlReader conversion!" << endl;
#endif
	return d->doc ? d->doc->loadXML(0, xmldoc) : false;
}

QString ScriptingModule::toXML()
{
	return d->doc ? d->doc->saveXML().toString(2) : QString();
}

bool ScriptingModule::openUrl(const QString& url)
{
    return d->doc ? d->doc->openURL(url) : false;
}

bool ScriptingModule::saveUrl(const QString& url)
{
	return d->doc ? d->doc->saveAs(url) : false;
}

bool ScriptingModule::importUrl(const QString& url)
{
	return d->doc ? d->doc->import(url) : false;
}

bool ScriptingModule::exportUrl(const QString& url)
{
	return d->doc ? d->doc->exp0rt(url) : false;
}

#include "ScriptingModule.moc"
