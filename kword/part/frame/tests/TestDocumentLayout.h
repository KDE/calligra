#ifndef TESTDOCUMENTAYOUT_H
#define TESTDOCUMENTAYOUT_H

#include <QObject>
#include <qtest_kde.h>

#include <KoTextShapeData.h>
#include <KoShape.h>

class QPainter;
class KoViewConverter;
class KoStyleManager;
class KWTextFrameSet;
class MockTextShape;
class QTextDocument;
class QTextLayout;
class KWTextDocumentLayout;

class TestDocumentLayout : public QObject {
    Q_OBJECT
public:
    TestDocumentLayout() {}

private slots:
    void initTestCase();

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

class MockTextShape : public KoShape {
  public:
    MockTextShape() {
        setUserData(new KoTextShapeData());
    }
    void paint(QPainter &painter, const KoViewConverter &converter) {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
    }
};

#endif
