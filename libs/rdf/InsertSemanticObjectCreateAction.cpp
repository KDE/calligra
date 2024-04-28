/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertSemanticObjectCreateAction.h"
#include "KoDocumentRdf.h"

#include "KoCanvasBase.h"
#include "KoTextEditor.h"
#include "KoToolProxy.h"

#include <KLocalizedString>
#include <kdebug.h>
#include <kpagedialog.h>

#include <QVBoxLayout>

InsertSemanticObjectCreateAction::InsertSemanticObjectCreateAction(KoCanvasBase *canvas, KoDocumentRdf *rdf, const QString &name)
    : InsertSemanticObjectActionBase(canvas, rdf, name)
    , m_semanticClass(name)
{
}

InsertSemanticObjectCreateAction::~InsertSemanticObjectCreateAction()
{
}

void InsertSemanticObjectCreateAction::activated()
{
    kDebug(30015) << "create semantic action...";
    QWidget *widget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(widget);
    widget->setLayout(lay);
    lay->setMargin(0);
    kDebug(30015) << "semanticClass:" << m_semanticClass;
    hKoRdfSemanticItem semItem(static_cast<KoRdfSemanticItem *>(m_rdf->createSemanticItem(m_semanticClass, m_rdf).data()));
    QWidget *w = semItem->createEditor(widget);
    lay->addWidget(w);
    KPageDialog dialog(m_canvas->canvasWidget());
    dialog.setWindowTitle(i18n("%1 Options", text().remove('&'))); // TODO add comment using i18nc
    dialog.addPage(widget, QString());
    if (dialog.exec() == KPageDialog::Accepted) {
        kDebug(30015) << "activated...";
        semItem->updateFromEditorData();
        semItem->insert(m_canvas);
    }
}
