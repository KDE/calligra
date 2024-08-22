/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWStatisticsWidget.h"

#include "KWDocument.h"
#include "KWDocumentStatistics.h"
#include "Words.h"
#include "dockers/StatisticsPreferencesPopup.h"
#include "ui_StatisticsPreferencesPopup.h"
#include <KoIcon.h>
#include <KoTextDocumentLayout.h>

#include <KConfigGroup>
#include <KSharedConfig>

#include <QLocale>

KWStatisticsWidget::KWStatisticsWidget(QWidget *parent, bool shortVersion)
    : QWidget(parent)
    , m_document(nullptr)
{
    this->shortVersion = shortVersion;

    initUi();
    initLayout();
    // All kind of stuff related to the option menu, unnecessary stuff in short version
    if (!shortVersion) {
        m_menu = new StatisticsPreferencesPopup(m_preferencesButton);
        m_preferencesButton->setMenu(m_menu);
        m_preferencesButton->setPopupMode(QToolButton::InstantPopup);
        m_preferencesButton->setIcon(koIcon("configure"));

        connect(m_menu, &StatisticsPreferencesPopup::wordsDisplayChange, this, &KWStatisticsWidget::wordsDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::sentencesDisplayChange, this, &KWStatisticsWidget::sentencesDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::linesDisplayChange, this, &KWStatisticsWidget::linesDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::syllablesDisplayChange, this, &KWStatisticsWidget::syllablesDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::charspaceDisplayChange, this, &KWStatisticsWidget::charspaceDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::charnospaceDisplayChange, this, &KWStatisticsWidget::charnospaceDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::eastDisplayChange, this, &KWStatisticsWidget::eastDisplayChanged);
        connect(m_menu, &StatisticsPreferencesPopup::fleschDisplayChange, this, &KWStatisticsWidget::fleschDisplayChanged);

        connect(m_preferencesButton, &QAbstractButton::clicked, m_preferencesButton, &QToolButton::showMenu);
    }

    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    bool visible = false;

    // --- Elements present in short AND full version ---
    visible = cfgGroup.readEntry("WordsVisible", true);
    visible |= shortVersion;
    m_wordsLabel->setVisible(visible);
    m_countWords->setVisible(visible);
    if (visible && !shortVersion) {
        m_menu->w->check_words->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("SentencesVisible", true);
    visible |= shortVersion;
    m_sentencesLabel->setVisible(visible);
    m_countSentences->setVisible(visible);
    if (visible && !shortVersion) {
        m_menu->w->check_sentences->setCheckState(Qt::Checked);
    }

    // --- Elements present ONLY in full version --
    visible = cfgGroup.readEntry("FleschVisible", true);
    visible &= !shortVersion;
    m_fleschLabel->setVisible(visible);
    m_countFlesch->setVisible(visible);
    if (visible) {
        m_menu->w->check_flesch->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("SyllablesVisible", true);
    visible &= !shortVersion;
    m_syllablesLabel->setVisible(visible);
    m_countSyllables->setVisible(visible);
    if (visible) {
        m_menu->w->check_syllables->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("LinesVisible", true);
    visible &= !shortVersion;
    m_linesLabel->setVisible(visible);
    m_countLines->setVisible(visible);
    if (visible) {
        m_menu->w->check_lines->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("EastAsianCharactersVisible", true);
    visible &= !shortVersion;
    m_cjkcharsLabel->setVisible(visible);
    m_countCjkchars->setVisible(visible);
    if (visible) {
        m_menu->w->check_east->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("CharspacesVisible", true);
    visible &= !shortVersion;
    m_spacesLabel->setVisible(visible);
    m_countSpaces->setVisible(visible);
    if (visible) {
        m_menu->w->check_charspace->setCheckState(Qt::Checked);
    }

    visible = cfgGroup.readEntry("CharnospacesVisible", true);
    visible &= !shortVersion;
    m_nospacesLabel->setVisible(visible);
    m_countNospaces->setVisible(visible);
    if (visible) {
        m_menu->w->check_charnospace->setCheckState(Qt::Checked);
    }
}

KWStatisticsWidget::~KWStatisticsWidget() = default;

void KWStatisticsWidget::initUi()
{
    m_wordsLabel = new QLabel(i18n("Words:"));
    m_countWords = new QLabel;

    m_sentencesLabel = new QLabel(i18n("Sentences:"));
    m_countSentences = new QLabel;

    m_syllablesLabel = new QLabel(i18n("Syllables:"));
    m_countSyllables = new QLabel;

    m_spacesLabel = new QLabel(i18n("Characters (spaces):"));
    m_countSpaces = new QLabel;

    m_nospacesLabel = new QLabel(i18n("Characters (no spaces):"));
    m_countNospaces = new QLabel;

    m_linesLabel = new QLabel(i18n("Lines:"));
    m_countLines = new QLabel;

    m_fleschLabel = new QLabel(i18n("Readability:"));
    m_countFlesch = new QLabel;
    m_fleschLabel->setToolTip(i18n("Flesch reading ease"));

    m_cjkcharsLabel = new QLabel(i18n("East asian characters:"));
    m_countCjkchars = new QLabel;

    if (!shortVersion) {
        m_preferencesButton = new QToolButton;
    }
}

void KWStatisticsWidget::initLayout()
{
    m_mainBox = new QBoxLayout(QBoxLayout::LeftToRight, this);

    m_wordsLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_wordsLayout);
    m_wordsLayout->addWidget(m_wordsLabel);
    m_wordsLayout->addWidget(m_countWords);

    m_sentencesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_sentencesLayout);
    m_sentencesLayout->addWidget(m_sentencesLabel);
    m_sentencesLayout->addWidget(m_countSentences);

    m_syllablesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_syllablesLayout);
    m_syllablesLayout->addWidget(m_syllablesLabel);
    m_syllablesLayout->addWidget(m_countSyllables);

    m_cjkcharsLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_cjkcharsLayout);
    m_cjkcharsLayout->addWidget(m_cjkcharsLabel);
    m_cjkcharsLayout->addWidget(m_countCjkchars);

    m_spacesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_spacesLayout);
    m_spacesLayout->addWidget(m_spacesLabel);
    m_spacesLayout->addWidget(m_countSpaces);

    m_nospacesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_nospacesLayout);
    m_nospacesLayout->addWidget(m_nospacesLabel);
    m_nospacesLayout->addWidget(m_countNospaces);

    m_fleschLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_fleschLayout);
    m_fleschLayout->addWidget(m_fleschLabel);
    m_fleschLayout->addWidget(m_countFlesch);

    m_linesLayout = new QHBoxLayout();
    m_mainBox->addLayout(m_linesLayout);
    m_linesLayout->addWidget(m_linesLabel);
    m_linesLayout->addWidget(m_countLines);

    if (!shortVersion) {
        // The button that opens the preferences dialog.
        m_mainBox->addWidget(m_preferencesButton);
    }

    setLayout(m_mainBox); // FIXME: Is this necessary?
}

void KWStatisticsWidget::setLayoutDirection(KWStatisticsWidget::LayoutDirection direction)
{
    if (direction == KWStatisticsWidget::LayoutHorizontal) {
        m_mainBox->setDirection(QBoxLayout::LeftToRight);
    } else {
        m_mainBox->setDirection(QBoxLayout::TopToBottom);
    }
}

void KWStatisticsWidget::setCanvas(KWCanvas *canvas)
{
    if (!canvas) {
        return;
    }
    if (m_document)
        disconnect(m_document->statistics(), &KWDocumentStatistics::refreshed, this, &KWStatisticsWidget::updateDataUi);
    m_document = canvas->document();
    connect(m_document->statistics(), &KWDocumentStatistics::refreshed, this, &KWStatisticsWidget::updateDataUi);
}

void KWStatisticsWidget::unsetCanvas()
{
    m_document = nullptr;
}

void KWStatisticsWidget::updateDataUi()
{
    QLocale locale;
    const auto stats = m_document->statistics();
    QString flesch = locale.toString(stats->fleschScore(), 'f', 2);
    QString newText[8];
    newText[0] = locale.toString(stats->words());
    m_countWords->setText(newText[0]);

    newText[1] = locale.toString(stats->sentences());
    m_countSentences->setText(newText[1]);

    newText[2] = locale.toString(stats->syllables());
    m_countSyllables->setText(newText[2]);

    newText[3] = locale.toString(stats->lines());
    m_countLines->setText(newText[3]);

    newText[4] = locale.toString(stats->charsWithSpace());
    m_countSpaces->setText(newText[4]);

    newText[5] = locale.toString(stats->charsWithoutSpace());
    m_countNospaces->setText(newText[5]);

    newText[6] = locale.toString(stats->cjkChars());
    m_countCjkchars->setText(newText[6]);

    newText[7] = flesch;
    m_countFlesch->setText(newText[7]);
}

// ----------------------------------------------------------------

void KWStatisticsWidget::wordsDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_wordsLabel->show();
        m_countWords->show();
        cfgGroup.writeEntry("WordsVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_wordsLabel->hide();
        m_countWords->hide();
        cfgGroup.writeEntry("WordsVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::sentencesDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_sentencesLabel->show();
        m_countSentences->show();
        cfgGroup.writeEntry("SentencesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_sentencesLabel->hide();
        m_countSentences->hide();
        cfgGroup.writeEntry("SentencesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::linesDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_linesLabel->show();
        m_countLines->show();
        cfgGroup.writeEntry("LinesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_linesLabel->hide();
        m_countLines->hide();
        cfgGroup.writeEntry("LinesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::syllablesDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_syllablesLabel->show();
        m_countSyllables->show();
        cfgGroup.writeEntry("SyllablesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_syllablesLabel->hide();
        m_countSyllables->hide();
        cfgGroup.writeEntry("SyllablesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::charspaceDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_spacesLabel->show();
        m_countSpaces->show();
        cfgGroup.writeEntry("CharspacesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_spacesLabel->hide();
        m_countSpaces->hide();
        cfgGroup.writeEntry("CharspacesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::charnospaceDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_nospacesLabel->show();
        m_countNospaces->show();
        cfgGroup.writeEntry("CharnospacesVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_nospacesLabel->hide();
        m_countNospaces->hide();
        cfgGroup.writeEntry("CharnospacesVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::eastDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_cjkcharsLabel->show();
        m_countCjkchars->show();
        cfgGroup.writeEntry("EastAsianCharactersVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_cjkcharsLabel->hide();
        m_countCjkchars->hide();
        cfgGroup.writeEntry("EastAsianCharactersVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}

void KWStatisticsWidget::fleschDisplayChanged(int state)
{
    KConfigGroup cfgGroup = KSharedConfig::openConfig()->group("Statistics");
    switch (state) {
    case Qt::Checked:
        m_fleschLabel->show();
        m_countFlesch->show();
        cfgGroup.writeEntry("FleschVisible", true);
        cfgGroup.sync();
        break;
    case Qt::Unchecked:
        m_fleschLabel->hide();
        m_countFlesch->hide();
        cfgGroup.writeEntry("FleschVisible", false);
        cfgGroup.sync();
        break;
    default:
        break;
    }
}
