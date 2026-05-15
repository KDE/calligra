// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DocumentCommentPcdataDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
DocumentCommentPcdataDestination::DocumentCommentPcdataDestination(AbstractRtfOutput *output, const QString &name)
    : PcdataDestination(output, name)
{
}

DocumentCommentPcdataDestination::~DocumentCommentPcdataDestination() = default;

void DocumentCommentPcdataDestination::aboutToEndDestination()
{
    m_output->setDocumentComment(m_pcdata);
}
}
