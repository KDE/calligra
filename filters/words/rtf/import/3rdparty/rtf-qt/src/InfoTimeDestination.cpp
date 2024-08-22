// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "InfoTimeDestination.h"

#include "rtfdebug.h"
#include "rtfreader.h"

namespace RtfReader
{
InfoTimeDestination::InfoTimeDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
{
}

InfoTimeDestination::~InfoTimeDestination() = default;

void InfoTimeDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    Q_UNUSED(hasValue);
    if (controlWord == "yr") {
        m_year = value;
    } else if (controlWord == "mo") {
        m_month = value;
    } else if (controlWord == "dy") {
        m_day = value;
    } else if (controlWord == "hr") {
        m_hour = value;
    } else if (controlWord == "min") {
        m_minute = value;
    } else {
        qCDebug(lcRtf) << "unexpected control word in" << m_name << ": " << controlWord;
    }
}

void InfoTimeDestination::handlePlainText(const QByteArray &plainText)
{
    qCDebug(lcRtf) << "unexpected text in InfoTimeDestination:" << plainText;
}

QDateTime InfoTimeDestination::dateTime() const
{
    return QDateTime(QDate(m_year, m_month, m_day), QTime(m_hour, m_minute, 0));
}
}
