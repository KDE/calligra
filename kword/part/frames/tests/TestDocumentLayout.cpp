#include "TestDocumentLayout.h"

#include <KWTextFrame.h>
#include <KWTextFrameSet.h>
#include <KWTextDocumentLayout.h>

#include <KoParagraphStyle.h>
#include <KoListStyle.h>
#include <KoTextBlockData.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>
#include <MockShapes.h>
#include <KoTextAnchor.h>
#include <KoInlineTextObjectManager.h>

#include <kdebug.h>
#include <kcomponentdata.h>

#define ROUNDING 0.126
#define FRAME_SPACING 10.0

class MockLayoutState : public KoTextDocumentLayout::LayoutState
{
public:
    MockLayoutState(QTextDocument *doc) :m_doc(doc) {}
    bool start() {
        m_currentBlock = m_doc->begin();
        layout = m_currentBlock.layout();
        layout->beginLayout();
        m_y = 0;
        return true;
    }
    void end() {}
    void reset() {}
    bool interrupted() {
        return false;
    }
    int numColumns() {
        return 0;
    }
    qreal width() {
        return 200;
    }
    qreal x() {
        return 0;
    }
    qreal y() {
        return m_y;
    }
    qreal docOffsetInShape() const {
        return 0;
    }
    bool addLine(QTextLine &) {
        m_y+=14.4;
        return false;
    }
    bool nextParag() {
        if (layout)
            layout->endLayout();
        m_currentBlock = m_currentBlock.next();
        if (! m_currentBlock.isValid())
            return false;
        layout = m_currentBlock.layout();
        layout->beginLayout();
        return true;
    }
    bool previousParag() {
        if (layout)
            layout->endLayout();
        m_currentBlock = m_currentBlock.previous();
        if (! m_currentBlock.isValid())
            return false;
        layout = m_currentBlock.layout();
        layout->beginLayout();
        return true;
    }
    qreal documentOffsetInShape() {
        return 0;
    }
    void draw(QPainter *, const KoTextDocumentLayout::PaintContext &) {}

    bool setFollowupShape(KoShape *) {
        return false;
    }
    void clearTillEnd() {}
    int cursorPosition() const {
        return m_currentBlock.position();
    }
    void registerInlineObject(const QTextInlineObject &) {}
    QTextTableCell hitTestTable(QTextTable *, const QPointF &) {
        return QTextTableCell();
    }

    QTextDocument *m_doc;
    QTextBlock m_currentBlock;
    qreal m_y;
};

void TestDocumentLayout::initTestCase()
{
    frameSet = 0;
    shape1 = 0;
    doc = 0;
    layout = 0;
    blockLayout = 0;

    loremIpsum = QString("Lorem ipsum dolor sit amet, XgXgectetuer adiXiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.");
}

void TestDocumentLayout::initForNewTest(const QString &initText)
{
    // this leaks memory like mad, but who cares ;)
    frameSet = new KWTextFrameSet(0);
    shape1 = new MockTextShape();
    shape1->setSize(QSizeF(200, 1000));
    new KWTextFrame(shape1, frameSet);
    doc = frameSet->document();
    Q_ASSERT(doc);
    layout = dynamic_cast<KWTextDocumentLayout*>(doc->documentLayout());
    Q_ASSERT(layout);
    styleManager = new KoStyleManager();
    KoTextDocument(doc).setStyleManager(styleManager);

    QTextBlock block = doc->begin();
    if (initText.length() > 0) {
        QTextCursor cursor(doc);
        cursor.insertText(initText);
        KoParagraphStyle style;
        style.setStyleId(101); // needed to do manually since we don't use the stylemanager
        QTextBlock b2 = doc->begin();
        while (b2.isValid()) {
            style.applyStyle(b2);
            b2 = b2.next();
        }
    }
    blockLayout = block.layout();
}

void TestDocumentLayout::placeAnchoredFrame()
{
    initForNewTest(QString());
    MockShape *picture = new MockShape();
    KoTextAnchor *anchor = new KoTextAnchor(picture);
    anchor->setOffset(QPointF(23, 45));
    QTextCursor cursor(doc);

    KoInlineTextObjectManager *manager = new KoInlineTextObjectManager();
    layout->setInlineTextObjectManager(manager);
    MockLayoutState *state = new MockLayoutState(doc);
    layout->setLayout(state);
    state->shape = shape1;
    QCOMPARE(doc->begin().text().length(), 0);
    manager->insertInlineObject(cursor, anchor);
    QCOMPARE(doc->begin().text().length(), 1);
    QCOMPARE(cursor.position(), 1);

    shape1->setPosition(QPointF(300, 300));
    layout->layout();
    QCOMPARE(picture->parent(), shape1);
    QCOMPARE(picture->position(), QPointF(23, 45));

    cursor.setPosition(0);
    cursor.insertText("foo");
    layout->layout();
    QCOMPARE(picture->parent(), shape1);
    QPointF newPos = picture->position();
    QVERIFY(newPos.x() > 23.);
    QVERIFY(newPos.y() > 45.); // it adds the baseline now

    cursor.movePosition(QTextCursor::End);
    cursor.insertText("\nNew Line\nAnd another");

    layout->layout();
    QCOMPARE(newPos, picture->position());
}

QTEST_KDEMAIN(TestDocumentLayout, GUI)

#include "TestDocumentLayout.moc"
