/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#include "CAuOutlinerWidget.h"

#include <frames/KWTextFrameSet.h>
#include <KoTextDocument.h>
#include <KoSectionManager.h>
#include <KoSection.h>
#include <KoTextEditor.h>
#include <KoTextInlineRdf.h>
#include <KWView.h>
#include <KoTextLayoutRootArea.h>
#include <KoDocumentRdf.h>
#include <KoShapeController.h>
#include <KoDocumentResourceManager.h>
#include <KoRdfPrefixMapping.h>

#include <KPageDialog>
#include <QModelIndexList>
#include <QVBoxLayout>

using namespace Soprano;

CAuOutlinerWidget::CAuOutlinerWidget(QWidget *parent)
    : QWidget(parent)
    , m_canvas(0)
    , m_layout(0)
    , m_document(0)
    , m_updateTimer(new QTimer(this))
{
    initUi();
    initLayout();

    m_updateTimer->setSingleShot(true);
}

CAuOutlinerWidget::~CAuOutlinerWidget()
{
}

void CAuOutlinerWidget::initUi()
{
    m_sectionTree = new QTreeView(this);
    m_sectionTree->setItemsExpandable(false);
    m_sectionTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_sectionTree->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(m_sectionTree, SIGNAL(clicked(QModelIndex)), this, SLOT(sectionClicked(QModelIndex)));

    m_editButton = new QPushButton(this);
    m_editButton->setText(i18n("Edit section"));

    connect(m_editButton, SIGNAL(clicked(bool)), this, SLOT(sectionEditClicked()));
}

void CAuOutlinerWidget::initLayout()
{
    QVBoxLayout *mainBox = new QVBoxLayout(this);

    mainBox->addWidget(m_sectionTree);
    mainBox->addWidget(m_editButton);

    setLayout(mainBox);
}

void CAuOutlinerWidget::sectionClicked(QModelIndex idx)
{
    if (idx.column() == 0) {
        int position = static_cast<KoSection *>(
            m_sectionTree->model()->data(idx, Qt::UserRole + 1).value<void *>())->bounds().first;

        KoTextDocument(m_document).textEditor()->setPosition(position); // placing cursor
        m_canvas->view()->setFocus(); // passing focus

        KoTextLayoutRootArea *a = m_layout->rootAreaForPosition(position);
        m_canvas->view()->goToPage(*(static_cast<KWPage *>(a->page()))); // showing needed page
    }
}

void CAuOutlinerWidget::setCanvas(KWCanvas* canvas)
{
    m_document = canvas->document()->mainFrameSet()->document();
    if (m_layout) {
        disconnect(m_layout, SIGNAL(finishedLayout()), this, SLOT(updateData()));
    }
    m_layout = qobject_cast<KoTextDocumentLayout *>(m_document->documentLayout());
    connect(m_layout, SIGNAL(finishedLayout()), this, SLOT(updateData()));
    connect(KoTextDocument(m_document).textEditor(), SIGNAL(cursorPositionChanged()), this, SLOT(updateSelection()));

    m_canvas = canvas;
}

void CAuOutlinerWidget::unsetCanvas()
{
    if (m_layout) {
        disconnect(m_layout, SIGNAL(finishedLayout()), this, SLOT(updateData()));
        disconnect(KoTextDocument(m_document).textEditor(), SIGNAL(cursorPositionChanged()), this, SLOT(updateSelection()));
    }
    m_layout = 0;
    m_document = 0;
}

void CAuOutlinerWidget::updateData()
{
    if (!isVisible()) {
        return;
    }

    // don't refresh too often
    if (m_updateTimer->isActive()) {
        return;
    }

    KoSectionManager *manager = KoTextDocument(m_document).sectionManager();
    m_sectionTree->setModel(manager->update(true));
    m_sectionTree->expandAll();
    m_sectionTree->resizeColumnToContents(1);

    updateSelection();

    m_updateTimer->start(300); // FIXME: move the constant out of there
}

void CAuOutlinerWidget::updateSelection()
{
    if (!isVisible()) {
        return;
    }

    int curPos = KoTextDocument(m_document).textEditor()->position();
    KoSectionManager *manager = KoTextDocument(m_document).sectionManager();

    KoSection *sec = manager->sectionAtPosition(curPos);

    if (!sec) {
        m_sectionTree->setCurrentIndex(m_sectionTree->rootIndex());
    } else {
        QModelIndexList lst = m_sectionTree->model()->match(
            m_sectionTree->model()->index(0,0),
            Qt::UserRole + 1,
            qVariantFromValue(static_cast<void *>(sec)),
            1,
            Qt::MatchRecursive
        );
        // need next "if", because sometimes this is called before updateData()
        // is called, so new section can not be found in model
        if (lst.empty()) {
            return;
        }
        m_sectionTree->setCurrentIndex(lst.first());
    }
}

void CAuOutlinerWidget::sectionEditClicked()
{
    if (!m_sectionTree->currentIndex().isValid()) { //FIXME: make button inactive in such case
        return;
    }

    KoSection *sec = static_cast<KoSection *>(
        m_sectionTree->model()->itemData(m_sectionTree->currentIndex())[Qt::UserRole + 1].value<void *>()
    );

    KoTextInlineRdf *inlineRdf = sec->inlineRdf();
    if (!inlineRdf) {
        inlineRdf = new KoTextInlineRdf(m_document, sec);
        inlineRdf->setXmlId(inlineRdf->createXmlId());
        sec->setInlineRdf(inlineRdf);
    }

    KWDocument *kwdoc = dynamic_cast<KWDocument *>(m_canvas->shapeController()->resourceManager()->odfDocument());
    KoDocumentRdf *rdf = dynamic_cast<KoDocumentRdf *>(kwdoc->documentRdf());

    QList<hKoRdfBasicSemanticItem> lst = rdf->semanticItems("AuthorSection");

    hKoRdfBasicSemanticItem semItem;
    bool found = false;
    foreach (const hKoRdfBasicSemanticItem &item, lst) {
        if (item->xmlIdList().contains(sec->inlineRdf()->xmlId())) {
            found = true;
            semItem = item;
            break;
        }
    }

    if (!found) {
        semItem = rdf->createSemanticItem("AuthorSection", rdf);
    }

    QWidget *widget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(widget);
    widget->setLayout(lay);
    lay->setMargin(0);
    QWidget *w = semItem->createEditor(widget);
    lay->addWidget(w);

    KPageDialog dialog(m_canvas->canvasWidget());
    dialog.setCaption(i18n("Section options"));
    dialog.addPage(widget, QString());
    if (dialog.exec() == KPageDialog::Accepted) {
        semItem->updateFromEditorData();

        const QString sparqlQuery = QLatin1String(
            "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
            "PREFIX odf: <http://docs.oasis-open.org/ns/office/1.2/meta/odf#> \n"
            "PREFIX pkg: <http://docs.oasis-open.org/ns/office/1.2/meta/pkg#> \n"
            "SELECT ?metafile ?metafilename \n"
            "WHERE { \n"
            "    ?metafile rdf:type odf:MetaDataFile . \n"
            "    ?metafile pkg:path ?metafilename    . \n"
            "    FILTER( str(?metafilename) = \"author.rdf\" ) \n"
            "} \n"
        );

        QueryResultIterator it = rdf->model()->executeQuery(
            sparqlQuery,
            Query::QueryLanguageSparql
        );

        int cnt = it.bindingCount();
        it.close();

        if (!cnt) {
            Node authorRdfFileNode = Node::createBlankNode("CAU_META_DATA_FILE");
            rdf->model()->addStatement(
                authorRdfFileNode,
                Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("rdf:type")),
                Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("odf:MetaDataFile")),
                rdf->manifestRdfNode()
            );

            rdf->model()->addStatement(
                authorRdfFileNode,
                Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("pkg:path")),
                Node::createLiteralNode("author.rdf"),
                rdf->manifestRdfNode()
            );
        }

        rdf->model()->addStatement(
            semItem->linkingSubject(),
            Node::createResourceNode(QUrl("http://docs.oasis-open.org/ns/office/1.2/meta/pkg#idref")),
            Node::createLiteralNode(sec->inlineRdf()->xmlId()),
            rdf->manifestRdfNode()
        );

        rdf->rememberNewInlineRdfObject(inlineRdf);
    }
}
