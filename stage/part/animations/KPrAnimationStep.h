/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRANIMATIONSTEP_H
#define KPRANIMATIONSTEP_H

#include "KPrAnimationData.h"
#include "stage_export.h"
#include <QSequentialAnimationGroup>

class KoPASavingContext;

class STAGE_EXPORT KPrAnimationStep : public QSequentialAnimationGroup, KPrAnimationData
{
public:
    KPrAnimationStep();
    ~KPrAnimationStep() override;

    void init(KPrAnimationCache *animationCache, int step) override;

    virtual bool saveOdf(KoPASavingContext &paContext) const;
    virtual void deactivate();
};

#endif /* KPRANIMATIONSTEP_H */
