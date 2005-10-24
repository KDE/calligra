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
#include <qstring.h>

#include <klocale.h>

#include <kdebug.h>

#include "kptaccount.h"

namespace KPlato
{


KPTAccount::KPTAccount() {
}

KPTAccount::KPTAccount(QString name, QString description)
    : m_name(name),
      m_description(description) {
    
    m_accountList.setAutoDelete(true);
}

KPTAccount::~KPTAccount() {
    kdDebug()<<k_funcinfo<<m_name<<endl;
    removeId();
}
    

void KPTAccount::setName(QString name) {
    if (findAccount() == this) {
        removeId();
    }
    m_name = name;
    insertId();
}

void KPTAccount::append(KPTAccount *account){
    Q_ASSERT(account);
    m_accountList.append(account); 
    account->setList(m_list);
    account->setParent(this);
    insertId(account);
    kdDebug()<<k_funcinfo<<account->name()<<endl;
}

void KPTAccount::insertChildren() {
    KPTAccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->setList(m_list);
        it.current()->setParent(this);
        insertId(it.current());
        it.current()->insertChildren();
    }
}

void KPTAccount::remove(KPTAccount *account){
    if (account == 0) {
        return;
    }
    if (account->parent() == this) {
        m_accountList.take(m_accountList.findRef(account));
    } else {
        account->parent()->remove(account);
    }
    kdDebug()<<k_funcinfo<<account->name()<<endl;
    delete account;
}
    
bool KPTAccount::load(QDomElement &element) {
    m_name = element.attribute("name");
    m_description = element.attribute("description");
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "account") {
                KPTAccount *child = new KPTAccount();
                if (child->load(e)) {
                    m_accountList.append(child);
                } else {
                    // TODO: Complain about this
                    kdWarning()<<k_funcinfo<<"Loading failed"<<endl;
                    delete child;
                }
            }
        }
    }
    return true;
}

void KPTAccount::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("account");
    element.appendChild(me);
    me.setAttribute("name", m_name);
    me.setAttribute("description", m_description);
    KPTAccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->save(me);
    }
}

KPTAccount *KPTAccount::findAccount(const QString &id) const {
    if (m_list) 
        return m_list->findAccount(id);
    return 0;
}

bool KPTAccount::removeId(const QString &id) {
    return (m_list ? m_list->removeId(id) : false);
}

bool KPTAccount::insertId() {
    return insertId(this);
}

bool KPTAccount::insertId(const KPTAccount *account) {
    return (m_list ? m_list->insertId(account) : false);
}

//------------------------------------
KPTAccounts::KPTAccounts() {
    m_accountList.setAutoDelete(true);
}

KPTAccounts::~KPTAccounts() {
}

void KPTAccounts::append(KPTAccount *account) {
    Q_ASSERT(account);
    m_accountList.append(account); 
    account->setList(this);
    account->setParent(0); // incase...
    insertId(account);
    kdDebug()<<k_funcinfo<<account->name()<<endl;
    account->insertChildren();
}

void KPTAccounts::remove(KPTAccount *account){
    if (account == 0) {
        return;
    }
    removeId(account->name());
    if (account->parent()) {
        account->parent()->remove(account);
        return;
    }
    m_accountList.take(m_accountList.findRef(account));
    kdDebug()<<k_funcinfo<<account->name()<<endl;
    delete account;
}
    
bool KPTAccounts::load(QDomElement &element) {
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "account") {
                KPTAccount *child = new KPTAccount();
                if (child->load(e)) {
                    append(child);
                } else {
                    // TODO: Complain about this
                    kdWarning()<<k_funcinfo<<"Loading failed"<<endl;
                    delete child;
                }
            }
        }
    }
    return true;
}

void KPTAccounts::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("accounts");
    element.appendChild(me);
    KPTAccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->save(me);
    }
}

QStringList KPTAccounts::costElements() const {
    QDictIterator<KPTAccount> it(m_idDict);
    QStringList l;
    for(; it.current(); ++it) {
        if (it.current()->isElement())
            l << it.currentKey();
    }
    return l;
}
    

QStringList KPTAccounts::nameList() const {
    QDictIterator<KPTAccount> it(m_idDict);
    QStringList l;
    for(; it.current(); ++it) {
        l << it.currentKey();
    }
    return l;
}
    
KPTAccount *KPTAccounts::findAccount(const QString &id) const {
    return m_idDict.find(id);
}

bool KPTAccounts::insertId(const KPTAccount *account) {
    Q_ASSERT(account);
    KPTAccount *a = m_idDict.find(account->name());
    if (a == 0) {
        kdDebug()<<k_funcinfo<<"'"<<account->name()<<"' inserted"<<endl;
        m_idDict.insert(account->name(), account);
        return true;
    }
    if (a == account) {
        kdDebug()<<k_funcinfo<<"'"<<a->name()<<"' allready exists"<<endl;
        return true;
    }
    //TODO: Create unique id?
    kdWarning()<<k_funcinfo<<"Insert failed"<<endl;
    return false;
}

bool KPTAccounts::removeId(const QString &id) {
    return m_idDict.remove(id);
}

} //namespace KPlato
