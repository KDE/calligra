/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIACTIONPROXY_H
#define KEXIACTIONPROXY_H

#include <qguardedptr.h>
#include <qasciidict.h>
#include <qobject.h>
#include <qpair.h>

class KexiMainWindow;
class QSignal;
class KAction;

class KEXICORE_EXPORT KexiActionProxy
{
	public:
		KexiActionProxy(KexiMainWindow *main, QObject *receiver);
		~KexiActionProxy();

		void activateAction(const char *action_name);

	protected:
		void plugAction(const char *action_name, const char *slot);
		KAction* action(const char* name);

		inline QObject *receiver() const { return m_receiver; }

		bool isAvailable(const char* action_name);
		void setAvailable(const char* action_name, bool set);

		QGuardedPtr<KexiMainWindow> m_main;
		QGuardedPtr<QObject> m_receiver;
		QAsciiDict< QPair<QSignal*,bool> > m_signals;

		QObject *m_signal_parent;
	friend class KexiMainWindow;
};

#endif

