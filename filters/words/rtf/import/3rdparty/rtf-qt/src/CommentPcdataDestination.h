// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_COMMENTPCDATADESTINATION_H
#define RTFREADER_COMMENTPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT CommentPcdataDestination : public PcdataDestination
{
public:
    CommentPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~CommentPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
