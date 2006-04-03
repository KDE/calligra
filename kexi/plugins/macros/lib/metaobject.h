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

#ifndef KOMACRO_METAOBJECT_H
#define KOMACRO_METAOBJECT_H

#include <qobject.h>
#include <ksharedptr.h>

#include "komacro_export.h"

namespace KoMacro {

	// forward declarations.
	class Variable;
	class MetaMethod;

	/**
	* Class to provide abstract access to extended QObject functionality
	* like the undocumented QUObject-API in Qt3. 
	*
	* The design tried to limit future porting to Qt4 by providing a
	* somewhat similar API to the Qt4 QMeta* stuff.
	*/
	class KOMACRO_EXPORT MetaObject : public KShared
	{
		public:

			/**
			* Shared pointer to implement reference-counting.
			*/
			typedef KSharedPtr<MetaObject> Ptr;

			/**
			* Constructor.
			* 
			* @param object The QObject instance this @a MetaObject provides
			* abstract access to.
			*/
			explicit MetaObject(QObject* const object);

			/**
			* Destructor.
			*/
			~MetaObject();

			/**
			* @return the QObject this @a MetaObject provides abstract
			* access to.
			*/
			QObject* const object() const;

			//QStrList signalNames() const;
			//QStrList slotNames() const;

			/**
			* @return the index of the signal @p signal .
			*/
			int indexOfSignal(const char* signal) const;

			/**
			* @return the index of the slot @p slot .
			*/
			int indexOfSlot(const char* slot) const;

			/**
			* @return the @a MetaMethod that matches to the
			* index @p index .
			*/
			KSharedPtr<MetaMethod> method(int index);

			/**
			* @return a @a MetaMethod for the signal @p signal .
			*/
			KSharedPtr<MetaMethod> signal(const char* signal);

			/**
			* @return a @a MetaMethod for the slot @p slot .
			*/
			KSharedPtr<MetaMethod> slot(const char* slot);

//KSharedPtr<MetaMethod> addSlot(const char* slot);
//void connectSignal(QObject* obj, const char* signal);

			/**
			* Invoke the @a MetaMethod that has the index @p index .
			* 
			* @param index The index the signal or slot has. Use
			* @a indexOfSignal() and @a indexOfSlot() to determinate
			* those index.
			* @param arguments The optional arguments passed to the
			* method.
			* @return The returnvalue the method provides and that got
			* returned if the execution is done.
			*/
			KSharedPtr<Variable> invokeMethod(int index, QValueList< KSharedPtr<Variable> > arguments);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
