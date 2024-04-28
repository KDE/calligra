// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_RTFGROUPSTATE_H
#define RTFREADER_RTFGROUPSTATE_H
#include "rtfreader_export.h"
namespace RtfReader
{
class RTFREADER_EXPORT RtfGroupState
{
public:
    RtfGroupState()
        : didChangeDestination(false)
        , endOfFile(false)
    {
    }
    bool didChangeDestination;
    bool endOfFile;
};
}

#endif
