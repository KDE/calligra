#ifndef TESTDOCUMENTAYOUT_H
#define TESTDOCUMENTAYOUT_H

#include <QObject>
#include <qtest_kde.h>

#include <KoTextShapeData.h>
#include <KoShapeContainer.h>

class QPainter;
class KoViewConverter;
class KoStyleManager;
class KWTextFrameSet;
class MockTextShape;
class QTextDocument;
class QTextLayout;
class KWTextDocumentLayout;

class TestDocumentLayout : public QObject
{
    Q_OBJECT
public:
    TestDocumentLayout() {}

private slots:
    void initTestCase();
    void placeAnchoredFrame();
    void placeAnchoredFrame2_data();
    void placeAnchoredFrame2();
    void placeAnchoredFrame3();
    void insertPicture(QTextCursor &cursor, QPointF offSet, QSizeF size);
    void insertPicture(QTextCursor &cursor, QSizeF size);
    void testLine(int linenumber, QPointF position, qreal width);
    void initAdvancedRunAroundTest();
    void testAdvancedRunAround1();
    void testAdvancedRunAround2();
    void testAdvancedRunAround3();
    void testAdvancedRunAround4();
    void testAdvancedRunAround5();
    void testAdvancedRunAround6();
    void noRunAroundFrame();

private:
    void initForNewTest(const QString &initText = QString());

private:
    KWTextFrameSet *frameSet;
    MockTextShape *shape1;
    QTextDocument *doc;
    KWTextDocumentLayout *layout;
    QTextLayout *blockLayout;
    QString loremIpsum;
    KoStyleManager *styleManager;
};

class MockTextShape : public KoShapeContainer
{
public:
    MockTextShape() {
        setUserData(new KoTextShapeData());
    }
    virtual void paintComponent(QPainter &, const KoViewConverter &) { }
    virtual void saveOdf(KoShapeSavingContext &) const {}
    virtual bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) {
        return false;
    }
};

#endif
