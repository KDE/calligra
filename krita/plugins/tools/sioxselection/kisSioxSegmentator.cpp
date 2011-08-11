#include <kis_iterator_ng.h>
#include <kis_paint_device.h>
#include <kis_types.h>
#include "kisSioxSegmentator.h"

namespace {

/**
  Returns the squared euclidian distance between two Lab color pixels.
 */
inline float getLabColorDiffSquared(/*const quint16* c0, const quint16* c1*/);

}

KisSioxSegmentator::KisSioxSegmentator(const KisPaintDeviceSP& pimage, float plimitL,
    float plimitA, float plimitB)
    : image(pimage),
    labImage(new KisPaintDevice(*pimage)),
    labelField(pimage->exactBounds().width() * pimage->exactBounds().height(), -1),
    knownBg(pimage->exactBounds().width() * pimage->exactBounds().height()),
    knownFg(pimage->exactBounds().width() * pimage->exactBounds().height()),
    limitL(plimitL),
    limitA(plimitA),
    limitB(plimitB),
    segmentated(false)
{
    const KoColorSpace* labCS = KoColorSpaceRegistry::instance()->lab16();
    KUndo2Command* cmd = labImage->convertTo(labCS);
    delete cmd;
}

void KisSioxSegmentator::fillColorRegions(float confidenceMatrix[])
{
    QVector< quint64 > pixelsToVisit;
    QRect labImageBounds = labImage->exactBounds();
    int imgWidth = labImageBounds.width();
    int imgHeight = labImageBounds.height();

    // Iteration over confidenceMatrix
    KisRectConstIteratorSP labImageIterator = labImage->createRectConstIteratorNG(
        labImageBounds);
    quint64 i = 0;
    do {
        // Already visited or background.
        if (labelField[i] != -1 ||
            confidenceMatrix[i] < UNKNOWN_REGION_CONFIDENCE) {
            continue;
        }

        //Q_ASSERT(i < imageSize);

        //const quint16* origColor = reinterpret_cast<const quint16*>(
        //    labImageIterator->oldRawData());
        const quint64 curLabel = i + 1;

        labelField[i] = curLabel;
        confidenceMatrix[i] = CERTAIN_FOREGROUND_CONFIDENCE;
        pixelsToVisit.push_back(i);

        // Depth first search to fill region.
        while (!pixelsToVisit.isEmpty()) {
            const int pos(pixelsToVisit.last());
            pixelsToVisit.pop_back();
            const int x = pos % imgWidth;
            const int y = pos / imgWidth;

            // Check all four neighbours.
            const int left = pos - 1;

            // The 'false' is for not fill up util implement getLabColorDiffSquared.
            if (x - 1 >= 0 && labelField[left] == -1 && false
                /*getLabColorDiffSquared(image[left], origColor) < 1.0*/) {

                labelField[left] = curLabel;
                confidenceMatrix[left] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(left);
            }

            const int right = pos + 1;

            if (x + 1 < imgWidth && labelField[right] == -1 && false
                /*getLabColorDiffSquared(image[right], origColor) < 1.0*/) {

                labelField[right] = curLabel;
                confidenceMatrix[right] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(right);
            }

            const int top = pos - imgWidth;

            if (y - 1 >= 0 && labelField[top] == -1 && false
                /*getLabColorDiffSquared(image[top], origColor) < 1.0*/) {

                labelField[top] = curLabel;
                confidenceMatrix[top] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(top);
            }

            const int bottom = pos + imgWidth;

            if (y + 1 < imgHeight && labelField[bottom] == -1 && false
                /*getLabColorDiffSquared(image[bottom], origColor) < 1.0*/) {

                labelField[bottom] = curLabel;
                confidenceMatrix[bottom] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(bottom);
            }
        }

        i++;
    } while (labImageIterator->nextPixel());
}

namespace {

inline float getLabColorDiffSquared(/*const quint16* c0, const quint16* c1*/) {
    float euclid = 0;

    // TODO Convert c0 and c1 to float
//    for (int k = 0; k < 3; k++) {
//        euclid += (c0[k] - c1[k]) * (c0[k] - c1[k]);
//    }

    return euclid;
}

}
