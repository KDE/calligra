/* This file is part of the KDE project
   Copyright (C) 2005 - 2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTACCOUNT_H
#define KPTACCOUNT_H

#include "kplatokernel_export.h"

#include <QDateTime>
#include <QMap>
#include <QList>
#include <QStringList>

#include "kptglobal.h"
#include "kpteffortcostmap.h"
#include "kptnode.h"
#include "kptresource.h"

#include <kdebug.h>

#include <KoXmlReader.h>

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
class KPLATOKERNEL_EXPORT Account
{
public:

    /**
     * Constructor.
     */
    Account();

    /**
     * 
     */
    explicit Account(const QString& name, const QString& description=QString());
   
    /**
     * Destructor.
     */
     ~Account();
    

    QString name() const { return m_name; }
    void setName(const QString& name);
    
    QString description() const { return m_description; }
    void setDescription(const QString& desc);

    bool isElement() const { return m_accountList.isEmpty(); }
    bool isDefaultAccount() const;
    
    Accounts *list() const { return m_list; }
    void setList(Accounts *list) { m_list = list; }
    Account *parent() const { return m_parent; }
    void setParent(Account *parent) { m_parent = parent; }
    void clear() { m_accountList.clear(); }
    void insert(Account *account, int index = -1);
    void take(Account *account);
    bool isChildOf( const Account *account ) const;
    void insertChildren();
    int indexOf( Account *account ) const { return m_accountList.indexOf( account ); }
    
    bool isBaselined( long id = BASELINESCHEDULE ) const;

    bool load(KoXmlElement &element, Project &project);
    void save(QDomElement &element) const;
    
    const QList<Account*> &accountList() const { return m_accountList; }
    int childCount() const { return m_accountList.count(); }
    Account *childAt( int index ) const { return m_accountList.value( index ); }
    
    Account *findAccount() const { return findAccount(m_name); }
    Account *findAccount(const QString &id) const;
    bool removeId() { return removeId(m_name); }
    bool removeId(const QString &id);
    bool insertId();
    bool insertId(Account *account);
    
    void changed();
    
    class CostPlace {
    public:
        /// Create an empty cost place
        CostPlace() 
            : m_account(0), m_objectId(), m_node(0), m_resource(0), m_running(false), m_startup(false), m_shutdown(false)
        {}
        /// Create an empty cost place for account @p acc
        CostPlace(Account *acc) 
            : m_account(acc), m_objectId(), m_node(0), m_resource(0), m_running(false), m_startup(false), m_shutdown(false)
        {}
        /// Create a cost place for a task
        CostPlace(Account *acc, Node *node, bool running=false, bool strtup=false, bool shutdown=false);

        /// Create a cost place for a resource
        CostPlace(Account *acc, Resource *resource, bool running=false);

        CostPlace(CostPlace *cp) {
            m_account = cp->m_account;
            m_objectId = cp->m_objectId;
            m_node = cp->m_node;
            m_resource = cp->m_resource;
            m_running = cp->m_running;
            m_startup = cp->m_startup;
            m_shutdown = cp->m_shutdown;
        }
        ~CostPlace();

        bool isBaselined( long id = BASELINESCHEDULE ) const;

        bool isEmpty() { return !(m_running || m_startup || m_shutdown); }
        Node *node() const { return m_node; }
        void setNode( Node *node );

        Resource *resource() const { return m_resource; }
        void setResource( Resource *resource );

        bool running() const { return m_running; }
        void setRunning(bool on );
        bool startup() const  { return m_startup; }
        void setStartup(bool on);
        bool shutdown() const  { return m_shutdown; }
        void setShutdown(bool on);
    
        bool load(KoXmlElement &element, Project &project);
        void save(QDomElement &element) const;
        // for loading xml
        void setObjectId( const QString &id );
        QString objectId() const;

    private:
        Account *m_account;
        QString m_objectId;
        Node *m_node;
        Resource *m_resource;
        bool m_running;
        bool m_startup;
        bool m_shutdown;
    };
    
    void append(CostPlace *cp) { m_costPlaces.append(cp); }
    const QList<CostPlace*> &costPlaces() const {return m_costPlaces; }
    Account::CostPlace *findCostPlace(const Node &node) const;
    
    Account::CostPlace *findCostPlace(const Resource &resource) const;
    CostPlace *findRunning(const Resource &resource) const;
    void removeRunning(const Resource &resource);
    void addRunning(Resource &resource);

    CostPlace *findRunning(const Node &node) const;
    void removeRunning(const Node &node);
    void addRunning(Node &node);
    CostPlace *findStartup(const Node &node) const;
    void removeStartup(const Node &node);
    void addStartup(Node &node);
    CostPlace *findShutdown(const Node &node) const;
    void removeShutdown(const Node &node);
    void addShutdown(Node &node);
    void deleteCostPlace(CostPlace *cp);
    
    EffortCostMap plannedCost(long id = BASELINESCHEDULE) const;
    EffortCostMap plannedCost(const QDate &start, const QDate &end, long id = BASELINESCHEDULE) const;

    EffortCostMap actualCost(long id = BASELINESCHEDULE) const;
    EffortCostMap actualCost(const QDate &start, const QDate &end, long id = BASELINESCHEDULE) const;
    
protected:
    EffortCostMap plannedCost(const CostPlace &cp, const QDate &start, const QDate &end, long id ) const;
    EffortCostMap actualCost(const Account::CostPlace &cp, const QDate &start, const QDate &end, long id) const;

private:
    QString m_name;
    QString m_description;
    Accounts *m_list;
    Account *m_parent;
    QList<Account*> m_accountList;
    QList<CostPlace*> m_costPlaces;
    
#ifndef NDEBUG
public:
    void printDebug(const QString& indent);
#endif
};

typedef QList<Account*> AccountList;
typedef QListIterator<Account*> AccountListIterator;

/**
 *  Accounts administrates all accounts.
 */

class KPLATOKERNEL_EXPORT Accounts : public QObject
{
    Q_OBJECT
public:
    explicit Accounts(Project &project);
    ~Accounts();
    
    Account *defaultAccount() const { return m_defaultAccount; }
    void setDefaultAccount(Account *account);
    
    /// Return the planned cost from all cost places of this account added to cost from all sub-accounts
    EffortCostMap plannedCost(const Account &account, long id = BASELINESCHEDULE) const;

    /// Return the planned cost from all cost places of this account added to cost from all sub-accounts
    /// for the interval @p start to @p end inclusive
    EffortCostMap plannedCost(const Account &account, const QDate &start, const QDate &end, long id = BASELINESCHEDULE) const;
    
    /// Return the actual cost from all cost places of this account added to cost from all sub-accounts
    EffortCostMap actualCost(const Account &account, long id = BASELINESCHEDULE) const;

    /// Return the actual cost from all cost places of this account added to cost from all sub-accounts
    /// for the interval @p start to @p end inclusive
    EffortCostMap actualCost(const Account &account, const QDate &start, const QDate &end, long id = BASELINESCHEDULE) const;
    
    void clear() { m_accountList.clear(); m_idDict.clear(); }
    void insert(Account *account, Account *parent=0, int index = -1);
    void take(Account *account);
    
    bool load(KoXmlElement &element, Project &project);
    void save(QDomElement &element) const;

    QStringList costElements() const;
    QStringList nameList() const;
        
    const AccountList &accountList() const { return m_accountList; }
    int accountCount() const { return m_accountList.count(); }
    Account *accountAt( int index ) const { return m_accountList.value( index ); }
    int indexOf( Account *account ) const { return m_accountList.indexOf( account ); }

    Account *findRunningAccount(const Resource &resource) const;

    Account *findRunningAccount(const Node &node) const;
    Account *findStartupAccount(const Node &node) const;
    Account *findShutdownAccount(const Node &node) const;
    Account *findAccount(const QString &id) const;
    bool insertId(Account *account);
    bool removeId(const QString &id);
    QString uniqueId( const QString &seed ) const;
    
    void accountDeleted(Account *account) 
        { if (account == m_defaultAccount) m_defaultAccount = 0; }

    void accountChanged( Account *account );
    QList<Account*> allAccounts() const { return m_idDict.values(); }
    QList<Node*> allNodes() const;
    
signals:
    void accountAdded( const Account * );
    void accountToBeAdded( const Account *, int );
    void accountRemoved( const Account * );
    void accountToBeRemoved( const Account * );
    void changed( Account *);
    void defaultAccountChanged();

private:
    Project &m_project;
    AccountList m_accountList;
    QMap<QString, Account*> m_idDict;

    Account *m_defaultAccount;

#ifndef NDEBUG
public:
    void printDebug(const QString& indent);
#endif
};

} //namespace KPlato

#endif
