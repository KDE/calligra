// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "CategoryPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
CategoryPcdataDestination::CategoryPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

CategoryPcdataDestination::~CategoryPcdataDestination() = default;

void CategoryPcdataDestination::aboutToEndDestination()
{
    m_output->setCategory(m_pcdata);
}
}
