// SPDX-FileCopyrightText: 2008, 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_DESTINATION_H
#define RTFREADER_DESTINATION_H

#include <QDebug>
#include <QString>
#include <QTextCharFormat>

#include "AbstractRtfOutput.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

/**
   A representation of a destination
*/
class RTFREADER_EXPORT Destination
{
public:
    // TODO: remove reader
    Destination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    virtual ~Destination();

    bool hasName(const QString &name);

    QString name() const;

    virtual void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value);

    virtual void handlePlainText(const QByteArray &plainText);

    virtual void aboutToEndDestination();

    // TODO: this doesn't belong here - remove it.
    QTextCharFormat m_textCharFormat;

protected:
    QString m_name;
    Reader *m_reader;
    AbstractRtfOutput *m_output;
};

QDebug operator<<(QDebug dbg, const Destination &dest);
}

#endif
