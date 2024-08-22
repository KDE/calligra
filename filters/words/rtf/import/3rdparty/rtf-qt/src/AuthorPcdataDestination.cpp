// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "AuthorPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
AuthorPcdataDestination::AuthorPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

AuthorPcdataDestination::~AuthorPcdataDestination() = default;

void AuthorPcdataDestination::aboutToEndDestination()
{
    m_output->setAuthor(m_pcdata);
}
}
