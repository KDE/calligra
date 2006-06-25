/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
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

#include "manager.h"
#include "action.h"
#include "function.h"
#include "macro.h"
#include "exception.h"

#include <qobject.h>
#include <qwidget.h>
#include <qdom.h>
#include <kxmlguibuilder.h>
#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Manager::Private
	{
		public:
			KXMLGUIClient* const xmlguiclient;
			QMap<QString, KSharedPtr<Macro > > macros;

			QStringList actionnames;
			QMap<QString, KSharedPtr<Action> > actions;

			QMap<QString, QGuardedPtr<QObject> > objects;

			Private(KXMLGUIClient* const xmlguiclient)
				: xmlguiclient(xmlguiclient)
			{
			}
	};

	/// Pointer to our static singleton.
	static ::KoMacro::Manager* _self = 0;

	/// Automatically deletes our singleton on termination.
	static KStaticDeleter< ::KoMacro::Manager > _manager;

}

void Manager::init(KXMLGUIClient* xmlguiclient)
{
	if(! _self) {
		::KoMacro::Manager* manager = new ::KoMacro::Manager(xmlguiclient);
		_manager.setObject(_self, manager);
	}
	else {
		throw Exception("Already initialized.");
	}
}

Manager* Manager::self()
{
	//Q_ASSERT(_self);
	return _self;
}

Manager::Manager(KXMLGUIClient* const xmlguiclient)
	: d( new Private(xmlguiclient) ) // create the private d-pointer instance.
{
	kdDebug() << "Manager::Manager() Ctor" << endl;
	QObject* obj = dynamic_cast<QObject*>(xmlguiclient);
	if(obj) {
		d->objects.replace(obj->name(), obj);
	}

	//TESTCASE
	d->objects.replace("TestCase", new QWidget());
}

Manager::~Manager()
{
	// destroy the private d-pointer instance.
	delete d;
}

KXMLGUIClient* Manager::guiClient() const
{
	return d->xmlguiclient;
}

bool Manager::hasMacro(const QString& macroname)
{
	return d->macros.contains(macroname);
}

KSharedPtr<Macro> Manager::getMacro(const QString& macroname)
{
	return d->macros[macroname];
}

void Manager::addMacro(const QString& macroname, KSharedPtr<Macro> macro)
{
	d->macros.replace(macroname, macro);
}

void Manager::removeMacro(const QString& macroname)
{
	d->macros.remove(macroname);
}

KSharedPtr<Macro> Manager::createMacro(const QString& macroname)
{
	KSharedPtr<Macro> macro = KSharedPtr<Macro>( new Macro(macroname) );
	return macro;
}

KSharedPtr<Action> Manager::action(const QString& name) const
{
	return d->actions[name];
}

Action::Map Manager::actions() const
{
	return d->actions;
}

QStringList Manager::actionNames() const
{
	return d->actionnames;
}

void Manager::publishAction(KSharedPtr<Action> action)
{
	const QString name = action->name();
	if(! d->actions.contains(name)) {
		d->actionnames.append(name);
	}
	d->actions.replace(name, action);
}

void Manager::publishObject(const QString& name, QObject* object)
{
	Q_ASSERT(! d->objects.contains(name));
	d->objects.replace(name, object);
}

QGuardedPtr<QObject> Manager::object(const QString& name) const
{
	return d->objects[name];
}

QMap<QString, QGuardedPtr<QObject> > Manager::objects() const
{
	return d->objects;
}
