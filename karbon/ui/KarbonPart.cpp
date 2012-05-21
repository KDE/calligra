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

#include "KarbonPart.h"

#include "KarbonView.h"
#include "KarbonDocument.h"
#include "KarbonKoDocument.h"
#include "KarbonFactory.h"
#include "KarbonCanvas.h"

#include <KoCanvasResourceManager.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoInteractionTool.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>

#include <kglobal.h>
#include <kconfiggroup.h>
#include <KMessageBox>

KarbonPart::KarbonPart(QObject *parent)
    : KoPart(parent)
{
    setTemplateType("karbon_template");
    setComponentData(KarbonFactory::componentData(), false);
}

KarbonPart::~KarbonPart()
{
}

void KarbonPart::setDocument(KoDocument *document)
{
    KoPart::setDocument(document);
    KarbonKoDocument *doc = qobject_cast<KarbonKoDocument*>(document);
    connect(doc, SIGNAL(applyCanvasConfiguration(KarbonCanvas*)), SLOT(applyCanvasConfiguration(KarbonCanvas*)));
}

KoView * KarbonPart::createViewInstance(QWidget *parent)
{
    KarbonKoDocument *doc = qobject_cast<KarbonKoDocument*>(document());

    KarbonView *result = new KarbonView(this, doc, parent);

    KoCanvasResourceManager * provider = result->canvasWidget()->resourceManager();
    provider->setResource(KoCanvasResourceManager::PageSize, doc->document().pageSize());

    applyCanvasConfiguration(result->canvasWidget());

    return result;
}

void KarbonPart::openTemplate(const KUrl& url)
{
    KoPart::openTemplate(url);

    // explicitly set the output mimetype to our native mimetype
    // so that autosaving works for not yet saved templates as well
    if (document()->outputMimeType().isEmpty()) {
        document()->setOutputMimeType("application/vnd.oasis.opendocument.graphics");
    }
}


void KarbonPart::applyCanvasConfiguration(KarbonCanvas *canvas)
{
    KSharedConfigPtr config = componentData().config();

    QColor color(Qt::white);
    if (config->hasGroup("Interface")) {
        color = config->group("Interface").readEntry("CanvasColor", color);
    }
    canvas->setBackgroundColor(color);
}


#include "KarbonPart.moc"
