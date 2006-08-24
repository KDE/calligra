/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

class ScriptingModule::Private
{
	public:
		KSpread::View* view;
};

ScriptingModule::ScriptingModule(KSpread::View* view)
	: QObject()
	, d(new Private())
{
	setObjectName("KSpreadScriptingModule");
	d->view = view;
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
	return d->view->doc()->findChild< KoDocumentAdaptor* >();
}

QObject* ScriptingModule::view()
{
	return d->view->findChild< KSpread::ViewAdaptor* >();
}

QObject* ScriptingModule::currentSheet()
{
	KSpread::Sheet* sheet = d->view->doc() ? d->view->doc()->displaySheet() : 0;
	return sheet ? sheet->findChild< KSpread::SheetAdaptor* >() : 0;
}

QObject* ScriptingModule::sheetByName(const QString& name)
{
	if(d->view->doc() && d->view->doc()->map())
		foreach(KSpread::Sheet* sheet, d->view->doc()->map()->sheetList())
			if(sheet->sheetName() == name)
				return sheet->findChild< KSpread::SheetAdaptor* >();
	return 0;
}

QStringList ScriptingModule::sheetNames()
{
	QStringList names;
	foreach(KSpread::Sheet* sheet, d->view->doc()->map()->sheetList())
		names.append(sheet->sheetName());
	return names;
}

bool ScriptingModule::fromXML(const QString& xml)
{
	KoXmlDocument doc;
#ifdef KOXML_USE_QDOM
	if(! doc.setContent(xml, true))
		return false;
#else
#warning Problem with KoXmlReader conversion!
	kWarning() << "Problem with KoXmlReader conversion!" << endl;
#endif
	return d->view->doc() ? d->view->doc()->loadXML(0, doc) : false;
}

QString ScriptingModule::toXML()
{
	return d->view->doc() ? d->view->doc()->saveXML().toString(2) : "";
}

#include "ScriptingModule.moc"
