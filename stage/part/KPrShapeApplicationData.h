/* This file is part of the KDE project
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include <QSet>
#include "stage_export.h"

class KPrShapeAnimation;

class STAGE_EXPORT KPrShapeApplicationData : public KoShapeApplicationData
{
public:
    KPrShapeApplicationData();
    ~KPrShapeApplicationData() override;

    QSet<KPrShapeAnimation *> & animations();

    void setDeleteAnimations(bool enabled);

private:
    // stores the animations of a shape
    QSet<KPrShapeAnimation *> m_animations;
    bool m_deleteAnimations;
};


#endif

