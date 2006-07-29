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

#include "exception.h"

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Exception::Private
	{
		public:

			/// A describing errormessage.
			const QString errormessage;

			/// A more detailed list of tracemessages.
			QString tracemessages;

			/**
			* Constructor.
			*/
			Private(const QString& errormessage)
				: errormessage(errormessage)
			{
			}

	};

}

//constructor
Exception::Exception(const QString& errormessage)
	: d( new Private(errormessage) ) // create the private d-pointer instance.
{
	//debuging infos
	kdDebug() << QString("Exception errormessage=\"%1\"").arg(errormessage) << endl;
}

//copy constructor
Exception::Exception (const Exception& e)
	: d( new Private( e.errorMessage() ) )
{
	d->tracemessages = e.traceMessages();
}

//deconstructor
Exception::~Exception()
{
	delete d;
}

//get d-pointer errormessage
const QString Exception::errorMessage() const
{
	return d->errormessage;
}

//get d-pointer tracemessages
const QString Exception::traceMessages() const
{
	return d->tracemessages;
}

//add a Qstring to d-pointer tracemessages
void Exception::addTraceMessage(const QString& tracemessage)
{
	//no tracemessages till now
	if(d->tracemessages.isEmpty())
		d->tracemessages = tracemessage;
	//append to existing ones
	else
		d->tracemessages += "\n" + tracemessage;
}

