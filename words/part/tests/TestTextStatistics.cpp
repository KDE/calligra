/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2021 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TestTextStatistics.h"

#include <KWDocumentStatistics.h>

#include "frames/KWTextFrameSet.h"
#include <KWCanvas.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWPageManager.h>
#include <MockShapes.h>

#include "MockPart.h"

#include <QApplication>
#include <QSignalSpy>
#include <QTextDocument>
#include <QThread>

#include <QtTest>

void TestTextStatistics::testTextStatistics()
{
    KWDocument doc(new MockPart);
    QSignalSpy spy(doc.statistics(), &KWDocumentStatistics::refreshed);

    doc.initEmpty();
    doc.mainFrameSet()->document()->setHtml(
        "<html><body><p>Test cases for Words statistics</p>"
        "<p>This document can be used to check Words Dockers->Statistics function. It contains about 159 syllables, 108 words, and 14 sentences. The Flesch "
        "reading ease score is &quot;approximately 75.2&quot;. If you make changes to this document, please adjust the numbers above.</p>"
        "<p>The U.S.A. is a big country, and this is just one sentence, despite the dots.</p>"
        "<p>#</p>"
        "<p>The headline gets counted as a sentence, too, even if it does not end with a dot.</p>"
        "<p>This is exactly one sentence!!!! And this is another one?!?!?! This is finally the last one...</p>"
        "<p>Floating point numbers: 6.025, 5.96 </p>"
        "<p>One sentence. Another sentence.</p>"
        "<p>Second column has a sentence, too.</p>"
        "</body></html>");

    qApp->processEvents();
    // There is a 2.5s timer before stats are refreshed
    QThread::sleep(3);
    qApp->processEvents();
    QCOMPARE(spy.count(), 1);
    const auto stats = doc.statistics();

    QCOMPARE(stats->words(), 109);
    QCOMPARE(stats->sentences(), 15);
    QCOMPARE(stats->syllables(), 160);
    QCOMPARE(stats->cjkChars(), 0);
    QCOMPARE(stats->charsWithoutSpace(), 542);
    QCOMPARE(stats->charsWithSpace(), 643);
    QCOMPARE(qRound(stats->fleschScore() * 100), 7555);
}

void TestTextStatistics::testListenBehaviour()
{
    KWDocument doc(new MockPart);

    doc.initEmpty();
    doc.mainFrameSet()->document()->setHtml("<html><body><p>Hello world</p></body></html>");

    qApp->processEvents();
    const auto stats = doc.statistics();

    // Stats should be empty, nobody is listening
    QCOMPARE(stats->lines(), 0);
    QThread::sleep(3);
    QCOMPARE(stats->lines(), 0);

    // Now, we will listen
    QSignalSpy spy(doc.statistics(), &KWDocumentStatistics::refreshed);
    QCOMPARE(stats->lines(), 0);
    QThread::sleep(3);
    // There is a 2.5s timer before stats are refreshed
    qApp->processEvents();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(stats->lines(), 1);
}

QTEST_MAIN(TestTextStatistics)
