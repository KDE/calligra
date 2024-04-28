/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_RdfSemanticTreeWidgetSelectAction_h__
#define __rdf_RdfSemanticTreeWidgetSelectAction_h__

#include "KoRdfSemanticItem.h"
#include "RdfSemanticTreeWidgetAction.h"
#include "kordf_export.h"

class KORDF_EXPORT RdfSemanticTreeWidgetSelectAction : public RdfSemanticTreeWidgetAction
{
    hKoRdfSemanticItem si;

public:
    RdfSemanticTreeWidgetSelectAction(QWidget *parent, KoCanvasBase *canvas, hKoRdfSemanticItem si, const QString &name = QString("Select"));

    virtual ~RdfSemanticTreeWidgetSelectAction();
    virtual void activated();
};
#endif
