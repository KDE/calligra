/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kwdrag_h
#define kwdrag_h

#include <qdragobject.h>
#include <qstring.h>

/**
 * Drag object for a text selection
 * Contains the text as plain/text (without formatting) and as kword XML (with formatting)
 */
class KWTextDrag : public QTextDrag
{
    Q_OBJECT

public:
    KWTextDrag( QWidget *dragSource = 0L, const char *name = 0L );

    void setPlain( const QString &_plain ) { setText( _plain ); }
    void setKWord( const QCString &_kword ) { kword = _kword; }

    virtual QByteArray encodedData( const char *mime ) const;
    virtual const char* format( int i ) const;

    static bool canDecode( QMimeSource* e );

    static const char * selectionMimeType();

    void setFrameSetNumber( int number );

    static int decodeFrameSetNumber( QMimeSource *e );
    static bool provides( QMimeSource* e , const char* mimeType, const char* acceptMimeType, QString &returnedTypeMime);

protected:
    QCString kword;
    int m_framesetNumber;
};

#endif
