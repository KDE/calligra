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

#ifndef KEXIMACRO_NAVIGATEACTION_H
#define KEXIMACRO_NAVIGATEACTION_H

#include "../lib/action.h"
#include "../lib/variable.h"

#include "kexiaction.h"

class KexiMainWindow;

namespace KoMacro {
	class Context;
}

namespace KexiMacro {

	/**
	* The NavigateAction class implements a @a KoMacro::Action
	* to provide functionality to execute an object like
	* e.g. a script or a macro.
	*/
	class NavigateAction : public KexiAction
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			*
			* @param mainwin Kexi's main window implementation.
			*/
			NavigateAction();
			
			/**
			* Destructor.
			*/
			virtual ~NavigateAction();

		public slots:

			/**
			* Called if the @a Action should be executed within the
			* defined @p context .
			*/
			virtual void activate(KSharedPtr<KoMacro::Context> context);

	};
}

#endif
