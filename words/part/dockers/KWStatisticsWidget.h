/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWSTATISTICSWIDGET_H
#define KWSTATISTICSWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidget>

#include "StatisticsPreferencesPopup.h"
#include <KWCanvas.h>

class QTimer;
class QTextDocument;
class QToolButton;
class KoCanvasResourceManager;
class KoSelection;
class KWDocument;
class StatisticsPreferencesPopup;

/** KWStatisticswidget shows text statistics about a text document.
 */

class KWStatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KWStatisticsWidget(QWidget *parent = nullptr, bool shortVersion = false);
    ~KWStatisticsWidget() override;

    enum LayoutDirection { LayoutVertical, LayoutHorizontal };

    void setLayoutDirection(LayoutDirection direction);

    friend class KWStatisticsDocker;
    friend class StatisticsPreferencesPopup;

    void setCanvas(KWCanvas *canvas);

    void unsetCanvas();

public Q_SLOTS:
    void wordsDisplayChanged(int);
    void sentencesDisplayChanged(int);
    void linesDisplayChanged(int);
    void syllablesDisplayChanged(int);
    void charspaceDisplayChanged(int);
    void charnospaceDisplayChanged(int);
    void eastDisplayChanged(int);
    void fleschDisplayChanged(int);

private:
    void initUi();
    void initLayout();

    void updateDataUi();

private:
    // to know if this instance is a short version or a full one
    bool shortVersion;

    // Labels, e.g. "Words:"
    QLabel *m_wordsLabel;
    QLabel *m_sentencesLabel;
    QLabel *m_syllablesLabel;
    QLabel *m_cjkcharsLabel;
    QLabel *m_spacesLabel;
    QLabel *m_nospacesLabel;
    QLabel *m_fleschLabel;
    QLabel *m_linesLabel;
    // QLabel *m_paragraphsLabel;

    // The values.
    QLabel *m_countWords;
    QLabel *m_countSentences;
    QLabel *m_countSyllables;
    QLabel *m_countCjkchars;
    QLabel *m_countSpaces;
    QLabel *m_countNospaces;
    QLabel *m_countFlesch;
    QLabel *m_countLines;

    // The main layout
    QBoxLayout *m_mainBox;

    // The layouts for the label/value QLabel pairs.
    QHBoxLayout *m_wordsLayout;
    QHBoxLayout *m_sentencesLayout;
    QHBoxLayout *m_syllablesLayout;
    QHBoxLayout *m_cjkcharsLayout;
    QHBoxLayout *m_spacesLayout;
    QHBoxLayout *m_nospacesLayout;
    QHBoxLayout *m_fleschLayout;
    QHBoxLayout *m_linesLayout;

    KWDocument *m_document;

    QToolButton *m_preferencesButton;
    StatisticsPreferencesPopup *m_menu;
};

#endif // KWSTATISTICSWIDGET_H
