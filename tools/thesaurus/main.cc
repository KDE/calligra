/*
   $Id$
   This file is part of the KDE project
   Copyright (C) 2001,2002,2003 Daniel Naber <daniel.naber@t-online.de>
   This is a thesaurus based on a subset of WordNet. It also offers an
   almost complete WordNet 1.7 frontend (WordNet is a powerful lexical
   database/thesaurus)
*/
/***************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

/*
TODO:
-Be more verbose if the result is empty
-See the TODO's in the source below

-If no match was found, use KSpell to offer alternative spellings?
-Don't start WordNet before its tab is activated?
-Maybe remove more uncommon words. However, the "polysemy/familiarity
 count" is sometimes very low for quite common word, e.g. "sky".

-Fix "no mimesource" warning of QTextBrowser? Seems really harmless.

NOT TODO:
-Add part of speech information -- I think this would blow up the
 filesize too much
*/

#include "main.h"

#include <QFile>
#include <QToolButton>
//Added by qt3to4:
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kdeversion.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

typedef KGenericFactory<Thesaurus, KDataTool> ThesaurusFactory;
K_EXPORT_COMPONENT_FACTORY( libthesaurustool, ThesaurusFactory("thesaurus_tool") )

/***************************************************
 *
 * Thesaurus *
 ***************************************************/

Thesaurus::Thesaurus(QObject* parent, const QStringList &)
    : KDataTool(parent)
{
    m_dialog = new KDialog(0);
    m_dialog->setButtons( KDialog::Help|KDialog::Ok|KDialog::Cancel );
    m_dialog->setDefaultButton( KDialog::Ok );
    m_dialog->setHelp(QString::null, "thesaurus");
    m_dialog->resize(600, 400);

    m_config = new KConfig("kthesaurusrc");
    m_data_file = m_config->readPathEntry("datafile");
    if( m_data_file.isEmpty() ) {
        m_data_file = KGlobal::dirs()->findResourceDir("data", "thesaurus/")
           + "thesaurus/thesaurus.txt";
    }
    setCaption();

    m_no_match = i18n("(No match)");

    m_replacement = false;
    m_history_pos = 1;
    m_page = new QFrame( 0L );
    m_dialog->setMainWidget(m_page);
    QVBoxLayout *m_top_layout = new QVBoxLayout(m_page/*, KDialog::marginHint(), KDialog::spacingHint()*/);
    m_top_layout->setMargin( KDialog::marginHint() );
    m_top_layout->setSpacing( KDialog::spacingHint() );

    QHBoxLayout *row1 = new QHBoxLayout();
    m_top_layout->addLayout( row1 );
    m_edit = new KHistoryCombo(m_page);
    m_edit_label = new QLabel( i18n("&Search for:"), m_page);
    m_edit_label->setBuddy( m_edit );

    m_search = new KPushButton(i18n("S&earch"), m_page);
    connect(m_search, SIGNAL(clicked()),
        this, SLOT(slotFindTerm()));
    row1->addWidget(m_edit_label, 0);
    row1->addWidget(m_edit, 1);
    row1->addWidget(m_search, 0);
    m_back = new QToolButton(m_page);
    m_back->setIcon(BarIconSet(QString::fromLatin1("back")));
    m_back->setToolTip( i18n("Back"));
    row1->addWidget(m_back, 0);
    m_forward = new QToolButton(m_page);
    m_forward->setIcon(BarIconSet(QString::fromLatin1("forward")));
    m_forward->setToolTip( i18n("Forward"));
    row1->addWidget(m_forward, 0);
    m_lang = new KPushButton(i18n("Change Language..."), m_page);
    connect(m_lang, SIGNAL(clicked()), this, SLOT(slotChangeLanguage()));
    row1->addWidget(m_lang, 0);

    connect(m_back, SIGNAL(clicked()), this, SLOT(slotBack()));
    connect(m_forward, SIGNAL(clicked()), this, SLOT(slotForward()));

    m_tab = new QTabWidget(m_page);
    m_top_layout->addWidget(m_tab);

    //
    // Thesaurus Tab
    //
    vbox = new QWidget(m_tab);
    m_tab->addTab(vbox, i18n("&Thesaurus"));
    QVBoxLayout *l1 = new QVBoxLayout;
    vbox->setLayout(l1);
    l1->setMargin(KDialog::marginHint());
    l1->setSpacing(KDialog::spacingHint());

    QWidget *hbox = new QWidget(vbox);
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->setSpacing(KDialog::spacingHint());

    grpbox_syn = new QGroupBox( i18n("Synonyms"), hbox);
    QHBoxLayout *l3 = new QHBoxLayout();
    grpbox_syn->setLayout(l3);
    m_thes_syn = new QListWidget(grpbox_syn);
    l3->addWidget(m_thes_syn);

    grpbox_hyper = new QGroupBox( i18n("More General Words"), hbox);
    QHBoxLayout *l4 = new QHBoxLayout();
    grpbox_hyper->setLayout(l4);
    m_thes_hyper = new QListWidget(grpbox_hyper);
    l4->addWidget(m_thes_hyper);

    grpbox_hypo = new QGroupBox( i18n("More Specific Words"), hbox);
    QHBoxLayout *l5 = new QHBoxLayout();
    grpbox_hypo->setLayout(l5);
    m_thes_hypo = new QListWidget(grpbox_hypo);
    l5->addWidget(m_thes_hypo);

    l2->addWidget(grpbox_syn);
    l2->addWidget(grpbox_hyper);
    l2->addWidget(grpbox_hypo);
    l1->addLayout(l2);

    // single click -- keep display unambiguous by removing other selections:

    connect(m_thes_syn, SIGNAL(itemClicked(QListWidgetItem *)), m_thes_hyper, SLOT(clearSelection()));
    connect(m_thes_syn, SIGNAL(itemClicked(QListWidgetItem *)), m_thes_hypo, SLOT(clearSelection()));
    connect(m_thes_syn, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotSetReplaceTermSyn()));

    connect(m_thes_hyper, SIGNAL(itemClicked(QListWidgetItem *)), m_thes_syn, SLOT(clearSelection()));
    connect(m_thes_hyper, SIGNAL(itemClicked(QListWidgetItem *)), m_thes_hypo, SLOT(clearSelection()));
    connect(m_thes_hyper, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotSetReplaceTermHyper()));

    connect(m_thes_hypo, SIGNAL(itemClicked(QListWidgetItem *)), m_thes_syn, SLOT(clearSelection()));
    connect(m_thes_hypo, SIGNAL(itemClicked(QListWidgetItem *)), m_thes_hyper, SLOT(clearSelection()));
    connect(m_thes_hypo, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotSetReplaceTermHypo()));

    // double click:
    connect(m_thes_syn, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
        this, SLOT(slotFindTermFromList(QListWidgetItem *)));
    connect(m_thes_hyper, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
        this, SLOT(slotFindTermFromList(QListWidgetItem *)));
    connect(m_thes_hypo, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
        this, SLOT(slotFindTermFromList(QListWidgetItem *)));

    //
    // WordNet Tab
    //

    vbox2 = new QWidget(m_tab);
    QVBoxLayout *l6 = new QVBoxLayout();
    vbox2->setLayout( l6 );
    m_tab->addTab(vbox2, i18n("&WordNet"));
    l6->setMargin(KDialog::marginHint());
    l6->setSpacing(KDialog::spacingHint());

    m_combobox = new QComboBox(vbox2);
    m_combobox->setEditable(false);
    connect(m_combobox, SIGNAL(activated(int)), this, SLOT(slotFindTerm()));
    l6->addWidget( m_combobox );

    m_resultbox = new QTextEdit(vbox2);
    m_resultbox->setReadOnly( true );
//     m_resultbox->setTextFormat(Qt::RichText);
    // TODO?: m_resultbox->setMimeSourceFactory(...); to avoid warning
    connect(m_resultbox, SIGNAL(linkClicked(const QString &)),
        this, SLOT(slotFindTerm(const QString &)));
    l6->addWidget( m_resultbox );

    // Connect for the history box
    m_edit->setTrapReturnKey(true);        // Do not use Return as default key...
    connect(m_edit, SIGNAL(returnPressed()), this, SLOT(slotEditReturnPressed()));
    connect(m_edit, SIGNAL(activated(int)), this, SLOT(slotGotoHistory(int)));

    QHBoxLayout *row2 = new QHBoxLayout( /*m_top_layout*/ );
    m_top_layout->addLayout( row2 );
    m_replace = new KLineEdit(m_page);
    m_replace_label = new QLabel( i18n("&Replace with:"), m_page);
    m_replace_label->setBuddy( m_replace );
    row2->addWidget(m_replace_label, 0);
    row2->addWidget(m_replace, 1);

    // Set focus
    m_edit->setFocus();
    slotUpdateNavButtons();

    //
    // The external command stuff
    //

    // calling the 'wn' binary
    m_wnproc = new KProcess;
    connect(m_wnproc, SIGNAL(processExited(KProcess*)), this, SLOT(wnExited(KProcess*)));
    connect(m_wnproc, SIGNAL(receivedStdout(KProcess*,char*,int)),
        this, SLOT(receivedWnStdout(KProcess*, char*, int)));
    connect(m_wnproc, SIGNAL(receivedStderr(KProcess*,char*,int)),
        this, SLOT(receivedWnStderr(KProcess*, char*, int)));

    // grep'ing the text file
    m_thesproc = new KProcess;
    connect(m_thesproc, SIGNAL(processExited(KProcess*)), this, SLOT(thesExited(KProcess*)));
    connect(m_thesproc, SIGNAL(receivedStdout(KProcess*,char*,int)),
        this, SLOT(receivedThesStdout(KProcess*, char*, int)));
    connect(m_thesproc, SIGNAL(receivedStderr(KProcess*,char*,int)),
        this, SLOT(receivedThesStderr(KProcess*, char*, int)));

}


Thesaurus::~Thesaurus()
{
    m_config->writePathEntry("datafile", m_data_file);
    m_config->sync();
    delete m_config;
    // FIXME?: this hopefully fixes the problem of a wrong cursor
    // and a crash (when closing e.g. konqueror) when the thesaurus dialog
    // gets close while it was still working and showing the wait cursor
    QApplication::restoreOverrideCursor();
    delete m_thesproc;
    delete m_wnproc;
    delete m_dialog;
}


bool Thesaurus::run(const QString& command, void* data, const QString& datatype, const QString& mimetype)
{

    // Check whether we can accept the data
    if ( datatype != "QString" ) {
        kDebug(31000) << "Thesaurus only accepts datatype QString" << endl;
        return false;
    }
    if ( mimetype != "text/plain" ) {
        kDebug(31000) << "Thesaurus only accepts mimetype text/plain" << endl;
        return false;
    }

    if ( command == "thesaurus" ) {
        // not called from an application like KWord, so make it possible
        // to replace text:
        m_replacement = true;
        m_dialog->setButtonGuiItem( KDialog::Ok, KGuiItem(i18n("&Replace")));
    } else if ( command == "thesaurus_standalone" ) {
        // not called from any application, but from KThesaurus
        m_replacement = false;
        m_dialog->showButton( KDialog::Ok, false);
        m_dialog->setButtonGuiItem( KDialog::Cancel, KGuiItem(i18n("&Close")));
        m_replace->setEnabled(false);
        m_replace_label->setEnabled(false);
    } else {
        kDebug(31000) << "Thesaurus does only accept the command 'thesaurus' or 'thesaurus_standalone'" << endl;
        kDebug(31000) << "The command " << command << " is not accepted" << endl;
        return false;
    }

    // Get data and clean it up:
    QString buffer = *((QString *)data);
    buffer = buffer.trimmed();
    QRegExp re("[.,;!?\"'()\\[\\]]");
    buffer.remove(re);
    buffer = buffer.left(100);        // limit maximum length

    m_wnproc_stdout = "";
    m_wnproc_stderr = "";

    m_thesproc_stdout = "";
    m_thesproc_stderr = "";

    if( ! buffer.isEmpty() ) {
        slotFindTerm(buffer);
    }

    if( m_dialog->exec() == QDialog::Accepted ) {    // "Replace"
        *((QString*)data) = m_replace->text();
    }

    return true;
}


void Thesaurus::slotChangeLanguage()
{
    QString filename = KFileDialog::getOpenFileName(
        KGlobal::dirs()->findResourceDir("data", "thesaurus/")+"thesaurus/");
    if( !filename.isNull() ) {
        m_data_file = filename;
        setCaption();
    }
}

void Thesaurus::setCaption()
{
    KUrl url = KUrl();
    url.setPath(m_data_file);
    m_dialog->setCaption(i18n("Related Words - %1" , url.fileName() ) );
}

// Enbale or disable back and forward button
void Thesaurus::slotUpdateNavButtons()
{
    if( m_history_pos <= 1 ) {    // 1 = first position
        m_back->setEnabled(false);
    } else {
        m_back->setEnabled(true);
    }
    if( m_history_pos >= m_edit->count() ) {
        m_forward->setEnabled(false);
    } else {
        m_forward->setEnabled(true);
    }
}

// Go to an item from the editbale combo box.
void Thesaurus::slotGotoHistory(int index)
{
    m_history_pos = m_edit->count() - index;
    slotFindTerm(m_edit->itemText(index), false);
}

// Triggered when the back button is clicked.
void Thesaurus::slotBack()
{
    m_history_pos--;
    int pos = m_edit->count() - m_history_pos;
    m_edit->setCurrentIndex(pos);
    slotFindTerm(m_edit->itemText(pos), false);
}

// Triggered when the forward button is clicked.
void Thesaurus::slotForward()
{
    m_history_pos++;
    int pos = m_edit->count() - m_history_pos;
    m_edit->setCurrentIndex(pos);
    slotFindTerm(m_edit->itemText(pos), false);
}

// Triggered when a word is selected in the list box.
void Thesaurus::slotSetReplaceTermSyn()
{
    QListWidgetItem *item = m_thes_syn->currentItem ();
    if( ! item )
        return;
    m_replace->setText(item->text());
}

void Thesaurus::slotSetReplaceTermHyper()
{
    QListWidgetItem *item = m_thes_hyper->currentItem (); 
    if( ! item )
        return;
    m_replace->setText(item->text());
}

void Thesaurus::slotSetReplaceTermHypo()
{
    QListWidgetItem *item = m_thes_hypo->currentItem ();
    if( ! item )
        return;
    m_replace->setText(item->text());
}


void Thesaurus::slotSetReplaceTerm(const QString &term)
{
    if( m_replacement && term != m_no_match ) {
        m_replace->setText(term);
    }
}


void Thesaurus::slotEditReturnPressed()
{
    slotFindTerm( m_edit->currentText() );
}

// Triggered when Return is pressed.
void Thesaurus::slotFindTerm()
{
    findTerm(m_edit->currentText());
}

// Triggered when a list item is double-clicked.
void Thesaurus::slotFindTermFromList(QListWidgetItem *item)
{
    slotFindTerm( item->text() );
}

// Triggered when a word is clicked
void Thesaurus::slotFindTerm(const QString &term, bool add_to_history)
{
    slotSetReplaceTerm(term);
    if( term.startsWith("http://") ) {
        (void) new KRun(KUrl(term),0L);
    } else {
        if( add_to_history ) {
            m_edit->insertItem(0, term);
            m_history_pos = m_edit->count();
            m_edit->setCurrentIndex(0);
        }
        slotUpdateNavButtons();
        findTerm(term);
    }
}

void Thesaurus::findTerm(const QString &term)
{
    findTermThesaurus(term);
    findTermWordnet(term);
}


//
// Thesaurus
//
void Thesaurus::findTermThesaurus(const QString &term)
{

    if( !QFile::exists(m_data_file) ) {
        KMessageBox::error(0, i18n("The thesaurus file '%1' was not found. "
            "Please use 'Change Language...' to select a thesaurus file.").
            arg(m_data_file));
        return;
    }

    QApplication::setOverrideCursor(KCursor::waitCursor());

    m_thesproc_stdout = "";
    m_thesproc_stderr = "";

    // Find only whole words. Looks clumsy, but this way we don't have to rely on
    // features that might only be in certain versions of grep:
    QString term_tmp = ';' + term.trimmed() + ';';
    m_thesproc->clearArguments();
    *m_thesproc << "grep" << "-i" << term_tmp;
    *m_thesproc << m_data_file;

    if( !m_thesproc->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
        KMessageBox::error(0, i18n("Failed to execute grep."));
        QApplication::restoreOverrideCursor();
        return;
    }
}

// The external process has ended, so we parse its result and put it in
// the list box.
void Thesaurus::thesExited(KProcess *)
{

    if( !m_thesproc_stderr.isEmpty() ) {
        KMessageBox::error(0, i18n("<b>Error:</b> Failed to execute grep. "
          "Output:<br>%1", m_thesproc_stderr) );
        QApplication::restoreOverrideCursor();
        return;
    }

    QString search_term = m_edit->currentText().trimmed();

    QStringList syn;
    QStringList hyper;
    QStringList hypo;

    QStringList lines = m_thesproc_stdout.split(QChar('\n'), QString::SkipEmptyParts  );
    for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
        QString line = (*it);
        if( line.startsWith("  ") ) {  // ignore license (two spaces)
            continue;
        }
        int sep_pos = line.indexOf("#");
        QString syn_part = line.left(sep_pos);
        QString hyper_part = line.right(line.length()-sep_pos-1);
        QStringList syn_tmp = syn_part.split(QChar(';'), QString::SkipEmptyParts );
        QStringList hyper_tmp = hyper_part.split(QChar(';'), QString::SkipEmptyParts);
        if( syn_tmp.filter(search_term, Qt::CaseInsensitive).size() > 0 ) {
            // match on the left side of the '#' -- synonyms
            for ( QStringList::Iterator it2 = syn_tmp.begin(); it2 != syn_tmp.end(); ++it2 ) {
                // add if it's not the term itself and if it's not yet in the list
                QString term = (*it2);
                if( term.toLower() != search_term.toLower() && syn.contains(term) == 0 ) {
                    syn.append(term);
                }
            }
            for ( QStringList::Iterator it2 = hyper_tmp.begin(); it2 != hyper_tmp.end(); ++it2 ) {
                QString term = (*it2);
                if( term.toLower() != search_term.toLower() && hyper.contains(term) == 0 ) {
                    hyper.append(term);
                }
            }
        }
        if( hyper_tmp.filter(search_term, Qt::CaseInsensitive).size() > 0 ) {
            // match on the right side of the '#' -- hypernyms
            for ( QStringList::Iterator it2 = syn_tmp.begin(); it2 != syn_tmp.end(); ++it2 ) {
                QString term = (*it2);
                if( term.toLower() != search_term && hypo.contains(term) == 0 ) {
                    hypo.append(term);
                }
            }
        }
    }

    m_thes_syn->clear();
    if( syn.size() > 0 ) {
        syn = sortQStringList(syn);
        m_thes_syn->addItems(syn);
        m_thes_syn->setEnabled(true);
    } else {
        m_thes_syn->addItem( m_no_match);
        m_thes_syn->setEnabled(false);
    }

    m_thes_hyper->clear();
    if( hyper.size() > 0 ) {
        hyper = sortQStringList(hyper);
        m_thes_hyper->addItems(hyper);
        m_thes_hyper->setEnabled(true);
    } else {
        m_thes_hyper->addItem(m_no_match);
        m_thes_hyper->setEnabled(false);
    }

    m_thes_hypo->clear();
    if( hypo.size() > 0 ) {
        hypo = sortQStringList(hypo);
        m_thes_hypo->addItems(hypo);
        m_thes_hypo->setEnabled(true);
    } else {
        m_thes_hypo->addItem(m_no_match);
        m_thes_hypo->setEnabled(false);
    }

    QApplication::restoreOverrideCursor();
}

void Thesaurus::receivedThesStdout(KProcess *, char *result, int len)
{
    m_thesproc_stdout += QString::fromLocal8Bit( QByteArray(result, len+1) );
}

void Thesaurus::receivedThesStderr(KProcess *, char *result, int len)
{
    m_thesproc_stderr += QString::fromLocal8Bit( QByteArray(result, len+1) );
}


//
// WordNet
//
void Thesaurus::findTermWordnet(const QString &term)
{
    QApplication::setOverrideCursor(KCursor::waitCursor());

    m_wnproc_stdout = "";
    m_wnproc_stderr = "";

    m_wnproc->clearArguments();
    *m_wnproc << "wn";
    *m_wnproc << term;

    // get all results: nouns, verbs, adjectives, adverbs (see below for order):
    if( m_combobox->currentIndex() == 0 ) {
        *m_wnproc << "-synsn" << "-synsv" << "-synsa" << "-synsr";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 1 ) {
        *m_wnproc << "-simsv";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 2 ) {
        *m_wnproc << "-antsn" << "-antsv" << "-antsa" << "-antsr";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 3 ) {
        *m_wnproc << "-hypon" << "-hypov";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 4 ) {
        *m_wnproc << "-meron";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 5 ) {
        *m_wnproc << "-holon";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 6 ) {
        // e.g. "size -> large/small"
        *m_wnproc << "-attrn" << "-attra";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 7 ) {
        // e.g. "kill -> die"
        *m_wnproc << "-causv";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 8 ) {
        // e.g. "walk -> step"
        *m_wnproc << "-entav";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 9 ) {
        *m_wnproc << "-famln" << "-famlv" << "-famla" << "-famlr";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 10 ) {
        *m_wnproc << "-framv";
        m_mode = other;
    } else if( m_combobox->currentIndex() == 11 ) {
        *m_wnproc << "-grepn" << "-grepv" << "-grepa" << "-grepr";
        m_mode = grep;
    } else if( m_combobox->currentIndex() == 12 ) {
        *m_wnproc << "-over";
        m_mode = other;
    }
    *m_wnproc << "-g";    // "Display gloss"

    int current = m_combobox->currentIndex();    // remember current position
    m_combobox->clear();

    // warning: order matters!
    // 0:
    m_combobox->insertItem(-1, i18n("Synonyms/Hypernyms - Ordered by Frequency"));
    m_combobox->insertItem(-1, i18n("Synonyms - Ordered by Similarity of Meaning (verbs only)"));
    m_combobox->insertItem(-1, i18n("Antonyms - Words with Opposite Meanings"));
    m_combobox->insertItem(-1, i18n("Hyponyms - ... is a (kind of) %1", m_edit->currentText()));
    m_combobox->insertItem(-1, i18n("Meronyms - %1 has a ...", m_edit->currentText()));
    // 5:
    m_combobox->insertItem(-1, i18n("Holonyms - ... has a %1", m_edit->currentText()));
    m_combobox->insertItem(-1, i18n("Attributes"));
    m_combobox->insertItem(-1, i18n("Cause To (for some verbs only)"));
    m_combobox->insertItem(-1, i18n("Verb Entailment (for some verbs only)"));
    m_combobox->insertItem(-1, i18n("Familiarity & Polysemy Count"));
    // 10:
    m_combobox->insertItem(-1, i18n("Verb Frames (examples of use)"));
    m_combobox->insertItem(-1, i18n("List of Compound Words"));
    m_combobox->insertItem(-1, i18n("Overview of Senses"));

    /** NOT todo:
      * -Hypernym tree: layout is difficult, you can get the same information
      *  by following links
      * -Coordinate terms (sisters): just go to synset and then use hyponyms
      * -Has Part Meronyms, Has Substance Meronyms, Has Member Meronyms,
      *  Member of Holonyms, Substance of Holonyms, Part of Holonyms:
      *  these are just subsets of Meronyms/Holonyms
      * -hmern, hholn: these are just compact versions, you can get the
      *  same information by following some links
      */

    /** TODO?:
      * -pert (e.g. nuclear -> nuclues, but "=>" are nested, difficult to display)
      * -nomn(n|v), e.g. deny -> denial, but this doesn't seem to work?
      */

    m_combobox->setCurrentIndex(current);    // reset previous position

    if( m_wnproc->isRunning() ) {
        // should never happen
        kDebug(31000) << "Warning: findTerm(): process is already running?!" << endl;
        QApplication::restoreOverrideCursor();
        return;
    }

    if( !m_wnproc->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
        m_resultbox->setHtml(i18n("<b>Error:</b> Failed to execute WordNet program 'wn'. "
            "WordNet has to be installed on your computer if you want to use it, "
            "and 'wn' has to be in your PATH. "
            "You can get WordNet at <a href=\"http://www.cogsci.princeton.edu/~wn/\">"
            "http://www.cogsci.princeton.edu/~wn/</a>. Note that WordNet only supports "
            "the English language."));
        m_combobox->setEnabled(false);
        QApplication::restoreOverrideCursor();
        return;
    }

}

// The process has ended, so parse its result and display it as Qt richtext.
void Thesaurus::wnExited(KProcess *)
{

    if( !m_wnproc_stderr.isEmpty() ) {
        m_resultbox->setHtml(i18n("<b>Error:</b> Failed to execute WordNet program 'wn'. "
          "Output:<br>%1", m_wnproc_stderr));
        QApplication::restoreOverrideCursor();
        return;
    }

    if( m_wnproc_stdout.isEmpty() ) {
        m_resultbox->setHtml(i18n("No match for '%1'.", m_edit->currentText()));
    } else {
        // render in a table, each line one row:
        QStringList lines = m_wnproc_stdout.split(QChar('\n'), QString::SkipEmptyParts);
        QString result = "<qt><table>\n";
        // TODO in Qt > 3.01: try without the following line (it's necessary to ensure the
        // first column is really always quite small):
        result += "<tr><td width=\"10%\"></td><td width=\"90%\"></td></tr>\n";
        uint ct = 0;
        for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
            QString l = (*it);
            // Remove some lines:
            QRegExp re("^\\d+( of \\d+)? senses? of \\w+");
            if( re.indexIn(l) != -1 ) {
                continue;
            }
            // Escape XML:
            l = l.replace('&', "&amp;");
            l = l.replace('<', "&lt;");
            l = l.replace('>', "&gt;");
            // TODO?:
            // move "=>" in own column?
            l = formatLine(l);
            // Table layout:
            result += "<tr>";
            if( l.startsWith(" ") ) {
                result += "\t<td width=\"15\"></td>";
                l = l.trimmed();
                result += "\t<td>" + l + "</td>";
            } else {
                l = l.trimmed();
                result += "<td colspan=\"2\">" + l + "</td>";
            }
            result += "</tr>\n";
            ct++;
        }
        result += "\n</table></qt>\n";
        m_resultbox->setHtml(result);
//         m_resultbox->setContentsPos(0,0);
        //kDebug() << result << endl;
    }

    QApplication::restoreOverrideCursor();
}

void Thesaurus::receivedWnStdout(KProcess *, char *result, int len)
{
    m_wnproc_stdout += QString::fromLocal8Bit( QByteArray(result, len+1) );
}

void Thesaurus::receivedWnStderr(KProcess *, char *result, int len)
{
    m_wnproc_stderr += QString::fromLocal8Bit( QByteArray(result, len+1) );
}


//
// Tools
//

// Format lines using Qt's simple richtext.
QString Thesaurus::formatLine(QString l)
{

    if( l == "--------------" ) {
        return QString("<hr>");
    }

    QRegExp re;

    re.setPattern("^(\\d+\\.)(.*)$");
    if( re.indexIn(l) != -1 ) {
        l = "<b>" +re.cap(1)+ "</b>" +re.cap(2);
        return l;
    }

    re.setPattern("^.* of (noun|verb|adj|adv) .*");
    if( re.indexIn(l) != -1 ) {
        l = "<font size=\"5\">" +re.cap()+ "</font>\n\n";
        return l;
    }

    if( m_mode == grep ) {
        l = l.trimmed();
        return QString("<a href=\"" +l+ "\">" +l+ "</a>");
    }

    re.setPattern("^(Sense \\d+)");
    if( re.indexIn(l) != -1 ) {
        l = "<b>" +re.cap()+ "</b>\n";
        return l;
    }

    re.setPattern("(.*)(Also See-&gt;)(.*)");
    // Example: first sense of verb "keep"
    if( re.indexIn(l) != -1 ) {
        l = re.cap(1);
        l += re.cap(2);
        QStringList links = re.cap(3).split(QChar(';'), QString::SkipEmptyParts);
        for ( QStringList::Iterator it = links.begin(); it != links.end(); ++it ) {
            QString link = (*it);
            if( it != links.begin() ) {
                l += ", ";
            }
            link = link.trimmed();
            link = link.remove(QRegExp("#\\d+"));
            l += "<a href=\"" +link+ "\">" +link+ "</a>";
        }
        l.prepend (' ');        // indent in table
    }

    re.setPattern("(.*)(=&gt;|HAS \\w+:|PART OF:)(.*) --");
    re.setMinimal(true);    // non-greedy
    if( re.indexIn(l) != -1 ) {
        int dash_pos = l.indexOf("--");
        QString line_end = l.mid(dash_pos+2, l.length()-dash_pos);
        l = re.cap(1);
        l += re.cap(2) + ' ';
        QStringList links = re.cap(3).split(QChar(','), QString::SkipEmptyParts);
        for ( QStringList::Iterator it = links.begin(); it != links.end(); ++it ) {
            QString link = (*it);
            if( it != links.begin() ) {
                l += ", ";
            }
            link = link.trimmed();
            l += "<a href=\"" +link+ "\">" +link+ "</a>";
        }
        l += "<font color=\"#777777\">" +line_end+ "</font>";
        l.prepend(' ');        // indent in table
        return l;
    }
    re.setMinimal(false);    // greedy again

    return l;
}

/**
 * Sort a list case insensitively.
 * Be careful: @p list is modified
 * TODO: use ksortablevaluelist?
 */
QStringList Thesaurus::sortQStringList(QStringList list)
{
    // Sort list case-insensitive. This looks strange but using a QMap
    // is even suggested by the Qt documentation.
    QMap<QString,QString> map_list;
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        QString str = *it;
        map_list[str.toLower()] = str;
    }
    list.clear();
    QMap<QString,QString>::Iterator it;
    // Qt doc: "the items are alphabetically sorted [by key] when iterating over the map":
    for( it = map_list.begin(); it != map_list.end(); ++it ) {
        list.append(it.value());
    }
    return list;
}

#include "main.moc"
