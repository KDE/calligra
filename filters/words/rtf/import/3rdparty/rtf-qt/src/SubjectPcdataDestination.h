// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_SUBJECTPCDATADESTINATION_H
#define RTFREADER_SUBJECTPCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT SubjectPcdataDestination : public PcdataDestination
{
public:
    SubjectPcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~SubjectPcdataDestination() override;

    void aboutToEndDestination() override;
};
}

#endif
