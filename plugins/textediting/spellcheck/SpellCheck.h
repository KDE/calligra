/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Christoph Goerlich <chgoerlich@gmx.de>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <KoTextEditingPlugin.h>

#include <QPointer>
#include <QQueue>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextStream>
#include <sonnet/speller.h>

class QTextDocument;
class QTextStream;
class BgSpellCheck;
class SpellCheckMenu;

class SpellCheck : public KoTextEditingPlugin
{
    Q_OBJECT
public:
    SpellCheck();

    /// reimplemented from superclass
    void finishedWord(QTextDocument *document, int cursorPosition) override;

    /// reimplemented from superclass
    void finishedParagraph(QTextDocument *document, int cursorPosition) override;

    /// reimplemented from superclass
    void startingSimpleEdit(QTextDocument *document, int cursorPosition) override;

    /// reimplemented from superclass
    void checkSection(QTextDocument *document, int startPosition, int endPosition) override;

    /// reimplemented from superclass
    void setCurrentCursorPosition(QTextDocument *document, int cursorPosition) override;

    QStringList availableBackends() const;
    QStringList availableLanguages() const;

    void setSkipAllUppercaseWords(bool b);
    void setSkipRunTogetherWords(bool b);

    QString defaultLanguage() const;
    bool backgroundSpellChecking();
    bool skipAllUppercaseWords();
    bool skipRunTogetherWords();

    bool addWordToPersonal(const QString &word, int startPosition);

    // reimplemented from Calligra2.0, we disconnect and re- connect the 'documentChanged' signal only when the document has replaced
    void setDocument(QTextDocument *document);

    void replaceWordBySuggestion(const QString &word, int startPosition, int lengthOfWord);

public Q_SLOTS:
    void setDefaultLanguage(const QString &lang);

private Q_SLOTS:
    void highlightMisspelled(const QString &word, int startPosition, bool misspelled = true);
    void finishedRun();
    void configureSpellCheck();
    void runQueue();
    void setBackgroundSpellChecking(bool b);
    void documentChanged(int from, int charsRemoved, int charsAdded);

private:
    Sonnet::Speller m_speller;
    QPointer<QTextDocument> m_document;
    QString m_word;
    BgSpellCheck *m_bgSpellCheck;
    struct SpellSections {
        SpellSections(QTextDocument *doc, int start, int end)
            : document(doc)
        {
            from = start;
            to = end;
        }
        QPointer<QTextDocument> document;
        int from;
        int to;
    };
    QQueue<SpellSections> m_documentsQueue;
    bool m_enableSpellCheck;
    bool m_documentIsLoading;
    bool m_isChecking;
    QTextStream stream;
    SpellCheckMenu *m_spellCheckMenu;
    SpellSections m_activeSection; // the section we are currently doing a run on;
    bool m_simpleEdit; // set when user is doing a simple edit, meaning we should not start spellchecking
    int m_cursorPosition; // simple edit cursor position
};

#endif
