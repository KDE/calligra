/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2009-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BgSpellCheck.h"
#include "SpellCheck.h"
#include "SpellCheckDebug.h"

#include <KoCharacterStyle.h>

#include <QCoreApplication>
#include <QTextBlock>
#include <QTextDocument>

#define MaxCharsPerRun 1000

BgSpellCheck::BgSpellCheck(const Speller &speller, QObject *parent)
    : BackgroundChecker(speller, parent)
{
    connect(this, &Sonnet::BackgroundChecker::misspelling, this, &BgSpellCheck::foundMisspelling);
    QString lang = speller.language();
    if (lang.isEmpty()) // have *some* default...
        lang = "en_US";
    setDefaultLanguage(lang);
}

BgSpellCheck::BgSpellCheck(QObject *parent)
    : BackgroundChecker(parent)
{
}

void BgSpellCheck::setDefaultLanguage(const QString &language)
{
    m_defaultCountry = "";
    m_defaultLanguage = language;
    int index = m_defaultLanguage.indexOf('_');
    if (index > 0) {
        m_defaultCountry = m_defaultLanguage.mid(index + 1);
        m_defaultLanguage = m_defaultLanguage.left(index);
    }
}

void BgSpellCheck::startRun(QTextDocument *document, int startPosition, int endPosition)
{
    m_document = document;
    m_currentPosition = startPosition;
    m_nextPosition = startPosition;
    m_endPosition = endPosition;
    if (m_currentLanguage != m_defaultLanguage || m_currentCountry != m_defaultCountry) {
        m_currentCountry = m_defaultCountry;
        m_currentLanguage = m_defaultLanguage;
        if (m_currentCountry.isEmpty()) {
            changeLanguage(m_currentLanguage);
        } else {
            changeLanguage(m_currentLanguage + '_' + m_currentCountry);
        }
    }
    if (m_currentPosition < m_endPosition) {
        debugSpellCheck << "Starting:" << m_currentPosition << m_endPosition;
        start();
    } else {
        Q_EMIT done();
    }
}

QString BgSpellCheck::fetchMoreText()
{
    m_currentPosition = m_nextPosition;
    if (m_currentPosition >= m_endPosition)
        return QString();

    QTextBlock block = m_document->findBlock(m_currentPosition);
    QTextBlock::iterator iter;
    while (true) {
        if (!block.isValid()) {
            m_nextPosition = m_endPosition; // ends run
            return QString();
        }
        if (m_currentPosition >= block.position() + block.length() - 1) { // only linefeed or empty block left
            block = block.next();
            m_currentPosition++;
            continue;
        }

        iter = block.begin();
        while (!iter.atEnd() && iter.fragment().position() + iter.fragment().length() <= m_currentPosition) {
            ++iter;
        }

        break;
    }

    int end = m_endPosition;
    QTextCharFormat cf = iter.fragment().charFormat();
    QString language;
    if (cf.hasProperty(KoCharacterStyle::Language))
        language = cf.property(KoCharacterStyle::Language).toString();
    else
        language = m_defaultLanguage;
    QString country;
    if (cf.hasProperty(KoCharacterStyle::Country))
        country = cf.property(KoCharacterStyle::Country).toString();
    else
        country = m_defaultCountry;

    // debugSpellCheck << "init" << language << country << "/" << iter.fragment().position();
    while (true) {
        end = iter.fragment().position() + iter.fragment().length();
        // debugSpellCheck << " + " << iter.fragment().position() << "-" << iter.fragment().position() + iter.fragment().length()
        // << block.text().mid(iter.fragment().position() - block.position(), iter.fragment().length());
        if (end >= qMin(m_endPosition, m_currentPosition + MaxCharsPerRun)) {
            break;
        }
        if (!iter.atEnd())
            ++iter;
        if (iter.atEnd()) { // end of block.
            m_nextPosition = block.position() + block.length();
            end = m_nextPosition - 1;
            break;
        }
        Q_ASSERT(iter.fragment().isValid());
        // debugSpellCheck << "Checking for viability forwarding to " << iter.fragment().position();
        cf = iter.fragment().charFormat();
        // debugSpellCheck << " new fragment language;"
        // << (cf.hasProperty(KoCharacterStyle::Language) ?  cf.property(KoCharacterStyle::Language).toString() : "unset");

        if ((cf.hasProperty(KoCharacterStyle::Language) && language != cf.property(KoCharacterStyle::Language).toString())
            || (!cf.hasProperty(KoCharacterStyle::Language) && language != m_defaultLanguage)) {
            break;
        }

        if ((cf.hasProperty(KoCharacterStyle::Country) && country != cf.property(KoCharacterStyle::Country).toString())
            || (!cf.hasProperty(KoCharacterStyle::Country) && country != m_defaultCountry)) {
            break;
        }
    }

    if (m_currentLanguage != language || m_currentCountry != country) {
        debugSpellCheck << "switching to language" << language << country;
        m_currentLanguage = language;
        m_currentCountry = country;
#if 0
     Disabling this as sonnet crashes on this. See https://bugs.kde.org/228271
        if (m_currentCountry.isEmpty()) {
            changeLanguage(m_currentLanguage);
        } else {
            changeLanguage(m_currentLanguage+'_'+m_currentCountry);
        }

#endif
    }

    QTextCursor cursor(m_document);
    cursor.setPosition(end);
    cursor.setPosition(m_currentPosition, QTextCursor::KeepAnchor);
    if (m_nextPosition < end)
        m_nextPosition = end;
    return cursor.selectedText();
}

void BgSpellCheck::foundMisspelling(const QString &word, int start)
{
    // debugSpellCheck << "Misspelling: " << word << " : " << start;
    Q_EMIT misspelledWord(word, m_currentPosition + start, true);
    BackgroundChecker::continueChecking();
}

QString BgSpellCheck::currentLanguage() const
{
    return m_currentLanguage;
}

QString BgSpellCheck::currentCountry() const
{
    return m_currentCountry;
}
