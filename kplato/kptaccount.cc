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


Account::Account() {
    m_accountList.setAutoDelete(true);
}

Account::Account(QString name, QString description)
    : m_name(name),
      m_description(description) {
    
    m_accountList.setAutoDelete(true);
}

Account::~Account() {
    kdDebug()<<k_funcinfo<<m_name<<": "<<m_accountList.count()<<endl;
    m_accountList.clear();
    removeId();
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
    kdDebug()<<k_funcinfo<<account->name()<<endl;
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

void Account::take(Account *account){
    if (account == 0) {
        return;
    }
    if (account->parent() == this) {
        m_accountList.take(m_accountList.findRef(account));
    } else {
        account->parent()->take(account);
    }
    kdDebug()<<k_funcinfo<<account->name()<<endl;
}
    
bool Account::load(QDomElement &element) {
    m_name = element.attribute("name");
    m_description = element.attribute("description");
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "account") {
                Account *child = new Account();
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

void Account::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("account");
    element.appendChild(me);
    me.setAttribute("name", m_name);
    me.setAttribute("description", m_description);
    AccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->save(me);
    }
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
Accounts::Accounts() {
    m_accountList.setAutoDelete(true);
}

Accounts::~Accounts() {
    m_accountList.clear();
}

void Accounts::append(Account *account) {
    Q_ASSERT(account);
    m_accountList.append(account); 
    account->setList(this);
    account->setParent(0); // incase...
    insertId(account);
    kdDebug()<<k_funcinfo<<account->name()<<endl;
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
    kdDebug()<<k_funcinfo<<account->name()<<endl;
}
    
bool Accounts::load(QDomElement &element) {
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "account") {
                Account *child = new Account();
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

void Accounts::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("accounts");
    element.appendChild(me);
    AccountListIterator it = m_accountList;
    for (; it.current(); ++it) {
        it.current()->save(me);
    }
}

QStringList Accounts::costElements() const {
    QDictIterator<Account> it(m_idDict);
    QStringList l;
    for(; it.current(); ++it) {
        if (it.current()->isElement())
            l << it.currentKey();
    }
    return l;
}
    

QStringList Accounts::nameList() const {
    QDictIterator<Account> it(m_idDict);
    QStringList l;
    for(; it.current(); ++it) {
        l << it.currentKey();
    }
    return l;
}
    
Account *Accounts::findAccount(const QString &id) const {
    return m_idDict.find(id);
}

bool Accounts::insertId(const Account *account) {
    Q_ASSERT(account);
    Account *a = m_idDict.find(account->name());
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

bool Accounts::removeId(const QString &id) {
    bool res = m_idDict.remove(id);
    kdDebug()<<k_funcinfo<<id<<": removed="<<res<<endl;
    return res;
}

} //namespace KPlato
