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
#include <KoColor.h>
#include "kis_image.h"
#include "kis_dynamic_bounds_test.h"
#include <qtest_kde.h>
#include "kis_painter.h"



class BaseDynamicBoundsTest : public TestUtil::QImageBasedTest
{
public:
    BaseDynamicBoundsTest()
        : QImageBasedTest("dynamic_bounds")
    {
    }


    void testPaintRect() {
        beginTest();

        KisNodeSP paint1 = findNode(m_image->root(), "paint1");
        KisPaintDeviceSP device1 = paint1->paintDevice();

        removeMaskEffects();
        m_image->refreshGraph(m_image->root(),m_image->bounds(),QRect(0,0,0,0));

        qDebug() << "Image bounds before painting:" << ppVar(m_image->bounds());

        KisPainter* painter1 = new KisPainter();
        painter1->begin(device1);
        painter1->setFillStyle(KisPainter::FillStyleForegroundColor);
        painter1->setPaintColor(KoColor(Qt::red, paint1->colorSpace()));

        painter1->paintRect(QRect(900,900,120,120));

        paint1->setDirty(painter1->takeDirtyRegion());
        m_image->waitForDone();

        delete painter1;

        qDebug() << "Image bounds after painting:" << ppVar(m_image->bounds());
        QVERIFY(checkLayers(m_image, "extended"));

        endTest();
    }

private:
    void beginTest() {
        KisSurrogateUndoStore *undoStore = new KisSurrogateUndoStore();
        m_image = createImage(undoStore);
        m_image->setCanvasInfinite();
        removeMaskEffects();
        m_image->initialRefreshGraph();

        QVERIFY(checkLayers(m_image, "initial"));
    }

    void endTest() {
        //undoStore->undo();
        //m_image->waitForDone();
        //QVERIFY(checkLayers(m_image, "initial"));
    }

    inline void removeMaskEffects()
    {

        KisTransparencyMaskSP tmask= dynamic_cast<KisTransparencyMask*>(findNode(m_image->root(),"tmask1").data());
        tmask->selection()->clear();
        tmask->selection()->getOrCreatePixelSelection()->clear();
        tmask->selection()->getOrCreatePixelSelection()->invert();
        KisCloneLayerSP clone=dynamic_cast<KisCloneLayer*>(findNode(m_image->root(),"clone1").data());
        clone=dynamic_cast<KisCloneLayer*>((findNode(m_image->root(),"paint1")->clone()).data());
    }

private:
    KisImageSP m_image;
};

void KisDynamicBoundsTest::drawRectTest()
{
    BaseDynamicBoundsTest tester;
    tester.testPaintRect();
}

QTEST_KDEMAIN(KisDynamicBoundsTest, GUI)
