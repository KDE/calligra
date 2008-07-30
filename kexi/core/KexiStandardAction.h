/* This file is part of the KDE project
   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISTANDARDACTION_H
#define KEXISTANDARDACTION_H

#include <kexi_export.h>
#include <QObject>
#include <QString>

class KAction;

//! @short Convenience methods to access all standard Kexi actions
/*! These actions should be used instead of hardcoding actions. 

 Note that calling any of these methods automatically adds the action
 to the action collection if the 'parent' parameter is of type KActionCollection.
*/
namespace KexiStandardAction
{
  /**
  * The standard menubar and toolbar actions.
  */
  enum StandardAction {
    ActionNone,

    // Data
    SortAscending, SortDescending
  };

  //! Creates an action corresponding to the KexiStandardAction::StandardAction enum.
  KEXICORE_EXPORT KAction* create(
    StandardAction id, const QObject *recvr, const char *slot, QObject *parent);

  //! @return the internal name of a given standard action
  KEXICORE_EXPORT const char* name( StandardAction id );

  //! Sorts data in ascending order
  KEXICORE_EXPORT KAction *sortAscending(const QObject *recvr, const char *slot, QObject *parent);

  //! Sorts data in descending order
  KEXICORE_EXPORT KAction *sortDescending(const QObject *recvr, const char *slot, QObject *parent);
}

#endif
