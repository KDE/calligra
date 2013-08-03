/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
    Copyright (c) 1998, 1999 Waldo Bastian <bastian@kde.org>

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

#ifndef KCOREAUTHORIZED_H
#define KCOREAUTHORIZED_H

#include <kconfigcore_export.h>

class QUrl;
class QString;
class QStringList;

/**
* Kiosk authorization framework
*
* Core functionality, see kauthorized.h for authorizeUrlAction.
*/
namespace KAuthorized
{
  /**
   * Returns whether a certain action is authorized
   * @param genericAction The name of a generic action
   * @return true if the action is authorized
   * @todo what are the generic actions?
   */
  KCONFIGCORE_EXPORT bool authorize(const QString& genericAction);

  /**
   * Returns whether a certain KAction is authorized.
   *
   * @param action The name of a KAction action. The name is prepended
   * with "action/" before being passed to authorize()
   * @return true if the KAction is authorized
   */
  KCONFIGCORE_EXPORT bool authorizeKAction(const QString& action);

  /**
   * Returns whether access to a certain control module is authorized.
   *
   * @param menuId identifying the control module, e.g. kde-mouse.desktop
   * @return true if access to the module is authorized, false otherwise.
   */
  KCONFIGCORE_EXPORT bool authorizeControlModule(const QString& menuId);

  /**
   * Returns which control modules from a given list are authorized for access.
   *
   * @param menuIds list of menu-ids of control modules;
   * an example of a menu-id is kde-mouse.desktop.
   * @return Those control modules for which access has been authorized.
   */
  KCONFIGCORE_EXPORT QStringList authorizeControlModules(const QStringList& menuIds);

}

#endif
