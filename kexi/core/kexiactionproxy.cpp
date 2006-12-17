/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiactionproxy.h"
#include "kexiactionproxy_p.h"

#include <kdebug.h>
#include <kaction.h>
#include <kmainwindow.h>
#include <kshortcut.h>

#include <qwidget.h>
#include <qsignal.h>
#include <qiconset.h>

KAction_setEnabled_Helper::KAction_setEnabled_Helper(KexiActionProxy* proxy)
 : QObject(0,"KAction_setEnabled_Helper")
 , m_proxy( proxy )
{
}

void KAction_setEnabled_Helper::slotSetEnabled(bool enabled)
{
	if (sender()->inherits("KAction")) {
		const KAction *a = static_cast<const KAction*>(sender());
		m_proxy->setAvailable(a->name(), enabled);
	}
}

//=======================

KexiSharedActionConnector::KexiSharedActionConnector( KexiActionProxy* proxy, QObject *obj ) 
 : m_proxy(proxy)
 , m_object(obj)
{
}

KexiSharedActionConnector::~KexiSharedActionConnector()
{
}

void KexiSharedActionConnector::plugSharedAction(const char *action_name, const char *slot)
{
	m_proxy->plugSharedAction(action_name, m_object, slot);
}

void KexiSharedActionConnector::plugSharedActionToExternalGUI(
	const char *action_name, KXMLGUIClient *client) 
{
	m_proxy->plugSharedActionToExternalGUI(action_name, client);
}

void KexiSharedActionConnector::plugSharedActionsToExternalGUI(
	const QValueList<QCString>& action_names, KXMLGUIClient *client)
{
	m_proxy->plugSharedActionsToExternalGUI(action_names, client);
}


//=======================

KexiActionProxy::KexiActionProxy(QObject *receiver, KexiSharedActionHost *host)
 : m_host( host ? host : &KexiSharedActionHost::defaultHost() )
 , m_receiver(receiver)
 , m_signals(47)
 , m_actionProxyParent(0)
 , m_signal_parent( 0, "signal_parent" )
 , m_KAction_setEnabled_helper( new KAction_setEnabled_Helper(this) )
 , m_focusedChild(0)
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

	delete m_KAction_setEnabled_helper;
}

void KexiActionProxy::plugSharedAction(const char *action_name, QObject* receiver, const char *slot)
{
	if (!action_name)// || !receiver || !slot)
		return;
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p) {
		p = new QPair<QSignal*,bool>( new QSignal(&m_signal_parent), true );
		m_signals.insert(action_name, p);
	}
	if (receiver && slot)
		p->first->connect( receiver, slot );
}

void KexiActionProxy::unplugSharedAction(const char *action_name)
{
	QPair<QSignal*,bool> *p = m_signals.take(action_name);
	if (!p)
		return;
	delete p->first;
	delete p;
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

void KexiActionProxy::unplugSharedAction(const char *action_name, QWidget* w)
{
	KAction *a = sharedAction(action_name);
	if (!a) {
		kdWarning() << "KexiActionProxy::unplugSharedAction(): NO SUCH ACTION: " << action_name << endl;
		return;
	}
	a->unplug(w);
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

//OK?
	m_host->updateActionAvailable(action_name, true, m_receiver);

	return alt_act;
}

void KexiActionProxy::plugSharedActionToExternalGUI(const char *action_name, KXMLGUIClient *client)
{
	KAction *a = client->action(action_name);
	if (!a)
		return;
	plugSharedAction(a->name(), a, SLOT(activate()));

	//update availability
	setAvailable(a->name(), a->isEnabled());
	//changes will be signaled
	QObject::connect(a, SIGNAL(enabled(bool)), m_KAction_setEnabled_helper, SLOT(slotSetEnabled(bool)));
}

void KexiActionProxy::plugSharedActionsToExternalGUI(
	const QValueList<QCString>& action_names, KXMLGUIClient *client)
{
	for (QValueList<QCString>::const_iterator it = action_names.constBegin(); it!=action_names.constEnd(); ++it) {
		plugSharedActionToExternalGUI(*it, client);
	}
}

bool KexiActionProxy::activateSharedAction(const char *action_name, bool alsoCheckInChildren)
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p || !p->second) {
		//try in children...
		if (alsoCheckInChildren) {
			QPtrListIterator<KexiActionProxy> it( m_sharedActionChildren );
			for( ; it.current(); ++it ) {
				if (it.current()->activateSharedAction( action_name, alsoCheckInChildren ))
					return true;
			}
		}
		return m_actionProxyParent ? m_actionProxyParent->activateSharedAction(action_name, false) : false; //last chance: parent
	}
	//activate in this proxy...
	p->first->activate();
	return true;
}

KAction* KexiActionProxy::sharedAction(const char* action_name)
{
	return m_host->mainWindow()->actionCollection()->action(action_name);
}

bool KexiActionProxy::isSupported(const char* action_name) const
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p) {
		//not supported explicitly - try in children...
		if (m_focusedChild)
			return m_focusedChild->isSupported(action_name);
		QPtrListIterator<KexiActionProxy> it( m_sharedActionChildren );
		for( ; it.current(); ++it ) {
			if (it.current()->isSupported(action_name))
				return true;
		}
		return false; //not suported
	}
	return p != 0;
}

bool KexiActionProxy::isAvailable(const char* action_name, bool alsoCheckInChildren) const
{
	QPair<QSignal*,bool> *p = m_signals[action_name];
	if (!p) {
		//not supported explicitly - try in children...
		if (alsoCheckInChildren) {
			if (m_focusedChild)
				return m_focusedChild->isAvailable(action_name, alsoCheckInChildren);
			QPtrListIterator<KexiActionProxy> it( m_sharedActionChildren );
			for( ; it.current(); ++it ) {
				if (it.current()->isSupported(action_name))
					return it.current()->isAvailable(action_name, alsoCheckInChildren);
			}
		}
		return m_actionProxyParent ? m_actionProxyParent->isAvailable(action_name, false) : false; //last chance: parent
	}
	//supported explicitly:
	return p->second != 0;
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

#include "kexiactionproxy_p.moc"

