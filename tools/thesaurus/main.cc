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

#include <qfile.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <Q3CString>
#include <QLabel>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
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

Thesaurus::Thesaurus(QObject* parent, const char* name, const QStringList &)
    : KDataTool(parent)
{
   setObjectName(name); 
    m_dialog = new KDialogBase(KJanusWidget::Plain, QString::null,
        KDialogBase::Help|KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok);
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
    
    m_page = m_dialog->plainPage();
    Q3VBoxLayout *m_top_layout = new Q3VBoxLayout(m_page, KDialog::marginHint(), KDialog::spacingHint());

    Q3HBoxLayout *row1 = new Q3HBoxLayout(m_top_layout);
    m_edit = new KHistoryCombo(m_page);
    m_edit_label = new QLabel(m_edit, i18n("&Search for:"), m_page);
    m_search = new KPushButton(i18n("S&earch"), m_page);
    connect(m_search, SIGNAL(clicked()),
        this, SLOT(slotFindTerm()));
    row1->addWidget(m_edit_label, 0);
    row1->addWidget(m_edit, 1);
    row1->addWidget(m_search, 0);
    m_back = new QToolButton(m_page);
    m_back->setIconSet(BarIconSet(QString::fromLatin1("back")));
    m_back->setToolTip( i18n("Back"));
    row1->addWidget(m_back, 0);
    m_forward = new QToolButton(m_page);
    m_forward->setIconSet(BarIconSet(QString::fromLatin1("forward")));
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
    
    vbox = new Q3VBox(m_tab);
    m_tab->addTab(vbox, i18n("&Thesaurus"));
    vbox->setMargin(KDialog::marginHint());
    vbox->setSpacing(KDialog::spacingHint());
    
    Q3HBox *hbox = new Q3HBox(vbox);
    hbox->setSpacing(KDialog::spacingHint());

    grpbox_syn = new Q3GroupBox( 1, Qt::Horizontal, i18n("Synonyms"), hbox);
    m_thes_syn = new Q3ListBox(grpbox_syn);
    
    grpbox_hyper = new Q3GroupBox( 1, Qt::Horizontal, i18n("More General Words"), hbox);
    m_thes_hyper = new Q3ListBox(grpbox_hyper);

    grpbox_hypo = new Q3GroupBox( 1, Qt::Horizontal, i18n("More Specific Words"), hbox);
    m_thes_hypo = new Q3ListBox(grpbox_hypo);

    // single click -- keep display unambiguous by removing other selections:
    
    connect(m_thes_syn, SIGNAL(clicked(Q3ListBoxItem *)), m_thes_hyper, SLOT(clearSelection()));
    connect(m_thes_syn, SIGNAL(clicked(Q3ListBoxItem *)), m_thes_hypo, SLOT(clearSelection()));
    connect(m_thes_syn, SIGNAL(selectionChanged(Q3ListBoxItem *)),
        this, SLOT(slotSetReplaceTerm(Q3ListBoxItem *)));

    connect(m_thes_hyper, SIGNAL(clicked(Q3ListBoxItem *)), m_thes_syn, SLOT(clearSelection()));
    connect(m_thes_hyper, SIGNAL(clicked(Q3ListBoxItem *)), m_thes_hypo, SLOT(clearSelection()));
    connect(m_thes_hyper, SIGNAL(selectionChanged(Q3ListBoxItem *)),
        this, SLOT(slotSetReplaceTerm(Q3ListBoxItem *)));

    connect(m_thes_hypo, SIGNAL(clicked(Q3ListBoxItem *)), m_thes_syn, SLOT(clearSelection()));
    connect(m_thes_hypo, SIGNAL(clicked(Q3ListBoxItem *)), m_thes_hyper, SLOT(clearSelection()));
    connect(m_thes_hypo, SIGNAL(selectionChanged(Q3ListBoxItem *)),
        this, SLOT(slotSetReplaceTerm(Q3ListBoxItem *)));

    // double click:
    connect(m_thes_syn, SIGNAL(selected(const QString &)),
        this, SLOT(slotFindTerm(const QString &)));
    connect(m_thes_hyper, SIGNAL(selected(const QString &)),
        this, SLOT(slotFindTerm(const QString &)));
    connect(m_thes_hypo, SIGNAL(selected(const QString &)),
        this, SLOT(slotFindTerm(const QString &)));

    //
    // WordNet Tab
    //

    vbox2 = new Q3VBox(m_tab);
    m_tab->addTab(vbox2, i18n("&WordNet"));
    vbox2->setMargin(KDialog::marginHint());    
    vbox2->setSpacing(KDialog::spacingHint());    

    m_combobox = new QComboBox(vbox2);
    m_combobox->setEditable(false);
    connect(m_combobox, SIGNAL(activated(int)), this, SLOT(slotFindTerm()));

    m_resultbox = new Q3TextBrowser(vbox2);
    m_resultbox->setTextFormat(Qt::RichText);
    // TODO?: m_resultbox->setMimeSourceFactory(...); to avoid warning
    connect(m_resultbox, SIGNAL(linkClicked(const QString &)),
        this, SLOT(slotFindTerm(const QString &)));

    // Connect for the history box
    m_edit->setTrapReturnKey(true);        // Do not use Return as default key...
    connect(m_edit, SIGNAL(returnPressed(const QString&)), this, SLOT(slotFindTerm(const QString&)));
    connect(m_edit, SIGNAL(activated(int)), this, SLOT(slotGotoHistory(int)));

    Q3HBoxLayout *row2 = new Q3HBoxLayout( m_top_layout );
    m_replace = new KLineEdit(m_page);
    m_replace_label = new QLabel(m_replace, i18n("&Replace with:"), m_page);
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
        return FALSE;
    }
    if ( mimetype != "text/plain" ) {
        kDebug(31000) << "Thesaurus only accepts mimetype text/plain" << endl;
        return FALSE;
    }

    if ( command == "thesaurus" ) {
        // not called from an application like KWord, so make it possible
        // to replace text:
        m_replacement = true;
        m_dialog->setButtonOK(i18n("&Replace"));
    } else if ( command == "thesaurus_standalone" ) {
        // not called from any application, but from KThesaurus
        m_replacement = false;
        m_dialog->showButtonOK(false);
        m_dialog->setButtonCancel(i18n("&Close"));
        m_replace->setEnabled(false);
        m_replace_label->setEnabled(false);
    } else {
        kDebug(31000) << "Thesaurus does only accept the command 'thesaurus' or 'thesaurus_standalone'" << endl;
        kDebug(31000) << "The command " << command << " is not accepted" << endl;
        return FALSE;
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

    return TRUE;
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
    m_dialog->setCaption(i18n("Related Words - %1").arg(url.fileName()));
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
    slotFindTerm(m_edit->text(index), false);
}

// Triggered when the back button is clicked.
void Thesaurus::slotBack()
{
    m_history_pos--;
    int pos = m_edit->count() - m_history_pos;
    m_edit->setCurrentItem(pos);
    slotFindTerm(m_edit->text(pos), false);
}

// Triggered when the forward button is clicked.
void Thesaurus::slotForward()
{
    m_history_pos++;
    int pos = m_edit->count() - m_history_pos;
    m_edit->setCurrentItem(pos);
    slotFindTerm(m_edit->text(pos), false);
}

// Triggered when a word is selected in the list box.
void Thesaurus::slotSetReplaceTerm(Q3ListBoxItem *item)
{
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

// Triggered when Return is pressed.
void Thesaurus::slotFindTerm()
{
    findTerm(m_edit->currentText());
}

// Triggered when a word is clicked / a list item is double-clicked.
void Thesaurus::slotFindTerm(const QString &term, bool add_to_history)
{
    slotSetReplaceTerm(term);
    if( term.startsWith("http://") ) {
        (void) new KRun(KUrl(term),0L);
    } else {
        if( add_to_history ) {
            m_edit->insertItem(term, 0);
            m_history_pos = m_edit->count();
            m_edit->setCurrentItem(0);
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
    QString term_tmp = ";" + term.trimmed() + ";";
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
          "Output:<br>%1").arg(m_thesproc_stderr));
        QApplication::restoreOverrideCursor();
        return;
    }

    QString search_term = m_edit->currentText().trimmed();
    
    QStringList syn;
    QStringList hyper;
    QStringList hypo;

    QStringList lines = lines.split(QChar('\n'), m_thesproc_stdout, false);
    for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
        QString line = (*it);
        if( line.startsWith("  ") ) {  // ignore license (two spaces)
            continue;
        }
        int sep_pos = line.find("#");
        QString syn_part = line.left(sep_pos);
        QString hyper_part = line.right(line.length()-sep_pos-1);
        QStringList syn_tmp = QStringList::split(QChar(';'), syn_part);
        QStringList hyper_tmp = QStringList::split(QChar(';'), hyper_part);
        if( syn_tmp.grep(search_term, false).size() > 0 ) {
            // match on the left side of the '#' -- synonyms
            for ( QStringList::Iterator it2 = syn_tmp.begin(); it2 != syn_tmp.end(); ++it2 ) {
                // add if it's not the term itself and if it's not yet in the list
                QString term = (*it2);
                if( term.lower() != search_term.lower() && syn.contains(term) == 0 ) {
                    syn.append(term);
                }
            }
            for ( QStringList::Iterator it2 = hyper_tmp.begin(); it2 != hyper_tmp.end(); ++it2 ) {
                QString term = (*it2);
                if( term.lower() != search_term.lower() && hyper.contains(term) == 0 ) {
                    hyper.append(term);
                }
            }
        }
        if( hyper_tmp.grep(search_term, false).size() > 0 ) {
            // match on the right side of the '#' -- hypernyms
            for ( QStringList::Iterator it2 = syn_tmp.begin(); it2 != syn_tmp.end(); ++it2 ) {
                QString term = (*it2);
                if( term.lower() != search_term && hypo.contains(term) == 0 ) {
                    hypo.append(term);
                }
            }
        }
    }

    m_thes_syn->clear();
    if( syn.size() > 0 ) {
        syn = sortQStringList(syn);
        m_thes_syn->insertStringList(syn);
        m_thes_syn->setEnabled(true);
    } else {
        m_thes_syn->insertItem(m_no_match);
        m_thes_syn->setEnabled(false);
    }
    
    m_thes_hyper->clear();
    if( hyper.size() > 0 ) {
        hyper = sortQStringList(hyper);
        m_thes_hyper->insertStringList(hyper);
        m_thes_hyper->setEnabled(true);
    } else {
        m_thes_hyper->insertItem(m_no_match);
        m_thes_hyper->setEnabled(false);
    }

    m_thes_hypo->clear();
    if( hypo.size() > 0 ) {
        hypo = sortQStringList(hypo);
        m_thes_hypo->insertStringList(hypo);
        m_thes_hypo->setEnabled(true);
    } else {
        m_thes_hypo->insertItem(m_no_match);
        m_thes_hypo->setEnabled(false);
    }

    QApplication::restoreOverrideCursor();
}

void Thesaurus::receivedThesStdout(KProcess *, char *result, int len)
{
    m_thesproc_stdout += QString::fromLocal8Bit( Q3CString(result, len+1) );
}

void Thesaurus::receivedThesStderr(KProcess *, char *result, int len)
{
    m_thesproc_stderr += QString::fromLocal8Bit( Q3CString(result, len+1) );
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
    if( m_combobox->currentItem() == 0 ) {
        *m_wnproc << "-synsn" << "-synsv" << "-synsa" << "-synsr";
        m_mode = other;
    } else if( m_combobox->currentItem() == 1 ) {
        *m_wnproc << "-simsv";
        m_mode = other;
    } else if( m_combobox->currentItem() == 2 ) {
        *m_wnproc << "-antsn" << "-antsv" << "-antsa" << "-antsr";
        m_mode = other;
    } else if( m_combobox->currentItem() == 3 ) {
        *m_wnproc << "-hypon" << "-hypov";
        m_mode = other;
    } else if( m_combobox->currentItem() == 4 ) {
        *m_wnproc << "-meron";
        m_mode = other;
    } else if( m_combobox->currentItem() == 5 ) {
        *m_wnproc << "-holon";
        m_mode = other;
    } else if( m_combobox->currentItem() == 6 ) {
        // e.g. "size -> large/small"
        *m_wnproc << "-attrn" << "-attra";
        m_mode = other;
    } else if( m_combobox->currentItem() == 7 ) {
        // e.g. "kill -> die"
        *m_wnproc << "-causv";
        m_mode = other;
    } else if( m_combobox->currentItem() == 8 ) {
        // e.g. "walk -> step"
        *m_wnproc << "-entav";
        m_mode = other;
    } else if( m_combobox->currentItem() == 9 ) {
        *m_wnproc << "-famln" << "-famlv" << "-famla" << "-famlr";
        m_mode = other;
    } else if( m_combobox->currentItem() == 10 ) {
        *m_wnproc << "-framv";
        m_mode = other;
    } else if( m_combobox->currentItem() == 11 ) {
        *m_wnproc << "-grepn" << "-grepv" << "-grepa" << "-grepr";
        m_mode = grep;
    } else if( m_combobox->currentItem() == 12 ) {
        *m_wnproc << "-over";
        m_mode = other;
    }
    *m_wnproc << "-g";    // "Display gloss"

    int current = m_combobox->currentItem();    // remember current position
    m_combobox->clear();
    
    // warning: order matters!
    // 0:    
    m_combobox->insertItem(i18n("Synonyms/Hypernyms - Ordered by Frequency"));
    m_combobox->insertItem(i18n("Synonyms - Ordered by Similarity of Meaning (verbs only)"));
    m_combobox->insertItem(i18n("Antonyms - Words with Opposite Meanings"));
    m_combobox->insertItem(i18n("Hyponyms - ... is a (kind of) %1").arg(m_edit->currentText()));
    m_combobox->insertItem(i18n("Meronyms - %1 has a ...").arg(m_edit->currentText()));
    // 5:
    m_combobox->insertItem(i18n("Holonyms - ... has a %1").arg(m_edit->currentText()));
    m_combobox->insertItem(i18n("Attributes"));
    m_combobox->insertItem(i18n("Cause To (for some verbs only)"));
    m_combobox->insertItem(i18n("Verb Entailment (for some verbs only)"));
    m_combobox->insertItem(i18n("Familiarity & Polysemy Count"));
    // 10:
    m_combobox->insertItem(i18n("Verb Frames (examples of use)"));
    m_combobox->insertItem(i18n("List of Compound Words"));
    m_combobox->insertItem(i18n("Overview of Senses"));

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

    m_combobox->setCurrentItem(current);    // reset previous position

    if( m_wnproc->isRunning() ) {
        // should never happen
        kDebug(31000) << "Warning: findTerm(): process is already running?!" << endl;
        QApplication::restoreOverrideCursor();
        return;
    }

    if( !m_wnproc->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
        m_resultbox->setText(i18n("<b>Error:</b> Failed to execute WordNet program 'wn'. "
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
        m_resultbox->setText(i18n("<b>Error:</b> Failed to execute WordNet program 'wn'. "
          "Output:<br>%1").arg(m_wnproc_stderr));
        QApplication::restoreOverrideCursor();
        return;
    }

    if( m_wnproc_stdout.isEmpty() ) {
        m_resultbox->setText(i18n("No match for '%1'.").arg(m_edit->currentText()));
    } else {
        // render in a table, each line one row:
        QStringList lines = lines.split(QChar('\n'), m_wnproc_stdout, false);
        QString result = "<qt><table>\n";
        // TODO in Qt > 3.01: try without the following line (it's necessary to ensure the
        // first column is really always quite small):
        result += "<tr><td width=\"10%\"></td><td width=\"90%\"></td></tr>\n";
        uint ct = 0;
        for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
            QString l = (*it);
            // Remove some lines:
            QRegExp re("^\\d+( of \\d+)? senses? of \\w+");
            if( re.search(l) != -1 ) {
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
        m_resultbox->setText(result);
        m_resultbox->setContentsPos(0,0);
        //kDebug() << result << endl;
    }
    
    QApplication::restoreOverrideCursor();
}

void Thesaurus::receivedWnStdout(KProcess *, char *result, int len)
{
    m_wnproc_stdout += QString::fromLocal8Bit( Q3CString(result, len+1) );
}

void Thesaurus::receivedWnStderr(KProcess *, char *result, int len)
{
    m_wnproc_stderr += QString::fromLocal8Bit( Q3CString(result, len+1) );
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
    if( re.search(l) != -1 ) {
        l = "<b>" +re.cap(1)+ "</b>" +re.cap(2);
        return l;
    } 

    re.setPattern("^.* of (noun|verb|adj|adv) .*");
    if( re.search(l) != -1 ) {
        l = "<font size=\"5\">" +re.cap()+ "</font>\n\n";
        return l;
    } 

    if( m_mode == grep ) {
        l = l.trimmed();
        return QString("<a href=\"" +l+ "\">" +l+ "</a>");
    }

    re.setPattern("^(Sense \\d+)");
    if( re.search(l) != -1 ) {
        l = "<b>" +re.cap()+ "</b>\n";
        return l;
    }
    
    re.setPattern("(.*)(Also See-&gt;)(.*)");
    // Example: first sense of verb "keep"
    if( re.search(l) != -1 ) {
        l = re.cap(1);
        l += re.cap(2);
        QStringList links = links.split(QChar(';'), re.cap(3), false);
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
    if( re.search(l) != -1 ) {
        int dash_pos = l.find("--");
        QString line_end = l.mid(dash_pos+2, l.length()-dash_pos);
        l = re.cap(1);
        l += re.cap(2) + " ";
        QStringList links = links.split(QChar(','), re.cap(3), false);
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
        map_list[str.lower()] = str;
    }
    list.clear();
    QMap<QString,QString>::Iterator it;
    // Qt doc: "the items are alphabetically sorted [by key] when iterating over the map":
    for( it = map_list.begin(); it != map_list.end(); ++it ) {
        list.append(it.data());
    }
    return list;
}

#include "main.moc"
