/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "Part.h"

#include "Factory.h"
#include "View.h"
#include "Doc.h"
#include "CanvasItem.h"
#include "Map.h"
#include "LoadingInfo.h"

#include "ui/Selection.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoToolManager.h>
#include <KoInteractionTool.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>

#include <kglobal.h>
#include <kmessagebox.h>

using namespace Calligra::Sheets;

Part::Part(QObject *parent)
    : KoPart(parent)
{
    setComponentData(Factory::global(), false);
    setTemplateType("sheets_template");
}

Part::~Part()
{
}

void Part::setDocument(Doc *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KoView* Part::createViewInstance(QWidget* parent)
{
    View *view = new View(this, parent, m_document);
    // If we don't have this here, the next call will die horribly
    KoToolManager::instance()->addController(view->canvasController());
    // explicit switch tool to be sure that the list of option-widgets (CellToolOptionWidget
    // as returned by KoToolBase::optionWidgets) is updated to prevent crashes like bug 278896.
    KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);
    // We need to set the active sheet, otherwise we will break various other bits of the API
    // which expect your view to actually be ready for interaction after being created (e.g.
    // printing)
    view->setActiveSheet(m_document->map()->sheet(0));
    return view;
}

QGraphicsItem *Part::createCanvasItem()
{
    return new CanvasItem(m_document);
}

void Part::openTemplate(const KUrl& url)
{
    m_document->map()->loadingInfo()->setLoadTemplate(true);
    KoPart::openTemplate(url);
    m_document->map()->deleteLoadingInfo();
    m_document->initConfig();
}

void Part::addView(KoView *_view)
{
    KoPart::addView(_view);
    foreach(KoView* view, views())
    static_cast<View*>(view)->selection()->emitCloseEditor(true);
}

