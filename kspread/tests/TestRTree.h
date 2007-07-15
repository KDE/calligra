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
    void testInsertShiftRight();
    void testInsertShiftDown();
    void testRemoveShiftLeft();
    void testRemoveShiftUp();
    void testInsertColumns();
    void testInsertRows();
    void testRemoveColumns();
    void testRemoveRows();
    void testPrimitive();
};

} // namespace KSpread

#endif // KSPREAD_TEST_RTREE
