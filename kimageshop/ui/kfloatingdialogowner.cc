/*
 *  kfloatingdialogowner.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "kfloatingdialogowner.h"

KFloatingDialogOwner::KFloatingDialogOwner()
{
  m_fdList.setAutoDelete(false);
}

void KFloatingDialogOwner::addDialog(KFloatingDialog *d)
{
  m_fdList.append(d);
}

void KFloatingDialogOwner::removeDialog(KFloatingDialog *d)
{
  m_fdList.removeRef(d);
}

void KFloatingDialogOwner::alignRight(int width, int /*height*/)
{
  QListIterator<KFloatingDialog> it(m_fdList);
  for( ; it.current(); ++it )
    {
      KFloatingDialog *current = it.current();
      current->move(width- current->width(), current->y());
    }
}
