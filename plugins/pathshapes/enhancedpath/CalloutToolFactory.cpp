/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CalloutToolFactory.h"
#include "CalloutShape.h"
#include "CalloutDebug.h"

#include <KoIcon.h>
#include <KoShapeManager.h>
#include <KoCanvasBase.h>
#include <KoSnapGuide.h>

#include <klocalizedstring.h>

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

CalloutToolFactory::~CalloutToolFactory()
{
}

KoToolBase *CalloutToolFactory::createTool(KoCanvasBase *canvas)
{
    return new CalloutPathTool(canvas);
}


CalloutPathTool::CalloutPathTool(KoCanvasBase *canvas)
    : KoPathTool(canvas)
{
    QAction *a = action("convert-to-path");
    qInfo()<<Q_FUNC_INFO<<a;
    if (a) {
        a->disconnect();
    }
}

QList<QPointer<QWidget> >  CalloutPathTool::createOptionWidgets()
{
    QList<QPointer<QWidget> > list;
    return list;
}


void CalloutPathTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    // retrieve the actual global handle radius
    m_handleRadius = handleRadius();
    canvas()->snapGuide()->reset();
    
    repaintDecorations();
    QList<KoPathShape*> selectedShapes;
    foreach(KoShape *shape, shapes) {
        PathShape *pathShape = dynamic_cast<PathShape*>(shape);
        debugCalloutF<<shape->shapeId();
        if (shape->isSelectable() && pathShape) {
            // as the tool is just in activation repaintDecorations does not yet get called
            // so we need to use repaint of the tool and it is only needed to repaint the
            // current canvas
            repaint(pathShape->boundingRect());
            selectedShapes.append(pathShape);
        }
    }
    if (selectedShapes.isEmpty()) {
        emit done();
        return;
    }
    m_pointSelection.setSelectedShapes(selectedShapes);
    useCursor(m_selectCursor);
//     connect(canvas()->shapeManager()->selection(), SIGNAL(selectionChanged()), this, SLOT(activate()));
    updateOptionsWidget();
    updateActions();
}
