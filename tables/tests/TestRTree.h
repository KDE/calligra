#ifndef CALLIGRA_TABLES_TEST_RTREE
#define CALLIGRA_TABLES_TEST_RTREE

#include <QtTest>

namespace Calligra
{
namespace Tables
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

} // namespace Tables
} // namespace Calligra

#endif // CALLIGRA_TABLES_TEST_RTREE
