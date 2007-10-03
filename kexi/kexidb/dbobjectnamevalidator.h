/* This file is part of the KDE project
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDBOBJECTNAMEVALIDATOR_H
#define KEXIDBOBJECTNAMEVALIDATOR_H

#include "global.h"
#include <kexiutils/validator.h>
#include <qstring.h>
#include <qpointer.h>

namespace KexiDB {

	class Driver;

	/*! Validates input: 
	 accepts if the name is not reserved for internal kexi objects. */
	class KEXI_DB_EXPORT ObjectNameValidator : public KexiUtils::Validator
	{
		public:
			/*! \a drv is a KexiDB driver on which isSystemObjectName() will be 
			 called inside check(). If \a drv is 0, KexiDB::Driver::isKexiDBSystemObjectName()
			 static function is called instead. */
			ObjectNameValidator(KexiDB::Driver *drv, QObject * parent = 0);
			virtual ~ObjectNameValidator();

		protected:
			virtual KexiUtils::Validator::Result internalCheck(const QString &valueName, const QVariant& v, 
				QString &message, QString &details);
			QPointer<KexiDB::Driver> m_drv;
	};
}

#endif
