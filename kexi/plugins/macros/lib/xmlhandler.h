/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
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

#ifndef KOMACRO_XMLHANDLER_H
#define KOMACRO_XMLHANDLER_H

//#include <qmap.h>
//#include <qguardedptr.h>
//#include <ksharedptr.h>
//#include <kxmlguiclient.h>
#include "macro.h"

class QObject;
class QDomElement;

namespace KoMacro {

	/**
	* The XMLHandler class manages the (un-)serialization of
	* a @a Macro instance to/from XML.
	*/
	class KOMACRO_EXPORT XMLHandler
	{
		public:

			/**
			* Constructor to init a @a XMLHandler .
			* @param macro The @a Macro instance which will
			* be managed.
			*/
			XMLHandler(Macro::Ptr macro);

			/**
			* Destructor to @a XMLHandler .
			*/
			~XMLHandler();

			/**
			* Reads a given @a QDomElement, extracts given
			* Actions into the managed Macro-Instance.
			* @param element The @a QDomElement within
			* the @a Macro.
			*/
			bool fromXML(const QDomElement& element);

			/**
			* Converts the macro to a @a QDomElement.
			* @return The resulten @a QDomElement from
			* the @a Macro.
			*/
			QDomElement toXML();

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};
}

#endif
