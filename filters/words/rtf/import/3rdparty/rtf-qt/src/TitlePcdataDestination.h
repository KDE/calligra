// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_TITLEPCDATADESTINATION_H
#define RTFREADER_TITLEPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT TitlePcdataDestination : public PcdataDestination
{
public:
    TitlePcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~TitlePcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
