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

#ifndef KPTACCOUNT_H
#define KPTACCOUNT_H

#include <qdict.h>
#include <qptrlist.h>
#include <qstringlist.h>

class QDomElement;
class QString;

namespace KPlato
{

class Accounts;
class Account;


/**
 *  Account holds one account.
 *  An account can have any number of sub-accounts.
 *  Account names must be unique.
 */
class Account
{
public:
    Account();
    Account(QString name, QString description=QString::null);
    ~Account();
    
    QString name() const { return m_name; }
    void setName(QString name);
    
    QString description() const { return m_description; }
    void setDescription(QString desc) { m_description = desc; }

    bool isElement() const { return m_accountList.isEmpty(); }
    
    Accounts *list() const { return m_list; }
    void setList(Accounts *list) { m_list = list; }
    Account *parent() const { return m_parent; }
    void setParent(Account *parent) { m_parent = parent; }
    void clear() { m_accountList.clear(); }
    void append(Account *account);
    void take(Account *account);
    void insertChildren();
    
    bool load(QDomElement &element);
    void save(QDomElement &element) const;
    
    const QPtrList<Account> &accountList() const { return m_accountList; }
    
    Account *findAccount() const { return findAccount(m_name); }
    Account *findAccount(const QString &id) const;
    bool removeId() { return removeId(m_name); }
    bool removeId(const QString &id);
    bool insertId();
    bool insertId(const Account *account);
    
private:
    QString m_name;
    QString m_description;
    Accounts *m_list;
    Account *m_parent;
    QPtrList<Account> m_accountList;
};

typedef QPtrList<Account> AccountList;
typedef QPtrListIterator<Account> AccountListIterator;

/**
 *  Accounts administrates all accounts.
 */

class Accounts
{
public:
    Accounts();
    ~Accounts();
    void clear() { m_accountList.clear(); m_idDict.clear(); }
    void append(Account *account);
    void take(Account *account);
    
    bool load(QDomElement &element);
    void save(QDomElement &element) const;

    QStringList costElements() const;
    QStringList nameList() const;
        
    const AccountList &accountList() const { return m_accountList; }
    Account *findAccount(const QString &id) const;
    bool insertId(const Account *account);
    bool removeId(const QString &id);
    
private:
    AccountList m_accountList;
    QDict<Account> m_idDict;
};

} //namespace KPlato

#endif
