/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jarosław Staniek <staniek@kde.org>

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
#include "KexiMainWindowIface.h"

#include <kdebug.h>
#include <kaction.h>
#include <kshortcut.h>
#include <kxmlguiclient.h>
#include <kactioncollection.h>

#include <QWidget>
#include <QIcon>
#include <kexi_global.h>

KAction_setEnabled_Helper::KAction_setEnabled_Helper(KexiActionProxy* proxy)
        : QObject()
        , m_proxy(proxy)
{
    setObjectName("KAction_setEnabled_Helper");
}

void KAction_setEnabled_Helper::slotSetEnabled(bool enabled)
{
    if (sender()->inherits("KAction")) {
        const KAction *a = static_cast<const KAction*>(sender());
        m_proxy->setAvailable(a->objectName(), enabled);
    }
}

//=======================

KexiSharedActionConnector::KexiSharedActionConnector(KexiActionProxy* proxy, QObject *obj)
        : m_proxy(proxy)
        , m_object(obj)
{
}

KexiSharedActionConnector::~KexiSharedActionConnector()
{
}

void KexiSharedActionConnector::plugSharedAction(const QString& action_name, const char *slot)
{
    m_proxy->plugSharedAction(action_name, m_object, slot);
}

void KexiSharedActionConnector::plugSharedActionToExternalGUI(
    const QString& action_name, KXMLGUIClient *client)
{
    m_proxy->plugSharedActionToExternalGUI(action_name, client);
}

void KexiSharedActionConnector::plugSharedActionsToExternalGUI(
    QList<QString> action_names, KXMLGUIClient *client)
{
    m_proxy->plugSharedActionsToExternalGUI(action_names, client);
}

//=======================

class KexiActionProxy::Private {
public:
    Private() {}

    QMap<QString, QPair<KexiActionProxySignal*, bool>* > signalsMap;
};


KexiActionProxy::KexiActionProxy(QObject *receiver, KexiSharedActionHost *host)
        : m_host(host ? host : KexiSharedActionHost::defaultHost())
        , m_receiver(receiver)
        , m_actionProxyParent(0)
        , m_signal_parent(0)
        , m_KAction_setEnabled_helper(new KAction_setEnabled_Helper(this))
        , m_focusedChild(0)
        , d(new Private)
{
    m_signal_parent.setObjectName("signal_parent");
    //m_sharedActionChildren.setAutoDelete(false); //TODO port logic to KDE4
    //m_alternativeActions.setAutoDelete(true); //TODO port logic to KDE4
    m_host->plugActionProxy(this);
}

KexiActionProxy::~KexiActionProxy()
{
    qDeleteAll(d->signalsMap);
    d->signalsMap.clear();
    //detach myself from every child
    foreach(KexiActionProxy *proxy, m_sharedActionChildren) {
        proxy->setActionProxyParent_internal(0);
    }
    //take me from parent
    if (m_actionProxyParent)
        m_actionProxyParent->takeActionProxyChild(this);

    m_host->takeActionProxyFor(m_receiver);

    delete m_KAction_setEnabled_helper;
    delete d;
}

void KexiActionProxy::plugSharedAction(const QString& action_name, QObject* receiver, const char *slot)
{
    if (action_name.isEmpty())// || !receiver || !slot)
        return;
    QPair<KexiActionProxySignal*, bool> *p = d->signalsMap.value(action_name);
    if (! p) {
        p = new QPair<KexiActionProxySignal*, bool>(new KexiActionProxySignal(&m_signal_parent), true);
        d->signalsMap.insert(action_name, p);
    }
    if (receiver && slot)
        QObject::connect(p->first, SIGNAL(invoke()), receiver, slot);
}

void KexiActionProxy::unplugSharedAction(const QString& action_name)
{
    QPair<KexiActionProxySignal*, bool> *p = d->signalsMap.take(action_name);
    if (! p)
        return;
    delete p->first;
    delete p;
}

void KexiActionProxy::plugSharedAction(const QString& action_name, QWidget* w)
{
    QAction *a = sharedAction(action_name);
    if (!a) {
        kWarning() << "KexiActionProxy::plugSharedAction(): NO SUCH ACTION: " << action_name;
        return;
    }
    w->addAction(a);
}

void KexiActionProxy::unplugSharedAction(const QString& action_name, QWidget* w)
{
    QAction *a = sharedAction(action_name);
    if (!a) {
        kWarning() << "KexiActionProxy::unplugSharedAction(): NO SUCH ACTION: " << action_name;
        return;
    }
    w->removeAction(a);
}

KAction* KexiActionProxy::plugSharedAction(const QString& action_name, const QString& alternativeText, QWidget* w)
{
    QAction *a = sharedAction(action_name);
    if (!a) {
        kWarning() << "KexiActionProxy::plugSharedAction(): NO SUCH ACTION: " << action_name;
        return 0;
    }
    QString altName = a->objectName() + "_alt";

    KAction *ka = dynamic_cast<KAction*>(a);
    Q_ASSERT(ka);
    KAction *alt_act = new KAction(0);
    alt_act->setObjectName(altName);
    alt_act->setText(alternativeText);
    alt_act->setParent(ka->parent());
    alt_act->setIcon(ka->icon());
    alt_act->setShortcut(ka->shortcut());

    QObject::connect(alt_act, SIGNAL(activated()), a, SLOT(trigger()));
    w->addAction(alt_act);

    //OK?
    m_host->updateActionAvailable(action_name, true, m_receiver);

    return alt_act;
}

void KexiActionProxy::plugSharedActionToExternalGUI(const QString& action_name, KXMLGUIClient *client)
{
    QAction *a = client->action(action_name.toLatin1().constData());
    if (!a)
        return;
    plugSharedAction(a->objectName(), a, SLOT(trigger()));

    //update availability
    setAvailable(a->objectName(), a->isEnabled());
    //changes will be signaled
    QObject::connect(a, SIGNAL(enabled(bool)), m_KAction_setEnabled_helper, SLOT(slotSetEnabled(bool)));
}

void KexiActionProxy::plugSharedActionsToExternalGUI(QList<QString> action_names, KXMLGUIClient *client)
{
    foreach(const QString &n, action_names) {
        plugSharedActionToExternalGUI(n, client);
    }
}

bool KexiActionProxy::activateSharedAction(const QString& action_name, bool alsoCheckInChildren)
{
    QPair<KexiActionProxySignal*, bool> *p = d->signalsMap.value(action_name);
    if (!p || !p->second) {
        //try in children...
        if (alsoCheckInChildren) {
            foreach(KexiActionProxy *proxy, m_sharedActionChildren) {
                if (proxy->activateSharedAction(action_name, alsoCheckInChildren))
                    return true;
            }
        }
        return m_actionProxyParent ? m_actionProxyParent->activateSharedAction(action_name, false) : false; //last chance: parent
    }
    //activate in this proxy...
    p->first->activate();
    return true;
}

QAction* KexiActionProxy::sharedAction(const QString& action_name)
{
    return m_host->mainWindow()->actionCollection()->action(action_name);
}

bool KexiActionProxy::isSupported(const QString& action_name) const
{
    QPair<KexiActionProxySignal*, bool> *p = d->signalsMap.value(action_name);
    if (!p) {
        //not supported explicitly - try in children...
        if (m_focusedChild)
            return m_focusedChild->isSupported(action_name);
        foreach(KexiActionProxy *proxy, m_sharedActionChildren) {
            if (proxy->isSupported(action_name))
                return true;
        }
        return false; //not suported
    }
    return p != 0;
}

bool KexiActionProxy::isAvailable(const QString& action_name, bool alsoCheckInChildren) const
{
    QPair<KexiActionProxySignal*, bool> *p = d->signalsMap.value(action_name);
    if (!p) {
        //not supported explicitly - try in children...
        if (alsoCheckInChildren) {
            if (m_focusedChild)
                return m_focusedChild->isAvailable(action_name, alsoCheckInChildren);
            foreach(KexiActionProxy *proxy, m_sharedActionChildren) {
                if (proxy->isSupported(action_name))
                    return proxy->isAvailable(action_name, alsoCheckInChildren);
            }
        }
        return m_actionProxyParent ? m_actionProxyParent->isAvailable(action_name, false) : false; //last chance: parent
    }
    //supported explicitly:
    return p->second != 0;
}

void KexiActionProxy::setAvailable(const QString& action_name, bool set)
{
    QPair<KexiActionProxySignal*, bool> *p = d->signalsMap.value(action_name);
    if (!p)
        return;
    p->second = set;
    m_host->updateActionAvailable(action_name, set, m_receiver);
}

void KexiActionProxy::addActionProxyChild(KexiActionProxy* child)
{
    if (!child || child == this)
        return;
    child->setActionProxyParent_internal(this);
    m_sharedActionChildren.append(child);
}

void KexiActionProxy::takeActionProxyChild(KexiActionProxy* child)
{
kDebug() << child;
    const int index = m_sharedActionChildren.indexOf(child);
    if (index != -1)
        m_sharedActionChildren.removeAt(index);
}

void KexiActionProxy::setActionProxyParent_internal(KexiActionProxy* parent)
{
    m_actionProxyParent = parent;
}

#include "kexiactionproxy_p.moc"

