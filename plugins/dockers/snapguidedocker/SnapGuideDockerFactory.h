/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SNAPGUIDEDOCKERFACTORY_H
#define SNAPGUIDEDOCKERFACTORY_H

#include <KoDockFactoryBase.h>
#include <QDockWidget>

/// the factory which creates the stroke docker
class SnapGuideDockerFactory : public KoDockFactoryBase
{
public:
    SnapGuideDockerFactory();

    QString id() const override;
    QDockWidget *createDockWidget() override;
    KoDockFactoryBase::DockPosition defaultDockPosition() const override;
};

#endif // SNAPGUIDEDOCKERFACTORY_H
