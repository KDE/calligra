#include "TestDocumentLayout.h"

#include <KWTextFrame.h>
#include <KWTextFrameSet.h>
#include <KWTextDocumentLayout.h>

#include <KoCharacterStyle.h>

#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>
#include <QApplication>
#include <QPainter>
#include <QMainWindow>

#include <kdebug.h>
#include <kinstance.h>


void TestDocumentLayout::initTestCase() {
    //  To get around this error
    // ASSERT failure in QFontDatabase: "A QApplication object needs to be constructed before FontConfig is used.", file text/qfontdatabase_x11.cpp, line 942
    QCoreApplication::instance()->exit();
    QCoreApplication::instance()->~QCoreApplication();
    QApplication::setQuitOnLastWindowClosed(false);
    m_app = new QApplication(0, 0, false);

    frameSet = 0;
    shape1 = 0;
    doc = 0;
    layout = 0;
    blockLayout = 0;

    loremIpsum = QString("Lorem ipsum dolor sit amet, XgXgectetuer adiXiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.");
}

void TestDocumentLayout::cleanupTestCase() {
    delete frameSet;
    frameSet = 0;
    shape1 = 0;
    doc = 0;
    layout = 0;
}

void TestDocumentLayout::initForNewTest(const QString &initText) {
    // all members are deleted as a result of this delete:
    delete frameSet;

    frameSet = new KWTextFrameSet();
    shape1 = new MockTextShape();
    shape1->resize(QSizeF(200, 1000));
    new KWTextFrame(shape1, frameSet);
    doc = frameSet->document();
    Q_ASSERT(doc);
    layout = dynamic_cast<KWTextDocumentLayout*> (doc->documentLayout());
    Q_ASSERT(layout);

    QTextBlock block = doc->begin();
    if(initText.length() > 0) {
        QTextCursor cursor(doc);
        cursor.insertText(initText);
        KoCharacterStyle style;
        style.applyStyle(block);
    }
    blockLayout = block.layout();
}

void TestDocumentLayout:: testHitTest() {
    // init a basic document with 3 parags.
    initForNewTest();
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
    QCOMPARE(lines, 7);

    // outside text
    QCOMPARE(layout->hitTest(QPointF(0, 0), Qt::FuzzyHit), 0);
    QCOMPARE(layout->hitTest(QPointF(0, 0), Qt::ExactHit), -1);
    QCOMPARE(layout->hitTest(QPointF(19, 49), Qt::ExactHit), -1);
    QCOMPARE(layout->hitTest(QPointF(71, 0), Qt::ExactHit), -1);
    QCOMPARE(layout->hitTest(QPointF(71, 51), Qt::ExactHit), -1);

    // first char
    QCOMPARE(layout->hitTest(QPointF(20, 51), Qt::ExactHit), 0);
    QCOMPARE(layout->hitTest(QPointF(20, 50), Qt::ExactHit), 0);

    // below line 1
    QCOMPARE(layout->hitTest(QPointF(20, 51+lineHeight), Qt::ExactHit), -1);
    QVERIFY(layout->hitTest(QPointF(20, 51+lineHeight), Qt::FuzzyHit) > 0); // line 2

    // parag2
    QCOMPARE(layout->hitTest(QPointF(20, paragOffets[1]), Qt::ExactHit), 109);
    QCOMPARE(layout->hitTest(QPointF(20, paragOffets[1]), Qt::FuzzyHit), 109);
    QVERIFY(layout->hitTest(QPointF(20, paragOffets[1] + 20), Qt::FuzzyHit) > 109);
}

void TestDocumentLayout::testLineBreaking() {
    initForNewTest(loremIpsum);
    layout->layout();

    QCOMPARE(blockLayout->lineCount(), 16);
    QCOMPARE(blockLayout->lineForTextPosition(1).width(), 200.0);
}

void TestDocumentLayout::testMultiFrameLineBreaking() {
    /// Test breaking lines for frames with different widths.
    // TODO
    initForNewTest();
}

void TestDocumentLayout::testBasicLineSpacing() {
    /// Tests incrementing Y pos based on the font size
    initForNewTest(loremIpsum);
    QTextCursor cursor (doc);
    cursor.setPosition(0);
    cursor.setPosition(loremIpsum.length()-1, QTextCursor::KeepAnchor);
    QTextCharFormat charFormat = cursor.charFormat();
    charFormat.setFontPointSize(12);
    cursor.mergeCharFormat(charFormat);
    layout->layout();

    const double fontHeight12 = 12;
    double lineSpacing12 = fontHeight12 * 1.2; // 120% is the normal lineSpacing.
    const double fontHeight18 = 18;
    double lineSpacing18 = fontHeight18 * 1.2; // 120% is the normal lineSpacing.

    QCOMPARE(blockLayout->lineCount(), 16);
    QCOMPARE(blockLayout->lineForTextPosition(1).width(), 200.0);
    QTextLine line;
    for(int i=0; i < 20; i++) {
        line = blockLayout->lineAt(i);
        QCOMPARE(line.y(), i * lineSpacing12);
    }

    // make first word smaller, should have zero effect on lineSpacing12.
    cursor.setPosition(0);
    cursor.setPosition(11, QTextCursor::KeepAnchor);
    charFormat.setFontPointSize(10);
    cursor.mergeCharFormat(charFormat);
    layout->layout();
    for(int i=0; i < 20; i++) {
        line = blockLayout->lineAt(i);
        QCOMPARE(line.y(), i * 21.0);
        QCOMPARE(line.y() + line.height(), (i+1) * lineSpacing12);
    }

    // make first word on second line word bigger, should move that line down a little.
    int pos = blockLayout->lineAt(1).textStart();
    cursor.setPosition(pos);
    cursor.setPosition(pos + 12, QTextCursor::KeepAnchor);
    charFormat.setFontPointSize(18);
    cursor.mergeCharFormat(charFormat);
    layout->layout();
    line = blockLayout->lineAt(0);
    QCOMPARE(line.y(), 0.0);
    line = blockLayout->lineAt(1);
    QCOMPARE(line.y(), lineSpacing12);
    QCOMPARE(line.height(), lineSpacing18);

    for(int i=2; i < 19; i++) {
        line = blockLayout->lineAt(i);
//qDebug() << "i: " << i << " gives: " << line.y() << " + " <<  line.ascent() << ", " << line.descent() << " = " << line.height();
        QCOMPARE(line.y(), lineSpacing12 + lineSpacing18 + (i-2) * lineSpacing12);
    }
// Test widget to show what we have
/*
    class Widget : public QWidget {
      public:
        Widget(KWTextDocumentLayout *layout) {
            m_layout = layout;
        }
        void paintEvent (QPaintEvent * e) {
            QPainter painter( this );
            QAbstractTextDocumentLayout::PaintContext pc;
            pc.cursorPosition = -1;
            m_layout->draw( &painter, pc);
        }
      private:
        KWTextDocumentLayout *m_layout;
    };

    QMainWindow mw;
    mw.setCentralWidget(new Widget(layout));
    mw.show();
    m_app->exec(); */
}

void TestDocumentLayout::testMargins() {
    initForNewTest(loremIpsum);
    QTextCursor cursor(doc);
    QTextBlockFormat bf = cursor.blockFormat();
    bf.setLeftMargin(10.0);
    cursor.setBlockFormat(bf);
    layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 10.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 190.0);

    bf.setRightMargin(15.0);
    cursor.setBlockFormat(bf);
    layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 10.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 175.0);

    bf.setLeftMargin(0.0);
    cursor.setBlockFormat(bf);
    layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 0.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 185.0); // still uses the right margin of 15

    cursor.setPosition(loremIpsum.length());
    cursor.insertText("\n");
    bf.setTopMargin(12);
    cursor.setBlockFormat(bf);
    cursor.insertText(loremIpsum);// create second parag
    layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 0.0); // parag 1
    QCOMPARE(blockLayout->lineAt(0).width(), 185.0);

    // and test parag 2
    QTextBlock block2 = doc->begin().next();
    QTextLayout *layout = block2.layout();
    QCOMPARE(layout->lineAt(0).x(), 0.0);
    QCOMPARE(layout->lineAt(0).width(), 185.0);

    QTextLine lastLineOfParag1 =  blockLayout->lineAt(blockLayout->lineCount()-1);
    QTextLine firstLineOfParag2 =  layout->lineAt(0);
    const double FONTSIZE = 12.0;
    QCOMPARE(lastLineOfParag1.y() + (FONTSIZE * 1.2) - firstLineOfParag2.y(), 12.0);
}

void TestDocumentLayout::testTextIndent() {
    // TODO
    initForNewTest();
}

void TestDocumentLayout::testTextAlignments() {
    // TODO
    initForNewTest();
}

void TestDocumentLayout::testLineSpacing() {
    /// Test fo:line-height, style:lineheight-at-least and style:line-spacing properties (15.5.1)
    // TODO
    initForNewTest();
}


QTEST_MAIN(TestDocumentLayout)
#include "TestDocumentLayout.moc"
