// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "HLinkBasePcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
HLinkBasePcdataDestination::HLinkBasePcdataDestination(AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(output, name)
{
}

HLinkBasePcdataDestination::~HLinkBasePcdataDestination() = default;

void HLinkBasePcdataDestination::aboutToEndDestination()
{
    m_output->setHLinkBase(m_pcdata);
}
}
