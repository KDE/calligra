/*
    This file is part of the Calligra project, made within the KDE community.

    Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "XFigStreamLineReader.h"

// Qt
#include <QIODevice>


XFigStreamLineReader::XFigStreamLineReader(QIODevice* device)
  : m_Device(device)
  , m_TextStream(device)
  , m_ObjectCode(-1)
  , m_HasError((device==0)||(! device->isReadable()))
{
}

XFigStreamLineReader::~XFigStreamLineReader()
{
}

bool XFigStreamLineReader::atEnd() const
{
    return m_HasError ||
           m_TextStream.atEnd();
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
        if (! m_HasError) {
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

    return (! m_HasError);
}
