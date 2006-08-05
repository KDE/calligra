/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KEXIMACRO_DATATABLEACTION_H
#define KEXIMACRO_DATATABLEACTION_H

#include "kexiaction.h"

class KexiMainWindow;

namespace KoMacro {
	class Context;
}

namespace KexiMacro {

	/**
	* The DataTableAction class implements a @a KoMacro::Action
	* to provide functionality to import or export a datatable.
	* The datatable is used to deal with comma separated values.
	*/
	class DataTableAction : public KexiAction
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			*/
			DataTableAction();
			
			/**
			* Destructor.
			*/
			virtual ~DataTableAction();

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
			virtual bool notifyUpdated(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name);

		public slots:

			/**
			* Called if the @a Action should be executed within the
			* defined @p context .
			*/
			virtual void activate(KSharedPtr<KoMacro::Context> context);

	};
}

#endif
