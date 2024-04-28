// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_INFOCREATEDTIMEDESTINATION_H
#define RTFREADER_INFOCREATEDTIMEDESTINATION_H

#include <QColor>
#include <QString>

#include "InfoTimeDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT InfoCreatedTimeDestination : public InfoTimeDestination
{
public:
    InfoCreatedTimeDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~InfoCreatedTimeDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
