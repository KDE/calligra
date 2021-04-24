// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_COMPANYPCDATADESTINATION_H
#define RTFREADER_COMPANYPCDATADESTINATION_H

#include <QString>
#include <QColor>

#include "PcdataDestination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
    class Reader;

    class RTFREADER_EXPORT CompanyPcdataDestination: public PcdataDestination
    {
      public:
	CompanyPcdataDestination( Reader *reader, AbstractRtfOutput *output, const QString &name );

	~CompanyPcdataDestination() override;

	void aboutToEndDestination() override;
      };
}

#endif
