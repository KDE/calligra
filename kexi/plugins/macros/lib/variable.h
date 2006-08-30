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

#ifndef KOMACRO_VARIABLE_H
#define KOMACRO_VARIABLE_H

#include <qobject.h>
#include <qdom.h>
#include <qvariant.h>
#include <ksharedptr.h>

#include "metaparameter.h"

namespace KoMacro {

	/**
	* A variable value used to provide abstract access to variables. The
	* class handles QVariant and QObject and provides access to them.
	* Variable inherits KShared and implements reference couting. So, it's
	* not needed to take care of memory-managment.
	*/
	class KOMACRO_EXPORT Variable : public MetaParameter
	{

			/**
			* Property to get and set a QVariant as variable.
			*/
			Q_PROPERTY(QVariant variant READ variant WRITE setVariant)

			/**
			* Property to get and set a QObject as variable.
			*/
			Q_PROPERTY(QObject* object READ object WRITE setObject)

			/**
			* Property to get a string-representation of the variable.
			*/
			Q_PROPERTY(QString string READ toString)

		public:

			/**
			* A list of variables.
			*/
			typedef QValueList<KSharedPtr<Variable > > List;

			/**
			* A map of variables.
			*/
			typedef QMap<QString, KSharedPtr<Variable > > Map;

			/**
			* Default constructor.
			*/
			explicit Variable();

			/**
			* Constructor from the QVariant @p variant .
			* 
			* @param variant The value this variable has.
			* @param name The unique @a name() this variable has.
			* @param text The describing @a text() this variable has.
			*/
			Variable(const QVariant& variant, const QString& name = QString::null, const QString& text = QString::null);

			/**
			* Constructor from the QObject @p object .
			*
			* @param object The value this variable has.
			*/
			Variable(const QObject* object);

			/**
			* Constructor from the QDomElement @p element .
			* @deprecated replaced with methods of @a XMLHandler.
			* @param element The QDomElement that may optional contains the
			* variable content or other additional informations.
			*/
			Variable(const QDomElement& element);

			/**
			* Destructor.
			*/
			virtual ~Variable();

			/**
			* @return the name this @a Variable has.
			*/
			QString name() const;

			/**
			* Set the name @param name this @a Variable has.
			*/
			void setName(const QString& name);

			/**
			* @return the caption this @a Variable has.
			*/
			QString text() const;

			/**
			* Set the caption @param text this @a Variable has.
			*/
			void setText(const QString& text);

			/**
			* Set the QObject @param object this variable has. A 
			* previously remembered value will be overwritten and 
			* the new type is a @a TypeObject .
			*/
			void setObject(const QObject* object);

			/**
			* @return the QVariant this variable has. If this
			* variable isn't a @a TypeVariant an invalid QVariant
			* got returned.
			*/
			const QVariant variant() const;

			/**
			* Set the QVariant @param variant this variable has. A 
			* previously remembered value will be overwritten and 
			* the new type is a @a TypeVariant . If @param detecttype is
			* true the method tries to set the @a variantType according
			* to the passed QVariant. If false the variantType won't
			* be changed.
			*/
			void setVariant(const QVariant& variant, bool detecttype = true);

			/**
			* @return the QObject this variable has. If this
			* variable isn't a @a TypeObject NULL got returned.
			*/
			const QObject* object() const;

			/**
			* Implicit conversion to QVariant operator. This method
			* calls @a variant() internaly.
			*/
			operator QVariant () const;

			/**
			* Implicit conversion to QObject operator. This method
			* calls @a object() internaly.
			*/
			operator const QObject* () const;

			/**
			* @return a string-represenation of the variable.
			*/
			const QString toString() const;

			/**
			* @return a integer-represenation of the variable.
			*/
			int toInt() const;

			/**
			* @return the optional list of @a Variable instances
			* that are children of this @a Variable .
			*
			* @note that the list is returned call-by-reference. The
			* list is accessed as getter/setter (read/write). So,
			* don't set this method to const!
			*/
			List children() const;

			/**
			* Append a @a Variable to the list of children this
			* @a Variable has.
			*/
			void appendChild(KSharedPtr<Variable> variable);

			/**
			* Clear the list of children this @a Variable has.
			*/
			void clearChildren();

			/**
			* Set the children this @a Variable has.
			*/
			void setChildren(const List& children);

#if 0
			/**
			* @return true if this @a Variable is enabled else
			* false is returned.
			*/
			bool isEnabled() const;

			/**
			* Set this @a Variable to be enabled if @param enabled is
			* true else the variable is disabled.
			*/
			void setEnabled(const bool enabled);
#endif
		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
