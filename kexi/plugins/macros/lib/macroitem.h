/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KOMACRO_MACROITEM_H
#define KOMACRO_MACROITEM_H

#include <qobject.h>

#include <ksharedptr.h>

// Forward declarations.
class QDomElement;

#include "action.h"
#include "context.h"

namespace KoMacro {

	// Forward-declarations.
	//class Action;

	/**
	* The MacroItem class is an item in a @a Macro and represents one
	* single execution step. Each MacroItem points to 0..1 @a Action
	* instances which implement the execution. So, the MacroItem provides
	* a simple state-pattern on the one hand (depending on the for this
	* MacroItem choosen @a Action implementation) and holds the by the
	* user defined modifications like e.g. the comment on the other hand.
	*/
	class KOMACRO_EXPORT MacroItem : public KShared
	{

		public:

			/**
			* A list of \a MacroItem instances.
			*/
			typedef QValueList<KSharedPtr<MacroItem > > List;

			/**
			* Constructor.
			*/
			explicit MacroItem();

			/**
			* Destructor.
			*/
			~MacroItem();

			/**
			* @return the comment defined by the user for
			* this @a MacroItem .
			*/
			QString comment() const;

			/**
			* Set the comment @param comment defined by the user for this
			* @a MacroItem .
			*/
			void setComment(const QString& comment);

			/**
			* @return the @a Action this @a MacroItem points
			* to. This method will return NULL if there is
			* no @a Action defined yet else the returned
			* @a Action will be used to implement the execution.
			*/
			KSharedPtr<Action> action() const;

			/**
			* Set the @a Action @param action this @a MacroItem points to.
			*/
			void setAction(KSharedPtr<Action> action);

			/**
			* @return @a Variant from the @a Variable identified with
			* the name @param name . If this @a MacroItem doesn't
			* have a @a Variable with that name NULL is
			* returned.
			* If the boolean value @param checkaction is true, we
			* also look if our @a Action may know about
			* such a @param name in the case this @a MacroItem
			* doesn't have such a name.
			*/
			QVariant variant(const QString& name, bool checkaction = false) const;

			/**
			* @return the @a Variable instance identified with
			* the name @param name . If this @a MacroItem doesn't
			* have a @a Variable with that name NULL is
			* returned.
			* If the boolean value @param checkaction is true, we
			* also look if our @a Action may know about
			* such a @param name in the case this @a MacroItem
			* doesn't have such a name.
			*/
			KSharedPtr<Variable> variable(const QString& name, bool checkaction = false) const;

			/**
			* @return a map of @a Variable instances.
			*/
			QMap<QString, KSharedPtr<Variable> > variables() const;

			/**
			* Set the @a QVariant @param variant as variable with the variablename
			* @param name .
			* @return a bool for successfull setting.
			*/
			bool setVariant(const QString& name, const QVariant& variant);

			/**
			* Add a new variable with the vaiablename @param name and the given
			* @a QVariant @param variant to our @a MacroItem instance.
			*/
			KSharedPtr<Variable> addVariable(const QString& name, const QVariant& variant);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
