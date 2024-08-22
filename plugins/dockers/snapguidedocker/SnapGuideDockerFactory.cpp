/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SnapGuideDockerFactory.h"
#include "SnapGuideDocker.h"

SnapGuideDockerFactory::SnapGuideDockerFactory() = default;

QString SnapGuideDockerFactory::id() const
{
    return QString("SnapGuide Properties");
}

QDockWidget *SnapGuideDockerFactory::createDockWidget()
{
    SnapGuideDocker *widget = new SnapGuideDocker();
    widget->setObjectName(id());

    return widget;
}

KoDockFactoryBase::DockPosition SnapGuideDockerFactory::defaultDockPosition() const
{
    return DockMinimized;
}
