/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "RdfSemanticTreeWidgetSelectAction.h"

#include "KoDocumentRdf.h"
#include "KoRdfSemanticItemViewSite.h"

#include <KoTextEditor.h>

#include <kdebug.h>

RdfSemanticTreeWidgetSelectAction::RdfSemanticTreeWidgetSelectAction(QWidget *parent, KoCanvasBase *canvas, hKoRdfSemanticItem si, const QString &name)
    : RdfSemanticTreeWidgetAction(parent, canvas, name)
    , si(si)
{
    kDebug(30015) << "select action ctor";
}

RdfSemanticTreeWidgetSelectAction::~RdfSemanticTreeWidgetSelectAction()
{
}

void RdfSemanticTreeWidgetSelectAction::activated()
{
    const KoDocumentRdf *rdf = si->documentRdf();
    QString xmlid = rdf->findXmlId(KoTextEditor::getTextEditorFromCanvas(m_canvas));
    kDebug(30015) << "selecting xmlid:" << xmlid;
    KoRdfSemanticItemViewSite vs(si, xmlid);
    vs.select(m_canvas);
}
