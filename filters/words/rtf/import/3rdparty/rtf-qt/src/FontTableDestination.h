// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_FONTTABLEDESTINATION_H
#define RTFREADER_FONTTABLEDESTINATION_H

#include <QColor>
#include <QString>

#include "Destination.h"
#include "FontTableEntry.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT FontTableDestination : public Destination
{
public:
    FontTableDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~FontTableDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;
    void aboutToEndDestination() override;

protected:
    quint32 m_currentFontTableIndex;
    FontTableEntry m_fontTableEntry;
};
}

#endif
