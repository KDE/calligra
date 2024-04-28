// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KPRSOUNDEVENTACTIONFACTORY_H
#define KPRSOUNDEVENTACTIONFACTORY_H

#include <KoEventActionFactoryBase.h>

class KPrSoundEventActionFactory : public KoEventActionFactoryBase
{
public:
    KPrSoundEventActionFactory();
    virtual ~KPrSoundEventActionFactory();

    KoEventAction *createEventAction() override;
    QWidget *createOptionWidget() override;
};

#endif /* KPRSOUNDEVENTACTIONFACTORY_H */
