/*
   $Id: $
   This file is part of the KDE project
   Last update: 2001-11-17
   Copyright (C) 2001 Daniel Naber <daniel.naber@t-online.de>
   This is a frontend for WordNet, a powerful lexical database/thesaurus.
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
-table broken for e.g. "perfomer" (first colum too wide)
-fix "no mimesource" warning of QTextBrowser
-see the fixme's in the source below

TODO:
-Add a WordNet subset
-Be more verbose if the result is empty
-function words (like "if" etc) are not part of WordNet. There  are not *that*
 many of them, so maybe we should add them. See search tools' stop word list for
 function words.
-Don't forget to insert comments for the translators where necessary
 (because WordNet is English language only)
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

Thesaurus::Thesaurus(QObject* parent, const char* name, const QStringList &)
    : KDataTool(parent, name)
{
    
    m_dialog = new KDialogBase("Thesaurus", KDialogBase::User1|KDialogBase::Cancel,
        KDialogBase::Yes, KDialogBase::Cancel,
        0, "thesaurus", true, false, QString::null, i18n("Replace"));
    m_dialog->setInitialSize(QSize(400, 400));

    m_layout = m_dialog->makeVBoxMainWidget();

    m_combobox = new QComboBox(m_layout);
    m_combobox->setEditable(false);
    // TODO: add all the other relations?
    connect(m_combobox, SIGNAL(activated(int)), this, SLOT(slotFindTerm()));

    m_edit = new KHistoryCombo(m_layout);
    // Do not use Return as default key...
    m_edit->setTrapReturnKey(true);
    // ...but search term when return is pressed or old term is selected:
    connect(m_edit, SIGNAL(returnPressed(const QString&)), this, SLOT(slotFindTerm(const QString&)));
    connect(m_edit, SIGNAL(activated(const QString &)), this, SLOT(slotFindTerm(const QString &)));

    // remember input:
    connect(m_edit, SIGNAL(returnPressed(const QString&)), m_edit, SLOT(addToHistory(const QString&)));

    m_thesaurusproc = new KProcess;
    connect(m_thesaurusproc, SIGNAL(processExited(KProcess*)),
        this, SLOT(thesaurusExited(KProcess*)));
    connect(m_thesaurusproc, SIGNAL(receivedStdout(KProcess*,char*,int)),
        this, SLOT(receivedStdout(KProcess*, char*, int)));
    connect(m_thesaurusproc, SIGNAL(receivedStderr(KProcess*,char*,int)),
        this, SLOT(receivedStderr(KProcess*, char*, int)));

    m_resultbox = new QTextBrowser(m_layout);
    m_resultbox->setTextFormat(Qt::RichText);
    // fixme?: m_resultbox->setMimeSourceFactory(...);
    connect(m_resultbox, SIGNAL(linkClicked(const QString &)),
        this, SLOT(slotFindTerm(const QString &)));
}


Thesaurus::~Thesaurus()
{
    if( m_dialog ) {
        delete m_dialog;
    }
    if( m_thesaurusproc ) {
        delete m_thesaurusproc;
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
    m_edit->insertItem(buffer, 0);

    m_procresult_stdout = "";
    m_procresult_stderr = "";

    slotFindTerm(buffer);

    if( m_dialog->exec() == KDialogBase::User1 ) {    // Replace
        QString replace_text = m_edit->currentText();
        *((QString*)data) = replace_text;
    }

    return TRUE;
}


// Triggered when Return is pressed.
void Thesaurus::slotFindTerm()
{
    findTerm(m_edit->currentText());
}


// Triggered when a link is clicked.
void Thesaurus::slotFindTerm(const QString &term)
{
    if( term.startsWith("http://") ) {
        (void) new KRun(KURL(term));
    } else {
        m_edit->insertItem(term, 0);
        m_edit->setCurrentItem(0);
        findTerm(term);
    }
}


void Thesaurus::findTerm(const QString &term)
{
    QApplication::setOverrideCursor(KCursor::waitCursor());

    m_procresult_stdout = "";
    m_procresult_stderr = "";

    m_thesaurusproc->clearArguments();
    *m_thesaurusproc << "wn";
    *m_thesaurusproc << term;
    // get all results: nouns, verbs, adjectives, adverbs (see below for order):
    if( m_combobox->currentItem() == 0 ) {
        *m_thesaurusproc << "-synsn" << "-synsv" << "-synsa" << "-synsr";
        m_mode = other;
    } else if( m_combobox->currentItem() == 1 ) {
        *m_thesaurusproc << "-simsv";
        m_mode = other;
    } else if( m_combobox->currentItem() == 2 ) {
        *m_thesaurusproc << "-antsn" << "-antsv" << "-antsa" << "-antsr";
        m_mode = other;
    } else if( m_combobox->currentItem() == 3 ) {
        *m_thesaurusproc << "-hypon" << "-hypov";
        m_mode = other;
    } else if( m_combobox->currentItem() == 4 ) {
        *m_thesaurusproc << "-hypen" << "-hypev";
        m_mode = other;
    } else if( m_combobox->currentItem() == 5 ) {
        *m_thesaurusproc << "-meron";
        m_mode = other;
    } else if( m_combobox->currentItem() == 6 ) {
        *m_thesaurusproc << "-causv";
        m_mode = other;
    } else if( m_combobox->currentItem() == 7 ) {
        *m_thesaurusproc << "-grepn" << "-grepv" << "-grepa" << "-grepr";
        m_mode = grep;
    } else if( m_combobox->currentItem() == 8 ) {
        *m_thesaurusproc << "-over";
        m_mode = other;
    }
    *m_thesaurusproc << "-g";    // "Display gloss"

    if( m_thesaurusproc->isRunning() ) {
        // should never happen
        kdDebug(31000) << "Warning: findTerm(): process is already running?!" << endl;
        QApplication::restoreOverrideCursor();
        return;
    }

    if( !m_thesaurusproc->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
        m_resultbox->setText(i18n("<b>Error:</b> Failed to execute WordNet program 'wn'. "
            "WordNet has to be installed on your computer if you want to use it, "
            "and 'wn' has to be in your PATH. "
            "You can get WordNet at <a href=\"http://www.cogsci.princeton.edu/~wn/\">"
            "http://www.cogsci.princeton.edu/~wn/</a>. Note that WordNet only supports "
            "the English language."));
        m_edit->setEnabled(false);
        QApplication::restoreOverrideCursor();
        return;
    }

     // warning: order matters:
    int current = m_combobox->currentItem();    // remember current position
    m_combobox->clear();
    m_combobox->insertItem(i18n("Synonyms/Hypernyms - ordered by frequency"));
    m_combobox->insertItem(i18n("Synonyms - ordered by similariy of meaning (verbs only)"));
    m_combobox->insertItem(i18n("Antonyms - words with opposite meanings"));
    m_combobox->insertItem(i18n("Hyponyms - ...is a (kind of) %1").arg(m_edit->currentText()));
    m_combobox->insertItem(i18n("Hypernyms - %1 is a (kind of)...").arg(m_edit->currentText()));
    m_combobox->insertItem(i18n("Meroyms - %1 has a...").arg(m_edit->currentText()));
    // Cause to: e.g. "kill -> die"
    m_combobox->insertItem(i18n("Cause to... (for some verbs only)"));
    m_combobox->insertItem(i18n("List of Compound Words"));
    m_combobox->insertItem(i18n("Overview of senses"));
    m_combobox->setCurrentItem(current);
}


void Thesaurus::thesaurusExited(KProcess *)
{
    
    if( !m_procresult_stderr.isEmpty() ) {
      m_resultbox->setText(i18n("<b>Error:</b> Failed to execute WordNet program 'wn'. "
        "Output:<br>%1").arg(m_procresult_stderr));
      QApplication::restoreOverrideCursor();
      return;
    }

    if( m_procresult_stdout.isEmpty() ) {
        m_resultbox->setText(i18n("No match for '%1'.").arg(m_edit->currentText()));
    } else {
        // render in a table, each line one row:
        QStringList lines = lines.split(QRegExp("\n"), m_procresult_stdout, false);
        QString result = "<qt><table>\n";
        uint ct = 0;
        for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
            QString l = (*it);
            // Remove some lines:
            QRegExp re("^\\d+( of \\d+)? senses? of \\w+");
            if( re.search(l) != -1 ) {
                continue;
            }
            // Escape XML:
            l = l.replace(QRegExp("<"), "&lt;");
            l = l.replace(QRegExp(">"), "&gt;");
            // TODO: 
            // move "=>" in own column?
            l = formatLine(l);
            // Table layout:
            result += "<tr>";
            if( l.startsWith(" ") ) {
                result += "\t<td width=\"15\"></td>";
                l = l.stripWhiteSpace();
                result += "\t<td>" + l + "</td>";
            } else {
                l = l.stripWhiteSpace();
                result += "<td colspan=\"2\">" + l + "</td>";
            }
            result += "</tr>\n";
            ct++;
        }
        result += "\n</table></qt>\n";
        m_resultbox->setText(result);
        m_resultbox->setContentsPos(0,0 );
        //kdDebug() << result << endl;
    }
    QApplication::restoreOverrideCursor();
}


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

    re.setPattern("^.* of (verb|noun|adj|adv) .*");        // fixme: more?
    if( re.search(l) != -1 ) {
        l = "<font size=\"5\">" +re.cap()+ "</font>\n\n";
        return l;
    } 

    if( m_mode == grep ) {
        l = l.stripWhiteSpace();
        return QString("<a href=\"" +l+ "\">" +l+ "</a>");
    }

    re.setPattern("^(Sense \\d+)");
    if( re.search(l) != -1 ) {
        l = "<b>" +re.cap()+ "</b>\n";
        return l;
    }
    
    re.setPattern("(.*)(=&gt; |HAS \\w+: )(.*) --");
    re.setMinimal(true);    // non-greedy
    if( re.search(l) != -1 ) {
        int dash_pos = l.find("--");
        QString line_end = l.mid(dash_pos+2, l.length()-dash_pos);
        l = re.cap(1);
        l = re.cap(2);
        QStringList links = links.split(QRegExp(","), re.cap(3), false);
        for ( QStringList::Iterator it = links.begin(); it != links.end(); ++it ) {
            QString link = (*it);
            if( it != links.begin() ) {
                l += ", ";
            }
            link = link.stripWhiteSpace();
            l += "<a href=\"" +link+ "\">" +link+ "</a>";
        }
        l += "<font color=\"#777777\">" +line_end+ "</font>";
        l = " " + l;        // indent in table
        return l;
    }

    return l;
}


void Thesaurus::receivedStdout(KProcess *, char *result, int len)
{
    m_procresult_stdout += QString::fromLocal8Bit( QCString(result, len+1) );
}


void Thesaurus::receivedStderr(KProcess *, char *result, int len)
{
    m_procresult_stderr += QString::fromLocal8Bit( QCString(result, len+1) );
}

#include "main.moc"
