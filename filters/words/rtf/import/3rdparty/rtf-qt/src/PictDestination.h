// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_PICTDESTINATION_H
#define RTFREADER_PICTDESTINATION_H

#include <QColor>
#include <QString>

#include "Destination.h"
#include "rtfreader_export.h"
namespace RtfReader
{
class Reader;

class RTFREADER_EXPORT PictDestination : public Destination
{
public:
    PictDestination(Reader *reader, AbstractRtfOutput *output, const QString &name);

    ~PictDestination() override;

    void handleControlWord(const QByteArray &controlWord, bool hasValue, const int value) override;
    void handlePlainText(const QByteArray &plainText) override;
    void aboutToEndDestination() override;

private:
    // The hexadecimal version of the data for the image that is currently being built
    QByteArray m_pictData;

    // The format information for the current image
    QTextImageFormat m_imageFormat;
    const char *m_format = nullptr;
    qreal m_xScale = 1.;
    qreal m_yScale = 1.;
    qreal m_goalWidth = 0.;
    qreal m_goalHeight = 0.;
};
}

#endif
