/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Footnotes / Endnotes					  */
/******************************************************************/

#include "footnote.h"
#include "kword_doc.h"
#include "fc.h"
#include "frame.h"
#include "parag.h"
#include "char.h"
#include "defs.h"
#include "kword_utils.h"

#include <klocale.h>
#include <unistd.h>

/******************************************************************/
/* Class: KWFootNoteManager					  */
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

    for ( fn = footNotes.first(); fn; fn = footNotes.next() )
    {
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

    while ( parag != _fc->getParag() )
    {
	KWString *str = parag->getKWString();
	for ( unsigned int i = 0; i < str->size(); i++ )
	{
	    if ( found == footNotes.count() )
		return curr;
	    if ( str->data()[ i ].attrib->getClassId() == ID_KWCharFootNote )
	    {
		curr = dynamic_cast<KWCharFootNote*>( str->data()[ i ].attrib )->getFootNote()->getEnd() + 1;
		found++;
	    }
	}

	parag = parag->getNext();
    }

    if ( found == footNotes.count() )
	return curr;

    if ( parag )
    {
	KWString *str = parag->getKWString();
	for ( unsigned int i = 0; i < str->size() && i <= _fc->getTextPos(); i++ )
	{
	    if ( found == footNotes.count() )
		return curr;
	    if ( str->data()[ i ].attrib->getClassId() == ID_KWCharFootNote )
	    {
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
    if ( fn->getStart() == 1 )
    {
	footNotes.insert( 0, fn );
	recalc();
	addFootNoteText( fn );
	return;
    }

    int i = 1;
    KWFootNote *_fn = 0L;
    for ( _fn = footNotes.first(); _fn; _fn = footNotes.next(), i++ )
    {
	if ( _fn->getEnd() != -1 && _fn->getEnd() == fn->getStart() - 1 || _fn->getStart() == fn->getStart() - 1 )
	{
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
    if ( n != -1 )
    {
	if ( n == 0 )
	{
	    if ( footNotes.count() > 1 )
	    {
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
    KWParag *next = 0L;

    if ( !firstParag.isEmpty() )
    {
	while ( parag && parag->getParagName() != firstParag )
	    parag = parag->getPrev();

	int i = start;
	while ( parag && i < fn->getStart() - 1 )
	{
	    parag = parag->getNext();
	    i++;
	}

	next = parag->getNext();
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
QDomElement KWFootNoteManager::save( QDomDocument& doc )
{
    QDomElement e = doc.createElement( "FOOTNODE-GLOBAL" );

    QDomElement s = doc.createElement( "START" );
    e.appendChild( s );
    s.setAttribute( "value", start );

    QDomElement format = doc.createElement( "FORMAT" );
    e.appendChild( format );
    format.setAttribute( "superscript", superscript );
    format.setAttribute( "type", (int)noteType );

    QDomElement fp = doc.createElement( "FIRSTPARAG" );
    e.appendChild( fp );
    fp.setAttribute( "ref", firstParag );

    return e;
}

/*================================================================*/
bool KWFootNoteManager::load( const QDomElement &e )
{
    bool ok;

    QDomElement s = e.namedItem( "START" ).toElement();
    if ( s.isNull() )
	return false;
    start = s.attribute( "value" ).toInt( &ok );
    if ( !ok ) return false;

    s = e.namedItem( "FORMAT" ).toElement();
    if ( s.isNull() )
	return false;
    superscript = (bool)s.attribute( "superscript" ).toInt();
    noteType = (NoteType)s.attribute( "type" ).toInt();

    s = e.namedItem( "FIRSTPARAG" ).toElement();
    if ( s.isNull() )
	return false;
    firstParag = s.attribute( "ref" );

    return true;
}

/******************************************************************/
/* Class: KWFootNote						  */
/******************************************************************/

/*================================================================*/
KWFootNote::KWFootNote( KWordDocument *_doc, QList<KWFootNoteInternal> *_parts )
    : start( 1 ), end( 1 )
{
    doc = _doc;
    parts = *_parts;
    makeText();


    if ( parts.isEmpty() )
	warning( i18n( "WARNING: Empty footnote/endnote inserted!" ) );
    else
    {
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
    for ( fn = parts.first(); fn; fn = parts.next() )
    {
	fn->from += diff;
	if ( fn->to != -1 ) fn->to += diff;
    }

    makeText();

    start = parts.first()->from;
    end = parts.last()->to == -1 ? parts.last()->from : parts.last()->to;

    return end;
}

/*================================================================*/
void KWFootNote::makeText()
{
    text = before;

    KWFootNoteInternal *fn = 0L;
    for ( fn = parts.first(); fn; fn = parts.next() )
    {
	text += QString().setNum( fn->from );
	if ( fn->to != -1 )
	{
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

    if ( p )
    {
	p->deleteText( 0, text.length() );
	setStart( _start );

	p->insertText( 0, text );
	KWFormat format( doc );
	format.setDefaults( doc );
	p->setFormat( 0, text.length(), format );
    }
    else
	warning( i18n( "Footnote couldn't find the parag with the footnote description" ) );
}

/*================================================================*/
void KWFootNote::makeTempNames()
{
    if ( parag.isEmpty() )
	return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
	p = p->getPrev();

    if ( p )
    {
	parag.prepend( "_" );
	p->setParagName( parag );
    }
    else
	warning( i18n( "Footnote couldn't find the parag with the footnote description" ) );

}

/*================================================================*/
void KWFootNote::updateNames()
{
    if ( parag.isEmpty() )
	return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
	p = p->getPrev();

    if ( p )
    {
	parag.sprintf( "Footnote/Endnote_%d", start );
	p->setParagName( parag );
    }
    else
	warning( i18n( "Footnote couldn't find the parag with the footnote description" ) );

}

/*================================================================*/
void KWFootNote::destroy()
{
    if ( parag.isEmpty() )
	return;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( 0 ) )->getLastParag();

    while ( p && p->getParagName() != parag )
	p = p->getPrev();

    if ( p )
    {
	KWParag *prev = p->getPrev();
	KWParag *next = p->getNext();

	if ( prev )
	    prev->setNext( next );
	if ( next )
	    next->setPrev( prev );

	delete p;
    }
    else
	warning( i18n( "Footnote couldn't find the parag with the footnote description" ) );
}

/*================================================================*/
QDomElement KWFootNote::save( QDomDocument& doc )
{
    QDomElement fn = doc.createElement( "FOOTNOTE" );
    fn.setAttribute( "start", start );
    fn.setAttribute( "end", end );
    fn.setAttribute( "before", before );
    fn.setAttribute( "after", after );
    fn.setAttribute( "ref", parag );

    QDomElement in = doc.createElement( "INTERNAL" );
    fn.appendChild( in );
    KWFootNoteInternal *fi = 0L;
    for ( fi = parts.first(); fi; fi = parts.next() ) {
	QDomElement part = doc.createElement( "PART" );
	in.appendChild( part );
	part.setAttribute( "from", fi->from );
	part.setAttribute( "to", fi->to );
	part.setAttribute( "space", fi->space );
    }

    return fn;
}

/*================================================================*/
// #### todo
// void KWFootNote::load( string name, string tag, KOMLParser &parser, vector<KOMLAttrib>& lst )
// {
//     if ( name == "INTERNAL" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	}

// 	while ( parser.open( 0L, tag ) )
// 	{
// 	    KOMLParser::parseTag( tag.c_str(), name, lst );
// 	    if ( name == "PART" )
// 	    {
// 		KOMLParser::parseTag( tag.c_str(), name, lst );
// 		vector<KOMLAttrib>::const_iterator it = lst.begin();

// 		KWFootNoteInternal *part = new KWFootNoteInternal;

// 		for( ; it != lst.end(); it++ )
// 		{
// 		    if ( ( *it ).m_strName == "from" )
// 			part->from = atoi( ( *it ).m_strValue.c_str() );
// 		    else if ( ( *it ).m_strName == "to" )
// 			part->to = atoi( ( *it ).m_strValue.c_str() );
// 		    else if ( ( *it ).m_strName == "space" )
// 			part->space = correctQString( ( *it ).m_strValue.c_str() );
// 		}
// 		parts.append( part );
// 	    }
// 	    else
// 		cerr << "Unknown tag '" << tag << "' in INTERNAL" << endl;

// 	    if ( !parser.close( tag ) )
// 	    {
// 		cerr << "ERR: Closing Child" << endl;
// 		return;
// 	    }
// 	}
//     }
//     else if ( name == "RANGE" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "start" )
// 		start = atoi( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "end" )
// 		end = atoi( ( *it ).m_strValue.c_str() );
// 	}
//     }
//     else if ( name == "TEXT" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "before" )
// 		before = correctQString( ( *it ).m_strValue.c_str() );
// 	    else if ( ( *it ).m_strName == "after" )
// 		after = correctQString( ( *it ).m_strValue.c_str() );
// 	}
//     }
//     else if ( name == "DESCRIPT" )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );
// 	vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	for( ; it != lst.end(); it++ )
// 	{
// 	    if ( ( *it ).m_strName == "ref" )
// 		parag = correctQString( ( *it ).m_strValue.c_str() );
// 	}
//     }

//     makeText();
// }
