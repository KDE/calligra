/* This file is part of the KDE project
  Copyright (C) 2007, 2012 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "kptglobal.h"

#include <klocale.h>

namespace KPlato
{

// namespace SchedulingState
// {
    QString SchedulingState::deleted( bool trans )
        { return trans ? i18n( "Deleted" ) : QString( "Deleted" ); }
    QString SchedulingState::notScheduled( bool trans )
        { return trans ? i18n( "Not scheduled" ) : QString( "Not scheduled" ); }
    QString SchedulingState::scheduled( bool trans )
        { return trans ? i18n( "Scheduled" ) : QString( "Scheduled" ); }
    QString SchedulingState::resourceOverbooked( bool trans )
        { return trans ? i18n( "Resource overbooked" ) : QString( "Resource overbooked" ); }
    QString SchedulingState::resourceNotAvailable( bool trans )
        { return trans ? i18n( "Resource not available" ) : QString( "Resource not available" ); }
    QString SchedulingState::resourceNotAllocated( bool trans )
        { return trans ? i18n( "No resource allocated" ) : QString( "No resource allocated" ); }
    QString SchedulingState::constraintsNotMet( bool trans )
        { return trans ? i18n( "Cannot fulfill constraints" ) : QString( "Cannot fulfill constraints" ); }
    QString SchedulingState::effortNotMet( bool trans )
        { return trans ? i18n( "Effort not met" ) : QString( "Effort not met" ); }
    QString SchedulingState::schedulingError( bool trans )
        { return trans ? i18n( "Scheduling error" ) : QString( "Scheduling error" ); }

//} namespace SchedulingState

} //namespace KPlato

