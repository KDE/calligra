/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TableOfContentsPreview.h"

#include "KoTableOfContentsGeneratorInfo.h"
#include "KoTextDocumentLayout.h"
#include "KoZoomHandler.h"
#include "TextTool.h"

#include <KoInlineTextObjectManager.h>
#include <KoParagraphStyle.h>
#include <KoShapePaintingContext.h>

TableOfContentsPreview::TableOfContentsPreview(QWidget *parent)
    : QFrame(parent)
    , m_previewPixSize(QSize(0, 0))
{
}

TableOfContentsPreview::~TableOfContentsPreview()
{
    deleteTextShape();
}

void TableOfContentsPreview::setStyleManager(KoStyleManager *styleManager)
{
    m_styleManager = styleManager;
}

void TableOfContentsPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.save();
    painter.translate(5.5, 1.5);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rectang = rect();
    rectang.adjust(-4, -4, -4, -4);

    if (m_pm) {
        painter.drawPixmap(rectang, *m_pm, m_pm->rect());
    } else {
        painter.fillRect(rectang, QBrush(QColor(Qt::white)));
    }

    painter.restore();
}

void TableOfContentsPreview::updatePreview(KoTableOfContentsGeneratorInfo *newToCInfo)
{
    QTextBlockFormat tocFormat;
    QTextDocument *tocDocument = new QTextDocument(this);
    KoTextDocument(tocDocument).setStyleManager(m_styleManager);
    KoTableOfContentsGeneratorInfo *info = newToCInfo->clone();
    // info->m_indexTitleTemplate.text = newToCInfo->m_indexTitleTemplate.text;
    // info->m_useOutlineLevel = newToCInfo->m_useOutlineLevel;

    tocFormat.setProperty(KoParagraphStyle::TableOfContentsData, QVariant::fromValue<KoTableOfContentsGeneratorInfo *>(info));
    tocFormat.setProperty(KoParagraphStyle::GeneratedDocument, QVariant::fromValue<QTextDocument *>(tocDocument));

    deleteTextShape();

    m_textShape = std::make_unique<TextShape>(&m_itom, &m_tlm);
    if (m_previewPixSize.isEmpty()) {
        m_textShape->setSize(size());
    } else {
        m_textShape->setSize(m_previewPixSize);
    }
    QTextCursor cursor(m_textShape->textShapeData()->document());

    QTextCharFormat textCharFormat = cursor.blockCharFormat();
    textCharFormat.setFontPointSize(11);
    textCharFormat.setFontWeight(QFont::Normal);

    // the brush is set to the background colour so that the actual text block(Heading 1,Heading 1.1 etc.) does not appear in the preview
    textCharFormat.setProperty(QTextCharFormat::ForegroundBrush, QBrush(Qt::white));
    cursor.setCharFormat(textCharFormat);

    cursor.insertBlock(tocFormat);
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

    // insert text for different heading styles
    QTextBlockFormat blockFormat;
    blockFormat.setProperty(KoParagraphStyle::OutlineLevel, 1);
    cursor.insertBlock(blockFormat, textCharFormat);
    cursor.insertText("Header 1");

    QTextBlockFormat blockFormat1;
    blockFormat1.setProperty(KoParagraphStyle::OutlineLevel, 2);
    cursor.insertBlock(blockFormat1, textCharFormat);
    cursor.insertText("Header 1.1");

    QTextBlockFormat blockFormat2;
    blockFormat2.setProperty(KoParagraphStyle::OutlineLevel, 2);
    cursor.insertBlock(blockFormat2, textCharFormat);
    cursor.insertText("Header 1.2");

    QTextBlockFormat blockFormat3;
    blockFormat3.setProperty(KoParagraphStyle::OutlineLevel, 1);
    cursor.insertBlock(blockFormat3, textCharFormat);
    cursor.insertText("Header 2");

    KoTextDocument(m_textShape->textShapeData()->document()).setStyleManager(m_styleManager);

    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout *>(m_textShape->textShapeData()->document()->documentLayout());
    connect(lay, &KoTextDocumentLayout::finishedLayout, this, &TableOfContentsPreview::finishedPreviewLayout);
    if (lay) {
        lay->layout();
    }
}

void TableOfContentsPreview::finishedPreviewLayout()
{
    if (m_previewPixSize.isEmpty()) {
        m_pm = std::make_unique<QPixmap>(size());
    } else {
        m_pm = std::make_unique<QPixmap>(m_previewPixSize);
    }
    m_pm->fill(Qt::white);
    m_zoomHandler.setZoom(0.9);
    m_zoomHandler.setDpi(72, 72);
    QPainter p(m_pm.get());

    if (m_textShape) {
        if (m_previewPixSize.isEmpty()) {
            m_textShape->setSize(size());
        } else {
            m_textShape->setSize(m_previewPixSize);
        }
        KoShapePaintingContext paintContext; // FIXME
        m_textShape->paintComponent(p, m_zoomHandler, paintContext);
    }
    Q_EMIT pixmapGenerated(*m_pm);
    update();
}

QPixmap TableOfContentsPreview::previewPixmap()
{
    if (m_pm) {
        return QPixmap(*m_pm);
    } else {
        return {};
    }
}

void TableOfContentsPreview::deleteTextShape()
{
    if (m_textShape) {
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout *>(m_textShape->textShapeData()->document()->documentLayout());
        if (lay) {
            lay->setContinuousLayout(false);
            lay->setBlockLayout(true);
        }
        m_textShape.reset(nullptr);
    }
}

void TableOfContentsPreview::setPreviewSize(const QSize &size)
{
    m_previewPixSize = size;
}
