// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_PCDATADESTINATION_H
#define RTFREADER_PCDATADESTINATION_H

#include <QColor>
#include <QString>

#include "Destination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT PcdataDestination : public Destination
{
public:
    PcdataDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~PcdataDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;
    void aboutToEndDestination() override = 0;

protected:
    QString m_pcdata;
};
}

#endif
