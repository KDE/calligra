// SPDX-FileCopyrightText: 2008, 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "Destination.h"
#include "rtfdebug.h"

#include <QStack>

namespace RtfReader
{
Destination::Destination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : m_name(name)
    , m_reader(reader)
    , m_output(output)
{
}

Destination::~Destination() = default;

bool Destination::hasName(const QString &name)
{
    return m_name == name;
}

QString Destination::name() const
{
    return m_name;
}

void Destination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    Q_UNUSED(controlWord);
    Q_UNUSED(hasValue);
    Q_UNUSED(value);
}

void Destination::handlePlainText(const QByteArray &plainText)
{
    qCDebug(lcRtf) << "plain text:" << plainText << "in" << m_name;
};

void Destination::aboutToEndDestination(){};
}
