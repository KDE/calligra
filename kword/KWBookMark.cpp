/* This file is part of the KDE project
   Copyright (C) 2002-2003 Laurent Montel <montel@kde.org>

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

#include <qstring.h> // TODO move to .h (no time to recompile right now :)
#include "KWBookMark.h"

KWBookMark::KWBookMark(const QString &_name)
    : m_name(_name),
      m_startParag(0L),
      m_endParag(0L),
      m_frameSet(0L),
      m_startIndex( 0 ),
      m_endIndex( 0)
{
}

KWBookMark::KWBookMark(const QString &_name, KoTextParag *_startParag, KoTextParag *_endParag,KWFrameSet *_frameSet, int _pos, int _end)
    : m_name(_name),
      m_startParag(_startParag),
      m_endParag(_endParag),
      m_frameSet(_frameSet),
      m_startIndex( _pos ),
      m_endIndex( _end )
{
}

KWBookMark::~KWBookMark()
{
    m_startParag=0L;
    m_endParag=0L;
    m_frameSet=0L;
}

