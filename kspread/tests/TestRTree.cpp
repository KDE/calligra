#include <QSharedData>

#include "RTree.h"

#include "TestRTree.h"

using namespace KSpread;

class TestClass : public QSharedData
{
public:
    TestClass() : member() {}
    TestClass(const QString& m) : member(m) {}
    virtual ~TestClass() {}
    virtual int type() const {
        return 0;
    }
    bool operator<(const TestClass& other) const {
        return member < other.member;
    }
    bool operator==(const TestClass& other) const {
        return member == other.member;
    }
    QString member;
};

class SharedTestClass
{
public:
    SharedTestClass() : d(new TestClass()) {}
    SharedTestClass(TestClass* subStyle) : d(subStyle) {}
    inline const TestClass *operator->() const {
        return d.data();
    }
    bool operator<(const SharedTestClass& o) const {
        return d->operator<(*o.d.data());
    }
    bool operator==(const SharedTestClass& o) const {
        return d->operator==(*o.d.data());
    }

private:
    QSharedDataPointer<TestClass> d;
};

class DerivedClass : public TestClass
{
public:
    DerivedClass() : TestClass() {}
    DerivedClass(const QString& s) : TestClass(s) {}
    virtual int type() const {
        return 1;
    }
};

void TestRTree::testIntersectingPairs()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(1, 1, 1, 1), new DerivedClass(QString("foo")));
    QList< QPair<QRectF, SharedTestClass> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QVERIFY(pairs.count() == 1);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 1, 1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[0].second->type(), 1);
}

void TestRTree::testInsertShiftRight()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 2, 1), new DerivedClass(QString("foo")));
    tree.insertShiftRight(QRect(2, 1, 3, 4));
    QList< QPair<QRectF, SharedTestClass> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 3);
    QCOMPARE(pairs[0].first, QRectF(2, 2, 2, 1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2, 1, KS_colMax - 1, 4));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(5, 2, 2, 1));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testInsertShiftDown()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 2, 1, 2), new DerivedClass(QString("foo")));
    tree.insertShiftDown(QRect(2, 1, 4, 3));
    QList< QPair<QRectF, SharedTestClass> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 3);
    QCOMPARE(pairs[0].first, QRectF(2, 2, 1, 2));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2, 1, 4, KS_rowMax));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(2, 5, 1, 2));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testRemoveShiftLeft()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(5, 2, 2, 1), new DerivedClass(QString("foo")));
    tree.removeShiftLeft(QRect(2, 1, 3, 4));
    QList< QPair<QRectF, SharedTestClass> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 3);
    QCOMPARE(pairs[0].first, QRectF(5, 2, 2, 1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2, 1, KS_colMax - 1, 4));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(2, 2, 2, 1));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testRemoveShiftUp()
{
    RTree<SharedTestClass> tree;
    tree.insert(QRect(2, 5, 1, 2), new DerivedClass(QString("foo")));
    tree.removeShiftUp(QRect(2, 1, 4, 3));
    QList< QPair<QRectF, SharedTestClass> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 3);
    QCOMPARE(pairs[0].first, QRectF(2, 5, 1, 2));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2, 1, 4, KS_rowMax));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(2, 2, 1, 2));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testInsertColumns()
{
    // RTree::InsertMode = RTree::CopyPrevious
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
    QList< QPair<QRectF, QString> > undo = tree.insertColumns(3, 3);
    QList< QPair<QRectF, QString> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 9);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 5, 1));
    QCOMPARE(pairs[0].second, QString("1"));
    QCOMPARE(pairs[1].first, QRectF(1, 2, 6, 1));
    QCOMPARE(pairs[1].second, QString("2"));
    QCOMPARE(pairs[2].first, QRectF(2, 3, 7, 1));
    QCOMPARE(pairs[2].second, QString("3"));
    QCOMPARE(pairs[3].first, QRectF(2, 4, 8, 1));
    QCOMPARE(pairs[3].second, QString("4"));
    QCOMPARE(pairs[4].first, QRectF(6, 5, 3, 1));
    QCOMPARE(pairs[4].second, QString("5"));
    QCOMPARE(pairs[5].first, QRectF(6, 6, 4, 1));
    QCOMPARE(pairs[5].second, QString("6"));
    QCOMPARE(pairs[6].first, QRectF(7, 7, 2, 1));
    QCOMPARE(pairs[6].second, QString("7"));
    QCOMPARE(pairs[7].first, QRectF(7, 8, 3, 1));
    QCOMPARE(pairs[7].second, QString("8"));
    QCOMPARE(pairs[8].first, QRectF(9, 9, 3, 1));
    QCOMPARE(pairs[8].second, QString("9"));
    QCOMPARE(undo.count(), 0);
#if 0
    // RTree::InsertMode = RTree::CopyCurrent
    tree.clear();
    tree.insert(QRect(1, 1, 2, 1), QString("1"));
    tree.insert(QRect(1, 2, 3, 1), QString("2"));
    undo = tree.insertColumns(3, 3, RTree<QString>::CopyCurrent);
    pairs = tree.intersectingPairs(QRect(1, 1, 10, 10));
    QCOMPARE(pairs.count(), 2);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 2, 1));
    QCOMPARE(pairs[1].first, QRectF(1, 2, 6, 1));
    QCOMPARE(undo.count(), 0);

    // RTree::InsertMode = RTree::CopyNone
    tree.clear();
    tree.insert(QRect(1, 1, 2, 1), QString("1"));
    tree.insert(QRect(1, 2, 3, 1), QString("2"));
    undo = tree.insertColumns(3, 3, RTree<QString>::CopyNone);
    pairs = tree.intersectingPairs(QRect(1, 1, 10, 10));
    QCOMPARE(pairs.count(), 3);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 2, 1));
    QCOMPARE(pairs[1].first, QRectF(1, 2, 2, 1));
    QCOMPARE(pairs[2].first, QRectF(6, 2, 1, 1));
    QCOMPARE(undo.count(), 0);
#endif
}

void TestRTree::testInsertRows()
{
    // RTree::InsertMode = RTree::CopyPrevious
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
    QList< QPair<QRectF, QString> > undo = tree.insertRows(3, 3);
    QList< QPair<QRectF, QString> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 9);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 1, 5));
    QCOMPARE(pairs[0].second, QString("1"));
    QCOMPARE(pairs[1].first, QRectF(2, 1, 1, 6));
    QCOMPARE(pairs[1].second, QString("2"));
    QCOMPARE(pairs[2].first, QRectF(3, 2, 1, 7));
    QCOMPARE(pairs[2].second, QString("3"));
    QCOMPARE(pairs[3].first, QRectF(4, 2, 1, 8));
    QCOMPARE(pairs[3].second, QString("4"));
    QCOMPARE(pairs[4].first, QRectF(5, 6, 1, 3));
    QCOMPARE(pairs[4].second, QString("5"));
    QCOMPARE(pairs[5].first, QRectF(6, 6, 1, 4));
    QCOMPARE(pairs[5].second, QString("6"));
    QCOMPARE(pairs[6].first, QRectF(7, 7, 1, 2));
    QCOMPARE(pairs[6].second, QString("7"));
    QCOMPARE(pairs[7].first, QRectF(8, 7, 1, 3));
    QCOMPARE(pairs[7].second, QString("8"));
    QCOMPARE(pairs[8].first, QRectF(9, 9, 1, 3));
    QCOMPARE(pairs[8].second, QString("9"));
    QCOMPARE(undo.count(), 0);
#if 0
    // RTree::InsertMode = RTree::CopyCurrent
    tree.clear();
    tree.insert(QRect(1, 1, 1, 2), QString("1"));
    tree.insert(QRect(2, 1, 1, 3), QString("2"));
    undo = tree.insertColumns(3, 3, RTree<QString>::CopyCurrent);
    pairs = tree.intersectingPairs(QRect(1, 1, 10, 10));
    QCOMPARE(pairs.count(), 2);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 1, 2));
    QCOMPARE(pairs[1].first, QRectF(2, 1, 1, 6));
    QCOMPARE(undo.count(), 0);
#endif
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
    QList< QPair<QRectF, QString> > undo = tree.removeColumns(3, 3);
    QList< QPair<QRectF, QString> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 7);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 2, 1));
    QCOMPARE(pairs[0].second, QString("1"));
    QCOMPARE(pairs[1].first, QRectF(1, 2, 2, 1));
    QCOMPARE(pairs[1].second, QString("2"));
    QCOMPARE(pairs[2].first, QRectF(2, 3, 1, 1));
    QCOMPARE(pairs[2].second, QString("3"));
    QCOMPARE(pairs[3].first, QRectF(2, 4, 2, 1));
    QCOMPARE(pairs[3].second, QString("4"));
    QCOMPARE(pairs[4].first, QRectF(3, 6, 1, 1));
    QCOMPARE(pairs[4].second, QString("6"));
    QCOMPARE(pairs[5].first, QRectF(3, 8, 1, 1));
    QCOMPARE(pairs[5].second, QString("8"));
    QCOMPARE(pairs[6].first, QRectF(3, 9, 3, 1));
    QCOMPARE(pairs[6].second, QString("9"));
    QCOMPARE(undo.count(), 2);
    QCOMPARE(undo[0].first.toRect(), QRect(3, 5, 3, 1));
    QCOMPARE(undo[0].second, QString("5"));
    QCOMPARE(undo[1].first.toRect(), QRect(4, 7, 2, 1));
    QCOMPARE(undo[1].second, QString("7"));
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
    QList< QPair<QRectF, QString> > undo = tree.removeRows(3, 3);
    QList< QPair<QRectF, QString> > pairs = tree.intersectingPairs(QRect(1, 1, 10, 10)).values();
    QCOMPARE(pairs.count(), 7);
    QCOMPARE(pairs[0].first, QRectF(1, 1, 1, 2));
    QCOMPARE(pairs[0].second, QString("1"));
    QCOMPARE(pairs[1].first, QRectF(2, 1, 1, 2));
    QCOMPARE(pairs[1].second, QString("2"));
    QCOMPARE(pairs[2].first, QRectF(3, 2, 1, 1));
    QCOMPARE(pairs[2].second, QString("3"));
    QCOMPARE(pairs[3].first, QRectF(4, 2, 1, 2));
    QCOMPARE(pairs[3].second, QString("4"));
    QCOMPARE(pairs[4].first, QRectF(6, 3, 1, 1));
    QCOMPARE(pairs[4].second, QString("6"));
    QCOMPARE(pairs[5].first, QRectF(8, 3, 1, 1));
    QCOMPARE(pairs[5].second, QString("8"));
    QCOMPARE(pairs[6].first, QRectF(9, 3, 1, 3));
    QCOMPARE(pairs[6].second, QString("9"));
    QCOMPARE(undo.count(), 2);
    QCOMPARE(undo[0].first.toRect(), QRect(5, 3, 1, 3));
    QCOMPARE(undo[0].second, QString("5"));
    QCOMPARE(undo[1].first.toRect(), QRect(7, 4, 1, 2));
    QCOMPARE(undo[1].second, QString("7"));
}

void TestRTree::testPrimitive()
{
    RTree<bool> tree;
    tree.insert(QRect(2, 5, 1, 2), true);
    QCOMPARE(tree.contains(QPoint(2, 2)).isEmpty(), true);
    QCOMPARE(tree.contains(QPoint(2, 5)).first(), true);
    QCOMPARE(tree.contains(QPoint(3, 5)).isEmpty(), true);
    QCOMPARE(tree.contains(QPoint(2, 6)).first(), true);
    const QList< QPair<QRectF, bool> > pairs = tree.intersectingPairs(QRect(2, 5, 1, 2)).values();
    QCOMPARE(pairs.count(), 1);
    QCOMPARE(pairs.first().first.toRect(), QRect(2, 5, 1, 2));
    QCOMPARE(pairs.first().second, true);
}
QTEST_MAIN(TestRTree)
#include "TestRTree.moc"
