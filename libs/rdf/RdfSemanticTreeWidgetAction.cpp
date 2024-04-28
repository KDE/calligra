/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "RdfSemanticTreeWidgetAction.h"

#include <KoCanvasBase.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

RdfSemanticTreeWidgetAction::RdfSemanticTreeWidgetAction(QWidget *parent, KoCanvasBase *canvas, const QString &name)
    : QAction(name, parent)
    , m_canvas(canvas)
{
    connect(this, SIGNAL(triggered(bool)), this, SLOT(activated()));
}

RdfSemanticTreeWidgetAction::~RdfSemanticTreeWidgetAction()
{
}

void RdfSemanticTreeWidgetAction::activated()
{
}
