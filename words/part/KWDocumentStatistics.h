/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2021 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWDOCUMENTSTATISTICS_H
#define KWDOCUMENTSTATISTICS_H

#include "words_export.h"

#include <QObject>

#include <memory>

class QTimer;
class QTextDocument;
class KWDocument;

class KWDocumentStatisticsPrivate;

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
    ~KWDocumentStatistics();

    void reset();

    float fleschScore() const;

    int charsWithSpace() const;
    int charsWithoutSpace() const;
    int words() const;
    int sentences() const;
    int lines() const;
    int syllables() const;
    int paragraphs() const;
    int cjkChars() const;

protected:
    void connectNotify(const QMetaMethod &signal) override;

private Q_SLOTS:
    void updateData();

Q_SIGNALS:
    void refreshed();

private:
    void computeStatistics(const QTextDocument &doc);
    int countCJKChars(const QString &text);
    std::unique_ptr<KWDocumentStatisticsPrivate> d;

    Q_DISABLE_COPY(KWDocumentStatistics);
};

#endif
