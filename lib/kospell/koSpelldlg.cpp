/* This file is part of the KDE libraries
   Copyright (C) 1997 David Sweet <dsweet@kde.org>
   Copyright (C) 2000 Rik Hemsley <rik@kde.org>
   Copyright (C) 2000-2001 Wolfram Diestel <wolfram@steloj.de>
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

#include <qstringlist.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kapplication.h>
#include <klocale.h>
#include <klistbox.h>
#include <klineedit.h>
#include <kbuttonbox.h>
#include <kdebug.h>
#include <qcombobox.h>
#include <koSconfig.h>
#include "koSpelldlg.h"
#include <qcheckbox.h>

KOSpellDlg::KOSpellDlg(
  QWidget * parent,
  KOSpellConfig *_ksc,
  const char * name,
  int indexOfLanguage,
  bool _modal,
  bool _autocorrect
)
  : KDialogBase(parent, name, _modal, i18n("Check Spelling"), Help|Cancel|User1, Cancel, true, i18n("&Stop"))
{
    QWidget * w = new QWidget(this);
    setMainWidget(w);
    m_indexLanguage=0;
    m_previous = 0L;

    wordlabel = new QLabel(w, "wordlabel");
    wordlabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

    editbox = new KLineEdit(w, "editbox");

    listbox = new KListBox(w, "listbox");


    QLabel * l_language = new QLabel(i18n("Language:"), w, "l_language");

    language = new QComboBox( w, "language");

    language->insertStringList( KOSpellConfig::listOfAspellLanguages());
    language->setCurrentItem( indexOfLanguage);
    if ( _autocorrect )
    {
        m_previous = new QCheckBox( i18n("Previous word"), w);
    }

    if( _ksc->client() == KOS_CLIENT_ISPELL)
    {
        language->hide();
        l_language->hide();
        if( m_previous )
            m_previous->hide();
    }

    QLabel * l_misspelled = new QLabel(i18n("Misspelled word:"), w, "l_misspelled");

    QLabel * l_replacement = new QLabel(i18n("Replacement:"), w, "l_replacement");

    QLabel * l_suggestions = new QLabel(i18n("Suggestions:"), w, "l_suggestions");
    l_suggestions->setAlignment(Qt::AlignLeft | Qt::AlignTop );

    KButtonBox * buttonBox = new KButtonBox(w, Vertical);

    QPushButton * b = 0L;

    b = buttonBox->addButton(i18n("&Replace"), this, SLOT(replace()));
    connect(this, SIGNAL(ready(bool)), b, SLOT(setEnabled(bool)));
    qpbrep = b;

    b = buttonBox->addButton(i18n("Replace &All"), this, SLOT(replaceAll()));
    connect(this, SIGNAL(ready(bool)), b, SLOT(setEnabled(bool)));
    qpbrepa = b;

    b = buttonBox->addButton(i18n("&Ignore"), this, SLOT(ignore()));
    connect(this, SIGNAL(ready(bool)), b, SLOT(setEnabled(bool)));

    b = buttonBox->addButton(i18n("I&gnore All"), this, SLOT(ignoreAll()));
    connect(this, SIGNAL(ready(bool)), this, SLOT(setEnabled(bool)));

    b = buttonBox->addButton(i18n("A&dd"), this, SLOT(add()));
    connect(this, SIGNAL(ready(bool)), b, SLOT(setEnabled(bool)));

    if ( _autocorrect )
    {
        b = buttonBox->addButton( i18n("AutoCorrection"), this, SLOT(addToAutoCorrect()));
        connect( this,  SIGNAL( ready(bool)), b, SLOT(setEnabled(bool)));
    }

    connect(this, SIGNAL(user1Clicked()), this, SLOT(stop()));

    connect( language, SIGNAL( activated ( int )), this, SLOT( changeLanguage( int)));

    buttonBox->layout();

    QHBoxLayout * layout = new QHBoxLayout(w, KDialog::marginHint(), KDialog::spacingHint());
    QGridLayout * leftGrid = new QGridLayout(layout);

    leftGrid->addWidget(l_misspelled,   0, 0);
    leftGrid->addWidget(l_replacement,  1, 0);

    leftGrid->addWidget(l_suggestions,  2, 0);
    leftGrid->addMultiCellWidget(wordlabel,      0,0, 1, 2);
    leftGrid->addMultiCellWidget(editbox, 1, 1, 1, 2);
    leftGrid->addMultiCellWidget(listbox,        2, 2, 1, 2);

    leftGrid->addWidget(l_language, 3, 0);
    leftGrid->addMultiCellWidget(language, 3, 3, 1, 2);

    if( m_previous )
        leftGrid->addMultiCellWidget( m_previous, 4, 4, 0, 2);

    layout->addWidget(buttonBox);

    connect( editbox, SIGNAL(textChanged(const QString &)), SLOT(textChanged(const QString &)));

    connect(editbox, SIGNAL(returnPressed()),   SLOT(replace()));
    connect(listbox, SIGNAL(selected(int)),     SLOT(selected(int)));
    connect(listbox, SIGNAL(highlighted(int)),  SLOT(highlighted (int)));

    QSize bs = sizeHint();
    if (bs.width() < bs.height()) {
        resize(9 * bs.height() / 6, bs.height());
    }

    setHelp("spelldlg", "kspell");

    emit(ready(false));
}

void KOSpellDlg::addToAutoCorrect()
{
    newword = editbox->text();
    done (KOS_ADDAUTOCORRECT);
}

void KOSpellDlg::spellCheckAgain()
{
    newword = editbox->text();
    done (KOS_CHECKAGAIN);
}

void KOSpellDlg::changeLanguage( int index)
{
    newword = word;
    m_indexLanguage = index;
    done (KOS_CHECKAGAINWITHNEWLANGUAGE);
}

void KOSpellDlg::changeSuggList( QStringList *_lst )
{
    sugg = _lst;

    listbox->clear();
    emit(ready(true));
    listbox->insertStringList(*_lst);
    changeButtonState( _lst );
}

void KOSpellDlg::init(const QString & _word, QStringList * _sugg)
{
    sugg = _sugg;
    word = _word;

    listbox->clear();
    listbox->insertStringList(*sugg);

    kdDebug(30006) << "KOSpellDlg::init [" << word << "]" << endl;

    emit(ready(true));

    wordlabel->setText(_word);

    if (sugg->count() == 0) {
        editbox->setText(_word);
    }
    changeButtonState( _sugg );

}

void KOSpellDlg::changeButtonState( QStringList * _sugg )
{
  if (_sugg->count() == 0) {
    qpbrep->setEnabled(false);
    qpbrepa->setEnabled(false);

  } else {

    editbox->setText((*_sugg)[0]);
    qpbrep->setEnabled(true);
    qpbrepa->setEnabled(true);
    listbox->setCurrentItem (0);
  }
}


void KOSpellDlg::textChanged (const QString &)
{
  qpbrep->setEnabled(true);
  qpbrepa->setEnabled(true);
}

void KOSpellDlg::selected (int i)
{
  highlighted (i);
  replace();
}

void KOSpellDlg::highlighted (int i)
{
  if (listbox->text (i)!=0)
    editbox->setText (listbox->text (i));
}

/*
  exit functions
  */

void KOSpellDlg::closeEvent( QCloseEvent * )
{
	cancel();
}

void KOSpellDlg::done (int result)
{
  emit command (result);
}
void KOSpellDlg::ignore()
{
  newword = word;
  done (KOS_IGNORE);
}

void KOSpellDlg::ignoreAll()
{
  newword = word;
  done (KOS_IGNOREALL);
}

void KOSpellDlg::add()
{
  newword = word;
  done (KOS_ADD);
}


void KOSpellDlg::cancel()
{
  newword=word;
  done (KOS_CANCEL);
}

void KOSpellDlg::replace()
{
  newword = editbox->text();
  done (KOS_REPLACE);
}

void KOSpellDlg::stop()
{
  newword = word;
  done (KOS_STOP);
}

void KOSpellDlg::replaceAll()
{
  newword = editbox->text();
  done (KOS_REPLACEALL);
}

bool KOSpellDlg::previousWord() const
{
    return m_previous ? m_previous->isChecked(): false;
}

#include "koSpelldlg.moc"
