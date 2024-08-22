/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Christoph Goerlich <chgoerlich@gmx.de>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpellCheckMenu.h"
#include "SpellCheck.h"
#include "SpellCheckDebug.h"

#include <KActionMenu>
#include <KLocalizedString>

#include <QAction>
#include <QMenu>

SpellCheckMenu::SpellCheckMenu(const Sonnet::Speller &speller, SpellCheck *spellCheck)
    : QObject(spellCheck)
    , m_spellCheck(spellCheck)
    , m_speller(speller)
    , m_suggestionsMenuAction(nullptr)
    , m_ignoreWordAction(nullptr)
    , m_addToDictionaryAction(nullptr)
    , m_suggestionsMenu(nullptr)
    , m_currentMisspelledPosition(-1)
{
    m_suggestionsMenuAction = new KActionMenu(i18n("Spelling"), this);
    m_suggestionsMenu = m_suggestionsMenuAction->menu();
    connect(m_suggestionsMenu, &QMenu::aboutToShow, this, &SpellCheckMenu::createSuggestionsMenu);

    m_addToDictionaryAction = new QAction(i18n("Add to Dictionary"), this);
    connect(m_addToDictionaryAction, &QAction::triggered, this, &SpellCheckMenu::addWordToDictionary);

    // disabling this as if it calls the speller it's only changed in a local copy
    // see addWordToDictionary for how it should be done, except background checker
    // doesn't have suche a method for ignoreWord
    // Only option left is to personally ignore words

    // m_ignoreWordAction = new QAction(i18n("Ignore Word"), this);
    // connect(m_ignoreWordAction, SIGNAL(triggered()), this, SLOT(ignoreWord()));

    setEnabled(false);
    setVisible(false);
}

SpellCheckMenu::~SpellCheckMenu() = default;

QPair<QString, QAction *> SpellCheckMenu::menuAction()
{
    return QPair<QString, QAction *>("spelling_suggestions", m_suggestionsMenuAction);
}

void SpellCheckMenu::createSuggestionsMenu()
{
    m_suggestions.clear();
    m_suggestionsMenu->clear();

    m_suggestionsMenu->addAction(m_ignoreWordAction);
    m_suggestionsMenu->addAction(m_addToDictionaryAction);
    m_suggestionsMenu->addSeparator();
    if (!m_currentMisspelled.isEmpty()) {
        m_suggestions = m_speller.suggest(m_currentMisspelled);
        for (int i = 0; i < m_suggestions.count(); ++i) {
            const QString &suggestion = m_suggestions.at(i);
            QAction *action = new QAction(suggestion, m_suggestionsMenu);
            connect(action, &QAction::triggered, [this, suggestion] {
                replaceWord(suggestion);
            });
            m_suggestionsMenu->addAction(action);
        }
    }
}

void SpellCheckMenu::ignoreWord()
{
    if (m_currentMisspelled.isEmpty() || m_currentMisspelledPosition < 0)
        return;

    // see comment in ctor why this will never work
    m_speller.addToSession(m_currentMisspelled);

    Q_EMIT clearHighlightingForWord(m_currentMisspelledPosition);

    m_currentMisspelled.clear();
    m_currentMisspelledPosition = -1;
}

void SpellCheckMenu::addWordToDictionary()
{
    if (m_currentMisspelled.isEmpty() || m_currentMisspelledPosition < 0)
        return;

    m_spellCheck->addWordToPersonal(m_currentMisspelled, m_currentMisspelledPosition);

    m_currentMisspelled.clear();
    m_currentMisspelledPosition = -1;
}

void SpellCheckMenu::setMisspelled(const QString &word, int position, int length)
{
    m_currentMisspelled = word;
    m_lengthMisspelled = length;
    m_currentMisspelledPosition = position;
}

void SpellCheckMenu::setEnabled(bool b)
{
    if (m_suggestionsMenuAction)
        m_suggestionsMenuAction->setEnabled(b);

    if (m_addToDictionaryAction)
        m_addToDictionaryAction->setEnabled(b);

    if (m_ignoreWordAction)
        m_ignoreWordAction->setEnabled(b);
}

void SpellCheckMenu::setVisible(bool b)
{
    if (m_suggestionsMenuAction)
        m_suggestionsMenuAction->setVisible(b);

    if (m_addToDictionaryAction)
        m_addToDictionaryAction->setVisible(b);

    if (m_ignoreWordAction)
        m_ignoreWordAction->setVisible(b);
}

void SpellCheckMenu::replaceWord(const QString &suggestion)
{
    if (suggestion.isEmpty() || m_currentMisspelledPosition < 0)
        return;

    m_spellCheck->replaceWordBySuggestion(suggestion, m_currentMisspelledPosition, m_lengthMisspelled);

    m_currentMisspelled.clear();
    m_currentMisspelledPosition = -1;
}

void SpellCheckMenu::setCurrentLanguage(const QString &language)
{
    m_speller.setLanguage(language);
}
