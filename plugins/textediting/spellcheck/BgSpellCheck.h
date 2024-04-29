/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BGSPELLCHECK_H
#define BGSPELLCHECK_H

#include <sonnet/backgroundchecker.h>
#include <sonnet/speller.h>

#include <QTextCursor>

using namespace Sonnet;

class QTextDocument;

class BgSpellCheck : public BackgroundChecker
{
    Q_OBJECT
public:
    explicit BgSpellCheck(const Speller &speller, QObject *parent = nullptr);
    explicit BgSpellCheck(QObject *parent = nullptr);
    void startRun(QTextDocument *document, int startPosition, int endPosition);
    QString currentLanguage() const;
    QString currentCountry() const;

protected:
    /// reimplemented
    QString fetchMoreText() override;

public Q_SLOTS:
    void setDefaultLanguage(const QString &language);

private Q_SLOTS:
    void foundMisspelling(const QString &word, int start);

Q_SIGNALS:
    void misspelledWord(const QString &word, int startPosition, bool misspelled);

private:
    QTextDocument *m_document;

    int m_currentPosition;
    int m_nextPosition; // the position we expect the next fetchMoreText to start from
    int m_endPosition;
    QString m_currentLanguage;
    QString m_currentCountry;
    QString m_defaultLanguage;
    QString m_defaultCountry;
};

#endif
