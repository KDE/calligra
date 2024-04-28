/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
