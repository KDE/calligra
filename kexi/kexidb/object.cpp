/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/object.h>
#include <kexidb/error.h>
#include <kexidb/msghandler.h>

#include <klocale.h>
#include <kdebug.h>

using namespace KexiDB;

#define ERRMSG(a) \
	{ if (m_msgHandler) m_msgHandler->showErrorMessage(a); }

Object::Object(MessageHandler* handler)
: m_previousServerResultNum(0)
, m_previousServerResultNum2(0)
, m_msgHandler(handler)
, d(0) //empty
{
	clearError();
}

#define STORE_PREV_ERR \
	m_previousServerResultNum = m_previousServerResultNum2; \
	m_previousServerResultName = m_previousServerResultName2; \
	m_previousServerResultNum2 = serverResult(); \
	m_previousServerResultName2 = serverResultName(); \
	KexiDBDbg << "Object ERROR: " << m_previousServerResultNum2 << ": " \
		<< m_previousServerResultName2 <<endl

void Object::setError( int code, const QString &msg )
{
	STORE_PREV_ERR;

	m_errno=code;
	if (m_errno==ERR_OTHER && msg.isEmpty())
		m_errMsg = i18n("Unspecified error encountered");
	else
		m_errMsg = msg;
	m_hasError = code!=ERR_NONE;

	if (m_hasError)
		ERRMSG(this);
}

void Object::setError( const QString &msg )
{
	STORE_PREV_ERR;

	m_errno=ERR_OTHER;
	m_errMsg = msg;
	m_hasError = true;
	if (m_hasError)
		ERRMSG(this);
}

void Object::setError( const QString& title, const QString &msg )
{
	STORE_PREV_ERR;

	m_errno=ERR_OTHER;
	QString origMsgTitle( m_msgTitle ); //store
	
	m_msgTitle += title;
	m_errMsg = msg;
	m_hasError = true;
	if (m_hasError)
		ERRMSG(this);
		
	m_msgTitle = origMsgTitle; //revert 
}

void Object::setError( KexiDB::Object *obj )
{
	STORE_PREV_ERR;

	if (obj) {
		m_errno = obj->errorNum();
		m_errMsg = obj->errorMsg();
		m_hasError = obj->error();
	}
	if (m_hasError)
		ERRMSG(this);
}

void Object::clearError()
{ 
	m_errno = 0;
	m_hasError = false;
	m_errMsg = QString::null;
	drv_clearServerResult();
}

QString Object::serverErrorMsg()
{
	return QString::null;
}

int Object::serverResult()
{
	return 0;
}

QString Object::serverResultName()
{
	return QString::null;
}

Object::~Object()
{
}

void Object::debugError()
{
	if (error()) {
		KexiDBDbg << "KEXIDB ERROR: " << errorMsg() << endl;
		QString s = serverErrorMsg(), sn = serverResultName();
		if (!s.isEmpty())
			KexiDBDbg << "KEXIDB SERVER ERRMSG: " << s << endl;
		if (!sn.isEmpty())
			KexiDBDbg << "KEXIDB SERVER RESULT NAME: " << sn << endl;
		if (serverResult()!=0)
			KexiDBDbg << "KEXIDB SERVER RESULT #: " << serverResult() << endl;
	} else
		KexiDBDbg << "KEXIDB OK." << endl;
}

