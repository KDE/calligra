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

#include <KGuiItem>
#include <KToggleAction>
#include <KActionMenu>
#include <KActionCollection>

#include <QApplication>
#include <QIcon>
#include <QDebug>

KexiSharedActionHostPrivate::KexiSharedActionHostPrivate(KexiSharedActionHost *h)
        : QObject()
        , actionMapper(this)
        , host(h)
{
    setObjectName("KexiSharedActionHostPrivate");
    connect(&actionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAction(QString)));
}

KexiSharedActionHostPrivate::~KexiSharedActionHostPrivate()
{
    qDeleteAll(volatileActions);
    volatileActions.clear();
}

void KexiSharedActionHostPrivate::slotAction(const QString& act_id)
{
    QWidget *w = host->focusWindow();
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
Q_GLOBAL_STATIC_WITH_ARGS(KexiSharedActionHost, KexiSharedActionHost_dummy, (0))

//! default host
KexiSharedActionHost* KexiSharedActionHost_defaultHost = 0;

KexiSharedActionHost* KexiSharedActionHost::defaultHost()
{
    if (!KexiSharedActionHost_defaultHost)
        return KexiSharedActionHost_dummy;
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
        KexiSharedActionHost_defaultHost = 0;
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

    KexiActionProxy *p = o ? d->actionProxies.value(o) : 0;
    foreach(QAction * a, d->sharedActions) {
        const bool avail = p && p->isAvailable(a->objectName());
        KexiVolatileActionData *va = d->volatileActions.value(a);
        if (va != 0) {
            if (p && p->isSupported(a->objectName())) {
                QList<QAction *> actions_list;
                actions_list.append(a);
                if (!va->plugged) {
                    va->plugged = true;
                }
            } else {
                if (va->plugged) {
                    va->plugged = false;
                }
            }
        }
        a->setEnabled(avail);
  //qDebug() << "Action " << a->name() << (avail ? " enabled." : " disabled.");
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

QWidget* KexiSharedActionHost::findWindow(QWidget * /*w*/)
{
    return 0;
}

QWidget* KexiSharedActionHost::focusWindow()
{
    QWidget *aw = QApplication::activeWindow();
    if (!aw)
        aw = dynamic_cast<QWidget*>(d->mainWin);
    QWidget *fw = aw->focusWidget();
    return findWindow(fw);
}

QAction * KexiSharedActionHost::createSharedActionInternal(QAction *action)
{
    QObject::connect(action, SIGNAL(triggered()), &d->actionMapper, SLOT(map()));
    d->actionMapper.setMapping(action, action->objectName());
    d->sharedActions.append(action);
    return action;
}

QList<QAction *> KexiSharedActionHost::sharedActions() const
{
    return d->sharedActions;
}

QAction * KexiSharedActionHost::createSharedAction(const QString &text, const QString &iconName,
        const QKeySequence &cut, const char *name, KActionCollection* col, const char *subclassName)
{
    if (!col)
        col = d->mainWin->actionCollection();

    if (subclassName == 0) {
        QAction *action = new QAction(QIcon::fromTheme(iconName), text, col);
        action->setObjectName(name);
        action->setShortcut(cut);
        col->addAction(name, action);
        return createSharedActionInternal(action);
    } else if (qstricmp(subclassName, "KToggleAction") == 0) {
        KToggleAction *action = new KToggleAction(QIcon::fromTheme(iconName), text, col);
        action->setObjectName(name);
        action->setShortcut(cut);
        col->addAction(name, action);
        return createSharedActionInternal(action);
    } else if (qstricmp(subclassName, "KActionMenu") == 0) {
        KActionMenu *action = new KActionMenu(QIcon::fromTheme(iconName), text, col);
        action->setObjectName(name);
        action->setShortcut(cut);
        col->addAction(name, action);
        return createSharedActionInternal(action);
    }
    //! @todo more QAction subclasses
    return 0;
}

QAction * KexiSharedActionHost::createSharedAction(KStandardAction::StandardAction id,
        const char *name, KActionCollection* col)
{
    if (!col)
        col = d->mainWin->actionCollection();

    QAction * action = KStandardAction::create(id, 0/*receiver*/, 0/*slot*/, col);
    if (name) {
        action->setObjectName(name);
    }
    (void)createSharedActionInternal(action);
    return action;
}

QAction * KexiSharedActionHost::createSharedAction(const KGuiItem& guiItem, const QKeySequence &cut,
        const char *name, KActionCollection* col)
{
    if (!col)
        col = d->mainWin->actionCollection();
    QAction * action = new QAction(guiItem.icon(), guiItem.text(), col);
    action->setObjectName(name);
    action->setShortcut(cut);
    action->setEnabled(guiItem.isEnabled());
    //! @todo how to update enable/disable? is it needed anyway?
    action->setToolTip(guiItem.toolTip());
    action->setWhatsThis(guiItem.whatsThis());
    return createSharedActionInternal(action);
}

void KexiSharedActionHost::setActionVolatile(QAction *a, bool set)
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
