/* This file is part of the KDE project
 * Copyright (C) 2021 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#ifndef KWDOCUMENTSTATISTICS_H
#define KWDOCUMENTSTATISTICS_H

#include "words_export.h"

#include <QObject>

class QTimer;
class QTextDocument;
class KWDocument;

/**
 * This class stores and compute statistics about a KWDocument
 * text content.
 * The @refreshed() signal must be listened to for the statistics to be enabled.
 */
class WORDS_EXPORT KWDocumentStatistics : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int charsWithSpace READ charsWithSpace NOTIFY refreshed);
    Q_PROPERTY(int charsWithoutSpace READ charsWithoutSpace NOTIFY refreshed);
    Q_PROPERTY(int words READ words NOTIFY refreshed);
    Q_PROPERTY(int sentences READ sentences NOTIFY refreshed);
    Q_PROPERTY(int lines READ lines NOTIFY refreshed);
    Q_PROPERTY(int syllables READ syllables NOTIFY refreshed);
    Q_PROPERTY(int paragraphs READ paragraphs NOTIFY refreshed);
    Q_PROPERTY(int cjkChars READ cjkChars NOTIFY refreshed);
    Q_PROPERTY(float fleschScore READ fleschScore NOTIFY refreshed);

public:
    explicit KWDocumentStatistics(KWDocument *document);

    void reset();

    float fleschScore() const {
        // calculate Flesch reading ease score
        if ((m_sentences == 0) || (m_words == 0))
            return 0;
        return 206.835 - (1.015 * (m_words / m_sentences)) - (84.6 * m_syllables / m_words);
    }

    int charsWithSpace() const { return m_charsWithSpace; }
    int charsWithoutSpace() const { return m_charsWithoutSpace; }
    int words() const { return m_words; }
    int sentences() const { return m_sentences; }
    int lines() const { return m_lines; }
    int syllables() const { return m_syllables; }
    int paragraphs() const { return m_paragraphs; }
    int cjkChars() const { return m_cjkChars; }

protected:
    void connectNotify(const QMetaMethod &signal) override;

private Q_SLOTS:
    void updateData();

Q_SIGNALS:
    void refreshed();

private:
    void computeStatistics(const QTextDocument &doc);
    int countCJKChars(const QString &text);

    KWDocument *m_document;
    QTimer *m_timer;
    bool m_running;
    int m_charsWithSpace;
    int m_charsWithoutSpace;
    int m_words;
    int m_sentences;
    int m_lines;
    int m_syllables;
    int m_paragraphs;
    int m_cjkChars;
};

#endif
