#ifndef KSPREAD_RTREE_TEST
#define KSPREAD_RTREE_TEST

#include <QtTest/QtTest>
#include <QSharedData>

#include "RTree.h"

namespace KSpread
{

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

class RTreeTest: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testIntersectingPairs();
private:
    RTree<SharedTestClass> m_tree;
};

} // namespace KSpread

#endif // KSPREAD_RTREE_TEST
