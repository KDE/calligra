/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kwtextdocument_h
#define kwtextdocument_h

#include "qrichtext_p.h"
using namespace Qt3;
class KWTextFrameSet;
class KWTextFormatCollection;

/**
 * This is our QTextDocument reimplementation, to create KWTextParag instead of QTextParags,
 * and to relate it to the text frameset it's in.
 */
class KWTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    KWTextDocument( KWTextFrameSet * textfs, QTextDocument *p, KWTextFormatCollection *fc );
    ~KWTextDocument();

    virtual QTextParag * createParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE );

    KWTextFrameSet * textFrameSet() const { return m_textfs; }

    // Used by ~KWTextParag to know if it should die quickly
    bool isDestroying() const { return m_bDestroying; }
private:
    KWTextFrameSet * m_textfs;
    bool m_bDestroying;
};

#endif
