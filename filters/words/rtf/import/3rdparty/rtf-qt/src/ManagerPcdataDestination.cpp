// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ManagerPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
ManagerPcdataDestination::ManagerPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

ManagerPcdataDestination::~ManagerPcdataDestination() = default;

void ManagerPcdataDestination::aboutToEndDestination()
{
    m_output->setManagerName(m_pcdata);
}
}
