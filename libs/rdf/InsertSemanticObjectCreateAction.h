/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_InsertSemanticObjectCreateAction_h__
#define __rdf_InsertSemanticObjectCreateAction_h__

#include "InsertSemanticObjectActionBase.h"

class InsertSemanticObjectCreateAction : public InsertSemanticObjectActionBase
{
    Q_OBJECT
public:
    InsertSemanticObjectCreateAction(KoCanvasBase *canvas, KoDocumentRdf *rdf, const QString &name);
    virtual ~InsertSemanticObjectCreateAction();

private Q_SLOTS:
    virtual void activated();

private:
    QString m_semanticClass;
};

#endif
