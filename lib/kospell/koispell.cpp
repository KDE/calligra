/* This file is part of the KDE libraries
   Copyright (C) 2002-2003 Laurent Montel <lmontel@mandrakesoft.com>

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
#include "koispell.moc"
#include "koispell.h"
#include "koSconfig.h"

#include <kwin.h>
#include <kprocio.h>

#define MAXLINELENGTH 10000

enum {
	GOOD=     0,
	IGNORE=   1,
	REPLACE=  2,
	MISTAKE=  3
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


//  Connects a slot to KProcIO's output signal
#define OUTPUT(x) (connect (proc, SIGNAL (readReady(KProcIO *)), this, SLOT (x(KProcIO *))))

// Disconnect a slot from...
#define NOOUTPUT(x) (disconnect (proc, SIGNAL (readReady(KProcIO *)), this, SLOT (x(KProcIO *))))





KOISpell::KOISpell( QWidget *_parent, const QString &_caption,
		QObject *obj, const char *slot, KOSpellConfig *_ksc,
		bool _progressbar, bool _modal, KOSpellerType _type )
    :KOSpell(_parent,_caption,_ksc,_modal,/*_autocorrect*/false,  _type)
{
    initialize( _parent, _caption, obj, slot, _ksc,
              _progressbar, _modal );
}

void KOISpell::startIspell()
  //trystart = {0,1,2}
{

    kdDebug(30006) << "Try #" << trystart << endl;
    if (trystart>0)
        proc->resetAll();

    switch (ksconfig->client())
    {
    case KOS_CLIENT_ISPELL:
        *proc << "ispell";
        kdDebug(30006) << "Using ispell" << endl;
        break;
    case KOS_CLIENT_ASPELL:
        // This can only happen if HAVE_LIBASPELL isn't defined
        *proc << "aspell";
        kdDebug(30006) << "Using aspell" << endl;
        break;
    case KOS_CLIENT_HSPELL:
        *proc << "hspell";
        kdDebug(30006) << "Using hspell" << endl;
        break;
    default:
        kdError(30006) << "Spelling configuration error, client=" << ksconfig->client() <<endl;
    }

    // TODO: add option -h to ignore HTML (XML) code
    if (ksconfig->client() == KOS_CLIENT_ISPELL || ksconfig->client() == KOS_CLIENT_ASPELL)
    {
        *proc << "-a" << "-S";
        switch ( type )
        {
        case HTML:
            //Debian uses an ispell version that has the -h option instead.
            //Not sure what they did, but the prefered spell checker
            //on that platform is aspell anyway, so use -H untill I'll come
            //up with something better.
            *proc << "-H";
            break;
        case TeX:
            //same for aspell and ispell
            *proc << "-t";
            break;
        case Nroff:
            //only ispell supports
            if ( ksconfig->client() == KOS_CLIENT_ISPELL )
                *proc << "-n";
            break;
        case Text:
        default:
            //nothing
            break;
        }

        if (ksconfig->noRootAffix())
        {
            *proc<<"-m";
        }
        if (ksconfig->runTogether())
        {
            *proc << "-B";
        }
        else
        {
            *proc << "-C";
        }

        if (trystart<2)
        {
            if (! ksconfig->dictionary().isEmpty())
            {
                kdDebug(30006) << "using dictionary [" << ksconfig->dictionary() << "]" << endl;
                *proc << "-d";
                *proc << ksconfig->dictionary();
            }
        }

        //Note to potential debuggers:  -Tlatin2 _is_ being added on the
        //  _first_ try.  But, some versions of ispell will fail with this
        // option, so kspell tries again without it.  That's why as 'ps -ax'
        // shows "ispell -a -S ..." withou the "-Tlatin2" option.

        if (trystart<1)
            switch (ksconfig->encoding())
            {
            case KOS_E_LATIN1:
                *proc << "-Tlatin1";
                break;
            case KOS_E_LATIN2:
                *proc << "-Tlatin2";
                break;
            case KOS_E_LATIN3:
                *proc << "-Tlatin3";
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
                *proc << "-Tutf8";
                break;

            case KOS_E_KOI8U:
                *proc << "-w'"; // add ' as a word char
                break;

            }
        /*
          if (ksconfig->personalDict()[0]!='\0')
          {
          kdDebug(30006) << "personal dictionary [" << ksconfig->personalDict() << "]" << endl;
          *proc << "-p";
          *proc << ksconfig->personalDict();
          }
        */


        // -a : pipe mode
        // -S : sort suggestions by probable correctness
    }
    else       // hspell doesn't need all the rest of the options
        *proc << "-a";
    if (trystart==0) //don't connect these multiple times
    {
        connect (proc, SIGNAL (  receivedStderr (KProcess *, char *, int)),
                 this, SLOT (ispellErrors (KProcess *, char *, int)));


        connect(proc, SIGNAL(processExited(KProcess *)),
                this, SLOT (ispellExit (KProcess *)));

        OUTPUT(KSpell2);
    }

    if ( proc->start() == false )
    {
        m_status = Error;
        QTimer::singleShot( 0, this, SLOT(emitDeath()));
    }
}

QStringList KOISpell::resultCheckWord( const QString &_word )
{
    disconnect();
    checkWord (_word, false, true);
    QStringList sug = suggestions();
    return sug;
}


void KOISpell::ispellErrors (KProcess *, char *buffer, int buflen)
{
  buffer [buflen-1] = '\0';
  //kdDebug(30006) << "ispellErrors [" << buffer << "]\n" << endl;
}

void KOISpell::KSpell2 (KProcIO *)

{
    kdDebug(30006) << "KSpell::KSpell2" << endl;
  trystart=maxtrystart;  //We've officially started ispell and don't want
       //to try again if it dies.
  QString line;

  if (proc->readln (line, true)==-1)
  {
     QTimer::singleShot( 0, this, SLOT(emitDeath()));
     return;
  }


  if (line[0]!='@') //@ indicates that ispell is working fine
  {
     QTimer::singleShot( 0, this, SLOT(emitDeath()));
     return;
  }

  //We want to recognize KDE in any text!
  if (ignore ("kde")==false)
  {
     kdDebug(30006) << "@KDE was false" << endl;
     QTimer::singleShot( 0, this, SLOT(emitDeath()));
     return;
  }

  //We want to recognize linux in any text!
  if (ignore ("linux")==false)
  {
     kdDebug(30006) << "@Linux was false" << endl;
     QTimer::singleShot( 0, this, SLOT(emitDeath()));
     return;
  }

  NOOUTPUT (KSpell2);

  m_status = Running;
  m_ready = true;
  emit ready(this);
}

void
KOISpell::setUpDialog (bool reallyuseprogressbar)
{
  if (dialogsetup)
    return;

  //Set up the dialog box
  ksdlg=new KOSpellDlg (parent, ksconfig, "dialog",
		       progressbar && reallyuseprogressbar, modaldlg );
  ksdlg->setCaption (caption);
  connect (ksdlg, SIGNAL (command (int)), this,
		SLOT (slotStopCancel (int)) );
  /**connect (this, SIGNAL ( progress (unsigned int) ),
     ksdlg, SLOT ( slotProgress (unsigned int) ));*/
#ifdef Q_WS_X11 // FIXME(E): Implement for Qt/Embedded
  KWin::setIcons (ksdlg->winId(), kapp->icon(), kapp->miniIcon());
#endif
  if ( modaldlg )
      ksdlg->setFocus();
  dialogsetup = true;
}

bool KOISpell::addPersonal (const QString & word)
{
  QString qs = word.simplifyWhiteSpace();

  //we'll let ispell do the work here b/c we can
  if (qs.find (' ')!=-1 || qs.isEmpty())    // make sure it's a _word_
    return false;

  qs.prepend ("*");
  personaldict=true;

  return proc->writeStdin(qs);
}

bool KOISpell::writePersonalDictionary ()
{
  return proc->writeStdin ("#");
}

bool KOISpell::ignore (const QString & word)
{
  QString qs = word.simplifyWhiteSpace();

  //we'll let ispell do the work here b/c we can
  if (qs.find (' ')!=-1 || qs.isEmpty())    // make sure it's a _word_
    return false;

  qs.prepend ("@");

  return proc->writeStdin(qs);
}

bool
KOISpell::cleanFputsWord (const QString & s, bool appendCR)
{
  QString qs(s);
  //bool firstchar = true;
  bool empty = true;

  for (unsigned int i=0; i<qs.length(); i++)
  {
    //we need some punctuation for ornaments
    if (qs[i] != '\'' && qs[i] != '\"' && qs[i] != '-'
	&& qs[i].isPunct() || qs[i].isSpace())
    {
      qs.remove(i,1);
      i--;
    } else {
      if (qs[i].isLetter()) empty=false;
    }
  }

  // don't check empty words, otherwise synchronisation will lost
  if (empty) return false;

  return proc->writeStdin("^"+qs, appendCR);
}

bool
KOISpell::cleanFputs (const QString & s, bool appendCR)
{
  QString qs(s);
  unsigned l = qs.length();

  // some uses of '$' (e.g. "$0") cause ispell to skip all following text
  for(unsigned int i = 0; i < l; ++i)
  {
    if(qs[i] == '$')
      qs[i] = ' ';
  }

  if (l<MAXLINELENGTH)
    {
      if (qs.isEmpty())
	qs="";

      return proc->writeStdin ("^"+qs, appendCR);
    }
  else
    return proc->writeStdin (QString("^\n"),appendCR);
}

bool KOISpell::checkWord (const QString & buffer, bool _usedialog)
{
  QString qs = buffer.simplifyWhiteSpace();
  if (qs.find (' ')!=-1 || qs.isEmpty())    // make sure it's a _word_
    return false;

  ///set the dialog signal handler
  dialog3slot = SLOT(checkWord3());

  usedialog=_usedialog;
  setUpDialog(false);
  if (_usedialog)
    {
      emitProgress();
      ksdlg->show();
    }
  else
    ksdlg->hide();

  OUTPUT (checkWord2);
  //  connect (this, SIGNAL (dialog3()), this, SLOT (checkWord3()));

  proc->writeStdin ("%"); // turn off terse mode
  cleanFputsWord( qs ); // send the word to ispell

  return true;
}

//it can't use dialog anyway
bool KOISpell::checkWord (const QString & buffer, bool _usedialog, bool synchronous )
{
  QString qs = buffer.simplifyWhiteSpace();
  if (qs.find (' ')!=-1 || qs.isEmpty())    // make sure it's a _word_
    return false;

  ///set the dialog signal handler
  dialog3slot = SLOT(checkWord3());

  usedialog=_usedialog;
  setUpDialog(false);

  ksdlg->hide();
  if ( synchronous ) {
    //ready signal is never call, after initialize
    if ( !m_ready ) {
      connect( this, SIGNAL(ready(KOSpell*)),
               SLOT(slotSynchronousReady()) );
      //MAGIC 1: here we wait for the initialization to finish
      enter_loop();
    }
    OUTPUT (checkWord2Synchronous);
  }
  else
    OUTPUT (checkWord2);

  proc->writeStdin ("%"); // turn off terse mode
  cleanFputsWord( qs ); // send the word to ispell

  //MAGIC 2: and here we wait for the results
  enter_loop();
  return true;
}

void KOISpell::checkWord2 (KProcIO *)
{
  QString word;

  QString line;
  proc->readln (line, true); //get ispell's response

/* ispell man page: "Each sentence of text input is terminated with an
   additional blank line,  indicating that ispell has completed processing
   the input line." */
  QString blank_line;
  proc->readln(blank_line, true); // eat the blank line

  NOOUTPUT(checkWord2);

  bool mistake = (parseOneResponse(line, word, sugg) == MISTAKE);
  if ( mistake && usedialog )
    {
      cwword=word;
      dialog (word, sugg, SLOT (checkWord3()));
      return;
    }
  else if( mistake )
    {
      misspellingWord (word, sugg, lastpos);
    }

  //emits a "corrected" signal _even_ if no change was made
  //so that the calling program knows when the check is complete
  emit corrected (word, word, 0L);
}

// This is not even cute... Just watch me abuse
// Qt, KDE, candy, cookies and make this stuff work
// through pure magic
void KOISpell::checkWord2Synchronous (KProcIO *)
{
  QString word;

  QString line;
  proc->readln (line, true); //get ispell's response

/* ispell man page: "Each sentence of text input is terminated with an
   additional blank line,  indicating that ispell has completed processing
   the input line." */
  QString blank_line;
  proc->readln(blank_line, true); // eat the blank line

  NOOUTPUT(checkWord2);

  bool mistake = (parseOneResponse(line, word, sugg) == MISTAKE);
  if( mistake )
  {
    misspellingWord (word, sugg, lastpos);
  }
  //emits a "corrected" signal _even_ if no change was made
  //so that the calling program knows when the check is complete
  emit corrected (word, word, 0L);
  qApp->exit_loop();
}

void KOISpell::checkWord3 ()
{
  disconnect (this, SIGNAL (dialog3()), this, SLOT (checkWord3()));

  emit corrected (cwword, replacement(), 0L);
}

QString KOISpell::funnyWord (const QString & word)
  // composes a guess from ispell to a readable word
  // e.g. "re+fry-y+ies" -> "refries"
{
  QString qs;
  unsigned int i=0;

  for (i=0; word [i]!='\0';i++)
    {
      if (word [i]=='+')
	continue;
      if (word [i]=='-')
	{
	  QString shorty;
	  unsigned int j;
	  int k;

	  for (j=i+1;word [j]!='\0' && word [j]!='+' &&
		 word [j]!='-';j++)
	    shorty+=word [j];
	  i=j-1;

	  if ((k=qs.findRev (shorty))==0 || k!=-1)
	    qs.remove (k,shorty.length());
	  else
	    {
              qs+='-';
              qs+=shorty;  //it was a hyphen, not a '-' from ispell
            }
	}
      else
	qs+=word [i];
    }
  return qs;
}


int KOISpell::parseOneResponse (const QString &buffer, QString &word, QStringList & sugg)
  // buffer is checked, word and sugg are filled in
  // returns
  //   GOOD    if word is fine
  //   IGNORE  if word is in ignorelist
  //   REPLACE if word is in replacelist
  //   MISTAKE if word is misspelled
{
  word = "";
  posinline=0;

  sugg.clear();

  if (buffer [0]=='*' || buffer[0] == '+' || buffer[0] == '-')
    {
      return GOOD;
    }

  if (buffer [0]=='&' || buffer [0]=='?' || buffer [0]=='#')
    {
      int i,j;


      word = buffer.mid (2,buffer.find (' ',3)-2);
      //check() needs this
      orig=word;

      if(m_bIgnoreTitleCase && word==word.upper())
          return IGNORE;

      if(m_bIgnoreUpperWords && word[0]==word[0].upper())
      {
          QString text=word[0]+word.right(word.length()-1).lower();
          if(text==word)
              return IGNORE;
      }

      /////// Ignore-list stuff //////////
      //We don't take advantage of ispell's ignore function because
      //we can't interrupt ispell's output (when checking a large
      //buffer) to add a word to _it's_ ignore-list.
      if (ignorelist.findIndex(word.lower())!=-1)
	return IGNORE;

      //// Position in line ///
      QString qs2;

      if (buffer.find(':')!=-1)
	qs2=buffer.left (buffer.find (':'));
      else
	qs2=buffer;

      posinline = qs2.right( qs2.length()-qs2.findRev(' ') ).toInt()-1;

      ///// Replace-list stuff ////
      QStringList::Iterator it = replacelist.begin();
      for(;it != replacelist.end(); ++it, ++it) // Skip two entries at a time.
      {
         if (word == *it) // Word matches
         {
            ++it;
            word = *it;   // Replace it with the next entry
            return REPLACE;
	 }
      }

      /////// Suggestions //////
      if (buffer [0] != '#')
	{
	  QString qs = buffer.mid(buffer.find(':')+2, buffer.length());
	  qs+=',';
	  sugg.clear();
	  i=j=0;
	  while ((unsigned int)i<qs.length())
	    {
	      QString temp = qs.mid (i,(j=qs.find (',',i))-i);
	      sugg.append (funnyWord (temp));

	      i=j+2;
	    }
	}

      if ((sugg.count()==1) && (sugg.first() == word))
	return GOOD;

      return MISTAKE;
    }


  kdError(750) << "HERE?: [" << buffer << "]" << endl;
  kdError(750) << "Please report this to dsweet@kde.org" << endl;
  kdError(750) << "Thank you!" << endl;
  emit done(false);
  emit done (KOISpell::origbuffer);
  return MISTAKE;
}

bool KOISpell::checkList (QStringList *_wordlist, bool _usedialog)
  // prepare check of string list
{
  wordlist=_wordlist;
  if ((totalpos=wordlist->count())==0)
    return false;
  wlIt = wordlist->begin();
  usedialog=_usedialog;

  // prepare the dialog
  setUpDialog();

  //set the dialog signal handler
  dialog3slot = SLOT (checkList4 ());

  proc->writeStdin ("%"); // turn off terse mode & check one word at a time

  //lastpos now counts which *word number* we are at in checkListReplaceCurrent()
  lastpos = -1;
  checkList2();

  // when checked, KProcIO calls checkList3a
  OUTPUT(checkList3a);

  return true;
}

void KOISpell::checkList2 ()
  // send one word from the list to KProcIO
  // invoked first time by checkList, later by checkListReplaceCurrent and checkList4
{
  // send next word
  if (wlIt != wordlist->end())
    {
      kdDebug(30006) << "KS::cklist2 " << lastpos << ": " << *wlIt << endl;

      endOfResponse = false;
      bool put;
      lastpos++; offset=0;
      put = cleanFputsWord (*wlIt);
      ++wlIt;

      // when cleanFPutsWord failed (e.g. on empty word)
      // try next word; may be this is not good for other
      // problems, because this will make read the list up to the end
      if (!put) {
	checkList2();
      }
    }
  else
    // end of word list
    {
      NOOUTPUT(checkList3a);
      ksdlg->hide();
      emit done(true);
    }
}

void KOISpell::checkList3a (KProcIO *)
  // invoked by KProcIO, when data from ispell are read
{
  //kdDebug(30006) << "start of checkList3a" << endl;

  // don't read more data, when dialog is waiting
  // for user interaction
  if (dlgon) {
    //kdDebug(30006) << "dlgon: don't read more data" << endl;
    return;
  }

  int e, tempe;

  QString word;
  QString line;

    do
      {
	tempe=proc->readln (line, true); //get ispell's response

	//kdDebug(30006) << "checkList3a: read bytes [" << tempe << "]" << endl;


	if (tempe == 0) {
	  endOfResponse = true;
	  //kdDebug(30006) << "checkList3a: end of resp" << endl;
	} else if (tempe>0) {
	  if ((e=parseOneResponse (line, word, sugg))==MISTAKE ||
	      e==REPLACE)
	    {
	      dlgresult=-1;

	      if (e==REPLACE)
		{
		  QString old = *(--wlIt); ++wlIt;
		  dlgreplacement=word;
		  checkListReplaceCurrent();
		  // inform application
		  emit corrected (old, *(--wlIt), lastpos); ++wlIt;
		}
	      else if( usedialog )
		{
		  cwword=word;
		  dlgon=true;
		  // show the dialog
		  dialog (word, sugg, SLOT (checkList4()));
		  return;
		}
	      else
		{
		  misspellingWord (word, sugg, lastpos);
 		}
	    }

	}
      	emitProgress (); //maybe

	// stop when empty line or no more data
      } while (tempe > 0);

    //kdDebug(30006) << "checkList3a: exit loop with [" << tempe << "]" << endl;

    // if we got an empty line, t.e. end of ispell/aspell response
    // and the dialog isn't waiting for user interaction, send next word
    if (endOfResponse && !dlgon) {
      //kdDebug(30006) << "checkList3a: send next word" << endl;
      checkList2();
    }
}

void KOISpell::checkListReplaceCurrent () {

  // go back to misspelled word
  wlIt--;

  QString s = *wlIt;
  s.replace(posinline+offset,orig.length(),replacement());
  offset += replacement().length()-orig.length();
  wordlist->insert (wlIt, s);
  wlIt = wordlist->remove (wlIt);
  // wlIt now points to the word after the repalced one

}

void KOISpell::checkList4 ()
  // evaluate dialog return, when a button was pressed there
{
  dlgon=false;
  QString old;

  disconnect (this, SIGNAL (dialog3()), this, SLOT (checkList4()));

  //others should have been processed by dialog() already
  switch (dlgresult)
    {
    case KOS_REPLACE:
    case KOS_REPLACEALL:
      kdDebug(30006) << "KS: cklist4: lastpos: " << lastpos << endl;
      old = *(--wlIt); ++wlIt;
      // replace word
      checkListReplaceCurrent();
      emit corrected (old, *(--wlIt), lastpos); ++wlIt;
      break;
    case KOS_CANCEL:
      ksdlg->hide();
      emit done (false);
      return;
    case KOS_STOP:
      ksdlg->hide();
      emit done (true);
      break;
    };

  // read more if there is more, otherwise send next word
  if (!endOfResponse) {
    //kdDebug(30006) << "checkList4: read more from response" << endl;
      checkList3a(NULL);
  }
}

bool KOISpell::check( const QString &_buffer, bool _usedialog )
{
  QString qs;

  usedialog=_usedialog;
  setUpDialog ();
  //set the dialog signal handler
  dialog3slot = SLOT (check3 ());

  kdDebug(30006) << "KS: check" << endl;
  origbuffer = _buffer;
  if ( ( totalpos = origbuffer.length() ) == 0 )
    {
      emit done(origbuffer);
      return false;
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

  // KProcIO calls check2 when read from ispell
  OUTPUT(check2);
  proc->writeStdin ("!");

  //lastpos is a position in newbuffer (it has offset in it)
  offset=lastlastline=lastpos=lastline=0;

  emitProgress ();

  // send first buffer line
  int i = origbuffer.find('\n', 0)+1;
  qs=origbuffer.mid (0,i);
  cleanFputs (qs,false);

  lastline=i; //the character position, not a line number

  if (usedialog)
    {
      emitProgress();
      ksdlg->show();
    }
  else
    ksdlg->hide();

  return true;
}

void KOISpell::check2 (KProcIO *)
  // invoked by KProcIO when read from ispell
{
  int e, tempe;
  QString word;
  QString line;
  static bool recursive = false;
  if (recursive &&
      (!ksdlg || ksdlg->isHidden()))
  {
    return;
  }
  recursive = true;

  do
    {
      tempe=proc->readln (line, false); //get ispell's response
      kdDebug(30006) << "KSpell::check2 (" << tempe << "b)" << endl;

      if (tempe>0)
	{
	  if ((e=parseOneResponse (line, word, sugg))==MISTAKE ||
	      e==REPLACE)
	    {
	      dlgresult=-1;

	      // for multibyte encoding posinline needs correction
	      if (ksconfig->encoding() == KOS_E_UTF8) {
		// kdDebug(30006) << "line: " << origbuffer.mid(lastlastline,
		// lastline-lastlastline) << endl;
		// kdDebug(30006) << "posinline uncorr: " << posinline << endl;

		// convert line to UTF-8, cut at pos, convert back to UCS-2
		// and get string length
		posinline = (QString::fromUtf8(
		   origbuffer.mid(lastlastline,lastline-lastlastline).utf8(),
		   posinline)).length();
		// kdDebug(30006) << "posinline corr: " << posinline << endl;
	      }

	      lastpos=posinline+lastlastline+offset;

	      //orig is set by parseOneResponse()

	      if (e==REPLACE)
		{
		  dlgreplacement=word;
		  emit corrected (orig, replacement(), lastpos);
		  offset+=replacement().length()-orig.length();
		  newbuffer.replace (lastpos, orig.length(), word);
		}
	      else  //MISTAKE
		{
		  cwword=word;
		  //kdDebug(30006) << "(Before dialog) word=[" << word << "] cwword =[" << cwword << "]\n" << endl;
                  if ( usedialog ) {
                      // show the word in the dialog
                      dialog (word, sugg, SLOT (check3()));
                  } else {
                      // No dialog, just emit misspelling and continue
                      misspellingWord (word, sugg, lastpos);
                      dlgresult = KOS_IGNORE;
                      check3();
                  }
                  recursive = false;
		  return;
		}
	    }

	  }

      emitProgress (); //maybe

    } while (tempe>0);

  proc->ackRead();

  if (tempe==-1) {//we were called, but no data seems to be ready...
    recursive = false;
    return;
  }

  proc->ackRead();
  //If there is more to check, then send another line to ISpell.
  if ((unsigned int)lastline<origbuffer.length())
    {
      int i;
      QString qs;

      //kdDebug(30006) << "[EOL](" << tempe << ")[" << temp << "]" << endl;

      lastpos=(lastlastline=lastline)+offset; //do we really want this?
      i=origbuffer.find('\n', lastline)+1;
      qs=origbuffer.mid (lastline, i-lastline);
      cleanFputs (qs,false);
      lastline=i;
      recursive = false;
      return;
    }
  else
  //This is the end of it all
    {
      ksdlg->hide();
      //      kdDebug(30006) << "check2() done" << endl;
      newbuffer.truncate (newbuffer.length()-2);
      emitProgress();
      emit done (newbuffer);
    }
  recursive = false;
}

void KOISpell::check3 ()
  // evaluates the return value of the dialog
{
  disconnect (this, SIGNAL (dialog3()), this, SLOT (check3()));

  kdDebug(30006) << "check3 [" << cwword << "] [" << replacement() << "] " << dlgresult << endl;

  //others should have been processed by dialog() already
  switch (dlgresult)
    {
    case KOS_REPLACE:
    case KOS_REPLACEALL:
      offset+=replacement().length()-cwword.length();
      newbuffer.replace (lastpos, cwword.length(),
			 replacement());
      emit corrected (dlgorigword, replacement(), lastpos);
      break;
    case KOS_CANCEL:
    //      kdDebug(30006) << "cancelled\n" << endl;
      ksdlg->hide();
      emit done (origbuffer);
      return;
    case KOS_STOP:
      ksdlg->hide();
      //buffer=newbuffer);
      emitProgress();
      emit done (newbuffer);
      return;
    };

  proc->ackRead();
}

void
KOISpell::slotStopCancel (int result)
{
  if (dialogwillprocess)
    return;

  kdDebug(30006) << "KSpell::slotStopCancel [" << result << "]" << endl;

  if (result==KOS_STOP || result==KOS_CANCEL)
    if (!dialog3slot.isEmpty())
      {
	dlgresult=result;
	connect (this, SIGNAL (dialog3()), this, dialog3slot.ascii());
	emit dialog3();
      }
}


void KOISpell::dialog(const QString & word, QStringList & sugg, const char *_slot)
{
  dlgorigword=word;

  dialog3slot=_slot;
  dialogwillprocess=true;
  connect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
  ksdlg->init (word, &sugg);
  misspellingWord (word, sugg, lastpos);

  emitProgress();
  ksdlg->show();
}

void KOISpell::dialog2 (int result)
{
  QString qs;

  disconnect (ksdlg, SIGNAL (command (int)), this, SLOT (dialog2(int)));
  dialogwillprocess=false;
  dlgresult=result;
  ksdlg->standby();

  dlgreplacement=ksdlg->replacement();

  //process result here
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
      personaldict=true;
      emit addword (dlgorigword);
      // adding to pesonal dict takes effect at the next line, not the current
      ignorelist.prepend(dlgorigword.lower());
      break;
    case KOS_REPLACEALL:
    {
      replacelist.append (dlgorigword);
      QString _replacement = replacement();
      replacelist.append (_replacement);
      emit replaceall( dlgorigword ,  _replacement );
    }
    break;
    case KOS_ADDAUTOCORRECT:
    {
        //todo add new word ????
        QString _replacement = replacement();
        emit addAutoCorrect (dlgorigword , _replacement);
        break;
    }
    }

  connect (this, SIGNAL (dialog3()), this, dialog3slot.ascii());
  emit dialog3();
}


KOISpell:: ~KOISpell ()
{
    delete proc;
}


void KOISpell::cleanUp ()
{
  if (m_status == Cleaning) return; // Ignore
  if (m_status == Running)
  {
    if (personaldict)
       writePersonalDictionary();
    m_status = Cleaning;
  }
  proc->closeStdin();
}

void KOISpell::ispellExit (KProcess *)
{
  kdDebug(30006) << "KSpell::ispellExit() " << m_status << endl;

  if ((m_status == Starting) && (trystart<maxtrystart))
  {
    trystart++;
    startIspell();
    return;
  }

  if (m_status == Starting)
     m_status = Error;
  else if (m_status == Cleaning)
      m_status = m_bNoMisspellingsEncountered ? FinishedNoMisspellingsEncountered : Finished;
  else if (m_status == Running)
     m_status = Crashed;
  else // Error, Finished, Crashed
     return; // Dead already

  kdDebug(30006) << "Death" << endl;
  QTimer::singleShot( 0, this, SLOT(emitDeath()));
}

// This is always called from the event loop to make
// sure that the receiver can safely delete the
// KOISpell object.
void KOISpell::emitDeath()
{
  bool deleteMe = autoDelete; // Can't access object after next call!
  emit death();
  if (deleteMe)
     deleteLater();
}

void KOISpell::setProgressResolution (unsigned int res)
{
  progres=res;
}

void KOISpell::emitProgress ()
{
  uint nextprog = (uint) (100.*lastpos/(double)totalpos);

  if (nextprog>=curprog)
    {
      curprog=nextprog;
      emit progress (curprog);
    }
}

// --------------------------------------------------
// Stuff for modal (blocking) spell checking
//
// Written by Torben Weis <weis@kde.org>. So please
// send bug reports regarding the modal stuff to me.
// --------------------------------------------------


int
KOISpell::modalCheck( QString& text, KOSpellConfig* _kcs )
{
    modalreturn = 0;
    modaltext = text;


    // kdDebug(30006) << "KOISpell1" << endl;
    KOISpell* spell = new KOISpell( 0L, i18n("Spell Checker"), 0 ,
				0, _kcs, true, true );
    //qApp->enter_loop();

    while (spell->status()!=Finished)
      kapp->processEvents();

    text = modaltext;


    delete spell;
    return modalreturn;
}

void KOISpell::slotSpellCheckerCorrected( const QString & oldText, const QString & newText, unsigned int pos )
{
    modaltext=modaltext.replace(pos,oldText.length(),newText);
}


void KOISpell::slotModalReady()
{
    //kdDebug(30006) << qApp->loopLevel() << endl;
    //kdDebug(30006) << "MODAL READY------------------" << endl;

    Q_ASSERT( m_status == Running );
    connect( this, SIGNAL( done( const QString & ) ),
             this, SLOT( slotModalDone( const QString & ) ) );
    QObject::connect( this, SIGNAL( corrected( const QString&, const QString&, unsigned int ) ),
                      this, SLOT( slotSpellCheckerCorrected( const QString&, const QString &, unsigned int ) ) );
     QObject::connect( this, SIGNAL( death() ),
                      this, SLOT( slotModalSpellCheckerFinished( ) ) );
    check( modaltext );
}

void KOISpell::slotModalDone( const QString &/*_buffer*/ )
{
    //kdDebug(30006) << "MODAL DONE " << _buffer << endl;
    //modaltext = _buffer;
    cleanUp();

    //kdDebug(30006) << "ABOUT TO EXIT LOOP" << endl;
    //qApp->exit_loop();

    slotModalSpellCheckerFinished();
}

void KOISpell::slotModalSpellCheckerFinished( )
{
    modalreturn=(int)this->status();
}


void KOISpell::initialize( QWidget *_parent, const QString &_caption,
                         QObject *obj, const char *slot, KOSpellConfig *_ksc,
                         bool _progressbar, bool _modal )
{
  m_ready = false;
  m_bIgnoreUpperWords=false;
  m_bIgnoreTitleCase=false;

  autoDelete = false;
  modaldlg = _modal;
  progressbar = _progressbar;

  proc=0;
  ksdlg=0;

  texmode=dlgon=false;

  dialogsetup = false;
  progres=10;
  curprog=0;

  dialogwillprocess=false;
  dialog3slot="";

  personaldict=false;
  dlgresult=-1;

  caption=_caption;

  parent=_parent;

  trystart=0;
  maxtrystart=2;

  if ( obj && slot )
      // caller wants to know when kspell is ready
      connect (this, SIGNAL (ready(KOSpell *)), obj, slot);
  else
      // Hack for modal spell checking
      connect (this, SIGNAL (ready(KOSpell *)), this, SLOT( slotModalReady() ) );
  proc=new KProcIO(codec);

  startIspell();
}

// This is retarded, if you don't get it, don't worry
// it's me working around 999999999 problems
void qt_enter_modal( QWidget *widget );
void qt_leave_modal( QWidget *widget );
void KOISpell::enter_loop()
{
  QWidget dummy(0,0,WType_Dialog | WShowModal);
  dummy.setFocusPolicy( QWidget::NoFocus );
  qt_enter_modal(&dummy);
  qApp->enter_loop();
  qt_leave_modal(&dummy);
}

void KOISpell::slotSynchronousReady()
{
  qApp->exit_loop();
}
