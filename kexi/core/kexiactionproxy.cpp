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
#include <kshortcut.h>

#include <qwidget.h>
#include <qsignal.h>
#include <qiconset.h>

KexiActionProxy::KexiActionProxy(QObject *receiver, KexiSharedActionHost *host)
 : m_host( host ? host : &KexiSharedActionHost::defaultHost() )
 , m_receiver(receiver)
 , m_signals(47)
 , m_actionProxyParent(0)
 , m_signal_parent( 0, "signal_parent" )
{
	m_signals.setAutoDelete(true);
	m_sharedActionChildren.setAutoDelete(false);
	m_alternativeActions.setAutoDelete(true);
	m_host->plugActionProxy( this );
}

KexiActionProxy::~KexiActionProxy()
{
	QPtrListIterator<KexiActionProxy> it(m_sharedActionChildren);
	//detach myself from every child
	for (;it.current();++it) {
		it.current()->setActionProxyParent_internal( 0 );
	}
	//take me from parent
	if (m_actionProxyParent)
		m_actionProxyParent->takeActionProxyChild( this );

	m_host->takeActionProxyFor(m_receiver);
}

void KexiActionProxy::plugSharedAction(const char *action_name, QObject* receiver, const char *slot)
{
	QPair<QSignal*,bool> *p = new QPair<QSignal*,bool>( new QSignal(&m_signal_parent), true );
	p->first->connect( receiver, slot );
	m_signals.insert(action_name, p);
}

int KexiActionProxy::plugSharedAction(const char *action_name, QWidget* w)
{
	KAction *a = sharedAction(action_name);
	if (!a) {
		kdWarning() << "KexiActionProxy::plugSharedAction(): NO SUCH ACTION: " << action_name << endl;
		return -1;
	}
	return a->plug(w);
}

KAction* KexiActionProxy::plugSharedAction(const char *action_name, const QString& alternativeText, QWidget* w)
{
	KAction *a = sharedAction(action_name);
	if (!a) {
		kdWarning() << "KexiActionProxy::plugSharedAction(): NO SUCH ACTION: " << action_name << endl;
		return 0;
	}
	QCString altName = a->name();
	altName += "_alt";
	KAction *alt_act = new KAction(alternativeText, a->iconSet(), a->shortcut(), 
		0, 0, a->parent(), altName);
	QObject::connect(alt_act, SIGNAL(activated()), a, SLOT(activate()));
	alt_act->plug(w);
	return alt_act;
}

bool KexiActionProxy::activateSharedAction(const char *action_name)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p || !p->second) {
		//try in children...
		QPtrListIterator<KexiActionProxy> it( m_sharedActionChildren );
		for( ; it.current(); ++it ) {
			if (it.current()->activateSharedAction( action_name ))
				return true;
		}
		return false;
	}
	//activate in this proxy...
	p->first->activate();
	return true;
}

KAction* KexiActionProxy::sharedAction(const char* name)
{
	return m_host->mainWindow()->actionCollection()->action(name);
}

bool KexiActionProxy::isSupported(const char* action_name) const
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	return p != 0;
}

bool KexiActionProxy::isAvailable(const char* action_name) const
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p) {
		//not supported explicity - try in children...
		QPtrListIterator<KexiActionProxy> it( m_sharedActionChildren );
		for( ; it.current(); ++it ) {
			if (it.current()->isSupported(action_name))
				return it.current()->isAvailable(action_name);
		}
		return false; //not suported
	}
	//supported explicity:
	return p->second;
}

void KexiActionProxy::setAvailable(const char* action_name, bool set)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p)
		return;
	p->second = set;
	m_host->updateActionAvailable(action_name, set, m_receiver);
}

void KexiActionProxy::addActionProxyChild( KexiActionProxy* child )
{
	if (!child || child==this)
		return;
	child->setActionProxyParent_internal( this );
	m_sharedActionChildren.append( child );
}

void KexiActionProxy::takeActionProxyChild( KexiActionProxy* child )
{
	if (m_sharedActionChildren.findRef( child ) != -1)
		m_sharedActionChildren.take();
}

void KexiActionProxy::setActionProxyParent_internal( KexiActionProxy* parent )
{
	m_actionProxyParent = parent;
}
