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

#include "metaobject.h"
#include "metamethod.h"
#include "variable.h"
#include "exception.h"

#include <qguardedptr.h>
#include <qmetaobject.h>

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class MetaObject::Private
	{
		public:

			/**
			* The QObject instance this @a MetaObject belongs to.
			*/
			QGuardedPtr<QObject> const object;

			/**
			* Constructor.
			*/
			Private(QObject* const object)
				: object(object)
			{
			}
	};

}

MetaObject::MetaObject(QObject* const object)
	: KShared()
	, d( new Private(object) ) // create the private d-pointer instance.
{
}

MetaObject::~MetaObject()
{
	delete d;
}

QObject* const MetaObject::object() const
{
	if(! d->object) {
		throw Exception(
			QString("Object is undefined."),
			"KoMacro::MetaObject::object()"
		);
	}
	return d->object;
}

/*
QStrList MetaObject::signalNames() const
{
	return object()->metaObject()->signalNames();
}

QStrList MetaObject::slotNames() const
{
	return object()->metaObject()->slotNames();
}
*/

int MetaObject::indexOfSignal(const char* signal) const
{
	QMetaObject* metaobject = object()->metaObject();
	int signalid = metaobject->findSignal(signal, false);
	if(signalid < 0) {
		throw Exception(
			QString("Invalid signal \"%1\"").arg(signal),
			"KoMacro::MetaObject::indexOfSignal()"
		);
	}
	return signalid;
}

int MetaObject::indexOfSlot(const char* slot) const
{
	QMetaObject* metaobject = object()->metaObject();
	int slotid = metaobject->findSlot(slot, false);
	if(slotid < 0) {
		throw Exception(
			QString("Invalid slot \"%1\"").arg(slot),
			"KoMacro::MetaObject::indexOfSlot()"
		);
	}
	return slotid;
}

KSharedPtr<MetaMethod> MetaObject::method(int index)
{
	QObject* obj = object();
	MetaMethod::Type type = MetaMethod::Slot;
	QMetaObject* metaobject = obj->metaObject();

	const QMetaData* metadata = metaobject->slot(index, true);
	if(! metadata) {
		// Try to get a signal with that index iff we failed to determinate
		// a matching slot.

		metadata = metaobject->signal(index, true);
		if(! metadata) {
			throw Exception(
				QString("Invalid method index \"%1\" in object \"%2\"").arg(index).arg(obj->name()),
				"KoMacro::MetaObject::method()"
			);
		}
		type = MetaMethod::Signal;
	}

	if(metadata->access != QMetaData::Public) {
		throw Exception(
			QString("Not allowed to access method \"%1\" in object \"%2\"").arg(metadata->name).arg(obj->name()),
			"KoMacro::MetaObject::method()"
		);
	}

	return new MetaMethod(metadata->name, type, this);
}

KSharedPtr<MetaMethod> MetaObject::signal(const char* signal)
{
	return method( indexOfSignal(signal) );
}

KSharedPtr<MetaMethod> MetaObject::slot(const char* slot)
{
	return method( indexOfSlot(slot) );
}

KSharedPtr<Variable> MetaObject::invokeMethod(int index, Variable::List arguments)
{
	// kdDebug() << "MetaObject::invokeMethod(int index, Variable::List arguments)" << endl;
	KSharedPtr<MetaMethod> m = method(index);
	// kdDebug() << "MetaObject::invokeMethod(int index, Variable::List arguments) return" << endl;
	return m->invoke(arguments);
}

