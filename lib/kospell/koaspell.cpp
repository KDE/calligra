/* This file is part of the KDE libraries
   Copyright (C) 1997 David Sweet <dsweet@kde.org>
   Copyright (C) 2000-2001 Wolfram Diestel <wolfram@steloj.de>
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBASPELL

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h> // atoi

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <qtextcodec.h>
#include <qtimer.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include "koaspell.h"
#include "koaspell.moc"
#include "koSpell.h"
#include "koSpelldlg.h"
#include <kwin.h>
#include <kprocio.h>

#include <qtimer.h>

#include <aspell.h>

#define MAXLINELENGTH 10000

enum {
	GOOD=     0,
	IGNORE=   1,
	REPLACE=  2,
	MISTAKE=  3
};


KOASpell::KOASpell( KOSpellConfig *_ksc )
    :KOSpell(_ksc)
{
    initSpell(_ksc);
    initConfig();
    QTimer::singleShot( 0, this, SLOT( slotEmitCheckerReady() ) );
}

//TODO FIXME !!!! KOSpellConfig not used.
void KOASpell::initSpell(KOSpellConfig *_ksc)
{

    m_bIgnoreUpperWords=false;
    m_bIgnoreTitleCase=false;
    autocorrect = false;
    autoDelete = false;
    modaldlg = false;
    speller = 0L;
    config = 0L;
    offset = 0;
    ksdlg=0;
    lastpos = -1;

    personaldict=FALSE;
    dlgresult=-1;

    caption=QString::null;

    parent=0L;
}

KOASpell::KOASpell (QWidget *_parent, const QString &_caption,
		KOSpellConfig *_ksc,
		bool _modal,  bool _autocorrect)
    :KOSpell(_parent,_caption,_ksc,_modal,_autocorrect)
{
    initSpell(_ksc);
    autocorrect = _autocorrect;
    modaldlg = _modal;
    caption=_caption;
    parent=_parent;

    setUpDialog();
    QTimer::singleShot( 0, this, SLOT( slotEmitCheckerReady() ) );
}

void KOASpell::slotEmitCheckerReady()
{
    emit spellCheckerReady();
}

bool KOASpell::initConfig(const QString & language)
{
    config = new_aspell_config();
    kdDebug()<<" ksconfig->dictionary() :"<<ksconfig->dictionary()<<endl;

    aspell_config_replace(config, "lang", language.isEmpty() ? (ksconfig->dictionary().isEmpty() ? "fr": ksconfig->dictionary().latin1()) : language.latin1() );

    kdDebug()<<" ksconfig->dictionary() :"<<ksconfig->dictionary()<<endl;

    AspellCanHaveError * ret;
    ret = new_aspell_speller(config);
    if (aspell_error(ret) != 0) {
        kdDebug()<<"Error :"<<aspell_error_message(ret)<<endl;
        delete_aspell_can_have_error(ret);
        return false;
    }
    switch (ksconfig->encoding())
    {
    case KOS_E_LATIN1:
	aspell_config_replace(config, "encoding", "latin1");
	break;
    case KOS_E_LATIN2:
        aspell_config_replace(config, "encoding", "latin2");
	break;
    case KOS_E_LATIN3:
	aspell_config_replace(config, "encoding", "latin3");
        break;
        // add the other charsets here
    case KOS_E_LATIN4:
    case KOS_E_LATIN5:
    case KOS_E_LATIN7:
    case KOS_E_LATIN8:
    case KOS_E_LATIN9:
    case KOS_E_LATIN13:
    case KOS_E_LATIN15:
	// will work, if this is the default charset in the dictionary
	kdError(750) << "charsets iso-8859-4 .. iso-8859-15 not supported yet" << endl;
	break;
    case KOS_E_UTF8:
        aspell_config_replace(config, "encoding", "utf8");
        break;
    case KOS_E_KOI8U:
        //todo
	break;
    }

    aspell_config_replace(config, "ignore-case", ksconfig->ignoreCase()?"true" : "false" );
    aspell_config_replace(config, "ignore-accents", ksconfig->ignoreAccent()?"true" : "false" );

    ret = new_aspell_speller(config);

    delete_aspell_config(config);

    if (aspell_error(ret) != 0) {
        printf("Error: %s\n",aspell_error_message(ret));
        delete_aspell_can_have_error(ret);
        return false;
    }
    speller = to_aspell_speller(ret);
    config = aspell_speller_config(speller);
    return true;
}

void
KOASpell::setUpDialog ()
{
    if (ksdlg)
        return;
    bool ret = initConfig();
    if ( !ret )
        return;

    //Set up the dialog box
    ksdlg=new KOSpellDlg (parent, ksconfig,"dialog", KOSpellConfig::indexFromLanguageFileName( ksconfig->dictionary()),  modaldlg, autocorrect );
    ksdlg->setCaption (caption);
#ifdef Q_WS_X11 // FIXME(E): Implement for Qt/Embedded
    KWin::setIcons (ksdlg->winId(), kapp->icon(), kapp->miniIcon());
#endif
    if ( modaldlg )
        ksdlg->setFocus();
}

bool KOASpell::addPersonal (const QString & word)
{
    if( !speller)
        return false;
    //add to aspell internal.
    aspell_speller_add_to_personal(speller, word.latin1(), word.length());
    //save directly into personnal dictionary.
    writePersonalDictionary();
    return true;
}

bool KOASpell::writePersonalDictionary ()
{
    if( !speller)
        return false;
    aspell_speller_save_all_word_lists(speller);
    kdDebug()<<"aspell_speller_error_message(speller) :"<<aspell_speller_error_message(speller)<<endl;
    return true;
}

bool KOASpell::ignore (const QString & /*word*/)
{
    //fixme !!!!!!!!!!!!!!!!
    return true;
}


QStringList KOASpell::resultCheckWord( const QString &_word )
{
    if (_word.isEmpty() || !speller)
        return QStringList();
    kdDebug()<<" aspell_config_retrieve(config, lang) :"<<aspell_config_retrieve(config, "lang")<<endl;
    QStringList result;
    const AspellWordList *wl = aspell_speller_suggest(speller, _word.latin1(), -1);
    if (wl == 0) {
        kdDebug()<<"Error: "<< aspell_speller_error_message(speller)<<endl;
    } else {
        AspellStringEnumeration * els = aspell_word_list_elements(wl);
        const char * word2;
        while ( (word2 = aspell_string_enumeration_next(els)) != 0) {
            result.append( word2 );
            kdDebug()<<" word2 :"<<word2<<endl;
        }
    }
    return result;
}

bool KOASpell::spellWord( const QString &_word )
{
    QStringList lst =resultCheckWord( _word );
    if ( lst.isEmpty() && ((lastpos >= (int)origbuffer.length()-1)|| lastpos<0) )
    {
        //change m_status before to emit signal otherwise
        //kword + multiframe doesn't work
        m_status = Finished;
        emit done( origbuffer );
        return false;
    }
    if ( lst.contains( _word ))
      return false;

    dialog( _word, lst);
    return true;
}

void KOASpell::nextWord()
{
    QString word;
    lastpos++;
    bool haveAnNumber = false;
    do
    {
        int i =0;
        for ( i = lastpos; i<(int)origbuffer.length();i++)
        {
            QChar ch = origbuffer[i];
            if ( ch.isSpace() || ch.isPunct() )
                break;
            if ( ch.isNumber() )
                haveAnNumber = true;
            word.append(ch);
        }
        lastpos = i;
        if ( !word.isEmpty() )
            testIgnoreWord( word, haveAnNumber );
        else
            lastpos++;
    }
    while ( word.isEmpty() && (lastpos < (int)origbuffer.length()-1));
    if ( m_status != Finished && !spellWord( word ))
    {
        checkNextWord();
    }
}

void KOASpell::testIgnoreWord( QString & word, bool haveAnNumber )
{
    if ( !ksconfig->spellWordWithNumber() && haveAnNumber )
    {
        word ="";
        return;
    }

    if(m_bIgnoreTitleCase && word==word.upper())
    {
        word ="";
        return;
    }

    if(m_bIgnoreUpperWords && word[0]==word[0].upper())
    {
        QString text=word[0]+word.right(word.length()-1).lower();
        if(text==word)
        {
            word ="";
            return;
        }
    }

    //We don't take advantage of ispell's ignore function because
    //we can't interrupt ispell's output (when checking a large
    //buffer) to add a word to _it's_ ignore-list.
    if (!word.isEmpty() &&ignorelist.findIndex(word.lower())!=-1)
    {
        word ="";
        return;
    }
    //
    QStringList::Iterator it = replacelist.begin();
    for(;it != replacelist.end(); ++it, ++it) // Skip two entries at a time.
    {
        if (word == *it) // Word matches
        {
            QString origWord = *it;
            ++it;
            word = *it;   // Replace it with the next entry
            correctWord( origWord ,  word);
            word ="";
        }
    }
}

void KOASpell::correctWord( const QString & originalword, const QString & newword )
{
    emit corrected (originalword ,  newword, lastpos+offset-originalword.length());
    offset+=newword.length()-originalword.length();
    newbuffer.replace (lastpos+offset, newword.length(), newword );
}

void KOASpell::previousWord()
{
    QString word;
    lastpos--;
    bool haveAnNumber = false;
    do
    {
        int i =0;
        for ( i = lastpos; i>=0;--i)
        {
            QChar ch = origbuffer[i];
            if ( ch.isSpace() || ch.isPunct() )
            {
                lastpos--;
                break;
            }
            if ( ch.isNumber() )
                haveAnNumber = true;
            word.prepend(ch);
        }
        lastpos = i;
        if ( !word.isEmpty() )
            testIgnoreWord( word, haveAnNumber );
        else
            lastpos--;
    }
    while ( word.isEmpty() && (lastpos >= 0));

    if ( m_status != Finished && !spellWord( word ))
    {
        checkNextWord();
    }

}

bool KOASpell::check( const QString &_buffer, bool _usedialog )
{
    if( !ksdlg )
        return false;
    lastpos = -1;
    usedialog = _usedialog;
    origbuffer = _buffer;
    m_status = Starting;
    if ( ( totalpos = origbuffer.length() ) == 0 )
    {
        emit done(origbuffer);
        return FALSE;
    }

    // Torben: I corrected the \n\n problem directly in the
    //         origbuffer since I got errors otherwise
    if ( origbuffer.right(2) != "\n\n" )
    {
        if (origbuffer.at(origbuffer.length()-1)!='\n')
	{
            origbuffer+='\n';
            origbuffer+='\n'; //shouldn't these be removed at some point?
	}
        else
            origbuffer+='\n';
    }

    newbuffer=origbuffer;
    //lastpos is a position in newbuffer (it has offset in it)
    offset=lastlastline=lastline=0;
    lastpos = -1;


    // send first buffer line
    int i = origbuffer.find('\n', 0)+1;
    QString qs;
    qs=origbuffer.mid (0,i);
    lastline=i; //the character position, not a line number
    if (_usedialog)
        ksdlg->show();
    else
        ksdlg->hide();

    //check new word.
    checkNextWord();
    return TRUE;
}

void KOASpell::checkNextWord()
{
    if ( !ksdlg)
        return;

    if ( !ksdlg->previousWord() )
        nextWord();
    else
        previousWord();
}

void KOASpell::dialog(const QString & word, QStringList & sugg )
{
    if ( !ksdlg )
        return;
    dlgorigword=word;

    connect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
    ksdlg->init (word, &sugg);
    if (!ksdlg->previousWord())
        misspellingWord (word, sugg, lastpos+offset-word.length());
    else
        misspellingWord (word, sugg, lastpos+offset+1);

    ksdlg->show();
}

void KOASpell::dialog2 (int result)
{
    if ( !ksdlg )
        return;
    QString qs;
    disconnect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
    dlgresult=result;
    ksdlg->standby();

    dlgreplacement=ksdlg->replacement();
    bool testNextWord = true;
    QString _replacement;
    switch (dlgresult)
    {
    case KOS_IGNORE:
        emit ignoreword(dlgorigword);
        break;
    case KOS_IGNOREALL:
        // would be better to lower case only words with beginning cap
        ignorelist.prepend(dlgorigword.lower());
        emit ignoreall (dlgorigword);
        break;
    case KOS_ADD:
        addPersonal (dlgorigword);
        personaldict=TRUE;
        emit addword (dlgorigword);
        // adding to personal dict takes effect at the next line, not the current
        ignorelist.prepend(dlgorigword.lower());
        break;
    case KOS_REPLACEALL:
        replacelist.append (dlgorigword);
        _replacement = replacement();
        replacelist.append (_replacement);

        emit replaceall( dlgorigword ,  _replacement );
        correctWord( dlgorigword ,  _replacement );
        break;
    case KOS_ADDAUTOCORRECT:
        //todo add new word ????
        emit addAutoCorrect (dlgorigword , replacement());
    case KOS_REPLACE:
        correctWord( dlgorigword ,  replacement() );
        break;
    case KOS_CHECKAGAINWITHNEWLANGUAGE:
        changeSpellLanguage( ksdlg->languageIndex());
        spellCheckReplaceWord( dlgreplacement);
        testNextWord = false;
        break;
    case KOS_CHECKAGAIN:
        spellCheckReplaceWord( dlgreplacement);
        testNextWord = false;
        break;
    case KOS_STOP:
        testNextWord = false;
        ksdlg->hide();
        //buffer=newbuffer);
        emit done (newbuffer);
        emit death();
        break;
    case KOS_CANCEL:
        testNextWord = false;
        //      kdDebug(750) << "cancelled\n" << endl;
        ksdlg->hide();
        emit done (origbuffer);
        emit death();
        break;
    }
    if ( testNextWord)
        checkNextWord();
}

void KOASpell::spellCheckReplaceWord( const QString & _word)
{
    if ( !ksdlg )
        return;
    connect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
    QStringList lst;
    lst=resultCheckWord( _word );
    ksdlg->changeSuggList( &lst);
    ksdlg->show();
}

void KOASpell::deleteSpellChecker()
{
    if( speller )
    {
        delete_aspell_speller(speller);
        speller = 0;
    }
}

KOASpell::~KOASpell ()
{
    deleteSpellChecker();
}

// This is always called from the event loop to make
// sure that the receiver can safely delete the
// KOASpell object.
void KOASpell::emitDeath()
{
#if 0
  bool deleteMe = autoDelete; // Can't access object after next call!
  emit death();
  if (deleteMe)
     delete this;
#endif
}

void KOASpell::changeSpellLanguage( int index )
{
    deleteSpellChecker();
    initConfig( KOSpellConfig::listOfLanguageFileName()[index].latin1());
#if 0
    kdDebug()<<"Before KOSpellConfig::listOfLanguageFileName()[index].latin1() :"<<KOSpellConfig::listOfLanguageFileName()[index].latin1()<<endl;
    aspell_config_replace(config, "lang",KOSpellConfig::listOfLanguageFileName()[index].latin1());
    kdDebug()<<" After aspell_config_retrieve(config, lang) :"<<aspell_config_retrieve(config, "lang")<<endl;
#endif
}


int KOASpell::modalCheck( QString& text, KOSpellConfig* _kcs )
{
    modalreturn = 0;
    modaltext = text;

    KOASpell* m_spell = new KOASpell(0L, i18n("Spell Checker"), 0 ,_kcs,true );
    QObject::connect( m_spell, SIGNAL( death() ),
                      m_spell, SLOT( slotModalSpellCheckerFinished() ) );
    QObject::connect( m_spell, SIGNAL( corrected( const QString &, const QString &, unsigned int ) ),
                      m_spell, SLOT( slotSpellCheckerCorrected( const QString &, const QString &, unsigned int ) ) );
    QObject::connect( m_spell, SIGNAL( done( const QString & ) ),
                      m_spell, SLOT( slotModalDone( const QString & ) ) );

    bool result = m_spell->check( text );
    if ( !result)
    {
        delete m_spell;
        m_spell=0L;
        return modalreturn;
    }

    while (m_spell->status()!=Finished)
        kapp->processEvents();

    text = modaltext;
    delete m_spell;
    return modalreturn;
}

void KOASpell::slotSpellCheckerCorrected( const QString & oldText, const QString & newText, unsigned int pos )
{
    modaltext=modaltext.replace(pos,oldText.length(),newText);
}


void KOASpell::slotModalDone( const QString &/*_buffer*/ )
{
    slotModalSpellCheckerFinished();
}

void KOASpell::slotModalSpellCheckerFinished()
{
    modalreturn=(int)this->status();
}
QString KOASpell::modaltext;
int KOASpell::modalreturn = 0;

#endif


