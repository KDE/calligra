// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "InfoRevisedTimeDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
InfoRevisedTimeDestination::InfoRevisedTimeDestination(AbstractRtfOutput *output, const QString &name)
    : InfoTimeDestination(output, name)
{
}

InfoRevisedTimeDestination::~InfoRevisedTimeDestination() = default;

void InfoRevisedTimeDestination::aboutToEndDestination()
{
    m_output->setRevisedDateTime(dateTime());
}
}
