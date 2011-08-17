/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2011 Boudewijn Rempt <boud@valdyas.org>
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
#include "Words.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"
#include "frames/KWTextFrameSet.h"

#include <KoTextEditor.h>
#include <KoShapeController.h>
#include <KoToolProxy.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include <klocale.h>
#include <kdebug.h>

#include <QTimer>
#include <QTextDocument>
#include <KMenu>

#include "KoRdfSemanticTreeWidgetItem.h"

KWRdfDocker::KWRdfDocker()
    : m_canvas(0),
      m_lastCursorPosition(-1),
      m_autoUpdate(false),
      m_document(0),
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
    m_rdfSemanticTree = KoRdfSemanticTree::createTree(widgetDocker.semanticView);

    widgetDocker.semanticView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(widgetDocker.semanticView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showSemanticViewContextMenu(const QPoint &)));
    setWidget(widget);

    connect(widgetDocker.refresh, SIGNAL(pressed()), this, SLOT(updateDataForced()));
    connect(widgetDocker.autoRefresh, SIGNAL(stateChanged(int)), this, SLOT(setAutoUpdate(int)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));

    widgetDocker.autoRefresh->setCheckState( Qt::Unchecked );
}

KWRdfDocker::~KWRdfDocker()
{
}

void KWRdfDocker::setCanvas(KoCanvasBase *canvas)
{
    kDebug(30015) << "canvas:" << canvas;
    
    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }
    
    //kDebug(30015) << "canvas:" << canvas;
    m_canvas = canvas;
    KWDocument *newDoc = dynamic_cast<KWDocument*>(m_canvas->shapeController()->resourceManager()->odfDocument());
    if (newDoc != m_document) {
        if (m_document) {
            m_document->disconnect(this);  // "Every connection you make emits a signal, so duplicate connections emit two signals"
        }
        
        m_document = newDoc;
        widgetDocker.semanticView->setDocumentRdf(m_document->documentRdf());
        connect(m_document->documentRdf(), SIGNAL(semanticObjectAdded(KoRdfSemanticItem*)),
                this, SLOT(semanticObjectAdded(KoRdfSemanticItem*)));
        connect(m_document->documentRdf(), SIGNAL(semanticObjectUpdated(KoRdfSemanticItem*)),
                this, SLOT(semanticObjectUpdated(KoRdfSemanticItem*)));
    }
    widgetDocker.semanticView->setCanvas(m_canvas);
    setAutoUpdate(widgetDocker.autoRefresh->checkState());
    connect(m_canvas->resourceManager(), SIGNAL(resourceChanged(int,const QVariant&)),
            this, SLOT(resourceChanged(int,const QVariant&)));
}

void KWRdfDocker::unsetCanvas()
{
    m_canvas = 0;
    m_document = 0;
    widgetDocker.semanticView->unsetCanvas();
}

KoCanvasBase* KWRdfDocker::canvas()
{
    return m_canvas;
}


void KWRdfDocker::semanticObjectAdded(KoRdfSemanticItem *item)
{
    Q_UNUSED(item);
    //kDebug(30015) << "new item...";
    updateData();
}

void KWRdfDocker::semanticObjectUpdated(KoRdfSemanticItem *item)
{
    Q_UNUSED(item);
    //kDebug(30015) << "updated item...";
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
        if (KoRdfSemanticTreeWidgetItem *item = dynamic_cast<KoRdfSemanticTreeWidgetItem*>(baseitem)) {
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
    if (!m_document || !m_canvas || !isVisible())
        return;

    kDebug(30015) << "doc:" << m_document << " canvas:" << m_canvas;

    // TODO try to get rid of 'editor' here by remembering the position in the resourceChanged()
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    KoDocumentRdf *rdf = m_document->documentRdf();
    if (editor && rdf)
    {
        //kDebug(30015) << "m_lastCursorPosition:" << m_lastCursorPosition;
        //kDebug(30015) << " currentpos:" << handler->position();

        // If the cursor hasn't moved, there is no work to do.
        if (m_lastCursorPosition == editor->position())
            return;
        m_lastCursorPosition = editor->position();
        Soprano::Model* model = rdf->findStatements(editor);
        //kDebug(30015) << "----- current Rdf ----- sz:" << model->statementCount();

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
    // XXX: autoupdate should probably not use a timer, but the text editor plugin
    //      functionality, like the statistics docker.
    if (m_canvas) {
        //kDebug(30015) << "m_textDocument:" << m_textDocument;
        if (state == Qt::Checked) {
            m_autoUpdate = true;
            m_timer->start();
        } else {
            m_autoUpdate = false;
            m_timer->stop();
        }
        widgetDocker.refresh->setVisible(!m_autoUpdate);
    }
}

void KWRdfDocker::resourceChanged(int key, const QVariant &value)
{
    if (key == KoText::CurrentTextDocument) {
        m_textDocument = static_cast<QTextDocument*>(value.value<void*>());
    } else if (key == KoText::CurrentTextPosition) {
        updateData();
    }
}
