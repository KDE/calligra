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

#include "kexivalidator.h"

KexiValidator::KexiValidator(QObject * parent, const char * name)
: QValidator(parent,name)
, m_acceptsEmptyValue(false)
{
}

KexiValidator::~KexiValidator()
{
}

KexiValidator::Result KexiValidator::check(const QString &valueName, const QVariant& v, 
	QString &message, QString &details)
{
	if (v.isNull() || v.type()==QVariant::String && v.toString().isEmpty()) {
		if (!m_acceptsEmptyValue) {
			message = KexiValidator::msgColumnNotEmpty().arg(valueName);
			return Error;
		}
		return Ok;
	}
	return internalCheck(valueName, v, message, details);
}

KexiValidator::Result KexiValidator::internalCheck(const QString & /*valueName*/, 
	const QVariant& /*v*/, QString & /*message*/, QString & /*details*/)
{
	return Error;
}

QValidator::State KexiValidator::validate ( QString & , int & ) const
{
	return QValidator::Acceptable;
}

//-----------------------------------------------------------

KexiMultiValidator::KexiMultiValidator(QObject* parent, const char * name)
 : KexiValidator(parent, name)
{
	m_ownedSubValidators.setAutoDelete(true);
}

KexiMultiValidator::KexiMultiValidator(KexiValidator *validator, 
	QObject * parent, const char * name)
 : KexiValidator(parent, name)
{
	addSubvalidator(validator);
}


void KexiMultiValidator::addSubvalidator( KexiValidator* validator, bool owned )
{
	if (!validator)
		return;
	m_subValidators.append(validator);
	if (owned && !validator->parent())
		m_ownedSubValidators.append(validator);
}

QValidator::State KexiMultiValidator::validate( QString & input, int & pos ) const
{
	if (m_subValidators.isEmpty())
		return Invalid;
	State s;
	QValueList<KexiValidator*>::const_iterator it;
	for ( it=m_subValidators.constBegin(); it!=m_subValidators.constEnd(); ++it) {
		s = (*it)->validate(input, pos);
		if (s==Intermediate || s==Invalid)
			return s;
	}
	return Acceptable;
}

void KexiMultiValidator::fixup ( QString & input ) const
{
	QValueList<KexiValidator*>::const_iterator it;
	for ( it=m_subValidators.constBegin(); it!=m_subValidators.constEnd(); ++it) {
		(*it)->fixup(input);
	}
}

KexiValidator::Result KexiMultiValidator::internalCheck(
	const QString &valueName, const QVariant& v, 
	QString &message, QString &details)
{
	if (m_subValidators.isEmpty())
		return Error;
	Result r;
	bool warning = false;
	QValueList<KexiValidator*>::const_iterator it;
	for ( it=m_subValidators.begin(); it!=m_subValidators.end(); ++it) {
		r = (*it)->internalCheck(valueName, v, message, details);
		if (r==Error)
			return Error;
		if (r==Warning)
			warning = true;
	}
	return warning ? Warning : Ok;
}

