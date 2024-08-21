/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BibliographyPreview.h"

#include "KoBibliographyInfo.h"
#include "KoTextDocumentLayout.h"
#include "KoZoomHandler.h"
#include "TextTool.h"

#include <KoInlineTextObjectManager.h>
#include <KoParagraphStyle.h>
#include <KoShapePaintingContext.h>

BibliographyPreview::BibliographyPreview(QWidget *parent)
    : QFrame(parent)
    , m_textShape(nullptr)
    , m_pm(nullptr)
    , m_styleManager(nullptr)
    , m_previewPixSize(QSize(0, 0))
{
}

BibliographyPreview::~BibliographyPreview()
{
    deleteTextShape();

    if (m_pm) {
        delete m_pm;
        m_pm = nullptr;
    }
}

void BibliographyPreview::setStyleManager(KoStyleManager *styleManager)
{
    m_styleManager = styleManager;
}

void BibliographyPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter *p = new QPainter(this);
    p->save();
    p->translate(5.5, 1.5);
    p->setRenderHint(QPainter::Antialiasing);
    QRect rectang = rect();
    rectang.adjust(-4, -4, -4, -4);

    if (m_pm) {
        p->drawPixmap(rectang, *m_pm, m_pm->rect());
    } else {
        p->fillRect(rectang, QBrush(QColor(Qt::white)));
    }

    p->restore();

    delete p;
}

void BibliographyPreview::updatePreview(KoBibliographyInfo *newbibInfo)
{
    QTextBlockFormat bibFormat;
    QTextDocument *bibDocument = new QTextDocument(this);
    KoTextDocument(bibDocument).setStyleManager(m_styleManager);
    KoBibliographyInfo *info = newbibInfo->clone();

    bibFormat.setProperty(KoParagraphStyle::BibliographyData, QVariant::fromValue<KoBibliographyInfo *>(info));
    bibFormat.setProperty(KoParagraphStyle::GeneratedDocument, QVariant::fromValue<QTextDocument *>(bibDocument));

    deleteTextShape();

    m_textShape = new TextShape(&m_itom, &m_tlm);
    if (m_previewPixSize.isEmpty()) {
        m_textShape->setSize(size());
    } else {
        m_textShape->setSize(m_previewPixSize);
    }
    QTextCursor cursor(m_textShape->textShapeData()->document());

    QTextCharFormat textCharFormat = cursor.blockCharFormat();
    textCharFormat.setFontPointSize(16);
    textCharFormat.setFontWeight(QFont::Bold);

    textCharFormat.setProperty(QTextCharFormat::ForegroundBrush, QBrush(Qt::black));
    cursor.setCharFormat(textCharFormat);

    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

    QTextBlockFormat titleBlockFormat;
    cursor.insertBlock(titleBlockFormat, textCharFormat);
    cursor.insertText(info->m_indexTitleTemplate.text);

    textCharFormat.setFontPointSize(12);
    textCharFormat.setFontWeight(QFont::Normal);
    QTextBlockFormat blockFormat;
    cursor.insertBlock(blockFormat, textCharFormat);
    cursor.insertBlock(blockFormat, textCharFormat);
    cursor.insertText("CIT01: Title, Author, Organisation, URL");

    KoTextDocument(m_textShape->textShapeData()->document()).setStyleManager(m_styleManager);

    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout *>(m_textShape->textShapeData()->document()->documentLayout());
    connect(lay, &KoTextDocumentLayout::finishedLayout, this, &BibliographyPreview::finishedPreviewLayout);
    if (lay) {
        lay->layout();
    }
}

void BibliographyPreview::finishedPreviewLayout()
{
    if (m_pm) {
        delete m_pm;
        m_pm = nullptr;
    }

    if (m_previewPixSize.isEmpty()) {
        m_pm = new QPixmap(size());
    } else {
        m_pm = new QPixmap(m_previewPixSize);
    }
    m_pm->fill(Qt::white);
    m_zoomHandler.setZoom(0.9);
    m_zoomHandler.setDpi(72, 72);
    QPainter p(m_pm);

    if (m_textShape) {
        if (m_previewPixSize.isEmpty()) {
            m_textShape->setSize(size());
        } else {
            m_textShape->setSize(m_previewPixSize);
        }
        KoShapePaintingContext paintContext; // FIXME
        m_textShape->paintComponent(p, m_zoomHandler, paintContext);
    }
    Q_EMIT pixmapGenerated();
    update();
}

QPixmap BibliographyPreview::previewPixmap()
{
    return QPixmap(*m_pm);
}

void BibliographyPreview::deleteTextShape()
{
    if (m_textShape) {
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout *>(m_textShape->textShapeData()->document()->documentLayout());
        if (lay) {
            lay->setContinuousLayout(false);
            lay->setBlockLayout(true);
        }
        delete m_textShape;
        m_textShape = nullptr;
    }
}

void BibliographyPreview::setPreviewSize(const QSize &size)
{
    m_previewPixSize = size;
}
