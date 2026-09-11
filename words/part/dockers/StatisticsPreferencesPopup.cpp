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
    , w(std::make_unique<Ui::StatisticsPreferencesPopup>())
{
    w->setupUi(this);
    connect(w->check_words, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::wordsDisplayChange);
    connect(w->check_sentences, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::sentencesDisplayChange);
    connect(w->check_syllables, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::syllablesDisplayChange);
    connect(w->check_lines, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::linesDisplayChange);
    connect(w->check_charspace, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::charspaceDisplayChange);
    connect(w->check_charnospace, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::charnospaceDisplayChange);
    connect(w->check_east, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::eastDisplayChange);
    connect(w->check_flesch, &QCheckBox::checkStateChanged, this, &StatisticsPreferencesPopup::fleschDisplayChange);
}

StatisticsPreferencesPopup::~StatisticsPreferencesPopup() = default;

QSize StatisticsPreferencesPopup::sizeHint() const
{
    return layout()->sizeHint();
}
