/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STENCILBOXDOCKERFACTORY_H
#define STENCILBOXDOCKERFACTORY_H

#include "StencilBoxDocker.h"
#include <KoDockFactoryBase.h>
#include <QDockWidget>

class StencilBoxDockerFactory : public KoDockFactoryBase
{
public:
    QString id() const override
    {
        return QString("StencilBox");
    }
    QDockWidget *createDockWidget() override
    {
        return new StencilBoxDocker();
    }
    DockPosition defaultDockPosition() const override
    {
        return DockLeft;
    }
    bool defaultVisible() const override
    {
        return false;
    }
};

#endif // STENCILBOXDOCKERFACTORY_H
