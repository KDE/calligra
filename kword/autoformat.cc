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
      m_dontUpper(false),m_maxlen( 0 )
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

    if(config->hasKey( "UpperCaseExceptions" ) )
        upperCaseExceptions=config->readListEntry( "UpperCaseExceptions" );

    if(config->hasKey( "TwoUpperLetterExceptions"))
        twoUpperLetterException=config->readListEntry( "TwoUpperLetterExceptions" );
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

    config->writeEntry( "UpperCaseExceptions",upperCaseExceptions );

    config->writeEntry( "TwoUpperLetterExceptions",twoUpperLetterException);

    config->sync();
}

QString KWAutoFormat::getLastWord(KWTextParag *parag, int index)
{
    QString lastWord;
    QTextString *s = parag->string();
    for ( int i = index - 1; i >= 0; --i )
    {
        QChar ch = s->at( i ).c;
        if ( ch.isSpace() || ch.isPunct() )
            break;
        lastWord.prepend( ch );
    }
    return lastWord;
}

void KWAutoFormat::doAutoFormat( QTextCursor* textEditCursor, KWTextParag *parag, int index, QChar ch )
{
    if ( !m_configRead )
        readConfig();

    //if ( !m_enabled )
    //    return;
    if( ch==".")
    {
        QString text=getLastWord(parag,index);
        text=text+".";
        m_dontUpper=(upperCaseExceptions.findIndex(text)!=-1);
        kdDebug()<<"m_dontUpper :"<<m_dontUpper<<endl;
    }
    // Auto-correction happens when pressing space, tab, CR etc.
    if ( ch.isSpace() )
    {
        if ( index > 0 )
        {
            QString lastWord=getLastWord(parag, index);
            kdDebug() << "KWAutoFormat::doAutoFormat lastWord=" << lastWord << endl;
            if ( !doAutoCorrect( textEditCursor, parag, index, lastWord ) )
            {
                if ( m_convertUpperUpper || m_convertUpperCase )
                    doUpperCase( textEditCursor, parag, index, lastWord );
                // todo doSpellCheck( textEditCursor, parag, index, lastWord );
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

void KWAutoFormat::doUpperCase( QTextCursor *textEditCursor, KWTextParag *parag,
                                int index, const QString & word )
{
    KWTextDocument * textdoc = parag->textDocument();
    unsigned int length = word.length();
    int start = index - length;
    QTextCursor backCursor( parag->document() );
    backCursor.setParag( parag );
    backCursor.setIndex( start );

    // backCursor now points at the first char of the word
    QChar firstChar = backCursor.parag()->at( backCursor.index() )->c;
    bool bNeedMove = false;

    if ( m_convertUpperCase && isLower( firstChar ) && !m_dontUpper)
    {

        bool beginningOfSentence = true; // true if beginning of text
        // Go back over any space/tab/CR
        while ( backCursor.index() > 0 || backCursor.parag()->prev() )
        {
            beginningOfSentence = false; // we could go back -> false unless we'll find '.'
            backCursor.gotoLeft();
            if ( !backCursor.parag()->at( backCursor.index() )->c.isSpace() )
                break;
        }
        // We are now at the first non-space char before the word
        if ( !beginningOfSentence )
            beginningOfSentence = isMark( backCursor.parag()->at( backCursor.index() )->c );

        if ( beginningOfSentence )
        {
            QTextCursor cursor( parag->document() );
            cursor.setParag( parag );
            cursor.setIndex( start );
            textdoc->setSelectionStart( KWTextFrameSet::HighlightSelection, &cursor );
            cursor.setIndex( start + 1 );
            textdoc->setSelectionEnd( KWTextFrameSet::HighlightSelection, &cursor );

            KWTextFrameSet * textfs = textdoc->textFrameSet();
            textfs->replaceSelection( textEditCursor, QString( firstChar.upper() ),
                                      KWTextFrameSet::HighlightSelection,
                                      i18n("Autocorrect (capitalize first letter)") );
            bNeedMove = true;
        }
    }
    else if ( m_convertUpperUpper && isUpper( firstChar ) && length > 2 )
    {
        backCursor.setIndex( backCursor.index() + 1 );
        QChar secondChar = backCursor.parag()->at( backCursor.index() )->c;
        if ( isUpper( secondChar ) )
        {
            // Check next letter - we still want to be able to write fully uppercase words...
            backCursor.setIndex( backCursor.index() + 1 );
            QChar thirdChar = backCursor.parag()->at( backCursor.index() )->c;
            if ( isLower( thirdChar ) && (twoUpperLetterException.findIndex(word)==-1))
            {
                // Ok, convert
                QTextCursor cursor( parag->document() );
                cursor.setParag( parag );
                cursor.setIndex( start + 1 ); // After all the first letter's fine, so only change the second letter
                textdoc->setSelectionStart( KWTextFrameSet::HighlightSelection, &cursor );
                cursor.setIndex( start + 2 );
                textdoc->setSelectionEnd( KWTextFrameSet::HighlightSelection, &cursor );

                QString replacement = word[1].lower();
                KWTextFrameSet * textfs = textdoc->textFrameSet();
                textfs->replaceSelection( textEditCursor, replacement,
                                          KWTextFrameSet::HighlightSelection,
                                          i18n("Autocorrect uppercase-uppercase") ); // hard to describe....
                bNeedMove = true;
            }
        }
    }
    if ( bNeedMove )
    {
        // Back to where we were
        KWTextFrameSet * textfs = textdoc->textFrameSet();
        textfs->emitHideCursor();
        textEditCursor->setParag( parag );
        textEditCursor->setIndex( index );
        textEditCursor->gotoRight(); // not the same thing as index+1, in case of CR
        textfs->emitShowCursor();
    }
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
    return c.lower() != c;
}

bool KWAutoFormat::isLower( const QChar &c )
{
    // Note that this is not the same as !isUpper !
    // For instance '1' is not lower nor upper,
    return c.upper() != c;
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
