/*
    Copyright (C) 2001, S.R.Haque <srhaque@iee.org>.
    This file is part of the KDE project

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

#include <qcheckbox.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <kapp.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <klocale.h>
#include <koFind.h>
#include <kmessagebox.h>

KoFindDialog::KoFindDialog(QWidget *parent, const char *name, long options, const QStringList &findStrings) :
    KDialogBase(parent, name, true, i18n("Find Text"), Ok | Cancel, Ok)
{
    init(false, findStrings);
    setOptions(options);
}

KoFindDialog::KoFindDialog(QWidget *parent, const char *name, bool /*forReplace*/) :
    KDialogBase(parent, name, true, i18n("Replace Text"), Ok | Cancel, Ok)
{
}

KoFindDialog::~KoFindDialog()
{
}

QWidget *KoFindDialog::findExtension()
{
    return m_findExtension;
}

QStringList KoFindDialog::findHistory() const
{
    return m_find->historyItems();
}

void KoFindDialog::init(bool forReplace, const QStringList &findStrings)
{
    QVBoxLayout *topLayout;
    QGridLayout *optionsLayout;

    // Create common parts of dialog.
    QWidget *page = new QWidget(this);
    setMainWidget(page);

    topLayout = new QVBoxLayout(page);
    topLayout->setSpacing(6);
    topLayout->setMargin(11);

    m_findGrp = new QGroupBox(0, Qt::Vertical, i18n("Find"), page);
    m_findGrp->layout()->setSpacing(0);
    m_findGrp->layout()->setMargin(0);
    m_findLayout = new QGridLayout(m_findGrp->layout());
    m_findLayout->setSpacing(6);
    m_findLayout->setMargin(11);

    m_findLabel = new QLabel(i18n("&Text To Find"), m_findGrp);
    m_find = new KHistoryCombo(true, m_findGrp);
    m_find->setMaxCount(10);
    m_find->setDuplicatesEnabled(false);
    m_regExp = new QCheckBox(i18n("Use Patterns"), m_findGrp);
    m_regExpItem = new QPushButton(i18n("Insert Pattern"), m_findGrp);
    m_regExpItem->setEnabled(false);
    m_findExtension = new QWidget(m_findGrp);

    m_findLayout->addWidget(m_findLabel, 0, 0);
    m_findLayout->addMultiCellWidget(m_find, 1, 1, 0, 1);
    m_findLayout->addWidget(m_regExp, 2, 0);
    m_findLayout->addWidget(m_regExpItem, 2, 1);
    m_findLayout->addMultiCellWidget(m_findExtension, 3, 3, 0, 1);
    topLayout->addWidget(m_findGrp);

    m_replaceGrp = new QGroupBox(0, Qt::Vertical, i18n("Replace With"), page);
    m_replaceGrp->layout()->setSpacing(0);
    m_replaceGrp->layout()->setMargin(0);
    m_replaceLayout = new QGridLayout(m_replaceGrp->layout());
    m_replaceLayout->setSpacing(6);
    m_replaceLayout->setMargin(11);

    m_replaceLabel = new QLabel(i18n("&Replacement Text"), m_replaceGrp);
    m_replace = new KHistoryCombo(true, m_replaceGrp);
    m_replace->setMaxCount(10);
    m_replace->setDuplicatesEnabled(false);
    m_backRef = new QCheckBox(i18n("&Use Placeholders"), m_replaceGrp);
    m_backRefItem = new QPushButton(i18n("Insert Placeholder"), m_replaceGrp);
    m_backRefItem->setEnabled(false);
    m_replaceExtension = new QWidget(m_replaceGrp);

    m_replaceLayout->addWidget(m_replaceLabel, 0, 0);
    m_replaceLayout->addMultiCellWidget(m_replace, 1, 1, 0, 1);
    m_replaceLayout->addWidget(m_backRef, 2, 0);
    m_replaceLayout->addWidget(m_backRefItem, 2, 1);
    m_replaceLayout->addMultiCellWidget(m_replaceExtension, 3, 3, 0, 1);
    topLayout->addWidget(m_replaceGrp);

    m_optionGrp = new QGroupBox(0, Qt::Vertical, i18n("Options"), page);
    m_optionGrp->layout()->setSpacing(0);
    m_optionGrp->layout()->setMargin(0);
    optionsLayout = new QGridLayout(m_optionGrp->layout());
    optionsLayout->setSpacing(6);
    optionsLayout->setMargin(11);

    m_caseSensitive = new QCheckBox(i18n("C&ase Sensitive"), m_optionGrp);
    m_wholeWordsOnly = new QCheckBox(i18n("&Whole Words Only"), m_optionGrp);
    m_fromCursor = new QCheckBox(i18n("&From Cursor"), m_optionGrp);
    m_findBackwards = new QCheckBox(i18n("Find &Backwards"), m_optionGrp);
    m_selectedText = new QCheckBox(i18n("&Selected Text"), m_optionGrp);
    m_promptOnReplace = new QCheckBox(i18n("&Prompt On Replace"), m_optionGrp);

    optionsLayout->addWidget(m_caseSensitive, 0, 0);
    optionsLayout->addWidget(m_wholeWordsOnly, 1, 0);
    optionsLayout->addWidget(m_fromCursor, 2, 0);
    optionsLayout->addWidget(m_findBackwards, 0, 1);
    optionsLayout->addWidget(m_selectedText, 1, 1);
    optionsLayout->addWidget(m_promptOnReplace, 2, 1);
    topLayout->addWidget(m_optionGrp);

    // We delay creation of these until needed.
    m_patterns = 0L;
    m_placeholders = 0L;

    // signals and slots connections
    connect(m_regExp, SIGNAL(toggled(bool)), m_regExpItem, SLOT(setEnabled(bool)));
    connect(m_backRef, SIGNAL(toggled(bool)), m_backRefItem, SLOT(setEnabled(bool)));
    connect(m_regExpItem, SIGNAL(pressed()), this, SLOT(showPatterns()));
    connect(m_backRefItem, SIGNAL(pressed()), this, SLOT(showPlaceholders()));

    // tab order
    setTabOrder(m_find, m_regExp);
    setTabOrder(m_regExp, m_regExpItem);
    setTabOrder(m_regExpItem, m_replace);
    setTabOrder(m_replace, m_backRef);
    setTabOrder(m_backRef, m_backRefItem);
    setTabOrder(m_backRefItem, m_caseSensitive);
    setTabOrder(m_caseSensitive, m_wholeWordsOnly);
    setTabOrder(m_wholeWordsOnly, m_fromCursor);
    setTabOrder(m_fromCursor, m_findBackwards);
    setTabOrder(m_findBackwards, m_selectedText);
    setTabOrder(m_selectedText, m_promptOnReplace);

    // buddies
    m_findLabel->setBuddy(m_find);
    m_replaceLabel->setBuddy(m_replace);

    if (!forReplace)
    {
        m_promptOnReplace->hide();
        m_replaceGrp->hide();
    }
    setFindHistory(findStrings);
    m_find->lineEdit()->selectAll();
    m_find->setFocus();
}

long KoFindDialog::options() const
{
    long options = 0;

    if (m_caseSensitive->isChecked())
        options |= CaseSensitive;
    if (m_wholeWordsOnly->isChecked())
        options |= WholeWordsOnly;
    if (m_fromCursor->isChecked())
        options |= FromCursor;
    if (m_findBackwards->isChecked())
        options |= FindBackwards;
    if (m_selectedText->isChecked())
        options |= SelectedText;
    if (m_regExp->isChecked())
        options |= RegularExpression;
    return options;
}

QString KoFindDialog::pattern() const
{
    return m_find->currentText();
}

void KoFindDialog::setFindHistory(const QStringList &strings)
{
    if (strings.count() > 0)
        m_find->setHistoryItems(strings, true);
    else
        m_find->clearHistory();
}

void KoFindDialog::setOptions(long options)
{
    m_caseSensitive->setChecked(options & CaseSensitive);
    m_wholeWordsOnly->setChecked(options & WholeWordsOnly);
    m_fromCursor->setChecked(options & FromCursor);
    m_findBackwards->setChecked(options & FindBackwards);
    m_selectedText->setChecked(options & SelectedText);
    m_regExp->setChecked(options & RegularExpression);
}

// Create a popup menu with a list of regular expression terms, to help the user
// compose a regular expression search pattern.
void KoFindDialog::showPatterns()
{
    typedef struct
    {
        const char *description;
        const char *regExp;
        int cursorAdjustment;
    } term;
    static const term items[] =
    {
        { I18N_NOOP("Any Character"),                 ".",        0 },
        { I18N_NOOP("Start of Line"),                 "^",        0 },
        { I18N_NOOP("End of Line"),                   "$",        0 },
        { I18N_NOOP("Set of Characters"),             "[]",       -1 },
        { I18N_NOOP("Repeats, zero or more times"),   "*",        0 },
        { I18N_NOOP("Repeats, one or more times"),    "+",        0 },
        { I18N_NOOP("Optional"),                      "?",        0 },
        { I18N_NOOP("Escape"),                        "\\",       0 },
        { I18N_NOOP("TAB"),                           "\\t",      0 },
        { I18N_NOOP("NL"),                            "\\n",      0 },
        { I18N_NOOP("CR"),                            "\\r",      0 },
        { I18N_NOOP("White Space"),                   "\\s",      0 },
        { I18N_NOOP("Digit"),                         "\\d",      0 },
        { I18N_NOOP("Unicode point"),                 "\x0000",   0 },
        { I18N_NOOP("ASCII/Latin-1 character"),       "\000",     0 }
    };
    int i;

    // Populate the popup menu.
    if (!m_patterns)
    {
        m_patterns = new QPopupMenu(this);
        for (i = 0; (unsigned)i < sizeof(items) / sizeof(items[0]); i++)
        {
            m_patterns->insertItem(items[i].description, i, i);
        }
    }

    // Insert the selection into the edit control.
    i = m_patterns->exec(QCursor::pos());
    if (i != -1)
    {
        QLineEdit *editor = m_find->lineEdit();

        editor->insert(items[i].regExp);
        editor->setCursorPosition(editor->cursorPosition() + items[i].cursorAdjustment);
    }
}

// Create a popup menu with a list of backreference terms, to help the user
// compose a regular expression replacement pattern.
void KoFindDialog::showPlaceholders()
{
    typedef struct
    {
        const char *description;
        const char *backRef;
    } term;
    static const term items[] =
    {
        { I18N_NOOP("Complete Text found"),             "/0" },
    };
    int i;

    // Populate the popup menu.
    if (!m_placeholders)
    {
        m_placeholders = new QPopupMenu(this);
        for (i = 0; (unsigned)i < sizeof(items) / sizeof(items[0]); i++)
        {
            m_placeholders->insertItem(items[i].description, i, i);
        }
    }

    // Insert the selection into the edit control.
    i = m_placeholders->exec(QCursor::pos());
    if (i != -1)
    {
        QLineEdit *editor = m_replace->lineEdit();

        editor->insert(items[i].backRef);
    }
}

void KoFindDialog::slotOk()
{
    // Nothing to find?
    if (pattern().isEmpty())
    {
        KMessageBox::error(this, i18n("You must enter some text to search for."));
        return;
    }

    if (m_regExp->isChecked())
    {
        // Check for a valid regular expression.
        QRegExp regExp(pattern());

        if (!regExp.isValid())
        {
            KMessageBox::error(this, i18n("Invalid regular expression."));
            return;
        }
    }
    m_find->addToHistory(pattern());
    emit okClicked();
    accept();
}

// Create the dialog.
KoFind::KoFind(const QString &pattern, long options, QWidget *parent) :
    KDialogBase(parent, __FILE__, false,  // non-modal!
        i18n("Find next %1").arg(pattern),
        User1 | Close,
        User1,
        false,
        i18n("&Yes"))
{
    m_cancelled = false;
    m_options = options;
    m_parent = parent;
    m_matches = 0;
    if (m_options & KoFindDialog::RegularExpression)
        m_regExp = new QRegExp(pattern, m_options & KoFindDialog::CaseSensitive);
    else
        m_pattern = pattern;
    resize(minimumSize());
}

KoFind::~KoFind()
{
    if (!m_matches)
        KMessageBox::information(m_parent, i18n("No match was found."));
}

void KoFind::closeEvent(QCloseEvent */*close*/)
{
    m_cancelled = true;
    kapp->exit_loop();
}

bool KoFind::find(const QString &text, const QRect &expose)
{
    if (m_options & KoFindDialog::FindBackwards)
    {
        m_index = text.length();
    }
    else
    {
        m_index = 0;
    }
    m_text = text;
    m_expose = expose;
    do
    {
        // Find the next match.
        if (m_options & KoFindDialog::RegularExpression)
            m_index = KoFind::find(m_text, *m_regExp, m_index, m_options, &m_matchedLength);
        else
            m_index = KoFind::find(m_text, m_pattern, m_index, m_options, &m_matchedLength);
        if (m_index != -1)
        {
            // Tell the world about the match we found, in case someone wants to
            // highlight it.
            emit highlight(m_text, m_index, m_matchedLength, m_expose);
            show();
            kapp->enter_loop();
        }
    }
    while ((m_index != -1) && !m_cancelled);

    // Should the user continue?
    return !m_cancelled;
}

int KoFind::find(const QString &text, const QString &pattern, int index, long options, int *matchedLength)
{
    // Handle regular expressions in the appropriate way.
    if (options & KoFindDialog::RegularExpression)
    {
        QRegExp regExp(pattern, options & KoFindDialog::CaseSensitive);

        return find(text, regExp, index, options, matchedLength);
    }

    bool caseSensitive = (options & KoFindDialog::CaseSensitive);

    if (options & KoFindDialog::WholeWordsOnly)
    {
        if (options & KoFindDialog::FindBackwards)
        {
            // Backward search, until the beginning of the line...
            while (index >= 0)
            {
                // ...find the next match.
                index = text.findRev(pattern, index, caseSensitive);
                if (index == -1)
                    break;

                // Is the match delimited correctly?
                *matchedLength = pattern.length();
                if (isWholeWords(text, index, *matchedLength))
                    break;
                index--;
            }
        }
        else
        {
            // Forward search, until the end of the line...
            while (index < (int)text.length())
            {
                // ...find the next match.
                index = text.find(pattern, index, caseSensitive);
                if (index == -1)
                    break;

                // Is the match delimited correctly?
                *matchedLength = pattern.length();
                if (isWholeWords(text, index, *matchedLength))
                    break;
                index++;
            }
        }
    }
    else
    {
        // Non-whole-word search.
        if (options & KoFindDialog::FindBackwards)
        {
            index = text.findRev(pattern, index, caseSensitive);
        }
        else
        {
            index = text.find(pattern, index, caseSensitive);
        }
        if (index != -1)
        {
            *matchedLength = pattern.length();
        }
    }
    return index;
}

int KoFind::find(const QString &text, const QRegExp &pattern, int index, long options, int *matchedLength)
{
    if (options & KoFindDialog::WholeWordsOnly)
    {
        if (options & KoFindDialog::FindBackwards)
        {
            // Backward search, until the beginning of the line...
            while (index >= 0)
            {
                // ...find the next match.
                index = text.findRev(pattern, index);
                if (index == -1)
                    break;

                // Is the match delimited correctly?
                pattern.match(text, index, matchedLength, false);
                if (isWholeWords(text, index, *matchedLength))
                    break;
                index--;
            }
        }
        else
        {
            // Forward search, until the end of the line...
            while (index < (int)text.length())
            {
                // ...find the next match.
                index = text.find(pattern, index);
                if (index == -1)
                    break;

                // Is the match delimited correctly?
                pattern.match(text, index, matchedLength, false);
                if (isWholeWords(text, index, *matchedLength))
                    break;
                index++;
            }
        }
    }
    else
    {
        // Non-whole-word search.
        if (options & KoFindDialog::FindBackwards)
        {
            index = text.findRev(pattern, index);
        }
        else
        {
            index = text.find(pattern, index);
        }
        if (index != -1)
        {
            pattern.match(text, index, matchedLength, false);
        }
    }
    return index;
}

bool KoFind::isInWord(QChar ch)
{
    return ch.isLetter() || ch.isDigit() || ch == '_';
}

bool KoFind::isWholeWords(const QString &text, int starts, int matchedLength)
{
    if ((starts == 0) || (!isInWord(text[starts - 1])))
    {
        int ends = starts + matchedLength;

        if ((ends == (int)text.length()) || (!isInWord(text[ends])))
            return true;
    }
    return false;
}

// Yes.
void KoFind::slotUser1()
{
    m_matches++;
    if (m_options & KoFindDialog::FindBackwards)
        m_index--;
    else
        m_index++;
    kapp->exit_loop();
}

#include "koFind.moc"
