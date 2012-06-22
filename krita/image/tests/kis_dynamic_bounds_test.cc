#include "testutil.h"
#include "qimage_based_test.h"
#include "kis_image.h"
#include "kis_dynamic_bounds_test.h"
#include <qtest_kde.h>
#include "kis_painter.h"

class BaseDynamicBoundsTest : public TestUtil::QImageBasedTest
{
public:
    BaseDynamicBoundsTest()
        : QImageBasedTest("bounds")
    {
    }

    void test(const QString &testname, KisPaintDeviceSP dev) {

        KisSurrogateUndoStore *undoStore = new KisSurrogateUndoStore();
        KisImageSP image = createImage(undoStore);
        image->setCanvasInfinite();

        image->initialRefreshGraph();

        QVERIFY(checkLayers(image, "initial"));

        KisPaintLayerSP layer = new KisPaintLayer(image, "test", OPACITY_OPAQUE_U8);
        layer->paintDevice() = dev;
        image->addNode(layer.data());

        image->waitForDone();
        image->resizeImage(image->bounds());

        QVERIFY(checkLayers(image, testname));

        undoStore->undo();
        image->waitForDone();

        QVERIFY(checkLayers(image, "initial"));
    }
};

void KisDynamicBoundsTest::drawRectTest()
{
    const KoColorSpace *cs8 = KoColorSpaceRegistry::instance()->rgb8();
    KisPaintDeviceSP device1 = new KisPaintDevice(cs8);
    KisPainter* painter1 = new KisPainter();
    painter1->begin(device1);
    painter1->paintRect(QRect(1,1,120,120));

    BaseDynamicBoundsTest tester;
    tester.test("drawRect", device1);
}

QTEST_KDEMAIN(KisDynamicBoundsTest, GUI)
