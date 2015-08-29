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

#include "CAuMetaDataManager.h"

#include <KoDocumentRdf.h>
#include <KoRdfPrefixMapping.h>
#include <KoRdfSemanticItemRegistry.h>
#include <KoTextInlineRdf.h>
#include <KoSection.h>

#include <author/metadata/CAuSectionSemanticItemFactory.h>
#include <author/metadata/CAuActorSemanticItemFactory.h>
#include <author/CAuDocument.h>
#include <frames/KWTextFrameSet.h>

#include <QVBoxLayout>

#include <KPageDialog>

using namespace Soprano;

const QString CAuMetaDataManager::AUTHOR_PREFIX = "http://www.calligra.org/author/";
const QString CAuMetaDataManager::AUTHOR_RDF_FILE_NAME = "author.rdf";

CAuMetaDataManager::CAuMetaDataManager(CAuDocument *caudoc)
    : QObject(caudoc)
    , m_doc(caudoc)
{
    registerAuthorRdfFile();
    registerAuthorRdfPrefix();
    registerAuthorSemanticItems();
}

void CAuMetaDataManager::registerAuthorRdfFile()
{
    KoDocumentRdf *rdf = dynamic_cast<KoDocumentRdf *>(m_doc->documentRdf());

    const QString sparqlQuery = QString(
        "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
        "PREFIX odf: <http://docs.oasis-open.org/ns/office/1.2/meta/odf#> \n"
        "PREFIX pkg: <http://docs.oasis-open.org/ns/office/1.2/meta/pkg#> \n"
        "SELECT ?metafile ?metafilename \n"
        "WHERE { \n"
        "    ?metafile rdf:type odf:MetaDataFile . \n"
        "    ?metafile pkg:path ?metafilename    . \n"
        "    FILTER( str(?metafilename) = \"%1\" ) \n"
        "} \n").arg(AUTHOR_RDF_FILE_NAME).toLatin1();

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
            Node::createLiteralNode(AUTHOR_RDF_FILE_NAME),
            rdf->manifestRdfNode()
        );
    }
}

void CAuMetaDataManager::registerAuthorRdfPrefix()
{
    KoDocumentRdf *rdf = dynamic_cast<KoDocumentRdf *>(m_doc->documentRdf());

    rdf->prefixMapping()->insert("cau", AUTHOR_PREFIX);
}

void CAuMetaDataManager::registerAuthorSemanticItems()
{
    KoRdfSemanticItemRegistry::instance()->add(new CAuSectionSemanticItemFactory());
    KoRdfSemanticItemRegistry::instance()->add(new CAuActorSemanticItemFactory());
}

Soprano::Node CAuMetaDataManager::authorContext()
{
    return Node(QUrl(KoDocumentRdf::RDF_PATH_CONTEXT_PREFIX + AUTHOR_RDF_FILE_NAME));
}

void CAuMetaDataManager::callEditor(KoSection *sec) const
{
    KoTextInlineRdf *inlineRdf = sec->inlineRdf();
    if (!inlineRdf) {
        inlineRdf = new KoTextInlineRdf(m_doc->mainFrameSet()->document(), sec);
        inlineRdf->setXmlId(inlineRdf->createXmlId());
        sec->setInlineRdf(inlineRdf);
    }

    KoDocumentRdf *rdf = dynamic_cast<KoDocumentRdf *>(m_doc->documentRdf());

    QList<hKoRdfBasicSemanticItem> lst = rdf->semanticItems("Section");

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
        semItem = rdf->createSemanticItem("Section", rdf);
    }

    QWidget *widget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(widget);
    widget->setLayout(lay);
    lay->setMargin(0);
    QWidget *w = semItem->createEditor(widget);
    lay->addWidget(w);

    KPageDialog dialog(m_doc->documentPart()->currentMainwindow());
    dialog.setWindowTitle(i18n("Section's options"));
    dialog.addPage(widget, QString());
    if (dialog.exec() == KPageDialog::Accepted) {
        semItem->updateFromEditorData();
    }

    m_doc->setModified(true);

    if (!found) { // we have created new item, need to save it properly
        rdf->model()->addStatement(
            semItem->linkingSubject(),
            Node::createResourceNode(QUrl("http://docs.oasis-open.org/ns/office/1.2/meta/pkg#idref")),
            Node::createLiteralNode(sec->inlineRdf()->xmlId()),
            rdf->manifestRdfNode()
        );

        rdf->rememberNewInlineRdfObject(inlineRdf);
    }
}
