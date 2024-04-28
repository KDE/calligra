// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_MANAGERPCDATADESTINATION_H
#define RTFREADER_MANAGERPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT ManagerPcdataDestination : public PcdataDestination
{
public:
    ManagerPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~ManagerPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
