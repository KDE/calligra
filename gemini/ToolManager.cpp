/*
    Copyright (C) 2012  Dan Leinir Turthra Jensen <admin@leinir.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "ToolManager.h"
#include <KoToolRegistry.h>
#include <KoToolManager.h>
#include <KoToolBase.h>
#include <KoCanvasController.h>
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

ToolManager::ToolManager(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private)
{
    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*,int)),
            this, SLOT(slotToolChanged(KoCanvasController*,int)));
}

ToolManager::~ToolManager()
{
    delete d;
}

void ToolManager::requestToolChange(QString toolID)
{
    d->toolManager->switchToolRequested(toolID);
}

QObject* ToolManager::currentTool() const
{
    return d->currentTool;
}

void ToolManager::slotToolChanged(KoCanvasController* canvas, int toolId)
{
    Q_UNUSED(canvas);
    Q_UNUSED(toolId);

    if(!d->toolManager)
        return;

    QString id = KoToolManager::instance()->activeToolId();
    d->currentTool = qobject_cast<KoToolBase*>(KoToolManager::instance()->toolById(canvas->canvas(), id));
    emit currentToolChanged();
}
