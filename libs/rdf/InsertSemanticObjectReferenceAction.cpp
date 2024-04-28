/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertSemanticObjectReferenceAction.h"

#include "KoDocumentRdf.h"

#include "KoRdfSemanticTreeWidgetItem.h"
#include <KoCanvasBase.h>
#include <KoTextEditor.h>
#include <KoToolProxy.h>

#include <KLocalizedString>
#include <kdebug.h>
#include <kpagedialog.h>

#include <QLabel>
#include <QVBoxLayout>

InsertSemanticObjectReferenceAction::InsertSemanticObjectReferenceAction(KoCanvasBase *canvas, KoDocumentRdf *rdf, const QString &name)
    : InsertSemanticObjectActionBase(canvas, rdf, name)
{
}

InsertSemanticObjectReferenceAction::~InsertSemanticObjectReferenceAction()
{
}

void InsertSemanticObjectReferenceAction::activated()
{
    kDebug(30015) << "create semantic item reference...";
    QWidget *widget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(widget);
    widget->setLayout(lay);
    lay->setMargin(0);

    QLabel *label = new QLabel(i18n("Select the object you want to reference"), widget);
    lay->addWidget(label);
    QTreeWidget *tree = new QTreeWidget(widget);
    KoRdfSemanticTree td = KoRdfSemanticTree::createTree(tree);
    td.update(const_cast<KoDocumentRdf *>(m_rdf));
    lay->addWidget(tree);

    KPageDialog dialog(m_canvas->canvasWidget());
    dialog.setWindowTitle(i18n("%1 Options", text())); // TODO add comment (i18nc)
    dialog.addPage(widget, QString());

    if (dialog.exec() == KPageDialog::Accepted && tree->currentItem()) {
        QTreeWidgetItem *item = tree->currentItem();
        if (KoRdfSemanticTreeWidgetItem *ditem = dynamic_cast<KoRdfSemanticTreeWidgetItem *>(item)) {
            kDebug(30015) << "InsertSemanticObjectReferenceAction::activated... item:" << item;
            ditem->insert(m_canvas);
        }
    }
}
