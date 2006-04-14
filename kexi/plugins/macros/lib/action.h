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

#ifndef KOMACRO_ACTION_H
#define KOMACRO_ACTION_H

#include <qobject.h>

#include <kaction.h>
#include <ksharedptr.h>

// Forward declarations.
class QDomElement;

#include "context.h"

namespace KoMacro {

	// Forward declarations.
	class Manager;

	/**
	 * The Action class extendes KAction to implement some additional
	 * functionality KAction doesn't provide.
	 */
	class KOMACRO_EXPORT Action
		: public KAction
		, public KShared
	{
			Q_OBJECT

			/// Property to get/set the comment-property.
			Q_PROPERTY(QString comment READ comment WRITE setComment)

			/// Property to get/set the blocking-property.
			Q_PROPERTY(bool blocking READ isBlocking WRITE setBlocking)

		public:

			/**
			* Shared pointer to implement reference-counting.
			*/
			typedef KSharedPtr<Action> Ptr;

			/**
			* Constructor.
			*
			* @param manager The @a Manager instance this @a Action
			* belongs to.
			* @param element The QDomElement used to fill the
			* @a Action .
			*/
			Action(Manager* const manager, const QDomElement& element);

			/**
			* Destructor.
			*/
			virtual ~Action();

			/**
			* @return the @a Manager instance this action is a
			* child of.
			*/
			Manager* const manager() const;

			/**
			* @return a string representation of the functionality
			* this action provides.
			*/
			virtual const QString toString() const;

			/**
			* @return the comment associated with this action.
			*/
			const QString comment() const;

			/**
			* Set the @p comment associated with this action.
			*/
			void setComment(const QString& comment);

			/**
			 * @return true if this Action should be executed synchron (blocking)
			 * or false if execution is asynchron (nonblocking).
			 */
			bool isBlocking() const;

			/**
			 * If @p blocking is true this Action will be executed synchron and
			 * therefore blocking and if its false the execution is done
			 * asynchron in a more nonblocking way.
			 */
			void setBlocking(bool blocking);

			// do we need to have them accessible by an unique key?
			//Variable::Ptr variable(const QString& name) const;
			//void setVariable(const QString& name, Variable::Ptr) const;

			/**
			* @return the list of variables this @a Action provides.
			*/
			Variable::List variables() const;

			/**
			 * Set the list of variables this @a Action provides.
			 */
			void setVariables(Variable::List variables);

			/**
			* @return this instance as serialized QDomElement.
			*
			* @deprecated Use XMLHandler for such functionality.
			*/
			const QDomElement domElement() const;

			/**
			* Set the @p action to be used within this @a Action instance.
			*/
			void setAction(const KAction* action);

		public slots:

			/**
			* Called if the @a Action should be executed.
			*/
			virtual void activate();

			/**
			* Called if the @a Action should be executed within the
			* defined @p context .
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
