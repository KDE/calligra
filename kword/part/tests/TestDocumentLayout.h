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

#endif
