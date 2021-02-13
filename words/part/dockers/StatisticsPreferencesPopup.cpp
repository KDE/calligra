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
    connect(w->check_words, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::wordsDisplayChange);
    connect(w->check_sentences, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::sentencesDisplayChange);
    connect(w->check_syllables, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::syllablesDisplayChange);
    connect(w->check_lines, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::linesDisplayChange);
    connect(w->check_charspace, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::charspaceDisplayChange);
    connect(w->check_charnospace, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::charnospaceDisplayChange);
    connect(w->check_east, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::eastDisplayChange);
    connect(w->check_flesch, &QCheckBox::stateChanged, this, &StatisticsPreferencesPopup::fleschDisplayChange);
}


QSize StatisticsPreferencesPopup::sizeHint() const
{
    return layout()->sizeHint();
}
