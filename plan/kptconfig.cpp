/* This file is part of the KDE project
   Copyright (C) 2004, 2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2011, 2012 Dag Andersen <danders@get2net.dk>

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

#include "plansettings.h"
#include "kptconfigskeleton.h"
#include "kptfactory.h"
#include "kptdebug.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kcomponentdata.h>

#include <QBrush>


namespace KPlato
{

Config::Config()
{
    kDebug(planDbg())<<"Leader:"<<KPlatoSettings::leader();
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
        case KPlatoSettings::EnumStartTimeUsage::CurrentdateTime:
            task.setConstraintStartTime( DateTime( QDateTime::currentDateTime() ) );
            break;
        case KPlatoSettings::EnumStartTimeUsage::CurrentDate:
            task.setConstraintStartTime( DateTime( QDate::currentDate(), KPlatoSettings::constraintStartTime().time() ) );
            break;
        case KPlatoSettings::EnumStartTimeUsage::SpecifiedDateTime: //fall through
        default:
            task.setConstraintStartTime( DateTime( KPlatoSettings::constraintStartTime() ) );
            break;
    }
    switch ( KPlatoSettings::endTimeUsage() ) {
        case KPlatoSettings::EnumEndTimeUsage::CurrentdateTime:
            task.setConstraintEndTime( DateTime( QDateTime::currentDateTime() ) );
            break;
        case KPlatoSettings::EnumEndTimeUsage::CurrentDate:
            task.setConstraintEndTime( DateTime( QDate::currentDate(), KPlatoSettings::constraintEndTime().time() ) );
            break;
        case KPlatoSettings::EnumEndTimeUsage::SpecifiedDateTime: //fall through
        default:
            task.setConstraintEndTime( DateTime( KPlatoSettings::constraintEndTime() ) );
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

KUrl Config::retrieveUrl() const
{
    return KPlatoSettings::retrieveUrl();
}


QBrush Config::summaryTaskDefaultColor() const
{
    QColor c = KPlatoSettings::summaryTaskDefaultColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

bool Config::summaryTaskLevelColorsEnabled() const
{
    return KPlatoSettings::summaryTaskLevelColorsEnabled();
}

QBrush Config::summaryTaskLevelColor_1() const
{
    QColor c = KPlatoSettings::summaryTaskLevelColor_1();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::summaryTaskLevelColor_2() const
{
    QColor c = KPlatoSettings::summaryTaskLevelColor_2();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::summaryTaskLevelColor_3() const
{
    QColor c = KPlatoSettings::summaryTaskLevelColor_3();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::summaryTaskLevelColor_4() const
{
    QColor c = KPlatoSettings::summaryTaskLevelColor_4();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::taskNormalColor() const
{
    QColor c = KPlatoSettings::taskNormalColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::taskErrorColor() const
{
    QColor c = KPlatoSettings::taskErrorColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::taskCriticalColor() const
{
    QColor c = KPlatoSettings::taskCriticalColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::taskFinishedColor() const
{
    QColor c = KPlatoSettings::taskFinishedColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::milestoneNormalColor() const
{
    QColor c = KPlatoSettings::milestoneNormalColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::milestoneErrorColor() const
{
    QColor c = KPlatoSettings::milestoneErrorColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::milestoneCriticalColor() const
{
    QColor c = KPlatoSettings::milestoneCriticalColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

QBrush Config::milestoneFinishedColor() const
{
    QColor c = KPlatoSettings::milestoneFinishedColor();
    if ( KPlatoSettings::colorGradientType() == KPlatoSettings::EnumColorGradientType::Linear ) {
        return gradientBrush( c );
    }
    return c;
}

}  //KPlato namespace

#include "kptconfig.moc"
