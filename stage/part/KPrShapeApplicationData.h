/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSHAPEAPPLICATIONDATA_H
#define KPRSHAPEAPPLICATIONDATA_H

#include <KoShapeApplicationData.h>

#include "stage_export.h"
#include <QSet>

class KPrShapeAnimation;

class STAGE_EXPORT KPrShapeApplicationData : public KoShapeApplicationData
{
public:
    KPrShapeApplicationData();
    ~KPrShapeApplicationData() override;

    QSet<KPrShapeAnimation *> &animations();

    void setDeleteAnimations(bool enabled);

private:
    // stores the animations of a shape
    QSet<KPrShapeAnimation *> m_animations;
    bool m_deleteAnimations;
};

#endif
