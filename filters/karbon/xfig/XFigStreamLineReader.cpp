/*
    This file is part of the Calligra project, made within the KDE community.

    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "XFigStreamLineReader.h"

// Qt
#include <QIODevice>

XFigStreamLineReader::XFigStreamLineReader(QIODevice *device)
    : m_TextStream(device)
    , m_ObjectCode(-1)
    , m_HasError((device == nullptr) || (!device->isReadable()))
{
}

XFigStreamLineReader::~XFigStreamLineReader() = default;

bool XFigStreamLineReader::atEnd() const
{
    return m_HasError || m_TextStream.atEnd();
}

bool XFigStreamLineReader::readNextObjectLine()
{
    if (m_HasError) {
        return false;
    }

    m_ObjectCode = 0;

    if (readNextLine(CollectComments)) {
        QTextStream textStream(&m_Line, QIODevice::ReadOnly);
        textStream >> m_ObjectCode;
        m_HasError = (textStream.status() != QTextStream::Ok);
        if (!m_HasError) {
            m_Line.remove(0, textStream.pos());
        }
    }

    return m_HasError;
}

bool XFigStreamLineReader::readNextLine(CommentReadModus commentModus)
{
    if (m_HasError) {
        return false;
    }

    m_Comment.clear();

    while (true) {
        if (m_TextStream.atEnd()) {
            m_HasError = true;
            break;
        }

        m_Line = m_TextStream.readLine();

        // skip empty lines
        if (m_Line.isEmpty()) {
            continue;
        }

        // skip (and collect) comment lines
        if (m_Line.startsWith(QLatin1Char('#'))) {
            if (commentModus == TakeComment) {
                break;
            } else if (commentModus == CollectComments) {
                m_Comment += m_Line.mid(1).trimmed() + QLatin1Char('\n');
            }
            continue;
        }

        break;
    }

    return (!m_HasError);
}
