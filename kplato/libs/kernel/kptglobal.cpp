/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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
    QString SchedulingState::deleted() { return i18n( "Deleted" ); }
    QString SchedulingState::notScheduled() { return i18n( "Not scheduled" ); }
    QString SchedulingState::scheduled() { return i18n( "Scheduled" ); }
    QString SchedulingState::resourceOverbooked() { return i18n( "Resource overbooked" ); }
    QString SchedulingState::resourceNotAvailable() { return i18n( "Resource not available" ); }
    QString SchedulingState::resourceNotAllocated() { return i18n( "No resource allocated" ); }
    QString SchedulingState::constraintsNotMet() { return i18n( "Cannot fulfill constraints" ); }

//} namespace SchedulingState

} //namespace KPlato

