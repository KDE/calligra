/* $Id$
   This file is part of KOffice
    Copyright (C) 1998 Kalle Dalheimer <kalle@kde.org>
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef __koffice_filter_manager_h__
#define __koffice_filter_manager_h__

#include <qobject.h>
#include <qstring.h>

/**
 * This class manages all filters for a KOffice application. It handles filter 
 * registration and invoking.
 * This class follows the singleton pattern. You don't create objects of this
 * class directly, but rather use the static method instance().
 * @author Kalle Dalheimer <kalle@kde.org>
 * @author Torben Weis <weis@kde.org>
 * @version $Id$
 */
class KoFilterManager : public QObject
{
public:
  enum Direction { Import, Export };

  /**
   * Returns a string list that is suitable for passing to
   * {Q,K}FileDialog::setFilters().
   *
   * @param direction Whether the dialog is for opening or for
   * saving. Is either KoFilterManager::Import or
   * KoFilterManager::Export.
   * @param allfiles Whether a wildcard should be added to the list.
   */
  QString fileSelectorList( Direction direction, const char *_format, bool allfiles ) const;

  /** 
   * Returns a pointer to the only instance of the KoFilterManager.
   * @return The pointer to the KoFilterManager instance
   */
  static KoFilterManager* self();
 
protected:
  KoFilterManager();

private:
  static KoFilterManager* s_pSelf;
};
	 

#endif
