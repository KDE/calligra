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

#include "FlowPart.h"

#include "FlowView.h"
#include "FlowDocument.h"
#include "FlowFactory.h"

#include <KoPACanvasItem.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoInteractionTool.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>

#include <kglobal.h>
#include <kmessagebox.h>

FlowPart::FlowPart(QObject *parent)
    : KoPart(parent)
{
    setTemplateType("flow_template");
    setComponentData(FlowFactory::componentData());
}

FlowPart::~FlowPart()
{
}

void FlowPart::setDocument(FlowDocument *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KoView * FlowPart::createViewInstance(KoDocument *document, QWidget *parent)
{
    FlowView *view = new FlowView(this, qobject_cast<FlowDocument*>(document), parent);
    connect(document, SIGNAL(shapeAdded(KoShape*)), view->viewMode(), SLOT(addShape(KoShape*)));
    connect(document, SIGNAL(shapeRemoved(KoShape*)), view->viewMode(), SLOT(removeShape(KoShape*)));
    connect(document, SIGNAL(replaceActivePage(KoPAPageBase*,KoPAPageBase*)), view, SLOT(replaceActivePage(KoPAPageBase*,KoPAPageBase*)));
    return view;
}

QGraphicsItem *FlowPart::createCanvasItem(KoDocument *document)
{
    KoPACanvasItem *canvasItem = new KoPACanvasItem(qobject_cast<KoPADocument*>(document));
    return canvasItem;
}

KoMainWindow *FlowPart::createMainWindow()
{
    return new KoMainWindow(MIME_TYPE, componentData());
}

void FlowPart::showStartUpWidget(KoMainWindow *parent, bool alwaysShow)
{
    // Go through all (optional) plugins we require and quit if necessary
    bool error = false;
    KoShapeFactoryBase *factory;

    factory = KoShapeRegistry::instance()->value("TextShapeID");
    if (!factory) {
        m_errorMessage = i18n("Can not find needed text component, Calligra Flow will quit now.");
        error = true;
    }
    factory = KoShapeRegistry::instance()->value("PictureShape");
    if (!factory) {
        m_errorMessage = i18n("Can not find needed picture component, Calligra Flow will quit now.");
        error = true;
    }

    if (error) {
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
    } else {
        KoPart::showStartUpWidget(parent, alwaysShow);
    }
}

void FlowPart::showErrorAndDie()
{
    KMessageBox::error(0, m_errorMessage, i18n( "Installation Error"));
    // This means "the environment is incorrect" on Windows
    // FIXME: Is this uniform on all platforms?
    QCoreApplication::exit(10);
}

