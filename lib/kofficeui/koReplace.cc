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
#include <qlabel.h>
#include <qregexp.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include "koReplace.h"

KoReplaceDialog::KoReplaceDialog(QWidget *parent, const char *name, long options, const QStringList &findStrings, const QStringList &replaceStrings, bool hasSelection) :
    KoFindDialog(parent, name, true)
{
    init(true, findStrings, hasSelection);
    setOptions(options);
    setReplacementHistory(replaceStrings);
}

KoReplaceDialog::~KoReplaceDialog()
{
}

long KoReplaceDialog::options() const
{
    long options = 0;

    options = KoFindDialog::options();
    if (m_promptOnReplace->isChecked())
        options |= PromptOnReplace;
    if (m_backRef->isChecked())
        options |= BackReference;
    return options;
}

QWidget *KoReplaceDialog::replaceExtension()
{
    return m_replaceExtension;
}

QString KoReplaceDialog::replacement() const
{
    return m_replace->currentText();
}

QStringList KoReplaceDialog::replacementHistory() const
{
    return m_find->historyItems();
}

void KoReplaceDialog::setOptions(long options)
{
    KoFindDialog::setOptions(options);
    m_promptOnReplace->setChecked(options & PromptOnReplace);
    m_backRef->setChecked(options & BackReference);
}

void KoReplaceDialog::setReplacementHistory(const QStringList &strings)
{
    if (strings.count() > 0)
        m_replace->setHistoryItems(strings, true);
    else
        m_replace->clearHistory();
}

void KoReplaceDialog::slotOk()
{
    KoFindDialog::slotOk();
    m_replace->addToHistory(replacement());
}

class KoReplace::KoReplacePrivate
{
public:
    void setLabel( const QString& pattern, const QString& replacement ) {
        m_mainLabel->setText( i18n("Replace '%1' with '%2'?").arg(pattern).arg(replacement) );
    }
    QLabel* m_mainLabel;
};

// Create the dialog.
KoReplace::KoReplace(const QString &pattern, const QString &replacement, long options, QWidget *parent) :
    KoFind( pattern, replacement, options, parent )
{
    d = new KoReplacePrivate;
    d->m_mainLabel = new QLabel( this );
    d->setLabel( pattern, replacement );
    setMainWidget( d->m_mainLabel );

    m_replacements = 0;
    m_replacement = replacement;
}

KoReplace::~KoReplace()
{
    if (displayFinalDialog() && !m_cancelled)
    {
        if ( !m_replacements )
            KMessageBox::information(parentWidget(), i18n("No text was replaced."));
        else
            KMessageBox::information(parentWidget(), i18n("1 replacement done.", "%n replacements done.", m_replacements ) );

    }
    setDisplayFinalDialog( false ); // don't display the KoFind dialog :)
    delete d;
}

bool KoReplace::replace(QString &text, const QRect &expose)
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
        if (m_options & KoReplaceDialog::RegularExpression)
            m_index = KoFind::find(m_text, *m_regExp, m_index, m_options, &m_matchedLength);
        else
            m_index = KoFind::find(m_text, m_pattern, m_index, m_options, &m_matchedLength);
        if (m_index != -1)
        {
            if (m_options & KoReplaceDialog::PromptOnReplace)
            {
                if ( validateMatch( m_text, m_index, m_matchedLength ))
                {
                    // Display accurate initial string and replacement string, they can vary
                    QString matchedText = m_text.mid( m_index, m_matchedLength );
                    QString rep = matchedText;
                    KoReplace::replace(rep, m_replacement, 0, m_matchedLength);
                    d->setLabel( matchedText, rep );

                    // Tell the world about the match we found, in case someone wants to
                    // highlight it.
                    emit highlight(m_text, m_index, m_matchedLength, m_expose);
                    show();
                    kapp->enter_loop();
                }
                else
                    m_index += m_matchedLength;
            }
            else
            {
                doReplace();
            }
        }
    }
    while ((m_index != -1) && !m_cancelled);
    text = m_text;

    // Should the user continue?
    return !m_cancelled;
}

int KoReplace::replace(QString &text, const QString &pattern, const QString &replacement, int index, long options, int *replacedLength)
{
    int matchedLength;

    index = KoFind::find(text, pattern, index, options, &matchedLength);
    if (index != -1)
    {
        *replacedLength = replace(text, replacement, index, matchedLength);
        if (options & KoReplaceDialog::FindBackwards)
            index--;
        else
            index += *replacedLength;
    }
    return index;
}

int KoReplace::replace(QString &text, const QRegExp &pattern, const QString &replacement, int index, long options, int *replacedLength)
{
    int matchedLength;

    index = KoFind::find(text, pattern, index, options, &matchedLength);
    if (index != -1)
    {
        *replacedLength = replace(text, replacement, index, matchedLength);
        if (options & KoReplaceDialog::FindBackwards)
            index--;
        else
            index += *replacedLength;
    }
    return index;
}

int KoReplace::replace(QString &text, const QString &replacement, int index, int length)
{
    // Backreferences: replace /0 with the right portion of 'text'
    QString rep = replacement;
    rep.replace( QRegExp("/0"), text.mid( index, length ) );
    // Then replace rep into the text
    text.replace(index, length, rep);
    return rep.length();
}

// All.
void KoReplace::slotUser1()
{
    doReplace();
    m_options &= ~KoReplaceDialog::PromptOnReplace;
    kapp->exit_loop();
}

// Skip.
void KoReplace::slotUser2()
{
    if (m_options & KoReplaceDialog::FindBackwards) m_index--;
       else m_index++;
    kapp->exit_loop();
}

// Yes.
void KoReplace::slotUser3()
{
    doReplace();
    kapp->exit_loop();
}

void KoReplace::doReplace()
{
    int replacedLength = KoReplace::replace(m_text, m_replacement, m_index, m_matchedLength);

    // Tell the world about the replacement we made, in case someone wants to
    // highlight it.
    emit replace(m_text, m_index, replacedLength, m_matchedLength, m_expose);
    m_replacements++;
    if (m_options & KoReplaceDialog::FindBackwards)
        m_index--;
    else
        m_index += replacedLength;
}

void KoReplace::resetCounts()
{
    KoFind::resetCounts();
    m_replacements = 0;
}

bool KoReplace::shouldRestart( bool forceAsking ) const
{
    // Only ask if we did a "find from cursor", otherwise it's pointless.
    // ... Or if the prompt-on-replace option was set.
    // Well, unless the user can modify the document during a search operation,
    // hence the force boolean.
    if ( !forceAsking && (m_options & KoFindDialog::FromCursor) == 0
         && (m_options & KoReplaceDialog::PromptOnReplace) == 0 )
        return false;
    QString message;
    if ( !m_replacements )
        message = i18n("No text was replaced.");
    else
        message = i18n("1 replacement done.", "%n replacements done.", m_replacements );

    // Hope this word puzzle is ok, it's a different sentence
    message += "\n";
    message += i18n("Do you want to restart search at the beginning?");

    int ret = KMessageBox::questionYesNo( parentWidget(), message );
    return( ret == KMessageBox::Yes );
}

#include "koReplace.moc"
