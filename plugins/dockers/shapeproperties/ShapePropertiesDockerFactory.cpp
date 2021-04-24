/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ShapePropertiesDockerFactory.h"
#include "ShapePropertiesDocker.h"

ShapePropertiesDockerFactory::ShapePropertiesDockerFactory()
{
}

QString ShapePropertiesDockerFactory::id() const
{
    return QString("Shape Properties");
}

QDockWidget* ShapePropertiesDockerFactory::createDockWidget()
{
    ShapePropertiesDocker* widget = new ShapePropertiesDocker();
    widget->setObjectName(id());

    return widget;
}

KoDockFactoryBase::DockPosition ShapePropertiesDockerFactory::defaultDockPosition() const
{
    return DockMinimized;
}
