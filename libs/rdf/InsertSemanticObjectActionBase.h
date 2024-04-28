/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_InsertSemanticObjectActionBase_h__
#define __rdf_InsertSemanticObjectActionBase_h__

#include "RdfForward.h"
#include <QAction>

class KoCanvasBase;

class InsertSemanticObjectActionBase : public QAction
{
    Q_OBJECT
public:
    InsertSemanticObjectActionBase(KoCanvasBase *canvas, KoDocumentRdf *rdf, const QString &name);
    virtual ~InsertSemanticObjectActionBase();

private Q_SLOTS:
    virtual void activated();

protected:
    KoCanvasBase *m_canvas;
    KoDocumentRdf *m_rdf;
};

#endif
