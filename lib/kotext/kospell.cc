/* This file is part of the KDE libraries
   Copyright (C) 1997 David Sweet <dsweet@kde.org>
   Copyright (C) 2000-2001 Wolfram Diestel <wolfram@steloj.de>

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

//#include <qptrqueue.h>
#include <qtextcodec.h>
#include <qtimer.h>
#include <kdebug.h>
#include <klocale.h>
#include <kprocio.h>
#include "kospell.h"
#include <qfileinfo.h>
#include <qdir.h>
#include <kglobal.h>
#define MAXLINELENGTH 10000

class KoSpell::KoSpellPrivate
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


//  Connects a slot to KProcIO's output signal
#define OUTPUT(x) (connect (proc, SIGNAL (readReady(KProcIO *)), this, SLOT (x(KProcIO *))))

// Disconnect a slot from...
#define NOOUTPUT(x) (disconnect (proc, SIGNAL (readReady(KProcIO *)), this, SLOT (x(KProcIO *))))



KoSpell::KoSpell(QWidget */*_parent*/, QObject *obj, const char *slot, KSpellConfig *_ksc)
{
	d=new KoSpellPrivate;

	d->m_bIgnoreUpperWords=false;
	d->m_bIgnoreTitleCase=false;

	proc=0;
	ksconfig=0;
	//won't be using the dialog in ksconfig, just the option values
	if (_ksc!=0)
		ksconfig = new KSpellConfig(*_ksc);
	else
		ksconfig = new KSpellConfig;
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
		default:
			break;
	}

	kdDebug(32500) << __FILE__ << ":" << __LINE__ << " Codec = " << (codec ? codec->name() : "<default>") << endl;

	m_status = Starting;

        ignorelist += ksconfig->ignoreList();

	// caller wants to know when kspell is ready
	if ( obj && slot )
		connect (this, SIGNAL(ready(KoSpell *)), obj, slot);

	proc=new KProcIO(codec);

	trystart=0;
	maxtrystart=2;
	startIspell();
}

//trystart = {0,1,2}
void KoSpell::startIspell()
{
	kdDebug(32500) << "Try #" << trystart << endl;
	if (trystart>0)
		proc->resetAll();

	switch (ksconfig->client())
	{
		case KS_CLIENT_ISPELL:
			*proc << "ispell";
			kdDebug(32500) << "Using ispell" << endl;
			break;
		case KS_CLIENT_ASPELL:
			*proc << "aspell";
			kdDebug(32500) << "Using aspell" << endl;
			break;
	}

	/* ispell(1):
	 *	-a     ispell used from other programs through a pipe.
	 *	-S     Sort the list of guesses by probable correctness.
	 *	-B     Report run-together words with missing blanks as spelling errors.
	 *	-C     Consider run-together words as legal compounds.
	 *	-m     Make possible root/affix combinations that aren't in the dictionary.
	 */

	*proc << "-a" << "-S";
	if (ksconfig->noRootAffix())
		*proc<<"-m";

	if (ksconfig->runTogether())
		*proc << "-B";
	else
		*proc << "-C";

	if (trystart<2)
	{
		if (! ksconfig->dictionary().isEmpty())
		{
			kdDebug(32500) << "using dictionary [" << ksconfig->dictionary() << "]" << endl;
			*proc << "-d";
			*proc << ksconfig->dictionary();
		}
	}

	// Note to potential debuggers:  -T<codec> _is_ being added on the
	// _first_ try.  But, some versions of ispell will fail with this
	// option, so kspell tries again without it.  That's why as 'ps -ax'
	// shows "ispell -a -S ..." withou the "-T<codec>" option.

	if (trystart<1)
		switch (ksconfig->encoding())
		{
			case KS_E_LATIN1:
				*proc << "-Tlatin1";
				break;
			case KS_E_LATIN2:
				*proc << "-Tlatin2";
				break;
			case KS_E_LATIN3:
				*proc << "-Tlatin3";
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
				kdError() << "charsets iso-8859-4 .. iso-8859-15 not supported yet" << endl;
				break;

			case KS_E_UTF8:
				*proc << "-Tutf8";
				break;

			case KS_E_KOI8U:
				*proc << "-w'"; // add ' as a word char
				break;
		}

	if(trystart==0) //don't connect these multiple times
	{
		connect(proc, SIGNAL (receivedStderr (KProcess *, char *, int)),
			this, SLOT (ispellErrors (KProcess *, char *, int)));

		connect(proc, SIGNAL(processExited(KProcess *)),
			this, SLOT (ispellExit (KProcess *)));

		OUTPUT(KoSpell2);
	}

	if(!proc->start())
	{
		m_status = Error;
		QTimer::singleShot(0, this, SLOT(emitDeath()));
	}
}

void KoSpell::ispellErrors(KProcess *, char *buffer, int buflen)
{
	buffer [buflen-1] = '\0';
	kdDebug(32500) << "ispellErrors [" << buffer << "]\n" << endl;
}

void KoSpell::KoSpell2 (KProcIO *)
{
	kdDebug(32500) << "KoSpell::KoSpell2" << endl;

	QString line;

	if(proc->fgets(line, true)==-1)
	{
		QTimer::singleShot(0, this, SLOT(emitDeath()));
		return;
	}

	if(line[0]!='@') //@ indicates that ispell is working fine
	{
		QTimer::singleShot(0, this, SLOT(emitDeath()));
		return;
	}

	// put ispell in "terse-mode" -- not outputting a '*' for each correct word
	proc->fputs("!");

	NOOUTPUT (KoSpell2);
	OUTPUT(check2);

	m_status = Running;
	emit ready(this);
}

bool KoSpell::addPersonal(const QString & word)
{
	QString w = word;

	//we'll let ispell do the work here b/c we can
	if(w.find (' ')!=-1 || w.isEmpty())    // make sure it's a _word_
		return false;

	w.prepend ("*");
	w.append( "\n#" ); // save immediately, there's no time on destruction

	return proc->fputs(w);
}

bool KoSpell::writePersonalDictionary()
{
	return proc->fputs("#");
}

bool KoSpell::ignore(const QString & word)
{
	QString qs = word.simplifyWhiteSpace();

	//we'll let ispell do the work here b/c we can
	if (qs.find (' ')!=-1 || qs.isEmpty())    // make sure it's a _word_
		return FALSE;

	qs.prepend ("@");

	return proc->fputs(qs);
}

// composes a guess from ispell to a readable word
// e.g. "re+fry-y+ies" -> "refries"
// BuTi: i don't believe that this makes sense!
QString KoSpell::funnyWord(const QString & word)
{
	QString qs;
	unsigned int i=0;

	for (i=0; i<word.length(); i++)
	{
		if (word[i]=='+')
			continue;

		if (word [i]=='-')
		{
			QString shorty;
			unsigned int j;
			int k;

			for(j=i+1; j<word.length() && word [j]!='+' && word [j]!='-'; j++)
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

KoSpell::Spelling KoSpell::parseLine(const QString &line, QString &word, int &pos)
{
    bool skip = false;

    //kdDebug(32500) << "KoSpell::parseLine(\"" << line << "\")" << endl;
    if(line.isEmpty())
        return SpellingDone;

    QChar ch = line[0];
    switch(ch)
    {
    case '*':
    case '+':
    case '-':
        return SpellingOk;
    case '&':
    case '?':
        skip = true;
    case '#':
    {
        int p		= line.find(QChar(' '), 2);
        word		= line.mid(2, p-2);
        p++;
        if(skip)
        {
            while(line[p].isDigit())
                p++;
            p++;
        }
        int l=0;
        while(line[p+l].isDigit())
            l++;
        bool ok=true;
        pos = line.mid(p,l).toInt(&ok);
        //kdDebug(32500) << "	pos=" << pos << " [" << line.mid(p,l) << "]" << endl;
//			if(!ok)
//				return SpellingError;
        return Misspelled;
    }
    default:
        return SpellingError;
    }
    return SpellingError;
}

bool KoSpell::check(const QString &buffer)
{
	//kdDebug(32500) << "KoSpell::check(\"" << buffer << "\")" << endl;
	if(buffer.isEmpty())
	{
            //kdDebug(32500) << "Empty -> KoSpell::done()" << endl;
            emit done();
            return true;
	}

        // replace '\n' with ' '
        QString buf( buffer );
        buf.replace( '\n', ' ' );

	// we need a fifo here !!
	m_buffer << buf;

	proc->fputs("^", false);
	proc->fputs(buf);

	return true;
}

// invoked by KProcIO when read from ispell
void KoSpell::check2(KProcIO *)
{
    //kdDebug(32500) << "KoSpell::check2()" << endl;
    QString line;
    int bytes;
    while((bytes=proc->fgets(line, true)) >= 0)
    {

        /* UTF-8 encoding
         * source: http://www.cl.cam.ac.uk/~mgk25/unicode.html
         *
         * U-00000000 - U-0000007F: 	0xxxxxxx
         * U-00000080 - U-000007FF:	110xxxxx 10xxxxxx
         * U-00000800 - U-0000FFFF:	1110xxxx 10xxxxxx 10xxxxxx
         * U-00010000 - U-001FFFFF:	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
         * U-00200000 - U-03FFFFFF:	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
         * U-04000000 - U-7FFFFFFF:	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
         */
        int pos=0;
        QString word;
        Spelling spelling = parseLine(line, word, pos);
        if(word.length()>1 && d->m_bIgnoreTitleCase && word==word.upper())
        {
            spelling=SpellingIgnore;
        }

        if(word.length()>1 && d->m_bIgnoreUpperWords && word[0]==word[0].upper())
        {
            QString text=word[0]+word.right(word.length()-1).lower();
            if(text==word)
                spelling=SpellingIgnore;
        }
        if (ignorelist.findIndex(word.lower())!=-1)
            spelling=SpellingIgnore;

        switch(spelling)
        {
        case Misspelled:
        {
            QString buffer = m_buffer.front();
            pos--;	// for the '^' we sent, which ispell also counts as 1 char
            // UTF8 is a multi-byte charset, adjust pos accordingly, without converting the whole string
            // (like kspell does)
            if ( ksconfig->encoding() == KS_E_UTF8 )
            {
                for(int i=0; i < pos; i++)
                {
                    ushort u = buffer[i].unicode();
                    if(u > 0x7f)
                        pos--;
                    else if(u > 0x7ff)
                        pos-=2;
                    /* ushort can't hold more than that, right?
                       else if(u > 0xffff)
                       pos-=3;
                    */
                }
            }
//            kdDebug(32500) << "KoSpell::misspelling(" << word << ", " << pos << ")" << endl;
            emit misspelling(word, pos);
            break;
        }

        case SpellingDone:
            //kdDebug(32500) << "KoSpell::check2() DONE" << endl;
            Q_ASSERT(!m_buffer.isEmpty());
            if (!m_buffer.isEmpty())
                m_buffer.pop_front();
            emit done();
            break;
        case SpellingIgnore:
            break;
        default:
            kdDebug(32500) << "KoSpell::check2() ERROR" << endl;
            break;
        }
    }

//	proc->ackRead();
}

KoSpell:: ~KoSpell ()
{
    delete d;
    delete proc;
    delete ksconfig;
}

KSpellConfig KoSpell::ksConfig () const
{
    ksconfig->setIgnoreList(ignorelist);
  return *ksconfig;
}

void KoSpell::cleanUp ()
{
  if (m_status == Cleaning) return; // Ignore
  if (m_status == Running)
  {
    m_status = Cleaning;
  }
  proc->closeStdin();
}

void KoSpell::ispellExit(KProcess *)
{
  kdDebug(32500) << "KoSpell::ispellExit() " << m_status << endl;

  if ((m_status == Starting) && (trystart<maxtrystart))
  {
    trystart++;
    startIspell();
    return;
  }

  if (m_status == Starting)
     m_status = Error;
  else if (m_status == Cleaning)
     m_status = Finished;
  else if (m_status == Running)
     m_status = Crashed;
  else // Error, Finished, Crashed
     return; // Dead already

  kdDebug(32500) << "Death" << endl;
  QTimer::singleShot( 0, this, SLOT(emitDeath()));
}

// This is always called from the event loop to make
// sure that the receiver can safely delete the
// KSpell object.
void KoSpell::emitDeath()
{
//  bool deleteMe = autoDelete; // Can't access object after next call!
  emit death();
//  if (deleteMe)
//     delete this;
}

void KoSpell::setIgnoreUpperWords(bool _ignore)
{
    d->m_bIgnoreUpperWords=_ignore;
}

void KoSpell::setIgnoreTitleCase(bool _ignore)
{
    d->m_bIgnoreTitleCase=_ignore;
}

//duplicate code from ksconfig
//remove it when we use kde3.1
QStringList KoSpell::getAvailDictsIspell ()
{
    QStringList listIspell;
    // dictionary path
    QFileInfo dir ("/usr/lib/ispell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/local/lib/ispell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/local/share/ispell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/share/ispell");
    /* TODO get them all instead of just one of them.
     * If /usr/local/lib exists, it skips the rest
     if (!dir.exists() || !dir.isDir())
     dir.setFile ("/usr/local/lib");
    */
    if (!dir.exists() || !dir.isDir()) return QStringList();

    kdDebug(32500) << "KoSpell::getAvailDictsIspell "
              << dir.filePath() << " " << dir.dirPath() << endl;

    QDir thedir (dir.filePath(),"*.hash");

    kdDebug(32500) << "KoSpell" << thedir.path() << "\n" << endl;
    kdDebug(32500) << "entryList().count()="
              << thedir.entryList().count() << endl;

    for (unsigned int i=0;i<thedir.entryList().count();i++)
    {
        QString fname, lname, hname;
        fname = thedir [i];

        // remove .hash
        if (fname.right(5) == ".hash") fname.remove (fname.length()-5,5);

        if (interpret (fname, lname, hname))
	{
            hname=i18n("default spelling dictionary"
                       ,"Default - %1 [%2]").arg(hname).arg(fname);
            listIspell.append(hname);
	}
        else
	{
            hname=hname+" ["+fname+"]";
            listIspell.append(hname);
	}
    }
    return listIspell;
}

QStringList KoSpell::getAvailDictsAspell () {

    QStringList listAspell;

    // dictionary path
    // FIXME: use "aspell dump config" to find out the dict-dir
    QFileInfo dir ("/usr/lib/aspell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/local/lib/aspell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/share/aspell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/local/share/aspell");
    if (!dir.exists() || !dir.isDir()) return QStringList();

    kdDebug(32500) << "KoSpell::getAvailDictsAspell "
                 << dir.filePath() << " " << dir.dirPath() << endl;

    QDir thedir (dir.filePath(),"*");

    kdDebug(32500) << "KSpellConfig" << thedir.path() << "\n" << endl;
    kdDebug(32500) << "entryList().count()="
                 << thedir.entryList().count() << endl;

    for (unsigned int i=0; i<thedir.entryList().count(); i++)
    {
        QString fname, lname, hname;
        fname = thedir [i];

        // consider only simple dicts without '-' in the name
        // FIXME: may be this is wrong an the list should contain
        // all *.multi files too, to allow using special dictionaries
        if (fname[0] != '.' &&  fname.find('-') < 0)
	{

            // remove .multi
            if (fname.right(6) == ".multi") fname.remove (fname.length()-6,6);

            if (interpret (fname, lname, hname))
	    {
                hname=i18n("default spelling dictionary"
                           ,"Default - %1 [%2]").arg(hname).arg(fname);
                listAspell.append(hname);
    	    }
            else
	    {
                hname=hname+" ["+fname+"]";
                listAspell.append(hname);
	    }
	}
    }
    return listAspell;
}


bool
KoSpell::interpret (QString &fname, QString &lname,
			      QString &hname)

{

  kdDebug(750) << "KSpellConfig::interpret [" << fname << "]" << endl;

  QString dname(fname);

  if(dname.right(1)=="+")
    dname.remove(dname.length()-1, 1);

  if(dname.right(3)=="sml" || dname.right(3)=="med" || dname.right(3)=="lrg" || dname.right(3)=="xlg")
     dname.remove(dname.length()-3,3);

  //These are mostly the ispell-langpack defaults
  if (dname=="english" || dname=="american" ||
      dname=="british" || dname=="canadian") {
    lname="en"; hname=i18n("English");
  }
  else if (dname=="espa~nol" || dname=="espanol") {
    lname="es"; hname=i18n("Spanish");
  }
  else if (dname=="dansk") {
    lname="da"; hname=i18n("Danish");
  }
  else if (dname=="deutsch") {
    lname="de"; hname=i18n("German");
  }
  else if (dname=="german") {
    lname="de"; hname=i18n("German (new orth.)");
  }
  else if (dname=="portuguesb" || dname=="br") {
    lname="br"; hname=i18n("Brazilian Portuguese");
  }
  else if (dname=="portugues") {
    lname="pt"; hname=i18n("Portuguese");
  }
  else if (dname=="esperanto") {
    lname="eo"; hname=i18n("Esperanto");
  }
  else if (dname=="norsk") {
    lname="no"; hname=i18n("Norwegian");
  }
  else if (dname=="polish") {
    lname="pl"; hname=i18n("Polish");
  }
  else if (dname=="russian") {
    lname="ru"; hname=i18n("Russian");
  }
  else if (dname=="slovensko") {
    lname="si"; hname=i18n("Slovenian");
  }
  else if (dname=="slovak"){
    lname="sk"; hname=i18n("Slovak");
  }
  else if (dname=="czech") {
    lname="cs"; hname=i18n("Czech");
  }
  else if (dname=="svenska") {
    lname="sv"; hname=i18n("Swedish");
  }
  else if (dname=="swiss") {
    lname="de"; hname=i18n("Swiss German");
  }
  else if (dname=="ukrainian") {
    lname="uk"; hname=i18n("Ukrainian");
  }
  else if (dname=="lietuviu" || dname=="lithuanian") {
     lname="lt"; hname=i18n("Lithuanian");
  }
  else if (dname=="francais" || dname=="french") {
    lname="fr"; hname=i18n("French");
  }
  else if (dname=="belarusian") {  // waiting for post 2.2 to not dissapoint translators
    lname="be"; hname=i18n("Belarusian");
  }
  else if( dname == "magyar" ) {
    lname="hu"; hname=i18n("Hungarian");
  }
  else {
    lname=""; hname=i18n("Unknown ispell dictionary", "Unknown");
  }

  //We have explicitly chosen English as the default here.
  if ( (KGlobal::locale()->language()==QString::fromLatin1("C") &&
	lname==QString::fromLatin1("en")) ||
       KGlobal::locale()->language()==lname)
    return TRUE;

  return FALSE;
}


#include "kospell.moc"

