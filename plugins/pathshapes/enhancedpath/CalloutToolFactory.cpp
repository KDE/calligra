/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CalloutToolFactory.h"
#include "CalloutDebug.h"
#include "CalloutShape.h"

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoShapeManager.h>
#include <KoSnapGuide.h>

#include <KLocalizedString>

#include <QAction>

CalloutToolFactory::CalloutToolFactory()
    : KoToolFactoryBase("CalloutPathToolFactoryId")
{
    setToolTip(i18n("Callout editing"));
    setToolType(dynamicToolType());
    setIconName(koIconName("editpath"));
    setPriority(10);
    setActivationShapeId(CalloutPathShapeId);
}

CalloutToolFactory::~CalloutToolFactory() = default;

KoToolBase *CalloutToolFactory::createTool(KoCanvasBase *canvas)
{
    return new CalloutPathTool(canvas);
}

CalloutPathTool::CalloutPathTool(KoCanvasBase *canvas)
    : KoPathTool(canvas)
{
    QAction *a = action("convert-to-path");
    if (a) {
        a->disconnect();
    }
}

QList<QPointer<QWidget>> CalloutPathTool::createOptionWidgets()
{
    QList<QPointer<QWidget>> list;
    return list;
}

void CalloutPathTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);
    // retrieve the actual global handle radius
    m_handleRadius = handleRadius();
    canvas()->snapGuide()->reset();

    repaintDecorations();
    QList<KoPathShape *> selectedShapes;
    foreach (KoShape *shape, shapes) {
        PathShape *pathShape = dynamic_cast<PathShape *>(shape);
        debugCalloutF << shape->shapeId();
        if (shape->isSelectable() && pathShape) {
            // as the tool is just in activation repaintDecorations does not yet get called
            // so we need to use repaint of the tool and it is only needed to repaint the
            // current canvas
            repaint(pathShape->boundingRect());
            selectedShapes.append(pathShape);
        }
    }
    if (selectedShapes.isEmpty()) {
        Q_EMIT done();
        return;
    }
    m_pointSelection.setSelectedShapes(selectedShapes);
    useCursor(m_selectCursor);
    //     connect(canvas()->shapeManager()->selection(), SIGNAL(selectionChanged()), this, SLOT(activate()));
    updateOptionsWidget();
    updateActions();
}
