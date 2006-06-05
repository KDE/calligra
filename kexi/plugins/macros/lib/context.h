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

#ifndef KOMACRO_CONTEXT_H
#define KOMACRO_CONTEXT_H

#include <qobject.h>
#include <ksharedptr.h>

#include "variable.h"

namespace KoMacro {

	// Forward declaration.
	class Macro;
	class MacroItem;
	class Action;
	class Exception;

	/**
	* The context of an execution. If a @a Macro got executed it creates
	* an instance of this class and passes it around all it's children
	* as local execution context.
	*/
	class KOMACRO_EXPORT Context
		: public QObject
		, public KShared
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			* 
			* @param macro The @a Macro this @a Context belongs to.
			*/
			explicit Context(KSharedPtr<Macro> macro);

			/**
			* Destructor.
			*/
			~Context();

			/**
			* @return true if there exists a variable with name @p name
			* else false got returned.
			*/
			bool hasVariable(const QString& name) const;

			/**
			* @return the @a Variable defined with name @p name or
			* NULL if there exists no such variable.
			*/
			KSharedPtr<Variable> variable(const QString& name) const;

			/**
			* @return a map of all @a Variable instance that are defined
			* within this context.
			*/
			Variable::Map variables() const;

			/**
			* Set the variable @p variable defined with name @p name . If
			* there exists already a variable with that name replace it.
			*/
			void setVariable(const QString& name, KSharedPtr<Variable> variable);

			/**
			* @return the associated macro 
			*/
			KSharedPtr<Macro> macro() const;

			/**
			* @return the currently selected @a MacroItem instance
			* or NULL if there is no @a MacroItem selected yet.
			*/
			KSharedPtr<MacroItem> macroItem() const;

			/**
			* @return true if the last @a activate() stopped with an
			* exception else false is returned.
			*/
			bool hadException() const;

			/**
			* @return the @a Exception instance that was thrown on
			* the last call of @a activate() . If there was no
			* exception NULL is returned.
			*/
			Exception* exception() const;

		private slots:

			/**
			* A @a Context does take care of an execution-chain which
			* should be activated one after another. The @a Context
			* remembers what @a Action should be executed next and
			* calling this slot just activates those @a Action .
			*/
			virtual void activate(QValueList<KSharedPtr <MacroItem> >::ConstIterator it);

		public slots:

			/**
			* This slot extends the slot above with the passed
			* @a Context @p context which will be used as
			* parent context for this context.
			*/
			virtual void activate(KSharedPtr<Context> context);

			/**
			* This slot continues execution.
			*/
			virtual void activateNext();

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
