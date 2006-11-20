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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexisharedactionhost.h"
#include "kexisharedactionhost_p.h"
#include "kexiactionproxy.h"
#include "kexidialogbase.h"

#include <kexiutils/utils.h>

#include <kiconloader.h>
#include <kdebug.h>
//Added by qt3to4:
#include <Q3PtrList>

KexiSharedActionHostPrivate::KexiSharedActionHostPrivate(KexiSharedActionHost *h)
: QObject(0,"KexiSharedActionHostPrivate")
, actionProxies(401)
, actionMapper( this )
, volatileActions(401)
, enablers(401, false)
, host(h)
{
	volatileActions.setAutoDelete(true);
	connect(&actionMapper, SIGNAL(mapped(const QString &)), this, SLOT(slotAction(const QString &)));
}

void KexiSharedActionHostPrivate::slotAction(const QString& act_id)
{
	QWidget *w = host->focusWindow(); //focusWidget();
//	while (w && !w->inherits("KexiDialogBase") && !w->inherits("KexiDockBase"))
//		w = w->parentWidget();

	KexiActionProxy *proxy = w ? actionProxies[ w ] : 0;

	if (!proxy || !proxy->activateSharedAction(act_id.latin1())) {
		//also try to find previous enabler
		w = enablers[act_id.latin1()];
		if (!w)
			return;
		proxy = actionProxies[ w ];
		if (!proxy)
			return;
		proxy->activateSharedAction(act_id.latin1());
	}
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

KexiSharedActionHost::KexiSharedActionHost(KexiMainWindow* mainWin)
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

void KexiSharedActionHost::setActionAvailable(const QString& action_name, bool avail)
{
	KAction *act = d->mainWin->actionCollection()->action(action_name);
	if (act) {
		act->setEnabled(avail);
	}
}

void KexiSharedActionHost::updateActionAvailable(const QString& action_name, bool avail, QObject *obj)
{
/*test	if (qstrcmp(action_name, "tablepart_toggle_pkey")==0) {
		kDebug() << "tablepart_toggle_pkey" << endl;
	}*/
	if (!d)
		return; //sanity
	QWidget *fw = d->mainWin->focusWidget();
	while (fw && obj!=fw)
		fw = fw->parentWidget();
	if (!fw)
		return;

	setActionAvailable(action_name, avail);
	if (avail) {
		d->enablers.replace(action_name, fw);
	}
	else {
		d->enablers.take(action_name);
	}
}

void KexiSharedActionHost::plugActionProxy(KexiActionProxy *proxy)
{
//	kDebug() << "KexiSharedActionHost::plugActionProxy():" << proxy->receiver()->name() << endl;
	d->actionProxies.insert( proxy->receiver(), proxy );
}

KexiMainWindow* KexiSharedActionHost::mainWindow() const
{
	return d->mainWin;
}

void KexiSharedActionHost::invalidateSharedActions(QObject *o)
{
	if (!d)
		return;
	bool insideDialogBase = o && (o->inherits("KexiDialogBase") || 0!=KexiUtils::findParent<KexiDialogBase>(o, "KexiDialogBase"));

	KexiActionProxy *p = o ? d->actionProxies[ o ] : 0;
	for (KActionPtrList::ConstIterator it=d->sharedActions.constBegin(); it!=d->sharedActions.constEnd(); ++it) {
//			setActionAvailable((*it)->name(),p && p->isAvailable((*it)->name()));
		KAction *a = *it;
		if (!insideDialogBase && d->mainWin->actionCollection()!=a->parentCollection()) {
			//o is not KexiDialogBase or its child:
			// only invalidate action if it comes from mainwindow's KActionCollection
			// (thus part-actions are untouched when the focus is e.g. in the Property Editor)
			continue;
		}
		const bool avail = p && p->isAvailable(a->name());
		KexiVolatileActionData *va = d->volatileActions[ a ];
		if (va != 0) {
			if (p && p->isSupported(a->name())) {
				Q3PtrList<KAction> actions_list;
				actions_list.append( a );
				if (!va->plugged) {
					va->plugged=true;
	//				d->mainWin->unplugActionList( a->name() );
					d->mainWin->plugActionList( a->name(), actions_list );
				}
			}
			else {
				if (va->plugged) {
					va->plugged=false;
					d->mainWin->unplugActionList( a->name() );
				}
			}
		}
//		a->setEnabled(p && p->isAvailable(a->name()));
		a->setEnabled(avail);
//		kDebug() << "Action " << a->name() << (avail ? " enabled." : " disabled.") << endl;
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
	QWidget *fw;
	if (dynamic_cast<KMdiMainFrm*>(d->mainWin)) {
		fw = dynamic_cast<KMdiMainFrm*>(d->mainWin)->activeWindow();
	}
	else {
		QWidget *aw = qApp->activeWindow();
		if (!aw)
			aw = d->mainWin;
		fw = aw->focusWidget();
	}
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

QList<KAction*> KexiSharedActionHost::sharedActions() const
{
	return d->sharedActions;
}

/*class KexiAction : public KAction
{
	public:
		KexiAction(const QString &text, const QIcon &pix,
			const KShortcut &cut, const QObject *receiver,
			const char *slot, KActionCollection *parent, const char *name)
		 : KAction(text,pix,cut,receiver,slot,parent,name)
		{
		}

	QPtrDict<QWidget> unplugged;
};*/

KAction* KexiSharedActionHost::createSharedAction(const QString &text, const QString &pix_name,
	const KShortcut &cut, const char *name, KActionCollection* col, const char *subclassName)
{
	if (subclassName==0)
		return createSharedActionInternal(
			new KAction(text, pix_name,
			cut, 0/*receiver*/, 0/*slot*/, col ? col : d->mainWin->actionCollection(), name)
		);
	else if (qstricmp(subclassName,"KToggleAction")==0)
		return createSharedActionInternal(
			new KToggleAction(text, pix_name,
			cut, 0/*receiver*/, 0/*slot*/, col ? col : d->mainWin->actionCollection(), name)
		);
	else if (qstricmp(subclassName,"KActionMenu")==0)
		return createSharedActionInternal(
			new KActionMenu(text, pix_name, col ? col : d->mainWin->actionCollection(), name)
		);
//TODO: more KAction subclasses

	return 0;
}

KAction* KexiSharedActionHost::createSharedAction( KStdAction::StdAction id, const char *name,
	KActionCollection* col)
{
	return createSharedActionInternal(
		KStdAction::create( id, name, 0/*receiver*/, 0/*slot*/, col ? col : d->mainWin->actionCollection() )
	);
}

void KexiSharedActionHost::setActionVolatile( KAction *a, bool set )
{
	if (!set) {
		d->volatileActions.remove( a );
		return;
	}
	if (d->volatileActions[ a ])
		return;
	d->volatileActions.insert( a, new KexiVolatileActionData() );
}

#include "kexisharedactionhost_p.moc"

