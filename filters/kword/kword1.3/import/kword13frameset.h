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

#ifndef _FILTER_KWORD_1_3__KWORDFRAMESET_H
#define _FILTER_KWORD_1_3__KWORDFRAMESET_H

class QString;
class QTextStream;

#include <qstring.h>
#include <qmap.h>

#include "kword13paragraph.h"

class KWord13Frameset
{
public:
    KWord13Frameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWord13Frameset( void );
public:
    virtual bool addParagraph(const KWord13Paragraph& para);
    virtual bool setKey( const QString& key );
    virtual void xmldump( QTextStream& iostream );
public:
    int m_numFrames; ///< Number of frames
    QMap<QString,QString> m_frameData; ///< Data of frames (### TODO: do better!)
protected:
    int m_frameType;
    int m_frameInfo;
    QString m_name; ///< Name of frameset
};

class KWordTextFrameset : public KWord13Frameset
{
public:
    KWordTextFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordTextFrameset( void );
public:
    virtual bool addParagraph(const KWord13Paragraph& para);
    virtual void xmldump( QTextStream& iostream );
public:
    KWord13ParagraphGroup m_paragraphGroup;
};

class KWord13PictureFrameset : public KWord13Frameset
{
public:
    KWord13PictureFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWord13PictureFrameset( void );
public:
    virtual bool setKey( const QString& key );
    virtual void xmldump( QTextStream& iostream );
public:
    QString m_pictureKey;
};

#endif // _FILTER_KWORD_1_3__KWORDFRAMESET_H
