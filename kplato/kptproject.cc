/* This file is part of the KDE project
 Copyright (C) 2001 Thomas zander <zander@kde.org>
 Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

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

#include "kptproject.h"
#include "kptappointment.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptpart.h"
#include "kptconfig.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"

#include <qdom.h>
#include <QString>
#include <qdatetime.h>
#include <qbrush.h>
#include <q3canvas.h>
#include <QList>

#include <kdatetimewidget.h>
#include <kdebug.h>

namespace KPlato
{


Project::Project( Node *parent )
        : Node( parent ),
        m_accounts( *this )
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_constraint = Node::MustStartOn;
    m_standardWorktime = new StandardWorktime();
    init();
}

void Project::init()
{
    if ( m_parent == 0 ) {
        // set sensible defaults for a project wo parent
        m_constraintStartTime = QDateTime( QDate::currentDate(), QTime() );
        m_constraintEndTime = m_constraintStartTime.addDays( 1 );
    }
}


Project::~Project()
{
    delete m_standardWorktime;
    while ( !m_resourceGroups.isEmpty() )
        delete m_resourceGroups.takeFirst();
    while ( !m_calendars.isEmpty() )
        delete m_calendars.takeFirst();
}

int Project::type() const { return Node::Type_Project; }

void Project::calculate( Schedule *schedule )
{
    if ( schedule == 0 ) {
        kError() << k_funcinfo << "Schedule == 0, cannot calculate" << endl;
        return ;
    }
    m_currentSchedule = schedule;
    calculate();
}

void Project::calculate( Effort::Use estType )
{
    m_currentSchedule = findSchedule( ( Schedule::Type ) estType );
    if ( m_currentSchedule == 0 ) {
        m_currentSchedule = createSchedule( i18n( "Standard" ), ( Schedule::Type ) estType );
    }
    calculate();
}

void Project::calculate()
{
    if ( m_currentSchedule == 0 ) {
        kError() << k_funcinfo << "No current schedule to calculate" << endl;
        return ;
    }
    Effort::Use estType = ( Effort::Use ) m_currentSchedule->type();
    if ( type() == Type_Project ) {
        initiateCalculation( *m_currentSchedule );
        if ( m_constraint == Node::MustStartOn ) {
            //kDebug()<<k_funcinfo<<"Node="<<m_name<<" Start="<<m_constraintStartTime.toString()<<endl;
            m_currentSchedule->startTime = m_constraintStartTime;
            m_currentSchedule->earliestStart = m_constraintStartTime;
            // Calculate from start time
            propagateEarliestStart( m_currentSchedule->earliestStart );
            m_currentSchedule->latestFinish = calculateForward( estType );
            propagateLatestFinish( m_currentSchedule->latestFinish );
            calculateBackward( estType );
            m_currentSchedule->endTime = scheduleForward( m_currentSchedule->startTime, estType );
            calcCriticalPath( false );
        } else {
            //kDebug()<<k_funcinfo<<"Node="<<m_name<<" End="<<m_constraintEndTime.toString()<<endl;
            m_currentSchedule->endTime = m_constraintEndTime;
            m_currentSchedule->latestFinish = m_constraintEndTime;
            // Calculate from end time
            propagateLatestFinish( m_currentSchedule->latestFinish );
            m_currentSchedule->earliestStart = calculateBackward( estType );
            propagateEarliestStart( m_currentSchedule->earliestStart );
            calculateForward( estType );
            m_currentSchedule->startTime = scheduleBackward( m_currentSchedule->endTime, estType );
            calcCriticalPath( true );
        }
        makeAppointments();
        calcResourceOverbooked();
        m_currentSchedule->notScheduled = false;
    } else if ( type() == Type_Subproject ) {
        kWarning() << k_funcinfo << "Subprojects not implemented" << endl;
    } else {
        kError() << k_funcinfo << "Illegal project type: " << type() << endl;
    }
}

bool Project::calcCriticalPath( bool fromEnd )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( fromEnd ) {
        QListIterator<Node*> startnodes = m_startNodes;
        while ( startnodes.hasNext() ) {
            startnodes.next() ->calcCriticalPath( fromEnd );
        }
    } else {
        QListIterator<Node*> endnodes = m_endNodes;
        while ( endnodes.hasNext() ) {
            endnodes.next() ->calcCriticalPath( fromEnd );
        }
    }
    return false;
}

DateTime Project::startTime() const
{
    //kDebug()<<k_funcinfo<<(m_currentSchedule?m_currentSchedule->id():-1)<<" "<<(m_currentSchedule?m_currentSchedule->typeToString():"")<<endl;
    if ( m_currentSchedule )
        return m_currentSchedule->startTime;

    return m_constraintStartTime;
}

DateTime Project::endTime() const
{
    //kDebug()<<k_funcinfo<<(m_currentSchedule?m_currentSchedule->id():-1)<<" "<<(m_currentSchedule?m_currentSchedule->typeToString():"")<<endl;
    if ( m_currentSchedule )
        return m_currentSchedule->endTime;

    return m_constraintEndTime;
}

Duration *Project::getExpectedDuration()
{
    //kDebug()<<k_funcinfo<<endl;
    return new Duration( getLatestFinish() - getEarliestStart() );
}

Duration *Project::getRandomDuration()
{
    return 0L;
}

DateTime Project::calculateForward( int use )
{
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if ( type() == Node::Type_Project ) {
        // Follow *parent* relations back and
        // calculate forwards following the child relations
        DateTime finish;
        DateTime time;
        QListIterator<Node*> endnodes = m_endNodes;
        while ( endnodes.hasNext() ) {
            time = endnodes.next() ->calculateForward( use );
            if ( !finish.isValid() || time > finish )
                finish = time;
        }
        //kDebug()<<k_funcinfo<<m_name<<" finish="<<finish.toString()<<endl;
        return finish;
    } else {
        //TODO: subproject
    }
    return DateTime();
}

DateTime Project::calculateBackward( int use )
{
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if ( type() == Node::Type_Project ) {
        // Follow *child* relations back and
        // calculate backwards following parent relation
        DateTime start;
        DateTime time;
        QListIterator<Node*> startnodes = m_startNodes;
        while ( startnodes.hasNext() ) {
            time = startnodes.next() ->calculateBackward( use );
            if ( !start.isValid() || time < start )
                start = time;
        }
        //kDebug()<<k_funcinfo<<m_name<<" start="<<start.toString()<<endl;
        return start;
    } else {
        //TODO: subproject
    }
    return DateTime();
}

DateTime Project::scheduleForward( const DateTime &earliest, int use )
{
    resetVisited();
    DateTime end = earliest;
    DateTime time;
    QListIterator<Node*> it( m_endNodes );
    while ( it.hasNext() ) {
        time = it.next() ->scheduleForward( earliest, use );
        if ( time > end )
            end = time;
    }
    // Fix summarytasks
    adjustSummarytask();
    return end;
}

DateTime Project::scheduleBackward( const DateTime &latest, int use )
{
    resetVisited();
    DateTime start = latest;
    DateTime time;
    QListIterator<Node*> it( m_startNodes );
    while ( it.hasNext() ) {
        time = it.next() ->scheduleBackward( latest, use );
        if ( time < start )
            start = time;
    }
    // Fix summarytasks
    adjustSummarytask();
    return start;
}

void Project::adjustSummarytask()
{
    QListIterator<Node*> it( m_summarytasks );
    while ( it.hasNext() ) {
        it.next() ->adjustSummarytask();
    }
}

void Project::initiateCalculation( Schedule &sch )
{
    //kDebug()<<k_funcinfo<<m_name<<endl;
    // clear all resource appointments
    m_visitedForward = false;
    m_visitedBackward = false;
    QListIterator<ResourceGroup*> git( m_resourceGroups );
    while ( git.hasNext() ) {
        git.next() ->initiateCalculation( sch );
    }
    Node::initiateCalculation( sch );
    m_startNodes.clear();
    m_endNodes.clear();
    m_summarytasks.clear();
    initiateCalculationLists( m_startNodes, m_endNodes, m_summarytasks );
}

void Project::initiateCalculationLists( QList<Node*> &startnodes, QList<Node*> &endnodes, QList<Node*> &summarytasks )
{
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if ( type() == Node::Type_Project ) {
        QListIterator<Node*> it = childNodeIterator();
        while ( it.hasNext() ) {
            it.next() ->initiateCalculationLists( startnodes, endnodes, summarytasks );
        }
    } else {
        //TODO: subproject
    }
}

bool Project::load( QDomElement &element )
{
    //kDebug()<<k_funcinfo<<"--->"<<endl;
    QString s;
    bool ok = false;
    QString id = element.attribute( "id" );
    if ( !setId( id ) ) {
        kWarning() << k_funcinfo << "Id must be unique: " << id << endl;
    }
    m_name = element.attribute( "name" );
    m_leader = element.attribute( "leader" );
    m_description = element.attribute( "description" );

    // Allow for both numeric and text
    s = element.attribute( "scheduling", "0" );
    m_constraint = ( Node::ConstraintType ) s.toInt( &ok );
    if ( !ok )
        setConstraint( s );
    if ( m_constraint != Node::MustStartOn &&
            m_constraint != Node::MustFinishOn ) {
        kError() << k_funcinfo << "Illegal constraint: " << constraintToString() << endl;
        setConstraint( Node::MustStartOn );
    }
    s = element.attribute( "start-time" );
    if ( !s.isEmpty() )
        m_constraintStartTime = DateTime::fromString( s );
    s = element.attribute( "end-time" );
    if ( !s.isEmpty() )
        m_constraintEndTime = DateTime::fromString( s );

    // Load the project children
    // Must do these first
    QDomNodeList list = element.childNodes();
    for ( unsigned int i = 0; i < list.count(); ++i ) {
        if ( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
            if ( e.tagName() == "calendar" ) {
                // Load the calendar.
                // References by resources
                Calendar * child = new Calendar();
                child->setProject( this );
                if ( child->load( e ) ) {
                    addCalendar( child );
                } else {
                    // TODO: Complain about this
                    kError() << k_funcinfo << "Failed to load calendar" << endl;
                    delete child;
                }
            } else if ( e.tagName() == "standard-worktime" ) {
                // Load standard worktime
                StandardWorktime * child = new StandardWorktime();
                if ( child->load( e ) ) {
                    setStandardWorktime( child );
                } else {
                    kError() << k_funcinfo << "Failed to load standard worktime" << endl;
                    delete child;
                }
            }
        }
    }
    for ( unsigned int i = 0; i < list.count(); ++i ) {
        if ( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();

            if ( e.tagName() == "resource-group" ) {
                // Load the resources
                // References calendars
                ResourceGroup * child = new ResourceGroup( this );
                if ( child->load( e ) ) {
                    addResourceGroup( child );
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            }
        }
    }
    for ( unsigned int i = 0; i < list.count(); ++i ) {
        if ( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();

            if ( e.tagName() == "project" ) {
                //kDebug()<<k_funcinfo<<"Sub project--->"<<endl;
                // Load the subproject
                Project * child = new Project( this );
                if ( child->load( e ) ) {
                    if ( !addTask( child, this ) ) {
                        delete child; // TODO: Complain about this
                    }
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if ( e.tagName() == "task" ) {
                //kDebug()<<k_funcinfo<<"Task--->"<<endl;
                // Load the task (and resourcerequests).
                // Depends on resources already loaded
                Task * child = new Task( this );
                if ( child->load( e, *this ) ) {
                    if ( !addTask( child, this ) ) {
                        delete child; // TODO: Complain about this
                    }
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            }
        }
    }
    // These go last
    for ( unsigned int i = 0; i < list.count(); ++i ) {
        if ( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
            if ( e.tagName() == "accounts" ) {
                //kDebug()<<k_funcinfo<<"Accounts--->"<<endl;
                // Load accounts
                // References tasks
                if ( !m_accounts.load( e, *this ) ) {
                    kError() << k_funcinfo << "Failed to load accounts" << endl;
                }
            } else if ( e.tagName() == "relation" ) {
                //kDebug()<<k_funcinfo<<"Relation--->"<<endl;
                // Load the relation
                // References tasks
                Relation * child = new Relation();
                if ( !child->load( e, *this ) ) {
                    // TODO: Complain about this
                    kError() << k_funcinfo << "Failed to load relation" << endl;
                    delete child;
                }
                //kDebug()<<k_funcinfo<<"Relation<---"<<endl;
            } else if ( e.tagName() == "schedules" ) {
                //kDebug()<<k_funcinfo<<"Project schedules & task appointments--->"<<endl;
                // Prepare for multiple schedules
                // References tasks and resources
                QDomNodeList lst = e.childNodes();
                for ( unsigned int i = 0; i < lst.count(); ++i ) {
                    if ( lst.item( i ).isElement() ) {
                        QDomElement el = lst.item( i ).toElement();
                        if ( el.tagName() == "schedule" ) {
                            MainSchedule * sch = new MainSchedule();
                            if ( sch->loadXML( el, *this ) ) {
                                addSchedule( sch );
                                sch->setNode( this );
                                setParentSchedule( sch );
                                // If it's here, it's scheduled!
                                sch->setScheduled( true );
                            } else {
                                kError() << k_funcinfo << "Failed to load schedule" << endl;
                                delete sch;
                            }
                        }
                    }
                }
                //kDebug()<<k_funcinfo<<"Node schedules<---"<<endl;
            }
        }
    }
    //kDebug()<<k_funcinfo<<"Calendars--->"<<endl;
    // calendars references calendars in arbritary saved order
    QListIterator<Calendar*> calit( m_calendars );
    while ( calit.hasNext() ) {
        Calendar * c = calit.next();
        if ( c->id() == c->parentId() ) {
            kError() << k_funcinfo << "Calendar want itself as parent" << endl;
            continue;
        }
        c->setParent( calendar( c->parentId() ) );
    }
    //kDebug()<<k_funcinfo<<"Project schedules--->"<<endl;
    foreach ( Schedule * s, m_schedules ) {
        if ( m_constraint == Node::MustFinishOn )
            m_constraintEndTime = s->endTime;
        else
            m_constraintStartTime = s->startTime;
    }
    //kDebug()<<k_funcinfo<<"Project schedules<---"<<endl;
    //kDebug()<<k_funcinfo<<"<---"<<endl;
    return true;
}

void Project::save( QDomElement &element ) const
{
    QDomElement me = element.ownerDocument().createElement( "project" );
    element.appendChild( me );

    me.setAttribute( "name", m_name );
    me.setAttribute( "leader", m_leader );
    me.setAttribute( "id", m_id );
    me.setAttribute( "description", m_description );

    me.setAttribute( "scheduling", constraintToString() );
    me.setAttribute( "start-time", m_constraintStartTime.toString( Qt::ISODate ) );
    me.setAttribute( "end-time", m_constraintEndTime.toString( Qt::ISODate ) );

    m_accounts.save( me );

    // save calendars
    QListIterator<Calendar*> calit( m_calendars );
    while ( calit.hasNext() ) {
        calit.next() ->save( me );
    }
    // save standard worktime
    if ( m_standardWorktime )
        m_standardWorktime->save( me );

    // save project resources, must be after calendars
    QListIterator<ResourceGroup*> git( m_resourceGroups );
    while ( git.hasNext() ) {
        git.next() ->save( me );
    }

    // Only save parent relations
    QListIterator<Relation*> it( m_dependParentNodes );
    while ( it.hasNext() ) {
        it.next() ->save( me );
    }

    for ( int i = 0; i < numChildren(); i++ )
        // Save all children
        childNode( i ) ->save( me );

    // Now we can save relations assuming no tasks have relations outside the project
    QListIterator<Node*> nodes( m_nodes );
    while ( nodes.hasNext() ) {
        nodes.next() ->saveRelations( me );
    }

    if ( !m_schedules.isEmpty() ) {
        QDomElement el = me.ownerDocument().createElement( "schedules" );
        me.appendChild( el );
        QHash<long, Schedule*> hash = m_schedules;
        foreach ( Schedule * s, hash ) {
            if ( !s->isDeleted() && s->isScheduled() ) {
                QDomElement schs = el.ownerDocument().createElement( "schedule" );
                el.appendChild( schs );
                s->saveXML( schs );
                //kDebug()<<k_funcinfo<<m_name<<" id="<<s->id()<<(s->isDeleted()?"  Deleted":"")<<endl;
                Node::saveAppointments( schs, s->id() );
            }
        }
    }
}

void Project::setParentSchedule( Schedule *sch )
{
    QListIterator<Node*> it = m_nodes;
    while ( it.hasNext() ) {
        it.next() ->setParentSchedule( sch );
    }
}

void Project::addResourceGroup( ResourceGroup * group )
{
    m_resourceGroups.append( group );
    emit resourceGroupAdded( group );
}

ResourceGroup *Project::takeResourceGroup( ResourceGroup *resource )
{
    int i = m_resourceGroups.indexOf( resource );
    if ( i == -1 ) {
        return 0;
    }
    ResourceGroup *g = m_resourceGroups.takeAt( i );
    emit resourceGroupRemoved( g );
    return g;
}

QList<ResourceGroup*> &Project::resourceGroups()
{
    return m_resourceGroups;
}

bool Project::addTask( Node* task, Node* position )
{
    // we want to add a task at the given position. => the new node will
    // become next sibling right after position.
    if ( 0 == position ) {
        kError() << k_funcinfo << "position=0, could not add task: " << task->name() << endl;
        return false;
    }
    //kDebug()<<k_funcinfo<<"Add "<<task->name()<<" after "<<position->name()<<endl;
    // in case we want to add to the main project, we make it child element
    // of the root element.
    if ( Node::Type_Project == position->type() ) {
        return addSubTask( task, position );
    }
    // find the position
    // we have to tell the parent that we want to delete one of its children
    Node* parentNode = position->getParent();
    if ( !parentNode ) {
        kDebug() << k_funcinfo << "parent node not found???" << endl;
        return false;
    }
    int index = parentNode->findChildNode( position );
    if ( -1 == index ) {
        // ok, it does not exist
        kDebug() << k_funcinfo << "Task not found???" << endl;
        return false;
    }
    return addSubTask( task, index + 1, parentNode );
}

bool Project::addSubTask( Node* task, Node* position )
{
    // we want to add a subtask to the node "position". It will become
    // position's last child.
    if ( 0 == position ) {
        kError() << k_funcinfo << "No parent, can not add subtask: " << task->name() << endl;
        return false;
    }
    if ( !registerNodeId( task ) ) {
        kError() << k_funcinfo << "Failed to register node id, can not add subtask: " << task->name() << endl;
        return false;
    }
    emit nodeToBeAdded( position );
    position->addChildNode( task );
    emit nodeAdded( task );
    kDebug()<<k_funcinfo<<endl;
    return true;
}

bool Project::addSubTask( Node* task, int index, Node* parent )
{
    // we want to add a subtask to the node "parent" at the given index.
    if ( 0 == parent ) {
        kError() << k_funcinfo << "No parent, can not add subtask: " << task->name() << endl;
        return false;
    }
    if ( !registerNodeId( task ) ) {
        kError() << k_funcinfo << "Failed to register node id, can not add subtask: " << task->name() << endl;
        return false;
    }
    emit nodeToBeAdded( parent );
    parent->insertChildNode( index, task );
    emit nodeAdded( task );
    return true;
}

void Project::delTask( Node *node )
{
    Node * parent = node->getParent();
    if ( parent == 0 ) {
        kDebug() << k_funcinfo << "Node must have a parent!" << endl;
        return ;
    }
    removeId( node->id() );
    emit nodeToBeRemoved( parent );
    parent->takeChildNode( node );
    emit nodeRemoved( node );
}


bool Project::canIndentTask( Node* node )
{
    if ( 0 == node ) {
        // should always be != 0. At least we would get the Project,
        // but you never know who might change that, so better be careful
        return false;
    }
    if ( node->type() == Node::Type_Project ) {
        //kDebug()<<k_funcinfo<<"The root node cannot be indented"<<endl;
        return false;
    }
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->getParent();
    if ( !parentNode ) {
        return false;
    }
    if ( parentNode->findChildNode( node ) == -1 ) {
        kError() << k_funcinfo << "Tasknot found???" << endl;
        return false;
    }
    Node *sib = node->siblingBefore();
    if ( !sib ) {
        //kDebug()<<k_funcinfo<<"new parent node not found"<<endl;
        return false;
    }
    if ( node->findParentRelation( sib ) || node->findChildRelation( sib ) ) {
        //kDebug()<<k_funcinfo<<"Cannot have relations to parent"<<endl;
        return false;
    }
    return true;
}

bool Project::indentTask( Node* node, int index )
{
    if ( canIndentTask( node ) ) {
        Node * newParent = node->siblingBefore();
        emit nodeToBeMoved( node );
        node->getParent() ->takeChildNode( node );
        newParent->insertChildNode( index, node );
        emit nodeMoved( node );
        kDebug()<<k_funcinfo<<endl;
        return true;
    }
    return false;
}

bool Project::canUnindentTask( Node* node )
{
    if ( 0 == node ) {
        // is always != 0. At least we would get the Project, but you
        // never know who might change that, so better be careful
        return false;
    }
    if ( Node::Type_Project == node->type() ) {
        //kDebug()<<k_funcinfo<<"The root node cannot be unindented"<<endl;
        return false;
    }
    // we have to find the parent of task to manipulate its list of children
    // and we need the parent's parent too
    Node* parentNode = node->getParent();
    if ( !parentNode ) {
        return false;
    }
    Node* grandParentNode = parentNode->getParent();
    if ( !grandParentNode ) {
        //kDebug()<<k_funcinfo<<"This node already is at the top level"<<endl;
        return false;
    }
    int index = parentNode->findChildNode( node );
    if ( -1 == index ) {
        kError() << k_funcinfo << "Tasknot found???" << endl;
        return false;
    }
    return true;
}

bool Project::unindentTask( Node* node )
{
    if ( canUnindentTask( node ) ) {
        Node * parentNode = node->getParent();
        emit nodeToBeMoved( node );
        Node *grandParentNode = parentNode->getParent();
        parentNode->takeChildNode( node );
        grandParentNode->addChildNode( node, parentNode );
        emit nodeMoved( node );
        kDebug()<<k_funcinfo<<endl;
        return true;
    }
    return false;
}

bool Project::canMoveTaskUp( Node* node )
{
    if ( node == 0 )
        return false; // safety
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->getParent();
    if ( !parentNode ) {
        //kDebug()<<k_funcinfo<<"No parent found"<<endl;
        return false;
    }
    if ( parentNode->findChildNode( node ) == -1 ) {
        kError() << k_funcinfo << "Tasknot found???" << endl;
        return false;
    }
    if ( node->siblingBefore() ) {
        return true;
    }
    return false;
}

bool Project::moveTaskUp( Node* node )
{
    if ( canMoveTaskUp( node ) ) {
        emit nodeToBeMoved( node );
        if ( node->getParent()->moveChildUp( node ) ) {
            emit nodeMoved( node );
            kDebug()<<k_funcinfo<<endl;
            return true;
        }
        emit nodeMoved( node ); // Not actually moved, but what to do ?
    }
    return false;
}

bool Project::canMoveTaskDown( Node* node )
{
    if ( node == 0 )
        return false; // safety
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->getParent();
    if ( !parentNode ) {
        return false;
    }
    if ( parentNode->findChildNode( node ) == -1 ) {
        kError() << k_funcinfo << "Tasknot found???" << endl;
        return false;
    }
    if ( node->siblingAfter() ) {
        return true;
    }
    return false;
}

bool Project::moveTaskDown( Node* node )
{
    if ( canMoveTaskDown( node ) ) {
        emit nodeToBeMoved( node );
        if ( node->getParent() ->moveChildDown( node ) ) {
            emit nodeMoved( node );
            return true;
        }
        emit nodeMoved( node ); // Not actually moved, but what to do ?
    }
    return false;
}

Task *Project::createTask( Node* parent )
{
    Task * node = new Task( parent );
    node->setId( uniqueNodeId() );
    return node;
}

Task *Project::createTask( Task &def, Node* parent )
{
    Task * node = new Task( def, parent );
    node->setId( uniqueNodeId() );
    return node;
}

QString Project::uniqueNodeId( int seed )
{
    int i = seed;
    while ( findNode( QString( "%1" ).arg( i ) ) ) {
        ++i;
    }
    return QString( "%1" ).arg( i );
}

bool Project::removeId( const QString &id )
{
    kDebug() << k_funcinfo << "id=" << id << endl;
    return ( m_parent ? m_parent->removeId( id ) : nodeIdDict.remove( id ) );
}

void Project::insertId( const QString &id, Node *node )
{
    kDebug() << k_funcinfo << "id=" << id << " " << node->name() << endl;
    if ( m_parent == 0 )
        return ( void ) nodeIdDict.insert( id, node );
    m_parent->insertId( id, node );
}

bool Project::registerNodeId( Node *node )
{
    if ( node->id().isEmpty() ) {
        kError() << k_funcinfo << "Id is empty." << endl;
        return false;
    }
    Node *rn = findNode( node->id() );
    if ( rn == 0 ) {
        insertId( node->id(), node );
        return true;
    }
    if ( rn != node ) {
        kError() << k_funcinfo << "Id already exists for different task: " << node->id() << endl;
        return false;
    }
    return true;
}


ResourceGroup *Project::group( QString id )
{
    return findResourceGroup( id );
}

Resource *Project::resource( QString id )
{
    return findResource( id );
}

// TODO
EffortCostMap Project::plannedEffortCostPrDay( const QDate & /*start*/, const QDate & /*end*/ ) const
{
    //kDebug()<<k_funcinfo<<endl;
    EffortCostMap ec;
    return ec;

}

// Returns the total planned effort for this project (or subproject)
Duration Project::plannedEffort()
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort();
    }
    return eff;
}

// Returns the total planned effort for this project (or subproject) on date
Duration Project::plannedEffort( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( date );
    }
    return eff;
}

// Returns the total planned effort for this project (or subproject) upto and including date
Duration Project::plannedEffortTo( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffortTo( date );
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject)
Duration Project::actualEffort()
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffort();
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) on date
Duration Project::actualEffort( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffort( date );
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) upto and including date
Duration Project::actualEffortTo( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffortTo( date );
    }
    return eff;
}

double Project::plannedCost()
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost();
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) on date
double Project::plannedCost( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost( date );
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) upto and including date
double Project::plannedCostTo( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCostTo( date );
    }
    return c;
}

double Project::actualCost()
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->actualCost();
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) on date
double Project::actualCost( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->actualCost( date );
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) upto and including date
double Project::actualCostTo( const QDate &date )
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->actualCostTo( date );
    }
    return c;
}

void Project::addCalendar( Calendar *calendar )
{
    //kDebug()<<k_funcinfo<<calendar->name()<<endl;
    m_calendars.append( calendar );
}

Calendar *Project::calendar( const QString id ) const
{
    return findCalendar( id );
}

QList<Calendar*> Project::calendars()
{
    QList<Calendar*> list;
    QListIterator<Calendar*> it = m_calendars;
    while ( it.hasNext() ) {
        Calendar * c = it.next();
        if ( !c->isDeleted() ) {
            list.append( c );
        }
    }
    return list;
}

void Project::setStandardWorktime( StandardWorktime * worktime )
{
    if ( m_standardWorktime != worktime ) {
        delete m_standardWorktime;
        m_standardWorktime = worktime;
    }
}

bool Project::legalToLink( Node *par, Node *child )
{
    //kDebug()<<k_funcinfo<<par.name()<<" ("<<par.numDependParentNodes()<<" parents) "<<child.name()<<" ("<<child.numDependChildNodes()<<" children)"<<endl;

    if ( !child || par->isDependChildOf( child ) ) {
        return false;
    }
    bool legal = true;
    // see if par/child is related
    if ( par->isParentOf( child ) || child->isParentOf( par ) ) {
        legal = false;
    }
    if ( legal )
        legal = legalChildren( par, child );
    if ( legal )
        legal = legalParents( par, child );

    return legal;
}

bool Project::legalParents( Node *par, Node *child )
{
    bool legal = true;
    //kDebug()<<k_funcinfo<<par->name()<<" ("<<par->numDependParentNodes()<<" parents) "<<child->name()<<" ("<<child->numDependChildNodes()<<" children)"<<endl;
    for ( int i = 0; i < par->numDependParentNodes() && legal; ++i ) {
        Node *pNode = par->getDependParentNode( i ) ->parent();
        if ( child->isParentOf( pNode ) || pNode->isParentOf( child ) ) {
            //kDebug()<<k_funcinfo<<"Found: "<<pNode->name()<<" is related to "<<child->name()<<endl;
            legal = false;
        } else {
            legal = legalChildren( pNode, child );
        }
        if ( legal )
            legal = legalParents( pNode, child );
    }
    return legal;
}

bool Project::legalChildren( Node *par, Node *child )
{
    bool legal = true;
    //kDebug()<<k_funcinfo<<par->name()<<" ("<<par->numDependParentNodes()<<" parents) "<<child->name()<<" ("<<child->numDependChildNodes()<<" children)"<<endl;
    for ( int j = 0; j < child->numDependChildNodes() && legal; ++j ) {
        Node *cNode = child->getDependChildNode( j ) ->child();
        if ( par->isParentOf( cNode ) || cNode->isParentOf( par ) ) {
            //kDebug()<<k_funcinfo<<"Found: "<<par->name()<<" is related to "<<cNode->name()<<endl;
            legal = false;
        } else {
            legal = legalChildren( par, cNode );
        }
    }
    return legal;
}

void Project::generateWBS( int count, WBSDefinition &def, QString wbs )
{
    if ( type() == Type_Subproject || def.level0Enabled() ) {
        Node::generateWBS( count, def, wbs );
    } else {
        QListIterator<Node*> it = m_nodes;
        int i = 0;
        while ( it.hasNext() ) {
            it.next() ->generateWBS( ++i, def, m_wbs );
        }
    }
}

void Project::setCurrentSchedule( long id )
{
    setCurrentSchedulePtr( findSchedule( id ) );
    Node::setCurrentSchedule( id );
    QHash<QString, Resource*> hash = resourceIdDict;
    foreach ( Resource * r, hash ) {
        r->setCurrentSchedule( id );
    }
}

MainSchedule *Project::createSchedule( QString name, Schedule::Type type )
{
    //kDebug()<<k_funcinfo<<"No of schedules: "<<m_schedules.count()<<endl;
    long i = 1;
    while ( m_schedules.contains( i ) ) {
        ++i;
    }
    MainSchedule *sch = new MainSchedule( this, name, type, i );
    addSchedule( sch );
    return sch;
}

bool Project::removeCalendarId( const QString &id )
{
    kDebug() << k_funcinfo << "id=" << id << endl;
    return calendarIdDict.remove( id );
}

void Project::insertCalendarId( const QString &id, Calendar *calendar )
{
    kDebug() << k_funcinfo << "id=" << id << ": " << calendar->name() << endl;
    calendarIdDict.insert( id, calendar );
}

void Project::changed( Node *node )
{
    if ( m_parent == 0 ) {
        emit nodeChanged( node );
        return;
    }
    Node::changed( node );
}

void Project::changed( ResourceGroup *group )
{
    emit resourceGroupChanged( group );
}

void Project::sendResourceAdded( const ResourceGroup *group, const Resource *resource )
{
    emit resourceAdded( group, resource );
}

void Project::sendResourceToBeAdded( const ResourceGroup *group, const Resource *resource )
{
    emit resourceToBeAdded( group, resource );
}

void Project::sendResourceRemoved( const ResourceGroup *group, const Resource *resource )
{
    emit resourceRemoved( group, resource );
}
void Project::sendResourceToBeRemoved( const ResourceGroup *group, const Resource *resource )
{
    emit resourceToBeRemoved( group, resource );
}

void Project::changed( Resource *resource )
{
    emit resourceChanged( resource );
}

#ifndef NDEBUG
void Project::printDebug( bool children, QByteArray indent )
{

    kDebug() << indent << "+ Project node: " << Node::name() << endl; //FIXME: QT3 support
    indent += '!';
    QListIterator<ResourceGroup*> it( resourceGroups() );
    while ( it.hasNext() )
        it.next() ->printDebug( indent );

    Node::printDebug( children, indent );
}
void Project::printCalendarDebug( QByteArray indent )
{
    kDebug() << indent << "-------- Calendars debug printout --------" << endl;
    QListIterator
    <Calendar*> it = m_calendars;
    while ( it.hasNext() ) {
        it.next() ->printDebug( indent + "--" );
        kDebug() << endl;
    }
    if ( m_standardWorktime )
        m_standardWorktime->printDebug();
}
#endif

}  //KPlato namespace

#include "kptproject.moc"
