/* This file is part of the KDE project
   Copyright (C) 2001 Daniel Naber <daniel.naber@t-online.de>
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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

/*
FIXME:
-Get rid of the broken icon
-KDataTool in general: can it be accessed without using the mouse?
-Improve the user interface, so that every word can be used to replace
 the current selection
-see the fixme's in the source below
-function words (like "if" etc) are not part of wordnet. There  are not *that*
 many of them, so maybe we should add them. See search tools' stop word list for
 function words.
*/

/* QA:
-example for a multi-part-word: "mental faculty"
*/

#include "main.h"

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( libthesaurustool, KGenericFactory<Thesaurus> );

/***************************************************
 *
 * Thesaurus *
 ***************************************************/

Thesaurus::Thesaurus(QObject* parent, const char* name, const QStringList)
    : KDataTool(parent, name)
{
    dialog = new KDialogBase("Thesaurus", KDialogBase::User1|KDialogBase::Cancel,
        KDialogBase::Yes, KDialogBase::Cancel,
        0, "thesaurus", true, false, QString::null, i18n("Replace"));
    dialog->setInitialSize(QSize(350, 400));

    layout = dialog->makeVBoxMainWidget();

    combobox = new QComboBox(layout);
    combobox->setEditable(false);
    // warning: order matters:
    combobox->insertItem(i18n("Synonyms"));
    combobox->insertItem(i18n("Antonyms"));
    combobox->insertItem(i18n("Hyponyms - ...is a (kind of) X"));
    combobox->insertItem(i18n("Meroyms - X has a..."));
    // TODO: add all the other relations?
    connect(combobox, SIGNAL(activated(int)), this, SLOT(slotFindTerm()));

    edit = new KHistoryCombo(layout);
    // Do not use Return as default key...
    edit->setTrapReturnKey(true);
    // ...but search term when return is pressed or old term is selected:
    connect(edit, SIGNAL(returnPressed(const QString&)), this, SLOT(slotFindTerm(const QString&)));
    connect(edit, SIGNAL(activated(const QString &)), this, SLOT(slotFindTerm(const QString &)));

    // remember input:
    connect(edit, SIGNAL(returnPressed(const QString&)), edit, SLOT(addToHistory(const QString&)));

    thesaurusproc = new KProcess;
    connect(thesaurusproc, SIGNAL(processExited(KProcess*)),
        this, SLOT(thesaurusExited(KProcess*)));
    connect(thesaurusproc, SIGNAL(receivedStdout(KProcess*,char*,int)),
        this, SLOT(receivedStdout(KProcess*, char*, int)));
    connect(thesaurusproc, SIGNAL(receivedStderr(KProcess*,char*,int)),
        this, SLOT(receivedStderr(KProcess*, char*, int)));

    listbox = new QListBox(layout);
    connect(listbox, SIGNAL(selected(int)), this, SLOT(slotFindTerm(int)));

    // indentation for items in the listbox:
    indent = "   ";
}


Thesaurus::~Thesaurus()
{
    if( dialog ) {
        delete dialog;
    }
    if( thesaurusproc ) {
        delete thesaurusproc;
    }
    // TODO: can we ignore the others because they are children of layout?
}


bool Thesaurus::run(const QString& command, void* data, const QString& datatype, const QString& mimetype)
{
    if ( command != "thesaurus" )
    {
        kdDebug(31000) << "Thesaurus does only accept the command 'thesaurus'" << endl;
        kdDebug(31000) << "   The command " << command << " is not accepted" << endl;
        return FALSE;
    }
    // Check wether we can accept the data
    if ( datatype != "QString" )
    {
        kdDebug(31000) << "Thesaurus only accepts datatype QString" << endl;
        return FALSE;
    }
    if ( mimetype != "text/plain" )
    {
        kdDebug(31000) << "Thesaurus only accepts mimetype text/plain" << endl;
        return FALSE;
    }

    // Get data and clean it up:
    QString buffer = *((QString *)data);
    buffer = buffer.stripWhiteSpace();
    QRegExp re("[.,;!?\"'()\\[\\]]");
    buffer.replace(re, "");
    edit->insertItem(buffer, 0);

    procresult_stdout = "";
    procresult_stderr = "";

    if( ! slotFindTerm(buffer) ) {
        return FALSE;
    }

    if( dialog->exec() == KDialogBase::User1 ) {    // Replace
        QString replace_text = listbox->currentText().stripWhiteSpace();
        if( replace_text.isEmpty() ) {
            replace_text = edit->currentText();
        }
        *((QString*)data) = replace_text;
    }
                
    return TRUE;
}


bool Thesaurus::slotFindTerm(int index)
{
    //kdDebug(31000) << "##KWWordInfo::slotFindTerm(" << index << ")" << endl;
    if( listbox->currentItem() != -1 && listbox->text(index).left(indent.length()) != indent ) {
        return false;
    }
    QString term = listbox->text(index).stripWhiteSpace();
    edit->insertItem(term, 0);
    edit->setCurrentItem(0);
    return slotFindTerm(term);
}


bool Thesaurus::slotFindTerm()
{
    return findTerm(edit->currentText());
}


bool Thesaurus::slotFindTerm(const QString &term)
{
    return findTerm(term);
}


bool Thesaurus::findTerm(const QString &term)
{
    // TODO: show waiting mouse pointer

    procresult_stdout = "";
    procresult_stderr = "";
    listbox->clear();

    thesaurusproc->clearArguments();
    *thesaurusproc << "wn";
    *thesaurusproc << term;
    // get all results: nouns, verbs, adjectives, adverbs:
    if( combobox->currentItem() == 0 ) {
        *thesaurusproc << "-synsn" << "-synsv" << "-synsa" << "-synsr";
    } else if( combobox->currentItem() == 1 ) {
        *thesaurusproc << "-antsn" << "-antsv" << "-antsa" << "-antsr";
    } else if( combobox->currentItem() == 2 ) {
        *thesaurusproc << "-hypon" << "-hypov";
    } else if( combobox->currentItem() == 3 ) {
        *thesaurusproc << "-meron";
    }

    if( thesaurusproc->isRunning() ) {
        // should never happen
        kdDebug(31000) << "Warning: findTerm(): process is already running?!" << endl;
        return false;
    }

    if( !thesaurusproc->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
        KMessageBox::error(0, i18n("Failed to execute thesaurus program 'wn' (wordnet)"));	// TODO: add tips
        return false;
    }
    return true;
}


void Thesaurus::thesaurusExited(KProcess *)
{
    if( !procresult_stderr.isEmpty() ) {
      KMessageBox::error(0, i18n("Failed to execute thesaurus program 'wn' (wordnet)."
        "Output:\n%1").arg(procresult_stderr));
      return;
    }

    if( procresult_stdout.isEmpty() ) {
        listbox->insertItem(i18n("No match for '%1'.").arg(edit->currentText()));
    } else {
        // fixme:
        QStringList list = list.split(QRegExp("\n"), procresult_stdout, true);
        // improve output:
        for( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            if( (*it).find(" senses of ") != -1 ) {	// fixme: make it more safe
                // not interesting
                //it = list.remove(it);
            } else if( (*it).find("Sense ") == 0 ) {
                // fixme: merge with next line
            } else if( (*it).contains(" => ") > 0 ) {
                QString line = (*it);
                line.replace( QRegExp("=>"), "");
                // fixme: starting with "--":
                // fixme: manual linebreaks
                QStringList sublist = sublist.split(QRegExp(","), line, true);
                for( QStringList::Iterator sub_it = sublist.begin(); sub_it != sublist.end(); ++sub_it ) {
                    QString syn = *(sub_it);
                    syn = indent + syn.stripWhiteSpace();
                    list.insert(it, syn);
                }
                it = list.remove(it);
            }
        }
        listbox->insertStringList(list);
    }
}


void Thesaurus::receivedStdout(KProcess *, char *result, int len)
{
    procresult_stdout += QString::fromLocal8Bit( QCString(result, len+1) );
}


void Thesaurus::receivedStderr(KProcess *, char *result, int len)
{
    procresult_stderr.replace(procresult_stderr.length(), len, result);
    //kdDebug(31000) << "#### stderr: " << QString(procresult_stderr) << endl;
}

#include "main.moc"
