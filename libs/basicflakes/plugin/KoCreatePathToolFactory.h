/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "KoToolFactoryBase.h"

/// The factory for the KoCreatePathTool
class KoCreatePathToolFactory : public KoToolFactoryBase
{
public:
    /// Constructor
    KoCreatePathToolFactory();
    /// Destructor
    ~KoCreatePathToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
