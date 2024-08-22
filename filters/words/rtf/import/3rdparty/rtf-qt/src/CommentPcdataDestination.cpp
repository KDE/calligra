// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "CommentPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
CommentPcdataDestination::CommentPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(reader, output, name)
{
}

CommentPcdataDestination::~CommentPcdataDestination() = default;

void CommentPcdataDestination::aboutToEndDestination()
{
    m_output->setComment(m_pcdata);
}
}
