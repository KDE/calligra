/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIMSGHANDLER_H
#define KEXIMSGHANDLER_H

#include <kexidb/object.h>
#include <qguardedptr.h>
#include <qwidget.h>

/*! A prototype for Message Handler usable 
 for reacting on messages sent by KexiProject object(s) and (probably) other objects.
*/
class KEXICORE_EXPORT KexiMessageHandler
{
	public:
		enum MessageType { Error, Sorry };

		/*! Constructs mesage handler, \a parent is a widget that will be a parent 
		 for displaying gui elements (e.g. message boxes). Can be 0 for non-gui usage. */
		KexiMessageHandler(QWidget *parent = 0);
		virtual ~KexiMessageHandler();

		/*! This method can be used to block/unblock messages.
		 Sometimes you are receiving both lower- and higher-level messages,
		 but you do not need to display two message boxes but only one (higher level with details).
		 All you need is to call enableMessages(false) before action that can fail
		 and restore messages by enableMessages(true) after the action. 
		 See KexiMainWindowImpl::renameObject() implementation for example. */
		inline void enableMessages(bool enable) { m_enableMessages = enable; }

		virtual void showErrorMessage(const QString &title, const QString &details = QString::null) = 0;
		virtual void showErrorMessage(const QString& msg, KexiDB::Object *obj) = 0;

	protected:
		QGuardedPtr<QWidget> m_messageHandlerParentWidget;
		bool m_enableMessages : 1;
};

#endif
