/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrSnakeWipeBottomLeftDiagonalStrategy.h"
#include "KPrSnakeWipeBottomRightDiagonalStrategy.h"
#include "KPrSnakeWipeBottomRightHorizontalStrategy.h"
#include "KPrSnakeWipeBottomRightVerticalStrategy.h"
#include "KPrSnakeWipeTopLeftDiagonalStrategy.h"
#include "KPrSnakeWipeTopLeftHorizontalStrategy.h"
#include "KPrSnakeWipeTopLeftVerticalStrategy.h"
#include "KPrSnakeWipeTopRightDiagonalStrategy.h"

#define SnakeWipeEffectId "SnakeWipeEffect"

KPrSnakeWipeEffectFactory::KPrSnakeWipeEffectFactory()
    : KPrPageEffectFactory(SnakeWipeEffectId, i18n("Snake"))
{
    addStrategy(new KPrSnakeWipeTopLeftHorizontalStrategy());
    addStrategy(new KPrSnakeWipeBottomRightHorizontalStrategy());
    addStrategy(new KPrSnakeWipeTopLeftVerticalStrategy());
    addStrategy(new KPrSnakeWipeBottomRightVerticalStrategy());
    addStrategy(new KPrSnakeWipeTopLeftDiagonalStrategy());
    addStrategy(new KPrSnakeWipeTopRightDiagonalStrategy());
    addStrategy(new KPrSnakeWipeBottomRightDiagonalStrategy());
    addStrategy(new KPrSnakeWipeBottomLeftDiagonalStrategy());
}

KPrSnakeWipeEffectFactory::~KPrSnakeWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Left"),
                                                  kli18n("From Right"),
                                                  kli18n("From Top"),
                                                  kli18n("From Bottom"),
                                                  kli18n("From Top Left"),
                                                  kli18n("From Top Right"),
                                                  kli18n("From Bottom Left"),
                                                  kli18n("From Bottom Right")};

QString KPrSnakeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
