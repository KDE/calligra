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

#ifndef footnote_h
#define footnote_h

#include <qlist.h>
#include <qpainter.h>

#include <koStream.h>

#include <qtextstream.h>

class KWFootNote;
class KWFormatContext;
class KWParag;
class KWordDocument;

/******************************************************************/
/* Class: KWFootNoteManager                                       */
/******************************************************************/

class KWFootNoteManager
{
public:
    enum NoteType {FootNotes, EndNotes};

    KWFootNoteManager( KWordDocument *_doc );

    int getStart() const { return start; }
    void setStart( int s ) { start = s; recalc(); }

    void recalc();

    int findStart( KWFormatContext *_fc );

    bool showFootNotesSuperscript() const { return superscript; }
    void setShowFootNotesSuperscript( bool _s ) { superscript = _s; }

    void insertFootNote( KWFootNote *fn );
    void removeFootNote( KWFootNote *fn );
    void insertFootNoteInternal( KWFootNote *fn )
    { footNotes.append( fn ); }

    NoteType getNoteType() const { return noteType; }
    void setNoteType( NoteType nt ) { noteType = nt; }

    void save( QTextStream&out );
    void load( KOMLParser&, QValueList<KOMLAttrib>& );

protected:
    void addFootNoteText( KWFootNote *fn );

    KWordDocument *doc;
    int start;
    QList<KWFootNote> footNotes;
    bool superscript;
    NoteType noteType;
    QString firstParag;

    friend class KWFootNote;

};

/******************************************************************/
/* Class: KWFootNote                                              */
/******************************************************************/

class KWFootNote
{
public:
    struct KWFootNoteInternal
    {
        int from;
        int to;
        QString space;
    };

    KWFootNote( KWordDocument *_doc, QList<KWFootNoteInternal> *_parts );

    KWFootNote *copy();

    int getStart() { return start; }
    int getEnd() { return end; }

    QString getText() { return text; }

    /**
     * returns new end
     */
    int setStart( int _start );

    void setBefore( const QString &_before ) { before = _before; }
    void setAfter( const QString &_after ) { after = _after; }

    void setParag( KWParag *_parag );
    QString getParag() { return parag; }

    void updateDescription( int _start );
    void makeTempNames();
    void updateNames();

    void destroy();

    void save( QTextStream&out );
    void load( QString name, QString tag, KOMLParser &parser, QValueList<KOMLAttrib>& lst );

protected:
    void makeText();

    KWordDocument *doc;
    int start, end;
    QList<KWFootNoteInternal> parts;
    QString before, after, text;
    QString parag;

};

#endif
