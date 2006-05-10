/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <QString>

#include <klocale.h>

#include <kdebug.h>

#include "kptaccount.h"
#include "kptduration.h"
#include "kptproject.h"

namespace KPlato
{

Account::Account()
    : m_name(),
      m_description(),
      m_list(0),
      m_parent(0),
      m_accountList(),
      m_costPlaces() {
    
    m_accountList.setAutoDelete(true);
    m_costPlaces.setAutoDelete(true);
}

Account::Account(QString name, QString description)
    : m_name(name),
      m_description(description),
      m_list(0),
      m_parent(0),
      m_accountList(),
      m_costPlaces() {
    
    m_accountList.setAutoDelete(true);
    m_costPlaces.setAutoDelete(true);
}

Account::~Account() {
    m_accountList.clear();
    if (findAccount() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    if (m_list)
        m_list->accountDeleted(this);
}
    

void Account::setName(QString name) {
    if (findAccount() == this) {
        removeId();
    }
    m_name = name;
    insertId();
}

void Account::append(Account *account){
    Q_ASSERT(account);
    m_accountList.append(account); 
    account->setList(m_list);
    account->setParent(this);
    insertId(account);
}

void Account::insertChildren() {
    AccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->setList(m_list);
        it.current()->setParent(this);
        insertId(it.current());
        it.current()->insertChildren();
    }
}

void Account::take(Account *account) {
    if (account == 0) {
        return;
    }
    if (account->parent() == this) {
        m_accountList.take(m_accountList.findRef(account));
    } else if (account->parent()) {
        account->parent()->take(account);
    } else {
        m_list->take(account);
    }
    //kDebug()<<k_funcinfo<<account->name()<<endl;
}
    
bool Account::load(QDomElement &element, const Project &project) {
    m_name = element.attribute("name");
    m_description = element.attribute("description");
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "costplace") {
                Account::CostPlace *child = new Account::CostPlace(this);
                if (child->load(e, project)) {
                    append(child);
                } else {
                    delete child;
                }
            } else if (e.tagName() == "account") {
                Account *child = new Account();
                if (child->load(e, project)) {
                    m_accountList.append(child);
                } else {
                    // TODO: Complain about this
                    kWarning()<<k_funcinfo<<"Loading failed"<<endl;
                    delete child;
                }
            }
        }
    }
    return true;
}

void Account::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("account");
    element.appendChild(me);
    me.setAttribute("name", m_name);
    me.setAttribute("description", m_description);
    Q3PtrListIterator<Account::CostPlace> cit = m_costPlaces;
    for (; cit.current(); ++cit) {
        cit.current()->save(me);
    }
    AccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->save(me);
    }
}

Account::CostPlace *Account::findCostPlace(const Node &node) const {
    Q3PtrListIterator<CostPlace> it = m_costPlaces;
    for (; it.current(); ++it) {
        if (&node == it.current()->node()) {
            return it.current();
        }
    }
    return 0;    
}

Account::CostPlace *Account::findRunning(const Node &node) const {
    Account::CostPlace *cp = findCostPlace(node);
    return cp && cp->running() ? cp : 0;
}

void Account::removeRunning(const Node &node) {
    Account::CostPlace *cp = findRunning(node);
    if (cp) {
        cp->setRunning(false);
        if (cp->isEmpty()) {
            m_costPlaces.removeRef(cp);
        }
    }
}

void Account::addRunning(Node &node) {
    Account::CostPlace *cp = findCostPlace(node);
    if (cp) {
        cp->setRunning(true);
        return;
    }
    append(new CostPlace(this, &node, true));
}

Account::CostPlace *Account::findStartup(const Node &node) const {
    Account::CostPlace *cp = findCostPlace(node);
    return cp && cp->startup() ? cp : 0;
}

void Account::removeStartup(const Node &node) {
    Account::CostPlace *cp = findStartup(node);
    if (cp) {
        cp->setStartup(false);
        if (cp->isEmpty()) {
            m_costPlaces.removeRef(cp);
        }
    }
}

void Account::addStartup(Node &node) {
    Account::CostPlace *cp = findCostPlace(node);
    if (cp) {
        cp->setStartup(true);
        return;
    }
    append(new CostPlace(this, &node, false, true));

}

Account::CostPlace *Account::findShutdown(const Node &node) const {
    Account::CostPlace *cp = findCostPlace(node);
    return cp && cp->shutdown() ? cp : 0;
}

void Account::removeShutdown(const Node &node) {
    Account::CostPlace *cp = findShutdown(node);
    if (cp) {
        cp->setShutdown(false);
        if (cp->isEmpty()) {
            m_costPlaces.removeRef(cp);
        }
    }
}

void Account::addShutdown(Node &node) {
    Account::CostPlace *cp = findCostPlace(node);
    if (cp) {
        cp->setShutdown(true);
        return;
    }
    append(new CostPlace(this, &node, false, false, true));
}

Account *Account::findAccount(const QString &id) const {
    if (m_list) 
        return m_list->findAccount(id);
    return 0;
}

bool Account::removeId(const QString &id) {
    return (m_list ? m_list->removeId(id) : false);
}

bool Account::insertId() {
    return insertId(this);
}

bool Account::insertId(const Account *account) {
    return (m_list ? m_list->insertId(account) : false);
}

//------------------------------------
Account::CostPlace::~CostPlace() {
    if (m_node) {
        if (m_running)
            m_node->setRunningAccount(0);
        if (m_startup)
            m_node->setStartupAccount(0);
        if (m_shutdown)
            m_node->setShutdownAccount(0);
    }
}

void Account::CostPlace::setRunning(bool on ) { 
    m_running = on;
    if (m_node)
        m_node->setRunningAccount(on ? m_account : 0);
}

void Account::CostPlace::setStartup(bool on ) { 
    m_startup = on;
    if (m_node)
        m_node->setStartupAccount(on ? m_account : 0);
}

void Account::CostPlace::setShutdown(bool on ) { 
    m_shutdown = on;
    if (m_node)
        m_node->setShutdownAccount(on ? m_account : 0);
}

bool Account::CostPlace::load(QDomElement &element, const Project &project) {
    //kDebug()<<k_funcinfo<<endl;
    m_nodeId = element.attribute("node-id");
    if (m_nodeId.isEmpty()) {
        kError()<<k_funcinfo<<"No node id"<<endl;
        return false;
    }
    m_node = project.findNode(m_nodeId);
    if (m_node == 0) {
        kError()<<k_funcinfo<<"Cannot not find node with id: "<<m_nodeId<<endl;
        return false;
    }
    setRunning(element.attribute("running-cost").toInt());
    setStartup(element.attribute("startup-cost").toInt());
    setShutdown(element.attribute("shutdown-cost").toInt());
    return true;
}

void Account::CostPlace::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("costplace");
    element.appendChild(me);
    me.setAttribute("node-id", m_nodeId);
    me.setAttribute("running-cost", m_running);
    me.setAttribute("startup-cost", m_startup);
    me.setAttribute("shutdown-cost", m_shutdown);
    
}

//---------------------------------
Accounts::Accounts(Project &project)
    : m_project(project),
      m_accountList(),
      m_idDict(),
      m_defaultAccount(0) {
      
    m_accountList.setAutoDelete(true);
}

Accounts::~Accounts() {
    m_accountList.clear();
}

EffortCostMap Accounts::plannedCost(const Account &account, const QDate &start, const QDate &end) {
    EffortCostMap ec;
    Q3PtrListIterator<Account::CostPlace> it = account.costPlaces();
    for (; it.current(); ++it) {
        Node *n = it.current()->node();
        if (n == 0) {
            continue;
        }
        //kDebug()<<k_funcinfo<<"n="<<n->name()<<endl;
        if (it.current()->running()) {
            ec += n->plannedEffortCostPrDay(start, end);
        }
        if (it.current()->startup()) {
            if (n->startTime().date() >= start &&
                n->startTime().date() <= end)
                ec.add(n->startTime().date(), EffortCost(Duration::zeroDuration, n->startupCost()));
        }
        if (it.current()->shutdown()) {
            if (n->endTime().date() >= start &&
                n->endTime().date() <= end)
                ec.add(n->endTime().date(), EffortCost(Duration::zeroDuration, n->shutdownCost()));
        }
    }
    if (&account == m_defaultAccount) {
        Q3DictIterator<Node> nit = m_project.nodeDict();
        for (; nit.current(); ++nit) {
            Node *n = nit.current();
            if (n->runningAccount() == 0) {
                ec += n->plannedEffortCostPrDay(start, end);
            }
            if (n->startupAccount() == 0) {
                if (n->startTime().date() >= start &&
                    n->startTime().date() <= end)
                    ec.add(n->startTime().date(), EffortCost(Duration::zeroDuration, n->startupCost()));
            }
            if (n->shutdownAccount() == 0) {
                if (n->endTime().date() >= start &&
                    n->endTime().date() <= end)
                    ec.add(n->endTime().date(), EffortCost(Duration::zeroDuration, n->shutdownCost()));
            }
        }
    }
    return ec;
}

void Accounts::append(Account *account) {
    Q_ASSERT(account);
    m_accountList.append(account); 
    account->setList(this);
    account->setParent(0); // incase...
    insertId(account);
    //kDebug()<<k_funcinfo<<account->name()<<endl;
    account->insertChildren();
}

void Accounts::take(Account *account){
    if (account == 0) {
        return;
    }
    removeId(account->name());
    if (account->parent()) {
        account->parent()->take(account);
        return;
    }
    m_accountList.take(m_accountList.findRef(account));
    //kDebug()<<k_funcinfo<<account->name()<<endl;
}
    
bool Accounts::load(QDomElement &element, const Project &project) {
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "account") {
                Account *child = new Account();
                if (child->load(e, project)) {
                    append(child);
                } else {
                    // TODO: Complain about this
                    kWarning()<<k_funcinfo<<"Loading failed"<<endl;
                    delete child;
                }
            }
        }
    }
    if (element.hasAttribute("default-account")) {
        m_defaultAccount = findAccount(element.attribute("default-account"));
        if (m_defaultAccount == 0) {
            kWarning()<<k_funcinfo<<"Could not find default account."<<endl;
        }
    }
    return true;
}

void Accounts::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("accounts");
    element.appendChild(me);
    if (m_defaultAccount) {
        me.setAttribute("default-account", m_defaultAccount->name());
    }
    AccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->save(me);
    }
}

QStringList Accounts::costElements() const {
    Q3DictIterator<Account> it(m_idDict);
    QStringList l;
    for(; it.current(); ++it) {
        if (it.current()->isElement())
            l << it.currentKey();
    }
    return l;
}
    

QStringList Accounts::nameList() const {
    Q3DictIterator<Account> it(m_idDict);
    QStringList l;
    for(; it.current(); ++it) {
        l << it.currentKey();
    }
    return l;
}
    
Account *Accounts::findRunningAccount(const Node &node) const {
    Q3DictIterator<Account> it = m_idDict;
    for (; it.current(); ++it) {
        if (it.current()->findRunning(node))
            return it.current();
    }
    return 0;
}

Account *Accounts::findStartupAccount(const Node &node) const {
    Q3DictIterator<Account> it = m_idDict;
    for (; it.current(); ++it) {
        if (it.current()->findStartup(node))
            return it.current();
    }
    return 0;
}

Account *Accounts::findShutdownAccount(const Node &node) const {
    Q3DictIterator<Account> it = m_idDict;
    for (; it.current(); ++it) {
        if (it.current()->findShutdown(node))
            return it.current();
    }
    return 0;
}

Account *Accounts::findAccount(const QString &id) const {
    return m_idDict.find(id);
}

bool Accounts::insertId(const Account *account) {
    Q_ASSERT(account);
    Account *a = m_idDict.find(account->name());
    if (a == 0) {
        //kDebug()<<k_funcinfo<<"'"<<account->name()<<"' inserted"<<endl;
        m_idDict.insert(account->name(), account);
        return true;
    }
    if (a == account) {
        kDebug()<<k_funcinfo<<"'"<<a->name()<<"' allready exists"<<endl;
        return true;
    }
    //TODO: Create unique id?
    kWarning()<<k_funcinfo<<"Insert failed"<<endl;
    return false;
}

bool Accounts::removeId(const QString &id) {
    bool res = m_idDict.remove(id);
    //kDebug()<<k_funcinfo<<id<<": removed="<<res<<endl;
    return res;
}

#ifndef NDEBUG
void Accounts::printDebug(QString /*indent*/) {
}
void Account::printDebug(QString /*indent*/) {
}
#endif
} //namespace KPlato
