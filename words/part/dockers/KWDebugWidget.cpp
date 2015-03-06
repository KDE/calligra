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

#include "KWDebugWidget.h"
#include <KoTextEditor.h>
#include <KoParagraphStyle.h>
#include <KoSection.h>
#include <KoSectionEnd.h>
#include <KoTextDocument.h>
#include <KoSectionManager.h>
#include <KoDocumentRdf.h>
#include <KoElementReference.h>
#include <KoShapeController.h>
#include <KoRdfPrefixMapping.h>
#include <KoTextInlineRdf.h>
#include <KoSectionUtils.h>

#include <QHBoxLayout>
#include <QTimer>

KWDebugWidget::KWDebugWidget(QWidget *parent)
    : QWidget(parent)
    , m_canvas(0)
{
    initUi();
    initLayout();

    updateData();
}

KWDebugWidget::~KWDebugWidget()
{
}

void KWDebugWidget::initUi()
{
    m_label = new QLabel(this);
    m_label->setText("Some debug info will be here."); // No i18n as it's for debug only.

    m_buttonSet = new QPushButton(this);
    m_buttonSet->setText("Set"); // No i18n as it's for debug only.
    connect(m_buttonSet, SIGNAL(clicked(bool)), this, SLOT(doSetMagic()));

    m_buttonGet = new QPushButton(this);
    m_buttonGet->setText("Get"); // No i18n as it's for debug only.
    connect(m_buttonGet, SIGNAL(clicked(bool)), this, SLOT(doGetMagic()));
}

void KWDebugWidget::initLayout()
{
    QVBoxLayout *mainBox = new QVBoxLayout(this);
    mainBox->addWidget(m_label);
    mainBox->addWidget(m_buttonSet);
    mainBox->addWidget(m_buttonGet);

    setLayout(mainBox);
}

void KWDebugWidget::updateData()
{
    QTimer().singleShot(100, this, SLOT(updateData()));
    if (!isVisible()) {
        return;
    }
    
    KoTextEditor *editor = 0;
    if (m_canvas) {
        editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
        if (!editor) {
            return;
        }
    } else {
        return;
    }

    QTextBlock curBlock = editor->block();
    QTextBlockFormat fmt = curBlock.blockFormat();

    QString willShow = "This sections starts here :";
    foreach (const KoSection *sec, KoSectionUtils::sectionStartings(fmt)) {
        willShow += " \"" + sec->name() + "\"";
    }
    willShow.append("\n");

    willShow += "This sections end here :";
    foreach (const KoSectionEnd *sec, KoSectionUtils::sectionEndings(fmt)) {
        willShow += " \"" + sec->name() + "\"";
    }
    willShow.append("\n");

    willShow += "block number is " + QString::number(editor->constCursor().block().blockNumber()) + "\n";
    willShow += "cur pos " + QString::number(editor->constCursor().position()) + "\n";
    willShow += "pos in block " + QString::number(editor->constCursor().positionInBlock()) + "\n";
    willShow += "length of block " + QString::number(editor->constCursor().block().length()) + "\n";

    m_label->setText(willShow);
}

void KWDebugWidget::setCanvas(KWCanvas* canvas)
{
    m_canvas = canvas;
}

void KWDebugWidget::unsetCanvas()
{
}

void KWDebugWidget::doSetMagic()
{
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    if (!editor) {
        return;
    }

    QTextDocument *doc = editor->document();
    KoSectionManager *manager = KoTextDocument(doc).sectionManager();

    int pos = editor->position();

    KoSection *sec = manager->sectionAtPosition(pos);

    if (!sec) {
        return;
    }

    KWDocument *kwdoc = dynamic_cast<KWDocument *>(m_canvas->shapeController()->resourceManager()->odfDocument());
    if (!kwdoc) {
        return;
    }

    KoDocumentRdf *rdf = dynamic_cast<KoDocumentRdf *>(kwdoc->documentRdf());
    if (!rdf) {
        return;
    }
//     KoTextInlineRdf *inlineRdf = new KoTextInlineRdf(doc, sec);
//     sec->setInlineRdf(inlineRdf);

//     rdf->rememberNewInlineRdfObject(inlineRdf);

    Soprano::Node sectionNode = Soprano::Node::createResourceNode(QUrl("http://www.caligra.org/author/sections/UID_HERE"));

    rdf->prefixMapping()->insert("cau", "http://www.caligra.org/author#");

    qDebug() << rdf->model()->addStatement(
        sectionNode,
        Soprano::Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("rdf:type")),
        Soprano::Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("cau:Section")),
        rdf->manifestRdfNode());

//     qDebug() << rdf->model()->addStatement(
//         sectionNode,
//         Soprano::Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("pkg:idref")),
//         Soprano::Node::createLiteralNode(inlineRdf->xmlId()),
//         rdf->manifestRdfNode());

    Soprano::Node authorContext = Soprano::Node::createResourceNode(QUrl(rdf->RDF_PATH_CONTEXT_PREFIX + "author.rdf"));

    qDebug() << rdf->model()->addStatement(
        sectionNode,
        Soprano::Node::createResourceNode(QUrl("http://www.caligra.org/author/section#descr")),
        Soprano::Node::createLiteralNode("Some TEST descr"),
        authorContext);

    Soprano::Node authorRdfFileNode = Soprano::Node::createBlankNode("CAU_META_DATA_FILE");
    qDebug() << rdf->model()->addStatement(
        authorRdfFileNode,
        Soprano::Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("rdf:type")),
        Soprano::Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("odf:MetaDataFile")),
        rdf->manifestRdfNode());

    qDebug() << rdf->model()->addStatement(
        authorRdfFileNode,
        Soprano::Node::createResourceNode(rdf->prefixMapping()->PrefexedLocalnameToURI("pkg:path")),
        Soprano::Node::createLiteralNode("author.rdf"),
        rdf->manifestRdfNode());
}

void KWDebugWidget::doGetMagic()
{
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    if (!editor) {
        return;
    }

    QTextDocument *doc = editor->document();
    KoSectionManager *manager = KoTextDocument(doc).sectionManager();

    int pos = editor->position();

    KoSection *sec = manager->sectionAtPosition(pos);

    if (!sec) {
        return;
    }

    KWDocument *kwdoc = dynamic_cast<KWDocument *>(m_canvas->shapeController()->resourceManager()->odfDocument());
    if (!kwdoc) {
        return;
    }

    KoDocumentRdf *rdf = dynamic_cast<KoDocumentRdf *>(kwdoc->documentRdf());
    if (!rdf) {
        return;
    }

//     KoTextInlineRdf *inlineRdf = sec->inlineRdf();

    Soprano::Node authorContext = Soprano::Node::createResourceNode(QUrl(rdf->RDF_PATH_CONTEXT_PREFIX + "author.rdf"));

    Soprano::StatementIterator it = rdf->model()->listStatements(
        Soprano::Node::createResourceNode(QUrl("http://www.caligra.org/author/sections/UID_HERE")),
        Soprano::Node::createResourceNode(QUrl("http://www.caligra.org/author/section#descr")),
        Soprano::Node::createEmptyNode(),
        authorContext);

    m_buttonGet->setText(it.current().object().toString());
}
