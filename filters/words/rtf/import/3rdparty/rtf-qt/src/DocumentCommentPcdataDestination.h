// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_DOCUMENTCOMMENTPCDATADESTINATION_H
#define RTFREADER_DOCUMENTCOMMENTPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT DocumentCommentPcdataDestination : public PcdataDestination
{
public:
    DocumentCommentPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~DocumentCommentPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
