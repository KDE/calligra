#ifndef TESTDOCUMENTAYOUT_H
#define TESTDOCUMENTAYOUT_H

#include <QObject>
#include <QtTest/QtTest>

#include <KoTextShapeData.h>
#include <KoShape.h>

class QPainter;
class KoViewConverter;


class TestDocumentLayout : public QObject {
    Q_OBJECT
public:
    TestDocumentLayout() {}

private slots:
    // tests
    void init();
    void testHitTest();

private:
    QApplication *m_app;
};

class MockTextShape : public KoShape {
  public:
    MockTextShape() {
        setUserData(new KoTextShapeData());
    }
    void paint(QPainter &painter, KoViewConverter &converter) {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
    }
};

#endif
