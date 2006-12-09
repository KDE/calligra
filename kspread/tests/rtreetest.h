#ifndef KSPREAD_RTREE_TEST
#define KSPREAD_RTREE_TEST

#include <QtTest/QtTest>

namespace KSpread
{

class RTreeTest: public QObject
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

#endif // KSPREAD_RTREE_TEST
