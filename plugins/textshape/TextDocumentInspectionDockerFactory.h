/* This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <friedrich@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEXTDOCUMENTINSPECTIONDOCKERFACTORY_H
#define TEXTDOCUMENTINSPECTIONDOCKERFACTORY_H

#include <KoDockFactoryBase.h>

class TextDocumentInspectionDockerFactory : public KoDockFactoryBase
{
public:
    TextDocumentInspectionDockerFactory();

    QString id() const override;
    KoDockFactoryBase::DockPosition defaultDockPosition() const override;
    QDockWidget *createDockWidget() override;
    bool isCollapsable() const override;
};

#endif // TEXTDOCUMENTINSPECTIONDOCKERFACTORY_H
