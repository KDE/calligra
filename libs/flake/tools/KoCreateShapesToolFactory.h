/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCREATESHAPESTOOLFACTORY_H
#define KOCREATESHAPESTOOLFACTORY_H

#include "KoToolFactoryBase.h"

/// The factory for the KoCreateShapesTool
class KoCreateShapesToolFactory : public KoToolFactoryBase
{
public:
    /// Constructor
    KoCreateShapesToolFactory();
    /// Destructor
    ~KoCreateShapesToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
#endif
