/* This file is part of the KDE project
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef STATISTICSPREFERENCESPOPUP_H
#define STATISTICSPREFERENCESPOPUP_H

#include <QWidget>
#include<QToolButton>
#include<QMenu>

class QMenu;


namespace Ui {
    class StatisticsPreferencesPopup;
}

class StatisticsPreferencesPopup : public QMenu
{
    Q_OBJECT
public:
    explicit StatisticsPreferencesPopup(QWidget *parent = 0);
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

