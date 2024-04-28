// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_CATEGORYPCDATADESTINATION_H
#define RTFREADER_CATEGORYPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT CategoryPcdataDestination : public PcdataDestination
{
public:
    CategoryPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~CategoryPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
