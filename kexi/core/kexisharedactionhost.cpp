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

#include "kexisharedactionhost.h"
#include "kexisharedactionhost_p.h"
#include "kexiactionproxy.h"

#include <kiconloader.h>
#include <kdebug.h>

//! internal class
KexiSharedActionHostPrivate::KexiSharedActionHostPrivate(KexiSharedActionHost *h)
: QObject(0,"KexiSharedActionHostPrivate")
, actionProxies(401)
, actionMapper( this )
, host(h)
{
	connect(&actionMapper, SIGNAL(mapped(const QString &)), this, SLOT(slotAction(const QString &)));
}

void KexiSharedActionHostPrivate::slotAction(const QString& act_id)
{
	QWidget *w = host->focusWindow(); //focusWidget();
//	while (w && !w->inherits("KexiDialogBase") && !w->inherits("KexiDockBase"))
//		w = w->parentWidget();
	if (!w)
		return;
	KexiActionProxy * proxy = actionProxies[ w ];
	if (!proxy)
		return;
	proxy->activateSharedAction(act_id.latin1());
}

//--------------------------------------------------

//! dummy host to avoid crashes
KexiSharedActionHost KexiSharedActionHost_dummy = KexiSharedActionHost(0);

//! default host
KexiSharedActionHost* KexiSharedActionHost_defaultHost = &KexiSharedActionHost_dummy;

KexiSharedActionHost& KexiSharedActionHost::defaultHost()
{
	return *KexiSharedActionHost_defaultHost;
}

void KexiSharedActionHost::setAsDefaultHost()
{
	KexiSharedActionHost_defaultHost = this;
}

//--------------------------------------------------

KexiSharedActionHost::KexiSharedActionHost(KMainWindow* mainWin)
: d( new KexiSharedActionHostPrivate(this) )
{
	d->mainWin = mainWin;
}

KexiSharedActionHost::~KexiSharedActionHost()
{
	if (KexiSharedActionHost_defaultHost == this) {
		//default host is destroyed! - restore dummy
		KexiSharedActionHost_defaultHost = &KexiSharedActionHost_dummy;
	}
	delete d;
	d=0; //! to let takeActionProxyFor() know that we are almost dead :)
}

void KexiSharedActionHost::setActionAvailable(const char *action_name, bool avail)
{
	KAction *act = d->mainWin->actionCollection()->action(action_name);
	if (!act)
		return;
	act->setEnabled(avail);
}

void KexiSharedActionHost::updateActionAvailable(const char *action_name, bool avail, QObject *obj)
{
	QWidget *fw = d->mainWin->focusWidget();
	while (fw && obj!=fw)
		fw = fw->parentWidget();
	if (!fw)
		return;

	setActionAvailable(action_name, avail);
}

void KexiSharedActionHost::plugActionProxy(KexiActionProxy *proxy)
{
	d->actionProxies.insert( proxy->receiver(), proxy );
}

KMainWindow* KexiSharedActionHost::mainWindow() const
{
	return d->mainWin;
}

void KexiSharedActionHost::invalidateSharedActions(QObject *o)
{
	KexiActionProxy *p = o ? d->actionProxies[ o ] : 0;
	for (KActionPtrList::Iterator it=d->sharedActions.begin(); it!=d->sharedActions.end(); ++it) {
//			setActionAvailable((*it)->name(),p && p->isAvailable((*it)->name()));
		(*it)->setEnabled(p && p->isAvailable((*it)->name()));
		kdDebug() << "Action " << (*it)->name() << (p && p->isAvailable((*it)->name()) ? " enabled." : " disabled.") << endl;
	}
}

KexiActionProxy* KexiSharedActionHost::actionProxyFor(QObject *o) const
{
	return d->actionProxies[ o ];
}

KexiActionProxy* KexiSharedActionHost::takeActionProxyFor(QObject *o)
{
	if (d)
		return d->actionProxies.take( o );
	return 0;
}

bool KexiSharedActionHost::acceptsSharedActions(QObject *)
{
	return false;
}

QWidget* KexiSharedActionHost::focusWindow()
{
	QWidget* fw = d->mainWin->focusWidget();
	while (fw && !acceptsSharedActions(fw))
		fw = fw->parentWidget();
	return fw;
}

KAction* KexiSharedActionHost::createSharedActionInternal( KAction *action )
{
	QObject::connect(action,SIGNAL(activated()), &d->actionMapper, SLOT(map()));
	d->actionMapper.setMapping(action, action->name());
	d->sharedActions.append( action );
	return action;
}

KAction* KexiSharedActionHost::createSharedAction(const QString &text, const QString &pix_name, 
	const KShortcut &cut, const char *name)
{
	return createSharedActionInternal( 
		new KAction(text, (pix_name.isEmpty() ? QIconSet() : SmallIconSet(pix_name)),
		cut, 0/*receiver*/, 0/*slot*/, d->mainWin->actionCollection(), name)
	);
}

KAction* KexiSharedActionHost::createSharedAction( KStdAction::StdAction id, const char *name)
{
	return createSharedActionInternal( 
		KStdAction::create( id, name, 0/*receiver*/, 0/*slot*/, d->mainWin->actionCollection() )
	);
}


#include "kexisharedactionhost_p.moc"

