// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "SubjectPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
SubjectPcdataDestination::SubjectPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

SubjectPcdataDestination::~SubjectPcdataDestination() = default;

void SubjectPcdataDestination::aboutToEndDestination()
{
    m_output->setSubject(m_pcdata);
}
}
