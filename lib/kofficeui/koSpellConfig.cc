/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <koSpellConfig.h>
#include <kdebug.h>
#include "koSpellConfig.moc"
#include "klocale.h"
#include <kdialog.h>
#include <kspell.h>
#include <klineedit.h>
#include <keditlistbox.h>
#include <kio/netaccess.h>


#include <qcheckbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qgroupbox.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>

KoSpellConfigWidget::KoSpellConfigWidget( QWidget *_parent, KSpellConfig *_config, bool backgroundSpellCheck )
    : QWidget( _parent)
{
    QTabWidget *tab = new QTabWidget(_parent);
    QGroupBox* tmpQGroupBox = new QGroupBox( tab, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Spelling"));
    QGridLayout *grid1 = new QGridLayout(tmpQGroupBox, 6, 1, KDialog::marginHint(), KDialog::spacingHint());
    grid1->addRowSpacing( 0, KDialog::marginHint() + 5 );
    grid1->setRowStretch( 5, 10 );
    m_spellConfig = new KSpellConfig(tmpQGroupBox, 0L, _config, false );
    grid1->addWidget(m_spellConfig,1,0);

    m_dontCheckUpperWord= new QCheckBox(i18n("Ignore uppercase words"),tmpQGroupBox);
    QWhatsThis::add( m_dontCheckUpperWord, i18n("This option tells the spell-checker to accept words that are written in uppercase, such as KDE.") );
    grid1->addWidget(m_dontCheckUpperWord,2,0);

    m_dontCheckTitleCase= new QCheckBox(i18n("Ignore title case words"),tmpQGroupBox);
    QWhatsThis::add( m_dontCheckTitleCase, i18n("This option tells the spell-checker to accept words starting with an uppercase letter, such as United States."));
    grid1->addWidget(m_dontCheckTitleCase,3,0);

    m_cbBackgroundSpellCheck=new QCheckBox(i18n("Show misspelled words in document"),tmpQGroupBox);
    grid1->addWidget(m_cbBackgroundSpellCheck,4,0);

    if ( !backgroundSpellCheck )
        m_cbBackgroundSpellCheck->hide();

    tab->addTab(tmpQGroupBox, i18n("General"));

    QVGroupBox* tmpQGroupBox2 = new QVGroupBox( i18n("IgnoreAllList"), tab, "GroupBox2" );
    m_listignoreall =  new KEditListBox( i18n("Word:"),
                                    tmpQGroupBox2, "list_ignoreall" , false,
                                    KEditListBox::Add|KEditListBox::Remove );

    m_clearIgnoreAllHistory= new QPushButton( i18n("Clear Ignore All Word History..."),tmpQGroupBox2);
    connect( m_clearIgnoreAllHistory, SIGNAL(clicked()),this, SLOT(slotClearIgnoreAllHistory()));

    tab->addTab(tmpQGroupBox2, i18n("IgnoreAll"));


    QVGroupBox* tmpQGroupBox3 = new QVGroupBox( i18n("Personal dictionary"), tab, "GroupBox3" );
    m_dictionary =  new KEditListBox( i18n("Word:"),
                                      tmpQGroupBox3, "dictionary" , false,
                                      KEditListBox::Add|KEditListBox::Remove );

    m_clearDictionary = new QPushButton( i18n("Clear Dictionary"),tmpQGroupBox3);
    connect( m_clearDictionary, SIGNAL(clicked()),this, SLOT(slotClearDictionary()));

    tab->addTab(tmpQGroupBox3, i18n("Dictionary"));
    initDictionaryListWord();
}


void KoSpellConfigWidget::initDictionaryListWord()
{
    if ( m_spellConfig->client() == KS_CLIENT_ISPELL )
    {

        QString directoryName = QDir::homeDirPath() + "/.ispell_"+m_spellConfig->dictionary();
        kdDebug()<<" directoryName :"<<directoryName<<endl;
        QFile f( directoryName );
        if ( f.exists() && f.open(IO_ReadOnly))
        {
            QTextStream t( &f );
            QString result = t.read();
            f.close();
            QStringList lst = QStringList::split("\n", result);
            m_dictionary->insertStringList( lst );
        }
    }
    else
        m_dictionary->setEnabled( false );
}

bool KoSpellConfigWidget::backgroundSpellCheck()
{
    return m_cbBackgroundSpellCheck->isChecked();
}

void KoSpellConfigWidget::setBackgroundSpellCheck( bool _b )
{
    m_cbBackgroundSpellCheck->setChecked(_b);
}

bool KoSpellConfigWidget::dontCheckUpperWord()
{
    return m_dontCheckUpperWord->isChecked();
}

void KoSpellConfigWidget::setDontCheckUpperWord( bool _b )
{
    m_dontCheckUpperWord->setChecked(_b);
}

bool KoSpellConfigWidget::dontCheckTitleCase()
{
    return m_dontCheckTitleCase->isChecked();
}

void KoSpellConfigWidget::setDontCheckTitleCase( bool _b )
{
    m_dontCheckTitleCase->setChecked( _b);
}

void KoSpellConfigWidget::slotClearIgnoreAllHistory()
{
    m_listignoreall->listBox()->clear();
    m_listignoreall->lineEdit()->clear();

}

void KoSpellConfigWidget::addIgnoreList( const QStringList & lst)
{
    m_listignoreall->listBox()->insertStringList( lst );

}

QStringList KoSpellConfigWidget::ignoreList()
{
    QStringList lst;
    for (int i = 0; i< (int)m_listignoreall->listBox()->count() ; i++)
      lst << m_listignoreall->listBox()->text( i );
    return lst;
}

void KoSpellConfigWidget::setDefault()
{
    m_spellConfig->setNoRootAffix( 0);
    m_spellConfig->setRunTogether(0);
    m_spellConfig->setDictionary( "");
    m_spellConfig->setDictFromList( FALSE);
    m_spellConfig->setEncoding (KS_E_ASCII);
    m_spellConfig->setClient (KS_CLIENT_ISPELL);
    m_dontCheckUpperWord->setChecked(false);
    m_dontCheckTitleCase->setChecked(false);
    m_cbBackgroundSpellCheck->setChecked(false);
    m_listignoreall->listBox()->clear();
}

void KoSpellConfigWidget::slotClearDictionary()
{
    m_dictionary->listBox()->clear();
    m_dictionary->lineEdit()->clear();
}

void KoSpellConfigWidget::saveDictionary()
{
    QString lst;
    for (int i = 0; i< (int)m_dictionary->listBox()->count() ; i++)
        lst+= m_dictionary->listBox()->text( i ) +"\n";

    if ( m_spellConfig->client() == KS_CLIENT_ISPELL )
    {
        QString directoryName = QDir::homeDirPath() + "/.ispell_"+m_spellConfig->dictionary();
        QFile f( directoryName );
        (void )KIO::NetAccess::del( KURL( f.name() ) );
        if ( f.open(IO_ReadWrite))
        {
            QTextStream t( &f );
            t <<lst;
            f.close();
        }
    }

}
