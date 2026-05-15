// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "InfoCreatedTimeDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
InfoCreatedTimeDestination::InfoCreatedTimeDestination(AbstractRtfOutput *output, const QString &name)
    : InfoTimeDestination(output, name)
{
}

InfoCreatedTimeDestination::~InfoCreatedTimeDestination() = default;

void InfoCreatedTimeDestination::aboutToEndDestination()
{
    m_output->setCreatedDateTime(dateTime());
}
}
