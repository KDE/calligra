/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef __KEXI_H__
#define __KEXI_H__

#include "kexiprojectdata.h"
#include "kexipartmanager.h"
#include "startup/KexiDBConnectionSet.h"
#include "startup/KexiProjectSet.h"
#include <kexidb/drivermanager.h>

#include <qvalidator.h>

namespace Kexi
{
	//! A set of known connections
	KEXICORE_EXPORT KexiDBConnectionSet& connset();
	
	//! A set avaiulable of project infos
	KEXICORE_EXPORT KexiProjectSet& recentProjects();
	
	//! shared driver manager
	KEXICORE_EXPORT KexiDB::DriverManager& driverManager();
	
	//! shared part manager
	KEXICORE_EXPORT KexiPart::Manager& partManager();

	//some utils
	
	//! \return valid filename based on \a s
	KEXICORE_EXPORT QString string2FileName(const QString &s);
	
	/*! always returns valid identifier based on \a s.
	 Non alphanumeric chars (or spaces) are replaced with '_'.
	 If a number char is at the beginning, '_' is added at start.
	 Empty strings are not changed.
	*/
	KEXICORE_EXPORT QString string2Identifier(const QString &s);
	
	//! class validates input for identifier name
	class KEXICORE_EXPORT IdentifierValidator : public QValidator
	{
		public:
			IdentifierValidator(QObject * parent, const char * name = 0);
			~IdentifierValidator();
			virtual State validate( QString & input, int & pos) const;
	};

}

//! sometimes we leave a space in the form of empty QFrame and want to insert here
//! a widget that must be instantiated by hand.
//! This macro inserts a widget \a what into a frame \a where.
#define GLUE_WIDGET(what, where) \
	{ QVBoxLayout *lyr = new QVBoxLayout(where); \
	  lyr->addWidget(what); }

#endif

