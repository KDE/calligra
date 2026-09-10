/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *  SPDX-FileCopyrightText: 2009-2010 C. Boemann <cbo@kogmbh.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "TestDocumentLayout.h"
#include "MockRootAreaProvider.h"
#include <QTest>

#include <TextLayoutDebug.h>

#include <KoInlineTextObjectManager.h>
#include <KoShape.h>
#include <KoStyleManager.h>
#include <KoTextBlockBorderData.h>
#include <KoTextBlockData.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>

namespace
{
class InlineObjectTestLayout : public KoTextDocumentLayout
{
public:
    InlineObjectTestLayout(QTextDocument *document, KoTextLayoutRootAreaProvider *provider)
        : KoTextDocumentLayout(document, provider)
    {
    }

    int resizeCalls = 0;
    int paintCalls = 0;

protected:
    void positionInlineObject(QTextInlineObject, int, const QTextFormat &) override
    {
    }

    void resizeInlineObject(QTextInlineObject object, int, const QTextFormat &) override
    {
        ++resizeCalls;
        object.setWidth(70);
        object.setAscent(20);
        object.setDescent(0);
    }

    void drawInlineObject(QPainter *, const QRectF &, QTextInlineObject, int, const QTextFormat &) override
    {
        ++paintCalls;
    }
};
}

void TestDocumentLayout::testInlineObjectCallbacks()
{
    MockRootAreaProvider provider;
    QTextDocument document;
    auto *layout = new InlineObjectTestLayout(&document, &provider);
    document.setDocumentLayout(layout);

    QTextCursor cursor(&document);
    QTextCharFormat format;
    format.setObjectType(QTextFormat::UserObject + 1);
    cursor.insertText(QString(2, QChar::ObjectReplacementCharacter), format);

    QTextLayout *blockLayout = document.begin().layout();
    blockLayout->beginLayout();
    QTextLine line = blockLayout->createLine();
    QVERIFY(line.isValid());
    line.setLineWidth(300);
    blockLayout->endLayout();

    // Without a registered handler Qt lays these out as two ordinary glyphs,
    // leaving inline shapes hidden and skipping both layout and paint callbacks.
    QCOMPARE(layout->resizeCalls, 2);
    QCOMPARE(line.naturalTextWidth(), 140.0);

    QImage image(300, 50, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    line.draw(&painter, QPointF());
    QCOMPARE(layout->paintCalls, 2);
}

void TestDocumentLayout::initTestCase()
{
    m_doc = nullptr;
    m_layout = nullptr;
    m_styleManager = nullptr;
    m_provider = nullptr;
    m_paragraphStyle = nullptr;
}

void TestDocumentLayout::setupTest(const QString &initText)
{
    cleanupTest();
    m_doc = new QTextDocument;
    Q_ASSERT(m_doc);

    m_provider = new MockRootAreaProvider();
    Q_ASSERT(m_provider);
    KoTextDocument(m_doc).setInlineTextObjectManager(new KoInlineTextObjectManager(m_doc));

    m_doc->setDefaultFont(QFont("Sans Serif", 12, QFont::Normal, false)); // do it manually since we do not load the appDefaultStyle

    m_styleManager = new KoStyleManager(m_doc);
    KoTextDocument(m_doc).setStyleManager(m_styleManager);

    m_layout = new KoTextDocumentLayout(m_doc, m_provider);
    Q_ASSERT(m_layout);
    m_doc->setDocumentLayout(m_layout);

    if (!initText.isEmpty()) {
        QTextCursor cursor(m_doc);
        cursor.insertText(initText);
        m_paragraphStyle = new KoParagraphStyle;
        m_paragraphStyle->setStyleId(101); // needed to do manually since we don't use the stylemanager
        QTextBlock b2 = m_doc->begin();
        while (b2.isValid()) {
            m_paragraphStyle->applyStyle(b2);
            b2 = b2.next();
        }
    }
}

void TestDocumentLayout::cleanupTest()
{
    delete m_doc;
    m_doc = nullptr;
    m_layout = nullptr;
    delete m_provider;
    m_provider = nullptr;
    delete m_paragraphStyle;
    m_paragraphStyle = nullptr;
    m_styleManager = nullptr;
}

void TestDocumentLayout::cleanupTestCase()
{
    cleanupTest();
}

void TestDocumentLayout::testHitTest()
{
    // init a basic document with 3 parags.
    setupTest();
    m_doc->setHtml(
        "<p>lsdjflkdsjf lsdkjf lsdlflksejrl sdflsd flksjdf "
        "lksjrpdslfjfsdhtwkr[ivxxmvlwerponldsjf;dslflkjsorindfsn;epsdf</p><p>sldkfnwerpodsnf</p><p>sldkjfnpqwrdsf</p>");
    QTextBlock block = m_doc->begin();
    qreal offset = 50.0;
    qreal lineHeight = 0;
    int lines = 0, parag = 0;
    qreal paragOffets[3];
    while (1) {
        if (!block.isValid())
            break;
        paragOffets[parag++] = offset;
        QTextLayout *txtLayout = block.layout();
        txtLayout->beginLayout();
        while (1) {
            QTextLine line = txtLayout->createLine();
            if (!line.isValid())
                break;
            ++lines;
            line.setLineWidth(50);
            line.setPosition(QPointF(20, offset));
            offset += 20;
            lineHeight = line.height();
        }
        txtLayout->endLayout();
        block = block.next();
    }

    m_layout->layout();

    MockRootAreaProvider *p = dynamic_cast<MockRootAreaProvider *>(m_layout->provider());
    QVERIFY(p->area());

    QCOMPARE(lines, 8);

    /* Following is device-/font-dependent and therefore can be different on other computers
    // outside text
    QCOMPARE(p->area()->hitTest(QPointF(0, 0), Qt::FuzzyHit).position, 0);
    QCOMPARE(p->area()->hitTest(QPointF(0, 0), Qt::ExactHit).position, -1);
    QCOMPARE(p->area()->hitTest(QPointF(19, 49), Qt::ExactHit).position, 51);
    QCOMPARE(p->area()->hitTest(QPointF(71, 0), Qt::ExactHit).position, -1);
    QCOMPARE(p->area()->hitTest(QPointF(71, 51), Qt::ExactHit).position, 62);

    // first char
    QCOMPARE(p->area()->hitTest(QPointF(20, 51), Qt::ExactHit).position, 52);
    QCOMPARE(p->area()->hitTest(QPointF(20, 50), Qt::ExactHit).position, 52);

    // below line 1
    //QCOMPARE(p->area()->hitTest(QPointF(20, 51 + lineHeight), Qt::ExactHit).position, -1);
    //QVERIFY(p->area()->hitTest(QPointF(20, 51 + lineHeight), Qt::FuzzyHit).position > 0); // line 2

    // parag2
    QCOMPARE(p->area()->hitTest(QPointF(20, paragOffets[1]), Qt::ExactHit).position, 139);
    QCOMPARE(p->area()->hitTest(QPointF(20, paragOffets[1]), Qt::FuzzyHit).position, 139);
    QVERIFY(p->area()->hitTest(QPointF(20, paragOffets[1] + 20), Qt::FuzzyHit).position >= 139);
    */
    Q_UNUSED(lineHeight); // used in the above commented piece of code
    Q_UNUSED(paragOffets); // used in the above commented piece of code
}

void TestDocumentLayout::testRootAreaZeroWidth()
{
    setupTest("a");

    MockRootAreaProvider *provider = dynamic_cast<MockRootAreaProvider *>(m_layout->provider());
    provider->setSuggestedRect(QRectF(10., 10., 0., 200.));

    m_layout->layout();

    QVERIFY(!provider->m_askedForMoreThenOneArea);
    QVERIFY(provider->area());
    QVERIFY(!provider->area()->isDirty());
    QVERIFY(!provider->area()->virginPage());
    QVERIFY(provider->area()->nextStartOfArea());
    QVERIFY(!provider->area()->isStartingAt(provider->area()->nextStartOfArea()));
    QCOMPARE(provider->area()->boundingRect().topLeft(), QPointF(10., 10.));
    // QCOMPARE(provider->area()->boundingRect().height(), qreal(14.4));
    QCOMPARE(provider->area()->referenceRect().topLeft(), QPointF(10., 10.));
    // QCOMPARE(provider->area()->referenceRect().bottomLeft(), QPointF(0.,14.4));
}

void TestDocumentLayout::testRootAreaZeroHeight()
{
    setupTest("a");

    MockRootAreaProvider *provider = dynamic_cast<MockRootAreaProvider *>(m_layout->provider());
    provider->setSuggestedRect(QRectF(10., 10., 200., 0.));

    m_layout->layout();

    QVERIFY(!provider->m_askedForMoreThenOneArea); // we add the text anyways even if it does not match in height
    QVERIFY(provider->area());
    QVERIFY(!provider->area()->isDirty());
    QVERIFY(!provider->area()->virginPage()); // should not be virigin any longer cause we added text
    QVERIFY(provider->area()->nextStartOfArea());
    QVERIFY(!provider->area()->isStartingAt(provider->area()->nextStartOfArea())); // start- and end-iterator should not be equal cause we added text
    QCOMPARE(provider->area()->boundingRect(), QRectF(10., 10., 200., 0.));
    QCOMPARE(provider->area()->referenceRect(), QRectF(10., 10., 200., 0.));
}

void TestDocumentLayout::testRootAreaZeroWidthAndHeight()
{
    setupTest("a");

    MockRootAreaProvider *provider = dynamic_cast<MockRootAreaProvider *>(m_layout->provider());
    provider->setSuggestedRect(QRectF(10., 10., 0., 0.));

    m_layout->layout();

    QVERIFY(!provider->m_askedForMoreThenOneArea);
    QVERIFY(provider->area());
    QVERIFY(!provider->area()->isDirty());
    QVERIFY(!provider->area()->virginPage());
    QVERIFY(provider->area()->nextStartOfArea());
    QVERIFY(!provider->area()->isStartingAt(provider->area()->nextStartOfArea()));
    QCOMPARE(provider->area()->boundingRect().topLeft(), QPointF(10., 10.));
    QCOMPARE(provider->area()->boundingRect().height(), qreal(0.));
    // QCOMPARE(provider->area()->boundingRect().width(), qreal(6.67188));
    QCOMPARE(provider->area()->referenceRect(), QRectF(10., 10., 0., 0.));
}

QTEST_MAIN(TestDocumentLayout)
