/* This file is part of the KDE project
 * Copyright (C) 2005, 2007 Thomas Zander <zander@kde.org>
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

#ifndef KWSTATISTICS_H
#define KWSTATISTICS_H


#include <ui_KWStatistics.h>
#include <ui_KWStatisticsDocker.h>
#include "dockers/StatisticsPreferencesPopup.h"
#include <ui_StatisticsPreferencesPopup.h>
#include <QToolButton>
#include <QWidget>
#include <QDialog>
#include <QtGui/QVBoxLayout>
#include <QCheckBox>
#include <QMenu>

class QTimer;
class QTextDocument;
class KoCanvasResourceManager;
class KoSelection;
class KWDocument;
class StatisticsPreferencesPopup;

class KWStatistics : public QWidget
{
    Q_OBJECT
public:
    KWStatistics(KoCanvasResourceManager *provider, KWDocument *m_document,
                 KoSelection *selection = 0, QWidget *parent = 0);

    void updateDataUi();

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
    void setAutoUpdate(int);
    void selectionChanged();
    void on_refresh();

private:
    int countCJKChars(const QString &text);
    Ui::KWStatisticsDocker widgetDocker;

    KoCanvasResourceManager *m_resourceManager;
    KoSelection *m_selection;
    KWDocument *m_document;
    QTextDocument *m_textDocument;
    QTimer *m_timer;
    StatisticsPreferencesPopup *m_menu;

    long m_charsWithSpace;
    long m_charsWithoutSpace;
    long m_words;
    long m_sentences;
    long m_lines;
    long m_syllables;
    long m_paragraphs;
    long m_cjkChars;
    bool m_autoUpdate;
    bool m_showInDocker;
};

#endif
