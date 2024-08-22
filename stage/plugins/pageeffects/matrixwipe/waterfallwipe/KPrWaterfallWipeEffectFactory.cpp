/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWaterfallWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrWaterfallWipeBottomLeftStrategy.h"
#include "KPrWaterfallWipeBottomRightStrategy.h"
#include "KPrWaterfallWipeTopLeftStrategy.h"
#include "KPrWaterfallWipeTopRightStrategy.h"

#define WaterfallWipeEffectId "WaterfallWipeEffect"

KPrWaterfallWipeEffectFactory::KPrWaterfallWipeEffectFactory()
    : KPrPageEffectFactory(WaterfallWipeEffectId, i18n("Waterfall"))
{
    addStrategy(new KPrWaterfallWipeTopLeftStrategy(KPrMatrixWipeStrategy::TopToBottom));
    addStrategy(new KPrWaterfallWipeTopLeftStrategy(KPrMatrixWipeStrategy::LeftToRight));
    addStrategy(new KPrWaterfallWipeTopRightStrategy(KPrMatrixWipeStrategy::TopToBottom));
    addStrategy(new KPrWaterfallWipeTopRightStrategy(KPrMatrixWipeStrategy::RightToLeft));
    addStrategy(new KPrWaterfallWipeBottomLeftStrategy(KPrMatrixWipeStrategy::BottomToTop));
    addStrategy(new KPrWaterfallWipeBottomLeftStrategy(KPrMatrixWipeStrategy::LeftToRight));
    addStrategy(new KPrWaterfallWipeBottomRightStrategy(KPrMatrixWipeStrategy::BottomToTop));
    addStrategy(new KPrWaterfallWipeBottomRightStrategy(KPrMatrixWipeStrategy::RightToLeft));
}

KPrWaterfallWipeEffectFactory::~KPrWaterfallWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Top Left Vertical"),
                                                  kli18n("Top Left Horizontal"),
                                                  kli18n("Top Right Vertical"),
                                                  kli18n("Top Right Horizontal"),
                                                  kli18n("Bottom Left Vertical"),
                                                  kli18n("Bottom Left Horizontal"),
                                                  kli18n("Bottom Right Vertical"),
                                                  kli18n("Bottom Right Horizontal")};

QString KPrWaterfallWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
