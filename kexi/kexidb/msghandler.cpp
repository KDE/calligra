/* This file is part of the KDE project
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kexidb/msghandler.h>

using namespace KexiDB;

MessageTitle::MessageTitle(Object* o, const QString& msg)
	: m_obj(o)
	, m_prevMsgTitle(o->m_msgTitle)
{ 
	m_obj->m_msgTitle = msg;
}

MessageTitle::~MessageTitle()
{
	m_obj->m_msgTitle = m_prevMsgTitle;
}

//------------------------------------------------

MessageHandler::MessageHandler(QWidget *parent)
 : m_messageHandlerParentWidget(parent)
 , m_enableMessages(true)
{
}

MessageHandler::~MessageHandler()
{
}

