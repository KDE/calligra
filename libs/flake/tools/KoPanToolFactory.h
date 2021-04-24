/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPANTOOLFACTORY_H
#define KOPANTOOLFACTORY_H

#include "KoToolFactoryBase.h"

/// Factory for the KoPanTool
class KoPanToolFactory : public KoToolFactoryBase
{
public:
    /// constructor
    KoPanToolFactory();

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
#endif
