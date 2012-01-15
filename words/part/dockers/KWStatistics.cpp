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

#include "KWStatistics.h"

#include "Words.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include <ui_KWStatisticsDocker.h>
#include "dockers/StatisticsPreferencesPopup.h"
#include <KoCanvasResourceManager.h>
#include <KoSelection.h>
#include <KoShape.h>

#include <QTextLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <QTimer>

KWStatistics::KWStatistics(KoCanvasResourceManager *provider, KWDocument *document, KoSelection *selection, QWidget *parent)
        : QWidget(parent),
        m_resourceManager(provider),
        m_selection(selection),
        m_document(document),
        m_textDocument(0),
        m_timer(0),
        m_charsWithSpace(0),
        m_charsWithoutSpace(0),
        m_words(0),
        m_sentences(0),
        m_lines(0),
        m_syllables(0),
        m_paragraphs(0),
        m_autoUpdate(true)
{
    m_showInDocker = true;
    m_autoUpdate = false;
    m_timer = new QTimer(this);
    m_timer->start(2500);
    widgetDocker.setupUi(this);
    m_menu = new StatisticsPreferencesPopup(widgetDocker.preferences);
    widgetDocker.preferences->setMenu(m_menu);
    widgetDocker.preferences->setPopupMode(QToolButton::InstantPopup);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));
    connect(widgetDocker.preferences, SIGNAL(clicked()), widgetDocker.preferences, SLOT(showMenu()));
    connect(m_menu, SIGNAL(wordsDisplayChange(int)), this, SLOT(wordsDisplayChanged(int)));
    connect(m_menu, SIGNAL(sentencesDisplayChange(int)), this, SLOT(sentencesDisplayChanged(int)));
    connect(m_menu, SIGNAL(linesDisplayChange(int)), this, SLOT(linesDisplayChanged(int)));
    connect(m_menu, SIGNAL(syllablesDisplayChange(int)), this, SLOT(syllablesDisplayChanged(int)));
    connect(m_menu, SIGNAL(charspaceDisplayChange(int)), this, SLOT(charspaceDisplayChanged(int)));
    connect(m_menu, SIGNAL(charnospaceDisplayChange(int)), this, SLOT(charnospaceDisplayChanged(int)));
    connect(m_menu, SIGNAL(eastDisplayChange(int)), this, SLOT(eastDisplayChanged(int)));
    connect(m_menu, SIGNAL(fleschDisplayChange(int)), this, SLOT(fleschDisplayChanged(int)));


}


void KWStatistics::updateData()
{
    m_charsWithSpace = 0;
    m_charsWithoutSpace = 0;
    m_words = 0;
    m_sentences = 0;
    m_lines = 0;
    m_syllables = 0;
    m_paragraphs = 0;
    m_cjkChars = 0;

    // parts of words for better counting of syllables:
    // (only use reg exp if necessary -> speed up)

    QStringList subs_syl;
    subs_syl << "cial" << "tia" << "cius" << "cious" << "giu" << "ion" << "iou";
    QStringList subs_syl_regexp;
    subs_syl_regexp << "sia$" << "ely$";

    QStringList add_syl;
    add_syl << "ia" << "riet" << "dien" << "iu" << "io" << "ii";
    QStringList add_syl_regexp;
    add_syl_regexp << "[aeiouym]bl$" << "[aeiou]{3}" << "^mc" << "ism$"
    << "[^l]lien" << "^coa[dglx]." << "[^gq]ua[^auieo]" << "dnt$";

    foreach (KWFrameSet *fs, m_document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0) continue;

        QTextDocument *doc = tfs->document();
        QTextBlock block = doc->begin();
        while (block.isValid()) {
            m_paragraphs += 1;
            m_charsWithSpace += block.text().length();
            m_charsWithoutSpace += block.text().length() - block.text().count(QRegExp("\\s"));
            if (block.layout())
                m_lines += block.layout()->lineCount();
            m_cjkChars += countCJKChars(block.text());

            QString s = block.text();

            // Syllable and Word count
            // Algorithm mostly taken from Greg Fast's Lingua::EN::Syllable module for Perl.
            // This guesses correct for 70-90% of English words, but the overall value
            // is quite good, as some words get a number that's too high and others get
            // one that's too low.
            // IMPORTANT: please test any changes against demos/statistics.kwd
            QRegExp re("\\s+");
            const QStringList wordlist = s.split(re, QString::SkipEmptyParts);
            m_words += wordlist.count();
            re.setCaseSensitivity(Qt::CaseInsensitive);
            for (QStringList::ConstIterator it1 = wordlist.begin(); it1 != wordlist.end(); ++it1) {
                QString word1 = *it1;
                QString word = *it1;
                re.setPattern("[!?.,:_\"-]");    // clean word from punctuation
                word.remove(re);
                if (word.length() <= 3) {  // extension to the original algorithm
                    m_syllables++;
                    continue;
                }
                re.setPattern("e$");
                word.remove(re);
                re.setPattern("[^aeiouy]+");
                QStringList syls = word.split(re, QString::SkipEmptyParts);
                int word_syllables = 0;
                for (QStringList::Iterator it = subs_syl.begin(); it != subs_syl.end(); ++it) {
                    if (word.indexOf(*it, 0, Qt::CaseInsensitive) != -1)
                        word_syllables--;
                }
                for (QStringList::Iterator it = subs_syl_regexp.begin(); it != subs_syl_regexp.end(); ++it) {
                    re.setPattern(*it);
                    if (word.indexOf(re) != -1)
                        word_syllables--;
                }
                for (QStringList::Iterator it = add_syl.begin(); it != add_syl.end(); ++it) {
                    if (word.indexOf(*it, 0, Qt::CaseInsensitive) != -1)
                        word_syllables++;
                }
                for (QStringList::Iterator it = add_syl_regexp.begin(); it != add_syl_regexp.end(); ++it) {
                    re.setPattern(*it);
                    if (word.indexOf(re) != -1)
                        word_syllables++;
                }
                word_syllables += syls.count();
                if (word_syllables == 0)
                    word_syllables = 1;
                m_syllables += word_syllables;
            }
            re.setCaseSensitivity(Qt::CaseSensitive);

            block = block.next();

            // Sentence count
            // Clean up for better result, destroys the original text but we only want to count
            s = s.trimmed();
            if (s.isEmpty())
                continue;
            QChar lastchar = s.at(s.length() - 1);
            if (! s.isEmpty() && lastchar != QChar('.') && lastchar != QChar('?') && lastchar != QChar('!')) {  // e.g. for headlines
                s = s + '.';
            }
            re.setPattern("[.?!]+");         // count "..." as only one "."
            s.replace(re, ".");
            re.setPattern("\\d\\.\\d");      // don't count floating point numbers as sentences
            s.replace(re, "0,0");
            re.setPattern("[A-Z]\\.+");      // don't count "U.S.A." as three sentences
            s.replace(re, "*");
            for (int i = 0 ; i < s.length(); ++i) {
                QChar ch = s[i];
                if (ch == QChar('.') || ch == QChar('?') || ch == QChar('!'))
                    ++m_sentences;
            }
        }
    }
updateDataUi();

}

void KWStatistics::updateDataUi()
{


    // calculate Flesch reading ease score:
    float flesch_score = 0;
    if (m_words > 0 && m_sentences > 0)
        flesch_score = 206.835 - (1.015 * (m_words / m_sentences)) - (84.6 * m_syllables / m_words);
    QString flesch = KGlobal::locale()->formatNumber(flesch_score);



         QString newText[8];
         newText[0] = KGlobal::locale()->formatNumber(m_words, 0);
         widgetDocker.count_words->setText(newText[0]);

         newText[1] = KGlobal::locale()->formatNumber(m_sentences, 0);
          widgetDocker.count_sentences->setText(newText[1]);

         newText[2] = KGlobal::locale()->formatNumber(m_syllables, 0);
         widgetDocker.count_syllables->setText(newText[2]);

         newText[3] = KGlobal::locale()->formatNumber(m_lines, 0);
         widgetDocker.count_lines->setText(newText[3]);

         newText[4] = KGlobal::locale()->formatNumber(m_charsWithSpace, 0);
         widgetDocker.count_spaces->setText(newText[4]);

         newText[5] = KGlobal::locale()->formatNumber(m_charsWithoutSpace, 0);
         widgetDocker.count_nospaces->setText(newText[5]);

         newText[6] = KGlobal::locale()->formatNumber(m_cjkChars, 0);
         widgetDocker.count_cjkchars->setText(newText[6]);

         newText[7] = flesch;
         widgetDocker.count_flesch->setText(newText[7]);


}

void KWStatistics::setAutoUpdate(int state)
{
    if (state == Qt::Checked) {
        m_autoUpdate = true;
        connect(m_textDocument, SIGNAL(contentsChanged()), m_timer, SLOT(start()));
        updateData();
    } else {
        m_autoUpdate = false;
        disconnect(m_textDocument, SIGNAL(contentsChanged()), m_timer, SLOT(start()));
    }

}

void KWStatistics::selectionChanged()
{
    if (m_selection->count() != 1)
        return;

    KoShape *shape = m_selection->firstSelectedShape();
    if (!shape) return;
    KWFrame *frame = dynamic_cast<KWFrame*>(shape->applicationData());
    if (!frame) return; // you can have embedded shapes selected, in that case it surely is no text frameset.
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
    if (fs) {
        if (m_textDocument && m_autoUpdate)
            disconnect(m_textDocument, SIGNAL(contentsChanged()), m_timer, SLOT(start()));
        m_textDocument = fs->document();
        if (m_autoUpdate)
            connect(m_textDocument, SIGNAL(contentsChanged()), m_timer, SLOT(start()));
    }
}

int KWStatistics::countCJKChars(const QString &text)
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
        if ((qChar >= 0x3040 && qChar <= 0x309F)
                || (qChar >= 0x30A0 && qChar <= 0x30FF)
                || (qChar >= 0x4E00 && qChar <= 0x9FFF)
                || (qChar >= 0xAC00 && qChar <= 0xD7AF)) {
            count++;
        }
    }

    return count;
}

void KWStatistics::wordsDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.Words->show();
        widgetDocker.count_words->show();
        break;
    case Qt::Unchecked:
        widgetDocker.Words->hide();
        widgetDocker.count_words->hide();
        break;
    default:
        break;
    }
}

void KWStatistics::sentencesDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.Sentences->show();
        widgetDocker.count_sentences->show();
        break;
    case Qt::Unchecked:
        widgetDocker.Sentences->hide();
        widgetDocker.count_sentences->hide();
        break;
    default:
        break;
    }
}

void KWStatistics::linesDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.Lines->show();
        widgetDocker.count_lines->show();
        break;
    case Qt::Unchecked:
        widgetDocker.Lines->hide();
        widgetDocker.count_lines->hide();
        break;
    default:
        break;
    }
}
void KWStatistics::syllablesDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.Syllables->show();
        widgetDocker.count_syllables->show();
        break;
    case Qt::Unchecked:
        widgetDocker.Syllables->hide();
        widgetDocker.count_syllables->hide();
        break;
    default:
        break;
    }
}
void KWStatistics::charspaceDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.spaces->show();
        widgetDocker.count_spaces->show();
        break;
    case Qt::Unchecked:
        widgetDocker.spaces->hide();
        widgetDocker.count_spaces->hide();
        break;
    default:
        break;
    }
}
void KWStatistics::charnospaceDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.nospaces->show();
        widgetDocker.count_nospaces->show();
        break;
    case Qt::Unchecked:
        widgetDocker.nospaces->hide();
        widgetDocker.count_nospaces->hide();
        break;
    default:
        break;
    }
}
void KWStatistics::eastDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.Cjkchars->show();
        widgetDocker.count_cjkchars->show();
        break;
    case Qt::Unchecked:
        widgetDocker.Cjkchars->hide();
        widgetDocker.count_cjkchars->hide();
        break;
    default:
        break;
    }
}
void KWStatistics::fleschDisplayChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        widgetDocker.Flesch->show();
        widgetDocker.count_flesch->show();
        break;
    case Qt::Unchecked:
        widgetDocker.Flesch->hide();
        widgetDocker.count_flesch->hide();
        break;
    default:
        break;
    }
}


