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

#include <kglobal.h>

// FIXME: Don't cross include
//#include "KWStatisticsDocker.h"
#include "StatisticsPreferencesPopup.h"

class QTimer;
class QTextDocument;
class QToolButton;
class KConfigGroup;
class KoCanvasResourceManager;
class KoSelection;
class KWDocument;
class StatisticsPreferencesPopup;

/** KWStatisticswidget shows text statistics about a text document.
 *
 * In addition to being a widget, it also contains the statistics data
 * itself and functions to update this data.
 *
 * FIXME: The pure statistics part should be separated from the
 *        widget, e.g. to a QAbstractListModel so that it could also
 *        be used from a QML based interface.
 */ 

class KWStatisticsWidget : public QWidget
{
    Q_OBJECT
public:
    KWStatisticsWidget(KoCanvasResourceManager *provider, KWDocument *m_document,
                       KoSelection *selection = 0, QWidget *parent = 0);

    virtual ~KWStatisticsWidget();
    void updateDataUi();
    void initUi();
    friend class KWStatisticsDocker;
    friend class StatisticsPreferencesPopup;
    
public slots:
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
    int countCJKChars(const QString &text);

    // Labels, e.g. "Words:"
    QLabel *m_wordsLabel;
    QLabel *m_sentencesLabel;
    QLabel *m_syllablesLabel;
    QLabel *m_spacesLabel;
    QLabel *m_fleschLabel;
    QLabel *m_cjkcharsLabel;
    QLabel *m_nospacesLabel;
    QLabel *m_linesLabel;

    // The values.
    QLabel *m_countWords;
    QLabel *m_countSentences;
    QLabel *m_countSyllables;
    QLabel *m_countSpaces;
    QLabel *m_countFlesch;
    QLabel *m_countCjkchars;
    QLabel *m_countNospaces;
    QLabel *m_countLines;

    KoCanvasResourceManager *m_resourceManager;
    KoSelection *m_selection;
    KWDocument *m_document;
    QTextDocument *m_textDocument;
    QTimer *m_timer;

    QToolButton *m_preferencesButton;
    StatisticsPreferencesPopup *m_menu;

    // The actual data.
    long m_charsWithSpace;
    long m_charsWithoutSpace;
    long m_words;
    long m_sentences;
    long m_lines;
    long m_syllables;
    long m_paragraphs;
    long m_cjkChars;
};

#endif // KWSTATISTICSWIDGET_H
