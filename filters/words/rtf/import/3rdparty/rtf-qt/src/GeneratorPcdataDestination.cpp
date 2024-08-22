// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "GeneratorPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
GeneratorPcdataDestination::GeneratorPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

GeneratorPcdataDestination::~GeneratorPcdataDestination() = default;

void GeneratorPcdataDestination::aboutToEndDestination()
{
    if (m_pcdata.endsWith(QLatin1Char(';'))) {
        // trim off the trailing semicolon
        m_pcdata.chop(1);
        m_output->setGeneratorInformation(m_pcdata);
    }
}
}
