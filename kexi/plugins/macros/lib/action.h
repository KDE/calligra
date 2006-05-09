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
#include <ksharedptr.h>

// Forward declarations.
class QDomElement;

#include "manager.h"
#include "context.h"
#include "variable.h"

namespace KoMacro {

	// Forward declarations.
	//class Manager;

	/**
	 * The Action class extendes KAction to implement some additional
	 * functionality KAction doesn't provide.
	 */
	class KOMACRO_EXPORT Action
		: public QObject // Qt functionality like signals and slots
		, public KShared // shared reference-counting
	{
			Q_OBJECT

			/// Property to get/set the name.
			Q_PROPERTY(QString name READ name WRITE setName)

			/// Property to get/set the text.
			Q_PROPERTY(QString text READ text WRITE setText)

			/// Property to get/set the comment.
			Q_PROPERTY(QString comment READ comment WRITE setComment)

			/// Property to get/set the blocking enabled.
			Q_PROPERTY(bool blocking READ isBlocking WRITE setBlocking)

		public:

			/**
			* Shared pointer to implement reference-counting.
			*/
			typedef KSharedPtr<Action> Ptr;
			
			/**
			* Shared pointer to implement reference-counting.
			*/
			typedef QMap<QString, KSharedPtr<Action> > Map;

			/**
			* Constructor.
			*
			* @param name The unique name this @a Action has.
			* @param text The i18n-caption text this @a Action has.
			*/
			explicit Action(const QString& name, const QString& text = QString::null);

			/**
			* Destructor.
			*/
			virtual ~Action();

			/**
			* @return a string representation of the functionality
			* this action provides.
			*/
			virtual const QString toString() const;

			/**
			* The name this @a Action has.
			*/
			const QString name() const;
			
			/**
			* Set the name of the @a Action to @p name .
			*/
			void setName(const QString& name); 

			/**
			* @return the i18n-caption text this @a Action has.
			*/
			const QString text() const;

			/**
			* Set the i18n-caption text this @a Action has.
			*/
			void setText(const QString& text);

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

			/**
			* @return true if there exists a variable with the
			* name @p name else false is returned.
			*/
			bool hasVariable(const QString& name) const;

			/**
			* @return the variable @a Variable defined for the
			* name @p name . If there exists no @a Variable with
			* such a name, NULL is returned.
			*/
			Variable::Ptr variable(const QString& name) const;

			/**
			* @return the map of variables this @a Action provides.
			*/
			Variable::Map variables() const;

			/**
			* @return a list of variablenames this @a Action provides.s
			*/
			QStringList variableNames() const;

			/**
			 * Append the @a Variable @p variable to list of variables
			 * this @a Action provides.
			 */
			void setVariable(Variable::Ptr variable);

			/**
			 * Set the variable.
			 *
			 * @param name The name the variable should have.
			 * @param text The i18n-caption used for display.
			 * @param variant The QVariant value.
			 */
			void setVariable(const QString& name, const QString& text, const QVariant& variant);

			/**
			* Remove the variable defined with @p name . If there exists
			* no such variable, nothing is done.
			*/
			void removeVariable(const QString& name);

			/**
			* This function is called, when a @a Variable provided by this
			* @a Action is changed.
			*/
			virtual Variable::List notifyUpdated(const QString& variablename, Variable::Map variablemap) {
				Q_UNUSED(variablename);
				Q_UNUSED(variablemap);
				// The default implementation does nothing.
				// So, an empty list is returned.
				return Variable::List();
			}

		public slots:

			/**
			* Called if the @a Action should be executed within the
			* defined @p context .
			*/
			virtual void activate(Context::Ptr context) = 0;

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
