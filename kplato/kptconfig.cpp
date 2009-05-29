/* This file is part of the KDE project
   Copyright (C) 2004, 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptconfig.h"

#include "kplatosettings.h"
#include "kptconfigskeleton.h"
#include "kptfactory.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kcomponentdata.h>

namespace KPlato
{

Config::Config()
{
    kDebug()<<"Leader:"<<KPlatoSettings::leader();
}

Config::~Config()
{
    //save();
}

void Config::readConfig()
{
}
void Config::saveSettings()
{
//    KPlatoSettings::setResponsible( m_responsible );
    KPlatoSettings::self()->writeConfig();
}

void Config::setDefaultValues( Task &task )
{
    task.setLeader( KPlatoSettings::leader() );
    task.setDescription( KPlatoSettings::description() );
    task.setConstraint( (Node::ConstraintType) KPlatoSettings::constraintType() );
    
    // avoid problems with start <= end & end >= start
    task.setConstraintStartTime( DateTime() );
    task.setConstraintEndTime( DateTime() );
    switch ( KPlatoSettings::startTimeUsage() ) {
        case 0:
            task.setConstraintStartTime( DateTime( QDateTime::currentDateTime(), KDateTime::Spec::LocalZone() ) );
            break;
        case 1:
            task.setConstraintStartTime( DateTime( QDate::currentDate(), KPlatoSettings::constraintStartTime().time(), KDateTime::Spec::LocalZone() ) );
            break;
        default:
            task.setConstraintStartTime( DateTime( KPlatoSettings::constraintStartTime(), KDateTime::Spec::LocalZone() ) );
            break;
    }
    switch ( KPlatoSettings::endTimeUsage() ) {
        case 0:
            task.setConstraintEndTime( DateTime( QDateTime::currentDateTime(), KDateTime::Spec::LocalZone() ) );
            break;
        case 1:
            task.setConstraintEndTime( DateTime( QDate::currentDate(), KPlatoSettings::constraintEndTime().time(), KDateTime::Spec::LocalZone() ) );
            break;
        default:
            task.setConstraintEndTime( DateTime( KPlatoSettings::constraintEndTime(), KDateTime::Spec::LocalZone() ) );
            break;
    }

    task.estimate()->setType( (Estimate::Type) KPlatoSettings::estimateType() );
    task.estimate()->setUnit( (Duration::Unit) KPlatoSettings::unit() );
    task.estimate()->setExpectedEstimate( KPlatoSettings::expectedEstimate() );
    task.estimate()->setPessimisticRatio( KPlatoSettings::pessimisticRatio() );
    task.estimate()->setOptimisticRatio( KPlatoSettings::optimisticRatio() );
}

void Config::load() {
    kDebug()<<KGlobal::config()->groupList();
    KSharedConfigPtr config = Factory::global().config();
    kDebug()<<config->groupList();
    kDebug()<<(config->componentData() == KGlobal::config()->componentData());

/*    if( config->hasGroup("Behavior"))
    {
        config->setGroup("Behavior");
        m_behavior.calculationMode = config->readEntry("CalculationMode",m_behavior.calculationMode);
        m_behavior.allowOverbooking =  config->readEntry("AllowOverbooking",m_behavior.allowOverbooking);
    }*/
    if( config->hasGroup("Task defaults"))
    {
        //TODO: make this default stuff timezone neutral, use LocalZone for now
        KConfigGroup grp = config->group( "Task defaults" );
        taskDefaults().setLeader(grp.readEntry("Leader"));
        taskDefaults().setDescription(grp.readEntry("Description"));
        taskDefaults().setConstraint((Node::ConstraintType)grp.readEntry("ConstraintType",0));
        
        QDateTime dt = QDateTime::fromString(grp.readEntry("ConstraintStartTime", QString()), Qt::ISODate);
        taskDefaults().setConstraintStartTime( DateTime( dt, KDateTime::Spec::LocalZone() ) );
        kDebug()<<"ConstraintStartTime"<<grp.readEntry("ConstraintStartTime")<<taskDefaults().constraintStartTime().toString();
        
        dt = QDateTime::fromString(grp.readEntry("ConstraintEndTime", QString()), Qt::ISODate);
        taskDefaults().setConstraintEndTime( DateTime( dt, KDateTime::Spec::LocalZone() ) );
        
        taskDefaults().estimate()->setType((Estimate::Type)grp.readEntry("EstimateType",0));
        taskDefaults().estimate()->setUnit(Duration::unitFromString(grp.readEntry("Unit","h")));
        double value = grp.readEntry("ExpectedEstimate",1);
        taskDefaults().estimate()->setExpectedEstimate(grp.readEntry("ExpectedEstimate",1.0));
        taskDefaults().estimate()->setPessimisticRatio(grp.readEntry("PessimisticEstimate",0));
        taskDefaults().estimate()->setOptimisticRatio(grp.readEntry("OptimisticEstimate",0));
    }
}

void Config::save() {
    //kDebug()<<m_readWrite;
    if (!m_readWrite)
        return;

    KConfigGroup config = Factory::global().config()->group("Task defaults");

    config.writeEntry("Leader", taskDefaults().leader());
    config.writeEntry("Description", taskDefaults().description());
    config.writeEntry("ConstraintType", (int)taskDefaults().constraint());
    config.writeEntry("ConstraintStartTime", taskDefaults().constraintStartTime().dateTime().toString( Qt::ISODate));
    config.writeEntry("ConstraintEndTime", taskDefaults().constraintEndTime().dateTime().toString( Qt::ISODate));
    config.writeEntry("EstimateType", (int)taskDefaults().estimate()->type());
    config.writeEntry("Unit", Duration::unitToString(taskDefaults().estimate()->unit()));
    config.writeEntry("ExpectedEstimate", taskDefaults().estimate()->expectedEstimate());
    config.writeEntry("PessimisticEstimate", taskDefaults().estimate()->pessimisticRatio());
    config.writeEntry("OptimisticEstimate", taskDefaults().estimate()->optimisticRatio());

    KSharedConfigPtr c = Factory::global().config();
    kDebug()<<"Before sync:";
    kDebug()<<KGlobal::config()->groupList();
    kDebug()<<c->groupList();
    kDebug()<<KGlobal::config()->group("Task defaults").entryMap();
    kDebug()<<(c->group("Task defaults").entryMap());

    config.sync();

    kDebug()<<"After sync:";
    kDebug()<<KGlobal::config()->groupList();
    kDebug()<<c->groupList();
    kDebug()<<KGlobal::config()->group("Task defaults").entryMap();
    kDebug()<<(c->group("Task defaults").entryMap());
}

}  //KPlato namespace
