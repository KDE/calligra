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

#include "footnote.h"
#include "kword_doc.h"
#include "fc.h"
#include "kword_frame.h"
#include "parag.h"
#include "char.h"
#include "defs.h"
#include "kword_utils.h"

#include <klocale.h>

#include <kdebug.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

/******************************************************************/
/* Class: KWFootNoteManager                                       */
/******************************************************************/

/*================================================================*/
KWFootNoteManager::KWFootNoteManager( KWordDocument *_doc )
    : start( 1 ), superscript( true ), firstParag()
{
    noteType = EndNotes;
    doc = _doc;
}

/*================================================================*/
void KWFootNoteManager::recalc()
{
    KWFootNote *fn = 0L;
    int curr = start;

    for ( fn = footNotes.first(); fn; fn = footNotes.next() ) {
        fn->updateDescription( curr );
        curr = fn->setStart( curr ) + 1;
    }
    for ( fn = footNotes.first(); fn; fn = footNotes.next() )
        fn->makeTempNames();
    for ( fn = footNotes.first(); fn; fn = footNotes.next() )
        fn->updateNames();
}

/*================================================================*/
int KWFootNoteManager::findStart( KWFormatContext *_fc )
{
    if ( _fc->getFrameSet() > 1 )
        return -1;

    if ( footNotes.isEmpty() )
        return start;

    KWFormatContext fc( doc, _fc->getFrameSet() );
    fc.init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( _fc->getFrameSet() - 1 ) )->getFirstParag() );
    int curr = start;
    KWParag *parag = fc.getParag();
    unsigned int found = 0;

    while ( parag != _fc->getParag() ) {
        KWString *str = parag->getKWString();
        for ( unsigned int i = 0; i < str->size(); i++ ) {
            if ( found == footNotes.count() )
                return curr;
            if ( str->data()[ i ].attrib->getClassId() == ID_KWCharFootNote ) {
                curr = dynamic_cast<KWCharFootNote*>( str->data()[ i ].attrib )->getFootNote()->getEnd() + 1;
                found++;
            }
        }

        parag = parag->getNext();
    }

    if ( found == footNotes.count() )
        return curr;

    if ( parag ) {
        KWString *str = parag->getKWString();
        for ( unsigned int i = 0; i < str->size() && i <= _fc->getTextPos(); i++ ) {
            if ( found == footNotes.count() )
                return curr;
            if ( str->data()[ i ].attrib->getClassId() == ID_KWCharFootNote ) {
                curr = dynamic_cast<KWCharFootNote*>( str->data()[ i ].attrib )->getFootNote()->getEnd() + 1;
                found++;
            }
        }
    }

    return curr;
}

/*================================================================*/
void KWFootNoteManager::insertFootNote( KWFootNote *fn )
{
    if ( fn->getStart() == 1 ) {
        footNotes.insert( 0, fn );
        recalc();
        addFootNoteText( fn );
        return;
    }

    int i = 1;
    KWFootNote *_fn = 0L;
    for ( _fn = footNotes.first(); _fn; _fn = footNotes.next(), i++ ) {
        if ( _fn->getEnd() != -1 && _fn->getEnd() == fn->getStart() - 1 || _fn->getStart() == fn->getStart() - 1 ) {
            footNotes.insert( i, fn );
            break;
        }
    }

    recalc();
    addFootNoteText( fn );
}

/*================================================================*/
void KWFootNoteManager::removeFootNote( KWFootNote *fn )
{
    int n = footNotes.findRef( fn );
    if ( n != -1 ) {
        if ( n == 0 ) {
            if ( footNotes.count() > 1 ) {
                firstParag = footNotes.at( 1 )->getParag();
                KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

                while ( p && p->getParagName() != firstParag )
                    p = p->getPrev();

                if ( p )
                    p->setHardBreak( true );
            }
            else
                firstParag = QString::null;
        }

        fn->destroy();
        footNotes.take( n );
    }
    recalc();
}

/*================================================================*/
void KWFootNoteManager::addFootNoteText( KWFootNote *fn )
{
    bool hardBreak = false;

    if ( firstParag.isEmpty() )
        hardBreak = true;

    KWTextFrameSet *frameSet = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) );
    KWParag *parag = frameSet->getLastParag();
    KWParag *next = 0, *op = parag;

    if ( !firstParag.isEmpty() ) {
        while ( parag && parag->getParagName() != firstParag )
            parag = parag->getPrev();

        int i = start;
        while ( parag && i < fn->getStart() - 1 ) {
            parag = parag->getNext();
            i++;
        }

        if ( parag )
            next = parag->getNext();
        else
            parag = op;
    }

    KWParag *parag2 = new KWParag( frameSet, doc, parag, next, doc->findParagLayout( "Standard" ) );
    parag2->setHardBreak( hardBreak );
    QString paragName;
    paragName.sprintf( "Footnote/Endnote_%d", fn->getStart() );
    parag2->setParagName( paragName );
    QString txt = fn->getText();
    txt += " ";
    parag2->insertText( 0, txt );
    KWFormat format( doc );
    format.setDefaults( doc );
    parag2->setFormat( 0, fn->getText().length() + 1, format );
    parag2->setInfo( KWParag::PI_FOOTNOTE );

    fn->setParag( parag2 );

    if ( firstParag.isEmpty() )
        firstParag = parag2->getParagName();

}

/*================================================================*/
void KWFootNoteManager::save( QTextStream&out )
{
    out << indent << "<START value=\"" << start << "\"/>" << endl;
    out << indent << "<FORMAT superscript=\"" << superscript
        << "\" type=\"" << static_cast<int>( noteType ) << "\"/>" << endl;
    out << indent << "<FIRSTPARAG ref=\"" << correctQString( firstParag ).latin1() << "\"/>" << endl;
}

/*================================================================*/
void KWFootNoteManager::load( KOMLParser &parser, QValueList<KOMLAttrib> &lst )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        if ( name == "START" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    start = ( *it ).m_strValue.toInt();
            }
        } else if ( name == "FORMAT" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "superscript" )
                    superscript = static_cast<bool>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "type" )
                    noteType = static_cast<NoteType>( ( *it ).m_strValue.toInt() );
            }
        } else if ( name == "FIRSTPARAG" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "ref" )
                    firstParag = correctQString( ( *it ).m_strValue );
            }
        }

        else
            kdError(32001) << "Unknown tag '" << tag << "' in FOOTNOTEMGR" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
}

/******************************************************************/
/* Class: KWFootNote                                              */
/******************************************************************/

/*================================================================*/
KWFootNote::KWFootNote( KWordDocument *_doc, QList<KWFootNoteInternal> *_parts )
    : start( 1 ), end( 1 )
{
    doc = _doc;
    parts = *_parts;
    makeText();


    if ( parts.isEmpty() )
        kdWarning() << i18n( "WARNING: Empty footnote/endnote inserted!" ) << endl;
    else {
        start = parts.first()->from;
        end = parts.last()->to == -1 ? parts.last()->from : parts.last()->to;
    }
}

/*================================================================*/
int KWFootNote::setStart( int _start )
{
    if ( parts.isEmpty() ) return _start;

    int diff = _start - parts.first()->from;

    KWFootNoteInternal *fn = 0L;
    for ( fn = parts.first(); fn; fn = parts.next() ) {
        fn->from += diff;
        if ( fn->to != -1 ) fn->to += diff;
    }

    makeText();

    start = parts.first()->from;
    end = parts.last()->to == -1 ? parts.last()->from : parts.last()->to;

    return end;
}

/*================================================================*/
KWFootNote *KWFootNote::copy()
{
    KWFootNote *fn = new KWFootNote( doc, new QList<KWFootNoteInternal>( parts ) );
    fn->start = start;
    fn->end = end;
    fn->before = before;
    fn->after = after;
    fn->text = text;
    fn->parag = parag;

    KWFootNoteManager &m = doc->getFootNoteManager();
    int i = m.footNotes.findRef( this );
    if ( i != -1 ) {
        m.footNotes.take( i );
        m.footNotes.insert( i, fn );
    }

    return fn;
}

/*================================================================*/
void KWFootNote::makeText()
{
    text = before;

    KWFootNoteInternal *fn = 0L;
    for ( fn = parts.first(); fn; fn = parts.next() ) {
        text += QString().setNum( fn->from );
        if ( fn->to != -1 ) {
            text += fn->space;
            text += QString().setNum( fn->to );
        }
        if ( fn != parts.last() )
            text += ", ";
    }

    text += after;
}

/*================================================================*/
void KWFootNote::setParag( KWParag *_parag )
{
    parag = _parag->getParagName();
}

/*================================================================*/
void KWFootNote::updateDescription( int _start )
{
    if ( parag.isEmpty() )
        return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
        p = p->getPrev();

    if ( p ) {
        p->deleteText( 0, text.length() );
        setStart( _start );

        p->insertText( 0, text );
        KWFormat format( doc );
        format.setDefaults( doc );
        p->setFormat( 0, text.length(), format );
    }
    else
        kdWarning() << i18n( "Footnote couldn't find the parag with the footnote description" ) << endl;
}

/*================================================================*/
void KWFootNote::makeTempNames()
{
    if ( parag.isEmpty() )
        return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
        p = p->getPrev();

    if ( p ) {
        parag.prepend( "_" );
        p->setParagName( parag );
    }
    else
        kdWarning() << i18n( "Footnote couldn't find the parag with the footnote description" ) << endl;

}

/*================================================================*/
void KWFootNote::updateNames()
{
    if ( parag.isEmpty() )
        return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
        p = p->getPrev();

    if ( p ) {
        parag.sprintf( "Footnote/Endnote_%d", start );
        p->setParagName( parag );
    }
    else
        kdWarning() << i18n( "Footnote couldn't find the parag with the footnote description" ) << endl;

}

/*================================================================*/
void KWFootNote::destroy()
{
    if ( parag.isEmpty() )
        return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
        p = p->getPrev();

    if ( p ) {
        KWParag *prev = p->getPrev();
        KWParag *next = p->getNext();

        if ( prev )
            prev->setNext( next );
        if ( next )
            next->setPrev( prev );

        delete p;
    }
    else
        kdWarning() << i18n( "Footnote couldn't find the parag with the footnote description" ) << endl;
}

/*================================================================*/
void KWFootNote::save( QTextStream&out )
{
    out << otag << "<INTERNAL>" << endl;
    KWFootNoteInternal *fi = 0L;
    for ( fi = parts.first(); fi; fi = parts.next() )
        out << indent << "<PART from=\"" << fi->from << "\" to=\"" << fi->to
            << "\" space=\"" << correctQString( fi->space ).latin1() << "\"/>" << endl;
    out << etag << "</INTERNAL>" << endl;
    out << indent << "<RANGE start=\"" << start << "\" end=\"" << end << "\"/>" << endl;
    out << indent << "<TEXT before=\"" << correctQString( before ).latin1() << "\" after=\"" << correctQString( after ).latin1() << "\"/>" << endl;
    out << indent << "<DESCRIPT ref=\"" << correctQString( parag ).latin1() << "\"/>" << endl;
}

/*================================================================*/
void KWFootNote::load( QString name, QString tag, KOMLParser &parser, QValueList<KOMLAttrib>& lst )
{
    if ( name == "INTERNAL" ) {
        parser.parseTag( tag, name, lst );
        //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        //for( ; it != lst.end(); ++it ) {
        //}

        while ( parser.open( QString::null, tag ) ) {
            parser.parseTag( tag, name, lst );
            if ( name == "PART" ) {
                parser.parseTag( tag, name, lst );
                QValueList<KOMLAttrib>::ConstIterator it = lst.begin();

                KWFootNoteInternal *part = new KWFootNoteInternal;

                for( ; it != lst.end(); ++it ) {
                    if ( ( *it ).m_strName == "from" )
                        part->from = ( *it ).m_strValue.toInt();
                    else if ( ( *it ).m_strName == "to" )
                        part->to = ( *it ).m_strValue.toInt();
                    else if ( ( *it ).m_strName == "space" )
                        part->space = correctQString( ( *it ).m_strValue );
                }
                parts.append( part );
            } else
                kdError(32001) << "Unknown tag '" << tag << "' in INTERNAL" << endl;

            if ( !parser.close( tag ) ) {
                kdError(32001) << "Closing " << tag << endl;
                return;
            }
        }
    } else if ( name == "RANGE" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for( ; it != lst.end(); ++it )
        {
            if ( ( *it ).m_strName == "start" )
                start = ( *it ).m_strValue.toInt();
            else if ( ( *it ).m_strName == "end" )
                end = ( *it ).m_strValue.toInt();
        }
    } else if ( name == "TEXT" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for( ; it != lst.end(); ++it )
        {
            if ( ( *it ).m_strName == "before" )
                before = correctQString( ( *it ).m_strValue );
            else if ( ( *it ).m_strName == "after" )
                after = correctQString( ( *it ).m_strValue );
        }
    } else if ( name == "DESCRIPT" ) {
        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for( ; it != lst.end(); ++it )
        {
            if ( ( *it ).m_strName == "ref" )
                parag = correctQString( ( *it ).m_strValue );
        }
    }

    makeText();
}
