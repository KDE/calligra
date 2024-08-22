// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "OperatorPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
OperatorPcdataDestination::OperatorPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

OperatorPcdataDestination::~OperatorPcdataDestination() = default;

void OperatorPcdataDestination::aboutToEndDestination()
{
    m_output->setOperatorName(m_pcdata);
}
}
