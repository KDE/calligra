// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_AUTHORPCDATADESTINATION_H
#define RTFREADER_AUTHORPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT AuthorPcdataDestination : public PcdataDestination
{
public:
    AuthorPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~AuthorPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
