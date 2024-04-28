/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertSemanticObjectActionBase.h"
#include <KoCanvasBase.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

InsertSemanticObjectActionBase::InsertSemanticObjectActionBase(KoCanvasBase *canvas, KoDocumentRdf *rdf, const QString &name)
    : QAction(name, canvas->canvasWidget())
    , m_canvas(canvas)
    , m_rdf(rdf)
{
    connect(this, SIGNAL(triggered(bool)), this, SLOT(activated()));
}

InsertSemanticObjectActionBase::~InsertSemanticObjectActionBase()
{
}

void InsertSemanticObjectActionBase::activated()
{
}
