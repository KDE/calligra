// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "PcdataDestination.h"

#include "rtfreader.h"
#include "rtfdebug.h"

namespace RtfReader
{
    PcdataDestination::PcdataDestination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
      Destination( reader, output, name )
    {
    }

    PcdataDestination::~PcdataDestination()
    {
    }

    void PcdataDestination::handleControlWord( const QByteArray &controlWord, bool hasValue, const int value )
    {
        Q_UNUSED(hasValue);
        Q_UNUSED(value);
        qCDebug(lcRtf) << "unexpected control word in" << m_name << ": " << controlWord;
    }

    void PcdataDestination::handlePlainText( const QByteArray &plainText )
    {
	m_pcdata = plainText;
    }
}
