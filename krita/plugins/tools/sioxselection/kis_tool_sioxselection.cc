#include <kis_tool_sioxselection.h>

#include <qpainter.h>

#include <kis_debug.h>
#include <klocale.h>

#include <KoCanvasController.h>
#include <KoPointerEvent.h>

#include <kis_canvas2.h>
#include <kis_cursor.h>
#include <kis_view2.h>

// TODO - Tests inlcudes. Remove unnecessary.
#include <KoColor.h>
#include <KoFilterManager.h>
#include <kis_doc2.h>
#include <kis_paint_device.h>
#include <kis_iterator_ng.h>
#include <kis_types.h>
#include "kisSioxSegmentator.h"
#include <kdebug.h>


KisToolSioxSelection::KisToolSioxSelection(KoCanvasBase * canvas)
    : KisTool(canvas, KisCursor::arrowCursor()), m_canvas( dynamic_cast<KisCanvas2*>(canvas) )
{
    Q_ASSERT(m_canvas);
    setObjectName("tool_sioxselection");
}

KisToolSioxSelection::~KisToolSioxSelection()
{
}

void KisToolSioxSelection::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    // Add code here to initialize your tool when it got activated
    KisTool::activate(toolActivation, shapes);

    kWarning() << "SIOX tool activated.";

    // Load test image.
    KisDoc2 document1;
    KoFilterManager manager1(&document1);
    KoFilter::ConversionStatus status1;

    //manager1.importDocument("/home/bruno/test/img_llama.png", "", status1);
    manager1.importDocument("/home/bruno/test/horse.jpg", "", status1);
    KisPaintDeviceSP imageDevice = document1.image()->projection();
    //imageDevice->convertToQImage(0).save("/home/bruno/test/test.png");

    // Load test trimap.
    KisDoc2 document2;
    KoFilterManager manager2(&document2);
    KoFilter::ConversionStatus status2;

    //manager2.importDocument("/home/bruno/test/tri_llama.png", "", status2);
    manager2.importDocument("/home/bruno/test/horse_tri.png", "", status2);
    KisPaintDeviceSP trimapDevice = document2.image()->projection();

    // Call siox.
    KisSioxSegmentator segmentator(imageDevice, trimapDevice, 0, 4);
    kWarning() << "segmentaion returned: " << segmentator.segmentate();

    KisPaintDeviceSP alphaSegmentation = segmentator.getSegmentedConfidenceMatrix();

    {
        const qint32 width = alphaSegmentation->exactBounds().width();
        const qint32 height = alphaSegmentation->exactBounds().height();

        KisHLineIteratorSP alphaSegmentationIter = alphaSegmentation->createHLineIteratorNG(0, 0, width);
        KisHLineIteratorSP imageIter = imageDevice->createHLineIteratorNG(0, 0, width);

        for (qint32 y = 0; y < height; y++) {
            do {
                quint8* alphaSegmentationData = reinterpret_cast<quint8*>(
                    alphaSegmentationIter->rawData());

                quint8* imageData = reinterpret_cast<quint8*>(imageIter->rawData());
                imageData[3] = alphaSegmentationData[0];

                //kWarning() << "seg data: " << alphaSegmentationData[0];

            } while (alphaSegmentationIter->nextPixel() & imageIter->nextPixel());

            alphaSegmentationIter->nextRow();
            imageIter->nextRow();
        }
    }

    imageDevice->convertToQImage(0).save("/home/bruno/test/test_seg.png");


//    KisPaintDeviceSP result;// = ...; // get alpha channel, apply etc...
//    QImage qimage = result->convertToQImage(0);
//    qimage.save("test.png");

//    {
//        KisRectIteratorSP confidenceMatrixIter = confidenceMatrix->createRectIteratorNG(
//            confidenceMatrix->exactBounds());

//        KisRectIteratorSP trimapIter = trimapDevice->createRectIteratorNG(
//            trimapDevice->exactBounds());

//        KisRectIteratorSP imageIter = imageDevice->createRectIteratorNG(
//            imageDevice->exactBounds());
//        do {
//            //quint8* confidenceMatrixData = reinterpret_cast<quint8*>(
//            //    confidenceMatrixIter->rawData());

//            //quint8* trimapData = reinterpret_cast<quint8*>(trimapIter->rawData());

//            quint8* imageData = reinterpret_cast<quint8*>(imageIter->rawData());
//            imageData[3] = 100; // test alpha channel

//        } while (confidenceMatrixIter->nextPixel() & trimapIter->nextPixel() &
//                 imageIter->nextPixel());
//    }

//    imageDevice->convertToQImage(0).save("/home/bruno/test/test_alpha.png");


}

void KisToolSioxSelection::deactivate()
{
    // Add code here to initialize your tool when it got deactivated
    KisTool::deactivate();
}

void KisToolSioxSelection::mousePressEvent(KoPointerEvent *event)
{
    event->ignore();
}


void KisToolSioxSelection::mouseMoveEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KisToolSioxSelection::mouseReleaseEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KisToolSioxSelection::paint(QPainter& gc, const KoViewConverter &converter)
{
    Q_UNUSED(gc);
    Q_UNUSED(converter);
}

// Uncomment if you have a configuration widget
// QWidget* KisToolSioxSelection::createOptionWidget()
// {
//     return 0;
// }
//
// QWidget* KisToolSioxSelection::optionWidget()
// {
//         return 0;
// }

#include "kis_tool_sioxselection.moc"
