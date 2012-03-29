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

#ifndef XFIGSTREAMLINEREADER_H
#define XFIGSTREAMLINEREADER_H

// Qt
#include <QTextStream>
#include <QString>

class QIODevice;


class XFigStreamLineReader
{
public:
    enum CommentReadModus {DropComments, TakeComment, CollectComments};

    explicit XFigStreamLineReader(QIODevice* device);
    ~XFigStreamLineReader();

    bool atEnd() const;
//     Error error() const;
    QString errorString() const;
    bool hasError() const;

    int objectCode() const;
    QString comment() const;
    QString line() const;

    bool readNextObjectLine();
    bool readNextLine(CommentReadModus commentModus = DropComments);

private:
    QIODevice* const m_Device;
    QTextStream m_TextStream;

    QString m_Comment;
    QString m_Line;
    int m_ObjectCode;

    bool m_HasError;
    QString m_ErrorString;
};


inline bool XFigStreamLineReader::hasError() const
{
    return m_HasError;
}
inline QString XFigStreamLineReader::errorString() const
{
    return m_ErrorString;
}
inline int XFigStreamLineReader::objectCode() const
{
    return m_ObjectCode;
}
inline QString XFigStreamLineReader::comment() const
{
    return m_Comment;
}
inline QString XFigStreamLineReader::line() const
{
    return m_Line;
}

#endif
