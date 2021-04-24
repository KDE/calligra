/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_InsertSemanticObjectReferenceAction_h__
#define __rdf_InsertSemanticObjectReferenceAction_h__

#include "InsertSemanticObjectActionBase.h"

class InsertSemanticObjectReferenceAction : public InsertSemanticObjectActionBase
{
    Q_OBJECT
public:
    InsertSemanticObjectReferenceAction(KoCanvasBase *canvas, KoDocumentRdf *rdf, const QString &name);
    virtual ~InsertSemanticObjectReferenceAction();

private Q_SLOTS:
    virtual void activated();
};
#endif
