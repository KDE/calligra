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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kpushbutton.h>
#include <qwhatsthis.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klineedit.h>
#include <klocale.h>
#include <kstdguiitem.h>

#include "koSconfig.h"

class KOSpellConfigPrivate
{
public:
    KOSpellConfigPrivate() : shown( false ) {}
    QStringList replacelist;
    bool shown; // false initially, true after the first showEvent
};


KOSpellConfig::KOSpellConfig (const KOSpellConfig &_ksc)
  : QWidget(0, 0), nodialog(true)
  , kc(0)
  , cb1(0)
  , cb2(0)
  , dictlist(0)
  , dictcombo(0)
  , encodingcombo(0)
  ,clientcombo(0)
  ,cbIgnoreCase(0)
  ,cbIgnoreAccent(0)
  ,cbSpellWordWithNumber(0)
  ,cbDontCheckUpperWord(0)
  ,cbDontCheckTitleCase(0)

{
    m_bIgnoreCase = false;
    m_bIgnoreAccent = false;
    d= new KOSpellConfigPrivate;
    setReplaceAllList( _ksc.replaceAllList ());
    setNoRootAffix (_ksc.noRootAffix());
    setRunTogether (_ksc.runTogether());
    setDictionary  (_ksc.dictionary());
    setDictFromList (_ksc.dictFromList());
    setIgnoreCase ( _ksc.ignoreCase ());
    setIgnoreAccent( _ksc.ignoreAccent());
    setIgnoreList (_ksc.ignoreList());
    setEncoding (_ksc.encoding());
    setSpellWordWithNumber( _ksc.spellWordWithNumber());
    setDontCheckTitleCase( _ksc.dontCheckTitleCase());
    setDontCheckUpperWord( _ksc.dontCheckUpperWord());
    setClient (_ksc.client());
}


KOSpellConfig::KOSpellConfig( QWidget *parent, const char *name,
			    KOSpellConfig *_ksc, bool addHelpButton )
  : QWidget (parent, name), nodialog(false)
  , kc(0)
  , cb1(0)
  , cb2(0)
  , dictlist(0)
  , dictcombo(0)
  , encodingcombo(0)
  ,clientcombo(0)
  ,cbIgnoreCase(0)
  ,cbIgnoreAccent(0)
  ,cbSpellWordWithNumber(0)
  ,cbDontCheckUpperWord(0)
  ,cbDontCheckTitleCase(0)
{
    m_bIgnoreCase = false;
    m_bIgnoreAccent = false;
    m_bSpellWordWithNumber = false;
    d= new KOSpellConfigPrivate;
    kc = KGlobal::config();
    if( _ksc == 0 )
    {
        readGlobalSettings();
    }
    else
    {
        setReplaceAllList( _ksc->replaceAllList ());
        setNoRootAffix (_ksc->noRootAffix());
        setRunTogether (_ksc->runTogether());
        setDictionary  (_ksc->dictionary());
        setDictFromList (_ksc->dictFromList());
        setIgnoreList (_ksc->ignoreList());
        setEncoding (_ksc->encoding());
        setIgnoreCase ( _ksc->ignoreCase ());
        setIgnoreAccent( _ksc->ignoreAccent());
        setSpellWordWithNumber( _ksc->spellWordWithNumber());
        setDontCheckTitleCase( _ksc->dontCheckTitleCase());
        setDontCheckUpperWord( _ksc->dontCheckUpperWord());
        setClient (_ksc->client());
    }

    QGridLayout *glay = new QGridLayout (this, 8, 3, 0, KDialog::spacingHint() );
    cb1 = new QCheckBox(i18n("Create root/affix combinations"
                             " not in dictionary"), this );
    connect( cb1, SIGNAL(toggled(bool)), SLOT(sNoAff(bool)) );
    glay->addMultiCellWidget( cb1, 0, 0, 0, 2 );

    cb2 = new QCheckBox( i18n("Consider run-together words"
                              " as spelling errors"), this );
    connect( cb2, SIGNAL(toggled(bool)), SLOT(sRunTogether(bool)) );
    glay->addMultiCellWidget( cb2, 1, 1, 0, 2 );

    dictcombo = new QComboBox( this );
    dictcombo->setInsertionPolicy (QComboBox::NoInsertion);
    connect (dictcombo, SIGNAL (activated (int)),
             this, SLOT (sSetDictionary (int)));
    glay->addMultiCellWidget( dictcombo, 2, 2, 1, 2 );

    dictlist = new QLabel (dictcombo, i18n("Dictionary:"), this);
    glay->addWidget( dictlist, 2 ,0 );

    encodingcombo = new QComboBox( this );
    encodingcombo->insertItem ("US-ASCII");
    encodingcombo->insertItem ("ISO 8859-1");
    encodingcombo->insertItem ("ISO 8859-2");
    encodingcombo->insertItem ("ISO 8859-3");
    encodingcombo->insertItem ("ISO 8859-4");
    encodingcombo->insertItem ("ISO 8859-5");
    encodingcombo->insertItem ("ISO 8859-7");
    encodingcombo->insertItem ("ISO 8859-8");
    encodingcombo->insertItem ("ISO 8859-9");
    encodingcombo->insertItem ("ISO 8859-13");
    encodingcombo->insertItem ("ISO 8859-15");
    encodingcombo->insertItem ("UTF-8");
    encodingcombo->insertItem ("KOI8-R");
    encodingcombo->insertItem ("KOI8-U");
    encodingcombo->insertItem ("CP1251");

    connect (encodingcombo, SIGNAL (activated(int)), this,
             SLOT (sChangeEncoding(int)));
    glay->addMultiCellWidget (encodingcombo, 3, 3, 1, 2);

    QLabel *tmpQLabel = new QLabel( encodingcombo, i18n("Encoding:"), this);
    glay->addWidget( tmpQLabel, 3, 0 );

  clientcombo = new QComboBox( this );
  clientcombo->insertItem (i18n("International Ispell"));
  clientcombo->insertItem (i18n("Aspell"));
  clientcombo->insertItem (i18n("Hspell"));
  connect (clientcombo, SIGNAL (activated(int)), this,
	   SLOT (sChangeClient(int)));
  glay->addMultiCellWidget( clientcombo, 4, 4, 1, 2 );

  tmpQLabel = new QLabel( clientcombo, i18n("Client:"), this );
  glay->addWidget( tmpQLabel, 4, 0 );

    if( addHelpButton == true )
    {
        QPushButton *pushButton = new KPushButton( KStdGuiItem::help(), this );
        connect( pushButton, SIGNAL(clicked()), this, SLOT(sHelp()) );
        glay->addWidget(pushButton, 10, 2);
    }

    cbIgnoreCase = new QCheckBox(i18n("Ignore case when checking words"), this );
    connect( cbIgnoreCase , SIGNAL(toggled(bool)), this, SLOT(slotIgnoreCase(bool)) );

    cbIgnoreAccent = new QCheckBox(i18n("Ignore accents when checking words"), this );
    connect( cbIgnoreAccent , SIGNAL(toggled(bool)), this, SLOT(slotIgnoreAccent(bool)) );

    cbSpellWordWithNumber = new QCheckBox(i18n("Check words with numbers"), this );
    connect( cbSpellWordWithNumber , SIGNAL(toggled(bool)), this, SLOT(slotSpellWordWithNumber(bool)) );

    cbDontCheckUpperWord= new QCheckBox(i18n("Ignore uppercase words"),this);
    connect( cbDontCheckUpperWord , SIGNAL(toggled(bool)), this, SLOT(slotDontSpellCheckUpperWord(bool)) );

    QWhatsThis::add( cbDontCheckUpperWord, i18n("This option tells the spell-checker to accept words that are written in uppercase, such as KDE.") );

    cbDontCheckTitleCase= new QCheckBox(i18n("Ignore title case words"),this);
    connect( cbDontCheckTitleCase , SIGNAL(toggled(bool)), this, SLOT(slotDontCheckTitleCase(bool)) );

    QWhatsThis::add( cbDontCheckTitleCase, i18n("This option tells the spell-checker to accept words starting with an uppercase letter, such as United States."));



    glay->addMultiCellWidget( cbIgnoreCase, 5,5,0 ,2 );
    glay->addMultiCellWidget( cbIgnoreAccent, 6,6,0 ,2 );
    glay->addMultiCellWidget( cbSpellWordWithNumber, 7,7,0 ,2 );
    glay->addMultiCellWidget( cbDontCheckUpperWord, 8,8,0 ,2 );
    glay->addMultiCellWidget( cbDontCheckTitleCase, 9,9,0 ,2 );


    // Will be done at showEvent time
    //fillInDialog();
}

KOSpellConfig::~KOSpellConfig ()
{
    delete d;
}

bool KOSpellConfig::dictFromList () const
{
  return dictfromlist;
}

bool KOSpellConfig::ignoreCase () const
{
    return m_bIgnoreCase;
}

bool KOSpellConfig::ignoreAccent() const
{
    return m_bIgnoreAccent;
}

bool KOSpellConfig::readGlobalSettings ()
{
  KConfigGroupSaver cs(kc,"KSpell");

  setNoRootAffix   (kc->readNumEntry ("KSpell_NoRootAffix", 0));
  setRunTogether   (kc->readNumEntry ("KSpell_RunTogether", 0));
  setDictionary    (kc->readEntry ("KSpell_Dictionary", ""));
  setDictFromList  (kc->readNumEntry ("KSpell_DictFromList", FALSE));
  setEncoding (kc->readNumEntry ("KSpell_Encoding", KOS_E_ASCII));
  setIgnoreCase( kc->readNumEntry( "KSpell_IgnoreCase", 0));
  setIgnoreAccent( kc->readNumEntry( "KSpell_IgnoreAccent", 0));
  setSpellWordWithNumber( kc->readNumEntry("KSpell_SpellWordWithNumber", false));

  setDontCheckTitleCase( kc->readNumEntry("KSpell_dont_check_title_case", false));
  setDontCheckUpperWord( kc->readNumEntry("KSpell_dont_check_upper_word",false));

  setClient (kc->readNumEntry ("KSpell_Client", KOS_CLIENT_ASPELL));
  return TRUE;
}

bool KOSpellConfig::writeGlobalSettings ()
{
  KConfigGroupSaver cs(kc,"KSpell");
  kc->writeEntry ("KSpell_NoRootAffix",(int) noRootAffix (), TRUE, TRUE);
  kc->writeEntry ("KSpell_RunTogether", (int) runTogether (), TRUE, TRUE);
  kc->writeEntry ("KSpell_Dictionary", dictionary (), TRUE, TRUE);
  kc->writeEntry ("KSpell_DictFromList",(int) dictFromList(), TRUE, TRUE);
  kc->writeEntry ("KSpell_Encoding", (int) encoding(),
		  TRUE, TRUE);
  kc->writeEntry ("KSpell_IgnoreCase",(int) ignoreCase(), TRUE, TRUE);
  kc->writeEntry( "KSpell_IgnoreAccent", (int)ignoreAccent(), TRUE, TRUE);
  kc->writeEntry( "KSpell_SpellWordWithNumber", (int)spellWordWithNumber(), TRUE, TRUE);

  kc->writeEntry( "KSpell_dont_check_title_case", (int)dontCheckTitleCase(),TRUE,TRUE);
  kc->writeEntry( "KSpell_dont_check_upper_word", (int)dontCheckUpperWord(),TRUE,TRUE);


  kc->writeEntry ("KSpell_Client", client(),
		  TRUE, TRUE);
  kc->sync();
  return TRUE;
}

void KOSpellConfig::slotSpellWordWithNumber(bool b)
{
    setSpellWordWithNumber ( b );
    emit configChanged();
}

void KOSpellConfig::slotDontSpellCheckUpperWord(bool b)
{
    setDontCheckUpperWord(b);
    emit configChanged();
}

void KOSpellConfig::slotDontCheckTitleCase(bool b)
{
    setDontCheckTitleCase(b);
    emit configChanged();
}

void KOSpellConfig::slotIgnoreCase(bool b)
{
    setIgnoreCase ( b );
    emit configChanged();
}

void KOSpellConfig::slotIgnoreAccent(bool b)
{
    setIgnoreAccent ( b );
    emit configChanged();
}

void KOSpellConfig::sChangeClient (int i)
{
  setClient (i);

  // read in new dict list
  if (dictcombo) {
    if (iclient == KOS_CLIENT_ISPELL)
      getAvailDictsIspell();
    else if( iclient == KOS_CLIENT_ASPELL)
      getAvailDictsAspell();
    else if (iclient == KOS_CLIENT_HSPELL)
    {
      langfnames.clear();
      dictcombo->clear();
      dictcombo->insertItem(i18n("Hebrew"));
      sChangeEncoding(KOS_E_LATIN8);
    }
#if 0 //for the futur :)
    else if( iclient == KOS_CLIENT_MYSPELL)
      getAvailDictsMyspell();
#endif
  }
  emit configChanged();
}




void KOSpellConfig::sChangeEncoding(int i)
{
    kdDebug(30006) << "KOSpellConfig::sChangeEncoding(" << i << ")" << endl;
  setEncoding (i);
  emit configChanged();
}

bool KOSpellConfig::interpret (QString &fname, QString &lname,
			      QString &hname)
{

  kdDebug(30006) << "KOSpellConfig::interpret [" << fname << "]" << endl;

  QString dname(fname);

  if(dname.right(1)=="+")
    dname.remove(dname.length()-1, 1);

  if(dname.right(3)=="sml" || dname.right(3)=="med" || dname.right(3)=="lrg" || dname.right(3)=="xlg")
     dname.remove(dname.length()-3,3);

  QString extension;

  int i = dname.find('-');
  if (i != -1)
  {
    extension = dname.mid(i+1);
    dname.truncate(i);
  }

  // Aspell uses 2 alpha language codes or 2 alpha language + 2 alpha country
  if (dname.length() == 2) {
    lname = dname;
    hname = KGlobal::locale()->twoAlphaToLanguageName(lname);
  }
  else if ((dname.length() == 5) && (dname[2] == '_')) {
    lname = dname.left(2);
    hname = KGlobal::locale()->twoAlphaToLanguageName(lname);
    QString country = KGlobal::locale()->twoAlphaToCountryName(dname.right(2));
    if (extension.isEmpty())
      extension = country;
    else
      extension = country + " - " + extension;
  }
    //These are mostly the ispell-langpack defaults
  else if (dname=="english" || dname=="american" ||
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
    lname="de"; hname=i18n("German (new spelling)");
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
      lname="pl"; hname=i18n("Polish"); //sChangeEncoding(KOS_E_LATIN2);
  }
  else if (dname=="russian") {
    lname="ru"; hname=i18n("Russian");
  }
  else if (dname=="slovensko") {
      lname="si"; hname=i18n("Slovenian"); //sChangeEncoding(KOS_E_LATIN2);
  }
  else if (dname=="slovak"){
      lname="sk"; hname=i18n("Slovak"); //sChangeEncoding(KOS_E_LATIN2);
  }  else if (dname=="czech") {
      lname="cs"; hname=i18n("Czech"); //sChangeEncoding(KOS_E_LATIN2);
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
    //sChangeEncoding(KOS_E_LATIN2);
  }
  else {
    lname=""; hname=i18n("Unknown ispell dictionary", "Unknown");
  }
  if (!extension.isEmpty())
  {
    hname = hname + " (" + extension + ")";
  }

  //We have explicitly chosen English as the default here.
  if ( (KGlobal::locale()->language()==QString::fromLatin1("C") &&
	lname==QString::fromLatin1("en")) ||
       KGlobal::locale()->language()==lname)
    return TRUE;

  return FALSE;
}

void KOSpellConfig::fillInDialog ()
{
    if (nodialog)
        return;

    kdDebug(30006) << "KOSpellConfig::fillinDialog" << endl;

    cb1->setChecked (noRootAffix());
    cb2->setChecked (runTogether());
    cbIgnoreCase->setChecked(ignoreCase());
    cbIgnoreAccent->setChecked(ignoreAccent());
    cbSpellWordWithNumber->setChecked(spellWordWithNumber());
    cbDontCheckUpperWord->setChecked(dontCheckUpperWord());
    cbDontCheckTitleCase->setChecked(dontCheckTitleCase());

    encodingcombo->setCurrentItem (encoding());
    clientcombo->setCurrentItem( client() );

    // get list of available dictionaries
    // get list of available dictionaries
    if (iclient == KOS_CLIENT_ISPELL)
        getAvailDictsIspell();
    else
        getAvailDictsAspell();

    // select the used dictionary in the list
    int whichelement=-1;

    if (dictFromList())
    {
        if (iclient == KOS_CLIENT_ISPELL)
        {
            for (unsigned int i=0; i<langfnames.count(); i++)
            {
                if (langfnames[i] == dictionary())
                    whichelement=i;
            }
        }
        else if(iclient == KOS_CLIENT_ASPELL)
        {
            for (unsigned int i=0; i<listOfLanguageFileName().count(); i++)
            {
                if (listOfLanguageFileName()[i] == dictionary())
                {
                    whichelement=i;
                    break;
                }
            }
        }
        else if (iclient == KOS_CLIENT_HSPELL)
        {
            langfnames.clear();
            dictcombo->clear();
            dictcombo->insertItem(i18n("Hebrew"));
        }

    }
    dictcombo->setMinimumWidth (dictcombo->sizeHint().width());

    if (dictionary().isEmpty() ||  whichelement!=-1)
    {
        setDictFromList (TRUE);
        if (whichelement!=-1)
            dictcombo->setCurrentItem(whichelement);
    }
    else
    {
        //don't use langname !!!!!!!!!
        if (langfnames.count()>=1)
        {
            setDictFromList (TRUE);
            dictcombo->setCurrentItem(0);
        }
        else
            setDictFromList (FALSE);
    }
    sDictionary (dictFromList());
    sPathDictionary (!dictFromList());

}

void KOSpellConfig::getAvailDictsIspell () {

  langfnames.clear();
  dictcombo->clear();
  langfnames.append(""); // Default
  dictcombo->insertItem (i18n("ISpell Default"));

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
  if (!dir.exists() || !dir.isDir()) return;

  kdDebug(30006) << "KSpellConfig::getAvailDictsIspell "
	       << dir.filePath() << " " << dir.dirPath() << endl;

  QDir thedir (dir.filePath(),"*.hash");

  kdDebug(30006) << "KSpellConfig" << thedir.path() << "\n" << endl;
  kdDebug(30006) << "entryList().count()="
	       << thedir.entryList().count() << endl;

  for (unsigned int i=0;i<thedir.entryList().count();i++)
    {
      QString fname, lname, hname;
      fname = thedir [i];

      // remove .hash
      if (fname.right(5) == ".hash") fname.remove (fname.length()-5,5);

      if (interpret (fname, lname, hname) && langfnames[0].isEmpty())
	{ // This one is the KDE default language
	  // so place it first in the lists (overwrite "Default")

   	  langfnames.remove ( langfnames.begin() );
	  langfnames.prepend ( fname );

	  hname=i18n("default spelling dictionary"
		     ,"Default - %1 [%2]").arg(hname).arg(fname);

	  dictcombo->changeItem (hname,0);
	}
      else
	{
	  langfnames.append (fname);
	  hname=hname+" ["+fname+"]";

	  dictcombo->insertItem (hname);
	}
    }
}

void KOSpellConfig::setClient (int c)
{
  iclient = c;
  //kdDebug(30006)<<" c :"<<c<<endl;
  if (clientcombo)
      clientcombo->setCurrentItem(c);
}

int KOSpellConfig::client () const
{
  return iclient;
}

void KOSpellConfig::getAvailDictsAspell ()
{
    langfnames.clear();
    dictcombo->clear();
    dictcombo->insertStringList( listOfAspellLanguages() );
}

/*
 * Options setting routines.
 */


void KOSpellConfig::setNoRootAffix (bool b)
{
  bnorootaffix=b;

  if(cb1)
      cb1->setChecked(b);
}

void KOSpellConfig::setRunTogether(bool b)
{
  bruntogether=b;

  if(cb2)
      cb2->setChecked(b);
}

void KOSpellConfig::setDictionary (const QString &s)
{
    qsdict=s; //.copy();

    if (qsdict.length()>5)
        if ((signed)qsdict.find(".hash")==(signed)qsdict.length()-5)
            qsdict.remove (qsdict.length()-5,5);


    if(dictcombo)
    {
        int whichelement=-1;
        if (dictFromList())
        {
            if (iclient == KOS_CLIENT_ISPELL)
            {
                for (unsigned int i=0; i<langfnames.count(); i++)
                {
                    if (langfnames[i] == s )
                    {
                        whichelement=i;
                        break;
                    }
                }
            }
            else if(iclient == KOS_CLIENT_ASPELL)
            {
                for (unsigned int i=0; i<listOfLanguageFileName().count(); i++)
                {
                    if (listOfLanguageFileName()[i] == s )
                    {
                        whichelement=i;
                        break;
                    }
                }
            }
#if 0
            else if (iclient == KOS_CLIENT_HSPELL)
            {
                langfnames.clear();
                dictcombo->clear();
                dictcombo->insertItem(i18n("Hebrew"));
            }
#endif
            if(whichelement >= 0)
            {
                dictcombo->setCurrentItem(whichelement);
            }
        }
    }
    QString ab, desc;
    interpret( qsdict, ab, desc );
    qsdict = ab;
}

void KOSpellConfig::setDictFromList (bool dfl)
{
  //  kdebug (KDEBUG_INFO, 750, "sdfl = %d", dfl);
  dictfromlist=dfl;
}

void KOSpellConfig::setEncoding (int enctype)
{
  enc=enctype;

  if(encodingcombo)
    encodingcombo->setCurrentItem(enctype);
}


bool KOSpellConfig::noRootAffix () const
{
  return bnorootaffix;
}

bool KOSpellConfig::runTogether() const
{
  return bruntogether;
}

const QString KOSpellConfig::dictionary () const
{
  return qsdict;
}

int KOSpellConfig::encoding () const
{
  return enc;
}

void KOSpellConfig::sRunTogether(bool)
{
  setRunTogether (cb2->isChecked());
  emit configChanged();
}

void KOSpellConfig::sNoAff(bool)
{
  setNoRootAffix (cb1->isChecked());
  emit configChanged();
}


QString KOSpellConfig::getLanguage( int i )
{
    if (iclient == KOS_CLIENT_ISPELL)
    {
        return langfnames[i];
    }
    else if(iclient == KOS_CLIENT_ASPELL)
    {
        return listOfLanguageFileName()[i];
    }
#if 0
    else if (iclient == KOS_CLIENT_HSPELL)
    {
        langfnames.clear();
        dictcombo->clear();
        dictcombo->insertItem(i18n("Hebrew"));
    }
#endif
    else
        return QString::null;
}

void KOSpellConfig::sSetDictionary (int i)
{
  setDictionary (getLanguage( i ));
  setDictFromList (TRUE);
  emit configChanged();
}

void
KOSpellConfig::sDictionary(bool on)
{
  if (on)
    {
      dictcombo->setEnabled (TRUE);
      setDictionary (getLanguage( dictcombo->currentItem() ));
      setDictFromList (TRUE);
    }
  else
    {
      dictcombo->setEnabled (FALSE);
    }
  emit configChanged();
}

void
KOSpellConfig::sPathDictionary(bool on)
{
  return; //enough for now


  if (on)
    {
      //kle1->setEnabled (TRUE);
      //      browsebutton1->setEnabled (TRUE);
      //setDictionary (kle1->text());
      setDictFromList (FALSE);
    }
  else
    {
      //kle1->setEnabled (FALSE);
      //browsebutton1->setEnabled (FALSE);
    }
  emit configChanged();
}


void KOSpellConfig::activateHelp( void )
{
  sHelp();
}

void KOSpellConfig::sHelp( void )
{
  kapp->invokeHelp("configuration", "kspell");
}

void KOSpellConfig::operator= (const KOSpellConfig &ksc)
{
  //We want to copy the data members, but not the
  //pointers to the child widgets
  setNoRootAffix (ksc.noRootAffix());
  setRunTogether (ksc.runTogether());
  setDictionary (ksc.dictionary());
  setDictFromList (ksc.dictFromList());
  //  setPersonalDict (ksc.personalDict());
  setEncoding (ksc.encoding());
  setIgnoreAccent (ksc.ignoreAccent());
  setIgnoreCase (ksc.ignoreCase());

  if ( isVisible() )
      fillInDialog();
  else
      d->shown = false; // will force a fillInDialog when showing
}

void KOSpellConfig::setIgnoreList (QStringList _ignorelist)
{
  ignorelist=_ignorelist;
}

QStringList KOSpellConfig::ignoreList () const
{
  return ignorelist;
}

bool KOSpellConfig::spellWordWithNumber()const
{
    return m_bSpellWordWithNumber;
}

bool KOSpellConfig::dontCheckTitleCase()const
{
    return m_bDontCheckTitleCase;
}

bool KOSpellConfig::dontCheckUpperWord()const
{
    return m_bDontCheckUpperWord;
}

void KOSpellConfig::setReplaceAllList (QStringList _replacelist)
{
  d->replacelist=_replacelist;
}

QStringList KOSpellConfig::replaceAllList () const
{
  return d->replacelist;
}

void KOSpellConfig::setIgnoreCase ( bool b )
{
    m_bIgnoreCase=b;
    if(cbIgnoreCase)
      cbIgnoreCase->setChecked(b);

}

void KOSpellConfig::setIgnoreAccent ( bool b )
{
    m_bIgnoreAccent=b;
    if(cbIgnoreAccent)
      cbIgnoreAccent->setChecked(b);

}

void KOSpellConfig::setSpellWordWithNumber ( bool b )
{
    m_bSpellWordWithNumber = b;
    if(cbSpellWordWithNumber)
      cbSpellWordWithNumber->setChecked(b);

}

void KOSpellConfig::setDontCheckTitleCase(bool b)
{
    m_bDontCheckTitleCase = b;
    if(cbDontCheckTitleCase)
      cbDontCheckTitleCase->setChecked(b);

}

void KOSpellConfig::setDontCheckUpperWord(bool b)
{
    m_bDontCheckUpperWord=b;
    if(cbDontCheckUpperWord)
      cbDontCheckUpperWord->setChecked(b);

}


QStringList KOSpellConfig::s_aspellLanguageList = QStringList();
QStringList KOSpellConfig::s_aspellLanguageFileName = QStringList();

QStringList KOSpellConfig::listOfAspellLanguages()
{
    if ( s_aspellLanguageList.count()==0 )
        createListOfLanguages();
    return s_aspellLanguageList;
}

QStringList KOSpellConfig::listOfLanguageFileName()
{
    if ( s_aspellLanguageFileName.count()==0 )
        createListOfLanguages();
    return s_aspellLanguageFileName;
}

QString KOSpellConfig::fileNameFromLanguage( const QString & _lang)
{
    int pos = s_aspellLanguageList.findIndex( _lang );
    if ( pos != -1)
    {
        return s_aspellLanguageFileName[ pos ];
    }
    return QString::null;
}

QString KOSpellConfig::languageFromFileName( const QString &_lang )
{
    int pos = s_aspellLanguageFileName.findIndex( _lang );
    if ( pos != -1)
        return s_aspellLanguageList[ pos ];
    else
        return QString::null;
}

void KOSpellConfig::createListOfLanguages()
{
    s_aspellLanguageFileName.append(""); // Default
    s_aspellLanguageList.append(i18n("ASpell Default"));

    // dictionary path
    // FIXME: use "aspell dump config" to find out the dict-dir
    QFileInfo dir ("/usr/lib/aspell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/local/lib/aspell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/share/aspell");
    if (!dir.exists() || !dir.isDir())
        dir.setFile ("/usr/local/share/aspell");
    if (!dir.exists() || !dir.isDir()) return;

    kdDebug(30006) << "KOSpellConfig::getAvailDictsAspell "
                 << dir.filePath() << " " << dir.dirPath() << endl;

    QDir thedir (dir.filePath(),"*");

    kdDebug(30006) << "KOSpellConfig" << thedir.path() << "\n" << endl;
    kdDebug(30006) << "entryList().count()="
                 << thedir.entryList().count() << endl;

    for (unsigned int i=0; i<thedir.entryList().count(); i++)
    {
        QString fname, lname, hname;
        fname = thedir [i];

        // consider only simple dicts without '-' in the name
        // FIXME: may be this is wrong an the list should contain
        // all *.multi files too, to allow using special dictionaries
        if (fname[0] != '.')
	{

            // remove .multi
            if (fname.right(6) == ".multi")
                fname.remove (fname.length()-6,6);

            if (interpret (fname, lname, hname) && s_aspellLanguageFileName[0].isEmpty())
	    { // This one is the KDE default language
	      // so place it first in the lists (overwrite "Default")

                s_aspellLanguageFileName.remove ( s_aspellLanguageFileName.begin() );
                s_aspellLanguageFileName.prepend ( fname );

                hname=i18n("default spelling dictionary"
                           ,"Default - %1").arg(hname);
                s_aspellLanguageList[0]=hname;
	    }
            else
	    {
                s_aspellLanguageFileName.append( fname);
                s_aspellLanguageList.append( hname );
	    }
	}
    }

}

int KOSpellConfig::indexFromLanguageFileName( const QString &name)
{
    int whichelement = 0;
    for (unsigned int i=0; i<listOfLanguageFileName().count(); i++)
      {
	if (listOfLanguageFileName()[i] == name)
        {
	  whichelement=i;
          break;
        }
      }
    return whichelement;
}


void KOSpellConfig::showEvent( QShowEvent* ev )
{
    if ( !d->shown )
    {
        d->shown = true;
        fillInDialog();
    }
    QWidget::showEvent( ev );
}

#include "koSconfig.moc"



