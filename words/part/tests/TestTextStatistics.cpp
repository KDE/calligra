#include "TestTextStatistics.h"

#include <dockers/KWStatisticsWidget.h>
#include <QTextDocument>

#include <QtTest>

void TestTextStatistics::testTextStatistics()
{
    KWDocumentStatistics stats;
    QTextDocument doc;
    doc.setHtml("<html><body><p>Test cases for Words statistics</p></body></html>");
    KWStatisticsWidget::computeStatistics(doc, stats);
    QCOMPARE(stats.words, 5);
    
    doc.setHtml("<html><body><p>Test cases for Words statistics</p>"
                "<p>This document can be used to check Words Dockers->Statistics function. It contains about 159 syllables, 108 words, and 14 sentences. The Flesch reading ease score is &quot;approximately 75.2&quot;. If you make changes to this document, please adjust the numbers above.</p>"
                "<p>The U.S.A. is a big country, and this is just one sentence, despite the dots.</p>"
                "<p>#</p>"
                "<p>The headline gets counted as a sentence, too, even if it does not end with a dot.</p>"
                "<p>This is exactly one sentence!!!! And this is another one?!?!?! This is finally the last one...</p>"
                "<p>Floating point numbers: 6.025, 5.96 </p>"
                "<p>One sentence. Another sentence.</p>"
                "<p>Second column has a sentence, too.</p>"
                "</body></html>");
    stats.reset();
    KWStatisticsWidget::computeStatistics(doc, stats);
    QCOMPARE(stats.words, 109);
    QCOMPARE(stats.sentences, 15);
    QCOMPARE(stats.syllables, 160);
    QCOMPARE(stats.cjkChars, 0);
    QCOMPARE(stats.charsWithoutSpace, 542);
    QCOMPARE(stats.charsWithSpace, 643);
    QCOMPARE(qRound(stats.fleschScore() * 100), 7555);
    // NB : lines statistics can not be computed without a layout
}

QTEST_MAIN(TestTextStatistics)
