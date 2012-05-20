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
#include <KMessageBox>

Part::Part(QObject *parent)
    : KoPart(parent)
{
    setComponentData( Factory::global(), false ); // Do not load plugins now (the view will load them)
    setTemplateType( "plan_template" );
}

Part::~Part()
{
}

void Part::setDocument(FlowDocument *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KoView *Part::createViewInstance(QWidget *parent)
{
    // syncronize view selector
    View *view = dynamic_cast<View*>(views().value(0));
    if (view && m_context) {
        QDomDocument doc = m_context->save(view);
        m_context->setContent(doc.toString());
    }
    view = new View(this, m_document, parent);
    connect(view, SIGNAL(destroyed()), this, SLOT(slotViewDestroyed()));
    return view;
}
