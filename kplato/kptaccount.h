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

#include <qdatetime.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qstringlist.h>

#include "kpteffortcostmap.h"
#include "kptnode.h"

#include <kdebug.h>

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
    
    bool load(QDomElement &element, const Project &project);
    void save(QDomElement &element) const;
    
    const QPtrList<Account> &accountList() const { return m_accountList; }
    
    Account *findAccount() const { return findAccount(m_name); }
    Account *findAccount(const QString &id) const;
    bool removeId() { return removeId(m_name); }
    bool removeId(const QString &id);
    bool insertId();
    bool insertId(const Account *account);
    
    class CostPlace {
    public:
        CostPlace() 
            : m_account(0), m_nodeId(), m_node(0), m_running(false), m_startup(false), m_shutdown(false)
        {}
        CostPlace(Account *acc) 
            : m_account(acc), m_nodeId(), m_node(0), m_running(false), m_startup(false), m_shutdown(false)
        {}
        CostPlace(Account *acc, Node *node, bool running=false, bool strtup=false, bool shutdown=false)
            : m_account(acc), m_nodeId(node->id()), m_node(node) {
            if (node) {
                setRunning(running);
                setStartup(strtup);
                setShutdown(shutdown);
            }
        }
        CostPlace(CostPlace *cp) {
            m_account = cp->m_account;
            m_nodeId = cp->m_nodeId;
            m_node = cp->m_node;
            m_running = cp->m_running;
            m_startup = cp->m_startup;
            m_shutdown = cp->m_shutdown;
        }
        ~CostPlace();
        
        bool isEmpty() { return !(m_running || m_startup || m_shutdown); }
        Node *node() const { return m_node; }
        
        bool running() const { return m_running; }
        void setRunning(bool on );
        bool startup() const  { return m_startup; }
        void setStartup(bool on);
        bool shutdown() const  { return m_shutdown; }
        void setShutdown(bool on);
    
        bool load(QDomElement &element, const Project &project);
        void save(QDomElement &element) const;
    
    private:
        Account *m_account;
        QString m_nodeId;
        Node *m_node;
        bool m_running;
        bool m_startup;
        bool m_shutdown;
    };
    
    void append(const CostPlace *cp) { m_costPlaces.append(cp); }
    const QPtrList<CostPlace> &costPlaces() const {return m_costPlaces; }
    Account::CostPlace *findCostPlace(const Node &node) const;
    CostPlace *findRunning(const Node &node) const;
    void removeRunning(const Node &node);
    void addRunning(Node &node);
    CostPlace *findStartup(const Node &node) const;
    void removeStartup(const Node &node);
    void addStartup(Node &node);
    CostPlace *findShutdown(const Node &node) const;
    void removeShutdown(const Node &node);
    void addShutdown(Node &node);

private:
    QString m_name;
    QString m_description;
    Accounts *m_list;
    Account *m_parent;
    QPtrList<Account> m_accountList;
    QPtrList<CostPlace> m_costPlaces;
    
#ifndef NDEBUG
public:
    void printDebug(QString indent);
#endif
};

typedef QPtrList<Account> AccountList;
typedef QPtrListIterator<Account> AccountListIterator;

/**
 *  Accounts administrates all accounts.
 */

class Accounts
{
public:
    Accounts(Project &project);
    ~Accounts();
    
    Account *defaultAccount() const { return m_defaultAccount; }
    void setDefaultAccount(Account *account) { m_defaultAccount = account; }
    
    EffortCostMap plannedCost(const Account &account, const QDate &start, const QDate &end);
    
    void clear() { m_accountList.clear(); m_idDict.clear(); }
    void append(Account *account);
    void take(Account *account);
    
    bool load(QDomElement &element, const Project &project);
    void save(QDomElement &element) const;

    QStringList costElements() const;
    QStringList nameList() const;
        
    const AccountList &accountList() const { return m_accountList; }
    
    Account *findRunningAccount(const Node &node) const;
    Account *findStartupAccount(const Node &node) const;
    Account *findShutdownAccount(const Node &node) const;
    Account *findAccount(const QString &id) const;
    bool insertId(const Account *account);
    bool removeId(const QString &id);
    
    void accountDeleted(Account *account) 
        { if (account == m_defaultAccount) m_defaultAccount = 0; }
private:
    Project &m_project;
    AccountList m_accountList;
    QDict<Account> m_idDict;

    Account *m_defaultAccount;

#ifndef NDEBUG
public:
    void printDebug(QString indent);
#endif
};

} //namespace KPlato

#endif
