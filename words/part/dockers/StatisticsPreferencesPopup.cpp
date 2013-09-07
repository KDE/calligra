/* This file is part of the KDE project
 * Copyright (C) 2012 Shreya Pandit<shreya@shreyapandit.com>
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

#include "StatisticsPreferencesPopup.h"
#include "ui_StatisticsPreferencesPopup.h"
#include <QMenu>

StatisticsPreferencesPopup::StatisticsPreferencesPopup(QWidget *parent)
    : QMenu(parent)
{
    w = new Ui::StatisticsPreferencesPopup();
    w->setupUi(this);
    connect(w->check_words, SIGNAL(stateChanged(int)), this, SIGNAL(wordsDisplayChange(int)));
    connect(w->check_sentences, SIGNAL(stateChanged(int)), this, SIGNAL(sentencesDisplayChange(int)));
    connect(w->check_syllables, SIGNAL(stateChanged(int)), this, SIGNAL(syllablesDisplayChange(int)));
    connect(w->check_lines, SIGNAL(stateChanged(int)), this, SIGNAL(linesDisplayChange(int)));
    connect(w->check_charspace, SIGNAL(stateChanged(int)), this, SIGNAL(charspaceDisplayChange(int)));
    connect(w->check_charnospace, SIGNAL(stateChanged(int)), this, SIGNAL(charnospaceDisplayChange(int)));
    connect(w->check_east, SIGNAL(stateChanged(int)), this, SIGNAL(eastDisplayChange(int)));
    connect(w->check_flesch, SIGNAL(stateChanged(int)), this, SIGNAL(fleschDisplayChange(int)));
}


QSize StatisticsPreferencesPopup::sizeHint() const
{
    return layout()->sizeHint();
}
