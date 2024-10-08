// SPDX-FileCopyrightText: 2010, 2011 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DocumentDestination.h"

#include "controlword.h"
#include "rtfdebug.h"
#include "rtfreader.h"

namespace RtfReader
{
DocumentDestination::DocumentDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
    , m_charactersToSkip(0)
    , m_unicodeSkip(1)
{
}

DocumentDestination::~DocumentDestination() = default;

void DocumentDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    if (controlWord == "par") {
        m_output->insertPar();
    } else if (controlWord == "tab") {
        m_output->insertTab();
    } else if (controlWord == "lquote") {
        m_output->insertLeftQuote();
    } else if (controlWord == "rquote") {
        m_output->insertRightQuote();
    } else if (controlWord == "ldblquote") {
        m_output->insertLeftDoubleQuote();
    } else if (controlWord == "rdblquote") {
        m_output->insertRightDoubleQuote();
    } else if (controlWord == "endash") {
        m_output->insertEnDash();
    } else if (controlWord == "emdash") {
        m_output->insertEmDash();
    } else if (controlWord == "enspace") {
        m_output->insertEnSpace();
    } else if (controlWord == "emspace") {
        m_output->insertEmSpace();
    } else if (controlWord == "bullet") {
        m_output->insertBullet();
    } else if (controlWord == "pard") {
        m_output->resetParagraphFormat();
    } else if (controlWord == "i") {
        m_output->setFontItalic(!hasValue || (hasValue && value != 0));
    } else if (controlWord == "b") {
        m_output->setFontBold(!hasValue || (hasValue && value != 0));
    } else if (controlWord == "ul") {
        m_output->setFontUnderline(!hasValue || (hasValue && value != 0));
    } else if (controlWord == "fs") {
        if (hasValue && (value != 0)) {
            m_output->setFontPointSize(value / 2.0);
        } else {
            m_output->setFontPointSize(24.0 / 2.0); // use default
        }
    } else if (controlWord == "cf") {
        m_output->setForegroundColour(value);
    } else if (controlWord == "cbpat") {
        m_output->setParagraphPatternBackgroundColour(value);
    } else if (controlWord == "highlight") {
        m_output->setHighlightColour(value);
    } else if ((controlWord == "deff") && hasValue) {
        m_output->setDefaultFont(value);
    } else if (controlWord == "f") {
        m_output->setFont(value);
    } else if (controlWord == "super") {
        m_output->setFontSuperscript();
    } else if (controlWord == "sub") {
        m_output->setFontSubscript();
    } else if (controlWord == "ql") {
        m_output->setParagraphAlignmentLeft();
    } else if (controlWord == "qj") {
        m_output->setParagraphAlignmentJustified();
    } else if (controlWord == "qc") {
        m_output->setParagraphAlignmentCentred();
    } else if (controlWord == "qr") {
        m_output->setParagraphAlignmentRight();
    } else if ((controlWord == "fi") && hasValue) {
        m_output->setFirstLineIndent(value);
    } else if ((controlWord == "li") && hasValue) {
        m_output->setLeftIndent(value);
    } else if ((controlWord == "ri") && hasValue) {
        m_output->setRightIndent(value);
    } else if ((controlWord == "sb") && hasValue) {
        m_output->setSpaceBefore(value);
    } else if (controlWord == "sb") {
        m_output->setSpaceAfter(0);
    } else if ((controlWord == "sa") && hasValue) {
        m_output->setSpaceAfter(value);
    } else if (controlWord == "sa") {
        m_output->setSpaceAfter(0);
    } else if (controlWord == "ltrch") {
        m_output->setTextDirectionLeftToRight();
    } else if (controlWord == "rtlch") {
        m_output->setTextDirectionRightToLeft();
    } else if (controlWord == "plain") {
        m_output->resetCharacterProperties();
    } else if (controlWord == "paperh") {
        m_output->setPageHeight(value);
    } else if (controlWord == "paperw") {
        m_output->setPageWidth(value);
    } else if (controlWord == "strike") {
        m_output->setFontStrikeout(!hasValue || (hasValue && value != 0));
    } else if ((controlWord == "u") && hasValue) {
        m_output->appendText(QString(1, QChar(value)));
        m_charactersToSkip = m_unicodeSkip;
    } else if (controlWord == "uc" && hasValue) {
        m_unicodeSkip = value;
    } else if ((controlWord == "\'") && hasValue) {
        qCDebug(lcRtf) << "special character value:" << value;
    } else if (controlWord == "line") {
        m_output->appendText(QString("\n"));
    } else if (controlWord == "*") {
        // handled elsewhere
    } else {
        if (ControlWord::isDestination(controlWord)) {
            qCDebug(lcRtf) << "unhandled *Destination* control word in DocumentDestination:" << controlWord;
        } else {
            qCDebug(lcRtf) << "unhandled control word in DocumentDestination:" << controlWord;
        }
    }
}

void DocumentDestination::handlePlainText(const QByteArray &plainText)
{
    if (m_charactersToSkip > 0) {
        qCDebug(lcRtf) << "skipping" << m_charactersToSkip << "of" << plainText;
        if (m_charactersToSkip >= plainText.size()) {
            m_charactersToSkip -= plainText.size();
            return;
        } else if (plainText.size() > m_charactersToSkip) {
            QByteArray partiallySkippedText(plainText);
            partiallySkippedText.remove(0, m_charactersToSkip);
            m_output->appendText(partiallySkippedText);
            m_charactersToSkip = 0;
            return;
        }
    }
    m_output->appendText(plainText);
}

void DocumentDestination::aboutToEndDestination()
{
    // TODO
}
}
