#include <qtest_kde.h>

#include <QDebug>

#include "../dialogs/StylesModel.h"
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>

class TestStylesModel : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void init();
    void cleanup();

private Q_SLOTS:
    void testPrecalcCache();
    void testSetManager();

private:
    void fillManager();
    KoStyleManager *manager;
};

class MockModel : public StylesModel
{
public:
    MockModel(KoStyleManager *manager, QObject *parent = nullptr)
        : StylesModel(manager, parent)
    {
    }

    QList<int> rootStyleIds()
    {
        return m_styleList;
    }
};

void TestStylesModel::init()
{
    manager = new KoStyleManager();
}

void TestStylesModel::cleanup()
{
    delete manager;
}

void TestStylesModel::testPrecalcCache()
{
    fillManager();
    MockModel model(manager);
    QCOMPARE(model.rootStyleIds().count(), 15);

    KoParagraphStyle *s = manager->paragraphStyle(model.rootStyleIds().at(0));
    QVERIFY(s);
    // QCOMPARE(s->name(), QString("Default"));
    KoParagraphStyle *code = manager->paragraphStyle(model.rootStyleIds().at(0));
    QVERIFY(code);
    QCOMPARE(code->name(), QString("code"));
    KoParagraphStyle *altered = manager->paragraphStyle(model.rootStyleIds().at(1));
    QVERIFY(altered);
    QCOMPARE(altered->name(), QString("altered"));
    KoParagraphStyle *headers = manager->paragraphStyle(model.rootStyleIds().at(3));
    QVERIFY(headers);
    QCOMPARE(headers->name(), QString("headers"));

    KoCharacterStyle *red = manager->characterStyle(model.rootStyleIds().at(6));
    QVERIFY(red);
    QCOMPARE(red->name(), QString("red"));
}

void TestStylesModel::testSetManager()
{
    MockModel model(0);
    QCOMPARE(model.rootStyleIds().count(), 0);
    fillManager();
    model.setStyleManager(manager);
    QCOMPARE(model.rootStyleIds().count(), 5);
}

void TestStylesModel::fillManager()
{
    KoParagraphStyle *ps = new KoParagraphStyle();
    ps->setName("code");
    manager->add(ps);
    ps = new KoParagraphStyle();
    ps->setName("altered");
    manager->add(ps);

    ps = new KoParagraphStyle();
    ps->setName("headers");
    KoParagraphStyle *head = new KoParagraphStyle();
    head->setParentStyle(ps);
    head->setName("Head 1");
    manager->add(head);
    head = new KoParagraphStyle();
    head->setParentStyle(ps);
    head->setName("Head 2");
    manager->add(head);
    manager->add(ps);
    head = new KoParagraphStyle();
    head->setParentStyle(ps);
    head->setName("Head 3");
    manager->add(head);

    KoCharacterStyle *style = new KoCharacterStyle();
    style->setName("red");
    manager->add(style);
}

QTEST_KDEMAIN(TestStylesModel, GUI)
