/*
    This file is part of the Calligra project, made within the KDE community.

    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef XFIGSTREAMLINEREADER_H
#define XFIGSTREAMLINEREADER_H

// Qt
#include <QString>
#include <QTextStream>

class QIODevice;

class XFigStreamLineReader
{
public:
    enum CommentReadModus { DropComments, TakeComment, CollectComments };

    explicit XFigStreamLineReader(QIODevice *device);
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
