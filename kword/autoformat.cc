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

#include "autoformat.h"
#include "kwdoc.h"
#include "kwtextparag.h"
#include "kwtextdocument.h"
#include "kwtextframeset.h"
#include <kdebug.h>
#include <klocale.h>
#include <kinstance.h>
#include <kconfig.h>
#include <qrichtext_p.h>

/******************************************************************/
/* Class: KWAutoFormat						  */
/******************************************************************/
KWAutoFormat::KWAutoFormat( KWDocument *_doc )
    : m_doc( _doc ), m_configRead( false ), m_typographicQuotes(), /*m_enabled( true ),*/
      m_convertUpperCase( false ), m_convertUpperUpper( false ),
      m_maxlen( 0 )
{
}

void KWAutoFormat::readConfig()
{
    // Read the autoformat configuration
    // This is done on demand (when typing the first char, or when opening the config dialog)
    // so that loading is faster and to avoid doing it for readonly documents.
    if ( m_configRead )
        return;
    KConfig * config = m_doc->instance()->config();
    KConfigGroupSaver cgs( config, "AutoFormat" );
    m_convertUpperCase = config->readBoolEntry( "ConvertUpperCase", false );
    m_convertUpperUpper = config->readBoolEntry( "ConvertUpperUpper", false );
    QString begin = config->readEntry( "TypographicQuotesBegin", "«" );
    m_typographicQuotes.begin = begin[0];
    QString end = config->readEntry( "TypographicQuotesEnd", "»" );
    m_typographicQuotes.end = end[0];
    m_typographicQuotes.replace = config->readBoolEntry( "TypographicQuotesEnabled", false )
                                  && !begin.isEmpty()
                                  && !end.isEmpty();

    ASSERT( m_entries.isEmpty() ); // readConfig is only called once...
    config->setGroup( "AutoFormatEntries" );

    QStringList find, replace;
    if ( config->hasKey( "Find" ) ) // Note that this allows saving an empty list and not getting the defaults
        find = config->readListEntry( "Find" );
    else
        find << "(C)" << "(c)" << "(R)" << "(r)";
    if ( config->hasKey( "Replace" ) )
        replace = config->readListEntry( "Replace" );
    else
        replace << "©" << "©" << "®" << "®";

    QStringList::Iterator fit = find.begin();
    QStringList::Iterator rit = replace.begin();
    for ( ; fit != find.end() && rit != replace.end() ; ++fit, ++rit )
    {
        m_entries.insert( ( *fit ), KWAutoFormatEntry( ( *rit ) ) );
    }
    buildMaxLen();

    m_configRead = true;
}

void KWAutoFormat::saveConfig()
{
    KConfig * config = m_doc->instance()->config();
    KConfigGroupSaver cgs( config, "AutoFormat" );
    config->writeEntry( "ConvertUpperCase", m_convertUpperCase );
    config->writeEntry( "ConvertUpperUpper", m_convertUpperUpper );
    config->writeEntry( "TypographicQuotesBegin", QString( m_typographicQuotes.begin ) );
    config->writeEntry( "TypographicQuotesEnd", QString( m_typographicQuotes.end ) );
    config->writeEntry( "TypographicQuotesEnabled", m_typographicQuotes.replace );
    config->setGroup( "AutoFormatEntries" );
    QStringList find, replace;
    KWAutoFormatEntryMap::Iterator it = m_entries.begin();
    for ( ; it != m_entries.end() ; ++it )
    {
        find.append( it.key() );
        replace.append( it.data().replace() );
    }
    config->writeEntry( "Find", find );
    config->writeEntry( "Replace", replace );
    config->sync();
}

void KWAutoFormat::doAutoFormat( QTextCursor* textEditCursor, KWTextParag *parag, int index, QChar ch )
{
    if ( !m_configRead )
        readConfig();

    //if ( !m_enabled )
    //    return;

    // Auto-correction happens when pressing space, tab, CR etc.
    if ( ch.isSpace() )
    {
        if ( index > 0 )
        {
            QString lastWord;
            QTextString *s = parag->string();
            for ( int i = index - 1; i >= 0; --i )
            {
                if ( s->at( i ).c.isSpace() )
                    break;
                lastWord.prepend( s->at( i ).c );
            }
            kdDebug() << "KWAutoFormat::doAutoFormat lastWord=" << lastWord << endl;
            if ( !doAutoCorrect( textEditCursor, parag, index, lastWord ) )
            {
                doUpperCase( textEditCursor, parag, index, lastWord );
                doSpellCheck( textEditCursor, parag, index, lastWord );
            }
        }
    }
    else if ( ch == '"' && m_typographicQuotes.replace )
    {
        doTypographicQuotes( textEditCursor, parag, index );
    }
}

bool KWAutoFormat::doAutoCorrect( QTextCursor* textEditCursor, KWTextParag *parag, int index, const QString & word )
{
    KWAutoFormatEntryMap::Iterator it = m_entries.find( word );
    if ( it != m_entries.end()  )
    {
        KWTextDocument * textdoc = parag->textDocument();
	unsigned int length = word.length();
        int start = index - length;
        QTextCursor cursor( parag->document() );
        cursor.setParag( parag );
        cursor.setIndex( start );
        textdoc->setSelectionStart( KWTextFrameSet::HighlightSelection, &cursor );
        cursor.setIndex( start + length );
        textdoc->setSelectionEnd( KWTextFrameSet::HighlightSelection, &cursor );

        KWTextFrameSet * textfs = textdoc->textFrameSet();
        textfs->replaceSelection( textEditCursor, it.data().replace(),
                                  KWTextFrameSet::HighlightSelection,
                                  i18n("Autocorrect word") );
        // The space/tab/CR that we inserted is still there but delete/insert moved the cursor
        // -> go right
        textfs->emitHideCursor();
        textEditCursor->gotoRight();
        textfs->emitShowCursor();
        return true;
    }
    return false;
}

void KWAutoFormat::doTypographicQuotes( QTextCursor* textEditCursor, KWTextParag *parag, int index )
{
    KWTextDocument * textdoc = parag->textDocument();
    QTextCursor cursor( parag->document() );
    cursor.setParag( parag );
    cursor.setIndex( index );
    textdoc->setSelectionStart( KWTextFrameSet::HighlightSelection, &cursor );
    cursor.setIndex( index + 1 );
    textdoc->setSelectionEnd( KWTextFrameSet::HighlightSelection, &cursor );

    // Need to determine if we want a starting or ending quote.
    // I see two solutions: either simply alternate, or depend on leading space.
    // MSWord does the latter afaics...
    QString replacement;
    if ( index > 0 && !parag->at( index - 1 )->c.isSpace() )
        replacement = m_typographicQuotes.end;
    else
        replacement = m_typographicQuotes.begin;

    KWTextFrameSet * textfs = textdoc->textFrameSet();
    textfs->replaceSelection( textEditCursor, replacement,
                              KWTextFrameSet::HighlightSelection,
                              i18n("Typographic quote") );
}

void KWAutoFormat::doUpperCase( QTextCursor *,KWTextParag */*parag*/, int /*index*/, const QString & /*word*/ )
{
#if 0
    bool converted = false;
    if ( m_convertUpperCase ) {
	if ( lastWasDotSpace &&
	     !isMark( parag->string()->data()[ fc->getTextPos() ].c ) &&
	     parag->string()->data()[ fc->getTextPos() ].c != QChar( ' ' ) &&
	     isLower( parag->string()->data()[ fc->getTextPos() ].c ) ) {
	    if ( parag->string()->data()[ fc->getTextPos() ].autoformat )
		delete parag->string()->data()[ fc->getTextPos() ].autoformat;

	    AutoformatInfo *info = new AutoformatInfo;
	    info->c = QChar( parag->string()->data()[ fc->getTextPos() ].c );
	    info->type = AT_UpperCase;

	    parag->string()->data()[ fc->getTextPos() ].autoformat = info;

	    parag->string()->data()[ fc->getTextPos() ].c
		= parag->string()->data()[ fc->getTextPos() ].c.upper();
	    converted = true;
	}
    } else if ( parag->string()->data()[ fc->getTextPos() ].autoformat &&
		parag->string()->data()[ fc->getTextPos() ].autoformat->type == AT_UpperCase ) {
	parag->string()->data()[ fc->getTextPos() ].c
	    = QChar( parag->string()->data()[ fc->getTextPos() ].autoformat->c );
	delete parag->string()->data()[ fc->getTextPos() ].autoformat;
	parag->string()->data()[ fc->getTextPos() ].autoformat = 0L;
    }

    if ( m_convertUpperUpper ) {
	if ( !lastWasDotSpace && lastWasUpper &&
	     isUpper( parag->string()->data()[ fc->getTextPos() ].c ) )
        {
	    if ( parag->string()->data()[ fc->getTextPos() ].autoformat )
		delete parag->string()->data()[ fc->getTextPos() ].autoformat;

	    AutoformatInfo *info = new AutoformatInfo;
	    info->c = QChar( parag->string()->data()[ fc->getTextPos() ].c );
	    info->type = AT_UpperUpper;

	    parag->string()->data()[ fc->getTextPos() ].autoformat = info;

	    parag->string()->data()[ fc->getTextPos() ].c
		= parag->string()->data()[ fc->getTextPos() ].c.lower();
	    converted = true;
	}
    } else if ( parag->string()->data()[ fc->getTextPos() ].autoformat &&
	      parag->string()->data()[ fc->getTextPos() ].autoformat->type
		== AT_UpperUpper ) {
	parag->string()->data()[ fc->getTextPos() ].c
	    = QChar( parag->string()->data()[ fc->getTextPos() ].autoformat->c );
	delete parag->string()->data()[ fc->getTextPos() ].autoformat;
	parag->string()->data()[ fc->getTextPos() ].autoformat = 0L;
    }

    if ( convertUpperUpper || convertUpperCase ) {
	if ( isMark( parag->string()->data()[ fc->getTextPos() ].c ) )
	    lastWasDotSpace = true;
	else if ( !isMark( parag->string()->data()[ fc->getTextPos() ].c ) &&
		  parag->string()->data()[ fc->getTextPos() ].c != QChar( ' ' ) )
	    lastWasDotSpace = false;
    }

    if ( convertUpperUpper ) {
	lastWasUpper = isUpper( parag->string()->data()[ fc->getTextPos() ].c );
    }
    return converted;
#endif
}

void KWAutoFormat::doSpellCheck( QTextCursor *,KWTextParag */*parag*/, int /*index*/, const QString & /*word*/ )
{
#if 0
    if ( !enabled || !doc->onLineSpellCheck() )
	return;
    if ( isSeparator( parag->string()->data()[ fc->getTextPos() ].c ) ) {
	if ( !spBuffer.isEmpty() && spBegin ) {
	    //qDebug( "spellcheck: %s", spBuffer.latin1() );
	    spBuffer = QString::null;
	    spBegin = 0;
	}
	return;
    }

    if ( spBuffer.isEmpty() )
	spBegin = &parag->string()->data()[ fc->getTextPos() ];
    spBuffer += parag->string()->data()[ fc->getTextPos() ].c;
#endif
}

void KWAutoFormat::configTypographicQuotes( TypographicQuotes _tq )
{
    m_typographicQuotes = _tq;
}

void KWAutoFormat::configUpperCase( bool _uc )
{
    m_convertUpperCase = _uc;
}

void KWAutoFormat::configUpperUpper( bool _uu )
{
    m_convertUpperUpper = _uu;
}

bool KWAutoFormat::isUpper( const QChar &c )
{
    QChar c2( c );
    c2 = c2.lower();

    if ( c2 != c )
	return true;
    else
	return false;
}

bool KWAutoFormat::isLower( const QChar &c )
{
    QChar c2( c );
    c2 = c2.upper();

    if ( c2 != c )
	return true;
    else
	return false;
}

bool KWAutoFormat::isMark( const QChar &c )
{
    return ( c == QChar( '.' ) ||
	     c == QChar( '?' ) ||
	     c == QChar( '!' ) );
}

bool KWAutoFormat::isSeparator( const QChar &c )
{
    return ( !c.isLetter() && !c.isNumber() && !c.isDigit() );
}

void KWAutoFormat::buildMaxLen()
{
    QMap< QString, KWAutoFormatEntry >::Iterator it = m_entries.begin();

    m_maxlen = 0;
    for ( ; it != m_entries.end(); ++it )
	m_maxlen = QMAX( m_maxlen, it.key().length() );
}
