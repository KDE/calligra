#include "rtreetest.h"

using namespace KSpread;

class DerivedClass : public TestClass
{
public:
    DerivedClass() : TestClass() {}
    DerivedClass( const QString& s ) : TestClass(s) {}
    virtual int type() const { return 1; }
};

void RTreeTest::testIntersectingPairs()
{
//     m_tree.clear();
    m_tree.insert( QRect(1,1,1,1), new DerivedClass(QString("foo") ) );
    QList< QPair<QRectF,SharedTestClass> > pairs = m_tree.intersectingPairs( QRect(1,1,10,10) );
    QVERIFY(pairs.count() == 1);
    QCOMPARE(pairs[0].first, QRectF(1,1,1,1));
    QCOMPARE(pairs[0].second->member, QString("foo"));
    QCOMPARE(pairs[0].second->type(), 1);
}

QTEST_MAIN(RTreeTest)
#include "rtreetest.moc"
