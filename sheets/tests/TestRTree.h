#ifndef CALLIGRA_SHEETS_TEST_RTREE
#define CALLIGRA_SHEETS_TEST_RTREE

#include <QObject>

namespace Calligra
{
namespace Sheets
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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_RTREE
