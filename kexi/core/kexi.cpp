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

#include "kexi_utils.h"

#include <kdebug.h>
#include <klocale.h>

using namespace Kexi;

KexiDBConnectionSet _connset;
KexiProjectSet _recentProjects;
KexiDB::DriverManager _driverManager;
KexiPart::Manager _partManager;

KexiDBConnectionSet& Kexi::connset() { return _connset; }
KexiProjectSet& Kexi::recentProjects() { return _recentProjects; }
KexiDB::DriverManager& Kexi::driverManager() { return _driverManager; }
KexiPart::Manager& Kexi::partManager() { return _partManager; }

//temp
bool _tempShowForms = 0;
bool& Kexi::tempShowForms() { return _tempShowForms; }

//--------------------------------------------------------------------------------

QString Kexi::string2FileName(const QString &s)
{
	QString fn = s.simplifyWhiteSpace();
	fn.replace(' ',"_"); fn.replace('$',"_");
	fn.replace('\\',"-"); fn.replace('/',"-"); 
	fn.replace(':',"-"); fn.replace('*',"-"); 
	return fn;
}

QString Kexi::string2Identifier(const QString &s)
{
	QString r, id = s.simplifyWhiteSpace();
	if (id.isEmpty())
		return id;
//		return "_";
	id.replace(' ',"_"); 
	QChar c = id[0].lower();
	if (!(c>='a' && c<='z') && c!='_')
		r="_";
	else
		r+=id[0];
	if (c>='0' && c<='9')
		r+=c;
	for (uint i=1; i<id.length(); i++) {
		QChar c = id.at(i).lower();
		if (!(c>='a' && c<='z') && !(c>='0' && c<='9') && c!='_')
			r+='_';
		else
			r+=c;
	}
	return r;
}

//--------------------------------------------------------------------------------

IdentifierValidator::IdentifierValidator(QObject * parent, const char * name)
: KexiValidator(parent,name)
{
}

IdentifierValidator::~IdentifierValidator()
{
}

QValidator::State IdentifierValidator::validate( QString& input, int& pos ) const
{
	uint i;
	for (i=0; i<input.length() && input.at(i)==' '; i++)
		;
	pos -= i; //i chars will be removed from beginning
	if (i<input.length() && input.at(i)>='0' && input.at(i)<='9')
		pos++; //_ will be added at the beginning
	bool addspace = (input.right(1)==" ");
	input = string2Identifier(input);
	if (addspace)
		input += "_";
	if((uint)pos>input.length())
		pos=input.length();
	return input.isEmpty() ? Valid : Acceptable;
}

KexiValidator::Result IdentifierValidator::internalCheck(
	const QString &valueName, const QVariant& v, 
	QString &message, QString &details)
{
	if (Kexi::isIdentifier(v.toString()))
		return KexiValidator::Ok;
	message = i18n("Value of \"%1\" column must be an identifier.\n"
		"\"%2\" is not a valid identifier.").arg(valueName).arg(v.toString());
	return KexiValidator::Error;
}

//--------------------------------------------------------------------------------

KexiDBObjectNameValidator::KexiDBObjectNameValidator(
	KexiDB::Driver *drv, QObject * parent, const char * name)
: KexiValidator(parent,name)
{
	m_drv = drv;
}

KexiDBObjectNameValidator::~KexiDBObjectNameValidator()
{
}

KexiValidator::Result KexiDBObjectNameValidator::internalCheck(
	const QString &valueName, const QVariant& v, 
	QString &message, QString &details)
{

	if (m_drv.isNull() ? !KexiDB::Driver::isKexiDBSystemObjectName(v.toString())
		 : !m_drv->isSystemObjectName(v.toString()))
		return KexiValidator::Ok;
	message = i18n("You cannot use name \"%1\" for your object.\n"
		"It is reserved for internal Kexi objects. Please choose another name.")
		.arg(v.toString());
	details = i18n("Names of internal Kexi objects are starting with \"kexi__\".");
	return KexiValidator::Error;
}

//--------------------------------------------------------------------------------
