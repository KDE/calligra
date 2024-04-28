/*
    SPDX-FileCopyrightText: 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ToolManager.h"
#include <KoCanvasController.h>
#include <KoToolBase.h>
#include <KoToolManager.h>
#include <KoToolRegistry.h>
#include <QPointer>

class ToolManager::Private
{
public:
    Private()
    {
        toolManager = KoToolManager::instance();
    };

    QPointer<KoToolManager> toolManager;
    QPointer<KoToolBase> currentTool;
};

ToolManager::ToolManager(QQuickItem *parent)
    : QQuickItem(parent)
    , d(new Private)
{
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this, &ToolManager::slotToolChanged);
}

ToolManager::~ToolManager()
{
    delete d;
}

void ToolManager::requestToolChange(QString toolID)
{
    d->toolManager->switchToolRequested(toolID);
}

QObject *ToolManager::currentTool() const
{
    return d->currentTool;
}

void ToolManager::slotToolChanged(KoCanvasController *canvas, int toolId)
{
    Q_UNUSED(canvas);
    Q_UNUSED(toolId);

    if (!d->toolManager)
        return;

    QString id = KoToolManager::instance()->activeToolId();
    d->currentTool = qobject_cast<KoToolBase *>(KoToolManager::instance()->toolById(canvas->canvas(), id));
    emit currentToolChanged();
}
