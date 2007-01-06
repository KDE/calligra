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
    virtual int type() const { return 0; }
    bool operator<( const TestClass& other ) const { return member < other.member; }
    bool operator==( const TestClass& other ) const { return member == other.member; }
    QString member;
};

class SharedTestClass
{
public:
    SharedTestClass() : d(new TestClass()) {}
    SharedTestClass(TestClass* subStyle) : d(subStyle) {}
    inline const TestClass *operator->() const { return d.data(); }
    bool operator<(const SharedTestClass& o) const { return d->operator<(*o.d.data()); }
    bool operator==(const SharedTestClass& o) const { return d->operator==(*o.d.data()); }

private:
    QSharedDataPointer<TestClass> d;
};

class DerivedClass : public TestClass
{
public:
    DerivedClass() : TestClass() {}
    DerivedClass( const QString& s ) : TestClass(s) {}
    virtual int type() const { return 1; }
};

void TestRTree::testIntersectingPairs()
{
    RTree<SharedTestClass> tree;
    tree.insert( QRect(1,1,1,1), new DerivedClass(QString("foo") ) );
    QList< QPair<QRectF,SharedTestClass> > pairs = tree.intersectingPairs( QRect(1,1,10,10) );
    QVERIFY(pairs.count() == 1);
    QCOMPARE(pairs[0].first, QRectF(1,1,1,1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[0].second->type(), 1);
}

void TestRTree::testShiftRows()
{
    RTree<SharedTestClass> tree;
    tree.insert( QRect(2,2,2,1), new DerivedClass(QString("foo") ) );
    tree.shiftRows( QRect(2,1,3,4) );
    QList< QPair<QRectF,SharedTestClass> > pairs = tree.intersectingPairs( QRect(1,1,10,10) );
    QCOMPARE(pairs.count(),3);
    QCOMPARE(pairs[0].first, QRectF(2,2,2,1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2,1,KS_colMax-1,4));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(5,2,2,1));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testShiftColumns()
{
    RTree<SharedTestClass> tree;
    tree.insert( QRect(2,2,1,2), new DerivedClass(QString("foo") ) );
    tree.shiftColumns( QRect(2,1,4,3) );
    QList< QPair<QRectF,SharedTestClass> > pairs = tree.intersectingPairs( QRect(1,1,10,10) );
    QCOMPARE(pairs.count(),3);
    QCOMPARE(pairs[0].first, QRectF(2,2,1,2));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2,1,4,KS_rowMax));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(2,5,1,2));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testUnshiftRows()
{
    RTree<SharedTestClass> tree;
    tree.insert( QRect(5,2,2,1), new DerivedClass(QString("foo") ) );
    tree.unshiftRows( QRect(2,1,3,4) );
    QList< QPair<QRectF,SharedTestClass> > pairs = tree.intersectingPairs( QRect(1,1,10,10) );
    QCOMPARE(pairs.count(),3);
    QCOMPARE(pairs[0].first, QRectF(5,2,2,1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2,1,KS_colMax-1,4));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(2,2,2,1));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

void TestRTree::testUnshiftColumns()
{
    RTree<SharedTestClass> tree;
    tree.insert( QRect(2,5,1,2), new DerivedClass(QString("foo") ) );
    tree.unshiftColumns( QRect(2,1,4,3) );
    QList< QPair<QRectF,SharedTestClass> > pairs = tree.intersectingPairs( QRect(1,1,10,10) );
    QCOMPARE(pairs.count(),3);
    QCOMPARE(pairs[0].first, QRectF(2,5,1,2));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[1].first, QRectF(2,1,4,KS_rowMax));
    QCOMPARE(pairs[1].second->member, QString(""));
    QCOMPARE(pairs[2].first, QRectF(2,2,1,2));
    QCOMPARE(pairs[2].second->member, QString("foo"));
}

QTEST_MAIN(TestRTree)
#include "TestRTree.moc"
