/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Part.h"
#include "CanvasItem.h"
#include "Doc.h"
#include "Factory.h"
#include "View.h"

#include "core/LoadingInfo.h"
#include "core/Map.h"
#include "core/Sheet.h"

#include "ui/Selection.h"

#include <KoComponentData.h>
#include <KoInteractionTool.h>
#include <KoToolManager.h>

using namespace Calligra::Sheets;

Part::Part(QObject *parent)
    : KoPart(Factory::global(), parent)
{
    setTemplatesResourcePath(QLatin1String("calligrasheets/templates/"));
}

Part::~Part() = default;

void Part::setDocument(Doc *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KoView *Part::createViewInstance(KoDocument *document, QWidget *parent)
{
    View *view = new View(this, parent, qobject_cast<Sheets::Doc *>(document));
    // If we don't have this here, the next call will die horribly
    KoToolManager::instance()->addController(view->canvasController());
    // explicit switch tool to be sure that the list of option-widgets (CellToolOptionWidget
    // as returned by KoToolBase::optionWidgets) is updated to prevent crashes like bug 278896.
    KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);
    // We need to set the active sheet, otherwise we will break various other bits of the API
    // which expect your view to actually be ready for interaction after being created (e.g.
    // printing)
    SheetBase *sheet = qobject_cast<Sheets::Doc *>(document)->map()->sheet(0);
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
    view->setActiveSheet(fullSheet);
    return view;
}

QGraphicsItem *Part::createCanvasItem(KoDocument *document)
{
    return new CanvasItem(qobject_cast<Sheets::Doc *>(document));
}

KoMainWindow *Part::createMainWindow()
{
    return new KoMainWindow(SHEETS_MIME_TYPE, componentData());
}

void Part::openTemplate(const QUrl &url)
{
    m_document->map()->loadingInfo()->setLoadTemplate(true);
    KoPart::openTemplate(url);
    m_document->map()->deleteLoadingInfo();
    m_document->initConfig();
}

void Part::addView(KoView *_view, KoDocument *document)
{
    KoPart::addView(_view, document);
    foreach (KoView *view, views()) {
        static_cast<View *>(view)->selection()->emitCloseEditor(true);
    }
}
