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

#ifndef KWORD_1_3_DOCUMENT_H
#define KWORD_1_3_DOCUMENT_H

class QIODevice;

#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>

#include "kword13frameset.h"

class KWord13Document
{
public:
    KWord13Document( void );
    ~KWord13Document( void );
    
public:
    void xmldump( QIODevice* io );

public:
    QMap<QString, QString> m_documentProperties;
    QValueList<KWord13Layout> m_styles;
    QPtrList<KWordTextFrameset> m_normalTextFramesetList; ///< List of \<FRAMESET\> having normal text
    QPtrList<KWordTextFrameset> m_tableFramesetList; ///< List of \<FRAMESET\> being part of tables
    QPtrList<KWordTextFrameset> m_headerFooterFramesetList; ///< List of \<FRAMESET\> having footer/header
    QPtrList<KWordTextFrameset> m_footEndNoteFramesetList; ///< List of \<FRAMESET\> having footnotes or endnotes
    QPtrList<KWordFrameset> m_otherFramesetList; ///< List of \<FRAMESET\> of other types
};

#endif // KWORD_1_3_DOCUMENT_H
