//

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _FILTER_KWORD_1_3__KWORDFRAMESET_H
#define _FILTER_KWORD_1_3__KWORDFRAMESET_H

class QTextStream;

#include <qstring.h>
#include <qmap.h>

#include "kword13paragraph.h"

class KWordFrameset
{
public:
    KWordFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordFrameset( void );
public:
    virtual bool addParagraph(const KWordParagraph& para);
    virtual void xmldump( QTextStream& iostream );
public:
    int m_numFrames; ///< Number of frames
    QMap<QString,QString> m_frameData; ///< Data of frames (### TODO: do better!)
protected:
    int m_frameType;
    int m_frameInfo;
    QString m_name; ///< Name of frameset
};

class KWordTextFrameset : public KWordFrameset
{
public:
    KWordTextFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordTextFrameset( void );
public:
    virtual bool addParagraph(const KWordParagraph& para);
    virtual void xmldump( QTextStream& iostream );
public:
    KWordParagraphGroup m_paragraphGroup;
};

#endif // _FILTER_KWORD_1_3__KWORDFRAMESET_H
