/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWRdfDockerFactory.h"
#include "KWRdfDocker.h"

KWRdfDockerFactory::KWRdfDockerFactory()
{
}

QString KWRdfDockerFactory::id() const
{
    return QString("Rdf");
}

QDockWidget *KWRdfDockerFactory::createDockWidget()
{
    KWRdfDocker *widget = new KWRdfDocker();
    widget->setObjectName(id());

    return widget;
}
