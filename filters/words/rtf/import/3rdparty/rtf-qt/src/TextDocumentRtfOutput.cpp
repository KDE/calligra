// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TextDocumentRtfOutput.h"
#include "rtfdebug.h"

#include <QDebug>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>
#include <QUrl>
#include <QUuid>

namespace RtfReader
{
TextDocumentRtfOutput::TextDocumentRtfOutput(QTextDocument *document)
    : AbstractRtfOutput()
    , m_haveSetFont(false)
    , m_document(document)
{
    m_cursor = new QTextCursor(m_document);
    QTextCharFormat defaultCharFormat;
    defaultCharFormat.setFontPointSize(12); // default of 24 "half-points"
    m_textCharFormatStack.push(defaultCharFormat);
}

TextDocumentRtfOutput::~TextDocumentRtfOutput()
{
    delete m_cursor;
}

void TextDocumentRtfOutput::startGroup()
{
    if (!m_haveSetFont) {
        // TODO: think harder about how to deal with default font cases.
        setFont(m_defaultFontIndex);
    }
    QTextCharFormat charFormat = m_textCharFormatStack.top(); // inherit all current properties
    m_textCharFormatStack.push(charFormat);
}

void TextDocumentRtfOutput::endGroup()
{
    m_textCharFormatStack.pop();
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::appendText(const QByteArray &text)
{
    static const QRegularExpression controlCharacters(QStringLiteral("[\\x00-\\x08\\x0B\\x0C\\x0E-\\x1F]"));
    m_cursor->insertText(
        (m_encoding != QStringConverter::Utf8 ? QStringDecoder(m_encoding).decode(text) : QString::fromLatin1(text)).remove(controlCharacters));
}

void TextDocumentRtfOutput::appendText(const QString &str)
{
    m_cursor->insertText(str);
}

void TextDocumentRtfOutput::insertPar()
{
    m_cursor->insertBlock();
}

void TextDocumentRtfOutput::insertTab()
{
    m_cursor->insertText("\t");
}

void TextDocumentRtfOutput::insertLeftQuote()
{
    m_cursor->insertText(QChar(0x2018));
}

void TextDocumentRtfOutput::insertRightQuote()
{
    m_cursor->insertText(QChar(0x2019));
}

void TextDocumentRtfOutput::insertLeftDoubleQuote()
{
    m_cursor->insertText(QChar(0x201c));
}

void TextDocumentRtfOutput::insertRightDoubleQuote()
{
    m_cursor->insertText(QChar(0x201d));
}

void TextDocumentRtfOutput::insertEnDash()
{
    m_cursor->insertText(QChar(0x2013));
}

void TextDocumentRtfOutput::insertEmDash()
{
    m_cursor->insertText(QChar(0x2014));
}

void TextDocumentRtfOutput::insertEmSpace()
{
    m_cursor->insertText(QChar(0x2003));
}

void TextDocumentRtfOutput::insertEnSpace()
{
    m_cursor->insertText(QChar(0x2002));
}

void TextDocumentRtfOutput::insertBullet()
{
    m_cursor->insertText(QChar(0x2022));
}

void TextDocumentRtfOutput::setFontItalic(const int value)
{
    m_textCharFormatStack.top().setFontItalic(value != 0);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setFontBold(const int value)
{
    int weight = QFont::Normal;
    if (value != 0) {
        weight = QFont::Bold;
    }
    m_textCharFormatStack.top().setFontWeight(weight);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setFontUnderline(const int value)
{
    m_textCharFormatStack.top().setFontUnderline(value != 0);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setFontStrikeout(const bool value)
{
    m_textCharFormatStack.top().setFontStrikeOut(value);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setFontPointSize(const int pointSize)
{
    m_textCharFormatStack.top().setFontPointSize(pointSize);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setForegroundColour(const int colourIndex)
{
    QColor colour = m_colourTable.value(colourIndex);
    if (colour.isValid()) {
        m_textCharFormatStack.top().setForeground(colour);
    } else {
        m_textCharFormatStack.top().clearForeground();
    }
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setHighlightColour(const int colourIndex)
{
    QColor colour = m_colourTable.value(colourIndex);
    if (colour.isValid()) {
        m_textCharFormatStack.top().setBackground(colour);
    } else {
        m_textCharFormatStack.top().clearBackground();
    }
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setParagraphPatternBackgroundColour(const int colourIndex)
{
    QColor colour = m_colourTable.value(colourIndex);
    if (colour.isValid()) {
        m_paragraphFormat.setBackground(colour);
    } else {
        m_paragraphFormat.clearBackground();
    }
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setFont(const int fontIndex)
{
    if (!m_fontTable.contains(fontIndex)) {
        qCDebug(lcRtf) << "attempted to select fontIndex" << fontIndex << "not in the font table";
        return;
    }
    FontTableEntry fontEntry = m_fontTable.value(fontIndex);
    qCDebug(lcRtf) << "selecting font:" << fontEntry.fontName();
    m_textCharFormatStack.top().setFontFamily(fontEntry.fontName());
    m_cursor->setCharFormat(m_textCharFormatStack.top());
    m_encoding = fontEntry.encoding();
    m_haveSetFont = true;
}

void TextDocumentRtfOutput::setDefaultFont(const int fontIndex)
{
    m_defaultFontIndex = fontIndex;
}

void TextDocumentRtfOutput::appendToColourTable(const QColor &colour)
{
    m_colourTable.append(colour);
}

void TextDocumentRtfOutput::insertFontTableEntry(FontTableEntry fontTableEntry, quint32 fontTableIndex)
{
    // qCDebug(lcRtf) << "inserting font entry:" << fontTableIndex << "with name:" << fontTableEntry.fontName();
    m_fontTable.insert(fontTableIndex, fontTableEntry);
}

void TextDocumentRtfOutput::insertStyleSheetTableEntry(quint32 stylesheetTableIndex, StyleSheetTableEntry stylesheetTableEntry)
{
    qCDebug(lcRtf) << "inserting stylesheet entry:" << stylesheetTableIndex << "with name:" << stylesheetTableEntry.styleName();
    m_stylesheetTable.insert(stylesheetTableIndex, stylesheetTableEntry);
}

void TextDocumentRtfOutput::resetParagraphFormat()
{
    m_paragraphFormat.setAlignment(Qt::AlignLeft);
    m_paragraphFormat.setTextIndent(0);
    m_paragraphFormat.setLeftMargin(0);
    m_paragraphFormat.setRightMargin(0);
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::resetCharacterProperties()
{
    m_textCharFormatStack.top().setFontPointSize(12); // default of 24 "half-points"
    m_textCharFormatStack.top().setFontWeight(QFont::Normal);
    m_textCharFormatStack.top().setFontItalic(false);
    m_textCharFormatStack.top().setFontUnderline(false);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setParagraphAlignmentLeft()
{
    m_paragraphFormat.setAlignment(Qt::AlignLeft);
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setParagraphAlignmentCentred()
{
    m_paragraphFormat.setAlignment(Qt::AlignHCenter);
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setParagraphAlignmentJustified()
{
    m_paragraphFormat.setAlignment(Qt::AlignJustify);
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setParagraphAlignmentRight()
{
    m_paragraphFormat.setAlignment(Qt::AlignRight);
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setLeftIndent(const int twips)
{
    m_paragraphFormat.setLeftMargin(pixelsFromTwips(twips));
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setRightIndent(const int twips)
{
    m_paragraphFormat.setRightMargin(pixelsFromTwips(twips));
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setSpaceBefore(const int twips)
{
    m_paragraphFormat.setTopMargin(pixelsFromTwips(twips));
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setSpaceAfter(const int twips)
{
    m_paragraphFormat.setBottomMargin(pixelsFromTwips(twips));
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setFirstLineIndent(const int twips)
{
    m_paragraphFormat.setTextIndent(pixelsFromTwips(twips));
    m_cursor->setBlockFormat(m_paragraphFormat);
}

void TextDocumentRtfOutput::setFontSuperscript()
{
    m_textCharFormatStack.top().setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setFontSubscript()
{
    m_textCharFormatStack.top().setVerticalAlignment(QTextCharFormat::AlignSubScript);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setTextDirectionLeftToRight()
{
    m_textCharFormatStack.top().setLayoutDirection(Qt::LeftToRight);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::setTextDirectionRightToLeft()
{
    m_textCharFormatStack.top().setLayoutDirection(Qt::RightToLeft);
    m_cursor->setCharFormat(m_textCharFormatStack.top());
}

void TextDocumentRtfOutput::createImage(const QByteArray &data, const QTextImageFormat &format)
{
    m_document->addResource(QTextDocument::ImageResource, QUrl(format.name()), data);
    m_cursor->insertImage(format);
}

void TextDocumentRtfOutput::setPageHeight(const int pageHeight)
{
    qCDebug(lcRtf) << "setPageHeight: " << pageHeight << " (" << pageHeight / 1440.0 << ")";
}

void TextDocumentRtfOutput::setPageWidth(const int pageWidth)
{
    qCDebug(lcRtf) << "setPageWidth: " << pageWidth << " (" << pageWidth / 1440.0 << ")";
}

qreal TextDocumentRtfOutput::pixelsFromTwips(const int twips)
{
    qreal inches = twips / 1440.0;
    qreal pixels = inches * 96.0;
    return pixels;
}
}
