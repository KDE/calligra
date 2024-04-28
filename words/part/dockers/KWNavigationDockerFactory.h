/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWNAVIGATIONDOCKERFACTORY_H
#define KWNAVIGATIONDOCKERFACTORY_H

#include <KoDockFactoryBase.h>

class KWNavigationDockerFactory : public KoDockFactoryBase
{
public:
    KWNavigationDockerFactory();
    QString id() const override;
    QDockWidget *createDockWidget() override;
    DockPosition defaultDockPosition() const override
    {
        return DockMinimized;
    }
};

#endif // KWNAVIGATIONDOCKERFACTORY_H
