/* This file is part of the KDE project
   Copyright (C) 2005, 2006, 2007, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptaccount.h"

#include <QDomDocument>
#include <QString>
#include <QDate>

#include <klocale.h>

#include "kptduration.h"
#include "kptproject.h"
#include "kptdebug.h"

#include <KoXmlReader.h>


namespace KPlato
{

Account::Account()
    : m_name(),
      m_description(),
      m_list(0),
      m_parent(0),
      m_accountList(),
      m_costPlaces() {
    
}

Account::Account(const QString& name, const QString& description)
    : m_name(name),
      m_description(description),
      m_list(0),
      m_parent(0),
      m_accountList(),
      m_costPlaces() {
    
}

Account::~Account() {
    //kDebug(planDbg())<<m_name;
    if (findAccount() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    if (m_list)
        m_list->accountDeleted(this);

    while (!m_accountList.isEmpty())
        delete m_accountList.takeFirst();
    
    while (!m_costPlaces.isEmpty())
        delete m_costPlaces.takeFirst();
}
    
bool Account::isDefaultAccount() const
{
    return m_list == 0 ? false : m_list->defaultAccount() == this;
}

void Account::changed() {
    if ( m_list ) {
        m_list->accountChanged( this );
    }
}

void Account::setName(const QString& name) {
    if (findAccount() == this) {
        removeId();
    }
    m_name = name;
    insertId();
    changed();
}

void Account::setDescription(const QString& desc)
{
    m_description = desc;
    changed();
}

void Account::insert(Account *account, int index) {
    Q_ASSERT(account);
    int i = index == -1 ? m_accountList.count() : index;
    m_accountList.insert(i, account);
    account->setList(m_list);
    account->setParent(this);
    insertId(account);
    account->insertChildren();
}

void Account::insertChildren() {
    foreach (Account *a, m_accountList) {
        a->setList(m_list);
        a->setParent(this);
        insertId(a);
        a->insertChildren();
    }
}

void Account::take(Account *account) {
    if (account == 0) {
        return;
    }
    if (account->parent() == this) {
        int i = m_accountList.indexOf(account);
        if (i != -1)
            m_accountList.removeAt(i);
    } else if (account->parent()) {
        account->parent()->take(account);
    } else {
        m_list->take(account);
    }
    //kDebug(planDbg())<<account->name();
}

bool Account::isChildOf( const Account *account) const
{
    if ( m_parent == 0 ) {
        return false;
    }
    if ( m_parent == account ) {
        return true;
    }
    return  m_parent->isChildOf( account );
}

bool Account::isBaselined( long id ) const
{
    foreach ( CostPlace *p, m_costPlaces ) {
        if ( p->isBaselined( id ) ) {
            return true;
        }
    }
    return false;
}

bool Account::load(KoXmlElement &element, Project &project) {
    m_name = element.attribute("name");
    m_description = element.attribute("description");
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
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
                kWarning()<<"Loading failed";
                delete child;
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
    foreach (Account::CostPlace *cp, m_costPlaces) {
        cp->save(me);
    }
    foreach (Account *a, m_accountList) {
        a->save(me);
    }
}

Account::CostPlace *Account::findCostPlace(const Node &node) const {
    foreach (Account::CostPlace *cp, m_costPlaces) {
        if (&node == cp->node()) {
            return cp;
        }
    }
    return 0;
}

Account::CostPlace *Account::findCostPlace(const Resource &resource) const {
    foreach (Account::CostPlace *cp, m_costPlaces) {
        if (&resource == cp->resource()) {
            return cp;
        }
    }
    return 0;
}

Account::CostPlace *Account::findRunning(const Resource &resource) const {
    Account::CostPlace *cp = findCostPlace(resource);
    return cp && cp->running() ? cp : 0;
}

void Account::removeRunning(const Resource &resource) {
    Account::CostPlace *cp = findRunning(resource);
    if (cp) {
        cp->setRunning(false);
        if (cp->isEmpty()) {
            deleteCostPlace(cp);
        }
    }
}

void Account::addRunning(Resource &resource) {
    Account::CostPlace *cp = findCostPlace(resource);
    if (cp) {
        cp->setRunning(true);
        changed();
        return;
    }
    cp = new CostPlace(this, &resource, true);
    append(cp);
    changed();
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
            deleteCostPlace(cp);
        }
    }
}

void Account::addRunning(Node &node) {
    Account::CostPlace *cp = findCostPlace(node);
    if (cp) {
        cp->setRunning(true);
        changed();
        return;
    }
    cp = new CostPlace(this, &node, true);
    append(cp);
    changed();
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
            deleteCostPlace(cp);
        }
        changed();
    }
}

void Account::addStartup(Node &node) {
    Account::CostPlace *cp = findCostPlace(node);
    if (cp) {
        cp->setStartup(true);
        changed();
        return;
    }
    cp = new CostPlace(this, &node, false, true, false);
    append(cp);
    changed();
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
            deleteCostPlace(cp);
        }
        changed();
    }
}

void Account::addShutdown(Node &node) {
    Account::CostPlace *cp = findCostPlace(node);
    if (cp) {
        cp->setShutdown(true);
        changed();
        return;
    }
    cp = new CostPlace(this, &node, false, false, true);
    append(cp);
    changed();
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

bool Account::insertId(Account *account) {
    return (m_list ? m_list->insertId(account) : false);
}

void Account::deleteCostPlace(CostPlace *cp) {
    //kDebug(planDbg());
    int i = m_costPlaces.indexOf(cp);
    if (i != -1)
        m_costPlaces.removeAt(i);
    delete cp;
}

EffortCostMap Account::plannedCost(long id) const
{
    return plannedCost( QDate(), QDate(), id );
}

EffortCostMap Account::plannedCost(const QDate &start, const QDate &end, long id) const {
    EffortCostMap ec;
    if ( ! isElement() ) {
        foreach ( Account *a, m_accountList ) {
            ec += a->plannedCost( start, end, id );
        }
    }
    foreach (Account::CostPlace *cp, m_costPlaces) {
        ec += plannedCost( *cp, start, end, id );
    }
    if (isDefaultAccount()) {
        QList<Node*> list = m_list == 0 ? QList<Node*>() : m_list->allNodes();
        foreach (Node *n, list) {
            if ( n->numChildren() > 0 ) {
                continue;
            }
            if (n->runningAccount() == 0) {
                ec += n->plannedEffortCostPrDay(start, end, id);
            }
            if (n->startupAccount() == 0) {
                if ( ( ! start.isValid() || n->startTime( id ).date() >= start ) &&
                     ( ! end.isValid() || n->startTime( id ).date() <= end ) ) {
                    ec.add(n->startTime( id ).date(), EffortCost(Duration::zeroDuration, n->startupCost()));
                }
            }
            if (n->shutdownAccount() == 0) {
                if ( ( ! start.isValid() || n->endTime( id ).date() >= start ) &&
                     ( ! end.isValid() || n->endTime( id ).date() <= end ) ) {
                    ec.add(n->endTime( id ).date(), EffortCost(Duration::zeroDuration, n->shutdownCost()));
                }
            }
        }
    }
    return ec;
}

EffortCostMap Account::plannedCost( const Account::CostPlace &cp, const QDate &start, const QDate &end, long id) const
{
    EffortCostMap ec;
    if ( cp.node() ) {
        Node &node = *(cp.node());
        //kDebug(planDbg())<<"n="<<n->name();
        if (cp.running()) {
            ec += node.plannedEffortCostPrDay(start, end, id);
        }
        if (cp.startup()) {
            if ( ( ! start.isValid() || node.startTime( id ).date() >= start ) &&
                 ( ! end.isValid() || node.startTime( id ).date() <= end ) ) {
                ec.add(node.startTime( id ).date(), EffortCost(Duration::zeroDuration, node.startupCost()));
            }
        }
        if (cp.shutdown()) {
            if ( ( ! start.isValid() || node.endTime( id ).date() >= start ) &&
                 ( ! end.isValid() || node.endTime( id ).date() <= end ) ) {
                ec.add(node.endTime( id ).date(), EffortCost(Duration::zeroDuration, node.shutdownCost()));
            }
        }
    } else if ( cp.resource() ) {
        if ( cp.running() ) {
            ec += cp.resource()->plannedEffortCostPrDay(start, end, id);
        }
    }
    return ec;
}

EffortCostMap Account::actualCost(long id) const
{
    return actualCost( QDate(), QDate(), id );
}

EffortCostMap Account::actualCost(const QDate &start, const QDate &end, long id) const
{
    EffortCostMap ec;
    if ( ! isElement() ) {
        foreach ( Account *a, m_accountList ) {
            ec += a->actualCost( start, end, id );
        }
    }
    foreach (Account::CostPlace *cp, costPlaces()) {
        ec += actualCost( *cp, start, end, id );
    }
    if (isDefaultAccount()) {
        QList<Node*> list = m_list == 0 ? QList<Node*>() : m_list->allNodes();
        foreach (Node *n, list) {
            if ( n->numChildren() > 0 ) {
                continue;
            }
            if (n->runningAccount() == 0) {
                //kDebug(planDbg())<<"default, running:"<<n->name();
                ec += n->actualEffortCostPrDay(start, end, id);
            }
            Task *t = dynamic_cast<Task*>( n ); // only tasks have completion
            if ( t ) {
                if (n->startupAccount() == 0 && t->completion().isStarted()) {
                    const QDate startDate = t->completion().startTime().date();
                    if ( ( ! start.isValid() || startDate >= start ) &&
                        ( ! end.isValid() || startDate <= end ) ) {
                        ec.add(startDate, EffortCost(Duration::zeroDuration, n->startupCost()));
                    }
                }
                if (n->shutdownAccount() == 0 && t->completion().isFinished()) {
                    //kDebug(planDbg())<<"default, shutdown:"<<n->name();
                    const QDate finishDate = t->completion().finishTime().date();
                    if ( ( ! start.isValid() || finishDate >= start ) &&
                        ( ! end.isValid() || finishDate <= end ) ) {
                        ec.add(finishDate, EffortCost(Duration::zeroDuration, n->shutdownCost()));
                    }
                }
            }
        }
    }
    return ec;
}

EffortCostMap Account::actualCost(const Account::CostPlace &cp, const QDate &start, const QDate &end, long id) const
{
    EffortCostMap ec;
    if ( cp.node() ) {
        Node &node = *(cp.node());
        if (cp.running()) {
            ec += node.actualEffortCostPrDay(start, end, id);
        }
        Task *t = dynamic_cast<Task*>( &node ); // only tasks have completion
        if ( t ) {
            if (cp.startup() && t->completion().isStarted()) {
                const QDate startDate = t->completion().startTime().date();
                if ( ( ! start.isValid() || startDate >= start ) &&
                    ( ! end.isValid() || startDate <= end ) ) {
                    ec.add(startDate, EffortCost(Duration::zeroDuration, node.startupCost()));
                }
            }
            if (cp.shutdown() && t->completion().isFinished()) {
                const QDate finishDate = t->completion().finishTime().date();
                if ( ( ! start.isValid() || finishDate >= start ) &&
                    ( ! end.isValid() || finishDate <= end ) ) {
                    ec.add(finishDate, EffortCost(Duration::zeroDuration, node.shutdownCost()));
                }
            }
        }
    } else if ( cp.resource() && m_list ) {
        foreach ( Node *n, m_list->allNodes() ) {
            if ( n->type() == Node::Type_Task ) {
                ec += n->actualEffortCostPrDay( cp.resource(), start, end, id );
            }
        }
    }
    return ec;
}


//------------------------------------
Account::CostPlace::CostPlace(Account *acc, Node *node, bool running, bool strtup, bool shutdown)
    : m_account(acc), 
    m_objectId(node->id()),
    m_node(node),
    m_resource(0),
    m_running( false ),
    m_startup( false ),
    m_shutdown( false )
{
    if (node) {
        if (running) setRunning(running);
        if (strtup) setStartup(strtup);
        if (shutdown) setShutdown(shutdown);
    }
}

Account::CostPlace::CostPlace(Account *acc, Resource *resource, bool running)
    : m_account(acc), 
    m_objectId(resource->id()),
    m_node(0),
    m_resource(resource),
    m_running( false ),
    m_startup( false ),
    m_shutdown( false )
{
    if (resource) {
        if (running) setRunning(running);
    }
}

Account::CostPlace::~CostPlace() {
    if (m_node) {
        if (m_running)
            m_node->setRunningAccount(0);
        if (m_startup)
            m_node->setStartupAccount(0);
        if (m_shutdown)
            m_node->setShutdownAccount(0);
    }
    if (m_resource) {
        if (m_running)
            m_resource->setAccount(0);
    }
}

bool Account::CostPlace::isBaselined( long id ) const
{
    if ( m_node ) {
        if ( m_running ) {
            if ( m_node->isBaselined( id ) ) {
                return true;
            }
        }
        if ( m_startup ) {
            if ( m_node->isBaselined( id ) ) {
                return true;
            }
        }
        if ( m_shutdown ) {
            if ( m_node->isBaselined( id ) ) {
                return true;
            }
        }
    }
    if ( m_resource ) {
        if ( m_running ) {
            if ( m_resource->isBaselined( id ) ) {
                return true;
            }
        }
    }
    return false;
}

void Account::CostPlace::setNode(Node* node)
{
    Q_ASSERT( ! m_node );
    m_node = node;
}

void Account::CostPlace::setResource(Resource* resource)
{
    Q_ASSERT( ! m_resource );
    m_resource = resource;
}

void Account::CostPlace::setRunning(bool on ) { 
    m_running = on;
    if (m_node) {
        m_node->setRunningAccount(on ? m_account : 0);
    } else if ( m_resource ) {
        m_resource->setAccount(on ? m_account : 0);
    }
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

//TODO
bool Account::CostPlace::load(KoXmlElement &element, Project &project) {
    //kDebug(planDbg());
    m_objectId = element.attribute("object-id");
    if (m_objectId.isEmpty()) {
        // check old format
        m_objectId = element.attribute("node-id");
        if (m_objectId.isEmpty()) {
            kError()<<"No object id";
            return false;
        }
    }
    m_node = project.findNode(m_objectId);
    if (m_node == 0) {
        m_resource = project.findResource(m_objectId);
        if ( m_resource == 0 ) {
            kError()<<"Cannot find object with id: "<<m_objectId;
            return false;
        }
    }
    bool on = (bool)(element.attribute("running-cost").toInt());
    if ( on ) setRunning( on );
    on = (bool)(element.attribute("startup-cost").toInt());
    if ( on ) setStartup( on );
    on = (bool)(element.attribute("shutdown-cost").toInt());
    if ( on ) setShutdown( on );
    return true;
}

void Account::CostPlace::save(QDomElement &element) const {
    //kDebug(planDbg());
    QDomElement me = element.ownerDocument().createElement("costplace");
    element.appendChild(me);
    me.setAttribute("object-id", m_objectId);
    me.setAttribute("running-cost", m_running);
    me.setAttribute("startup-cost", m_startup);
    me.setAttribute("shutdown-cost", m_shutdown);
    
}

void Account::CostPlace::setObjectId( const QString& id )
{
    m_objectId = id;
}

QString Account::CostPlace::objectId() const
{
    return m_objectId;
}

//---------------------------------
Accounts::Accounts(Project &project)
    : m_project(project),
      m_accountList(),
      m_idDict(),
      m_defaultAccount(0) {
      
}

Accounts::~Accounts() {
    //kDebug(planDbg());
    while (!m_accountList.isEmpty()) {
        delete m_accountList.takeFirst();
    }
}

EffortCostMap Accounts::plannedCost(const Account &account, long id) const
{
    return account.plannedCost( id );
}

EffortCostMap Accounts::plannedCost(const Account &account, const QDate &start, const QDate &end, long id) const
{
    return account.plannedCost( start, end, id );
}

EffortCostMap Accounts::actualCost(const Account &account, long id) const
{
    return account.actualCost( id );
}

EffortCostMap Accounts::actualCost(const Account &account, const QDate &start, const QDate &end, long id) const
{
    return account.actualCost( start, end, id );
}

void Accounts::insert(Account *account, Account *parent, int index) {
    Q_ASSERT(account);
    if ( parent == 0 ) {
        int i = index == -1 ? m_accountList.count() : index;
        emit accountToBeAdded( parent, i );
        m_accountList.insert(i, account);
        account->setList(this);
        account->setParent(0); // incase...
        insertId(account);
        account->insertChildren();
    } else {
        int i = index == -1 ? parent->accountList().count() : index;
        emit accountToBeAdded( parent, i );
        parent->insert( account, i );
    }
    //kDebug(planDbg())<<account->name();
    emit accountAdded( account );
}

void Accounts::take(Account *account){
    if (account == 0) {
        return;
    }
    removeId(account->name());
    if (account->parent()) {
        emit accountToBeRemoved( account );
        account->parent()->take(account);
        emit accountRemoved( account );
        //kDebug(planDbg())<<account->name();
        return;
    }
    int i = m_accountList.indexOf(account);
    if (i != -1) {
        emit accountToBeRemoved( account );
        m_accountList.removeAt(i);
        emit accountRemoved( account );
    }
    //kDebug(planDbg())<<account->name();
}
    
bool Accounts::load(KoXmlElement &element, Project &project) {
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "account") {
            Account *child = new Account();
            if (child->load(e, project)) {
                insert(child);
            } else {
                // TODO: Complain about this
                kWarning()<<"Loading failed";
                delete child;
            }
        }
    }
    if (element.hasAttribute("default-account")) {
        m_defaultAccount = findAccount(element.attribute("default-account"));
        if (m_defaultAccount == 0) {
            kWarning()<<"Could not find default account.";
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
    foreach (Account *a, m_accountList) {
        a->save(me);
    }
}

QStringList Accounts::costElements() const {
    QStringList l;
    foreach (const QString &key, m_idDict.uniqueKeys()) {
        if (m_idDict[key]->isElement())
            l << key;
    }
    return l;
}
    

QStringList Accounts::nameList() const {
    return m_idDict.uniqueKeys();
}

Account *Accounts::findRunningAccount(const Resource &resource) const {
    foreach (Account *a, m_idDict) {
        if (a->findRunning(resource)) {
            return a;
        }
    }
    return 0;
}

Account *Accounts::findRunningAccount(const Node &node) const {
    foreach (Account *a, m_idDict) {
        if (a->findRunning(node))
            return a;
    }
    return 0;
}

Account *Accounts::findStartupAccount(const Node &node) const {
    foreach (Account *a, m_idDict) {
        if (a->findStartup(node))
            return a;
    }
    return 0;
}

Account *Accounts::findShutdownAccount(const Node &node) const {
    foreach (Account *a, m_idDict) {
        if (a->findShutdown(node))
            return a;
    }
    return 0;
}

Account *Accounts::findAccount(const QString &id) const {
    return m_idDict.value(id);
}

bool Accounts::insertId(Account *account) {
    Q_ASSERT(account);
    Account *a = findAccount(account->name());
    if (a == 0) {
        //kDebug(planDbg())<<"'"<<account->name()<<"' inserted";
        m_idDict.insert(account->name(), account);
        return true;
    }
    if (a == account) {
        kDebug(planDbg())<<"'"<<a->name()<<"' already exists";
        return true;
    }
    //TODO: Create unique id?
    kWarning()<<"Insert failed, creating unique id";
    account->setName( uniqueId( account->name() ) ); // setName() calls insertId !!
    return false;
}

bool Accounts::removeId(const QString &id) {
    bool res = m_idDict.remove(id);
    //kDebug(planDbg())<<id<<": removed="<<res;
    return res;
}

QString Accounts::uniqueId( const QString &seed ) const
{
    QString s = seed.isEmpty() ? i18n( "Account" ) + ".%1" : seed + ".%1";
    int i = 1;
    QString n = s.arg( i );
    while (  findAccount( n ) ) {
        n = s.arg( ++i );
    }
    return n;
}

void Accounts::setDefaultAccount(Account *account)
{
    Account *a = m_defaultAccount;
    m_defaultAccount = account;
    if ( a ) {
        emit changed( a );
    }
    if ( account ) {
        emit changed( account );
    }
    if ( a != account ) {
        emit defaultAccountChanged();
    }
}

void Accounts::accountChanged( Account *account ) 
{
    emit changed( account );
}

QList<Node*> Accounts::allNodes() const
{
    return m_project.allNodes();
}

#ifndef NDEBUG
void Accounts::printDebug(const QString& indent) {
    kDebug(planDbg())<<indent<<"Accounts:"<<m_accountList.count()<<" children";
    foreach( Account *a, m_accountList ) {
        a->printDebug( indent + "    !" );
    }
}
void Account::printDebug(const QString& indent) {
    kDebug(planDbg())<<indent<<"--- Account:"<<m_name<<":"<<m_accountList.count()<<" children";
    foreach( Account *a, m_accountList ) {
        a->printDebug( indent + "    !" );
    }
}
#endif
} //namespace KPlato

#include "kptaccount.moc"
