// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ColorTableDestination.h"

#include "rtfdebug.h"
#include "rtfreader.h"

namespace RtfReader
{
ColorTableDestination::ColorTableDestination(Reader *reader, AbstractRtfOutput *output, const QString &name)
    : Destination(reader, output, name)
    , m_currentColor(Qt::black)
    , m_colorSet(false)
{
}

ColorTableDestination::~ColorTableDestination() = default;

void ColorTableDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    Q_UNUSED(hasValue);
    bool handled = true;
    if (controlWord == "red") {
        m_currentColor.setRed(value);
    } else if (controlWord == "green") {
        m_currentColor.setGreen(value);
    } else if (controlWord == "blue") {
        m_currentColor.setBlue(value);
    } else {
        handled = false;
        qCDebug(lcRtf) << "unexpected control word in colortbl:" << controlWord;
    }
    if (handled) {
        m_colorSet = true;
    }
}

void ColorTableDestination::handlePlainText(const QByteArray &plainText)
{
    if (plainText == ";") {
        m_output->appendToColourTable(m_colorSet ? m_currentColor : QColor());
        resetCurrentColor();
    } else {
        qCDebug(lcRtf) << "unexpected text in ColorTableDestination:" << plainText;
    }
}

void ColorTableDestination::resetCurrentColor()
{
    m_currentColor = Qt::black;
    m_colorSet = false;
}
}
