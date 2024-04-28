/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoRdfSemanticTreeWidgetItem.h"

#include "KoDocumentRdf.h"
// main
#include <KoCanvasBase.h>
#include <KoDocumentResourceManager.h>
#include <KoTextEditor.h>
#include <KoToolProxy.h>
// KF5
#include <KActionMenu>
#include <KLocalizedString>
#include <kdebug.h>
#include <kpagedialog.h>
// Qt
#include <QVBoxLayout>

using namespace Soprano;

class RdfSemanticTreeWidgetApplyStylesheet : public RdfSemanticTreeWidgetAction
{
    hKoRdfSemanticItem si;
    hKoSemanticStylesheet ss;

public:
    RdfSemanticTreeWidgetApplyStylesheet(QWidget *parent,
                                         KoCanvasBase *canvas,
                                         const QString &name,
                                         hKoRdfSemanticItem si,
                                         hKoSemanticStylesheet ss = hKoSemanticStylesheet(0));
    virtual ~RdfSemanticTreeWidgetApplyStylesheet();
    virtual void activated();
};

RdfSemanticTreeWidgetApplyStylesheet::RdfSemanticTreeWidgetApplyStylesheet(QWidget *parent,
                                                                           KoCanvasBase *canvas,
                                                                           const QString &name,
                                                                           hKoRdfSemanticItem si,
                                                                           hKoSemanticStylesheet ss)
    : RdfSemanticTreeWidgetAction(parent, canvas, name)
    , si(si)
    , ss(ss)
{
}

RdfSemanticTreeWidgetApplyStylesheet::~RdfSemanticTreeWidgetApplyStylesheet()
{
}

void RdfSemanticTreeWidgetApplyStylesheet::activated()
{
    kDebug(30015) << "apply selected stylesheet for semantic item...";
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    const KoDocumentRdf *rdf = si->documentRdf();
    QString xmlid = rdf->findXmlId(editor);
    kDebug(30015) << "semItem:" << si->name() << "xmlid:" << xmlid;
    KoRdfSemanticItemViewSite vs(si, xmlid);
    if (ss) {
        kDebug(30015) << "apply stylesheet, format(), sheet:" << ss->name() << " xmlid:" << xmlid;
        vs.applyStylesheet(editor, ss);
    } else {
        vs.disassociateStylesheet();
    }
}

KoRdfSemanticTreeWidgetItem::KoRdfSemanticTreeWidgetItem(QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
{
}

KoRdfSemanticTreeWidgetItem::~KoRdfSemanticTreeWidgetItem()
{
}

QAction *KoRdfSemanticTreeWidgetItem::createAction(QWidget *parent, KoCanvasBase *host, const QString &text)
{
    return new RdfSemanticTreeWidgetAction(parent, host, text);
}

void KoRdfSemanticTreeWidgetItem::addApplyStylesheetActions(QWidget *parent, QList<QAction *> &actions, KoCanvasBase *host)
{
    if (!host) {
        return;
    }
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(host);
    kDebug(30015) << " semanticItem:" << semanticItem();
    kDebug(30015) << " semanticItem.name:" << semanticItem()->name();
    if (!editor) {
        return;
    }
    QString xmlid = semanticItem()->documentRdf()->findXmlId(editor);
    if (!xmlid.size()) {
        return;
    }
    kDebug(30015) << "xmlid:" << xmlid;
    KActionMenu *topMenu = new KActionMenu(i18n("Apply Stylesheet"), parent);
    actions.append(topMenu);
    KActionMenu *subMenu = new KActionMenu(i18n("System"), topMenu);
    topMenu->addAction(subMenu);
    foreach (hKoSemanticStylesheet ss, semanticItem()->stylesheets()) {
        kDebug(30015) << "format(), sheet:" << ss->name() << " xmlid:" << xmlid;
        QAction *action = new RdfSemanticTreeWidgetApplyStylesheet(parent, host, ss->name(), semanticItem(), ss);
        subMenu->addAction(action);
    }
    subMenu = new KActionMenu(i18n("User"), topMenu);
    topMenu->addAction(subMenu);
    foreach (hKoSemanticStylesheet ss, semanticItem()->userStylesheets()) {
        kDebug(30015) << "format(), sheet:" << ss->name() << " xmlid:" << xmlid;
        QAction *action = new RdfSemanticTreeWidgetApplyStylesheet(parent, host, ss->name(), semanticItem(), ss);
        subMenu->addAction(action);
    }
    // add reapply current sheet option
    topMenu->addSeparator();
    KoRdfSemanticItemViewSite vs(semanticItem(), xmlid);
    if (hKoSemanticStylesheet ss = vs.stylesheet()) {
        QAction *action = new RdfSemanticTreeWidgetApplyStylesheet(parent, host, i18n("Reapply Current"), semanticItem(), ss);
        topMenu->addAction(action);
    }
    QAction *action = new RdfSemanticTreeWidgetApplyStylesheet(parent, host, i18n("Disassociate"), semanticItem(), hKoSemanticStylesheet(0));
    topMenu->addAction(action);
}

QList<QAction *> KoRdfSemanticTreeWidgetItem::actions(QWidget *parent, KoCanvasBase *host)
{
    Q_UNUSED(parent);
    Q_UNUSED(host);
    return QList<QAction *>();
}

void KoRdfSemanticTreeWidgetItem::insert(KoCanvasBase *host)
{
    Q_UNUSED(host);
    kDebug(30015) << "KoRdfSemanticTreeWidgetItem::insert";
}

void KoRdfSemanticTreeWidgetItem::edit()
{
    QString caption = i18n("Edit %1", uIObjectName());
    QWidget *widget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(widget);
    widget->setLayout(lay);
    lay->setMargin(0);
    QWidget *w = semanticItem()->createEditor(widget);
    lay->addWidget(w);
    KPageDialog dialog;
    dialog.setWindowTitle(caption);
    dialog.addPage(widget, QString());
    if (dialog.exec() == KPageDialog::Accepted) {
        kDebug(30015) << "KoRdfSemanticTreeWidgetItem::edit() accepted...";
        semanticItem()->updateFromEditorData();
    }
}
