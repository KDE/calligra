/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KPrShapeApplicationData.h>

#include "animations/KPrShapeAnimation.h"

KPrShapeApplicationData::KPrShapeApplicationData()
    : m_deleteAnimations(false)
{
}

KPrShapeApplicationData::~KPrShapeApplicationData()
{
    if (m_deleteAnimations) {
        qDeleteAll(m_animations);
    }
}

QSet<KPrShapeAnimation *> &KPrShapeApplicationData::animations()
{
    return m_animations;
}

void KPrShapeApplicationData::setDeleteAnimations(bool enabled)
{
    m_deleteAnimations = enabled;
}
