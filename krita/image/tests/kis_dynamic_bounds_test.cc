/*
 *  Copyright (c) 2012 Shrikrishna Holla shrikrishna.holla@gmail.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

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
    delete painter1;
}

QTEST_KDEMAIN(KisDynamicBoundsTest, GUI)
