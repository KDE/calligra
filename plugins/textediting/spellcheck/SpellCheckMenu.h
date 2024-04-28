/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Christoph Goerlich <chgoerlich@gmx.de>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPELLCHECKMENU_H
#define SPELLCHECKMENU_H

#include <QObject>
#include <QPair>

#include <sonnet/speller.h>

class KActionMenu;
class QAction;
class QAction;
class QMenu;
class SpellCheck;

class SpellCheckMenu : public QObject
{
    Q_OBJECT
public:
    explicit SpellCheckMenu(const Sonnet::Speller &speller, SpellCheck *spellCheck);
    ~SpellCheckMenu() override;

    QPair<QString, QAction *> menuAction();
    void setMisspelled(const QString &word, int position, int length);
    void setEnabled(bool b);
    void setVisible(bool b);
    void setCurrentLanguage(const QString &language);

Q_SIGNALS:
    void clearHighlightingForWord(int startPosition);

private Q_SLOTS:
    void createSuggestionsMenu();
    void replaceWord(const QString &suggestion);
    void ignoreWord();
    void addWordToDictionary();

private:
    SpellCheck *m_spellCheck;
    Sonnet::Speller m_speller;
    KActionMenu *m_suggestionsMenuAction;
    QAction *m_ignoreWordAction;
    QAction *m_addToDictionaryAction;
    QMenu *m_suggestionsMenu;
    int m_lengthMisspelled;
    int m_currentMisspelledPosition;
    QString m_currentMisspelled;
    QStringList m_suggestions;
};

#endif // SPELLCHECKMENU_H
