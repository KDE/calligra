// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_INFOPRINTEDTIMEDESTINATION_H
#define RTFREADER_INFOPRINTEDTIMEDESTINATION_H

#include <QColor>
#include <QString>

#include "InfoTimeDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT InfoPrintedTimeDestination : public InfoTimeDestination
{
public:
    InfoPrintedTimeDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~InfoPrintedTimeDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
