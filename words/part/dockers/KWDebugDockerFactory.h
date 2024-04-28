/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWDEBUGDOCKERFACTORY_H
#define KWDEBUGDOCKERFACTORY_H

#include <KoDockFactoryBase.h>

class KWDebugDockerFactory : public KoDockFactoryBase
{
public:
    KWDebugDockerFactory();
    QString id() const override;
    QDockWidget *createDockWidget() override;
    DockPosition defaultDockPosition() const override
    {
        return DockMinimized;
    }
};

#endif // KWDEBUGDOCKERFACTORY_H
