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

#include <kdebug.h>
#include <kaction.h>
#include <kmainwindow.h>

#include <qwidget.h>
#include <qsignal.h>

KexiActionProxy::KexiActionProxy(QObject *receiver, KexiSharedActionHost *host)
 : m_host( host ? host : &KexiSharedActionHost::defaultHost() )
 , m_receiver(receiver)
 , m_signals(47)
 , m_signal_parent( 0, "signal_parent" )
{
	m_signals.setAutoDelete(true);
	m_host->plugActionProxy( this );
}

KexiActionProxy::~KexiActionProxy()
{
//	delete m_signal_parent; //this will delete all signals
}

void KexiActionProxy::plugSharedAction(const char *action_name, QObject* receiver, const char *slot)
{
	QPair<QSignal*,bool> *p = new QPair<QSignal*,bool>( new QSignal(&m_signal_parent), true );
	p->first->connect( receiver, slot );
	m_signals.insert(action_name, p);
}

void KexiActionProxy::plugSharedAction(const char *action_name, QWidget* w)
{
	KAction *a = sharedAction(action_name);
	if (!a) {
		kdWarning() << "KexiActionProxy::plugAction(): NO SUCH ACTION: " << action_name << endl;
		return;
	}
	a->plug(w);
}

void KexiActionProxy::activateSharedAction(const char *action_name)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p || !p->second)
		return;
	p->first->activate();
}

KAction* KexiActionProxy::sharedAction(const char* name)
{
	return m_host->mainWindow()->actionCollection()->action(name);
}

void KexiActionProxy::setAvailable(const char* action_name, bool set)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p)
		return;
	p->second = set;
	m_host->updateActionAvailable(action_name, set, m_receiver);
}

bool KexiActionProxy::isAvailable(const char* action_name)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	return p && p->second;
}

