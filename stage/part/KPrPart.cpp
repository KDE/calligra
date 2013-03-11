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

#include "KPrPart.h"

#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrFactory.h"

#include <KoPACanvasItem.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoInteractionTool.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>
#include "KPrShapeManagerDisplayMasterStrategy.h"
#include "KPrPageSelectStrategyActive.h"

#include <kglobal.h>
#include <KMessageBox>

KPrPart::KPrPart(QObject *parent)
    : KoPart(parent)
{
    setTemplateType("stage_template");
    setComponentData(KPrFactory::componentData(), false);
}

KPrPart::~KPrPart()
{
}

void KPrPart::setDocument(KPrDocument *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KoView * KPrPart::createViewInstance(QWidget *parent)
{
    KPrView *view = new KPrView(this, m_document, parent);
    connect(m_document, SIGNAL(shapeAdded(KoShape *)), view->viewMode(), SLOT(addShape(KoShape *)));
    connect(m_document, SIGNAL(shapeRemoved(KoShape *)), view->viewMode(), SLOT(removeShape(KoShape *)));
    connect(m_document, SIGNAL(replaceActivePage(KoPAPageBase *, KoPAPageBase *)), view, SLOT(replaceActivePage(KoPAPageBase *, KoPAPageBase *)));
    return view;
}

QGraphicsItem *KPrPart::createCanvasItem()
{
    KoPACanvasItem *canvasItem = new KoPACanvasItem(m_document);
    canvasItem->masterShapeManager()->setPaintingStrategy(new KPrShapeManagerDisplayMasterStrategy(canvasItem->masterShapeManager(),
                                                  new KPrPageSelectStrategyActive(canvasItem)));
    return canvasItem;
}

void KPrPart::showStartUpWidget(KoMainWindow *parent, bool alwaysShow)
{
    // Go through all (optional) plugins we require and quit if necessary
    bool error = false;
    KoShapeFactoryBase *factory;

    factory = KoShapeRegistry::instance()->value("TextShapeID");
    if (!factory) {
        m_errorMessage = i18n("Can not find needed text component, Calligra Stage will quit now.");
        error = true;
    }
    factory = KoShapeRegistry::instance()->value("PictureShape");
    if (!factory) {
        m_errorMessage = i18n("Can not find needed picture component, Calligra Stage will quit now.");
        error = true;
    }

    if (error) {
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
    } else {
        KoPart::showStartUpWidget(parent, alwaysShow);
    }
}

void KPrPart::showErrorAndDie()
{
    KMessageBox::error(0, m_errorMessage, i18n( "Installation Error"));
    // This means "the environment is incorrect" on Windows
    // FIXME: Is this uniform on all platforms?
    QCoreApplication::exit(10);
}

