// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "IgnoredDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
IgnoredDestination::IgnoredDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
{
}

IgnoredDestination::~IgnoredDestination() = default;

void IgnoredDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    Q_UNUSED(controlWord);
    Q_UNUSED(hasValue);
    Q_UNUSED(value);
}

void IgnoredDestination::handlePlainText(const QByteArray &plainText)
{
    Q_UNUSED(plainText);
}

void IgnoredDestination::aboutToEndDestination()
{
}
}
