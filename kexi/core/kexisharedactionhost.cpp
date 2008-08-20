/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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
#include "KexiWindow.h"
#include "KexiMainWindowIface.h"

#include <kexiutils/utils.h>
#include <kexi_global.h>

#include <QApplication>
#include <kiconloader.h>
#include <kguiitem.h>
#include <kdebug.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kactioncollection.h>
#include <kicon.h>

KexiSharedActionHostPrivate::KexiSharedActionHostPrivate(KexiSharedActionHost *h)
        : QObject()
        , actionMapper(this)
        , host(h)
{
    setObjectName("KexiSharedActionHostPrivate");
    connect(&actionMapper, SIGNAL(mapped(const QString &)), this, SLOT(slotAction(const QString &)));
}

KexiSharedActionHostPrivate::~KexiSharedActionHostPrivate()
{
    qDeleteAll(volatileActions);
    volatileActions.clear();
}

void KexiSharedActionHostPrivate::slotAction(const QString& act_id)
{
    QWidget *w = host->focusWindow(); //focusWidget();
// while (w && !w->inherits("KexiWindow") && !w->inherits("KexiDockBase"))
//  w = w->parentWidget();

    KexiActionProxy *proxy = w ? actionProxies.value(w) : 0;

    if (!proxy || !proxy->activateSharedAction(act_id.toLatin1())) {
        //also try to find previous enabler
        w = enablers.contains(act_id) ? enablers.value(act_id) : 0;
        if (!w)
            return;
        proxy = actionProxies.value(w);
        if (!proxy)
            return;
        proxy->activateSharedAction(act_id.toLatin1());
    }
}

//--------------------------------------------------

//! dummy host to avoid crashes
K_GLOBAL_STATIC_WITH_ARGS(KexiSharedActionHost, KexiSharedActionHost_dummy, (0))

//! default host
KexiSharedActionHost* KexiSharedActionHost_defaultHost = 0;//KexiSharedActionHost_dummy;

KexiSharedActionHost* KexiSharedActionHost::defaultHost()
{
    if (!KexiSharedActionHost_defaultHost)
        return KexiSharedActionHost_dummy;
//  KexiSharedActionHost_defaultHost = KexiSharedActionHost_dummy;
    return KexiSharedActionHost_defaultHost;
}

void KexiSharedActionHost::setAsDefaultHost()
{
    KexiSharedActionHost_defaultHost = this;
}

//--------------------------------------------------

KexiSharedActionHost::KexiSharedActionHost(KexiMainWindowIface* mainWin)
        : d(new KexiSharedActionHostPrivate(this))
{
    d->mainWin = mainWin;
}

KexiSharedActionHost::~KexiSharedActionHost()
{
    if (KexiSharedActionHost_defaultHost == this) {
        //default host is destroyed! - restore dummy
        KexiSharedActionHost_defaultHost = 0;// KexiSharedActionHost_dummy;
    }
    delete d;
    d = 0; //! to let takeActionProxyFor() know that we are almost dead :)
}

void KexiSharedActionHost::setActionAvailable(const QString& action_name, bool avail)
{
    QAction *act = d->mainWin->actionCollection()->action(action_name);
    if (act) {
        act->setEnabled(avail);
    }
}

void KexiSharedActionHost::updateActionAvailable(const QString& action_name, bool avail, QObject *obj)
{
    /*test if (qstrcmp(action_name, "tablepart_toggle_pkey")==0) {
        kDebug() << "tablepart_toggle_pkey" << endl;
      }*/
    if (!d)
        return; //sanity
    QWidget *fw = d->mainWin->focusWidget();
    while (fw && obj != fw)
        fw = fw->parentWidget();
    if (!fw)
        return;

    setActionAvailable(action_name, avail);
    if (avail) {
        d->enablers.insert(action_name, fw);
    } else {
        d->enablers.take(action_name);
    }
}

void KexiSharedActionHost::plugActionProxy(KexiActionProxy *proxy)
{
// kDebug() << "KexiSharedActionHost::plugActionProxy():" << proxy->receiver()->name() << endl;
    d->actionProxies.insert(proxy->receiver(), proxy);
}

KexiMainWindowIface* KexiSharedActionHost::mainWindow() const
{
    return d->mainWin;
}

void KexiSharedActionHost::invalidateSharedActions(QObject *o)
{
    if (!d)
        return;
    //KDE3: bool insideWindow = o && (o->inherits("KexiWindow") || 0 != KexiUtils::findParent<KexiWindow>(o, "KexiWindow"));
    bool insideKexiWindow = o
                            && (o->inherits("KexiWindow") || 0 != KexiUtils::findParent<KexiWindow*>(o));

    KexiActionProxy *p = o ? d->actionProxies.value(o) : 0;
    foreach(KAction* a, d->sharedActions) {
        //setActionAvailable((*it)->name(),p && p->isAvailable((*it)->name()));
#ifdef __GNUC__
#warning TODO:  if (!insideKexiWindow && d->mainWin->actionCollection()!=a->parentCollection()) {
#else
#pragma WARNING( TODO:  if (!insideKexiWindow && d->mainWin->actionCollection()!=a->parentCollection()) { )
#endif
        //o is not KexiKexiWindow or its child:
        // only invalidate action if it comes from mainwindow's KActionCollection
        // (thus part-actions are untouched when the focus is e.g. in the Property Editor)
#ifdef __GNUC__
#warning TODO   continue;
#else
#pragma WARNING( TODO   continue; )
#endif
//todo  }
        const bool avail = p && p->isAvailable(a->objectName());
        KexiVolatileActionData *va = d->volatileActions.value(a);
        if (va != 0) {
            if (p && p->isSupported(a->objectName())) {
                QList<KAction*> actions_list;
                actions_list.append(a);
                if (!va->plugged) {
                    va->plugged = true;
                    //d->mainWin->unplugActionList( a->objectName() );
                    d->mainWin->plugActionList(a->objectName(), actions_list);
                }
            } else {
                if (va->plugged) {
                    va->plugged = false;
                    d->mainWin->unplugActionList(a->objectName());
                }
            }
        }
//  a->setEnabled(p && p->isAvailable(a->name()));
        a->setEnabled(avail);
//  kDebug() << "Action " << a->name() << (avail ? " enabled." : " disabled.") << endl;
    }
}

KexiActionProxy* KexiSharedActionHost::actionProxyFor(QObject *o) const
{
    return d->actionProxies.value(o);
}

KexiActionProxy* KexiSharedActionHost::takeActionProxyFor(QObject *o)
{
    if (d)
        return d->actionProxies.take(o);
    return 0;
}

bool KexiSharedActionHost::acceptsSharedActions(QObject *)
{
    return false;
}

QWidget* KexiSharedActionHost::focusWindow()
{
#if 0 //sebsauer 20061120: KDE3
    if (dynamic_cast<KMdiMainFrm*>(d->mainWin)) {
        fw = dynamic_cast<KMdiMainFrm*>(d->mainWin)->activeWindow();
    } else {
        QWidget *aw = qApp->activeWindow();
        if (!aw)
            aw = d->mainWin;
        fw = aw->focusWidget();
    }
    while (fw && !acceptsSharedActions(fw))
        fw = fw->parentWidget();
    return fw;
#else
    QWidget *aw = QApplication::activeWindow();
    if (!aw)
        aw = dynamic_cast<QWidget*>(d->mainWin);
    QWidget *fw = aw->focusWidget();
    while (fw && !acceptsSharedActions(fw))
        fw = fw->parentWidget();
    return fw;
#endif
}

KAction* KexiSharedActionHost::createSharedActionInternal(KAction *action)
{
    QObject::connect(action, SIGNAL(activated()), &d->actionMapper, SLOT(map()));
    d->actionMapper.setMapping(action, action->objectName());
    d->sharedActions.append(action);
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
    if (!col)
        col = d->mainWin->actionCollection();

    if (subclassName == 0) {
        KAction* action = new KAction(KIcon(pix_name), text, col);
        action->setObjectName(name);
        action->setShortcut(cut);
        col->addAction(name, action);
        return createSharedActionInternal(action);
    } else if (qstricmp(subclassName, "KToggleAction") == 0) {
        KToggleAction* action = new KToggleAction(KIcon(pix_name), text, col);
        action->setObjectName(name);
        action->setShortcut(cut);
        col->addAction(name, action);
        return createSharedActionInternal(action);
    } else if (qstricmp(subclassName, "KActionMenu") == 0) {
        KActionMenu* action = new KActionMenu(KIcon(pix_name), text, col);
        action->setObjectName(name);
        action->setShortcut(cut);
        col->addAction(name, action);
        return createSharedActionInternal(action);
    }
    //TODO: more KAction subclasses
    return 0;
}

KAction* KexiSharedActionHost::createSharedAction(KStandardAction::StandardAction id,
        const char *name, KActionCollection* col)
{
    if (!col)
        col = d->mainWin->actionCollection();

    KAction* action = createSharedActionInternal(
                          KStandardAction::create(id, 0/*receiver*/, 0/*slot*/, col)
                      );
    action->setObjectName(name);
    return action;
}

KAction* KexiSharedActionHost::createSharedAction(const KGuiItem& guiItem, const KShortcut &cut,
        const char *name, KActionCollection* col)
{
    if (!col)
        col = d->mainWin->actionCollection();
    KAction* action = new KAction(guiItem.icon(), guiItem.text(), col);
    action->setObjectName(name);
    action->setShortcut(cut);
    action->setEnabled(guiItem.isEnabled());   //TODO how to update enable/disable? is it needed anyway?
    action->setToolTip(guiItem.toolTip());
    action->setWhatsThis(guiItem.whatsThis());
    return createSharedActionInternal(action);
}

void KexiSharedActionHost::setActionVolatile(KAction *a, bool set)
{
    if (!set) {
        d->volatileActions.remove(a);
        delete a;
        return;
    }
    if (d->volatileActions.value(a))
        return;
    d->volatileActions.insert(a, new KexiVolatileActionData());
}

#include "kexisharedactionhost_p.moc"
