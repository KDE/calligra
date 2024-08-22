/* This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <friedrich@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TextDocumentInspectionDockerFactory.h"
#include "TextDocumentInspectionDocker.h"

TextDocumentInspectionDockerFactory::TextDocumentInspectionDockerFactory() = default;

QString TextDocumentInspectionDockerFactory::id() const
{
    return QLatin1String("TextDocumentInspectionDocker");
}

KoDockFactoryBase::DockPosition TextDocumentInspectionDockerFactory::defaultDockPosition() const
{
    return DockRight;
}

QDockWidget *TextDocumentInspectionDockerFactory::createDockWidget()
{
    TextDocumentInspectionDocker *widget = new TextDocumentInspectionDocker();
    widget->setObjectName(id());

    return widget;
}

bool TextDocumentInspectionDockerFactory::isCollapsable() const
{
    return false;
}
