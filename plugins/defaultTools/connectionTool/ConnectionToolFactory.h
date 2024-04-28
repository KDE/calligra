/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CONNECTIONTOOLFACTORY_H
#define CONNECTIONTOOLFACTORY_H

#include "KoToolFactoryBase.h"

/// The factory for the ConnectionTool
class ConnectionToolFactory : public KoToolFactoryBase
{
public:
    /// Constructor
    ConnectionToolFactory();
    /// Destructor
    ~ConnectionToolFactory() override;
    /// reimplemented
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
