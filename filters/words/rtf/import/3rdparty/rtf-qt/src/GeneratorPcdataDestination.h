// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_GENERATORPCDATADESTINATION_H
#define RTFREADER_GENERATORPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT GeneratorPcdataDestination : public PcdataDestination
{
public:
    GeneratorPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~GeneratorPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
