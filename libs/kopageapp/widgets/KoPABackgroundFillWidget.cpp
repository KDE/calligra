/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * Copyright (C) 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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

#include "KoPABackgroundFillWidget.h"

#include <KoPageApp.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeController.h>
#include <KoDocumentResourceManager.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <KoColorBackground.h>
#include <KoShapeLayer.h>
#include <KoSelection.h>

KoPABackgroundFillWidget::KoPABackgroundFillWidget(QWidget* parent)
: KoFillConfigWidget(parent)
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    connect(canvasController->canvas()->shapeManager()->selection(),
            SIGNAL(currentLayerChanged(const KoShapeLayer*)), this, SLOT(shapeChanged()));
}

void KoPABackgroundFillWidget::noColorSelected()
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoShape* slide = canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    if (! slide) {
        return;
    }
    canvasController->canvas()->addCommand(new KoShapeBackgroundCommand(slide, 0));
}

void KoPABackgroundFillWidget::colorChanged()
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoShape* slide = canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    if (! slide) {
        return;
    }

    KoShapeBackground *fill = new KoColorBackground(currentColor());
    KUndo2Command *firstCommand = new KoShapeBackgroundCommand(slide, fill);
    canvasController->canvas()->addCommand(firstCommand);
}

void KoPABackgroundFillWidget::gradientChanged(KoShapeBackground* background)
{
    KoShape* slide = canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    if (! slide) {
        return;
    }

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoGradientBackground *gradientBackground = dynamic_cast<KoGradientBackground*>(background);
    if (! gradientBackground) {
        return;
    }

    QGradientStops newStops = gradientBackground->gradient()->stops();
    delete gradientBackground;

    KUndo2Command *firstCommand = new KoShapeBackgroundCommand(slide, applyFillGradientStops(slide, newStops));
    canvasController->canvas()->addCommand(firstCommand);
}

void KoPABackgroundFillWidget::patternChanged(KoShapeBackground* background)
{
    KoShape* slide = canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    if (! slide) {
        return;
    }

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoPatternBackground *patternBackground = dynamic_cast<KoPatternBackground*>(background);
    if (! patternBackground) {
        return;
    }

    KoImageCollection *imageCollection = canvasController->canvas()->shapeController()->resourceManager()->imageCollection();
    if (imageCollection) {
        KoPatternBackground *fill = new KoPatternBackground(imageCollection);
        fill->setPattern(patternBackground->pattern());
        canvasController->canvas()->addCommand(new KoShapeBackgroundCommand(slide, fill));
    }
}

void KoPABackgroundFillWidget::shapeChanged()
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    setCanvas(canvasController->canvas());
    updateWidget(0);
}


void KoPABackgroundFillWidget::updateWidget(KoShape* shape)
{
    Q_UNUSED(shape);

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoShape* slide = canvasController->canvas()->resourceManager()->koShapeResource(KoPageApp::CurrentPage);
    if (! slide) {
        return;
    }
    KoFillConfigWidget::updateWidget(slide);
}
