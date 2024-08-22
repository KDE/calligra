// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "PictDestination.h"

#include "rtfdebug.h"
#include "rtfreader.h"
#include <QBuffer>
#include <QFile>
#include <QImageReader>
#include <QUuid>

namespace RtfReader
{
PictDestination::PictDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
{
}

PictDestination::~PictDestination() = default;

void PictDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    Q_UNUSED(hasValue);
    if (controlWord == "jpegblip") {
        qCDebug(lcRtf) << "JPEG";
        m_format = "jpg";
    } else if (controlWord == "pngblip") {
        qCDebug(lcRtf) << "PNG";
        m_format = "png";
    } else if (controlWord == "dibitmap") {
        qCDebug(lcRtf) << "BMP";
        m_format = "bmp";
    } else if (controlWord == "wmetafile") {
        qCDebug(lcRtf) << "todo: get WMF data";
    } else if (controlWord == "picw") {
        qCDebug(lcRtf) << "pict width: " << value;
    } else if (controlWord == "pich") {
        qCDebug(lcRtf) << "pict height: " << value;
    } else if (controlWord == "picscalex") {
        qCDebug(lcRtf) << "X scale: " << value;
        m_xScale = value / 100.;
    } else if (controlWord == "picscaley") {
        qCDebug(lcRtf) << "Y scale: " << value;
        m_yScale = value / 100.;
    } else if (controlWord == "piccropl") {
        qCDebug(lcRtf) << "Left crop:" << value;
    } else if (controlWord == "piccropr") {
        qCDebug(lcRtf) << "Right crop:" << value;
    } else if (controlWord == "piccropt") {
        qCDebug(lcRtf) << "Top crop:" << value;
    } else if (controlWord == "piccropb") {
        qCDebug(lcRtf) << "Bottom crop:" << value;
    } else if (controlWord == "pichgoal") {
        qCDebug(lcRtf) << "Goal Height:" << value;
        m_goalHeight = value * 96 / 1440.;
    } else if (controlWord == "picwgoal") {
        qCDebug(lcRtf) << "Goal Width:" << value;
        m_goalWidth = value * 96 / 1440.;
    } else {
        qCDebug(lcRtf) << "unexpected control word in pict:" << controlWord;
    }
}

void PictDestination::handlePlainText(const QByteArray &plainText)
{
    m_pictData += QByteArray::fromHex(plainText);
}

void PictDestination::aboutToEndDestination()
{
    if (m_format) {
        static int counter = 0;

        if (m_goalWidth == 0 || m_goalHeight == 0) {
            QBuffer buffer(&m_pictData);
            buffer.open(QIODevice::ReadOnly);
            QImageReader reader(&buffer);
            QSize size = reader.size();
            if (m_goalWidth == 0) {
                m_goalWidth = size.width();
            }
            if (m_goalHeight == 0) {
                m_goalHeight = size.height();
            }
        }

        m_imageFormat.setName(QStringLiteral("rtfparser://%1.%2").arg(++counter).arg(QString::fromUtf8(m_format)));
        m_imageFormat.setWidth(m_goalWidth * m_xScale);
        m_imageFormat.setHeight(m_goalHeight * m_yScale);

        m_output->createImage(m_pictData, m_imageFormat);
    } else {
        qCWarning(lcRtf) << "Embedded picture in unknown format";
    }
}
}
