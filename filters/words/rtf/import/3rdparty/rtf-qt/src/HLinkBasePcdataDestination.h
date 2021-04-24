// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_HLINKBASEPCDATADESTINATION_H
#define RTFREADER_HLINKBASEPCDATADESTINATION_H

#include <QString>
#include <QColor>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
    class Reader;

    class RTFREADER_EXPORT HLinkBasePcdataDestination: public PcdataDestination
    {
      public:
	HLinkBasePcdataDestination( Reader *reader, AbstractRtfOutput *output, const QString &name );

	~HLinkBasePcdataDestination() override;

	void aboutToEndDestination() override;
      };
}

#endif
