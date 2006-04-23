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

#ifndef KOMACRO_MACRO_H
#define KOMACRO_MACRO_H

#include <qobject.h>

#include "action.h"
#include "xmlhandler.h"

// Forward declarations.
class QDomElement;

namespace KoMacro {

	// Forward declarations.
	class Manager;
	class MacroItem;

	/**
	* The Macro class implements all the action-handling. Internaly the
	* Macro provides a collection of @a MacroItem instances which each
	* of them points to an @a Action instance.
	*/
	class KOMACRO_EXPORT Macro
		: public Action
		, public XMLHandler
	{
			Q_OBJECT

		public:

			/**
			* Shared pointer to implement reference-counting.
			*/
			typedef KSharedPtr<Macro> Ptr;

			/**
			* A QMap of @a Macro instances accessible by there unique name. Each
			* class should use this typemap rather then the QMap direct. That
			* way we are more flexible on future changes.
			*/
			typedef QMap<QString, Macro::Ptr> Map;

			/**
			* Constructor.
			*
			* @param name The internal name this @a Macro has. This
			* name will be used as unique identifier.
			*/
			explicit Macro(const QString& name);

			/**
			* Destructor.
			*/
			virtual ~Macro();

			/**
			* @return a string-representation of the macro.
			*/
			virtual const QString toString() const;

			/**
			* @return a list of @a MacroItem instances which
			* are children of this @a Macro .
			*/
			QValueList< KSharedPtr<MacroItem> > items() const;

			/**
			* Add the @a MacroItem @p item to the list of items
			* this @a Macro has.
			*/
			void addItem(KSharedPtr<MacroItem> item);

			/**
			* Removes all @a MacroItem instances this @a Macro has.
			*/
			void clearItems();

			/**
			* Connect the Qt signal @p signal of the QObject @p sender
			* with this @a Macro . If the signal got emitted this
			* @a Macro instance will be activated and the in the
			* signal passed arguments are transfered into the
			* activation @a Context .
			*/
			void connectSignal(const QObject* sender, const char* signal);

		public slots:

			/**
			* Called if the @a Macro should be executed.
			*/
			virtual void activate();

			/**
			* Called if the @a Macro should be executed.
			* 
			* @param context The @a Context this @a Macro should
			* be executed in.
			*/
			virtual void activate(Context::Ptr context);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
