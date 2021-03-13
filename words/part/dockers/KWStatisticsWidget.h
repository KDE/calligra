/* This file is part of the KDE project
 * Copyright (C) 2005, 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
 * Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>
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

#ifndef KWSTATISTICSWIDGET_H
#define KWSTATISTICSWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>

// FIXME: Don't cross include
//#include "KWStatisticsDocker.h"
#include "StatisticsPreferencesPopup.h"
#include <KWCanvas.h>

class QTimer;
class QTextDocument;
class QToolButton;
class KoCanvasResourceManager;
class KoSelection;
class KWDocument;
class StatisticsPreferencesPopup;

struct KWDocumentStatistics {
    int charsWithSpace;
    int charsWithoutSpace;
    int words;
    int sentences;
    int lines;
    int syllables;
    int paragraphs;
    int cjkChars;
    
    KWDocumentStatistics() {
        reset();
    }

    void reset() {
        charsWithSpace = 0;
        charsWithoutSpace = 0;
        words = 0;
        sentences = 0;
        lines = 0;
        syllables = 0;
        paragraphs = 0;
        cjkChars = 0;
    }
    
    float fleschScore() const {
        // calculate Flesch reading ease score
        if ((syllables == 0) || (words == 0))
            return 0;
        return 206.835 - (1.015 * (words / sentences)) - (84.6 * syllables / words);
    }
};

/** KWStatisticswidget shows text statistics about a text document.
 *
 * In addition to being a widget, it also contains the statistics data
 * itself and functions to update this data.
 *
 * FIXME: The pure statistics part should be separated from the
 *        widget, e.g. to a QAbstractListModel so that it could also
 *        be used from a QML based interface.
 * FIXME: stop exporting this object once the statistics are separated
 */ 

class WORDS_TEST_EXPORT KWStatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KWStatisticsWidget(QWidget *parent = 0, bool shortVersion = false);
    ~KWStatisticsWidget() override;

    enum LayoutDirection {
        LayoutVertical,
        LayoutHorizontal
    };

    void setLayoutDirection(LayoutDirection direction);

    friend class KWStatisticsDocker;
    friend class StatisticsPreferencesPopup;

    void setCanvas(KWCanvas* canvas);

    void unsetCanvas();
    
    static void computeStatistics(const QTextDocument &doc, KWDocumentStatistics &stats);
    
public Q_SLOTS:
    void wordsDisplayChanged(int);
    void sentencesDisplayChanged(int);
    void linesDisplayChanged(int);
    void syllablesDisplayChanged(int);
    void charspaceDisplayChanged(int);
    void charnospaceDisplayChanged(int);
    void eastDisplayChanged(int);
    void fleschDisplayChanged(int);

    void updateData();
    void selectionChanged();

private:
    void initUi();
    void initLayout();

    void updateDataUi();
    static int countCJKChars(const QString &text);

private:
    //to know if this instance is a short version or a full one
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
    //QLabel *m_paragraphsLabel;

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

    KoCanvasResourceManager *m_resourceManager;
    KoSelection *m_selection;
    KWDocument *m_document;
    QTextDocument *m_textDocument;
    QTimer *m_timer;

    QToolButton *m_preferencesButton;
    StatisticsPreferencesPopup *m_menu;

    // The actual data.
    KWDocumentStatistics m_stats;

    // to ensure we're not running over ourselves.
    bool m_running;
};

#endif // KWSTATISTICSWIDGET_H
