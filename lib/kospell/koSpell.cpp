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
#include "koSpell.h"
#include "koSpelldlg.h"
#include <kwin.h>
#include <kprocio.h>

#include <aspell.h>

#define MAXLINELENGTH 10000

enum {
	GOOD=     0,
	IGNORE=   1,
	REPLACE=  2,
	MISTAKE=  3
};

class KOSpell::KOSpellPrivate
{
public:
    bool endOfResponse;
    bool m_bIgnoreUpperWords;
    bool m_bIgnoreTitleCase;
};


//TODO
//Parse stderr output
//e.g. -- invalid dictionary name

/*
  Things to put in KSpellConfigDlg:
    make root/affix combinations that aren't in the dictionary (-m)
    don't generate any affix/root combinations (-P)
    Report  run-together  words   with   missing blanks as spelling errors.  (-B)
    default dictionary (-d [dictionary])
    personal dictionary (-p [dictionary])
    path to ispell -- NO: ispell should be in $PATH
    */

KOSpell::KOSpell (QWidget *_parent, const QString &_caption,
		KOSpellConfig *_ksc,
		bool _progressbar, bool _modal,  bool _autocorrect)
{
  d=new KOSpellPrivate;

  d->m_bIgnoreUpperWords=false;
  d->m_bIgnoreTitleCase=false;
  autocorrect = _autocorrect;
  autoDelete = false;
  modaldlg = _modal;
  progressbar = _progressbar;
  speller = 0L;
  config = 0L;
  offset = 0;
  ksconfig=0;
  ksdlg=0;
  lastpos = 0;
  //won't be using the dialog in ksconfig, just the option values
  if (_ksc!=0)
    ksconfig = new KOSpellConfig (*_ksc);
  else
    ksconfig = new KOSpellConfig;

  codec = 0;
  switch (ksconfig->encoding())
  {
  case KS_E_LATIN1:
     codec = QTextCodec::codecForName("ISO 8859-1");
     break;
  case KS_E_LATIN2:
     codec = QTextCodec::codecForName("ISO 8859-2");
     break;
  case KS_E_LATIN3:
      codec = QTextCodec::codecForName("ISO 8859-3");
      break;
  case KS_E_LATIN4:
      codec = QTextCodec::codecForName("ISO 8859-4");
      break;
  case KS_E_LATIN5:
      codec = QTextCodec::codecForName("ISO 8859-5");
      break;
  case KS_E_LATIN7:
      codec = QTextCodec::codecForName("ISO 8859-7");
      break;
  case KS_E_LATIN8:
      codec = QTextCodec::codecForName("ISO 8859-8");
      break;
  case KS_E_LATIN9:
      codec = QTextCodec::codecForName("ISO 8859-9");
      break;
  case KS_E_LATIN13:
      codec = QTextCodec::codecForName("ISO 8859-13");
      break;
  case KS_E_LATIN15:
      codec = QTextCodec::codecForName("ISO 8859-15");
      break;
  case KS_E_UTF8:
      codec = QTextCodec::codecForName("UTF-8");
      break;
  case KS_E_KOI8R:
      codec = QTextCodec::codecForName("KOI8-R");
      break;
  case KS_E_KOI8U:
      codec = QTextCodec::codecForName("KOI8-U");
      break;
  case KS_E_CP1251:
      codec = QTextCodec::codecForName("CP1251");
      break;
  default:
     break;
  }

  kdDebug(750) << __FILE__ << ":" << __LINE__ << " Codec = " << (codec ? codec->name() : "<default>") << endl;

  // copy ignore list from ksconfig
  ignorelist += ksconfig->ignoreList();

  replacelist += ksconfig->replaceAllList();
  texmode=dlgon=FALSE;
  m_status = Starting;
  progres=10;
  curprog=0;


  personaldict=FALSE;
  dlgresult=-1;

  caption=_caption;

  parent=_parent;

  trystart=0;
  maxtrystart=2;
  setUpDialog();
}

bool KOSpell::initConfig()
{
    config = new_aspell_config();
    kdDebug()<<" ksconfig->dictionary() :"<<ksconfig->dictionary()<<endl;

    aspell_config_replace(config, "lang", ksconfig->dictionary().isEmpty() ? "fr": ksconfig->dictionary().latin1());
    switch (ksconfig->encoding())
    {
    case KS_E_LATIN1:
	aspell_config_replace(config, "encoding", "latin1");
	break;
    case KS_E_LATIN2:
        aspell_config_replace(config, "encoding", "latin2");
	break;
    case KS_E_LATIN3:
	aspell_config_replace(config, "encoding", "latin3");
        break;
        // add the other charsets here
    case KS_E_LATIN4:
    case KS_E_LATIN5:
    case KS_E_LATIN7:
    case KS_E_LATIN8:
    case KS_E_LATIN9:
    case KS_E_LATIN13:
    case KS_E_LATIN15:
	// will work, if this is the default charset in the dictionary
	kdError(750) << "charsets iso-8859-4 .. iso-8859-15 not supported yet" << endl;
	break;
    case KS_E_UTF8:
        aspell_config_replace(config, "encoding", "utf8");
        break;
    case KS_E_KOI8U:
        //todo
	break;
    }

    aspell_config_replace(config, "ignore-case", ksconfig->ignoreCase()?"true" : "false" );
    aspell_config_replace(config, "ignore-accents", ksconfig->ignoreAccent()?"true" : "false" );

    AspellCanHaveError * ret;
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

void KOSpell::hide() { ksdlg->hide(); }

int KOSpell::heightDlg() const { return ksdlg->height(); }
int KOSpell::widthDlg() const { return ksdlg->width(); }

void
KOSpell::setUpDialog (bool reallyuseprogressbar)
{
    kdDebug()<<" KOSpell::setUpDialog (bool reallyuseprogressbar)*******\n";
    if (ksdlg)
        return;
    initConfig();

    //Set up the dialog box
    ksdlg=new KOSpellDlg (parent, "dialog", KOSpellConfig::indexFromLanguageFileName( ksconfig->dictionary()), progressbar && reallyuseprogressbar, modaldlg, autocorrect );
    ksdlg->setCaption (caption);
    connect (this, SIGNAL ( progress (unsigned int) ),
             ksdlg, SLOT ( slotProgress (unsigned int) ));
#ifdef Q_WS_X11 // FIXME(E): Implement for Qt/Embedded
    KWin::setIcons (ksdlg->winId(), kapp->icon(), kapp->miniIcon());
#endif
    if ( modaldlg )
        ksdlg->setFocus();
}

bool KOSpell::addPersonal (const QString & word)
{
    //add to aspell internal.
    aspell_speller_add_to_personal(speller, word.latin1(), word.length());
    //save directly into personnal dictionary.
    writePersonalDictionary();
    return true;
}

bool KOSpell::writePersonalDictionary ()
{
    aspell_speller_save_all_word_lists(speller);
    kdDebug()<<"aspell_speller_error_message(speller) :"<<aspell_speller_error_message(speller)<<endl;
    return true;
}

bool KOSpell::ignore (const QString & /*word*/)
{
    //fixme !!!!!!!!!!!!!!!!
    return true;
}


QStringList KOSpell::resultCheckWord( const QString &_word )
{
    if (_word.isEmpty())
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

bool KOSpell::spellWord( const QString &_word )
{
    QStringList lst =resultCheckWord( _word );
    if ( lst.isEmpty() && (lastpos >= (int)origbuffer.length()-1) )
    {
        emit death();
        return false;
    }
    if ( lst.contains( _word ))
      return false;

    dialog( _word, lst);
    return true;
}

void KOSpell::nextWord()
{
    QString word;
    do
    {
        int i =0;
        for ( i = lastpos; i<(int)origbuffer.length();i++)
        {
            QChar ch = origbuffer[i];
            if ( ch.isSpace() || ch.isPunct() )
                break;
            word.append(ch);
        }
        lastpos = i+1;


        if(d->m_bIgnoreTitleCase && word==word.upper())
            word ="";

        if(d->m_bIgnoreUpperWords && word[0]==word[0].upper())
        {
            QString text=word[0]+word.right(word.length()-1).lower();
            if(text==word)
                word ="";
        }

        //We don't take advantage of ispell's ignore function because
        //we can't interrupt ispell's output (when checking a large
        //buffer) to add a word to _it's_ ignore-list.
        if (ignorelist.findIndex(word.lower())!=-1)
            word ="";
    }
    while ( word.isEmpty() && (lastpos < (int)origbuffer.length()-1));

    if ( !spellWord( word ))
    {
        checkNextWord();
    }
}

void KOSpell::previousWord()
{
    //todo
}

bool KOSpell::check( const QString &_buffer, bool _usedialog )
{

    kdDebug()<<" _buffer:"<<_buffer<<endl;
    origbuffer = _buffer;
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
    offset=lastlastline=lastpos=lastline=0;

    emitProgress ();

    // send first buffer line
    int i = origbuffer.find('\n', 0)+1;
    QString qs;
    qs=origbuffer.mid (0,i);
    lastline=i; //the character position, not a line number

    if (usedialog)
    {
        emitProgress();
        ksdlg->show();
    }
    else
        ksdlg->hide();

    //check new word.
    checkNextWord();
    return TRUE;
}

void KOSpell::checkNextWord()
{
    if ( !ksdlg->previousWord() )
        nextWord();
    else
        previousWord();
}

void KOSpell::dialog(const QString & word, QStringList & sugg )
{
    dlgorigword=word;

    connect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
    ksdlg->init (word, &sugg);
    emit misspelling (word, sugg, lastpos-word.length()-1 /* -1 for next space*/);

    emitProgress();
    ksdlg->show();
}

void KOSpell::dialog2 (int result)
{
    QString qs;
    kdDebug()<<" dialog2 (int result) :"<<result<<endl;
    disconnect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
    dlgresult=result;
    ksdlg->standby();

    dlgreplacement=ksdlg->replacement();
    bool testNextWord = true;
    switch (dlgresult)
    {
    case KS_IGNORE:
        emit ignoreword(dlgorigword);
        break;
    case KS_IGNOREALL:
        // would be better to lower case only words with beginning cap
        ignorelist.prepend(dlgorigword.lower());
        emit ignoreall (dlgorigword);
        break;
    case KS_ADD:
        addPersonal (dlgorigword);
        personaldict=TRUE;
        emit addword (dlgorigword);
        // adding to personal dict takes effect at the next line, not the current
        ignorelist.prepend(dlgorigword.lower());
        break;
    case KS_REPLACEALL:
        replacelist.append (dlgorigword);
        replacelist.append (replacement());
        kdDebug()<<" newbuffer :"<<newbuffer<<endl;

        offset+=replacement().length()-replacement().length();
        newbuffer.replace (lastpos, replacement().length(), replacement()+" " );
        kdDebug()<<" apres :"<<newbuffer<<endl;
        emit replaceall( dlgorigword ,  replacement() );
        emit corrected (dlgorigword ,  replacement(), lastpos);
        break;
    case KS_REPLACE:
        kdDebug()<<" newbuffer :"<<newbuffer<<endl;

        offset+=replacement().length()-replacement().length();
        newbuffer.replace (lastpos, replacement().length(), replacement()+" " );
        kdDebug()<<" apres :"<<newbuffer<<endl;
        emit corrected (dlgorigword ,  replacement(), lastpos);
        break;
    case KS_CHECKAGAINWITHNEWLANGUAGE:
        changeSpellLanguage( ksdlg->languageIndex());
        spellCheckReplaceWord( dlgreplacement);
        testNextWord = false;
        break;
    case KS_CHECKAGAIN:
        spellCheckReplaceWord( dlgreplacement);
        testNextWord = false;
        break;
    case KS_ADDAUTOCORRECT:
        //todo add new word ????
        emit addAutoCorrect (dlgorigword , replacement());
        break;
    case KS_STOP:
        testNextWord = false;
        ksdlg->hide();
        //buffer=newbuffer);
        emitProgress();
        emit done (newbuffer);
        emit death();
        break;
    case KS_CANCEL:
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

void KOSpell::spellCheckReplaceWord( const QString & _word)
{
    connect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
    QStringList lst;
    lst=resultCheckWord( _word );
    ksdlg->changeSuggList( &lst);
    ksdlg->show();
}


KOSpell::~KOSpell ()
{
    delete d;
    delete ksconfig;
    delete ksdlg;
}


KOSpellConfig KOSpell::ksConfig () const
{
    ksconfig->setIgnoreList(ignorelist);
    ksconfig->setReplaceAllList(replacelist);
    return *ksconfig;

}


// This is always called from the event loop to make
// sure that the receiver can safely delete the
// KOSpell object.
void KOSpell::emitDeath()
{
#if 0
  bool deleteMe = autoDelete; // Can't access object after next call!
  emit death();
  if (deleteMe)
     delete this;
#endif
}

void KOSpell::setProgressResolution (unsigned int res)
{
  progres=res;
}

void KOSpell::emitProgress ()
{
  uint nextprog = (uint) (100.*lastpos/(double)totalpos);

  if (nextprog>=curprog)
    {
      curprog=nextprog;
      emit progress (curprog);
    }
}

void KOSpell::moveDlg (int x, int y)
{
  QPoint pt (x,y), pt2;
  pt2=parent->mapToGlobal (pt);
  ksdlg->move (pt2.x(),pt2.y());
}

void KOSpell::setIgnoreUpperWords(bool _ignore)
{
    d->m_bIgnoreUpperWords=_ignore;
}

void KOSpell::setIgnoreTitleCase(bool _ignore)
{
    d->m_bIgnoreTitleCase=_ignore;
}

void KOSpell::changeSpellLanguage( int index )
{
    AspellConfig * config2 = aspell_config_clone(config);
    aspell_config_replace(config2, "lang",KOSpellConfig::listOfLanguageFileName()[index].latin1() );
    //possible_err = new_aspell_speller(spell_config2);
    delete_aspell_config(config2);
}


int KOSpell::modalCheck( QString& text )
{
    return modalCheck( text,0 );
}

int KOSpell::modalCheck( QString& text, KOSpellConfig* _kcs )
{
    modalreturn = 0;
    modaltext = text;

    KOSpell* m_spell = new KOSpell(0L, i18n("Spell Checker"), 0 ,_kcs, true, true );
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
    }

    text = modaltext;

    return modalreturn;
}

void KOSpell::slotSpellCheckerCorrected( const QString & oldText, const QString & newText, unsigned int pos )
{
    modaltext=modaltext.replace(pos,oldText.length(),newText);
}


void KOSpell::slotModalDone( const QString &/*_buffer*/ )
{
    slotModalSpellCheckerFinished();
}

void KOSpell::slotModalSpellCheckerFinished()
{
    modalreturn=(int)this->status();
}
QString KOSpell::modaltext;
int KOSpell::modalreturn = 0;


#include "koSpell.moc"

