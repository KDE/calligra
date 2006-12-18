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

#ifndef KPRSHAPEAPPLICATIONDATA_H
#define KPRSHAPEAPPLICATIONDATA_H

#include <KoShapeApplicationData.h>

class KPrShapeUserData : public KoShapeApplicationData
{
public:
   KPrShapeUserData();
   
   void setAppearSoundEffectFileName( const QString & _a_fileName );

   void setDisappearSoundEffectFileName( const QString &_d_fileName );
   
   /**
    * Return sound name used when object appear.
    * @param return sound name used when object appear.
    */
   QString appearSoundEffectFileName() const;
   
   /**
    * Return sound name used when object disappear.
    * @param return sound name used when object disappear.
    */
   QString disappearSoundEffectFileName() const;


   void setShapeName( const QString &_name);
   
   /**
    * Return shape name
    * @param return shape name.
    */
   QString shapeName() const;

#if 0 //TODO
   void loadOasis(const QDomElement &element, KoOasisContext & context,  KPrLoadingInfo *info);
   virtual bool saveOasisObject( KPOasisSaveContext &sc ) const;
#endif

private:
   int m_appearTimer;
   int m_disappearTimer;
   QString m_a_soundFileName;
   QString m_d_soundFileName;
   QString m_shapeName;
};


#endif

