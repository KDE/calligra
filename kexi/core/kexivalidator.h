/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXI_VALIDATOR_H
#define KEXI_VALIDATOR_H

#include <qvalidator.h>
#include <qvariant.h>
#include <qstring.h>

/*! KexiValidator class extends QValidator with offline-checking 
 for value's validity (check() method).
 Thus, it groups two purposes into one container:
 -string validator for line editors (online checking, "on typing")
 -offline-checking for QVariant values.

 It also offers error and warning messages for check() method. 
 You may need to reimplement: 
 -  QValidator::State IdentifierValidator::validate( QString& input, int& pos ) const;
 -  Result check(const QString &valueName, QVariant v, QString &message, QString &details);
 */
class KEXICORE_EXPORT KexiValidator : public QValidator
{
	public:
		typedef enum Result { Error = 0, Ok = 1, Warning = 2 };

		KexiValidator(QObject * parent = 0, const char * name = 0);

		/*! Checks if value \a v is ok and returns one of \a Result value:
		 \a Error is returned on error, \a Ok on success, \a Warning if there 
		 is something to warn. In any case except \a Ok, 
		 i18n'ed \a message is set and (optionally) \a details are set, 
		 e.g. for use in a message box.
		 \a valueName can be used to contruct \a message as well, for example:
		 "[valueName] is not a valid login name".
		 For reimplementation, by default always returns \a Error.*/
		virtual Result check(const QString &valueName, QVariant v, QString &message,
			QString &details);
};

#endif

