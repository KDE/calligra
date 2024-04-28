// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_USERPROPSDESTINATION_H
#define RTFREADER_USERPROPSDESTINATION_H

#include <QColor>
#include <QString>
#include <QVariant>

#include "Destination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT UserPropsDestination : public Destination
{
public:
    UserPropsDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~UserPropsDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;

private:
    bool m_nextPlainTextIsPropertyName;
    QVariant::Type m_propertyType;
    QString m_propertyName;
};
}

#endif
