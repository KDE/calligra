/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2001       Sven Leiber         <s.leiber@web.de>

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

#include "koAutoFormat.h"
#include "kotextdocument.h"

#include <kdebug.h>
#include <klocale.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kstandarddirs.h>

#include <qfile.h>

#include <kotextobject.h>
#include <qdom.h>
#include <kglobal.h>
#include <koDocument.h>
#include "koVariable.h"
#include "koparagcounter.h"
#include <kcommand.h>
#include <kotextformat.h>

/******************************************************************/
/* Class: KoAutoFormat						  */
/******************************************************************/
KoAutoFormat::KoAutoFormat( KoDocument *_doc, KoVariableCollection *_varCollection, KoVariableFormatCollection *_varFormatCollection )
    : m_doc( _doc ),
      m_varCollection(_varCollection),
      m_varFormatCollection(_varFormatCollection),
      m_configRead( false ),
      m_convertUpperCase( false ), m_convertUpperUpper( false ),
      m_advancedAutoCorrect( true ),
      m_autoDetectUrl( false ),
      m_ignoreDoubleSpace( false ),
      m_typographicQuotes(),
      m_maxlen( 0 )
{
}

void KoAutoFormat::readConfig()
{

    // Read the autoformat configuration
    // This is done on demand (when typing the first char, or when opening the config dialog)
    // so that loading is faster and to avoid doing it for readonly documents.
    if ( m_configRead )
        return;
    KConfig config("kofficerc");
    KConfigGroupSaver cgs( &config, "AutoFormat" );
    m_convertUpperCase = config.readBoolEntry( "ConvertUpperCase", false );
    m_convertUpperUpper = config.readBoolEntry( "ConvertUpperUpper", false );
    m_advancedAutoCorrect = config.readBoolEntry( "AdvancedAutocorrect", true );
    m_autoDetectUrl = config.readBoolEntry("AutoDetectUrl",false);
    m_ignoreDoubleSpace = config.readBoolEntry("IgnoreDoubleSpace",false);
    m_removeSpaceBeginEndLine = config.readBoolEntry("RemoveSpaceBeginEndLine",false);

    m_useBulletStyle = config.readBoolEntry("UseBulletStyle",false);
    QString tmp = config.readEntry( "BulletStyle", "" );
    bulletStyle = tmp[0];

    m_autoChangeFormat = config.readBoolEntry( "AutoChangeFormat", false );


    QString begin = config.readEntry( "TypographicQuotesBegin", "«" );
    m_typographicQuotes.begin = begin[0];
    QString end = config.readEntry( "TypographicQuotesEnd", "»" );
    m_typographicQuotes.end = end[0];
    m_typographicQuotes.replace = config.readBoolEntry( "TypographicQuotesEnabled", false )
                                  && !begin.isEmpty()
                                  && !end.isEmpty();

    Q_ASSERT( m_entries.isEmpty() ); // readConfig is only called once...
    config.setGroup( "AutoFormatEntries" );

    bool fileNotFound = false;
    QFile xmlFile;
    KLocale klocale(m_doc->instance()->instanceName());
    xmlFile.setName(locate( "data", "koffice/autocorrect/" + klocale.languageList().front() + ".xml", m_doc->instance() ));
    if(!xmlFile.open(IO_ReadOnly)) {
        xmlFile.setName(locate( "data", "koffice/autocorrect/autocorrect.xml", m_doc->instance() ));
    if(!xmlFile.open(IO_ReadOnly)) {
	fileNotFound = true;
      }
    }

    if(!fileNotFound) {
      QDomDocument doc;
      if(!doc.setContent(&xmlFile)) {
        //return;
      }
      if(doc.doctype().name() != "autocorrection") {
        //return;
      }
      QDomElement de=doc.documentElement();
      QDomElement item = de.namedItem( "items" ).toElement();
      if(!item.isNull())
      {
          QDomNodeList nl = item.childNodes();
          m_maxFindLength=nl.count();
          for(uint i = 0; i < m_maxFindLength; i++) {
              m_entries.insert( nl.item(i).toElement().attribute("find"), KoAutoFormatEntry(nl.item(i).toElement().attribute("replace")) );
          }
      }

      QDomElement upper = de.namedItem( "UpperCaseExceptions" ).toElement();
      if(!upper.isNull())
      {
          QDomNodeList nl = upper.childNodes();
          for(uint i = 0; i < nl.count(); i++)
          {
              upperCaseExceptions+= nl.item(i).toElement().attribute("exception");
          }
      }

      QDomElement twoUpper = de.namedItem( "TwoUpperLetterExceptions" ).toElement();
      if(!twoUpper.isNull())
      {
          QDomNodeList nl = twoUpper.childNodes();
          for(uint i = 0; i < nl.count(); i++)
          {
              twoUpperLetterException+= nl.item(i).toElement().attribute("exception");
          }
      }
    }
    xmlFile.close();
    buildMaxLen();
    m_configRead = true;
}

void KoAutoFormat::saveConfig()
{
    KConfig config("kofficerc");
    KLocale klocale(m_doc->instance()->instanceName());
    KConfigGroupSaver cgs( &config, "AutoFormat" );
    config.writeEntry( "ConvertUpperCase", m_convertUpperCase );
    config.writeEntry( "ConvertUpperUpper", m_convertUpperUpper );
    config.writeEntry( "TypographicQuotesBegin", QString( m_typographicQuotes.begin ) );
    config.writeEntry( "TypographicQuotesEnd", QString( m_typographicQuotes.end ) );
    config.writeEntry( "TypographicQuotesEnabled", m_typographicQuotes.replace );
    config.writeEntry( "AdvancedAutocorrect", m_advancedAutoCorrect );
    config.writeEntry( "AutoDetectUrl",m_autoDetectUrl);

    config.writeEntry( "IgnoreDoubleSpace",m_ignoreDoubleSpace );
    config.writeEntry( "RemoveSpaceBeginEndLine",m_removeSpaceBeginEndLine );

    config.writeEntry( "UseBulletStyle", m_useBulletStyle);
    config.writeEntry( "BulletStyle", QString(bulletStyle));

    config.writeEntry( "AutoChangeFormat", m_autoChangeFormat);

    config.setGroup( "AutoFormatEntries" );
    KoAutoFormatEntryMap::Iterator it = m_entries.begin();

    //refresh m_maxFindLength
    m_maxFindLength=0;
    QDomDocument doc("autocorrection");

    QDomElement begin = doc.createElement( "Word" );
    doc.appendChild( begin );

    QDomElement items;
    items = doc.createElement("items");
    QDomElement data;
    for ( ; it != m_entries.end() ; ++it )
    {
	data = doc.createElement("item");
	data.setAttribute("find", it.key());
	data.setAttribute("replace", it.data().replace());
	items.appendChild(data);

        m_maxFindLength=QMAX(m_maxFindLength,it.key().length());
    }
    begin.appendChild(items);

    QDomElement upper;
    upper = doc.createElement("UpperCaseExceptions");
    for ( QStringList::Iterator it = upperCaseExceptions.begin(); it != upperCaseExceptions.end();++it )
    {
	data = doc.createElement("word");
	data.setAttribute("exception",(*it) );
	upper.appendChild(data);
    }
    begin.appendChild(upper);

    QDomElement twoUpper;
    twoUpper = doc.createElement("TwoUpperLetterExceptions");

    for ( QStringList::Iterator it = twoUpperLetterException.begin(); it != twoUpperLetterException.end();++it )
    {
	data = doc.createElement("word");
	data.setAttribute("exception",(*it) );
	twoUpper.appendChild(data);
    }
    begin.appendChild(twoUpper);

    QFile f(locateLocal("data", "koffice/autocorrect/"+klocale.languageList().front() + ".xml",m_doc->instance()));
    if(!f.open(IO_WriteOnly)) {
        kdWarning()<<"Error during saving autoformat to " << f.name() << endl;
	return;
    }
    QTextStream ts(&f);
    doc.save(ts, 2);
    f.close();

    config.sync();
}

QString KoAutoFormat::getLastWord(KoTextParag *parag, int index)
{
    QString lastWord;
    KoTextString *s = parag->string();
    for ( int i = index - 1; i >= 0; --i )
    {
        QChar ch = s->at( i ).c;
        if ( ch.isSpace() || ch.isPunct() )
            break;
        lastWord.prepend( ch );
    }
    return lastWord;
}

QString KoAutoFormat::getWordAfterSpace(KoTextParag *parag, int index)
{
    QString word;
    KoTextString *s = parag->string();
    for ( int i = index - 1; i >= 0; --i )
    {
        QChar ch = s->at( i ).c;
        if ( ch.isSpace() )
            break;
        word.prepend( ch );
    }
    return word;

}

void KoAutoFormat::doAutoFormat( QTextCursor* textEditCursor, KoTextParag *parag, int index, QChar ch,KoTextObject *txtObj )
{
    if ( !m_configRead )
        readConfig();

    if ( !m_useBulletStyle && !m_removeSpaceBeginEndLine && !m_autoDetectUrl
         && !m_convertUpperUpper && !m_convertUpperCase
         && !m_typographicQuotes.replace && m_entries.count()==0)
        return;

    if( ch.isSpace())
    {
        QString word=getWordAfterSpace(parag,index);
        if ( m_autoChangeFormat && index > 3)
        {
            doAutoChangeFormat( textEditCursor, parag,index, word, txtObj );

        }
        if ( m_autoDetectUrl && index > 0 )
        {
            doAutoDetectUrl( textEditCursor, parag,index, word, txtObj );
        }
    }

    if( ch =='\n' )
    {
        if( (m_removeSpaceBeginEndLine && index > 1) || m_useBulletStyle)
            doRemoveSpaceBeginEndLine( textEditCursor, parag, txtObj );
    }

    //kdDebug() << "KoAutoFormat::doAutoFormat ch=" << QString(ch) << endl;
    //if ( !m_enabled )
    //    return;
    // Auto-correction happens when pressing space, tab, CR, punct etc.
    if ( ( ch.isSpace() || ch.isPunct() ) && index > 0 )
    {
        QString lastWord = getLastWord(parag, index);
        //kdDebug() << "KoAutoFormat::doAutoFormat lastWord=" << lastWord << endl;
        if ( !doAutoCorrect( textEditCursor, parag, index, txtObj ) )
        {
            if ( m_convertUpperUpper || m_convertUpperCase )
                doUpperCase( textEditCursor, parag, index, lastWord, txtObj );
        }
    }
    if ( ch == '"' && m_typographicQuotes.replace )
    {
        doTypographicQuotes( textEditCursor, parag, index,txtObj );
    }
}

bool KoAutoFormat::doAutoCorrect( QTextCursor* textEditCursor, KoTextParag *parag, int index, KoTextObject *txtObj )
{
    if(!m_advancedAutoCorrect)
        return false;
    // Prepare an array with words of different lengths, all terminating at "index".
    // Obviously only full words are put into the array
    // But this allows 'find strings' with spaces and punctuation in them.
    QString * wordArray = new QString[m_maxFindLength+1];
    {
        QString word;
        KoTextString *s = parag->string();
        for ( int i = index - 1; i >= 0; --i )
        {
            QChar ch = s->at( i ).c;
            if ( ch.isSpace() || ch.isPunct() || i==0)
            {
                if(i==0 && word.length()<m_maxFindLength)
                   word.prepend( ch );
                wordArray[word.length()]=word;
            }
            word.prepend( ch );
            if (((index - 1)-i) == (int)m_maxFindLength)
                break;
        }
    }
    KoTextDocument * textdoc = parag->textDocument();

    // Now for each entry in the autocorrect list, look if
    // the word of the same size in wordArray matches.
    // This allows an o(n) behaviour instead of an o(n^2).
    for(int i=m_maxFindLength;i>0;--i)
    {
        KoAutoFormatEntryMap::ConstIterator it = m_entries.find(wordArray[i]);
        if ( wordArray[i]!=0 && it!=m_entries.end() )
        {
            unsigned int length = wordArray[i].length();
            int start = index - length;
            QTextCursor cursor( parag->document() );
            cursor.setParag( parag );
            cursor.setIndex( start );
            textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
            cursor.setIndex( start + length );
            textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
            txtObj->emitNewCommand(txtObj->replaceSelectionCommand( textEditCursor, it.data().replace(),
                                      KoTextObject::HighlightSelection,
                                      i18n("Autocorrect word") ));
            // The space/tab/CR that we inserted is still there but delete/insert moved the cursor
            // -> go right
            txtObj->emitHideCursor();
            textEditCursor->gotoRight();
            txtObj->emitShowCursor();
            delete [] wordArray;
            return true;
        }
    }
    delete [] wordArray;
    return false;
}

void KoAutoFormat::doTypographicQuotes( QTextCursor* textEditCursor, KoTextParag *parag, int index, KoTextObject *txtObj )
{
    kdDebug() << "KoAutoFormat::doTypographicQuotes" << endl;
    KoTextDocument * textdoc = parag->textDocument();
    QTextCursor cursor( parag->document() );
    cursor.setParag( parag );
    cursor.setIndex( index );
    textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
    cursor.setIndex( index + 1 );
    textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );

    // Need to determine if we want a starting or ending quote.
    // I see two solutions: either simply alternate, or depend on leading space.
    // MSWord does the latter afaics...
    QString replacement;
    if ( index > 0 && !parag->at( index - 1 )->c.isSpace() )
        replacement = m_typographicQuotes.end;
    else
        replacement = m_typographicQuotes.begin;
    txtObj->emitNewCommand(txtObj->replaceSelectionCommand( textEditCursor, replacement,
                              KoTextObject::HighlightSelection,
                              i18n("Typographic quote") ));
}

void KoAutoFormat::doUpperCase( QTextCursor *textEditCursor, KoTextParag *parag,
                                int index, const QString & word, KoTextObject *txtObj )
{
    KoTextDocument * textdoc = parag->textDocument();
    unsigned int length = word.length();
    int start = index - length;
    QTextCursor backCursor( parag->document() );
    backCursor.setParag( parag );
    backCursor.setIndex( start );

    // backCursor now points at the first char of the word
    QChar firstChar = backCursor.parag()->at( backCursor.index() )->c;
    bool bNeedMove = false;

    if ( m_convertUpperCase && isLower( firstChar ) )
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

        // Now look for exceptions
        if ( beginningOfSentence )
        {
            QChar punct = backCursor.parag()->at( backCursor.index() )->c;
            QString text = getLastWord( static_cast<KoTextParag*>( backCursor.parag() ), backCursor.index() )
                           + punct;
            // text has the word at the end of the 'sentence', including the termination. Example: "Mr."
            beginningOfSentence = (upperCaseExceptions.findIndex(text)==-1); // Ok if we can't find it
        }

        if ( beginningOfSentence )
        {
            QTextCursor cursor( parag->document() );
            cursor.setParag( parag );
            cursor.setIndex( start );
            textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
            cursor.setIndex( start + 1 );
            textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
            txtObj->emitNewCommand(txtObj->replaceSelectionCommand( textEditCursor, QString( firstChar.upper() ),
                                      KoTextObject::HighlightSelection,
                                      i18n("Autocorrect (capitalize first letter)") ));
            bNeedMove = true;
        }
    }
    else if ( m_convertUpperUpper && isUpper( firstChar ) && length > 2 )
    {
        backCursor.setIndex( backCursor.index() + 1 );
        QChar secondChar = backCursor.parag()->at( backCursor.index() )->c;
        //kdDebug()<<" secondChar :"<<secondChar<<endl;
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
                textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
                cursor.setIndex( start + 2 );
                textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );

                QString replacement = word[1].lower();
                txtObj->emitNewCommand(txtObj->replaceSelectionCommand( textEditCursor, replacement,KoTextObject::HighlightSelection,i18n("Autocorrect (Convert two Upper Case letters to one Upper Case and one Lower Case letter.)") ));

                bNeedMove = true;
            }
        }
    }
    if ( bNeedMove )
    {
        txtObj->emitHideCursor();
        textEditCursor->setParag( parag );
        textEditCursor->setIndex( index );
        textEditCursor->gotoRight(); // not the same thing as index+1, in case of CR
        txtObj->emitShowCursor();
    }
}

void KoAutoFormat::doAutoDetectUrl( QTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj )
{
    if (word.find("http://")!=-1 || word.find("mailto:")!=-1
        || word.find("ftp://")!=-1 || word.find("file:")!=-1)
    {
        unsigned int length = word.length();
        int start = index - length;
        QTextCursor cursor( parag->document() );
        KoTextDocument * textdoc = parag->textDocument();
        cursor.setParag( parag );
        cursor.setIndex( start );
        textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
        cursor.setIndex( start + length );
        textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
        KoVariable *var=new KoLinkVariable( textdoc, word, word ,m_varFormatCollection->format( "STRING" ), m_varCollection );

        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        KoTextFormat * lastFormat = static_cast<KoTextFormat *>(parag->at( parag->length() - 1 )->format());
        txtObj->insert( textEditCursor, lastFormat, KoTextObject::customItemChar(), false, true, i18n("Insert Variable"), customItemsMap,KoTextObject::HighlightSelection );
        txtObj->emitHideCursor();
        textEditCursor->gotoRight();
        txtObj->emitShowCursor();
    }

}

void KoAutoFormat::doAutoChangeFormat( QTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj )
{
    bool underline = (word.at(0)=='_' && word.at(word.length()-1)=='_');
    bool bold = (word.at(0)=='*' && word.at(word.length()-1)=='*');
    if( bold || underline)
    {
        QString replacement=word.mid(1,word.length()-2);
        int start = index - word.length();
        KoTextDocument * textdoc = parag->textDocument();
        KMacroCommand *macro=new KMacroCommand(i18n("Autocorrection : change format."));
        QTextCursor cursor( parag->document() );

        cursor.setParag( parag );
        cursor.setIndex( start );
        textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
        cursor.setIndex( start + word.length() );
        textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
        macro->addCommand(txtObj->replaceSelectionCommand( textEditCursor, replacement,
                                                           KoTextObject::HighlightSelection,
                                                           i18n("Autocorrect word") ));

        KoTextFormat * lastFormat = static_cast<KoTextFormat *>(parag->at( start )->format());
        KoTextFormat * newFormat = new KoTextFormat(*lastFormat);
        cursor.setIndex( start );
        textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
        cursor.setIndex( start + word.length()-2 );
        textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );

        if( bold)
        {
            newFormat->setBold(true);
            macro->addCommand(txtObj->setFormatCommand( textEditCursor, lastFormat, newFormat, QTextFormat::Bold , false,KoTextObject::HighlightSelection  ));
        }
        else if( underline )
        {
            newFormat->setUnderline(true);
            macro->addCommand(txtObj->setFormatCommand( textEditCursor, lastFormat, newFormat, QTextFormat::Underline , false,KoTextObject::HighlightSelection  ));
        }
        txtObj->emitNewCommand(macro);
        txtObj->emitHideCursor();
        textEditCursor->gotoRight();
        txtObj->emitShowCursor();
    }
}

void KoAutoFormat::doRemoveSpaceBeginEndLine( QTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj )
{
    KoTextString *s = parag->string();
    bool refreshCursor=false;
    QChar ch = s->at( 0 ).c;
    KoTextDocument * textdoc = parag->textDocument();
    QTextCursor cursor( parag->document() );
    if( m_useBulletStyle && ch =='*' && (s->at(1).c).isSpace() && parag->string()->length()>3)
    {
        cursor.setParag( parag );
        cursor.setIndex( 0 );
        textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
        cursor.setIndex( 2 );
        textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
        txtObj->removeSelectedText( &cursor, KoTextObject::HighlightSelection,QString::null, false  );

        KoParagCounter c;
        if( bulletStyle.isNull())
        {
            c.setNumbering( KoParagCounter::NUM_LIST );
            c.setStyle( KoParagCounter::STYLE_DISCBULLET );
        }
        else
        {
            c.setNumbering( KoParagCounter::NUM_LIST );
            c.setStyle( KoParagCounter::STYLE_CUSTOMBULLET );
            c.setCustomBulletCharacter( bulletStyle );
        }
        parag->setCounter(c);
        static_cast<KoTextParag*>(parag->next())->setCounter(c);
    }



    for ( int i = 0 ; i < parag->string()->length() ; i++ )
    {
        QChar ch = s->at( i ).c;
        if( !ch.isSpace())
        {
            if( i == 0 )
                break;
            cursor.setParag( parag );
            cursor.setIndex( 0 );
            textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
            cursor.setIndex( i );
            textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
            txtObj->removeSelectedText( &cursor, KoTextObject::HighlightSelection,QString::null, false  );
            refreshCursor=true;
            break;
        }
    }
    for ( int i = parag->string()->length()-1; i >= 0; --i )
    {
        QChar ch = s->at( i ).c;
        if( !ch.isSpace())
        {
            if( i == parag->string()->length()-1 )
                break;
            cursor.setParag( parag );
            cursor.setIndex( i+1 );
            textdoc->setSelectionStart( KoTextObject::HighlightSelection, &cursor );
            cursor.setIndex( parag->string()->length() );
            textdoc->setSelectionEnd( KoTextObject::HighlightSelection, &cursor );
            txtObj->removeSelectedText( &cursor, KoTextObject::HighlightSelection,QString::null, false  );
            refreshCursor=true;
            break;
        }
    }
    if( refreshCursor)
    {
        txtObj->emitHideCursor();
        textEditCursor->setParag( parag->next() );
        //textEditCursor->cursorgotoRight();
        txtObj->emitShowCursor();
    }
}

bool KoAutoFormat::doIgnoreDoubleSpace( KoTextParag *parag, int index,QChar ch )
{
    if( m_ignoreDoubleSpace && ch.isSpace() && index >=  0 )
    {
        KoTextString *s = parag->string();
        QChar ch = s->at( index ).c;
        if ( ch.isSpace() )
            return true;
    }
    return false;
}

void KoAutoFormat::configTypographicQuotes( TypographicQuotes _tq )
{
    m_typographicQuotes = _tq;
}

void KoAutoFormat::configUpperCase( bool _uc )
{
    m_convertUpperCase = _uc;
}

void KoAutoFormat::configUpperUpper( bool _uu )
{
    m_convertUpperUpper = _uu;
}

void KoAutoFormat::configAdvancedAutocorrect( bool _aa )
{
    m_advancedAutoCorrect = _aa;
}

void KoAutoFormat::configAutoDetectUrl(bool _au)
{
    m_autoDetectUrl=_au;
}

void KoAutoFormat::configIgnoreDoubleSpace( bool _ids)
{
    m_ignoreDoubleSpace=_ids;
}

void KoAutoFormat::configRemoveSpaceBeginEndLine( bool _space)
{
    m_removeSpaceBeginEndLine=_space;
}

void KoAutoFormat::configUseBulletStyle( bool _ubs)
{
    m_useBulletStyle=_ubs;
}

void KoAutoFormat::configBulletStyle( QChar b )
{
    bulletStyle = b;
}

void KoAutoFormat::configAutoChangeFormat( bool b)
{
    m_autoChangeFormat = b;
}


bool KoAutoFormat::isUpper( const QChar &c )
{
    return c.lower() != c;
}

bool KoAutoFormat::isLower( const QChar &c )
{
    // Note that this is not the same as !isUpper !
    // For instance '1' is not lower nor upper,
    return c.upper() != c;
}

bool KoAutoFormat::isMark( const QChar &c )
{
    return ( c == QChar( '.' ) ||
	     c == QChar( '?' ) ||
	     c == QChar( '!' ) );
}

bool KoAutoFormat::isSeparator( const QChar &c )
{
    return ( !c.isLetter() && !c.isNumber() && !c.isDigit() );
}

void KoAutoFormat::buildMaxLen()
{
    QMap< QString, KoAutoFormatEntry >::Iterator it = m_entries.begin();

    m_maxlen = 0;
    for ( ; it != m_entries.end(); ++it )
	m_maxlen = QMAX( m_maxlen, it.key().length() );
}
