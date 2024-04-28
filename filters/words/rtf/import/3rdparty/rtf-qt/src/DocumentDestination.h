// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_DOCUMENTDESTINATION_H
#define RTFREADER_DOCUMENTDESTINATION_H

#include <QColor>
#include <QString>

#include "Destination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT DocumentDestination : public Destination
{
public:
    DocumentDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~DocumentDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;
    void aboutToEndDestination() override;

protected:
    int m_charactersToSkip;
    int m_unicodeSkip;
};
}

#endif
