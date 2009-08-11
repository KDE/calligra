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
}

void Config::readConfig()
{
}
void Config::saveSettings()
{
    if ( ! m_readWrite ) {
        return;
    }
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

int Config::minimumDurationUnit() const
{
    return KPlatoSettings::minimumDurationUnit();
}

int Config::maximumDurationUnit() const
{
    return KPlatoSettings::maximumDurationUnit();
}

bool Config::checkForWorkPackages() const
{
    return KPlatoSettings::checkForWorkPackages();
}

KUrl Config::retreiveUrl() const
{
    return KPlatoSettings::retreiveUrl();
}

}  //KPlato namespace
