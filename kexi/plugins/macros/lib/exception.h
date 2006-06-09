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

#ifndef KOMACRO_EXCEPTION_H
#define KOMACRO_EXCEPTION_H

#include <qstring.h>
#include <qstringlist.h>

#include "komacro_export.h"

namespace KoMacro {

	/**
	* Base Exception class. All exceptions we like to use within KoMacro
	* need to inheritate from this exception.
	*/
	class KOMACRO_EXPORT Exception
	{
		public:

			/**
			* Constructor.
			*
			* @param errormessage A describing errormessage why the
			* exception got thrown.
			*/
			explicit Exception(const QString& errormessage);

			/**
			* Copy-constructor.
			*/
			Exception(const Exception&);

			/**
			* Destructor.
			*/
			virtual ~Exception();

			/**
			* @return a describing errormessage.
			*/
			const QString errorMessage() const;

			/**
			* @return a stringlist of traces. This are normaly just
			* simple strings to show the way the exception was gone
			* from bottom-up where the error was thrown till where
			* we finally catched the error to display it to the
			* user.
			*/
			const QString traceMessages() const;

			/**
			* Add the message @p tracemessage to the list of traces.
			*/
			void addTraceMessage(const QString& tracemessage);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
