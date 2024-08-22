/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoShapePaintingContext.h"

#include "KoCanvasBase.h"
#include "KoCanvasResourceManager.h"

KoShapePaintingContext::KoShapePaintingContext()
    : showFormattingCharacters(false)
    , showTextShapeOutlines(false)
    , showTableBorders(true)
    , showSectionBounds(false)
    , showSpellChecking(false)
    , showSelections(true)
    , showInlineObjectVisualization(false)
    , showAnnotations(false)
{
}

KoShapePaintingContext::KoShapePaintingContext(KoCanvasBase *canvas, bool forPrint)
{
    KoCanvasResourceManager *rm = canvas->resourceManager();

    showFormattingCharacters = rm->boolResource(KoCanvasResourceManager::ShowFormattingCharacters);
    if (forPrint) {
        showTextShapeOutlines = false;
        showFormattingCharacters = false;
        showTableBorders = false;
        showSectionBounds = false;
        showInlineObjectVisualization = false;
    } else {
        showTextShapeOutlines = rm->boolResource(KoCanvasResourceManager::ShowTextShapeOutlines);
        showInlineObjectVisualization = rm->boolResource(KoCanvasResourceManager::ShowInlineObjectVisualization);
        if (rm->hasResource(KoCanvasResourceManager::ShowTableBorders)) {
            showTableBorders = rm->boolResource(KoCanvasResourceManager::ShowTableBorders);
        } else {
            showTableBorders = true;
        }
        if (rm->hasResource(KoCanvasResourceManager::ShowSectionBounds)) {
            showSectionBounds = rm->boolResource(KoCanvasResourceManager::ShowSectionBounds);
        } else {
            showSectionBounds = true;
        }
    }
    showSpellChecking = !forPrint;
    showSelections = !forPrint;
    showAnnotations = !forPrint;
}

KoShapePaintingContext::~KoShapePaintingContext() = default;
