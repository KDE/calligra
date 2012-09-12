/* This file is part of the KDE project
   Copyright (C) 2009, 2010 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#include "kptappointment.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"

#include <kdebug.h>

#include <QStringList>
#include <QString>

namespace KPlato
{

class Debug
{
public:
    Debug() {}
static
void print( Calendar *c, const QString &str, bool full = true ) {
    Q_UNUSED(full);
    QString s;
    switch ( c->timeSpec().type() ) {
        case KDateTime::Invalid: s = "Invalid"; break;
        case KDateTime::UTC: s = "UTC"; break;
        case KDateTime::OffsetFromUTC: s = "OffsetFromUTC"; break;
        case KDateTime::TimeZone: s = "TimeZone: " + c->timeSpec().timeZone().name(); break;
        case KDateTime::LocalZone: s = "LocalZone"; break;
        case KDateTime::ClockTime: s = "Clocktime"; break;
    }

    qDebug()<<"Debug info: Calendar"<<c->name()<<s<<str;
    for ( int wd = 1; wd <= 7; ++wd ) {
        CalendarDay *d = c->weekday( wd );
        qDebug()<<"   "<<wd<<":"<<d->stateToString( d->state() );
        foreach ( TimeInterval *t,  d->timeIntervals() ) {
            qDebug()<<"      interval:"<<t->first<<t->second<<"("<<Duration( qint64(t->second) ).toString()<<")";
        }
    }
    foreach ( const CalendarDay *d, c->days() ) {
        qDebug()<<"   "<<d->date()<<":";
        foreach ( TimeInterval *t,  d->timeIntervals() ) {
            qDebug()<<"      interval:"<<t->first<<t->second;
        }
    }
}
static
QString printAvailable( Resource *r, const QString &lead = QString() ) {
    QStringList sl;
    sl<<lead<<"Available:"
        <<( r->availableFrom().isValid()
                ? r->availableFrom().toString()
                : ( r->project() ? ("("+r->project()->constraintStartTime().toString()+")" ) : QString() ) )
        <<( r->availableUntil().isValid()
                ? r->availableUntil().toString()
                : ( r->project() ? ("("+r->project()->constraintEndTime().toString()+")" ) : QString() ) )
        <<QString::number( r->units() )<<"%"
        <<"cost: normal"<<QString::number(r->normalRate() )<<" overtime"<<QString::number(r->overtimeRate() );
    return sl.join( " " );
}
static
void print( Resource *r, const QString &str, bool full = true ) {
    qDebug()<<"Debug info: Resource"<<r->name()<<str;
    qDebug()<<"Parent group:"<<( r->parentGroup()
                ? ( r->parentGroup()->name() + " Type: "+ r->parentGroup()->typeToString() )
                : QString( "None" ) );
    qDebug()<<"Available:"
        <<( r->availableFrom().isValid()
                ? r->availableFrom().toString()
                : ( r->project() ? ("("+r->project()->constraintStartTime().toString()+")" ) : QString() ) )
        <<( r->availableUntil().isValid()
                ? r->availableUntil().toString()
                : ( r->project() ? ("("+r->project()->constraintEndTime().toString()+")" ) : QString() ) )
        <<r->units()<<"%";
    qDebug()<<"Type:"<<r->typeToString();
    if ( r->type() == Resource::Type_Team ) {
        qDebug()<<"Team members:"<<r->teamMembers().count();
        foreach ( Resource *tm, r->teamMembers() ) {
            qDebug()<<"   "<<tm->name()<<"Available:"
                    <<( r->availableFrom().isValid()
                            ? r->availableFrom().toString()
                            : ( r->project() ? ("("+r->project()->constraintStartTime().toString()+")" ) : QString() ) )
                    <<( r->availableUntil().isValid()
                            ? r->availableUntil().toString()
                            : ( r->project() ? ("("+r->project()->constraintEndTime().toString()+")" ) : QString() ) )
                    <<r->units()<<"%";
        }
    } else {
        Calendar *cal = r->calendar( true );
        QString s;
        if ( cal ) {
            s = cal->name();
        } else {
            cal = r->calendar( false );
            if ( cal ) {
                s = cal->name() + " (Default)";
            } else {
                s = "No calendar";
            }
        }
        qDebug()<<"Calendar:"<<s;
        if ( cal ) {
            print( cal, "Resource calendar" );
        }
    }
    if ( ! full ) return;
    qDebug()<<"External appointments:"<<r->numExternalAppointments();
    foreach ( Appointment *a, r->externalAppointmentList() ) {
        qDebug()<<"   appointment:"<<a->startTime().toString()<<a->endTime().toString();
        foreach( const AppointmentInterval &i, a->intervals().map() ) {
            qDebug()<<"      "<<i.startTime().toString()<<i.endTime().toString()<<i.load();
        }
    }
}
static
void print( Project *p, const QString &str, bool all = false ) {
    qDebug()<<"Debug info: Project"<<p->name()<<str;
    qDebug()<<"project target start:"<<QTest::toString( p->constraintStartTime() );
    qDebug()<<"  project target end:"<<QTest::toString( p->constraintEndTime() );
    qDebug()<<"  project start time:"<<QTest::toString( p->startTime() );
    qDebug()<<"    project end time:"<<QTest::toString( p->endTime() );
    
    if ( ! all ) {
        return;
    }
    foreach ( Task *t, p->allTasks() ) {
        qDebug();
        print( t );
    }
}
static
void print( Project *p, Task *t, const QString &str, bool full = true ) {
    Q_UNUSED(full);
    print( p, str );
    print( t );
}
static
void print( Task *t, const QString &str, bool full = true ) {
    qDebug()<<"Debug info: Task"<<t->name()<<str;
    print( t, full );
}
static
void print( Task *t, bool full = true ) {
    Q_UNUSED(full);
    qDebug()<<"Task"<<t->name()<<t->typeToString()<<t->constraintToString();
    qDebug()<<"     earlyStart:"<<QTest::toString( t->earlyStart() );
    qDebug()<<"      lateStart:"<<QTest::toString( t->lateStart() );
    qDebug()<<"    earlyFinish:"<<QTest::toString( t->earlyFinish() );
    qDebug()<<"     lateFinish:"<<QTest::toString( t->lateFinish() );
    qDebug()<<"      startTime:"<<QTest::toString( t->startTime() );
    qDebug()<<"        endTime:"<<QTest::toString( t->endTime() );
    switch ( t->constraint() ) {
        case Node::MustStartOn:
        case Node::StartNotEarlier:
            qDebug()<<"startConstraint:"<<QTest::toString( t->constraintStartTime() );
            break;
        case Node::FixedInterval:
            qDebug()<<"startConstraint:"<<QTest::toString( t->constraintStartTime() );
        case Node::MustFinishOn:
        case Node::FinishNotLater:
            qDebug()<<"  endConstraint:"<<QTest::toString( t->constraintEndTime() );
            break;
        default: break;
    }
    foreach ( ResourceGroupRequest *gr, t->requests().requests() ) {
        qDebug()<<"Group request:"<<gr->group()->name()<<gr->units();
        foreach ( ResourceRequest *rr, gr->resourceRequests() ) {
            qDebug()<<printAvailable( rr->resource(), "   " + rr->resource()->name() );
        }
    }
    qDebug();
    qDebug()<<"Estimate   :"<<t->estimate()->expectedEstimate()<<Duration::unitToString(t->estimate()->unit())
			<<t->estimate()->typeToString()
            <<(t->estimate()->type() == Estimate::Type_Duration
                ? (t->estimate()->calendar()?t->estimate()->calendar()->name():"Fixed")
                : QString( "%1 h" ).arg( t->estimate()->expectedValue().toDouble( Duration::Unit_h ) ) );
    Schedule *s = t->currentSchedule();
    if ( s ) {
        foreach ( Appointment *a, s->appointments() ) {
            qDebug()<<"Resource:"<<a->resource()->resource()->name()<<"booked:"<<QTest::toString( a->startTime() )<<QTest::toString( a->endTime() )<<"effort:"<<a->effort( a->startTime(), a->endTime() ).toDouble( Duration::Unit_h )<<"h";
            if ( ! full ) { continue; }
            foreach( const AppointmentInterval &i, a->intervals().map() ) {
                qDebug()<<"  "<<QTest::toString( i.startTime() )<<QTest::toString( i.endTime() )<<i.load()<<"effort:"<<i.effort( i.startTime(), i.endTime() ).toDouble( Duration::Unit_h )<<"h";
            }
        }
    }
    if ( t->runningAccount() ) {
        qDebug()<<"Running account :"<<t->runningAccount()->name();
    }
    if ( t->startupAccount() ) {
        qDebug()<<"Startup account :"<<t->startupAccount()->name()<<" cost:"<<t->startupCost();
    }
    if ( t->shutdownAccount() ) {
        qDebug()<<"Shutdown account:"<<t->shutdownAccount()->name()<<" cost:"<<t->shutdownCost();
    }
}
static
void print( const Completion &c, const QString &name, const QString &s = QString() ) {
    qDebug()<<"Completion:"<<name<<s;
    qDebug()<<"  Entry mode:"<<c.entryModeToString();
    qDebug()<<"     Started:"<<c.isStarted()<<c.startTime();
    qDebug()<<"    Finished:"<<c.isFinished()<<c.finishTime();
    qDebug()<<"  Completion:"<<c.percentFinished()<<"%";
    
    if ( ! c.usedEffortMap().isEmpty() ) {
        qDebug()<<"     Used effort:";
        foreach ( const Resource *r, c.usedEffortMap().keys() ) {
            Completion::UsedEffort *ue = c.usedEffort( r );
            foreach ( const QDate &d, ue->actualEffortMap().keys() ) {
                qDebug()<<"         "<<r->name()<<":";
                qDebug()<<"             "<<d.toString( Qt::ISODate )<<":"<<c.actualEffort( d ).toString()<<c.actualCost( d );
            }
        }
    }
}
static
void print( const EffortCostMap &m, const QString &s = QString() ) {
    qDebug()<<"EffortCostMap"<<s;
    if ( m.days().isEmpty() ) {
        qDebug()<<"   Empty";
        return;
    }
    qDebug()<<m.startDate().toString(Qt::ISODate)<<m.endDate().toString(Qt::ISODate)
            <<" total="
            <<m.totalEffort().toString()
            <<m.totalCost()
            <<"("
            <<m.bcwpTotalEffort()
            <<m.bcwpTotalCost()
            <<")";

    if ( ! m.days().isEmpty() ) {
        foreach ( const QDate &d, m.days().keys() ) {
            qDebug()<<"   "<<d.toString(Qt::ISODate)<<":"<<m.hoursOnDate( d )<<"h"<<m.costOnDate( d )
                                                    <<"("<<m.bcwpEffortOnDate( d )<<"h"<<m.bcwpCostOnDate( d )<<")";
        }
    }
}
static
void printSchedulingLog( const ScheduleManager &sm, const QString &s )
{
    qDebug()<<"Scheduling log"<<s;
    qDebug()<<"Scheduling:"<<sm.name()<<(sm.recalculate()?QString("recalculate from %1").arg(sm.recalculateFrom().toString()):"");
    foreach ( const QString &s, sm.expected()->logMessages() ) {
        qDebug()<<s;
    }
}
static
void print( Account *a, long id = -1, const QString &s = QString() )
{
    qDebug()<<"Debug info Account:"<<a->name()<<s;
    qDebug()<<"Account:"<<a->name()<<(a->isDefaultAccount() ? "Default" : "");
    EffortCostMap ec = a->plannedCost( id );
    qDebug()<<"Planned cost:"<<ec.totalCost()<<"effort:"<<ec.totalEffort().toString();
    if ( ! a->isElement() ) {
        foreach ( Account *c, a->accountList() ) {
            print( c );
        }
        return;
    }
    qDebug()<<"Cost places:";
    foreach ( Account::CostPlace *cp, a->costPlaces() ) {
        qDebug()<<"     Node:"<<(cp->node() ? cp->node()->name() : "");
        qDebug()<<"  running:"<<cp->running();
        qDebug()<<"  startup:"<<cp->startup();
        qDebug()<<" shutdown:"<<cp->shutdown();
    }
}

static
void print( const AppointmentInterval &i, const QString &indent = QString() )
{
    QString s = indent + "Interval:";
    if ( ! i.isValid() ) {
        qDebug()<<s<<"Not valid";
    } else {
        qDebug()<<s<<i.startTime().toString()<<i.endTime().toString()<<i.load()<<"%";
    }
}

static
void print( const AppointmentIntervalList &lst, const QString &s = QString() )
{
    qDebug()<<"Interval list:"<<lst.map().count()<<s;
    foreach ( const AppointmentInterval &i, lst.map() ) {
        print( i, "  " );
    }
}

};

}
