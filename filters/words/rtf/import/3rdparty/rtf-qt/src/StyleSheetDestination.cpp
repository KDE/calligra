// SPDX-FileCopyrightText: 2010, 2011 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "StyleSheetDestination.h"

#include "controlword.h"
#include "rtfdebug.h"
#include "rtfreader.h"

namespace RtfReader
{

StyleSheetDestination::StyleSheetDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
{
    m_currentStyleHandleNumber = 0; /* default */
}

StyleSheetDestination::~StyleSheetDestination() = default;

void StyleSheetDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    if (controlWord == "ql") {
        m_style.setTextAlignment(LeftAligned);
    } else if ((controlWord == "li") && hasValue) {
        m_style.setLeftIndent(value);
    } else if ((controlWord == "ri") && hasValue) {
        m_style.setRightIndent(value);
    } else if (controlWord == "ltrch") {
        m_style.setLayoutDirection(Qt::LeftToRight);
    } else if (controlWord == "rtlch") {
        m_style.setLayoutDirection(Qt::RightToLeft);
    } else if ((controlWord == "s") && hasValue) {
        m_currentStyleHandleNumber = value;
    } else if ((controlWord == "sb") && hasValue) {
        m_style.setTopMargin(value);
    } else if (controlWord == "sb") {
        qCDebug(lcRtf) << "space before default (0)";
    } else {
        if (ControlWord::isDestination(controlWord)) {
            qCDebug(lcRtf) << "unhandled **Destination** control word in StyleSheetDestination:" << controlWord;
        } else {
            qCDebug(lcRtf) << "unhandled control word in StyleSheetDestination:" << controlWord;
        }
    }
}

void StyleSheetDestination::handlePlainText(const QByteArray &plainText)
{
    if (plainText == ";") {
        m_output->insertStyleSheetTableEntry(m_currentStyleHandleNumber, m_style);
    } else if (plainText.endsWith(";")) {
        // probably a style name with a terminating delimiter
        int delimiterPosition = plainText.indexOf(";");
        if (delimiterPosition == (plainText.length() - 1)) {
            // It is at the end, chop it off
            QString styleName = plainText.left(delimiterPosition);
            m_style.setStyleName(styleName);
            m_output->insertStyleSheetTableEntry(m_currentStyleHandleNumber, m_style);
        } else {
            // we were not expecting a name with a delimiter other than at the end
            qCDebug(lcRtf) << "Style name with embedded delimiter: " << plainText;
        }
    } else {
        // plain font name
        m_style.setStyleName(plainText);
    }
}

void StyleSheetDestination::aboutToEndDestination()
{
    // TODO
}
}
