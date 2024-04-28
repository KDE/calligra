// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_STYLESHEETDESTINATION_H
#define RTFREADER_STYLESHEETDESTINATION_H

#include <QColor>
#include <QString>

#include "Destination.h"
#include "StyleSheetTableEntry.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT StyleSheetDestination : public Destination
{
public:
    StyleSheetDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~StyleSheetDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;
    void aboutToEndDestination() override;

protected:
    quint32 m_currentStyleHandleNumber;
    StyleSheetTableEntry m_style;
};
}

#endif
