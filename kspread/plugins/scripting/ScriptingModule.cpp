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
#include "ScriptingFunction.h"

#include <QPointer>
#include <kapplication.h>
#include <kdebug.h>

#include <Doc.h>
#include <View.h>
#include <ViewAdaptor.h>
#include <Sheet.h>
#include <SheetAdaptor.h>
#include <Map.h>
#include <MapAdaptor.h>
#include <KoDocumentAdaptor.h>
#include <KoApplicationAdaptor.h>

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
		QPointer<KSpread::View> view;
		QPointer<KSpread::Doc> doc;
		QHash< QString, ScriptingFunction* > functions;
		QStringList functionnames;
};

ScriptingModule::ScriptingModule()
	: QObject()
	, d( new Private() )
{
	setObjectName("KSpreadScriptingModule");
	d->view = 0;
	d->doc = 0;
}

ScriptingModule::~ScriptingModule()
{
	kDebug() << "ScriptingModule::~ScriptingModule()" << endl;
	delete d;
}

KSpread::Doc* ScriptingModule::doc()
{
	if(! d->doc)
		d->doc = d->view ? d->view->doc() : new KSpread::Doc(0, this);
	return d->doc;
}

void ScriptingModule::setView(KSpread::View* view)
{
	d->view = view;
}

QObject* ScriptingModule::application()
{
	return KApplication::kApplication()->findChild< KoApplicationAdaptor* >();
}

QObject* ScriptingModule::document()
{
	return doc()->findChild< KoDocumentAdaptor* >();
}

QObject* ScriptingModule::map()
{
	return doc()->map()->findChild< KSpread::MapAdaptor* >();
}

QObject* ScriptingModule::view()
{
	return d->view ? d->view->findChild< KSpread::ViewAdaptor* >() : 0;
}

QObject* ScriptingModule::currentSheet()
{
	KSpread::Sheet* sheet = doc()->displaySheet();
	return sheet ? sheet->findChild< KSpread::SheetAdaptor* >() : 0;
}

QObject* ScriptingModule::sheetByName(const QString& name)
{
	if(doc()->map())
		foreach(KSpread::Sheet* sheet, doc()->map()->sheetList())
			if(sheet->sheetName() == name)
				return sheet->findChild< KSpread::SheetAdaptor* >();
	return 0;
}

QStringList ScriptingModule::sheetNames()
{
	QStringList names;
	foreach(KSpread::Sheet* sheet, doc()->map()->sheetList())
		names.append(sheet->sheetName());
	return names;
}

bool ScriptingModule::hasFunction(const QString& name)
{
	return d->functions.contains(name);
}

QObject* ScriptingModule::function(const QString& name)
{
	if( d->functions.contains(name) )
		return d->functions[name];
	ScriptingFunction* function = new ScriptingFunction(this);
	function->setName(name);
	d->functions.insert(name, function);
	d->functionnames.append(name);
	return function;
}

bool ScriptingModule::fromXML(const QString& xml)
{
	KoXmlDocument xmldoc;
	if(! xmldoc.setContent(xml, true))
		return false;
	return doc()->loadXML(0, xmldoc);
}

QString ScriptingModule::toXML()
{
	return doc()->saveXML().toString(2);
}

bool ScriptingModule::openUrl(const QString& url)
{
    return doc()->openURL(url);
}

bool ScriptingModule::saveUrl(const QString& url)
{
	return doc()->saveAs(url);
}

bool ScriptingModule::importUrl(const QString& url)
{
	return doc()->import(url);
}

bool ScriptingModule::exportUrl(const QString& url)
{
	return doc()->exp0rt(url);
}

#include "ScriptingModule.moc"
