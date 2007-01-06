#ifndef KSPREAD_TEST_RTREE
#define KSPREAD_TEST_RTREE

#include <QtTest/QtTest>

namespace KSpread
{

class TestRTree: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testIntersectingPairs();
    void testShiftColumns();
    void testShiftRows();
    void testUnshiftColumns();
    void testUnshiftRows();
};

} // namespace KSpread

#endif // KSPREAD_TEST_RTREE
