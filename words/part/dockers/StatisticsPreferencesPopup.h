/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STATISTICSPREFERENCESPOPUP_H
#define STATISTICSPREFERENCESPOPUP_H

#include <QMenu>
#include <QToolButton>
#include <QWidget>

class QMenu;

namespace Ui
{
class StatisticsPreferencesPopup;
}

class StatisticsPreferencesPopup : public QMenu
{
    Q_OBJECT
public:
    explicit StatisticsPreferencesPopup(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    Ui::StatisticsPreferencesPopup *w;

Q_SIGNALS:
    void wordsDisplayChange(int);
    void sentencesDisplayChange(int);
    void syllablesDisplayChange(int);
    void linesDisplayChange(int);
    void charspaceDisplayChange(int);
    void charnospaceDisplayChange(int);
    void eastDisplayChange(int);
    void fleschDisplayChange(int);
};

#endif // StatisticsPreferencesPopup_H
