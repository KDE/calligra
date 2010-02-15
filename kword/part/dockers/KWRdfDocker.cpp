/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWRdfDocker.h"

#include "KWCanvas.h"
#include "KWord.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"
#include "frames/KWTextFrameSet.h"

#include <KoTextEditor.h>
#include <KoToolProxy.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include <klocale.h>
#include <kdebug.h>

#include <QTimer>
#include <QTextDocument>
#include <KMenu>

KWRdfDocker::KWRdfDocker(KWDocument *document)
    : m_canvas(0),
    m_lastCursorPosition(-1),
    m_autoUpdate(false),
    m_document(document),
    m_timer(new QTimer(this)),
    m_textDocument(0)
{
    setWindowTitle(i18n("RDF"));
    m_timer->setInterval(300);
    m_timer->setSingleShot(true);

    QWidget *widget = new QWidget();
    widgetDocker.setupUi(widget);
    widgetDocker.refresh->setIcon(KIcon("view-refresh"));
    // Semantic view
    if (QTreeWidget *v = widgetDocker.semanticView) {
        m_rdfSemanticTree = RdfSemanticTree::createTree(v);

        v->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(v, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showSemanticViewContextMenu(const QPoint &)));

        if (m_document) {
            widgetDocker.semanticView->setDocumentRdf(m_document->documentRdf());
        }
        widgetDocker.semanticView->setCanvas(0); // TODO can this be removed?
    }
    if (m_document) {
        connect(m_document->documentRdf(), SIGNAL(semanticObjectAdded(RdfSemanticItem*)),
                this, SLOT(semanticObjectAdded(RdfSemanticItem*)));
        connect(m_document->documentRdf(), SIGNAL(semanticObjectUpdated(RdfSemanticItem*)),
                this, SLOT(semanticObjectUpdated(RdfSemanticItem*)));
    }
    setWidget(widget);

    connect(widgetDocker.refresh, SIGNAL(pressed()), this, SLOT(updateDataForced()));
    connect(widgetDocker.autoRefresh, SIGNAL(stateChanged(int)), this, SLOT(setAutoUpdate(int)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));
}

KWRdfDocker::~KWRdfDocker()
{
}

void KWRdfDocker::setCanvas(KoCanvasBase *canvas)
{
    kDebug(30015) << "canvas:" << canvas;
    m_canvas = canvas;
    widgetDocker.semanticView->setCanvas(m_canvas);

    connect(m_canvas->resourceManager(), SIGNAL(resourceChanged(int,const QVariant&)),
            this, SLOT(resourceChanged(int,const QVariant&)));
}

void KWRdfDocker::semanticObjectAdded(RdfSemanticItem *item)
{
    Q_UNUSED(item);
    kDebug(30015) << "new item...";
    updateData();
}

void KWRdfDocker::semanticObjectUpdated(RdfSemanticItem *item)
{
    Q_UNUSED(item);
    kDebug(30015) << "updated item...";
    updateData();
}

/**
 * As the QTreeWidgetItem for the actions that can be performed on it.
 */
void KWRdfDocker::showSemanticViewContextMenu(const QPoint &position)
{
    QPointer<KMenu> menu = new KMenu(0); // TODO why QPointer???
    QList<KAction *> actions;
    if (QTreeWidgetItem *baseitem = widgetDocker.semanticView->itemAt(position)) {
        if (RdfSemanticTreeWidgetItem *item = dynamic_cast<RdfSemanticTreeWidgetItem*>(baseitem)) {
            actions = item->actions(menu, m_canvas);
        }
    }
    if (actions.count() > 0) {
        foreach (KAction *a, actions) {
            menu->addAction(a);
        }
        menu->exec(widgetDocker.semanticView->mapToGlobal(position));
    }
    // TODO this leaks
}

void KWRdfDocker::updateDataForced()
{
    m_lastCursorPosition = -1;
    updateData();
}

void KWRdfDocker::updateData()
{
    kDebug(30015) << "doc:" << m_document << " canvas:" << m_canvas;
    if (!m_document || !m_canvas)
        return;
    kDebug(30015) << "updating docker...";
    // TODO try to get rid of 'handler' here by remembering the position in the resourceChanged()
    KoTextEditor *handler = qobject_cast<KoTextEditor*>(m_canvas->toolProxy()->selection());
    KoDocumentRdf *rdf = m_document->documentRdf();
    if (handler && rdf) {
        kDebug(30015) << "m_lastCursorPosition:" << m_lastCursorPosition;
        kDebug(30015) << " currentpos:" << handler->position();
        // If the cursor hasn't moved, there is no work to do.
        if (m_lastCursorPosition == handler->position())
            return;
        m_lastCursorPosition = handler->position();
        Soprano::Model* model = rdf->findStatements(handler);
        kDebug(30015) << "----- current Rdf ----- sz:" << model->statementCount();
        //
        // Now expand the found Rdf a little bit
        // and try to show any Semantic Objects
        // it contains. The user probably doesn't
        // care about the triples, just the meaning.
        //
        if (widgetDocker.semanticView) {
            m_rdfSemanticTree.update(rdf, model);
        }
        delete model;
    }
}

void KWRdfDocker::setAutoUpdate(int state)
{
    kDebug(30015) << "m_textDocument:" << m_textDocument;
    if (state == Qt::Checked) {
        KoDocumentRdf::ensureTextTool();
        m_autoUpdate = true;
        m_timer->start();
    } else {
        m_autoUpdate = false;
        m_timer->stop();
    }
    widgetDocker.refresh->setVisible(!m_autoUpdate);
}

void KWRdfDocker::resourceChanged(int key, const QVariant &value)
{
    if (key == KoText::CurrentTextDocument) {
        m_textDocument = static_cast<QTextDocument*>(value.value<void*>());
    } else if (key == KoText::CurrentTextPosition) {
        updateData();
    }
}
