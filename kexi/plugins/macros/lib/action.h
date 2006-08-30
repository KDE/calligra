/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
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

#include "manager.h"
#include "context.h"
#include "variable.h"

#include <qobject.h>
#include <ksharedptr.h>
#include <qstringlist.h>

namespace KoMacro {

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

		public:

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
			* @return true if there exists a variable with the
			* name @p name else false is returned.
			*/
			bool hasVariable(const QString& name) const;

			/**
			* @return the variable @a Variable defined for the
			* name @p name . If there exists no @a Variable with
			* such a name, NULL is returned.
			*/
			KSharedPtr<Variable> variable(const QString& name) const;

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
			void setVariable(KSharedPtr<Variable> variable);

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
			* This function is called, when the @a KoMacro::Variable
			* with name @p name used within the @a KoMacro::MacroItem
			* @p macroitem got changed.
			*
			* @param macroitem The @a KoMacro::MacroItem instance where
			* the variable defined with @p name is located in.
			* @param name The name the @a KoMacro::Variable has.
			* @return true if the update was successfully else false
			* is returned.
			*/
			virtual bool notifyUpdated(const KSharedPtr<MacroItem> &macroitem, const QString& name) {
				Q_UNUSED(macroitem);
				Q_UNUSED(name);
				return true; // The default implementation does nothing.
			}

		public slots:

			/**
			* Called if the @a Action should be executed within the
			* defined @p context .
			*/
			virtual void activate(KSharedPtr<Context> context) = 0;

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
