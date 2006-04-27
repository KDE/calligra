/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
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

#ifndef KEXIMACRO_OBJECTVARIABLE_H
#define KEXIMACRO_OBJECTVARIABLE_H

#include "../lib/action.h"
#include "../lib/variable.h"

#include <kdebug.h>

namespace KexiMacro {

	/**
	* The ObjectVariable class implements @a KoMacro::Variable to
	* provide a variable list of Kexi-objects. Those Kexi-objects
	* are KexiPart's like e.g. table, query, form or script.
	*/
	class ObjectVariable : public KoMacro::GenericVariable<ObjectVariable>
	{
		public:

			/**
			* Constructor.
			*
			* @param action The @a KoMacro::Action instance
			* this @a ObjectVariable is child of.
			*/
			explicit ObjectVariable(KoMacro::Action::Ptr action);

			/**
			* Destructor.
			*/
			virtual ~ObjectVariable();

	};

}

#endif
