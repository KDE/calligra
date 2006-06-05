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

#ifndef KEXIMACRO_KEXIACTION_H
#define KEXIMACRO_KEXIACTION_H

#include "../lib/manager.h"
#include "../lib/exception.h"
#include "../lib/action.h"
#include "../lib/variable.h"

#include <core/keximainwindow.h>

#include <ksharedptr.h>

namespace KoMacro {
	class Context;
}

namespace KexiMacro {

	/**
	* Template class to offer common functionality needed by all
	* @a KoMacro::Action implementations Kexi provides.
	*
	* All the actions Kexi provides are inherited from this
	* template class.
	*/
	class KexiAction : public KoMacro::Action
	{
		public:

			/**
			* Constructor.
			*
			* @param mainwin Kexi's main window implementation.
			* @param name The unique name the @a KoMacro::Action has. This
			* name will be used to identify the action.
			* @param text The i18n-caption text used for display purposes.
			*/
			KexiAction(const QString& name, const QString& text)
				: KoMacro::Action(name)
			{
				m_mainwin = dynamic_cast< KexiMainWindow* >( KoMacro::Manager::self()->guiClient() );
				if(! m_mainwin) {
					throw KoMacro::Exception("Invalid KexiMainWindow instance.", "KexiMacro::KexiAction::KexiAction()");
				}

				// Set the caption this action has.
				setText(text);

				// Publish this action.
				KoMacro::Manager::self()->publishAction( KSharedPtr<Action>(this) );
			}

			/**
			* @return the @a KexiMainWindow instance we are
			* running in.
			*/
			KexiMainWindow* mainWin() { return m_mainwin; }

		private:

			/// The @a KexiMainWindow instance.
			KexiMainWindow* m_mainwin;

	};
}

#endif
