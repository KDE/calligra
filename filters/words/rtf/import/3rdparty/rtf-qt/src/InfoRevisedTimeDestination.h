// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_INFOREVISEDTIMEDESTINATION_H
#define RTFREADER_INFOREVISEDTIMEDESTINATION_H

#include <QColor>
#include <QString>

#include "InfoTimeDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT InfoRevisedTimeDestination : public InfoTimeDestination
{
public:
    InfoRevisedTimeDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~InfoRevisedTimeDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
