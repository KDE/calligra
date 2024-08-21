/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Christoph Goerlich <chgoerlich@gmx.de>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpellCheck.h"
#include "BgSpellCheck.h"
#include "SpellCheckDebug.h"
#include "SpellCheckMenu.h"

#include <KoCharacterStyle.h>
#include <KoTextBlockData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootAreaProvider.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <ktoggleaction.h>
#include <sonnet/configdialog.h>

#include <QAction>
#include <QApplication>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QThread>
#include <QTimer>

SpellCheck::SpellCheck()
    : m_document(nullptr)
    , m_bgSpellCheck(nullptr)
    , m_enableSpellCheck(true)
    , m_documentIsLoading(false)
    , m_isChecking(false)
    , m_spellCheckMenu(nullptr)
    , m_activeSection(nullptr, 0, 0)
    , m_simpleEdit(false)
    , m_cursorPosition(0)
{
    /* setup actions for this plugin */
    QAction *configureAction = new QAction(i18n("Configure &Spell Checking..."), this);
    connect(configureAction, &QAction::triggered, this, &SpellCheck::configureSpellCheck);
    addAction("tool_configure_spellcheck", configureAction);

    KToggleAction *spellCheck = new KToggleAction(i18n("Auto Spell Check"), this);
    addAction("tool_auto_spellcheck", spellCheck);

    KConfigGroup spellConfig = KSharedConfig::openConfig()->group("Spelling");
    m_enableSpellCheck = spellConfig.readEntry("autoSpellCheck", m_enableSpellCheck);
    spellCheck->setChecked(m_enableSpellCheck);
    m_speller = Sonnet::Speller(spellConfig.readEntry("defaultLanguage", "en_US"));
    m_bgSpellCheck = new BgSpellCheck(m_speller, this);

    m_spellCheckMenu = new SpellCheckMenu(m_speller, this);
    QPair<QString, QAction *> pair = m_spellCheckMenu->menuAction();
    addAction(pair.first, pair.second);

    connect(m_bgSpellCheck, &BgSpellCheck::misspelledWord, this, &SpellCheck::highlightMisspelled);
    connect(m_bgSpellCheck, &Sonnet::BackgroundChecker::done, this, &SpellCheck::finishedRun);
    connect(spellCheck, &QAction::toggled, this, &SpellCheck::setBackgroundSpellChecking);
}

void SpellCheck::finishedWord(QTextDocument *document, int cursorPosition)
{
    setDocument(document);
    if (!m_enableSpellCheck)
        return;

    QTextBlock block = document->findBlock(cursorPosition);
    if (!block.isValid())
        return;
    KoTextBlockData blockData(block);
    blockData.setMarkupsLayoutValidity(KoTextBlockData::Misspell, false);
    checkSection(document, block.position(), block.position() + block.length() - 1);
}

void SpellCheck::finishedParagraph(QTextDocument *document, int cursorPosition)
{
    setDocument(document);
    Q_UNUSED(document);
    Q_UNUSED(cursorPosition);
}

void SpellCheck::startingSimpleEdit(QTextDocument *document, int cursorPosition)
{
    m_simpleEdit = true;
    setDocument(document);
    m_cursorPosition = cursorPosition;
}

void SpellCheck::checkSection(QTextDocument *document, int startPosition, int endPosition)
{
    if (startPosition >= endPosition) { // no work
        return;
    }

    foreach (const SpellSections &ss, m_documentsQueue) {
        if (ss.from <= startPosition && ss.to >= endPosition) {
            runQueue();
            m_spellCheckMenu->setVisible(true);
            return;
        }
        // TODO also check if we should replace an existing queued item with a longer span
    }

    SpellSections ss(document, startPosition, endPosition);
    m_documentsQueue.enqueue(ss);
    runQueue();
    m_spellCheckMenu->setVisible(true);
}

void SpellCheck::setDocument(QTextDocument *document)
{
    if (m_document == document)
        return;
    if (m_document)
        disconnect(document, &QTextDocument::contentsChange, this, &SpellCheck::documentChanged);

    m_document = document;
    connect(document, &QTextDocument::contentsChange, this, &SpellCheck::documentChanged);
}

QStringList SpellCheck::availableBackends() const
{
    return m_speller.availableBackends();
}

QStringList SpellCheck::availableLanguages() const
{
    return m_speller.availableLanguages();
}

void SpellCheck::setDefaultLanguage(const QString &language)
{
    m_speller.setDefaultLanguage(language);
    m_bgSpellCheck->setDefaultLanguage(language);
    if (m_enableSpellCheck && m_document) {
        checkSection(m_document, 0, m_document->characterCount() - 1);
    }
}

void SpellCheck::setBackgroundSpellChecking(bool on)
{
    if (m_enableSpellCheck == on)
        return;
    KConfigGroup spellConfig = KSharedConfig::openConfig()->group("Spelling");
    m_enableSpellCheck = on;
    spellConfig.writeEntry("autoSpellCheck", m_enableSpellCheck);
    if (m_document) {
        if (!m_enableSpellCheck) {
            for (QTextBlock block = m_document->begin(); block != m_document->end(); block = block.next()) {
                KoTextBlockData blockData(block);
                blockData.clearMarkups(KoTextBlockData::Misspell);
            }
            m_spellCheckMenu->setEnabled(false);
            m_spellCheckMenu->setVisible(false);
        } else {
            // when re-enabling 'Auto Spell Check' we want spellchecking the whole document
            checkSection(m_document, 0, m_document->characterCount() - 1);
            m_spellCheckMenu->setVisible(true);
        }
    }
}

void SpellCheck::setSkipAllUppercaseWords(bool on)
{
    m_speller.setAttribute(Speller::CheckUppercase, !on);
}

void SpellCheck::setSkipRunTogetherWords(bool on)
{
    m_speller.setAttribute(Speller::SkipRunTogether, on);
}

bool SpellCheck::addWordToPersonal(const QString &word, int startPosition)
{
    QTextBlock block = m_document->findBlock(startPosition);
    if (!block.isValid())
        return false;

    KoTextBlockData blockData(block);
    blockData.setMarkupsLayoutValidity(KoTextBlockData::Misspell, false);
    checkSection(m_document, block.position(), block.position() + block.length() - 1);
    // TODO we should probably recheck the entire document so other occurrences are also removed, but then again we should recheck every document (footer,header
    // etc) not sure how to do this
    return m_bgSpellCheck->addWordToPersonal(word);
}

QString SpellCheck::defaultLanguage() const
{
    return m_speller.defaultLanguage();
}

bool SpellCheck::backgroundSpellChecking()
{
    return m_enableSpellCheck;
}

bool SpellCheck::skipAllUppercaseWords()
{
    return m_speller.testAttribute(Speller::CheckUppercase);
}

bool SpellCheck::skipRunTogetherWords()
{
    return m_speller.testAttribute(Speller::SkipRunTogether);
}

// TODO:
// 1) When editing a misspelled word it should be spellchecked on the fly so the markup is removed when it is OK.
// 2) Deleting a character should be treated as a simple edit
void SpellCheck::highlightMisspelled(const QString &word, int startPosition, bool misspelled)
{
    if (!misspelled)
        return;

#if 0
    // DEBUG
class MyThread : public QThread { public: static void mySleep(unsigned long msecs) { msleep(msecs); }};
static_cast<MyThread*>(QThread::currentThread())->mySleep(400);
#endif

    QTextBlock block = m_activeSection.document->findBlock(startPosition);
    KoTextBlockData blockData(block);
    blockData.appendMarkup(KoTextBlockData::Misspell, startPosition - block.position(), startPosition - block.position() + word.trimmed().length());
}

void SpellCheck::documentChanged(int from, int charsRemoved, int charsAdded)
{
    QTextDocument *document = qobject_cast<QTextDocument *>(sender());
    if (document == nullptr)
        return;

    // If a simple edit, we use the cursor position to determine where
    // the change occurred. This makes it possible to handle cases
    // where formatting of a block has changed, eg. when dropcaps is used.
    // QTextDocument then reports the change as if the whole block has changed.
    // Ex: Having a 10 char line and you add a char at pos 7:
    // from = block->position()
    // charsRemoved = 10
    // charsAdded = 11
    // m_cursorPosition = 7
    int pos = m_simpleEdit ? m_cursorPosition : from;
    QTextBlock block = document->findBlock(pos);
    if (!block.isValid())
        return;

    do {
        KoTextBlockData blockData(block);
        if (m_enableSpellCheck) {
            // This block and all blocks after this must be relayouted
            blockData.setMarkupsLayoutValidity(KoTextBlockData::Misspell, false);
            // If it's a simple edit we will wait until finishedWord before spellchecking
            // but we need to adjust all markups behind the added/removed character(s)
            if (m_simpleEdit) {
                // Since markups work on positions within each block only the edited block must be rebased
                if (block.position() <= pos) {
                    blockData.rebaseMarkups(KoTextBlockData::Misspell, pos - block.position(), charsAdded - charsRemoved);
                }
            } else {
                // handle not so simple edits (like cut/paste etc)
                checkSection(document, block.position(), block.position() + block.length() - 1);
            }
        } else {
            blockData.clearMarkups(KoTextBlockData::Misspell);
        }
        block = block.next();
    } while (block.isValid() && block.position() <= from + charsAdded);

    m_simpleEdit = false;
}

void SpellCheck::runQueue()
{
    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
    if (m_isChecking)
        return;
    while (!m_documentsQueue.isEmpty()) {
        m_activeSection = m_documentsQueue.dequeue();
        if (m_activeSection.document.isNull())
            continue;
        QTextBlock block = m_activeSection.document->findBlock(m_activeSection.from);
        if (!block.isValid())
            continue;
        m_isChecking = true;
        do {
            KoTextBlockData blockData(block);
            blockData.clearMarkups(KoTextBlockData::Misspell);
            block = block.next();
        } while (block.isValid() && block.position() < m_activeSection.to);

        m_bgSpellCheck->startRun(m_activeSection.document, m_activeSection.from, m_activeSection.to);
        break;
    }
}

void SpellCheck::configureSpellCheck()
{
    Sonnet::ConfigDialog *dialog = new Sonnet::ConfigDialog(nullptr);
    connect(dialog, &Sonnet::ConfigDialog::languageChanged, this, &SpellCheck::setDefaultLanguage);
    dialog->exec();
    delete dialog;
}

void SpellCheck::finishedRun()
{
    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
    m_isChecking = false;

    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(m_activeSection.document->documentLayout());
    lay->provider()->updateAll();

    QTimer::singleShot(0, this, &SpellCheck::runQueue);
}

void SpellCheck::setCurrentCursorPosition(QTextDocument *document, int cursorPosition)
{
    setDocument(document);
    if (m_enableSpellCheck) {
        // check if word at cursor is misspelled
        QTextBlock block = m_document->findBlock(cursorPosition);
        if (block.isValid()) {
            KoTextBlockData blockData(block);
            KoTextBlockData::MarkupRange range = blockData.findMarkup(KoTextBlockData::Misspell, cursorPosition - block.position());
            if (int length = range.lastChar - range.firstChar) {
                QString word = block.text().mid(range.firstChar, length);
                m_spellCheckMenu->setMisspelled(word, block.position() + range.firstChar, length);
                QString language = m_bgSpellCheck->currentLanguage();
                if (!m_bgSpellCheck->currentLanguage().isEmpty() && !m_bgSpellCheck->currentCountry().isEmpty()) {
                    language += '_';
                }
                language += m_bgSpellCheck->currentCountry();
                m_spellCheckMenu->setCurrentLanguage(language);
                m_spellCheckMenu->setVisible(true);
                m_spellCheckMenu->setEnabled(true);
                return;
            }
            m_spellCheckMenu->setEnabled(false);
        } else {
            m_spellCheckMenu->setEnabled(false);
        }
    }
}

void SpellCheck::replaceWordBySuggestion(const QString &word, int startPosition, int lengthOfWord)
{
    if (!m_document)
        return;

    QTextBlock block = m_document->findBlock(startPosition);
    if (!block.isValid())
        return;

    QTextCursor cursor(m_document);
    cursor.setPosition(startPosition);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, lengthOfWord);
    cursor.removeSelectedText();
    cursor.insertText(word);
}
