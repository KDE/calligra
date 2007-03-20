/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include "kptconfig.h"

#include "kptfactory.h"

#include <kconfig.h>
#include <kdebug.h>
#include <kcomponentdata.h>

namespace KPlato
{

Config::Config()
{
    m_readWrite = true;
}

Config::~Config()
{
}

void Config::load() {
    //kDebug()<<k_funcinfo<<endl;
    KSharedConfigPtr config = Factory::global().config();

/*    if( config->hasGroup("Behavior"))
    {
        config->setGroup("Behavior");
        m_behavior.calculationMode = config->readEntry("CalculationMode",m_behavior.calculationMode);
        m_behavior.allowOverbooking =  config->readEntry("AllowOverbooking",m_behavior.allowOverbooking);
    }*/
    if( config->hasGroup("Task defaults"))
    {
        //TODO: make this default stuff timezone neutral, use LocalZone for now
        config->setGroup("Task defaults");
        m_taskDefaults.setLeader(config->readEntry("Leader"));
        m_taskDefaults.setDescription(config->readEntry("Description"));
        m_taskDefaults.setConstraint((Node::ConstraintType)config->readEntry("ConstraintType",0));
        
        QDateTime dt = config->readEntry("ConstraintStartTime",QDateTime());
        m_taskDefaults.setConstraintStartTime( DateTime( dt, KDateTime::Spec::LocalZone() ) );
        
        dt = config->readEntry("ConstraintEndTime",QDateTime());
        m_taskDefaults.setConstraintEndTime( DateTime( dt, KDateTime::Spec::LocalZone() ) );
        
        m_taskDefaults.effort()->setType((Effort::Type)config->readEntry("EffortType",0));
        m_taskDefaults.effort()->set(Duration((qint64)(config->readEntry("ExpectedEffort",0))*1000)); //FIXME
        m_taskDefaults.effort()->setPessimisticRatio(config->readEntry("PessimisticEffort",0));
        m_taskDefaults.effort()->setOptimisticRatio(config->readEntry("OptimisticEffort",0));
    }
}

void Config::save() {
    //kDebug()<<k_funcinfo<<m_readWrite<<endl;
    if (!m_readWrite)
        return;

    KConfigGroup config = Factory::global().config()->group("Task defaults");

//     config->setGroup( "Behavior" );
//     config->writeEntry("CalculationMode",m_behavior.calculationMode);
//     config->writeEntry("AllowOverbooking",m_behavior.allowOverbooking);

    config.writeEntry("Leader", m_taskDefaults.leader());
    config.writeEntry("Description", m_taskDefaults.description());
    config.writeEntry("ConstraintType", (int)m_taskDefaults.constraint());
    config.writeEntry("ConstraintStartTime", m_taskDefaults.constraintStartTime().dateTime());
    config.writeEntry("ConstraintEndTime", m_taskDefaults.constraintEndTime().dateTime());
    config.writeEntry("EffortType", (int)m_taskDefaults.effort()->type());
    config.writeEntry("ExpectedEffort", m_taskDefaults.effort()->expected().seconds()); //FIXME
    config.writeEntry("PessimisticEffort", m_taskDefaults.effort()->pessimisticRatio());
    config.writeEntry("OptimisticEffort", m_taskDefaults.effort()->optimisticRatio());
}

}  //KPlato namespace
