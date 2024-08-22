// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "KeywordsPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
KeywordsPcdataDestination::KeywordsPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

KeywordsPcdataDestination::~KeywordsPcdataDestination() = default;

void KeywordsPcdataDestination::aboutToEndDestination()
{
    m_output->setKeywords(m_pcdata);
}
}
