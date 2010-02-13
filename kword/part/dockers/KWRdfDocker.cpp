/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
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
#include "KWRdfDockerTree.h"

#include "KWView.h"
#include "KWCanvas.h"
#include "KWord.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"

#include <KoTextEditor.h>
#include <KoToolProxy.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoResourceManager.h>
#include <KoInlineObject.h>
#include <KoShapeContainer.h>
#include <KoInlineTextObjectManager.h>
#include <libs/kopageapp/KoShapeTraversal.h>
#include <KoBookmark.h>
#include <KoTextMeta.h>
#include <KoTextInlineRdf.h>
#include <KoTextRdfCore.h>

#include <klocale.h>
#include <kdebug.h>

#include <QTimer>
#include <QTextDocument>
#include <KMenu>

#include <rdf/KoDocumentRdf.h>


KWRdfDocker::KWRdfDocker(KWView *parent)
    :
    QDockWidget(parent),
    m_lastCursorPosition(-1),
    m_autoUpdate(false),
    m_document(0),
    m_selection(0),
    m_timer(0),
    m_textDocument(0)
{
    setWindowTitle(i18n("RDF"));
    m_canvas = parent->kwcanvas();
    m_document = parent->kwdocument();
    m_widget = new QWidget();
    m_selection = m_canvas->shapeManager()->selection();
    m_autoUpdate = false;
    m_timer = new QTimer(m_widget);
    m_timer->setInterval(300);
    m_timer->setSingleShot(false);

    widgetDocker.setupUi(m_widget);
    widgetDocker.refresh->setIcon(KIcon("view-refresh"));
    connect(widgetDocker.refresh, SIGNAL(pressed()), this, SLOT(updateDataForced()));
    connect(widgetDocker.autoRefresh, SIGNAL(stateChanged(int)), this, SLOT(setAutoUpdate(int)));
    connect(m_selection, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));
    // Semantic view
    if (QTreeWidget* v = widgetDocker.semanticView) {
        m_rdfSemanticTree = RdfSemanticTree::createTree(v);

        v->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(v, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showSemanticViewContextMenu(const QPoint &)));

        if (m_document) {
            widgetDocker.semanticView->setDocumentRdf(m_document->documentRdf());
        }
        widgetDocker.semanticView->setCanvas(m_canvas);
    }
    if (m_document) {
        connect(m_document->documentRdf(), SIGNAL(semanticObjectAdded(RdfSemanticItem*)),
                this, SLOT(semanticObjectAdded(RdfSemanticItem*)));
        connect(m_document->documentRdf(), SIGNAL(semanticObjectUpdated(RdfSemanticItem*)),
                this, SLOT(semanticObjectUpdated(RdfSemanticItem*)));
    }
    setWidget(m_widget);
    widgetDocker.autoRefresh->setChecked(m_autoUpdate);
}

KWRdfDocker::~KWRdfDocker()
{
}

KoCanvasBase *KWRdfDocker::canvas()
{
    return m_canvas;
}

void KWRdfDocker::setCanvas(KoCanvasBase *canvas)
{
    kDebug(30015) << "canvas:" << canvas;
    m_canvas = canvas;
    m_selection = m_canvas->shapeManager()->selection();
    widgetDocker.semanticView->setCanvas(m_canvas);
}

void KWRdfDocker::semanticObjectAdded(RdfSemanticItem* item)
{
    Q_UNUSED(item);
    kDebug(30015) << "new item...";
    updateData();
}

void KWRdfDocker::semanticObjectUpdated(RdfSemanticItem* item)
{
    Q_UNUSED(item);
    kDebug(30015) << "updated item...";
    updateData();
}

/**
 * As the QTreeWidgetItem for the actions that can be performed on it.
 */
void KWRdfDocker::showSemanticViewContextMenu(const QPoint& position)
{
    QPointer<KMenu> menu = new KMenu(0);
    QList<KAction *> actions;
    if (QTreeWidgetItem* baseitem = widgetDocker.semanticView->itemAt(position)) {
        if (RdfSemanticTreeWidgetItem* item = dynamic_cast<RdfSemanticTreeWidgetItem*>(baseitem)) {
            actions = item->actions(menu, m_canvas);
        }
    }
    if (actions.count() > 0) {
        foreach (KAction* a, actions) {
            menu->addAction(a);
        }
        menu->exec(widgetDocker.semanticView->mapToGlobal(position));
    }
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
    KoTextEditor* handler = qobject_cast<KoTextEditor*> (m_canvas->toolProxy()->selection());
    KoDocumentRdf* rdf = m_document->documentRdf();
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
        connect(m_textDocument, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SLOT(updateData()));
    } else {
        m_autoUpdate = false;
        m_timer->stop();
        disconnect(m_textDocument, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SLOT(updateData()));
    }
    widgetDocker.refresh->setVisible(!m_autoUpdate);
}

void KWRdfDocker::selectionChanged()
{
    if (m_textDocument) {
        disconnect(m_textDocument, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SLOT(updateData()));
    }
    if (m_selection->count() != 1) {
        return;
    }
    KoShape *shape = m_selection->firstSelectedShape();
    if (!shape) {
        return;
    }
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    if (!frame) {
        return;
    }
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
    if (fs) {
        m_textDocument = fs->document();
        if (m_autoUpdate) {
            connect(m_textDocument, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SLOT(updateData()));
        }
    }
}
