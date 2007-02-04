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
#include "ScriptingWidgets.h"

#include <QPointer>
#include <QApplication>
#include <QLayout>
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
    KDE_EXPORT QObject* krossmodule()
    {
        return new ScriptingModule();
    }
}

/// \internal d-pointer class.
class ScriptingModule::Private
{
	public:
		QPointer<KSpread::Doc> doc;
		QHash< QString, ScriptingFunction* > functions;
		QStringList functionnames;
};

ScriptingModule::ScriptingModule()
	: KoScriptingModule("KSpread")
	, d( new Private() )
{
	d->doc = 0;
}

ScriptingModule::~ScriptingModule()
{
	kDebug() << "ScriptingModule::~ScriptingModule()" << endl;
	delete d;
}

KSpread::View* ScriptingModule::kspreadView()
{
    return dynamic_cast< KSpread::View* >( KoScriptingModule::view() );
}

KSpread::Doc* ScriptingModule::kspreadDoc()
{
	if(! d->doc) {
		if( KSpread::View* v = kspreadView() )
			d->doc = v->doc();
		if( ! d->doc )
			d->doc = new KSpread::Doc(0, this);
	}
	return d->doc;
}

KoDocument* ScriptingModule::doc()
{
	return kspreadDoc();
}

QObject* ScriptingModule::map()
{
	return kspreadDoc()->map()->findChild< KSpread::MapAdaptor* >();
}

QObject* ScriptingModule::view()
{
	KSpread::View* v = kspreadView();
	return v ? v->findChild< KSpread::ViewAdaptor* >() : 0;
}

QObject* ScriptingModule::currentSheet()
{
	KSpread::Sheet* sheet = kspreadDoc()->displaySheet();
	return sheet ? sheet->findChild< KSpread::SheetAdaptor* >() : 0;
}

QObject* ScriptingModule::sheetByName(const QString& name)
{
	if(kspreadDoc()->map())
		foreach(KSpread::Sheet* sheet, kspreadDoc()->map()->sheetList())
			if(sheet->sheetName() == name)
				return sheet->findChild< KSpread::SheetAdaptor* >();
	return 0;
}

QStringList ScriptingModule::sheetNames()
{
	QStringList names;
	foreach(KSpread::Sheet* sheet, kspreadDoc()->map()->sheetList())
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
	return kspreadDoc()->loadXML(0, xmldoc);
}

QString ScriptingModule::toXML()
{
	return kspreadDoc()->saveXML().toString(2);
}

bool ScriptingModule::openUrl(const QString& url)
{
    return kspreadDoc()->openURL(url);
}

bool ScriptingModule::saveUrl(const QString& url)
{
	return kspreadDoc()->saveAs(url);
}

bool ScriptingModule::importUrl(const QString& url)
{
	return kspreadDoc()->import(url);
}

bool ScriptingModule::exportUrl(const QString& url)
{
	return kspreadDoc()->exp0rt(url);
}

QWidget* ScriptingModule::createSheetsListView(QWidget* parent)
{
	ScriptingSheetsListView* listview = new ScriptingSheetsListView(this, parent);
	if( parent && parent->layout() )
		parent->layout()->addWidget(listview);
	return listview;
}

#include "ScriptingModule.moc"
