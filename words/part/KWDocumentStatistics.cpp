/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2021 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWDocumentStatistics.h"
#include "KWDocument.h"

#include "KoTextDocumentLayout.h"
#include "frames/KWTextFrameSet.h"

#include <QDebug>
#include <QMetaMethod>
#include <QPointer>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocument>
#include <QTimer>

class KWDocumentStatisticsPrivate
{
public:
    KWDocument *document;
    QTimer *timer;
    bool running;
    int charsWithSpace;
    int charsWithoutSpace;
    int words;
    int sentences;
    int lines;
    int syllables;
    int paragraphs;
    int cjkChars;
};

KWDocumentStatistics::KWDocumentStatistics(KWDocument *doc)
    : QObject(doc)
    , d(new KWDocumentStatisticsPrivate())
{
    d->document = doc;
    d->running = false;
    d->timer = new QTimer(this);
    // Three seconds being the Human Moment, we're going to be slightly quicker than that
    // This means we're waiting long enough to let people start typing again, but not long
    // enough that they think something is wrong (which would happen if we waited longer)
    d->timer->setInterval(2500);
    d->timer->setSingleShot(true);
    connect(d->timer, &QTimer::timeout, this, &KWDocumentStatistics::updateData);

    auto newFrame = [this](KWFrameSet *fs) {
        KWTextFrameSet *tfs = qobject_cast<KWTextFrameSet *>(fs);
        if (tfs) {
            connect(tfs->document(), &QTextDocument::contentsChanged, d->timer, QOverload<>::of(&QTimer::start), Qt::UniqueConnection);
            KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(tfs->document()->documentLayout());
            if (lay) {
                connect(lay, &KoTextDocumentLayout::finishedLayout, d->timer, QOverload<>::of(&QTimer::start), Qt::UniqueConnection);
            }
        }
    };
    // We will have to connect to each frame set when it is created
    connect(d->document->frameLayout(), &KWFrameLayout::newFrameSet, this, newFrame);
    // This ensures that even if the document is entirely reset, we still pick up the main frame set
    connect(d->document, &KWDocument::pageSetupChanged, this, [this, newFrame]() {
        if (d->document->mainFrameSet()) {
            newFrame(d->document->mainFrameSet());
        }
    });

    // Initialize values
    reset();
}

KWDocumentStatistics::~KWDocumentStatistics() = default;

void KWDocumentStatistics::reset()
{
    d->charsWithSpace = 0;
    d->charsWithoutSpace = 0;
    d->words = 0;
    d->sentences = 0;
    d->lines = 0;
    d->syllables = 0;
    d->paragraphs = 0;
    d->cjkChars = 0;
}

float KWDocumentStatistics::fleschScore() const
{
    // calculate Flesch reading ease score
    if ((d->sentences == 0) || (d->words == 0))
        return 0;
    return 206.835 - (1.015 * (d->words / d->sentences)) - (84.6 * d->syllables / d->words);
}

int KWDocumentStatistics::charsWithSpace() const
{
    return d->charsWithSpace;
}

int KWDocumentStatistics::charsWithoutSpace() const
{
    return d->charsWithoutSpace;
}

int KWDocumentStatistics::words() const
{
    return d->words;
}

int KWDocumentStatistics::sentences() const
{
    return d->sentences;
}

int KWDocumentStatistics::lines() const
{
    return d->lines;
}

int KWDocumentStatistics::syllables() const
{
    return d->syllables;
}

int KWDocumentStatistics::paragraphs() const
{
    return d->paragraphs;
}

int KWDocumentStatistics::cjkChars() const
{
    return d->cjkChars;
}

void KWDocumentStatistics::connectNotify(const QMetaMethod &signal)
{
    // If someone connects to refreshed, better be sure we get the data.
    // Otherwise, in the following scenario, stats will be blank:
    // - fill-in document
    // - display statistics
    //   => this instantiates a statistics widget of some sort that listen to refreshed
    //   => no data ever come
    // We should probably add a fast-track refresh too
    if (signal == QMetaMethod::fromSignal(&KWDocumentStatistics::refreshed)) {
        d->timer->start();
    }
}

void KWDocumentStatistics::updateData()
{
    if (d->running) {
        return;
    }
    if (!isSignalConnected(QMetaMethod::fromSignal(&KWDocumentStatistics::refreshed))) {
        // rather than returning, schedule another update - this means we don't have to capture
        // the visible change signal and update then. This does mean we will have up to 2500 ms
        // before the update is done after showing the bar, but it's better than not updating
        // at all, and results in less code
        d->timer->start();
        return;
    }
    d->running = true;
    reset();

    foreach (KWFrameSet *fs, d->document->frameSets()) {
        QPointer<KWTextFrameSet> tfs = dynamic_cast<KWTextFrameSet *>(fs);
        if (!tfs)
            continue;

        QPointer<QTextDocument> doc = tfs->document();
        if (!doc)
            continue;
        computeStatistics(*doc);
    }
    Q_EMIT refreshed();
    d->running = false;
}

void KWDocumentStatistics::computeStatistics(const QTextDocument &doc)
{
    // parts of words for better counting of syllables:
    // (only use reg exp if necessary -> speed up)

    const QStringList subs_syl{QStringLiteral("cial"),
                               QStringLiteral("tia"),
                               QStringLiteral("cius"),
                               QStringLiteral("cious"),
                               QStringLiteral("giu"),
                               QStringLiteral("ion"),
                               QStringLiteral("iou")};

    static const QVector<QRegularExpression> subs_syl_regexp{QRegularExpression("sia$", QRegularExpression::CaseInsensitiveOption),
                                                             QRegularExpression("ely$", QRegularExpression::CaseInsensitiveOption)};

    const QStringList add_syl{QStringLiteral("ia"),
                              QStringLiteral("riet"),
                              QStringLiteral("dien"),
                              QStringLiteral("iu"),
                              QStringLiteral("io"),
                              QStringLiteral("ii")};
    static const QVector<QRegularExpression> add_syl_regexp{QRegularExpression("[aeiouym]bl$", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("[aeiou]{3}", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("^mc", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("ism$", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("[^l]lien", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("^coa[dglx].", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("[^gq]ua[^auieo]", QRegularExpression::CaseInsensitiveOption),
                                                            QRegularExpression("dnt$", QRegularExpression::CaseInsensitiveOption)};

    static QRegularExpression punctuation("[!?.,:_\"-]", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression final_e("e$", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression vowels("[^aeiouy]+", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression space("\\s+");
    static QRegularExpression multiplePunctuation("[.?!]+");
    static QRegularExpression floatingPoint("\\d\\.\\d");
    static QRegularExpression acronyms("[A-Z]\\.+");

    QTextBlock block = doc.begin();
    while (block.isValid()) {
        // Don't be so heavy on large documents...
        qApp->processEvents();
        d->paragraphs += 1;
        d->charsWithSpace += block.text().length();
        d->charsWithoutSpace += block.text().length() - block.text().count(QRegularExpression("\\s"));
        if (block.layout()) {
            d->lines += block.layout()->lineCount();
        }
        d->cjkChars += countCJKChars(block.text());

        QString s = block.text();

        // Syllable and Word count
        // Algorithm mostly taken from Greg Fast's Lingua::EN::Syllable module for Perl.
        // This guesses correct for 70-90% of English words, but the overall value
        // is quite good, as some words get a number that's too high and others get
        // one that's too low.
        // IMPORTANT: please test any changes against the unit test
        const QStringList wordlist = s.split(space, Qt::SkipEmptyParts);
        d->words += wordlist.count();
        for (QStringList::ConstIterator it1 = wordlist.begin(); it1 != wordlist.end(); ++it1) {
            QString word = *it1;
            word.remove(punctuation); // clean word from punctuation
            if (word.length() <= 3) { // extension to the original algorithm
                d->syllables++;
                continue;
            }
            word.remove(final_e);
            const QStringList syls = word.split(vowels, Qt::SkipEmptyParts);
            int word_syllables = 0;
            for (QStringList::ConstIterator it = subs_syl.begin(); it != subs_syl.end(); ++it) {
                if (word.indexOf(*it, 0, Qt::CaseInsensitive) != -1) {
                    word_syllables--;
                }
            }
            for (auto &regexp : subs_syl_regexp) {
                if (word.indexOf(regexp) != -1) {
                    word_syllables--;
                }
            }
            for (QStringList::ConstIterator it = add_syl.begin(); it != add_syl.end(); ++it) {
                if (word.indexOf(*it, 0, Qt::CaseInsensitive) != -1) {
                    word_syllables++;
                }
            }
            for (auto &regexp : add_syl_regexp) {
                if (word.indexOf(regexp) != -1) {
                    word_syllables++;
                }
            }
            word_syllables += syls.count();
            if (word_syllables == 0) {
                word_syllables = 1;
            }
            d->syllables += word_syllables;
        }

        block = block.next();

        // Sentence count
        // Clean up for better result, destroys the original text but we only want to count
        s = s.trimmed();
        if (s.isEmpty()) {
            continue;
        }
        QChar lastchar = s.at(s.length() - 1);
        if (!s.isEmpty() && lastchar != QChar('.') && lastchar != QChar('?') && lastchar != QChar('!')) { // e.g. for headlines
            s = s + '.';
        }
        s.replace(multiplePunctuation, "."); // count "..." as only one "."
        s.replace(floatingPoint, "0,0"); // don't count floating point numbers as sentences
        s.replace(acronyms, "*"); // don't count "U.S.A." as three sentences
        for (int i = 0; i < s.length(); ++i) {
            QChar ch = s[i];
            if (ch == QChar('.') || ch == QChar('?') || ch == QChar('!')) {
                ++d->sentences;
            }
        }
    }
}

int KWDocumentStatistics::countCJKChars(const QString &text)
{
    int count = 0;

    QString::const_iterator it;
    for (it = text.constBegin(); it != text.constEnd(); ++it) {
        QChar qChar = *it;
        /*
         * CJK punctuations: 0x3000 - 0x303F (but I believe we shouldn't include this in the statistics)
         * Hiragana: 0x3040 - 0x309F
         * Katakana: 0x30A0 - 0x30FF
         * CJK Unified Ideographs: 4E00 - 9FFF (Chinese Traditional & Simplified, Kanji and Hanja
         * Hangul: 0xAC00 - 0xD7AF
         */
        if ((qChar >= QChar(0x3040) && qChar <= QChar(0x309F)) || (qChar >= QChar(0x30A0) && qChar <= QChar(0x30FF))
            || (qChar >= QChar(0x4E00) && qChar <= QChar(0x9FFF)) || (qChar >= QChar(0xAC00) && qChar <= QChar(0xD7AF))) {
            count++;
        }
    }

    return count;
}
