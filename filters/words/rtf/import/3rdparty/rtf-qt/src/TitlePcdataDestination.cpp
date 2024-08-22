// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TitlePcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
TitlePcdataDestination::TitlePcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

TitlePcdataDestination::~TitlePcdataDestination() = default;

void TitlePcdataDestination::aboutToEndDestination()
{
    m_output->setTitle(m_pcdata);
}
}
