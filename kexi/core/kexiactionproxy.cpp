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

#include "kexiactionproxy.h"
#include "keximainwindow.h"

#include <qwidget.h>
#include <qsignal.h>

KexiActionProxy::KexiActionProxy(KexiMainWindow *main, QObject *receiver)
 : m_main(main)
 , m_receiver(receiver)
 , m_signals(47)
 , m_signal_parent( new QObject() )
{
	m_signals.setAutoDelete(true);
	m_main->plugActionProxy( this );
}

KexiActionProxy::~KexiActionProxy()
{
	delete m_signal_parent; //this will delete all signals
}

/*
typedef QPair<QObject*,const char *> ObjectActionPairBase;
class ObjectActionPair : public ObjectActionPairBase
{
	public:
		ObjectActionPair(QObject *o, const char *action_name) : ObjectActionPairBase(o, action_name)
		{}
		ObjectActionPair(const ObjectActionPair &oa) : ObjectActionPairBase(oa.first(), oa.second())
		{}
		ObjectActionPair() : ObjectActionPairBase(0, 0);
		ObjectActionPair& operator=(const ObjectActionPair& oa) {

		}
		{}
}*/

/*
void KexiMainWindow::plugAction(KexiActionProxy *proxy, const char *action_name)
{
	QPair<QObject*,const char *> p(proxy->receiver(), action_name)
	QMap< QPair<QObject*,const char*>, KexiActionProxy*>
	m[p]=receiver;
}*/

void KexiActionProxy::plugAction(const char *action_name, const char *slot)
{
//	m_main->plugActionProxy( this, action_name );
	QPair<QSignal*,bool> *p = new QPair<QSignal*,bool>( new QSignal(m_signal_parent), true );
	p->first->connect( m_receiver, slot );
	m_signals.insert(action_name, p);
}

void KexiActionProxy::activateAction(const char *action_name)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p || !p->second)
		return;
	p->first->activate();
}

KAction* KexiActionProxy::action(const char* name)
{
	return m_main->actionCollection()->action(name);
}

void KexiActionProxy::setAvailable(const char* action_name, bool set)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p)
		return;
	p->second = set;
	m_main->updateActionAvailable(action_name, set, m_receiver);
}

bool KexiActionProxy::isAvailable(const char* action_name)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	return p && p->second;
}

