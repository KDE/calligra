/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLOUTTOOLFACTORY_H
#define CALLOUTTOOLFACTORY_H

#include <KoPathTool.h>
#include <KoToolFactoryBase.h>

/// Factory for the KoPathTool
class CalloutToolFactory : public KoToolFactoryBase
{
public:
    CalloutToolFactory();
    ~CalloutToolFactory() override;

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

class CalloutPathTool : public KoPathTool
{
    Q_OBJECT
public:
    CalloutPathTool(KoCanvasBase *canvas);

    QList<QPointer<QWidget>> createOptionWidgets() override;

public Q_SLOTS:
    void activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
};

#endif
