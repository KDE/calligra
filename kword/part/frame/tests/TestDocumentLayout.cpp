#include "TestDocumentLayout.h"

#include <KWTextFrame.h>
#include <KWTextFrameSet.h>
#include <KWTextDocumentLayout.h>

#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>
#include <QApplication>

#include <kdebug.h>
#include <kinstance.h>


void TestDocumentLayout::init() {
    //  To get around this error
    // ASSERT failure in QFontDatabase: "A QApplication object needs to be constructed before FontConfig is used.", file text/qfontdatabase_x11.cpp, line 942
    QCoreApplication::instance()->exit();
    delete QCoreApplication::instance();
    QApplication::setQuitOnLastWindowClosed(false);
    m_app = new QApplication(0, 0, false);
}

void TestDocumentLayout:: testHitTest() {
    // init a basic document with 3 parags.
    KWTextFrameSet frameSet;
    KWTextFrame frame(new MockTextShape(), &frameSet);
    KWTextDocumentLayout layout(&frameSet);
    QTextDocument *doc = layout.document();
    doc->setDocumentLayout(&layout);
    doc->setHtml("<p>lsdjflkdsjf lsdkjf lsdlflksejrl sdflsd flksjdf lksjrpdslfjfsdhtwkr[ivxxmvlwerponldsjf;dslflkjsorindfsn;epsdf</p><p>sldkfnwerpodsnf</p><p>sldkjfnpqwrdsf</p>");
    QTextBlock block = doc->begin();
    double offset=50.0;
    double lineHeight = 0;
    int lines = 0, parag=0;
    double paragOffets[3];
    while(1) {
        if(!block.isValid()) break;
        paragOffets[parag++] = offset;
        QTextLayout *txtLayout = block.layout();
        txtLayout->beginLayout();
        while(1) {
            QTextLine line = txtLayout->createLine();
            if (!line.isValid()) break;
            lines++;
            line.setLineWidth(50);
            line.setPosition(QPointF(20, offset));
            offset += 20;
            lineHeight = line.height();
        }
        txtLayout->endLayout();
        block = block.next();
    }
    QCOMPARE(lines, 8);

    // outside text
    QCOMPARE(layout.hitTest(QPointF(0, 0), Qt::FuzzyHit), 0);
    QCOMPARE(layout.hitTest(QPointF(0, 0), Qt::ExactHit), -1);
    QCOMPARE(layout.hitTest(QPointF(19, 49), Qt::ExactHit), -1);
    QCOMPARE(layout.hitTest(QPointF(71, 0), Qt::ExactHit), -1);
    QCOMPARE(layout.hitTest(QPointF(71, 51), Qt::ExactHit), -1);

    // first char
    QCOMPARE(layout.hitTest(QPointF(20, 51), Qt::ExactHit), 0);
    QCOMPARE(layout.hitTest(QPointF(20, 50), Qt::ExactHit), 0);

    // below line 1
    QCOMPARE(layout.hitTest(QPointF(20, 51+lineHeight), Qt::ExactHit), -1);
    QVERIFY(layout.hitTest(QPointF(20, 51+lineHeight), Qt::FuzzyHit) > 0); // line 2

    // parag2
    QCOMPARE(layout.hitTest(QPointF(20, paragOffets[1]), Qt::ExactHit), 109);
    QCOMPARE(layout.hitTest(QPointF(20, paragOffets[1]), Qt::FuzzyHit), 109);
    QVERIFY(layout.hitTest(QPointF(20, paragOffets[1] + 20), Qt::FuzzyHit) > 109);
}

QTEST_MAIN(TestDocumentLayout)
#include "TestDocumentLayout.moc"
