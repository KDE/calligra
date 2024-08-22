// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "InfoDestination.h"

#include "rtfdebug.h"
#include "rtfreader.h"

namespace RtfReader
{
InfoDestination::InfoDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
{
}

InfoDestination::~InfoDestination() = default;

void InfoDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    if ((controlWord == "edmins") && hasValue) {
        m_output->setTotalEditingTime(value);
    } else if ((controlWord == "nofpages") && hasValue) {
        m_output->setNumberOfPages(value);
    } else if ((controlWord == "nofwords") && hasValue) {
        m_output->setNumberOfWords(value);
    } else if ((controlWord == "nofchars") && hasValue) {
        m_output->setNumberOfCharacters(value);
    } else if ((controlWord == "nofcharsws") && hasValue) {
        m_output->setNumberOfCharactersWithoutSpaces(value);
    } else if ((controlWord == "version") && hasValue) {
        m_output->setVersionNumber(value);
    } else if ((controlWord == "vern") && hasValue) {
        m_output->setInternalVersionNumber(value);
    } else if (controlWord == "*") {
        // handled elsewhere
    } else {
        qCDebug(lcRtf) << "unexpected control word in InfoDestination:" << controlWord;
    }
}

void InfoDestination::handlePlainText(const QByteArray &plainText)
{
    qCDebug(lcRtf) << "unexpected text in InfoDestination:" << plainText;
}
}
