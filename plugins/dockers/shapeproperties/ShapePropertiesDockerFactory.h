/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHAPEPROPERTIESDOCKERFACTORY_H
#define SHAPEPROPERTIESDOCKERFACTORY_H

#include <KoDockFactoryBase.h>
#include <QDockWidget>

/// the factory which creates the shape properties docker
class ShapePropertiesDockerFactory : public KoDockFactoryBase
{
public:
    ShapePropertiesDockerFactory();

    QString id() const override;
    QDockWidget* createDockWidget() override;
    DockPosition defaultDockPosition() const override;

};

#endif // SHAPEPROPERTIESDOCKERFACTORY_H
