/* This file is part of the KDE project
   Copyright (C) 2006 Laurent Montel <montel@kde.org>

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

#include <KPrShapeApplicationData.h>

KPrShapeApplicationData::KPrShapeApplicationData()
 : KoShapeApplicationData()
{
}

void KPrShapeApplicationData::setAppearSoundEffectFileName( const QString & _a_fileName )
{
  m_a_soundFileName = _a_fileName;
}

void KPrShapeApplicationData::setDisappearSoundEffectFileName( const QString &_d_fileName )
{
  m_d_soundFileName = _d_fileName;
}

QString KPrShapeApplicationData::appearSoundEffectFileName() const
{
  return m_a_soundFileName;
}

QString KPrShapeApplicationData::disappearSoundEffectFileName() const
{
  return m_d_soundFileName;
}


void KPrShapeApplicationData::setShapeName( const QString &_name)
{
    m_shapeName = _name;
}

QString KPrShapeApplicationData::shapeName() const
{
    return m_shapeName;
}
