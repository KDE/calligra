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



KoSpell::KoSpell(QWidget *_parent, QObject *obj, const char *slot, KSpellConfig *_ksc)
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

	kdDebug() << __FILE__ << ":" << __LINE__ << " Codec = " << (codec ? codec->name() : "<default>") << endl;

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
	kdDebug() << "Try #" << trystart << endl;
	if (trystart>0)
		proc->resetAll();

	switch (ksconfig->client())
	{
		case KS_CLIENT_ISPELL:
			*proc << "ispell";
			kdDebug() << "Using ispell" << endl;
			break;
		case KS_CLIENT_ASPELL:
			*proc << "aspell";
			kdDebug() << "Using aspell" << endl;
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
			kdDebug() << "using dictionary [" << ksconfig->dictionary() << "]" << endl;
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
	kdDebug() << "ispellErrors [" << buffer << "]\n" << endl;
}

void KoSpell::KoSpell2 (KProcIO *)
{
	kdDebug() << "KoSpell::KoSpell2" << endl;

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

	// put ispell in "terse-mode" -- not outputing a '*' for each correct word
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

    //kdDebug() << "KoSpell::parseLine(\"" << line << "\")" << endl;
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
        //kdDebug() << "	pos=" << pos << " [" << line.mid(p,l) << "]" << endl;
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
//	kdDebug() << "KoSpell::check(\"" << buffer << "\")" << endl;
	if(buffer.isEmpty())
	{
//		kdDebug() << "KoSpell::done()" << endl;
		emit done();
		return true;
	}

	// we need a fifo here !!
	m_buffer << buffer;

	// replace '\n' with ' ' ?
	proc->fputs("^", false);
	proc->fputs(buffer);

	return true;
}

// invoked by KProcIO when read from ispell
void KoSpell::check2(KProcIO *)
{
//	kdDebug() << "KoSpell::check2()" << endl;
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
        if(d->m_bIgnoreTitleCase && word==word.upper())
            spelling=SpellingDone;
        if(d->m_bIgnoreUpperWords && word[0]==word[0].upper())
        {
            QString text=word[0]+word.right(word.length()-1).lower();
            if(text==word)
                spelling=SpellingDone;
        }
        if (ignorelist.findIndex(word.lower())!=-1)
            spelling=SpellingDone;

        switch(spelling)
        {
        case Misspelled:
        {
            QString buffer = m_buffer.front();
            pos--;	// for the '^' we sent, which ispell also counts as 1 char
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
//				kdDebug() << "KoSpell::misspelling(" << word << ", " << pos << ")" << endl;
            emit misspelling(word, pos);
            break;
        }

        case SpellingDone:
//				kdDebug() << "KoSpell::done()" << endl;
            m_buffer.pop_front();
            emit done();
            break;

        default:
            kdDebug() << "KoSpell::check2() ERROR" << endl;
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
    //ksconfig->setIgnoreList(ignorelist);
  return *ksconfig;
}

void KoSpell::cleanUp ()
{
  if (m_status == Cleaning) return; // Ignore
  if (m_status == Running)
  {
//    if (personaldict)
//       writePersonalDictionary();
    m_status = Cleaning;
  }
  proc->closeStdin();
}

void KoSpell::ispellExit(KProcess *)
{
  kdDebug() << "KoSpell::ispellExit() " << m_status << endl;

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

  kdDebug() << "Death" << endl;
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

#include "kospell.moc"

