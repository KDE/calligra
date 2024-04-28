/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_RdfSemanticTreeWidgetAction_h__
#define __rdf_RdfSemanticTreeWidgetAction_h__

#include "kordf_export.h"
// Qt
#include <QAction>

class KoCanvasBase;

class KORDF_EXPORT RdfSemanticTreeWidgetAction : public QAction
{
    Q_OBJECT

public:
    RdfSemanticTreeWidgetAction(QWidget *parent, KoCanvasBase *canvas, const QString &name);
    virtual ~RdfSemanticTreeWidgetAction();

public Q_SLOTS:
    virtual void activated();

protected:
    KoCanvasBase *m_canvas;
};

#endif
