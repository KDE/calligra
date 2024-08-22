// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrSoundEventActionFactory.h"

#include "KPrSoundEventAction.h"
#include "KPrSoundEventActionWidget.h"

KPrSoundEventActionFactory::KPrSoundEventActionFactory()
    : KoEventActionFactoryBase(KPrSoundEventActionId, "sound")
{
}

KPrSoundEventActionFactory::~KPrSoundEventActionFactory() = default;

KoEventAction *KPrSoundEventActionFactory::createEventAction()
{
    return new KPrSoundEventAction();
}

QWidget *KPrSoundEventActionFactory::createOptionWidget()
{
    return new KPrSoundEventActionWidget();
}
