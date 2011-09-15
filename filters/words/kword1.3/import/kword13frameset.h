//Added by qt3to4:
#include <QTextStream>
/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _FILTER_WORDS_1_3__WORDSFRAMESET_H
#define _FILTER_WORDS_1_3__WORDSFRAMESET_H

class QString;
class QTextStream;

#include <QString>
#include <QMap>

#include "kword13paragraph.h"

class Words13Frameset
{
public:
    Words13Frameset(int frameType, int frameInfo, const QString& name);
    virtual ~Words13Frameset(void);
public:
    virtual bool addParagraph(const Words13Paragraph& para);
    virtual bool setKey(const QString& key);
    virtual void xmldump(QTextStream& iostream);
public:
    int m_numFrames; ///< Number of frames
    QMap<QString, QString> m_frameData; ///< Data of frames (### TODO: do better!)
protected:
    int m_frameType;
    int m_frameInfo;
    QString m_name; ///< Name of frameset
};

class WordsTextFrameset : public Words13Frameset
{
public:
    WordsTextFrameset(int frameType, int frameInfo, const QString& name);
    virtual ~WordsTextFrameset(void);
public:
    virtual bool addParagraph(const Words13Paragraph& para);
    virtual void xmldump(QTextStream& iostream);
public:
    Words13ParagraphGroup m_paragraphGroup;
};

class Words13PictureFrameset : public Words13Frameset
{
public:
    Words13PictureFrameset(int frameType, int frameInfo, const QString& name);
    virtual ~Words13PictureFrameset(void);
public:
    virtual bool setKey(const QString& key);
    virtual void xmldump(QTextStream& iostream);
public:
    QString m_pictureKey;
};

#endif // _FILTER_WORDS_1_3__WORDSFRAMESET_H
