#include "TestRTree.h"

#include "engine/RTree.h"

#include <QSharedData>
#include <QTest>

using namespace Calligra::Sheets;

class TestClass : public QSharedData
{
public:
    TestClass()
        : member()
    {
    }
    TestClass(const QString &m)
        : member(m)
    {
    }
    virtual ~TestClass() = default;
    virtual int type() const
    {
        return 0;
    }
    bool operator<(const TestClass &other) const
    {
        return member < other.member;
    }
    bool operator==(const TestClass &other) const
    {
        return member == other.member;
    }
    QString member;
};

class SharedTestClass
{
public:
    SharedTestClass()
        : d(new TestClass())
    {
    }
    SharedTestClass(TestClass *subStyle)
        : d(subStyle)
    {
    }
    inline const TestClass *operator->() const
    {
        return d.data();
    }
    bool operator<(const SharedTestClass &o) const
    {
        return d->operator<(*o.d.data());
    }
    bool operator==(const SharedTestClass &o) const
    {
        return d->operator==(*o.d.data());
    }

private:
    QSharedDataPointer<TestClass> d;
};

class DerivedClass : public TestClass
{
public:
    DerivedClass()
        : TestClass()
    {
    }
    DerivedClass(const QString &s)
        : TestClass(s)
    {
    }
    int type() const override
    {
        return 1;
    }
};

void TestRTree::testIntersectingPairs()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(1, 1, 1, 1), new DerivedClass(QString("foo")));
    QList<QPair<QRectF, SharedTestClass>> pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QVERIFY(pairs.count() == 1);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 1, 1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[0].second->type(), 1);
}

void TestRTree::testSplit()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 4, 3), new DerivedClass(QString("foo")));
    tree.splitBeforeRow(3);
    QVERIFY(tree.validate());
    QList<QPair<QRectF, SharedTestClass>> pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QVERIFY(pairs.count() == 2);
    QCOMPARE(pairs[0].first, QRectF(2, 2, 4, 1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2, 3, 4, 2));
    QCOMPARE(pairs[1].second->member, QString("foo"));
    QList<SharedTestClass> data;
    data = tree.contains(QPoint(3, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(2, 4));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(2, 5));
    QCOMPARE(data.count(), 0);

    RTree<SharedTestClass> tree2;
    tree2.insert(QRect(4, 9, 2, 2), new DerivedClass(QString("bar")));
    tree2.splitBeforeColumn(5);
    QVERIFY(tree2.validate());
    pairs = tree2.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QVERIFY(pairs.count() == 2);
    QCOMPARE(pairs[0].first, QRectF(4, 9, 1, 2));
    QCOMPARE(pairs[0].second->member, QString("bar"));
    QCOMPARE(pairs[1].first, QRectF(5, 9, 1, 2));
    QCOMPARE(pairs[1].second->member, QString("bar"));
    data = tree2.contains(QPoint(5, 10));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree2.contains(QPoint(4, 9));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree2.contains(QPoint(4, 10));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree2.contains(QPoint(4, 11));
    QCOMPARE(data.count(), 0);
}

void TestRTree::testInsertShiftRight()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 4, 3), new DerivedClass(QString("foo")));
    tree.insert(QRect(4, 9, 2, 2), new DerivedClass(QString("bar")));
    auto undo = tree.insertShiftRight(QRect(4, 3, 3, 8));
    QVERIFY(tree.validate());
    QCOMPARE(undo.count(), 0);
    QList<SharedTestClass> data;

    QList<QPair<QRectF, SharedTestClass>> pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();

    // row 3 is moved
    data = tree.contains(QPoint(3, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(7, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(8, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(9, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(6, 3));
    QCOMPARE(data.count(), 0);

    // row 2 remains unchanged
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(6, 2));
    QCOMPARE(data.count(), 0);

    // row 9 moved
    data = tree.contains(QPoint(7, 9));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(7, 9));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(9, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(6, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 9));
    QCOMPARE(data.count(), 0);
}

void TestRTree::testInsertShiftDown()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 4, 3), new DerivedClass(QString("foo")));
    tree.insert(QRect(4, 9, 2, 2), new DerivedClass(QString("bar")));
    auto undo = tree.insertShiftDown(QRect(4, 3, 1, 6));
    QVERIFY(tree.validate());
    QCOMPARE(undo.count(), 0);
    QList<SharedTestClass> data;

    // column 4 is moved
    data = tree.contains(QPoint(4, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 9));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 10));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 15));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(4, 16));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(4, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 4));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 8));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 11));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 14));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 17));
    QCOMPARE(data.count(), 0);

    // column 3 remains unchanged
    data = tree.contains(QPoint(3, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 4));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 15));
    QCOMPARE(data.count(), 0);

    // so is column 5
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(5, 9));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(5, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 15));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 8));
    QCOMPARE(data.count(), 0);
}

void TestRTree::testRemoveShiftLeft()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 4, 3), new DerivedClass(QString("foo")));
    tree.insert(QRect(4, 9, 2, 2), new DerivedClass(QString("bar")));
    tree.insert(QRect(7, 3, 2, 2), new DerivedClass(QString("third")));
    tree.removeShiftLeft(QRect(4, 3, 4, 7));
    QVERIFY(tree.validate());
    QList<SharedTestClass> data;

    QList<QPair<QRectF, SharedTestClass>> pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();

    // row 3 is moved
    data = tree.contains(QPoint(2, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("third"));
    data = tree.contains(QPoint(9, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(7, 3));
    QCOMPARE(data.count(), 0);

    // row 2 remains unchanged
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(6, 2));
    QCOMPARE(data.count(), 0);

    // row 9 moved
    data = tree.contains(QPoint(3, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(6, 9));
    QCOMPARE(data.count(), 0);

    // and row 10 unchanged
    data = tree.contains(QPoint(4, 10));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(5, 10));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(3, 10));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(6, 10));
    QCOMPARE(data.count(), 0);
}

void TestRTree::testRemoveShiftUp()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 4, 3), new DerivedClass(QString("foo")));
    tree.insert(QRect(4, 9, 2, 2), new DerivedClass(QString("bar")));
    tree.removeShiftUp(QRect(4, 3, 1, 6));
    QVERIFY(tree.validate());
    QList<SharedTestClass> data;

    // column 4 is moved
    data = tree.contains(QPoint(4, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(4, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(4, 4));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(4, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 10));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 11));
    QCOMPARE(data.count(), 0);

    // column 3 remains unchanged
    data = tree.contains(QPoint(3, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 4));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(3, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 9));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 15));
    QCOMPARE(data.count(), 0);

    // so is column 5
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("foo"));
    data = tree.contains(QPoint(5, 9));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0]->member, QString("bar"));
    data = tree.contains(QPoint(5, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 15));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 8));
    QCOMPARE(data.count(), 0);
}

void TestRTree::testInsertColumns()
{
    RTree<QString> tree;
    tree.insert(QRect(1, 1, 2, 1), QString("1"));
    tree.insert(QRect(1, 2, 3, 1), QString("2"));
    tree.insert(QRect(2, 3, 4, 1), QString("3"));
    tree.insert(QRect(2, 4, 5, 1), QString("4"));
    tree.insert(QRect(3, 5, 3, 1), QString("5"));
    tree.insert(QRect(3, 6, 4, 1), QString("6"));
    tree.insert(QRect(4, 7, 2, 1), QString("7"));
    tree.insert(QRect(4, 8, 3, 1), QString("8"));
    tree.insert(QRect(6, 9, 3, 1), QString("9"));
    QList<QPair<QRectF, QString>> pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 9);
    QVector<QPair<QRectF, QString>> undo = tree.insertColumns(3, 3);
    QVERIFY(tree.validate());
    QCOMPARE(undo.count(), 0);

    pairs = tree.intersectingPairs(QRect(1, 1, 20, 20)).values();
    QVERIFY(pairs.count() >= 9); // at least 9 numbers; in reality more as most got split

    QList<QString> data;
    // row 1
    data = tree.contains(QPoint(2, 1));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("1"));
    data = tree.contains(QPoint(3, 1));
    QCOMPARE(data.count(), 0);

    // row 2
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("2"));
    data = tree.contains(QPoint(6, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("2"));
    data = tree.contains(QPoint(3, 2));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(7, 2));
    QCOMPARE(data.count(), 0);

    // row 5
    data = tree.contains(QPoint(6, 5));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("5"));
    data = tree.contains(QPoint(8, 5));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("5"));
    data = tree.contains(QPoint(2, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(9, 5));
    QCOMPARE(data.count(), 0);

    // row 7
    data = tree.contains(QPoint(7, 7));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("7"));
    data = tree.contains(QPoint(8, 7));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("7"));
    data = tree.contains(QPoint(3, 7));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 7));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 7));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(9, 7));
    QCOMPARE(data.count(), 0);

    // row 10
    pairs = tree.intersectingPairs(QRect(1, 10, 100, 1)).values();
    QCOMPARE(pairs.count(), 0);
}

void TestRTree::testInsertRows()
{
    RTree<QString> tree;
    tree.insert(QRect(1, 1, 1, 2), QString("1"));
    tree.insert(QRect(2, 1, 1, 3), QString("2"));
    tree.insert(QRect(3, 2, 1, 4), QString("3"));
    tree.insert(QRect(4, 2, 1, 5), QString("4"));
    tree.insert(QRect(5, 3, 1, 3), QString("5"));
    tree.insert(QRect(6, 3, 1, 4), QString("6"));
    tree.insert(QRect(7, 4, 1, 2), QString("7"));
    tree.insert(QRect(8, 4, 1, 3), QString("8"));
    tree.insert(QRect(9, 6, 1, 3), QString("9"));

    QList<QPair<QRectF, QString>> pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 9);
    QVector<QPair<QRectF, QString>> undo = tree.insertRows(3, 3);
    QVERIFY(tree.validate());
    QCOMPARE(undo.count(), 0);

    pairs = tree.intersectingPairs(QRect(1, 1, 20, 20)).values();
    QVERIFY(pairs.count() >= 9); // at least 9 numbers; in reality more as most got split

    QList<QString> data;
    // col 1
    data = tree.contains(QPoint(1, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("1"));
    data = tree.contains(QPoint(1, 3));
    QCOMPARE(data.count(), 0);

    // col 2
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("2"));
    data = tree.contains(QPoint(2, 6));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("2"));
    data = tree.contains(QPoint(2, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(2, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(2, 7));
    QCOMPARE(data.count(), 0);

    // col 5
    data = tree.contains(QPoint(5, 6));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("5"));
    data = tree.contains(QPoint(5, 8));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("5"));
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 9));
    QCOMPARE(data.count(), 0);

    // col 7
    data = tree.contains(QPoint(7, 7));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("7"));
    data = tree.contains(QPoint(7, 8));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("7"));
    data = tree.contains(QPoint(7, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(7, 4));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(7, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(7, 9));
    QCOMPARE(data.count(), 0);

    // col 10
    pairs = tree.intersectingPairs(QRect(10, 1, 1, 100)).values();
    QCOMPARE(pairs.count(), 0);
}

void TestRTree::testRemoveColumns()
{
    RTree<QString> tree;
    tree.insert(QRect(1, 1, 2, 1), QString("1"));
    tree.insert(QRect(1, 2, 3, 1), QString("2"));
    tree.insert(QRect(2, 3, 4, 1), QString("3"));
    tree.insert(QRect(2, 4, 5, 1), QString("4"));
    tree.insert(QRect(3, 5, 3, 1), QString("5"));
    tree.insert(QRect(3, 6, 4, 1), QString("6"));
    tree.insert(QRect(4, 7, 2, 1), QString("7"));
    tree.insert(QRect(4, 8, 3, 1), QString("8"));
    tree.insert(QRect(6, 9, 3, 1), QString("9"));
    QVector<QPair<QRectF, QString>> undo = tree.removeColumns(3, 3);
    QVERIFY(tree.validate());

    QList<QString> data;
    // row 1
    data = tree.contains(QPoint(2, 1));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("1"));
    data = tree.contains(QPoint(3, 1));
    QCOMPARE(data.count(), 0);

    // row 2
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("2"));
    data = tree.contains(QPoint(3, 2));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 0);

    // row 5
    data = tree.contains(QPoint(2, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(3, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(9, 5));
    QCOMPARE(data.count(), 0);
    auto pairs = tree.intersectingPairs(QRect(1, 5, 100, 1)).values();
    QCOMPARE(pairs.count(), 0);

    // row 4
    data = tree.contains(QPoint(2, 4));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("4"));
    data = tree.contains(QPoint(3, 4));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("4"));
    data = tree.contains(QPoint(4, 4));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(1, 4));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 4));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(9, 4));
    QCOMPARE(data.count(), 0);

    // row 10
    pairs = tree.intersectingPairs(QRect(1, 10, 100, 1)).values();
    QCOMPARE(pairs.count(), 0);

    QCOMPARE(undo.count(), 8);
}

void TestRTree::testRemoveRows()
{
    RTree<QString> tree;
    tree.insert(QRect(1, 1, 1, 2), QString("1"));
    tree.insert(QRect(2, 1, 1, 3), QString("2"));
    tree.insert(QRect(3, 2, 1, 4), QString("3"));
    tree.insert(QRect(4, 2, 1, 5), QString("4"));
    tree.insert(QRect(5, 3, 1, 3), QString("5"));
    tree.insert(QRect(6, 3, 1, 4), QString("6"));
    tree.insert(QRect(7, 4, 1, 2), QString("7"));
    tree.insert(QRect(8, 4, 1, 3), QString("8"));
    tree.insert(QRect(9, 6, 1, 3), QString("9"));
    QVector<QPair<QRectF, QString>> undo = tree.removeRows(3, 3);
    QVERIFY(tree.validate());

    QList<QString> data;
    // col 1
    data = tree.contains(QPoint(1, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("1"));
    data = tree.contains(QPoint(1, 3));
    QCOMPARE(data.count(), 0);

    // col 2
    data = tree.contains(QPoint(2, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("2"));
    data = tree.contains(QPoint(2, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(2, 5));
    QCOMPARE(data.count(), 0);

    // col 5
    data = tree.contains(QPoint(5, 2));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 3));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(5, 9));
    QCOMPARE(data.count(), 0);
    auto pairs = tree.intersectingPairs(QRect(5, 1, 1, 100)).values();
    QCOMPARE(pairs.count(), 0);

    // col 4
    data = tree.contains(QPoint(4, 2));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("4"));
    data = tree.contains(QPoint(4, 3));
    QCOMPARE(data.count(), 1);
    QCOMPARE(data[0], QString("4"));
    data = tree.contains(QPoint(4, 4));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 1));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 5));
    QCOMPARE(data.count(), 0);
    data = tree.contains(QPoint(4, 9));
    QCOMPARE(data.count(), 0);

    // col 10
    pairs = tree.intersectingPairs(QRect(10, 1, 1, 100)).values();
    QCOMPARE(pairs.count(), 0);

    QCOMPARE(undo.count(), 8);
}

void TestRTree::testPrimitive()
{
    RTree<bool> tree;
    tree.insert(QRect(2, 5, 1, 2), true);
    QCOMPARE(tree.contains(QPoint(2, 2)).isEmpty(), true);
    QCOMPARE(tree.contains(QPoint(2, 5)).first(), true);
    QCOMPARE(tree.contains(QPoint(3, 5)).isEmpty(), true);
    QCOMPARE(tree.contains(QPoint(2, 6)).first(), true);
    const QList<QPair<QRectF, bool>> pairs = tree.intersectingPairs(QRect(2, 5, 1, 2)).values();
    QCOMPARE(pairs.count(), 1);
    QCOMPARE(pairs.first().first.toRect(), QRect(2, 5, 1, 2));
    QCOMPARE(pairs.first().second, true);
}

QTEST_MAIN(TestRTree)
