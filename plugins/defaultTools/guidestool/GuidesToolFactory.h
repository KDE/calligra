/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GUIDESTOOLFACTORY_H
#define GUIDESTOOLFACTORY_H

#include <KoToolFactoryBase.h>

// Warn; used in KoRuler.cpp too, keep in sync.
#define GuidesToolId "GuidesTool_ID"

/// The factory for the KoGuidesTool
class GuidesToolFactory : public KoToolFactoryBase
{
public:
    /// Constructor
    GuidesToolFactory();
    /// Destructor
    ~GuidesToolFactory() override;
    /// reimplemented
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // GUIDESTOOLFACTORY_H
