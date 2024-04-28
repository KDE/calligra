// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_INFODESTINATION_H
#define RTFREADER_INFODESTINATION_H

#include <QColor>
#include <QString>

#include "Destination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT InfoDestination : public Destination
{
public:
    InfoDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~InfoDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;

private:
};
}

#endif
