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

#ifndef KOMACRO_METAMETHOD_H
#define KOMACRO_METAMETHOD_H

#include <qstring.h>
#include <qvaluelist.h>
#include <ksharedptr.h>

#include "komacro_export.h"

struct QUObject;

namespace KoMacro {

	// forward declarations.
	class Variable;
	class MetaObject;
	class MetaParameter;
	class MetaProxy;

	/**
	* Class to provide abstract methods for the undocumented 
	* Qt3 QUObject-API functionality.
	*
	* The design tried to limit future porting to Qt4 by providing a
	* somewhat similar API to the Qt4 QMeta* stuff.
	*/
	class KOMACRO_EXPORT MetaMethod : public KShared
	{
		public:

			/**
			* The type of method this @a MetaMethod provides
			* access to.
			*/
			enum Type {
				Signal, /// The @a MetaMethod points to a Qt signal.
				Slot, /// The @a MetaMethod points to a Qt slot.
				Unknown /// The @a MetaMethod is not known.
			};

			/**
			* Constructor.
			*
			* @param signature The signature this @a MetaMethod has. This
			* includes the tagname and the arguments and could look like
			* "myslot(const QString&, int)".
			* @param type The @a MetaMethod::Type the @a MethodMethod
			* has.
			* @param object The @a MetaObject this @a MethodMethod
			* belongs to. Each @a MethodMethod is associated with
			* exactly one @a MetaObject .
			*/
			explicit MetaMethod(const QString& signature, Type type = Unknown, KSharedPtr<MetaObject> object = 0);

			/**
			* Destructor.
			*/
			~MetaMethod();

			/**
			* @return the @a MetaObject instance this @a MethodMethod
			* belongs to.
			*/
			KSharedPtr<MetaObject> const object() const;

			/**
			* @return the signature this @a MetaMethod has. It could
			* be something like "mySlot(const QString&,int)".
			*/
			const QString signature() const;

			/**
			* @return the signatures tagname this @a MetaMethod has.
			* At the signature "mySlot(const QString&,int)" the
			* tagname would be "mySlot".
			*/
			const QString signatureTag() const;

			/**
			* @return the signatures arguments this @a MetaMethod has.
			* At the signature "mySlot(const QString&,int)" the
			* arguments are "const QString&,int".
			*/
			const QString signatureArguments() const;

			/**
			* @return the @a Type of method this @a MetaMethod provides
			* access to.
			*/
			Type type() const;

			/**
			* @return the signature arguments as parsed list of
			* @a MetaParameter instances.
			*/
			QValueList< KSharedPtr<MetaParameter> > arguments() const;

			/**
			* Translate the passed @p arguments list of @a Variable instances
			* into a Qt3 QUObject* array.
			*/
			QUObject* toQUObject(QValueList< KSharedPtr<Variable> > arguments);

			/**
			* Translate the passed @p uo QUObject reference into an internal used
			* @a Variable instances.
			*/
			KSharedPtr<Variable> toVariable(QUObject* uo);

			/**
			* Translate the passed @p uo QUObject array into an internal used
			* list of @a Variable instances.
			*/
			QValueList< KSharedPtr<Variable> > toVariableList(QUObject* uo);

			/**
			* Invoke the @a MetaMethod with the optional arguments
			* @p arguments and return a variable.
			*/
			KSharedPtr<Variable> invoke(QValueList< KSharedPtr<Variable> > arguments);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
