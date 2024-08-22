// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "CompanyPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
CompanyPcdataDestination::CompanyPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

CompanyPcdataDestination::~CompanyPcdataDestination() = default;

void CompanyPcdataDestination::aboutToEndDestination()
{
    m_output->setCompanyName(m_pcdata);
}
}
