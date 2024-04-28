// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KPRPLACEHOLDERTOOLFACTORY_H
#define KPRPLACEHOLDERTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class KPrPlaceholderToolFactory : public KoToolFactoryBase
{
public:
    KPrPlaceholderToolFactory();
    ~KPrPlaceholderToolFactory() override;

    /// reimplemented
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif /* KPRPLACEHOLDERTOOLFACTORY_H */
