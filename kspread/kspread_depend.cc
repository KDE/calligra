/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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


#include "kspread_depend.h"

#include <qglobal.h>


KSpreadDependency::KSpreadDependency(int col, int row, KSpreadTable* table)
{
  Q_ASSERT(col > 0 && row > 0);

  m_left = m_right = col;
  m_top = m_bottom = row;

  m_table = table;
}

KSpreadDependency::KSpreadDependency(int left, int top, int right, int bottom, KSpreadTable* table)
{
  Q_ASSERT(left > 0 && top > 0 && right > 0 && bottom > 0);

  m_left = left;
  m_right = right;
  m_top = top;
  m_bottom = bottom;

  m_table = table;
}

int KSpreadDependency::Left()const
{
  return m_left;
}

int KSpreadDependency::Right()const
{
  return m_right;
}

int KSpreadDependency::Top()const
{
  return m_top;
}

int KSpreadDependency::Bottom()const
{
  return m_bottom;
}

KSpreadTable* KSpreadDependency::Table()const
{
  return m_table;
}
