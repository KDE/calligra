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

#include "kexi.h"

#include <kdebug.h>

using namespace Kexi;

KEXICORE_EXPORT KexiDBConnectionSet Kexi::connset;
KEXICORE_EXPORT KexiProjectSet Kexi::recentProjects;
KEXICORE_EXPORT KexiDB::DriverManager Kexi::driverManager;
KEXICORE_EXPORT KexiPart::Manager Kexi::partManager;

//--------------------------------------------------------------------------------

KEXICORE_EXPORT QString Kexi::string2FileName(const QString &s)
{
	QString fn = s.simplifyWhiteSpace();
	fn.replace(' ',"_"); fn.replace('$',"_");
	fn.replace('\\',"-"); fn.replace('/',"-"); 
	fn.replace(':',"-"); fn.replace('*',"-"); 
	return fn;
}

KEXICORE_EXPORT QString Kexi::string2Identifier(const QString &s)
{
	QString r, id = s.simplifyWhiteSpace();
	if (id.isEmpty())
		return id;
//		return "_";
	id.replace(' ',"_"); 
	QChar c = id[0].upper();
	if (!(c>='A' && c<='Z') && c!='_')
		r="_";
	r+=id[0];
	for (uint i=1; i<id.length(); i++) {
		QChar c = id[i].upper();
		if (!(c>='A' && c<='Z') && !(c>='0' && c<='9') && c!='_')
			r+='_';
		else
			r+=id[i];
	}
	return r;
}

//--------------------------------------------------------------------------------

IdentifierValidator::IdentifierValidator(QObject * parent, const char * name)
: QValidator(parent,name)
{
}

IdentifierValidator::~IdentifierValidator()
{
}

QValidator::State IdentifierValidator::validate( QString& input, int& pos) const
{
	uint i;
	for (i=0; i<input.length() && input[i]==' '; i++)
		;
	pos -= i; //i chars will be removed from beginning
	if (i<input.length() && input[i]>='0' && input[i]<='9')
		pos++; //_ will be added at the beginning
	bool addspace = (input.right(1)==" ");
	input = string2Identifier(input);
	if (addspace)
		input += "_";
	if((uint)pos>input.length())
		pos=input.length();
	return input.isEmpty() ? Valid : Acceptable;
}

//--------------------------------------------------------------------------------

