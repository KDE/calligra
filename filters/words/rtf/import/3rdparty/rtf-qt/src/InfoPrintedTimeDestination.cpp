// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "InfoPrintedTimeDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
InfoPrintedTimeDestination::InfoPrintedTimeDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : InfoTimeDestination(reader, output, name)
{
}

InfoPrintedTimeDestination::~InfoPrintedTimeDestination() = default;

void InfoPrintedTimeDestination::aboutToEndDestination()
{
    m_output->setPrintedDateTime(dateTime());
}
}
