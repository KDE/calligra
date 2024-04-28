// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_TEXTDOCUMENTRTFOUTPUT_H
#define RTFREADER_TEXTDOCUMENTRTFOUTPUT_H

#include "AbstractRtfOutput.h"

class QImage;
class QTextCursor;
class QTextDocument;
class QTextImageFormat;

#include "rtfreader_export.h"
#include <QStack>
#include <QTextCharFormat>
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT TextDocumentRtfOutput : public AbstractRtfOutput
{
public:
    explicit TextDocumentRtfOutput(QTextDocument *document);

    ~TextDocumentRtfOutput() override;

    void startGroup() override;

    void endGroup() override;

    void appendText(const QByteArray &text) override;
    void appendText(const QString &text) override;

    void insertPar() override;

    void insertTab() override;

    void insertLeftQuote() override;
    void insertRightQuote() override;
    void insertLeftDoubleQuote() override;
    void insertRightDoubleQuote() override;

    void insertEnDash() override;
    void insertEmDash() override;

    void insertEnSpace() override;
    void insertEmSpace() override;

    void insertBullet() override;

    void setFontItalic(const int value) override;

    void setFontBold(const int value) override;

    void setFontUnderline(const int value) override;

    void setFontStrikeout(const bool value) override;

    void setFontPointSize(const int pointSize) override;

    void setForegroundColour(const int colourIndex) override;
    void setHighlightColour(const int colourIndex) override;
    void setParagraphPatternBackgroundColour(const int colourIndex) override;

    void setFont(const int fontIndex) override;

    void setDefaultFont(const int fontIndex) override;

    void setFontSuperscript() override;
    void setFontSubscript() override;

    void setTextDirectionLeftToRight() override;
    void setTextDirectionRightToLeft() override;

    void appendToColourTable(const QColor &colour) override;

    void insertFontTableEntry(FontTableEntry fontTableEntry, quint32 fontTableIndex) override;
    void insertStyleSheetTableEntry(quint32 stylesheetTableIndex, StyleSheetTableEntry stylesheetTableEntry) override;

    void resetParagraphFormat() override;
    void resetCharacterProperties() override;

    void setParagraphAlignmentLeft() override;
    void setParagraphAlignmentCentred() override;
    void setParagraphAlignmentJustified() override;
    void setParagraphAlignmentRight() override;

    void setFirstLineIndent(const int twips) override;
    void setLeftIndent(const int twips) override;
    void setRightIndent(const int twips) override;

    void createImage(const QByteArray &data, const QTextImageFormat &format) override;

    void setPageHeight(const int pageHeight) override;
    void setPageWidth(const int pageWidth) override;

    void setSpaceBefore(const int twips) override;
    void setSpaceAfter(const int twips) override;

protected:
    // The text cursor on the document being generated
    QTextCursor *m_cursor;

    QStack<QTextCharFormat> m_textCharFormatStack;

    QTextBlockFormat m_paragraphFormat;

    QList<QColor> m_colourTable;

    QHash<int, FontTableEntry> m_fontTable;
    int m_defaultFontIndex;
    bool m_haveSetFont;

    QHash<int, StyleSheetTableEntry> m_stylesheetTable;

    QTextDocument *m_document;
    QStringConverter::Encoding m_encoding;

    /**
      Convenience routine to convert a size in twips into pixels
    */
    qreal pixelsFromTwips(const int twips);
};
}

#endif
